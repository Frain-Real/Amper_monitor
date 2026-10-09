
#include <usb_lib.hpp>
#include <usb_descr.hpp>
#include <usb_dev.hpp>

static ep_t endpoints[STM32ENDPOINTS];

static uint16_t USB_Addr = 0;
static uint8_t setupdatabuf[EP0DATABUF_SIZE];
static config_pack_t *setup_packet = (config_pack_t*) setupdatabuf;
volatile uint8_t usbON = 0;
static uint16_t configuration = 0;

static inline void std_d2h_req(){
    uint16_t st = 0;
    switch(setup_packet->bRequest){
        case GET_DESCRIPTOR:
            get_descriptor(setup_packet);
        break;
        case GET_STATUS:
            EP_WriteIRQ(0, (uint8_t *)&st, 2); // send status: Bus Powered
        break;
        case GET_CONFIGURATION:
            EP_WriteIRQ(0, (uint8_t*)&configuration, 1);
        break;
        default:
            EP_WriteIRQ(0, NULL, 0);
        break;
    }
}

static inline void std_h2d_req(){
    switch(setup_packet->bRequest){
        case SET_ADDRESS:
            // new address will be assigned later - after  acknowlegement or request to host
            USB_Addr = setup_packet->wValue;
        break;
        case SET_CONFIGURATION:
            configuration = setup_packet->wValue;
            set_configuration();
            usbON = 1;
        break;
        default:
        break;
    }
}

void __weak usb_standard_request(){
    uint8_t recipient = REQUEST_RECIPIENT(setup_packet->bmRequestType);
    uint8_t dev2host = (setup_packet->bmRequestType & 0x80) ? 1 : 0;
    switch(recipient){
        case REQ_RECIPIENT_DEVICE:
            if(dev2host){
                std_d2h_req();
            }else{
                std_h2d_req();
            }
        break;
        case REQ_RECIPIENT_INTERFACE:
            if(dev2host && setup_packet->bRequest == GET_DESCRIPTOR){
                get_descriptor(setup_packet);
            }
        break;
        default:
        break;
    }
    if(!dev2host) EP_WriteIRQ(0, NULL, 0);
}

void __weak usb_class_request(config_pack_t *req, uint8_t __attribute__((__unused__)) *data, uint16_t __attribute__((__unused__)) datalen){
    if(0 == (setup_packet->bmRequestType & 0x80))
        EP_WriteIRQ(0, NULL, 0);
}

void __weak usb_vendor_request(config_pack_t __attribute__((__unused__)) *packet, uint8_t __attribute__((__unused__)) *data, uint16_t __attribute__((__unused__)) datalen){
    if(0 == (setup_packet->bmRequestType & 0x80))
        EP_WriteIRQ(0, NULL, 0);
}

static void EP0_Handler(){
    uint8_t ep0dbuflen = 0;
    uint8_t ep0databuf[EP0DATABUF_SIZE];
    uint16_t epstatus = KEEP_DTOG(USB->EPnR[0]);
    int rxflag = RX_FLAG(epstatus);
    if(USB->ISTR & USB_ISTR_DIR) {
        if(epstatus & USB_EPnR_SETUP) {
            EP_Read(0, setupdatabuf);
        }else if(epstatus & USB_EPnR_CTR_RX) {
            ep0dbuflen = EP_Read(0, ep0databuf);
        }
    }
    if(rxflag){
        uint8_t reqtype = REQUEST_TYPE(setup_packet->bmRequestType);
        switch(reqtype){
            case REQ_TYPE_STANDARD:
                if(SETUP_FLAG(epstatus))
                    usb_standard_request();
                break;
            case REQ_TYPE_CLASS:
                usb_class_request(setup_packet, ep0databuf, ep0dbuflen);
                break;
            case REQ_TYPE_VENDOR:
                usb_vendor_request(setup_packet, ep0databuf, ep0dbuflen);
                break;
            default:
                EP_WriteIRQ(0, NULL, 0);
                break;
        }
    }
    if(TX_FLAG(epstatus))
        if ((USB->DADDR & USB_DADDR_ADD) != USB_Addr){
            USB->DADDR = USB_DADDR_EF | USB_Addr;
            usbON = 0;
        }

    if(rxflag) 
        epstatus ^= USB_EPnR_STAT_TX;
    else 
        epstatus &= ~USB_EPnR_STAT_TX;

    USB->EPnR[0] = (epstatus & ~(USB_EPnR_CTR_RX|USB_EPnR_CTR_TX)) ^ USB_EPnR_STAT_RX;
}

void EP_WriteIRQ(uint8_t number, const uint8_t *buf, uint16_t size){
    if(size > endpoints[number].txbufsz) size = endpoints[number].txbufsz;
    uint16_t N2 = (size + 1) >> 1;
    uint16_t *buf16 = (uint16_t *)buf;
    uint32_t *out = (uint32_t *)endpoints[number].tx_buf;
    for(int i = 0; i < N2; ++i, ++out){
        *out = buf16[i];
    }
    USB_BTABLE->EP[number].USB_COUNT_TX = size;
}

void EP_Write(uint8_t number, const uint8_t *buf, uint16_t size){
    EP_WriteIRQ(number, buf, size);
    uint16_t epstatus = KEEP_DTOG(USB->EPnR[number]);
    USB->EPnR[number] = (epstatus & ~(USB_EPnR_CTR_TX | USB_EPnR_STAT_RX)) ^ USB_EPnR_STAT_TX;
}

int EP_Read(uint8_t number, uint8_t *buf){
    int sz = endpoints[number].rx_cnt;
    if(!sz) return 0;
    endpoints[number].rx_cnt = 0;
    int n = (sz + 1) >> 1;
    uint32_t *in = (uint32_t*)endpoints[number].rx_buf;
    uint16_t *out = (uint16_t*)buf;
    for(int i = 0; i < n; ++i, ++in)
        out[i] = *(uint16_t*)in;
    return sz;
}

static uint16_t lastaddr = LASTADDR_DEFAULT;

int EP_Init(uint8_t number, uint8_t type, uint16_t txsz, uint16_t rxsz, void (*func)()){
    if(number >= STM32ENDPOINTS)
        return 4;

    if(txsz > USB_BTABLE_SIZE/ACCESSZ || rxsz > USB_BTABLE_SIZE/ACCESSZ)
        return 1;
    
    if(lastaddr + txsz + rxsz >= USB_BTABLE_SIZE/ACCESSZ)
        return 2;
    USB->EPnR[number] = (type << 9) | (number & USB_EPnR_EA);

    USB->EPnR[number] ^= USB_EPnR_STAT_RX | USB_EPnR_STAT_TX_1;

    if(rxsz & 1)
        return 3;

    uint16_t countrx = 0;
    if(rxsz < 64) 
        countrx = rxsz / 2;
    else{
        if(rxsz & 0x1f) 
            return 3;
        countrx = 31 + rxsz / 32;
    }

    USB_BTABLE->EP[number].USB_ADDR_TX = lastaddr;
    endpoints[number].tx_buf = (uint16_t *)(USB_BTABLE_BASE + lastaddr * ACCESSZ);
    endpoints[number].txbufsz = txsz;
    lastaddr += txsz;
    USB_BTABLE->EP[number].USB_COUNT_TX = 0;
    USB_BTABLE->EP[number].USB_ADDR_RX = lastaddr;
    endpoints[number].rx_buf = (uint8_t *)(USB_BTABLE_BASE + lastaddr * ACCESSZ);
    lastaddr += rxsz;
    USB_BTABLE->EP[number].USB_COUNT_RX = countrx << 10;
    endpoints[number].func = func;
    return 0;
}

void USB_IRQ(){
    uint32_t CNTR = USB->CNTR;
    USB->CNTR = 0;
    if(USB->ISTR & USB_ISTR_RESET){
        usbON = 0;
        CNTR = USB_CNTR_RESETM | USB_CNTR_CTRM | USB_CNTR_SUSPM;
        lastaddr = LASTADDR_DEFAULT;
        USB->DADDR = USB_DADDR_EF;
        USB->ISTR = ~USB_ISTR_RESET;
        if(EP_Init(0, EP_TYPE_CONTROL, USB_EP0BUFSZ, USB_EP0BUFSZ, EP0_Handler)){
            return;
        };
    }
    if(USB->ISTR & USB_ISTR_CTR){
        uint8_t n = USB->ISTR & USB_ISTR_EPID;
        endpoints[n].rx_cnt = USB_BTABLE->EP[n].USB_COUNT_RX & 0x3FF;
        if(endpoints[n].func) endpoints[n].func();
    }
    if(USB->ISTR & USB_ISTR_WKUP){ 
        CNTR &= ~(USB_CNTR_FSUSP | USB_CNTR_LP_MODE | USB_CNTR_WKUPM);
        USB->ISTR = ~USB_ISTR_WKUP;
    }
    if(USB->ISTR & USB_ISTR_SUSP){
        usbON = 0;
        CNTR |= USB_CNTR_FSUSP | USB_CNTR_LP_MODE | USB_CNTR_WKUPM;
        CNTR &= ~(USB_CNTR_SUSPM);
        USB->ISTR = ~USB_ISTR_SUSP;
    }
    USB->CNTR = CNTR;
}

void USB_setup(){
    NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
    NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);

    RCC->APB1ENR |= RCC_APB1ENR_USBEN;
    USB->CNTR   = USB_CNTR_FRES; 
    for(uint32_t ctr = 0; ctr < 7200; ++ctr) __NOP();
    USB->CNTR   = 0;
    USB->BTABLE = 0;
    USB->DADDR  = 0;
    USB->ISTR   = 0;
    USB->CNTR   = USB_CNTR_RESETM | USB_CNTR_WKUPM;

    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
    for(uint32_t ctr = 0; ctr < 7200; ++ctr) __NOP(); 
}

extern "C" void USB_LP_CAN1_RX0_IRQHandler() {
    USB_IRQ();
}

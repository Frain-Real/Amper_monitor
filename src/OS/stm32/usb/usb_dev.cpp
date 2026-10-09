#include <string.h>
#include <stdint.h>
#include <RingBuffer.hpp>
#include <usb_descr.hpp>
#include <usb_dev.hpp>

// Class-Specific Control Requests
#define SEND_ENCAPSULATED_COMMAND       0x00    // unused
#define GET_ENCAPSULATED_RESPONSE       0x01    // unused
#define SET_COMM_FEATURE                0x02    // unused
#define GET_COMM_FEATURE                0x03    // unused
#define CLEAR_COMM_FEATURE              0x04    // unused
#define SET_LINE_CODING                 0x20
#define GET_LINE_CODING                 0x21
#define SET_CONTROL_LINE_STATE          0x22
#define SEND_BREAK                      0x23

// control line states
#define CONTROL_DTR                     0x01
#define CONTROL_RTS                     0x02

// inbuf overflow when receiving
static uint8_t bufovrfl = 0;

// receive buffer: hold data until chkin() call
static uint8_t rcvbuf[USB_RXBUFSZ];
static uint8_t rcvbuflen = 0;
// line coding
usb_LineCoding lineCoding = {115200, USB_CDC_1_STOP_BITS, USB_CDC_NO_PARITY, 8};
// CDC configured and ready to use
volatile uint8_t CDCready = 0;

// ring buffers for incoming and outgoing data
static RingBuffer <uint8_t, RBOUTSZ> rbout;
static RingBuffer <uint8_t, RBINSZ> rbin;
// last send data size
static int lastdsz = -1;


static void chkin(){
    if(bufovrfl) return; // allow user to know that previous buffer was overflowed and cleared
    if(!rcvbuflen) return;

    int w = rbin.Write(rcvbuf, rcvbuflen);

    if(w < 0)
        return;
    if(w != rcvbuflen) 
        bufovrfl = 1;

    rcvbuflen = 0;
    uint16_t status = KEEP_DTOG(USB->EPnR[1]); // don't change DTOG
    USB->EPnR[1] = status ^ USB_EPnR_STAT_RX; // prepare to get next data portion
}

// called from transmit EP to send next data portion or by user - when new transmission starts
static void send_next(){
    uint8_t usbbuff[USB_TXBUFSZ];

    int buflen = rbout.Read(usbbuff, USB_TXBUFSZ);
    if(!CDCready){
        lastdsz = -1;
        return;
    }
    if(buflen == 0){
        if(lastdsz == USB_TXBUFSZ){
            EP_Write(1, NULL, 0); // send ZLP after 64 bits packet when nothing more to send
            lastdsz = 0;
        }else lastdsz = -1;
        return;
    }else if(buflen < 0){
        lastdsz = -1;
        return;
    }
    EP_Write(1, (uint8_t*)usbbuff, buflen);
    lastdsz = buflen;
}

// data IN/OUT handlers
static void rxtx_Handler(){ // EP3IN
    uint16_t epstatus = KEEP_DTOG(USB->EPnR[1]);
    if(RX_FLAG(epstatus)){ // receive data
        if(rcvbuflen){
            bufovrfl = 1; // lost last data
            rcvbuflen = 0;
        }
        rcvbuflen = EP_Read(1, (uint8_t*)rcvbuf);
        USB->EPnR[1] = epstatus & ~(USB_EPnR_CTR_RX | USB_EPnR_STAT_RX | USB_EPnR_STAT_TX); // keep RX in STALL state until read data
        chkin(); // try to write current data into RXbuf if it's not busy
    }else{ // tx successfull
        USB->EPnR[1] = (epstatus & ~(USB_EPnR_CTR_TX | USB_EPnR_STAT_TX)) ^ USB_EPnR_STAT_RX;
        send_next();
    }
}

void __weak linecoding_handler(usb_LineCoding *lc){
    lineCoding = *lc;
}

// SET_CONTROL_LINE_STATE
void __weak clstate_handler(uint16_t val){
    CDCready = val;
}

// SEND_BREAK
void __weak break_handler(){
    CDCready = 0;
}


// USB is configured: setup endpoints
void set_configuration(){
    EP_Init(1, EP_TYPE_BULK, USB_TXBUFSZ, USB_RXBUFSZ, rxtx_Handler); // OUT1 - receive data
}

// CLASS request
void usb_class_request(config_pack_t *req, uint8_t *data, uint16_t datalen){
    uint8_t recipient = REQUEST_RECIPIENT(req->bmRequestType);
    uint8_t dev2host = (req->bmRequestType & 0x80) ? 1 : 0;
    switch(recipient){
        case REQ_RECIPIENT_INTERFACE:
            switch(req->bRequest) {
                case 0x32:
                case SET_LINE_CODING:
                    if(!data || !datalen)
                        break;
                    if(datalen == sizeof(usb_LineCoding))
                        linecoding_handler((usb_LineCoding*)data);
                    break;
                case 0x33:
                case GET_LINE_CODING:
                    EP_WriteIRQ(0, (uint8_t*)&lineCoding, sizeof(lineCoding));
                    break;
                case SET_CONTROL_LINE_STATE:
                    clstate_handler(req->wValue);
                    break;
                case SEND_BREAK:
                    break_handler();
                    break;
                default:
                    break;
            }
            break;
        default:
            if(dev2host)
                EP_WriteIRQ(0, NULL, 0);
            break;
    }
    if(!dev2host)
        EP_WriteIRQ(0, NULL, 0);
}


// blocking send full content of ring buffer
int USB_sendall(){
    while(lastdsz > 0){
        if(!CDCready) 
            return 0;
    }
    return 1;
}

// put `buf` into queue to send
int USB_send(const uint8_t *buf, int len){
    if(!buf || !CDCready || !len) return 0;
    while(len){
        if(!CDCready) break;
        int a = rbout.Write(buf, len);
        if(a > 0){
            len -= a;
            buf += a;
        }else if(a == 0){
            if(lastdsz < 0) 
                send_next();
        }
    }
    if(buf[len-1] == '\n' && lastdsz < 0) 
        send_next();
    return 1;
}

int USB_putbyte(uint8_t byte){
    if(!CDCready) return 0;
    int l = 0;
    while((l = rbout.Write(&byte, 1)) != 1){
        if(!CDCready) return 0;
        IWDG->KR = IWDG_KEY_RELOAD;
        if(l == 0){ // overfull
            if(lastdsz < 0){
                send_next();
            }
            continue;
        }
    }
    if(byte == '\n' && lastdsz < 0)
        send_next();
    return 1;

}

int USB_sendstr(const char *string){
    if(!string || !CDCready) return 0;
    int len = 0;
    const char *b = string;
    while(*b++) ++len;
    if(!len) return 0;
    return USB_send((const uint8_t*)string, len);
}

/**
 * @brief USB_receive - get binary data from receiving ring-buffer
 * @param buf (i) - buffer for received data
 * @param len - length of `buf`
 * @return amount of received bytes (negative, if overfull happened)
 */
int USB_receive(uint8_t *buf, int len){
    chkin();
    if(bufovrfl){
        while(1 != rbin.Clear());
        bufovrfl = 0;
        return -1;
    }
    int sz = rbin.Read(buf,len);
    if(sz < 0) 
        return 0; // buffer in writting state
    return sz;
}

/**
 * @brief USB_receivestr - get string up to '\n' and replace '\n' with 0
 * @param buf - receiving buffer
 * @param len - its length
 * @return strlen or negative value indicating overflow (if so, string won't be ends with 0 and buffer should be cleared)
 */
int USB_receivestr(char *buf, int len){
    chkin();
    if(bufovrfl){
        while(1 != rbin.Clear());
        bufovrfl = 0;
        return -1;
    }
    int l = rbin.Readto('\n', (uint8_t*)(buf), len);
    if(l < 1){
        if(rbin.Datalen() == RBINSZ){
            while(1 != rbin.Clear());
            return -1;
        }
        return 0;
    }
    if(l == 0) return 0;
    buf[l-1] = 0;
    return l;
}


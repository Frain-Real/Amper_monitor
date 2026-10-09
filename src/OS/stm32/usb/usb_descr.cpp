#include "usb_lib.hpp"
#include <usb_descr.hpp>
// low/high for uint16_t
#define L16(x)              (x & 0xff)
#define H16(x)              (x >> 8)

static const uint8_t USB_DeviceDescriptor[] = {
    USB_DT_DEVICE_SIZE, // bLength
    USB_DT_DEVICE, // bDescriptorType
    L16(bcdUSB), // bcdUSB_L
    H16(bcdUSB), // bcdUSB_H
    USB_CLASS_COMM, // bDeviceClass
    bDeviceSubClass, // bDeviceSubClass
    bDeviceProtocol, // bDeviceProtocol
    USB_EP0BUFSZ, // bMaxPacketSize
    L16(idVendor), // idVendor_L
    H16(idVendor), // idVendor_H
    L16(idProduct), // idProduct_L
    H16(idProduct), // idProduct_H
    L16(bcdDevice_Ver), // bcdDevice_Ver_L
    H16(bcdDevice_Ver), // bcdDevice_Ver_H
    iMANUFACTURER_DESCR, // iManufacturer - indexes of string descriptors in array
    iPRODUCT_DESCR, // iProduct
    iSERIAL_DESCR, // iSerial
    bNumConfigurations // bNumConfigurations
};

static const uint8_t USB_DeviceQualifierDescriptor[] = {
    USB_DT_QUALIFIER_SIZE, //bLength
    USB_DT_QUALIFIER, // bDescriptorType
    L16(bcdUSB), // bcdUSB_L
    H16(bcdUSB), // bcdUSB_H
    USB_CLASS_PER_INTERFACE, // bDeviceClass
    bDeviceSubClass, // bDeviceSubClass
    bDeviceProtocol, // bDeviceProtocol
    USB_EP0BUFSZ, // bMaxPacketSize0
    bNumConfigurations, // bNumConfigurations
    0 // Reserved
};

#define wTotalLength  (USB_DT_CONFIG_SIZE + (bNumInterfaces * USB_DT_INTERFACE_SIZE) + (bTotNumEndpoints * USB_DT_ENDPOINT_SIZE) + (bNumCsInterfaces * USB_DT_CS_INTERFACE_SIZE) - 1)
static const uint8_t USB_ConfigDescriptor[] = {
    // Configuration Descriptor
    USB_DT_CONFIG_SIZE, // bLength: Configuration Descriptor size
    USB_DT_CONFIG, // bDescriptorType: Configuration
    L16(wTotalLength),   // wTotalLength.L :no of returned bytes
    H16(wTotalLength), // wTotalLength.H
    bNumInterfaces, // bNumInterfaces
    1, // bConfigurationValue: Current configuration value
    0, // iConfiguration: Index of string descriptor describing the configuration or 0
    BusPowered, // bmAttributes - Bus powered
    0x10, // MaxPower in 100mA units

    //---------------------------------------------------------------------------
    // Interface Descriptor
    USB_DT_INTERFACE_SIZE, // bLength: Interface Descriptor size
    USB_DT_INTERFACE, // bDescriptorType: Interface
    0, // bInterfaceNumber: Number of Interface
    0, // bAlternateSetting: Alternate setting
    1, // bNumEndpoints
    USB_CLASS_COMM, // bInterfaceClass
    2, // bInterfaceSubClass
    1, // bInterfaceProtocol
    iINTERFACE_DESCR1, // iInterface:

    //---------------------------------------------------------------------------
        // ---- CS Interfaces
    USB_DT_CS_INTERFACE_SIZE, // bLength
    USB_DT_CS_INTERFACE, // bDescriptorType: CS_INTERFACE
    0, // bDescriptorSubtype: Header Func Desc
    0x10, // bcdCDC: spec release number
    1, //bDataInterface
    USB_DT_CS_INTERFACE_SIZE, // bLength
    USB_DT_CS_INTERFACE, // bDescriptorType: CS_INTERFACE
    1, // bDescriptorSubtype: Call Management Func Desc
    0, // bmCapabilities: D0+D1
    1, //bDataInterface
    USB_DT_CS_INTERFACE_SIZE-1, // bLength
    USB_DT_CS_INTERFACE, // bDescriptorType: CS_INTERFACE
    2, // bDescriptorSubtype: Abstract Control Management desc
    2, // bmCapabilities
    USB_DT_CS_INTERFACE_SIZE, // bLength
    USB_DT_CS_INTERFACE, // bDescriptorType: CS_INTERFACE
    6, // bDescriptorSubtype: Union func desc
    0, // bMasterInterface: Communication class interface
    1, // bSlaveInterface0: Data Class Interface
    // Virtual endpoint 1 Descriptor
    USB_DT_ENDPOINT_SIZE, // bLength: Endpoint Descriptor size
    USB_DT_ENDPOINT, // bDescriptorType: Endpoint
    0x8A, // bEndpointAddress IN10
    USB_BM_ATTR_INTERRUPT, // bmAttributes: Interrupt
    L16(USB_EP1BUFSZ), // wMaxPacketSize LO
    H16(USB_EP1BUFSZ), // wMaxPacketSize HI
    0x10, // bInterval: 1ms

    //---------------------------------------------------------------------------
    // Data interface
    USB_DT_INTERFACE_SIZE, // bLength: Interface Descriptor size
    USB_DT_INTERFACE, // bDescriptorType: Interface
    1, // bInterfaceNumber: Number of Interface
    0, // bAlternateSetting: Alternate setting
    2, // bNumEndpoints: in and out
    USB_CLASS_DATA, // bInterfaceClass
    2, // bInterfaceSubClass: CDC
    0, // bInterfaceProtocol
    0, // iInterface
    //----------------------------------------------------------------------------
    // Endpoint IN2 Descriptor
    USB_DT_ENDPOINT_SIZE, // bLength: Endpoint Descriptor size
    USB_DT_ENDPOINT, // bDescriptorType: Endpoint
    0x01, // bEndpointAddress OUT1
    USB_BM_ATTR_BULK, // bmAttributes: Interrupt
    L16(USB_TXBUFSZ), // wMaxPacketSize LO
    H16(USB_TXBUFSZ), // wMaxPacketSize HI
    0, // bInterval: ignore for Bulk transfer
    // Endpoint OUT3 Descriptor
    USB_DT_ENDPOINT_SIZE, // bLength: Endpoint Descriptor size
    USB_DT_ENDPOINT, // bDescriptorType: Endpoint
    0x81, // bEndpointAddress: IN2
    USB_BM_ATTR_BULK, // bmAttributes: Bulk
    L16(USB_RXBUFSZ), // wMaxPacketSize LO
    H16(USB_RXBUFSZ), // wMaxPacketSize HI
    0 // bInterval: 1ms
};


_USB_LANG_ID_(LD, LANG_US);
_USB_STRING_(SD, u"1.0.0");
_USB_STRING_(MD, u"Rubetek");
_USB_STRING_(PD, u"Amper_Monitor");
_USB_STRING_(ID, u"amper_monitor");

static const void* const StringDescriptor[iDESCR_AMOUNT] = {
    [iLANGUAGE_DESCR] = &LD,
    [iMANUFACTURER_DESCR] = &MD,
    [iPRODUCT_DESCR] = &PD,
    [iSERIAL_DESCR] = &SD,
    [iINTERFACE_DESCR1] = &ID
};

static void wr0(const uint8_t *buf, uint16_t size, uint16_t askedsize){
    if(askedsize < size) 
        size = askedsize; // shortened request

    if(size < USB_EP0BUFSZ){
        EP_WriteIRQ(0, buf, size);
        return;
    }

    while(size){
        uint16_t l = size;
        if(l > USB_EP0BUFSZ) 
            l = USB_EP0BUFSZ;

        EP_WriteIRQ(0, buf, l);
        buf += l;
        size -= l;
        uint8_t needzlp = (l == USB_EP0BUFSZ) ? 1 : 0;

        if(size || needzlp){
            uint16_t epstatus = KEEP_DTOG(USB->EPnR[0]);
            USB->EPnR[0] = (epstatus & ~(USB_EPnR_CTR_RX|USB_EPnR_CTR_TX|USB_EPnR_STAT_RX))
                           ^ USB_EPnR_STAT_TX;
            uint32_t ctr = 1000000;
            while(--ctr && (USB->ISTR & USB_ISTR_CTR) == 0) {
                IWDG->KR = IWDG_KEY_RELOAD;
            }

            if((USB->ISTR & USB_ISTR_CTR) == 0)
                return;

            if(needzlp) 
                EP_WriteIRQ(0, NULL, 0);
        }
    }
}

void get_descriptor(config_pack_t *pack){
    uint8_t descrtype = pack->wValue >> 8,
        descridx = pack->wValue & 0xff;
    switch(descrtype){
        case DEVICE_DESCRIPTOR:
            wr0(USB_DeviceDescriptor, sizeof(USB_DeviceDescriptor), pack->wLength);
            break;
        case CONFIGURATION_DESCRIPTOR:
            wr0(USB_ConfigDescriptor, sizeof(USB_ConfigDescriptor), pack->wLength);
            break;
        case STRING_DESCRIPTOR:
            if(descridx < iDESCR_AMOUNT){
                wr0((const uint8_t *)StringDescriptor[descridx], *((uint8_t*)StringDescriptor[descridx]), pack->wLength);
            }else{
                EP_WriteIRQ(0, NULL, 0);
            }
            break;
        case DEVICE_QUALIFIER_DESCRIPTOR:
            wr0(USB_DeviceQualifierDescriptor, sizeof(USB_DeviceQualifierDescriptor), pack->wLength);
            break;
        default:
            break;
    }
}

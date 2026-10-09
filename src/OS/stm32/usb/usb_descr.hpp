#pragma once
#include <stdint.h>
#include <usb_lib.hpp>
#include <string>

// definition of parts common for USB_DeviceDescriptor & USB_DeviceQualifierDescriptor
// bcdUSB: 1.10
#define bcdUSB              0x0110
// Class - Misc (EF), subclass - common (2), protocol - interface association descr (1)
#define bDeviceSubClass     0x02
#define bDeviceProtocol     0x00
#define idVendor            0x0483
#define idProduct           0x5740
#define bcdDevice_Ver       0x0200
#define bNumConfigurations  1

// amount of interfaces and endpoints (except 0) used
#define bNumInterfaces      2
#define bTotNumEndpoints    3
#define bNumCsInterfaces    4

// powered
#define BusPowered          (1<<7)
#define SelfPowered         (1<<6)
#define RemoteWakeup        (1<<5)

// buffer sizes
// for USB FS EP0 buffers are from 8 to 64 bytes long
#define USB_EP0BUFSZ    64
#define USB_EP1BUFSZ    8
// Rx/Tx EPs
#define USB_RXBUFSZ     64
#define USB_TXBUFSZ     64

// string descriptors
enum{
    iLANGUAGE_DESCR,
    iMANUFACTURER_DESCR,
    iPRODUCT_DESCR,
    iSERIAL_DESCR,
    iINTERFACE_DESCR1,
   /* iINTERFACE_DESCR2,
    iINTERFACE_DESCR3,
    iINTERFACE_DESCR4,
    iINTERFACE_DESCR5,
    iINTERFACE_DESCR6,
    iINTERFACE_DESCR7,*/
    iDESCR_AMOUNT
};

void get_descriptor(config_pack_t *pack);


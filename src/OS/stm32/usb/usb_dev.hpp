#pragma once
#include <usb_lib.hpp>

typedef struct {
    uint32_t dwDTERate;
    uint8_t bCharFormat;
#define USB_CDC_1_STOP_BITS   0
#define USB_CDC_1_5_STOP_BITS 1
#define USB_CDC_2_STOP_BITS   2
    uint8_t bParityType;
#define USB_CDC_NO_PARITY     0
#define USB_CDC_ODD_PARITY    1
#define USB_CDC_EVEN_PARITY   2
#define USB_CDC_MARK_PARITY   3
#define USB_CDC_SPACE_PARITY  4
    uint8_t bDataBits;
} __attribute__ ((packed)) usb_LineCoding;

extern usb_LineCoding lineCoding;
extern volatile uint8_t CDCready;

void break_handler();
void clstate_handler(uint16_t val);
void linecoding_handler(usb_LineCoding *lc);

// sizes of ringbuffers for outgoing and incoming data
#define RBOUTSZ     1024
#define RBINSZ      1024

#define newline()   USB_putbyte('\n')
#define USND(s)     do{USB_sendstr(s); USB_putbyte('\n');}while(0)

int USB_sendall();
int USB_send(const uint8_t *buf, int len);
int USB_putbyte(uint8_t byte);
int USB_sendstr(const char *string);
int USB_receive(uint8_t *buf, int len);
int USB_receivestr(char *buf, int len);

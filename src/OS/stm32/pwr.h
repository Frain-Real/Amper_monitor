#ifndef OS_PWR_H
#define OS_PWR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
    OFF = 0,
    ON,
} pwr__state_e;

void pwr_init(void);

void pwr_change(int channel, pwr__state_e state);

#ifdef __cplusplus
}
#endif


#endif

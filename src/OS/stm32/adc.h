#ifndef OS_ADC_H
#define OS_ADC_H


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define ADC_CHANNEL_COUNT 10
void adc_init(void);
uint8_t adc_get(uint8_t channel_idx, float *res);


#ifdef __cplusplus
}
#endif


#endif

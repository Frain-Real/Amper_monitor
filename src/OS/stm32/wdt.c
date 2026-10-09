#include "wdt.h"
#include "stmlib/stm32f1xx.h"
#include <stmlib/stm32f1xx_hal_iwdg.h>


IWDG_HandleTypeDef hiwdg;
void wdt_init(uint32_t ms)
{
    uint32_t v=ms * 128 / 1000;
	if (v > 0xfff) v=0xfff;

    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
    hiwdg.Init.Reload = v;
    HAL_IWDG_Init(&hiwdg);
}


void wdt_reset(void)
{
	HAL_IWDG_Refresh(&hiwdg);
}

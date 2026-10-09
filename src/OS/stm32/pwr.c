#include "pwr.h"
#include <stmlib/stm32f1xx_hal_gpio.h>

static       GPIO_TypeDef* Power_GPIO[] = { GPIOB,          GPIOB,          GPIOB,          GPIOB,          GPIOB,          GPIOB,          GPIOB,          GPIOA,          GPIOB,          GPIOB    };
static const uint16_t      Power_PIN[]  = { GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_11};

void pwr_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	
	for (uint8_t i=0; i<3; i++) {
		GPIO_InitStructure.Pin = Power_PIN[i];
		HAL_GPIO_Init(Power_GPIO[i], &GPIO_InitStructure);
        HAL_GPIO_WritePin(Power_GPIO[i], Power_PIN[i], GPIO_PIN_RESET);
	}
}

void pwr_change(int res, pwr__state_e state) {
    if(res > (sizeof(Power_PIN) / sizeof(Power_PIN[0])))
        return;
    
    HAL_GPIO_WritePin(Power_GPIO[res], Power_PIN[res],(GPIO_PinState)state);
}

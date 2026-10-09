#include "adc.h"
#include "stmlib/stm32f1xx.h"

#include <stdint.h>
#include <stmlib/stm32f1xx_hal_rcc.h>
#include <stmlib/stm32f1xx_hal_gpio.h>
#include <stmlib/stm32f1xx_hal_adc.h>
#include <string.h>

#define ADC_BUFFER_SIZE	ADC_CHANNEL_COUNT
static uint16_t buf[ADC_BUFFER_SIZE];

ADC_HandleTypeDef hadc1;
static uint8_t channel_idx = 0;

static const uint32_t adc_channel_map[ADC_CHANNEL_COUNT] = {
    ADC_CHANNEL_0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_2,
    ADC_CHANNEL_3,
    ADC_CHANNEL_4,
    ADC_CHANNEL_5,
    ADC_CHANNEL_6,
    ADC_CHANNEL_7,
    ADC_CHANNEL_8,
    ADC_CHANNEL_9,
};

void adc_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	
    NVIC_DisableIRQ(TIM1_UP_IRQn);
    __HAL_RCC_TIM1_CLK_ENABLE();
    TIM1->PSC = 7;
    TIM1->ARR = 999;
    TIM1->DIER |= TIM_DIER_UIE;
    TIM1->CR1 = TIM_CR1_CEN;
    HAL_NVIC_SetPriority(TIM1_UP_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);

	// Очищаем буфер
	memset(buf, 0, sizeof(buf));
	
	// Включаем ADC
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_ADC_CONFIG(RCC_ADCPCLK2_DIV6);
	// Настраиваем порты АЦП
	/*
        | Пин | Номер     |
        | --- | --------- |
        | PA2 | ADC12_IN2 |
        | PA3 | ADC12_IN3 |
        | PA4 | ADC12_IN4 |
        | PA5 | ADC12_IN5 |
        | PA6 | ADC12_IN6 |
        | PA7 | ADC12_IN7 |
	 */
	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	// ADC1 regular channel configuration
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;       // Сканирование отключено, работаем с 1 каналом
    hadc1.Init.ContinuousConvMode = DISABLE;          // Однократное преобразование
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START; // Запуск по софту
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;                   // Количество преобразований = 1

    HAL_ADC_Init(&hadc1);

    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

}


uint8_t adc_get(uint8_t idx, float *res){

    if (res == NULL || idx >= ADC_CHANNEL_COUNT) return 0;
     __disable_irq();
    uint16_t adc_value = buf[idx];
    __enable_irq();
    *res = (float)adc_value / 4095.0f;

    return 1;
}

void TIM1_UP_IRQHandler(void) {
    if (TIM1->SR & TIM_SR_UIF) {
        TIM1->SR &= ~TIM_SR_UIF; // Сброс флага прерывания

        // Проверяем, что АЦП свободен, чтобы не сломать state machine HAL
        if (HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_READY) {
            ADC_ChannelConfTypeDef sConfig = {0};
            sConfig.Channel = adc_channel_map[channel_idx];
            sConfig.Rank = 1;
            sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;

            HAL_ADC_ConfigChannel(&hadc1, &sConfig);
            HAL_ADC_Start_IT(&hadc1); // Запускаем и разрешаем прерывание по завершению
        }
    }
}

void ADC1_2_IRQHandler(void) {
    HAL_ADC_IRQHandler(&hadc1); // Обрати внимание на &
}

// Callback, вызываемый внутри HAL_ADC_IRQHandler по завершению преобразования
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        // Сохраняем результат
        buf[channel_idx] = HAL_ADC_GetValue(&hadc1);
        
        // Циклический инкремент с защитой от переполнения
        channel_idx++;
        if (channel_idx >= ADC_CHANNEL_COUNT) {
            channel_idx = 0;
        }
    }
}
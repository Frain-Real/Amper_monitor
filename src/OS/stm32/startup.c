#include <stdint.h>
#include "stmlib/stm32f1xx.h"
#include "stmlib/system_stm32f1xx.h"
#include <stmlib/stm32f1xx_hal_rcc.h>
#include <stmlib/stm32f1xx_hal_cortex.h>
#include "wdt.h"
#include "adc.h"
#include "pwr.h"
#include "../../main.h"

static void StartHSE(){
    RCC->CR |= RCC_CR_HSEON;
    while ((RCC->CIR & RCC_CIR_HSERDYF) != 0);
    RCC->CIR |= RCC_CIR_HSERDYC; // clear rdy flag
    RCC->CR   |=  RCC_CR_CSSON;
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
    RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE_HSE_DIV2 | RCC_CFGR_PLLMULL9;
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    FLASH->ACR |= FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

int main(void) {

    StartHSE();
    SystemCoreClockUpdate();
	wdt_init(3000);
    
    // Включаем всю нужную переферию
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
	// Инитим периферию
	adc_init();
    pwr_init();
    // Запускаем основную программу
	main_program();
    return 0;
}

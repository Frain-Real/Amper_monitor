#include "OS/stm32/pwr.h"
#include "OS/stm32/stmlib/stm32f1xx_hal_gpio.h"
#include <stm32f1xx.h>
#include <stm32f1xx_hal_gpio.h>
#include <usb_dev.hpp>
#include <cstring>
#include <stdint.h>
#include <adc.h>
#include <pwr.h>
#include <main.h>
#include <wdt.h>

void main_program(){
    USB_setup();
    
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);

    static uint8_t command[16] = {0};
	while(1){
        wdt_reset();
        int l = USB_receive(command, 16);
        if(l > 0){
            bool correct  = false;
            if(!memcmp(command,"GET_ADC_1", 9)){
                uint8_t packet[4];
                float outs_data;
                adc_get(0, &outs_data);
                memcpy(packet, &outs_data, sizeof(float));
                USB_send(packet, 4);
                newline();
                correct = true;
            }
            if(!memcmp(command,"GET_ADC_2", 9)){
                uint8_t packet[4];
                float outs_data;
                adc_get(1, &outs_data);
                memcpy(packet, &outs_data, sizeof(float));
                USB_send(packet, 4);
                newline();
                correct = true;
            }
            if(!memcmp(command,"GET_ADC_3", 9)){
                uint8_t packet[4];
                float outs_data;
                adc_get(2, &outs_data);
                memcpy(packet, &outs_data, sizeof(float));
                USB_send(packet, 4);
                newline();
                correct = true;
            }
            if(!memcmp(command,"GET_ADC_4", 9)){
                uint8_t packet[4];
                float outs_data;
                adc_get(3, &outs_data);
                memcpy(packet, &outs_data, sizeof(float));
                USB_send(packet, 4);
                newline();
                correct = true;
            }
            if(!memcmp(command,"GET_ADC_5", 9)){
                uint8_t packet[4];
                float outs_data;
                adc_get(4, &outs_data);
                memcpy(packet, &outs_data, sizeof(float));
                USB_send(packet, 4);
                newline();
                correct = true;
            }
            if(!memcmp(command,"GET_ADC_6", 9)){
                uint8_t packet[4];
                float outs_data;
                adc_get(5, &outs_data);
                memcpy(packet, &outs_data, sizeof(float));
                USB_send(packet, 4);
                newline();
                correct = true;
            }
            if(!memcmp(command,"GET_ADC_7", 9)){
                uint8_t packet[4];
                float outs_data;
                adc_get(6, &outs_data);
                memcpy(packet, &outs_data, sizeof(float));
                USB_send(packet, 4);
                newline();
                correct = true;
            }
            if(!memcmp(command,"GET_ADC_8", 9)){
                uint8_t packet[4];
                float outs_data;
                adc_get(7, &outs_data);
                memcpy(packet, &outs_data, sizeof(float));
                USB_send(packet, 4);
                newline();
                correct = true;
            }
            if(!memcmp(command,"GET_ADC_9", 9)){
                uint8_t packet[4];
                float outs_data;
                adc_get(8, &outs_data);
                memcpy(packet, &outs_data, sizeof(float));
                USB_send(packet, 4);
                newline();
                correct = true;
            }
            if(!memcmp(command,"GET_ADC_10", 10)){
                uint8_t packet[4];
                float outs_data;
                adc_get(9, &outs_data);
                memcpy(packet, &outs_data, sizeof(float));
                USB_send(packet, 4);
                newline();
                correct = true;
            }
            if(!memcmp(command,"ON_CHANNEL_1", 12)){
                pwr_change(0, pwr__state_e::ON);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"OFF_CHANNEL_1", 13)){
                pwr_change(0, pwr__state_e::OFF);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"ON_CHANNEL_2", 12)){
                pwr_change(1, pwr__state_e::ON);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"OFF_CHANNEL_2", 13)){
                pwr_change(1, pwr__state_e::OFF);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"ON_CHANNEL_3", 12)){
                pwr_change(2, pwr__state_e::ON);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"OFF_CHANNEL_3", 13)){
                pwr_change(2, pwr__state_e::OFF);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"ON_CHANNEL_4", 12)){
                pwr_change(3, pwr__state_e::ON);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"OFF_CHANNEL_4", 13)){
                pwr_change(3, pwr__state_e::OFF);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"ON_CHANNEL_5", 12)){
                pwr_change(4, pwr__state_e::ON);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"OFF_CHANNEL_5", 13)){
                pwr_change(4, pwr__state_e::OFF);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"ON_CHANNEL_6", 12)){
                pwr_change(5, pwr__state_e::ON);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"OFF_CHANNEL_6", 13)){
                pwr_change(5, pwr__state_e::OFF);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"ON_CHANNEL_7", 12)){
                pwr_change(6, pwr__state_e::ON);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"OFF_CHANNEL_7", 13)){
                pwr_change(6, pwr__state_e::OFF);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"ON_CHANNEL_8", 12)){
                pwr_change(7, pwr__state_e::ON);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"OFF_CHANNEL_8", 13)){
                pwr_change(7, pwr__state_e::OFF);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"ON_CHANNEL_9", 12)){
                pwr_change(8, pwr__state_e::ON);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"OFF_CHANNEL_9", 13)){
                pwr_change(8, pwr__state_e::OFF);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"ON_CHANNEL_10", 13)){
                pwr_change(9, pwr__state_e::ON);
                USND("OK");
                correct = true;
            }
            if(!memcmp(command,"OFF_CHANNEL_10", 14)){
                pwr_change(9, pwr__state_e::OFF);
                USND("OK");
                correct = true;
            }
            if(!correct)
                USND("Error command");
            memset(command, 0, sizeof(command)); 
        }
    }
}
                

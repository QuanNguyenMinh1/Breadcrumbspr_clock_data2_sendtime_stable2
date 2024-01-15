#ifndef BUTTON_H
#define BUTTON_H

#include "main.h"
typedef struct
{
	uint8_t btn_current;
	uint8_t btn_last;
	uint8_t btn_filter;
	uint8_t is_debouncing;
	uint32_t time_debounce;
	uint32_t time_start_press;
	uint8_t is_press_timeout;
	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pin;
}Button_TypeDef;
void btn_pressing_callback(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void button_handle(Button_TypeDef *ButtonX);
void button_init(Button_TypeDef *ButtonX, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
//void btn_adjust(uint8_t);
//void btn_adjust();
#endif

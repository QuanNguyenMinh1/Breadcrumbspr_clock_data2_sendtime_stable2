#include "button.h"
//-------var button--------
__weak void btn_pressing_callback(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
__weak void btn_release_callback (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
__weak void btn_press_short_callback(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
__weak void btn_press_timeout_callback(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint8_t btn_current = 1;
uint8_t btn_last = 1;
uint8_t btn_filter = 1;
uint8_t is_debouncing;
uint32_t time_debounce;
uint32_t time_start_press;
uint8_t is_press_timeout;
void button_handle(Button_TypeDef *ButtonX)
{
	//-----------------LocNhieu---------------------
	uint8_t sta = HAL_GPIO_ReadPin(ButtonX->GPIOx, ButtonX->GPIO_Pin);
	if (sta != ButtonX->btn_filter)
	{
		ButtonX->btn_filter = sta;
		ButtonX->is_debouncing = 1;
		ButtonX->time_debounce = HAL_GetTick();
	}
	//---------tin hieu da xac nhan---------
	if ((ButtonX->is_debouncing) && (HAL_GetTick() - ButtonX->time_debounce >= 15))
	{
		ButtonX->btn_current = ButtonX->btn_filter;
		ButtonX->is_debouncing = 0;
	}
	//--------------------Xu ly-------------------------
	if (ButtonX->btn_current != ButtonX->btn_last)
	{
		if (ButtonX->btn_current == 0)//nhan xuong
		{
			ButtonX->is_press_timeout = 1;
			ButtonX->time_start_press = HAL_GetTick();
			btn_pressing_callback(ButtonX->GPIOx, ButtonX->GPIO_Pin );
			ButtonX->time_start_press = HAL_GetTick();
		}
		else //nha ra
		{
			if (HAL_GetTick() - ButtonX->time_start_press <= 1000)
			{
				btn_press_short_callback(ButtonX->GPIOx, ButtonX->GPIO_Pin );
			}
			btn_release_callback(ButtonX->GPIOx, ButtonX->GPIO_Pin );
			ButtonX->is_press_timeout = 0;
		}
		ButtonX->btn_last = ButtonX->btn_current;
	}
	//-----------Xu ly nhan giu-------------
	if ((ButtonX->is_press_timeout == 1) && (HAL_GetTick() - ButtonX->time_start_press >= 3000))
	{
		ButtonX->is_press_timeout = 0;
		btn_press_timeout_callback(ButtonX->GPIOx, ButtonX->GPIO_Pin );
	}
	}
void button_init(Button_TypeDef *ButtonX, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	ButtonX->GPIOx = GPIOx;
	ButtonX->GPIO_Pin = GPIO_Pin;
}

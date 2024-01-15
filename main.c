/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ILI9225.h"
#include "stdio.h"
#include <stdio.h>
#include "stdint.h"
#include <string.h>
#include "RTC.h"
#include "button.h"
#include "DHT.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint8_t second_present, min_present, hour_present, date_present, month_present, year_present;
uint8_t second_temp, min_temp, hour_temp, date_temp, month_temp, year_temp;
uint8_t date_max;
char strtempmin[20];
char strtemphour[20];
char strtempdate[20];
char temperature_string[5];
char humidity_string[5];
float Temperature;
float Humidity;
char str1[20];
char str2[20];
int count;
//int i = 0;
//char Rx_data[4] = { 0 };
char data_string[6];
DHT_DataTypedef DHT11_Data;
uint8_t Rx_data = 0;
DateTime_t datetime;
uint8_t Rx_Buf = 0;
uint8_t uart_buffer[7];
uint8_t uart_pointer = 0;
uint8_t uart_flag = 0;
int isOK = 0;
int live_flag;
uint16_t ring_hour=0;
uint16_t ring_min=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void btn_adjust_up(void);
void btn_adjust_down(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == huart1.Instance)
	{
		if(Rx_Buf != '\n')
		{
			uart_buffer[uart_pointer] = Rx_Buf;
	//nếu byte nhận được bên trong bộ đệm không phải dấu xuống dòng thì byte trong Rx_Buf đó được lưu vào mảng uart_buffer tại vị trí con tr ? "uart_pointer"//
			uart_pointer++;//tăng lên 1 để tr ? đến vị trí kế tiếp trong bộ đệm. HAHA ta khai báo cái mảng này lưu được tối đa tới 800 bits  nên vô tư ;))) //
		}
		else
		{
			uart_buffer[uart_pointer] = '\0'; //nếu byte nhận được trong
	//bộ đệm là dấu xuống dòng thì lưu giá trị rỗng vào mảng uart_buffer tại vị trí con tr ? uart_pointer.//
			uart_flag = 1;// biểu thị rằng 1 cái dòng data hoàn chỉnh đã được nhận từ thiết bị ngoại vi qua giao tiếp UART.
		}
		if(live_flag == 1)
		{
			uint8_t timer_out = HAL_GetTick();
			if(HAL_GetTick - timer_out > 3500)
			{
			snprintf(data_string, 7, "*%02g%02g#", Temperature, Humidity);
			HAL_UART_Transmit(&huart1, (uint8_t*)data_string, 7, 200);
			timer_out = HAL_GetTick();
			}
		}
	HAL_UART_Receive_IT(&huart1, &Rx_Buf, 1);//(trong vòng if), nhận data bằng ngắt rồi lưu vào huart1 tại bộ nhớ đệm chỉ duy nhất 1 bit=> rồi lại g ?i hàm call back nghĩa là lại nhảy vô if thêm lần nữa//
	}
}
void UART_Handle()
{
//	for(int j = 1; j <=6; j++){
//		if(uart_buffer[j] == '*')
//		{
//			for(int i = 0; i<=6; i++)
//			{
//				uart_buffer[i] = '\0';
//				uart_pointer = 0;
//			}
//		}
//	}
	if(uart_flag == 1)
	{ //nếu gặp /n thì nhảy sang cắt chuỗi
	//cut string//
		char *arg_list[10];//mảng có 10 ô, mỗi ô chứa tối đa 8 bits
		uint8_t arg_num = 0; // số thứ n trong mảng
		char *temp_token = strtok((char *)uart_buffer, " "); //Chỉ gán char trừ khoảng trắng cho con tr ? temp_token//
		while(temp_token != NULL)
		{ //vòng lặp chạy mãi cho tới khi token tạm th ?i = null
			arg_list[arg_num] = temp_token; //byte của temp_token sẽ được lưu vào mảng arg_list tại arg_num//
			arg_num++; //để bảo đảm là mỗi token đã được qua thực thi rồi
			temp_token = strtok(NULL, " ");
		}
// Handle
		// Chuỗi được truy�?n từ ESP32 có định dạng: "* xxxxx\n" //////
		if(strstr(arg_list[0], "*") != NULL)
		{
			for(int j = 1; j <=6; j++){
				if(uart_buffer[j] == '*')
				{
					for(int i = 0; i<=6; i++)
					{
						uart_buffer[i] = '\0';
						uart_pointer = 0;
					}
				}
			}
			if(strstr(arg_list[1], "live") != NULL)
			{
				live_flag = 1;
				isOK = 1;
				for(int i = 0; i<=6; i++)
				{
					uart_buffer[i] = '\0';
					uart_pointer = 0;
				}
//				uart_flag = 0;
			}
			else if(strstr(arg_list[1], "unli") != NULL)
			{
				live_flag = 0;
				isOK = 2;
				for(int i = 0; i<=6; i++)
				{
					uart_buffer[i] = '\0';
					uart_pointer = 0;
				}
				uart_flag = 0;
			}
			else if(strstr(arg_list[1], "LEDOF") != NULL)
			{
//				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);

//				for(int i = 0; i<=6; i++)
//				{
//					uart_buffer[i] = '\0';
//					uart_pointer = 0;
//				}
				uart_flag = 0;
				isOK = 3;
			}
//			else{
//			for(i = 0; i<=6; i++)
//			{
//
//				uart_buffer[i] = '\0';
//				uart_pointer = 0;
//			}
//			isOK = 99;
//			uart_flag = 0;
//			i = 0;
//			}
		}
		else if(strstr(arg_list[0], "^") != NULL)
		{
	    	for(int i = 4; i<=6; i++)
		    {
				uart_buffer[i] = '\0';
				uart_pointer = 0;
		    }
	    	for(int i = 1; i <= 6; i++)
	    	{
	    		if(uart_buffer[i] == '^')
	    		{
	    			count++;
	    		}
	    	}
	    	if(count == 0)
	    	{
	    		if(uart_buffer[3] == '\0')
	    		{
	    			uart_buffer[3] = uart_buffer[2];
	    			uart_buffer[2] = '0';
	    		}
				ring_hour = uart_buffer[2] * 10 + uart_buffer[3];
				ring_hour -= 528;
	    	}

//	    	if(uart_buffer[2] != '^' & uart_buffer[2] == 0)
//
//	    	for(int i = 1; i<= 6; i++){
//	    		if(uart_buffer[i] == '^')
//	    		{
//					ring_hour = (uart_buffer[2] * 10) + uart_buffer[3];
//	    		}
//	    	}

	    	for(int i = 0; i<=6; i++)
		    {
				uart_buffer[i] = '\0';
				uart_pointer = 0;
		    }
	    	count = 0;
	    	uart_flag = 0;
		}
		else if(strstr(arg_list[0], "#") != NULL)
		{
	    	for(int i = 4; i<=6; i++)
		    {
				uart_buffer[i] = '\0';
				uart_pointer = 0;
		    }
	    	for(int i = 1; i <= 6; i++)
	    	{
	    		if(uart_buffer[i] == '^')
	    		{
	    			count++;
	    		}
	    	}
	    	if(count == 0)
	    	{
	    		if(uart_buffer[3] == '\0')
	    		{
	    			uart_buffer[3] = uart_buffer[2];
	    			uart_buffer[2] = '0';
	    		}
				ring_min = uart_buffer[2] * 10 + uart_buffer[3];
				ring_min -= 528;
	    	}

	    	for(int i = 0; i<=6; i++)
		    {
				uart_buffer[i] = '\0';
				uart_pointer = 0;
		    }
	    	count = 0;
	    	uart_flag = 0;
		}
		else
		{
	    	for(int i = 0; i<=6; i++)
		    {
				uart_buffer[i] = '\0';
				uart_pointer = 0;
		    }
	    	 uart_pointer = 0;
			uart_flag = 0;
			isOK = 3;
	    }
	}
}

void Read_DataDHT(void)
{
	DHT_GetData(&DHT11_Data);
	Temperature = DHT11_Data.Temperature;
	Humidity = DHT11_Data.Humidity;
}
Button_TypeDef buttonmode;
Button_TypeDef buttonup;
Button_TypeDef buttondown;
Button_TypeDef *ButtonX;

typedef enum
{
	BINHTHUONG = 0,
	CHINHPHUT = 1,
	CHINHGIO = 2,
	CHINHNGAY = 3,
}Mode;

Mode mode; // khai báo là biến "mode" có kiểu dữ liệu "Mode"

//////////////////////////////////////////////////////////////CHƯƠNG TRÌNH PHỤC VỤ NGẮT//////////////////////////////////////////////////////////////
void btn_pressing_callback(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	if ((GPIOx == GPIOA) && (GPIO_Pin == GPIO_PIN_0))
	{
		switch (mode)
		{
		case BINHTHUONG: // mode = 0
			datetime.date = date_temp; //gán th ?i gian vừa cài đặt xong cho biến official (biến mà RTC sẽ tự động cập nhật)
			mode = CHINHPHUT;  // gán mode 0 thành 1
			fill_rectangle(110, 200, 230, 210, COLOR_WHITE);//toa do cua phut
			draw_string(110, 200, COLOR_GREEN, 1, strtempmin);//toa do cua phut
			break; // đang ở case BINHTHUONG thì chương trình sẽ gán cho biến mode (dòng 8	4), sau đó break ra kh ?i switch, rồi khi này chương trình sẽ không thực hiện lệnh gì trong khi đợi đi ?u kiện của 1 trong 3 cái if ở ngắt là đúng;
		// Vậy lúc này, nếu bấm button_mode một lần nữa thì chương trình sẽ xét switch rồi nhảy vô case CHINHGIO vì biến "mode" đã được gán = 2 ở dòng 88; Else nhấn button_up một lần thì chương trình
		// sẽ nhảy vào chạy hàm btn_adjust(up) hoặc nếu nhấn button_down thì chương trình sẽ nhảy vào chạy hàm btn_adjust(down).
		case CHINHPHUT: // mode = 1
//			datetime.min = min_temp; //Dat chinh
			min_temp = datetime.min;
			mode = CHINHGIO; // gán mode = 2
			fill_rectangle(110, 200, 230, 210, COLOR_WHITE);//toa do cua gio
			draw_string(110, 200, COLOR_GREEN, 1, strtemphour);//toa do cua gio
			break;
		case CHINHGIO: // mode = 2
			datetime.min = min_temp;
			hour_temp = datetime.hour;
//			datetime.hour = hour_temp; //Dat chinh
//			datetime.min = min_temp; //gán th ?i gian vừa cài đặt xong cho biến official (biến mà RTC sẽ tự động cập nhật)
			mode = CHINHNGAY; // gán mode = 3
			fill_rectangle(85, 215, 205, 235, COLOR_WHITE);
			draw_string(40, 220, COLOR_BLACK, 1, str2);
			break;
		case CHINHNGAY:      // mode = 3
//			datetime.date = date_temp; //Dat chinh
			datetime.hour = hour_temp; //gán th ?i gian vừa cài đặt xong cho biến official (biến mà RTC sẽ tự động cập nhật)
			date_temp = datetime.date;
			mode = BINHTHUONG; // gán mode = 0
			break;
		}
	}
  ////////////////BUTTON_UP///////////////
	if ((GPIOx == GPIOA) && (GPIO_Pin == GPIO_PIN_1))
	{
		btn_adjust_up();
	}

  ///////////////BUTTON_DOWN////////////
	if ((GPIOx == GPIOA) && (GPIO_Pin == GPIO_PIN_2))
	{
	    btn_adjust_down();
	}
}

//                    Tạo hàm sẽ chạy khi nhấn button_up
void btn_adjust_up (void)
{
	switch(mode)
	{
     	case CHINHPHUT:
			// viet code de cong 1 phut va khi den 60 thi quay lai
				min_temp ++;
				if (min_temp == 60)
				{
					min_temp = 0;
				}
//				draw_string(40, 200, COLOR_BLACK, 2, str1);
			  sprintf(strtempmin, "%.2d", datetime.min);
			fill_rectangle(110, 200, 230, 210, COLOR_WHITE);//toa do cua phut
			draw_string(40, 200, COLOR_GREEN, 1, strtempmin);//toa do cua phut
			break;
		case CHINHGIO:
			// viet code de cong 1 gio va khi den 60 thi quay lai
				hour_temp ++;
				if (hour_temp == 24)
				{
					hour_temp = 0;
				}
				fill_rectangle(110, 200, 230, 210, COLOR_WHITE);//toa do cua gio
				draw_string(40, 200, COLOR_GREEN, 1, strtemphour);//toa do cua gio
			 break;
		case CHINHNGAY:
			// viet code de cong 1 ngay va biet duoc so ngay trong thang de quay lai
			date_temp ++;
					switch (datetime.month)   // xét xem tháng hiện tại có tổng bao nhiêu ngày
					{
						case 1:
						case 3:
						case 5:
						case 7:
						case 8:
						case 10:
						case 12:
							date_max = 31 + 1;
							break;
						case 4:
						case 6:
						case 9:
						case 11:
							date_max = 30 + 1;
						case 2:
							if ((datetime.year - 2014) % 4 == 0)
							{
								date_max = 29 + 1;
							}
							else
							{
								date_max = 28 + 1;
							}
							break;
						default:
							break;
					}
					if (date_temp == date_max)
					{
						date_temp = 1;
					}
			fill_rectangle(85, 215, 205, 235, COLOR_WHITE);
//					draw_string(40, 220, COLOR_BLACK, 2, str2); // in cả string 2 (nhưng chỉ có thông số date thay đổi).
			draw_string(40, 220, COLOR_BLACK, 1, str2); // in cả string 2 (nhưng chỉ có thông số date thay đổi).
//			date_temp ++;
			break;
	}
}
//                       Tạo hàm sẽ chạy khi nhấn button_down
void btn_adjust_down(void)
{
	switch(mode)
	{
//		case BINHTHUONG:
//			break;
		case CHINHPHUT:
			// viet code de tru 1 phut va khi den 0 thi quay lai
					min_temp --;
					if (min_temp == -1)
					{
						min_temp = 59;
					}
//					draw_string(40, 200, COLOR_BLACK, 2, str1);
			fill_rectangle(110, 200, 230, 210, COLOR_WHITE);//toa do cua phut
			draw_string(40, 200, COLOR_BLACK, 1, str1);//toa do cua phut
//			min_temp --;
			break;
		case CHINHGIO:
			// viet code de tru 1 gio va khi den 0 thi quay lai
					hour_temp --;
					if (hour_temp == -1)
					{
						hour_temp = 23;
					}
//					draw_string(40, 200, COLOR_BLACK, 2, str1);
			fill_rectangle(110, 200, 230, 210, COLOR_WHITE);//toa do cua gio
			draw_string(40, 200, COLOR_BLACK, 1, str1);//toa do cua gio
//			hour_temp --;
//			fill_rectangle(110, 200, 230, 210, COLOR_WHITE);
			break;
		case CHINHNGAY:
			// viet code de tru 1 ngay va biet duoc so ngay trong thang de quay lai
					date_temp --;
					switch (datetime.month)   // xét xem tháng hiện tại có tổng bao nhiêu ngày
					{
						case 1:
						case 3:
						case 5:
						case 7:
						case 8:
						case 10:
						case 12:
							date_max = 31 + 1;
							break;
						case 4:
						case 6:
						case 9:
						case 11:
							date_max = 30 + 1;
						case 2:
							if ((datetime.year - 2014) % 4 == 0) // nếu năm hiện tại là năm nhuận thì số tổng số ngày của tháng 2 ở năm hiện tại là 29, còn không thì là 28.
							{
								date_max = 29 + 1;
							}
							else
							{
								date_max = 28 + 1;
							}
							break;
						default:
							break;
					}
					if (date_temp == 0)
					{
						date_temp = date_max - 1;
					}
//					draw_string(40, 220, COLOR_BLACK, 2, str2); // in cả string 2 (nhưng chỉ có thông số date thay đổi).
			fill_rectangle(85, 215, 205, 235, COLOR_WHITE);
			draw_string(40, 220, COLOR_BLACK, 1, str2);  // in cả string 2 (nhưng chỉ có thông số date thay đổi).
//			date_temp --;
			break;
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
    mode = 0; // Cho chương trình chạy ở MODE 0 trước tiên khi mới bắt đầu.
    DHT_Start();
    HAL_UART_Receive_IT(&huart1, &Rx_Buf, 1); // Nhận Data
	button_init(&buttonmode, GPIOA, GPIO_PIN_0);
	button_init(&buttonup, GPIOA, GPIO_PIN_1);
	button_init(&buttondown, GPIOA, GPIO_PIN_2);
	lcd_init();
	fill_rectangle(0, 0, WIDTH, HEIGHT, COLOR_WHITE);
	draw_string(0, 50, COLOR_GREENYELLOW, 5, "HELLO");
	HAL_Delay(2500);
	fill_rectangle(0, 0, WIDTH, HEIGHT, COLOR_WHITE);

    // Cài đặt th ?i gian ban đầu
	datetime.second = 15;
	datetime.min = 44;
	datetime.hour = 12;
	datetime.day = 8;
	datetime.date = 58;
	datetime.month = 12;
	datetime.year = 23;
	RTC_WriteTime(&datetime);

	draw_string(20, 50, COLOR_GOLD, 5, "QMDP");
	HAL_Delay(2500);
//  fill_rectangle(0, 0, 276, 220, COLOR_WHITE);
	fill_rectangle(0, 0, WIDTH, HEIGHT, COLOR_WHITE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  button_handle(&buttonmode);
	  button_handle(&buttonup);
	  button_handle(&buttondown);
////// Hàm xử lí UART //////
	  UART_Handle();
////////////////////////////
	  datetime.min = min_temp;

	  RTC_ReadTime(&datetime); //Lệnh đ ?c th ?i gian từ RTC (biến khác không có trong main.c)
	  second_temp = datetime.second;
	  min_temp = datetime.min;
	  hour_temp = datetime.hour;
	  date_temp = datetime.date;
	  month_temp = datetime.month;
	  year_temp = datetime.year;
	  sprintf(str1, "Time %.2d:%.2d:%.2d", datetime.hour, datetime.min, datetime.second);

	  if(second_temp != second_present)
	  {
		  fill_rectangle(140, 100, 160, 110, COLOR_WHITE);
		  second_present = second_temp;
	  }
	  draw_string(40, 100, COLOR_BLACK, 1, str1);

	  if(min_temp != min_present)
	  	  {
	  		  fill_rectangle(110, 100, 130, 110, COLOR_WHITE);
	  		  min_present = min_temp;
	  	  }
	  draw_string(40, 100, COLOR_BLACK, 1, str1);

	  if(hour_temp != hour_present)
	 	  	  {
	 	  		  fill_rectangle(85, 100, 105, 110, COLOR_WHITE);
	 	  		  hour_present = hour_temp;
	 	  	  }
	  draw_string(40, 100, COLOR_BLACK, 1, str1);

	  sprintf(str2,"Date %.2d/%.2d/%.2d",datetime.date,datetime.month,datetime.year );
// để sprintf ở đây cho nó chính xác, date, month, year lâu lắm
	  //để sprintf ở đây để convert, lệnh in lên LCD nằm trong thư viện của LCD
	  if(date_temp != date_present)
	  	 	  	  {
	  	 	  		  fill_rectangle(85, 115, 105, 135, COLOR_WHITE);
	  	 	  		  date_present = date_temp;
	  	 	  	  }
	  	  draw_string(40, 120, COLOR_BLACK, 1, str2);

	  	if(month_temp != month_present)
	  		 	  	  {
	  		 	  		  fill_rectangle(110, 115, 130, 135, COLOR_WHITE);
	  		 	  		  month_present = month_temp;
	  		 	  	  }
	  		  draw_string(40, 120, COLOR_BLACK, 1, str2);

	  		if(year_temp != year_present)
	  			 	  	  {
	  			 	  		  fill_rectangle(135, 115, 160, 135, COLOR_WHITE);
	  			 	  		  year_present = year_temp;
	  			 	  	  }
	  			  draw_string(40, 120, COLOR_BLACK, 1, str2);
       fill_rectangle(150, 20, 220, 50, COLOR_WHITE);
       Read_DataDHT();

/////////////////////////////////////////////////////////////
	   snprintf(temperature_string, 2, "%f", Temperature);
	   snprintf(humidity_string, 2, "%f", Humidity);
//      draw_string(150, 20, COLOR_BLUE, 1, temperature_string);
//	  draw_string(150, 40, COLOR_BLUE, 1, humidity_string);

//	   snprintf(data_string, 7, "*%02g%02g#", Temperature, Humidity);
//	   HAL_UART_Transmit(&huart1, (uint8_t*)data_string, 7, 200);
	   ////////////////////////////////////////////////////
	   HAL_Delay(1000);
  /* USER CODE END 3 */
 }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, OUT_DHT11_Pin|CMD_Pin|CS_Pin|RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : BUTTON_MODE_Pin BUTTON_UP_Pin BUTTON_DOWN_Pin */
  GPIO_InitStruct.Pin = BUTTON_MODE_Pin|BUTTON_UP_Pin|BUTTON_DOWN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : OUT_DHT11_Pin CMD_Pin CS_Pin RST_Pin */
  GPIO_InitStruct.Pin = OUT_DHT11_Pin|CMD_Pin|CS_Pin|RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

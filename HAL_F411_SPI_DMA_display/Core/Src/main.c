/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "./ILI9341_DMA_library/ILI9341_DMA_driver.h"
#include "./ILI9341_DMA_library/ILI9341_GFX.h"

//include Adafruit fonts
#include "./ILI9341_DMA_library/fonts/FreeMono12pt7b.h"
#include "./ILI9341_DMA_library/fonts/FreeSerifBold24pt7b.h"

//include bitmaps
#include "ILI9341_DMA_library/bitmaps/palette.h"
#include "ILI9341_DMA_library/bitmaps/settings_active.h"
#include "ILI9341_DMA_library/bitmaps/telemetry_active.h"
#include "ILI9341_DMA_library/bitmaps/calibration_active.h"

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

/* USER CODE BEGIN PV */
volatile uint8_t SPI2_TX_completed_flag = 1;    //flag indicating finish of SPI transmission
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//printf for serial port
int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart2, (uint8_t*) ptr, len, 50);
	return len;
}

//SPI transmission finished interrupt callback
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	SPI2_TX_completed_flag = 1;
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
  MX_DMA_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  ILI9341_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		//----------------------------------------------------------PERFORMANCE TEST
		ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
		uint32_t Timer_Counter = 0;
		for (uint32_t j = 0; j < 2; j++)
		{
			HAL_TIM_Base_Start(&htim1);
			for (uint16_t i = 0; i < 10; i++)
			{
				ILI9341_Fill_Screen(BLUE);
				ILI9341_Fill_Screen(YELLOW);
			}
		}

		HAL_TIM_Base_Stop(&htim1);
		Timer_Counter += __HAL_TIM_GET_COUNTER(&htim1);
		__HAL_TIM_SET_COUNTER(&htim1, 0);

		Timer_Counter /= 2;

		char counter_buff[30];
		ILI9341_Fill_Screen(WHITE);

		double seconds_passed = 2 * ((float) Timer_Counter / 20000);
		sprintf(counter_buff, "Time: %.3f Sec", seconds_passed);
		printf("%s\n", counter_buff);
		ILI9341_Draw_Text(counter_buff, 10, 30, BLACK, 2, WHITE);

		double timer_float = 20 / (((float) Timer_Counter) / 20000);//Frames per sec

		sprintf(counter_buff, "FPS:  %.2f", timer_float);
		printf("%s\n", counter_buff);
		ILI9341_Draw_Text(counter_buff, 10, 50, BLACK, 2, WHITE);
		double MB_PS = timer_float * 240 * 320 * 2 / 1000000;
		sprintf(counter_buff, "MB/S: %.2f", MB_PS);
		printf("%s\n", counter_buff);
		ILI9341_Draw_Text(counter_buff, 10, 70, BLACK, 2, WHITE);
		double SPI_utilized_percentage = (MB_PS / (6.25)) * 100;//50mbits / 8 bits
		sprintf(counter_buff, "SPI Utilized: %.2f", SPI_utilized_percentage);
		printf("%s\n", counter_buff);
		ILI9341_Draw_Text(counter_buff, 10, 90, BLACK, 2, WHITE);
		HAL_Delay(5000);


		//--------------------------------------------------------- DRAW FULL SCREEN BITMAP
		ILI9341_Draw_Image((uint8_t*)palette, 0, 0, 320, 240);
		HAL_Delay(5000);

		//--------------------------------------------------------- DRAW SMALL BITMAPS

		ILI9341_Fill_Screen(BLACK);
		ILI9341_Draw_Image((uint8_t*)calibration_active, 0, 0, 45, 45);
		ILI9341_Draw_Image((uint8_t*)settings_active, 45, 45, 45, 45);
		ILI9341_Draw_Image((uint8_t*)telemetry_active, 90, 90, 45, 45);
		HAL_Delay(5000);

		//--------------------------------------------------------- DRAW SOME TEXT WITH ADAFRUIT FONT
		ILI9341_Fill_Screen(BLACK);
		ILI9341_set_adafruit_font(&FreeSerifBold24pt7b);
		ILI9341_Draw_TextFont("J.R.R. Tolkien", 10, 40, YELLOW, 1, BLACK);
		ILI9341_Draw_TextFont("\"The Hobbit\"", 15, 100, YELLOW, 1, BLACK);

		HAL_Delay(3000);
		//--------------------------------------------------------- DRAW SOME TEXT WITH ADAFRUIT FONT
		ILI9341_Fill_Screen(BLACK);
		ILI9341_set_adafruit_font(&FreeMono12pt7b);

		ILI9341_Draw_TextFont("In a hole in the ground", 0, 20, YELLOW, 1, BLACK);
		ILI9341_Draw_TextFont("there lived a hobbit.", 0, 40, YELLOW, 1, BLACK);
		ILI9341_Draw_TextFont("Not a nasty, dirty, wet", 0, 60, YELLOW, 1, BLACK);
		ILI9341_Draw_TextFont("hole, filled with the", 0, 80, YELLOW, 1, BLACK);
		ILI9341_Draw_TextFont("ends of worms and an", 0, 100, YELLOW, 1, BLACK);
		ILI9341_Draw_TextFont("ozzy smell, nor yet a", 0, 120, YELLOW, 1, BLACK);
		ILI9341_Draw_TextFont("dry, bare, sandy hole ", 0, 140, YELLOW, 1, BLACK);
		ILI9341_Draw_TextFont("with nothing to sit", 0, 160, YELLOW, 1, BLACK);
		ILI9341_Draw_TextFont("down on or to eat: it", 0, 180, YELLOW, 1, BLACK);
		ILI9341_Draw_TextFont("was a hobbit-hole, and ", 0, 200, YELLOW, 1, BLACK);
		ILI9341_Draw_TextFont("that means comfort.", 0, 220, YELLOW, 1, BLACK);

		HAL_Delay(15000);



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "time.h"
#include "hx711.h"
#include "tm_stm32_ili9341.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "ILI9341_Touchscreen.h"
#include "menu.h"
#include "time.h"
#include "flash_ee.h"
#include "FlashEE.h"
#include <math.h>
#include "flash.h"
//#include "core_cm0.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t ch;
static uint8_t wait_tmr = NO_TIMER, turn_off_tmr = NO_TIMER;
uint8_t pwm_value = 0;
bool countup = 1;

flash_ee_t gram;

uint8_t turn_off = 1;

uint16_t battery_voltage;
uint16_t ADC1_buf[2];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 *
 * @retval None
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

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
	MX_USART1_UART_Init();
	MX_IWDG_Init();
	MX_TIM1_Init();
	MX_SPI1_Init();
	MX_ADC1_Init();
	MX_CRC_Init();
	/* USER CODE BEGIN 2 */
	HAL_IWDG_Init(&hiwdg);

	TM_ILI9341_Init();

	printf("\r\n");
	printf("RC weight scale V0.1\r\n");
	printf(__TIME__);
	printf("\r\n");
	printf(__DATE__);
	printf("\r\n");
	printf("\r\n");

	if (RCC->CSR & RCC_CSR_IWDGRSTF)
		printf("WDT reboot\r\n");
	RCC->CSR = RCC_CSR_RMVF;

	HAL_TIM_Base_Start_IT(&htim1); // Start Timer 2 IRQ
	timer_open();

	Init_get_battery_voltage(LOW_BATTERY_VOLTAGE + 200, &battery_voltage);
	//HAL_Delay(1000);

	adc_calib();
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) ADC1_buf, 1);
	HAL_ADC_Start(&hadc1);

	EN_SET;

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{

		HAL_IWDG_Refresh(&hiwdg);

		menu_handler(&battery_voltage);

		touchscreen_pressed();

		if (sci1_get(&ch))
		{
			sci1_putc(ch);

			if (ch == 'a')
			{
				calibrate_scale();
				printf("zero the scale\r\n");
			}

			if (ch == 'b')
			{
				uint16_t x;
				uint16_t y;

				TP_Read_Coordinates(&x, &y);
				printf("LCD cordinates: x= %d, y= %d\r\n", x, y);
			}
			if (ch == 'c')
			{
				printf("value FR = %ld\r\n", measurement(1));
			}
			if (ch == 'd')
			{
				printf("value FL = %ld\r\n", measurement(2));
			}
			if (ch == 'e')
			{
				printf("value RR = %ld\r\n", measurement(3));
			}
			if (ch == 'f')
			{
				printf("value RL = %ld\r\n", measurement(4));
			}
			if (ch == 'g')
			{
				printf("TP4056 status: Standby= %d Charge= %d\r\n", STDBY, CHARGE);
			}
			if (ch == 'h')
			{
				gram.FL = 600;
				gram.FR = 580;
				gram.RL = 880;
				gram.RR = 950;
				printf("variables given a value\r\n");
			}
			if (ch == 'i')
			{
				if (WriteStruct2Flash(gram))
					printf("failed to write values to FLASH\r\n");

				else
					printf("values written to FLASH\r\n");

			}
			if (ch == 'j')
			{
				DumpRam(FLASH_USER_START_ADDR_ORG, 0x100);
			}
			if (ch == 'k')
			{
				flash_ee_t read;
				if (ReadStruct2Flash(&read))
				{
					printf("failed to read values to FLASH\r\n");
				}
				else
				{
					printf("FLASH values: FL= %d, FR= %d, RL= %d, RR= %d, crc= %08X\r\n", read.FL, read.FR, read.RL, read.RR, read.crc);
				}

			}
			if (ch == 'l')
			{
				printf("battery voltage= %d \r\n", battery_voltage);
			}
			if (ch == 'm')
			{
				printf("ADC= %d \r\n", ADC1_buf[0]);
			}
			if (ch == 'n')
			{
				bool standy = STDBY;
				bool opladen = CHARGE;

				printf("standy= %d, charge= %d \r\n", standy, opladen);
			}

		}

		switch (turn_off)
		{
		case 1:
			if (!SWTICH_EN)
			{
				turn_off = 2;
			}
			break;

		case 2:
			if (SWTICH_EN)
			{
				if (turn_off_tmr == NO_TIMER)
				{
					turn_off_tmr = timer_get();
					timer_start(turn_off_tmr, 50, NULL);
				}
				if (timer_elapsed(turn_off_tmr))
				{
					timer_free(&turn_off_tmr);
					turn_off = 3;
				}
			}
			if (battery_voltage < LOW_BATTERY_VOLTAGE)
			{
				EN_RESET;
				ENABLE_RESET;
			}
			break;

		case 3:
			if (!SWTICH_EN)
			{
				EN_RESET;
				ENABLE_RESET;
			}
			break;
		}

		if (wait_tmr == NO_TIMER)
		{
			wait_tmr = timer_get();
			timer_start(wait_tmr, 100, NULL);
		}

		if (timer_elapsed(wait_tmr))
		{
			timer_start(wait_tmr, 100, NULL);
			PCB_LED_TOG;
			get_battery_voltage(&battery_voltage, ADC1_buf[0]);
		}
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

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  file: The file name as string.
 * @param  line: The line in file as a number.
 * @retval None
 */
void _Error_Handler(char *file, int line)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
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
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

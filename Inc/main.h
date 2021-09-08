/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define SCK_Pin GPIO_PIN_15
#define SCK_GPIO_Port GPIOC
#define ADC_BATT_Pin GPIO_PIN_1
#define ADC_BATT_GPIO_Port GPIOA
#define DATA_FR_Pin GPIO_PIN_3
#define DATA_FR_GPIO_Port GPIOA
#define DATA_FL_Pin GPIO_PIN_4
#define DATA_FL_GPIO_Port GPIOA
#define ILI9341_WRX_Pin GPIO_PIN_0
#define ILI9341_WRX_GPIO_Port GPIOB
#define ILI9341_RST_Pin GPIO_PIN_1
#define ILI9341_RST_GPIO_Port GPIOB
#define ILI9341_CS_Pin GPIO_PIN_2
#define ILI9341_CS_GPIO_Port GPIOB
#define TP_CLK_Pin GPIO_PIN_12
#define TP_CLK_GPIO_Port GPIOB
#define TP_CS_Pin GPIO_PIN_13
#define TP_CS_GPIO_Port GPIOB
#define TP_MOSI_Pin GPIO_PIN_14
#define TP_MOSI_GPIO_Port GPIOB
#define TP_MISO_Pin GPIO_PIN_15
#define TP_MISO_GPIO_Port GPIOB
#define TP_IRQ_Pin GPIO_PIN_8
#define TP_IRQ_GPIO_Port GPIOA
#define EN_Pin GPIO_PIN_15
#define EN_GPIO_Port GPIOA
#define CHARGE_Pin GPIO_PIN_3
#define CHARGE_GPIO_Port GPIOB
#define STANDBY_Pin GPIO_PIN_4
#define STANDBY_GPIO_Port GPIOB
#define ENABLE_Pin GPIO_PIN_5
#define ENABLE_GPIO_Port GPIOB
#define SWITCH_EN_Pin GPIO_PIN_6
#define SWITCH_EN_GPIO_Port GPIOB
#define PCB_LED_Pin GPIO_PIN_7
#define PCB_LED_GPIO_Port GPIOB
#define DATA_RL_Pin GPIO_PIN_8
#define DATA_RL_GPIO_Port GPIOB
#define DATA_RR_Pin GPIO_PIN_9
#define DATA_RR_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

#define R1 10000
#define R2 10000
#define ADC_VOLTAGE 1
#define LOW_BATTERY_VOLTAGE 3400
#define LOW_BATTERY_WARNING 3500
#define EN_SET						HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, GPIO_PIN_SET)
#define EN_RESET					HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, GPIO_PIN_RESET)
#define ENABLE_RESET				HAL_GPIO_WritePin(ENABLE_GPIO_Port, ENABLE_Pin, GPIO_PIN_RESET)

#define ILI9341_RST_SET				HAL_GPIO_WritePin(ILI9341_RST_GPIO_Port, ILI9341_RST_Pin, GPIO_PIN_SET)
#define ILI9341_RST_RESET			HAL_GPIO_WritePin(ILI9341_RST_GPIO_Port, ILI9341_RST_Pin, GPIO_PIN_RESET)
#define ILI9341_CS_SET				HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_SET)
#define ILI9341_CS_RESET			HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_RESET)
#define ILI9341_WRX_SET				HAL_GPIO_WritePin(ILI9341_WRX_GPIO_Port, ILI9341_WRX_Pin ,GPIO_PIN_SET)
#define ILI9341_WRX_RESET			HAL_GPIO_WritePin(ILI9341_WRX_GPIO_Port, ILI9341_WRX_Pin, GPIO_PIN_RESET)

#define PCB_LED_TOG 				HAL_GPIO_TogglePin(PCB_LED_GPIO_Port, PCB_LED_Pin)
#define PCB_LED_SET					HAL_GPIO_WritePin(PCD_LED_GPIO_Port, PCD_LED_Pin, GPIO_PIN_SET)
#define PCB_LED_RESET				HAL_GPIO_WritePin(PCD_LED_GPIO_Port, PCD_LED_Pin, GPIO_PIN_RESET)

#define TOUCHED						HAL_GPIO_ReadPin(TP_IRQ_GPIO_Port, TP_IRQ_Pin)

#define SWTICH_EN					HAL_GPIO_ReadPin(SWITCH_EN_GPIO_Port, SWITCH_EN_Pin)

#define STDBY						HAL_GPIO_ReadPin(STANDBY_GPIO_Port, STANDBY_Pin)
#define CHARGE						HAL_GPIO_ReadPin(CHARGE_GPIO_Port, CHARGE_Pin)
/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

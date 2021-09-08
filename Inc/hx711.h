#ifndef HX711_H_
#define HX711_H_

#include "stm32f1xx_hal.h"
#include "stdbool.h"

#define bit_set(D,i) ((D) |= (0x01 << i))

//#define GRAM 229 // 10kg loadcell

#define SCK_SET HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_SET)
#define SCK_RESET HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_RESET)

//#define DATA_FR HAL_GPIO_ReadPin(DATA_RR_GPIO_Port, DATA_RR_Pin)
//#define DATA_FL HAL_GPIO_ReadPin(DATA_RL_GPIO_Port, DATA_RL_Pin)
//#define DATA_RR HAL_GPIO_ReadPin(DATA_FR_GPIO_Port, DATA_FR_Pin)
//#define DATA_RL HAL_GPIO_ReadPin(DATA_FL_GPIO_Port, DATA_FL_Pin)

void calibrate_scale(void);
bool measure_pin(uint8_t pin);
uint32_t measurement(uint8_t i);
//void weight_grams(uint32_t *weight_FR, uint32_t *weight_FL, uint32_t *weight_RR, uint32_t *weight_RL);
void weight_grams(uint32_t *weight_FR, uint32_t *weight_FL, uint32_t *weight_RR, uint32_t *weight_RL, uint16_t FR, uint16_t FL, uint16_t RR, uint16_t RL);
#endif /* HX711_H_ */



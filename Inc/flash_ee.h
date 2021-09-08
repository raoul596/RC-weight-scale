/*
 * flash_ee.h
 *
 *  Created on: 25 sep. 2016
 *      Author: Mark
 */

#ifndef FLASH_EE_H_
#define FLASH_EE_H_

#include "stdbool.h"

typedef struct
{
	uint16_t FL; // 2kg loadcell, 942.813
	uint16_t FR; // 2kg loadcell, 853.5549
	uint16_t RL;// 2kg loadcell, 738.5
	uint16_t RR; // 2kg loadcell, 527.5741
	uint32_t crc;

}flash_ee_t;

uint8_t WriteStruct2I2C(flash_ee_t to_save);
uint8_t ReadStruct2I2C(flash_ee_t* to_read);
uint8_t WriteStruct2Flash(flash_ee_t to_save);
uint8_t ReadStruct2Flash(flash_ee_t * to_read);



/* Exported functions ------------------------------------------------------- */

#endif /* FLASH_EE_H_ */

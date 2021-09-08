/*
 * FlashEE.c
 *
 *  Created on: Oct 3, 2018
 *      Author: raoul
 */
#include "stm32f1xx_hal.h"
#include "stdbool.h"
#include "FlashEE.h"
#include "flash.h"

#define DATA_32                 ((uint32_t)0x12345678)


#if 0
uint32_t Address = 0, PAGEError = 0;

HAL_StatusTypeDef flash_erase()
{
	/*Variable used for Erase procedure*/
	static FLASH_EraseInitTypeDef EraseInitStruct;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Erase the user Flash area
	 (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
	EraseInitStruct.NbPages = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		/*
		 Error occurred while page erase.
		 User can add here some code to deal with this error.
		 PAGEError will contain the faulty page and then to know the code error on this page,
		 user can call function 'HAL_FLASH_GetError()'
		 */
		return HAL_ERROR;
	}

	return HAL_OK;
}

HAL_StatusTypeDef flash_write()
{
	Address = FLASH_USER_START_ADDR;

	while (Address < FLASH_USER_END_ADDR)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, DATA_32) == HAL_OK)
		{
			Address = Address + 4;
		}
		else
		{
			return HAL_ERROR;
		}
	}

	return HAL_OK;
}
#endif
/*!
 * \brief
 *
 * \param
 *
 * \return -.
 */
void DumpRam(uint32_t minaddress, uint32_t size)
{
	uint32_t i;
	uint32_t * memory;
	uint8_t j;
	char flashbuff[BYTES_PER_ROW];
	uint32_t maxaddress;

	maxaddress = minaddress + size;

	printf("Read Programflash 0x%04X - 0x%04X\r\n", minaddress, maxaddress);
	for (i = minaddress; i <= maxaddress; i += BYTES_PER_ROW)
	{
		memory = (uint32_t *) i;
		memcpy(flashbuff, memory, BYTES_PER_ROW);
//memory += BYTES_PER_ROW;
		printf("%08X: ", i);
		for (j = 0; j <= min((BYTES_PER_ROW - 1), (maxaddress - i)); j++)
		{
			printf("%02X ", *(flashbuff + j));
			*(flashbuff + j) &= 0x7f;  // for char output
		}
		for (j = 0; j <= min((BYTES_PER_ROW - 1), (maxaddress - i)); j++)
		{
			if (*(flashbuff + j) >= 0x20 && *(flashbuff + j) != 0x7f)
				printf("%c", *(flashbuff + j));
			else
				printf(".");
		}
		printf("\r\n");

	}
}

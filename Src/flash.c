/*
 * flash.c
 *
 *  Created on: 24 jan. 2017
 *      Author: Mark Ursum
 *
 *       * STM32F070xB flash memory map:
 *
 * Sector  0: 0x08000000 - 0x08003FFF  (16k) Bootloader
 * Sector  1: 0x08004000 - 0x08007FFF  (16k)
 * Sector  2: 0x08008000 - 0x0800BFFF  (16k)
 * Sector  3: 0x0800C000 - 0x0800FFFF  (16k)
 * Sector  4: 0x08010000 - 0x0801FFFF  (64k)
 * Sector  5: 0x08020000 - 0x0803FFFF (128k) Application
 * Sector  6: 0x08040000 - 0x0805FFFF (128k)
 * Sector  7: 0x08060000 - 0x0807FFFF (128k)
 * Sector  8: 0x08080000 - 0x0809FFFF (128k)
 * Sector  9: 0x080A0000 - 0x080BFFFF (128k)
 * Sector 10: 0x080C0000 - 0x080DFFFF (128k)
 * Sector 11: 0x080E0000 - 0x080FFFFF (128k)
 */

#include "main.h"
#include "stm32f1xx_hal.h"
#include "flash.h"
#include "string.h"
#include "stdbool.h"
#include "iwdg.h"



static FLASH_EraseInitTypeDef EraseInitStruct;



/*!
 * \brief Erase the application memory.
 *
 * \param -
 *
 * \return false if unsuccessful, else true
 */
HAL_StatusTypeDef stm32_flash_erase(uint32_t StartAddr, uint32_t size)
{
	uint32_t PageError = 0;

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = StartAddr;
	EraseInitStruct.NbPages = size / FLASH_PAGE_SIZE;
//	EraseInitStruct.NbPages = (FLASH_APP_END_ADDR - FLASH_APP_START_ADDR) / FLASH_PAGE_SIZE;

	HAL_FLASH_Unlock();
	// Erase the flash sectors used by the application

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
	{
		return (1);
	}

	HAL_FLASH_Lock();

	return (0); // Oke
}

/*!
 * \brief Write data to the application memory.
 *
 * \param address   Start address of write.
 * \param p_data   Pointer to data to write.
 * \param size   Size of data to write.
 *
 * \return	0 - ERR_OK
 * 			1 - size overflow
 * 			2 - HAL error
 *
 * \note The flash memory must be erased before it can be written.
 */
error_t stm32_flash_write(uint32_t address, uint8_t *p_data, uint32_t size)
{
	uint16_t flubber;
	uint32_t n = 0;

	if ((address + size) > ADDR_FLASH_PAGE_127)
		return ERR_SIZE;

	// Perform bit shift since 2 bytes are written at a time
	size >>= 1;
	HAL_FLASH_Unlock();

	// Iterate through the number of data bytes
	while (n++ < size)
	{
		wdt_clr();
		flubber = *p_data++ ;
		flubber |= (*p_data++ << 8);

		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, flubber) != HAL_OK)
		{
			return (ERR_HAL); // fout
		}

		address += 2;
		//p_data += 2;
	}

	HAL_FLASH_Lock();

	return (ERR_OK);
}




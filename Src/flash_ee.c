/*
 * flash_ee.c
 *
 *  Created on: 25 sep. 2016
 *      Author: Mark
 */

#include "stm32f1xx_hal.h"
#include "flash_ee.h"
#include "string.h"
#include "stdbool.h"
#include "flash.h"

static FLASH_EraseInitTypeDef EraseInitStruct;
//static CRC_HandleTypeDef CrcHandle;
extern CRC_HandleTypeDef hcrc;

static uint8_t do_WriteStruct2Flash(flash_ee_t to_save, uint32_t start_addr, uint32_t end_addr);

/*!
 * \brief This function Saves a struct to internal Flash
 *
 * \param structure to save
 *
 * \return false if successful, true on error.
 */
HAL_StatusTypeDef WriteStruct2Flash(flash_ee_t to_save)
{
	uint8_t res = 0;
	// write block 1
	res = do_WriteStruct2Flash(to_save, FLASH_USER_START_ADDR_ORG, FLASH_USER_END_ADDR_ORG);
	// write block 2
	res |= do_WriteStruct2Flash(to_save, FLASH_USER_START_ADDR_CPY, FLASH_USER_END_ADDR_CPY);

	return res;
}

/*!
 * \brief This function Saves a struct to internal Flash
 *
 * \param structure to save
 *
 * \return false if successful, true on error.
 */
static uint8_t do_WriteStruct2Flash(flash_ee_t to_save, uint32_t start_addr, uint32_t end_addr)
{

	uint32_t Address = 0, PageError = 0, EndAddress = 0;
	uint8_t array[sizeof(to_save)];
	uint8_t cnt = 0;
	uint32_t *temp;
	/* Erase the user Flash area
	 (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = start_addr;
	EraseInitStruct.NbPages = (end_addr - start_addr) / FLASH_PAGE_SIZE;
	HAL_FLASH_Unlock();

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
	{
		printf("Can't delete flash\r\n");
		return (1);
	}
	FLASH_WaitForLastOperation(0xFFFF);

	/* Program the user Flash area word by word
	 (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
	memcpy(array, &to_save, sizeof(to_save));

	/*##-2- Compute the CRC of "aDataBuffer" ###################################*/
	to_save.crc = HAL_CRC_Calculate(&hcrc, (uint32_t *) &array, (sizeof(to_save) - 4)/4); //

	memcpy(array, &to_save, sizeof(to_save)); // array with new crc value

	Address = start_addr;
	EndAddress = Address + (sizeof(to_save));
	temp = (uint32_t *) &array[cnt];
	while (Address < EndAddress)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, *temp) == HAL_OK)
		{
			Address += 4;
			temp++;
		}
		else
		{
			printf("Can't set flash\r\n");
			return (1);
		}
	}

	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();
	return (0);
}

/*!
 * \brief This function Reads a struct from internal Flash
 *
 * \param structure to save
 *
 * \return false if successful, true on error.
 */
uint8_t ReadStruct2Flash(flash_ee_t * to_read)
{
	uint32_t Address = 0, EndAddress = 0;
	uint8_t cnt = 0;
	uint32_t *temp;
	char array[sizeof(flash_ee_t)];

	/* Check if the programmed data is OK
	 MemoryProgramStatus = 0: data programmed correctly
	 MemoryProgramStatus != 0: number of words not programmed correctly ******/
	Address = FLASH_USER_START_ADDR_ORG;
	EndAddress = Address + (sizeof(flash_ee_t));

	temp = (uint32_t*) &array[cnt];

	while (Address < EndAddress)
	{
		*temp = (*(__IO uint32_t *) Address);
		temp++;
		Address += 4;
	}
	memcpy(to_read, array, sizeof(flash_ee_t));

	if (to_read->crc == HAL_CRC_Calculate(&hcrc, (uint32_t *) &array, (sizeof(array) - 4)/4))
	{
		return (0);
	}

	// checksum fail try block 2
	else
	{
		printf("Read flash copy\r\n");
		/* Check if the programmed data is OK
		 MemoryProgramStatus = 0: data programmed correctly
		 MemoryProgramStatus != 0: number of words not programmed correctly ******/
		Address = FLASH_USER_START_ADDR_CPY;
		EndAddress = Address + (sizeof(flash_ee_t));

		temp = (uint32_t*) &array[cnt];

		while (Address < EndAddress)
		{
			*temp = (*(__IO uint32_t *) Address);
			temp++;
			Address += 4;
		}
		memcpy(to_read, array, sizeof(flash_ee_t));

		if (to_read->crc == HAL_CRC_Calculate(&hcrc, (uint32_t *) &array, (sizeof(array) - 4)/4))
			return (0);
		else
			return (1);

	}
}

#ifdef I2C
/*!
 * \brief This function writes the struct to I2C.
 *
 * \param struct to write
 *
 * \return false if successful, true on error.
 */
uint8_t WriteStruct2I2C(flash_ee_t to_save)
{
	uint8_t array[sizeof(to_save)];

	memcpy(array, &to_save, sizeof(to_save));

	/*##-2- Compute the CRC of "aDataBuffer" ###################################*/
	to_save.crc = HAL_CRC_Calculate(&CrcHandle, (uint32_t *) &array, sizeof(to_save) - 4); //

	memcpy(array, &to_save, sizeof(to_save));// array with new crc value

	return e2p_write(0, array, sizeof(array));
}

/*!
 * \brief This function read the struct from I2C.
 *
 * \param struct to write
 *
 * \return false if successful, true on CRC error.
 */
uint8_t ReadStruct2I2C(flash_ee_t* to_read)
{
	uint8_t array[sizeof(flash_ee_t)];

	e2p_read(0, array, sizeof(flash_ee_t));

	memcpy(to_read, array, sizeof(flash_ee_t));

	if (to_read->crc == HAL_CRC_Calculate(&CrcHandle, (uint32_t *) &array, sizeof(array) - 4))
	return (0);
	else
	return (1);
}

#endif

#if 0
/*!
 * \brief This function Saves a struct to internal Flash
 *
 * \param structure to save
 *
 * \return false if successful, true on error.
 */
uint8_t WriteStruct2Flash(flash_ee_t to_save)
{

	uint32_t Address = 0, PageError = 0, EndAddress = 0;
	uint8_t array[sizeof(to_save)];
	uint8_t cnt = 0;
	uint32_t *temp;
	/* Erase the user Flash area
	 (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
	EraseInitStruct.NbPages = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;
	HAL_FLASH_Unlock();

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
	{
		return (1);
	}

	/* Program the user Flash area word by word
	 (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
	memcpy(array, &to_save, sizeof(to_save));

	/*##-2- Compute the CRC of "aDataBuffer" ###################################*/
	to_save.crc = HAL_CRC_Calculate(&CrcHandle, (uint32_t *) &array, sizeof(to_save) - 4); //

	memcpy(array, &to_save, sizeof(to_save));// array with new crc value

	Address = FLASH_USER_START_ADDR;
	EndAddress = Address + (sizeof(to_save));
	temp = (uint32_t *) &array[cnt];
	while (Address < EndAddress)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, *temp) == HAL_OK)
		{
			Address += 4;
			temp++;
		}
		else
		return (1);
	}
	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();
	return (0);
}

/*!
 * \brief This function Reads a struct from internal Flash
 *
 * \param structure to save
 *
 * \return true if successful, false on CRC error.
 */
bool ReadStruct2Flash(flash_ee_t* to_read)
{
	uint32_t Address = 0, EndAddress = 0;
	uint8_t cnt = 0;
	uint32_t *temp;
	char array[sizeof(flash_ee_t)];

	/* Check if the programmed data is OK
	 MemoryProgramStatus = 0: data programmed correctly
	 MemoryProgramStatus != 0: number of words not programmed correctly ******/
	Address = FLASH_USER_START_ADDR;
	EndAddress = Address + (sizeof(flash_ee_t));

	temp = (uint32_t*) &array[cnt];

	while (Address < EndAddress)
	{
		*temp = (*(__IO uint32_t *) Address);
		temp++;
		Address += 4;
	}
	memcpy(to_read, array, sizeof(flash_ee_t));

	if (to_read->crc == HAL_CRC_Calculate(&CrcHandle, (uint32_t *) & array, sizeof(array) - 4))
	return (1);
	else
	return (0);
}
#endif

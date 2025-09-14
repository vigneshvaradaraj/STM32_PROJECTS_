/*
 * flash.h
 *
 *  Created on: Aug 30, 2025
 *      Author: vigneshv
 */

#ifndef FLASH_FLASH_H_
#define FLASH_FLASH_H_

#include <stdint.h>

#define buffer_size (1024)
#define USER_FLASH_ADD (0x08100000)

#define FLASH_KEY1                0x45670123
#define FLASH_KEY2                0xCDEF89AB

#define CB_START_ADDRESS (USER_FLASH_ADD + 1024)
#define CB_SIZE (24)
#define CB_END  (CB_START_ADDRESS + CB_SIZE)

void Flash_Unlock(void);
void Flash_Lock(void);
void Flash_EraseSector(uint32_t Sector);
void Flash_WriteData(uint32_t Address, uint8_t *Data, uint32_t Length);
void Flash_ReadData(uint32_t Address, uint32_t *Data, uint32_t Length);

#endif /* FLASH_FLASH_H_ */

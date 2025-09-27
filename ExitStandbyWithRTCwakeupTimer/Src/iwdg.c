/*
 * iwdg.c
 *
 *  Created on: 23-Sep-2025
 *      Author: vigneshv
 */
#include "rtc.h"
#include "iwdg.h"

#define IWDG_KEY_ENABLE 			0x0000CCCCU
#define IWDG_KEY_WR_ACCESS_ENABLE 	0x00005555U
#define IWDG_PRESCALER_4 			0x00000000U
#define IWDG_RELOAD_VAL				0xFFFU

#define READ_BIT(REG, BIT) 					((REG) & (BIT))

static uint32_t isIwdg_ready(void)
{
	return ((READ_BIT(IWDG->SR, IWDG_SR_PVU | IWDG_SR_RVU) == 0U)? 1UL : 0UL);
}
void iwdg_init(void)
{
	/*Enable the IWDG by writing 0x0000CCCCin the IWDG_KR register*/
	IWDG->KR = IWDG_KEY_ENABLE;

	/*Enable register access by writing 0x0000 5555 in the IWDG_KR register*/
	IWDG->KR = IWDG_KEY_WR_ACCESS_ENABLE;

	/*Set the IWDG Prescaler*/
	/*by default input clock divided by 4*/
	IWDG->PR = IWDG_PRESCALER_4;

	/*Set the reload register(IWDG_RLR) to the largest value 0xFFF*/
	IWDG->RLR = IWDG_RELOAD_VAL;

	/*Wait for the register to be upated (IWDG_SR = 0x0000 0000)*/
	while(isIwdg_ready() != 1);

	/*Refresh the counter value with IWDG_KR (IWDG_KR = 0x0000 AAAA)*/
	IWDG->KR = IWDG_KEY_RELOAD;
}


/*
 * standby_mode.c
 *
 *  Created on: 27-Sep-2025
 *      Author: vigneshv
 */
#include "stm32_f429xx.h"
#include "standby_mode.h"
#include "cm4.h"
#define WK_PIN 				(1U << 0U)
#define PWR_MODE_STANDBY 	(PWR_CR_PDDS)

void wakeup_pin_init(void)
{
	/*Enable clock to GPIOC*/
	RCC->AHB1ENR |= (1U << 2);

	/*Set input mode for PC13*/
	/*by default it will be input*/
	GPIOC->MODER &= ~(1U << 26);
	GPIOC->MODER &= ~(1U << 27);
}
void standby_wakeup_pin_setup(void)
{
	/*Wait for wakeup pin to be released*/
	while(get_wakeup_pin_state() == 0);

	/*Disable wakeup pin*/
	PWR->CSR &= ~(1U << 8U);

	/*Clear all wakeup flags*/
	PWR->CR |= (1U << 2U);
	/*Enable wakeup pin*/
	PWR->CSR |= (1U << 8U);
	/*Enter standby mode*/
	set_power_mode(PWR_MODE_STANDBY);
	/*Set SLEEPDEEP bit in the system control register of Cortex-M*/
	SCB->SCR |= (1U << 2U);
	/*Wait for interrupt*/
	__asm volatile ("wfi");
}

uint32_t get_wakeup_pin_state(void)
{
	return ((GPIOA->IDR & WK_PIN) == WK_PIN);
}

void set_power_mode(uint32_t pwr_mode)
{
	MODIFY_REG(PWR->CR,(PWR_CR_PDDS|PWR_CR_LPDS|PWR_CR_FPDS|PWR_CR_LPLVDS|PWR_CR_MRLVDS),pwr_mode);
}




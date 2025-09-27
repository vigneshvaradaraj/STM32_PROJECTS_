/*
 * gpio_exti.c
 *
 *  Created on: 23-Sep-2025
 *      Author: vigneshv
 */

#include "gpio_exti.h"
#include "cm4.h"

void gpio_interrupt_init(void)
{
	/*Enable clock to GPIOC*/
	RCC->AHB1ENR |= (1U << 2);

	/*Set input mode for PC13*/
	/*by default it will be input*/
	GPIOC->MODER &= ~(1U << 26);
	GPIOC->MODER &= ~(1U << 27);

	/*Enable clock access to the SYSCFG*/
	RCC->APB2ENR |= (1U << 14U);

	/*select PORTC for EXTI13*/
	SYSCFG->EXTICR[3] |= (1U << 5U);

	/*Unmask EXTI13*/
	EXTI->IMR |= (1U << 13U);

	/*Select falling edge trigger*/
	EXTI->FTSR |= (1U << 13U);

	/*Enable EXTI13 line in NVIC*/
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}



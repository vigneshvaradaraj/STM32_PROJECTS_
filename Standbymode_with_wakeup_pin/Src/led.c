/*
 * led.c
 *
 *  Created on: 23-Sep-2025
 *      Author: vigneshv
 */

#include "led.h"

void led_init(void)
{
	/*Enable clock for PORT A*/
	RCC->AHB1ENR |= (1U << 0U);

	/*Enable output mode PA5*/
	GPIOA->MODER |= (1U << 10U);
	GPIOA->MODER &= ~(1U << 11U);
}

void led_on(void)
{
	GPIOA->ODR |= (1U << 5U);
}

void led_off(void)
{
	GPIOA->ODR &= ~(1U << 5U);
}

void led_toggle(void)
{
	GPIOA->ODR ^= (1U << 5U);
	for(int i = 0;i < 90000;i++);
}



/*
 * gpio.c
 *
 *  Created on: Aug 30, 2025
 *      Author: vigneshv
 */
#include "stm32_f429xx.h"
#include "cm4.h"
#include "gpio.h"
buttonPressState_t buttonState = BUTTON_PRESSED;

//TODO: clock init, systick init

void init_gpio(void)
{
	//init clock for PORTA
	RCC->AHB1ENR |= (1 << 0);

	//enable clock for syscfg
	RCC->APB2ENR |= (1 << 14);

	//select input mode for PORTA PIN0
	GPIOA->MODER |= (0 << 0);//By default it will be input state

	//select EXTI0 as PA0 interrupt
	SYSCFG->EXTICR[0] &= ~(0xf << 0);

	//enable rising edge trigger
	EXTI->RTSR |= (1 << 0);

	//enable falling edge trigger
	EXTI->FTSR |= (1 << 0);

	//enable exti0 to interrupt the NVIC
	EXTI->IMR |= (1 << 0);

	//enable NVIC EXTI0_IRQn
	NVIC_EnableIRQ(EXTI0_IRQn);
}

uint8_t readButton(void)
{
	return 0;
}
uint8_t val = 0;
void EXTI0_IRQHandler(void)
{
	//post long press or short press event here
	switch(buttonState)
	{
		case BUTTON_PRESSED:
		{
			buttonState = BUTTON_RELEASED;
			break;
		}
		case BUTTON_RELEASED:
		{
			//post event
			buttonState = BUTTON_PRESSED;
			break;
		}
	}
	//clear the pending register
	EXTI->PR |= (1 << 0);
}



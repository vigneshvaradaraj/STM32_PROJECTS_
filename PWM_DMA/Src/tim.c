/*
 * tim.c
 *
 *  Created on: 09-Sep-2025
 *      Author: vigneshv
 */
#include "tim.h"
#include "stm32_f429xx.h"

#define GPIOAEN   (1U << 0U)
#define TIM2EN   (1U << 0U)
#define CR1_CEN   (1U << 0U)
/*To know pin and alternate functionality go to datasheet*/

void tim2_pa0_pa1_pwm_init(void)
{
	/*GPIO Cfg*/
	/*Enable clock acess to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;
	/*Set PA0 and PA1 mode to alternate function mode*/
	GPIOA->MODER |= (2U << 0);
	GPIOA->MODER |= (2U << 2);
	/*Set alternate function type to TIM2 (AF01)*/
	/*PA0*/
	GPIOA->AFR[0] |= (1U << 0U);
	GPIOA->AFR[0] &= ~(1U << 1U);
	GPIOA->AFR[0] &= ~(1U << 2U);
	GPIOA->AFR[0] &= ~(1U << 3U);
	/*PA1*/
	GPIOA->AFR[0] |= (1U << 4U);
	GPIOA->AFR[0] &= ~(1U << 5U);
	GPIOA->AFR[0] &= ~(1U << 6U);
	GPIOA->AFR[0] &= ~(1U << 7U);
	/*TIM2 Cfg*/
	/*Enable clock access to TIM2*/
	RCC->APB1ENR |= TIM2EN;
	/*Set timer prescaler*/
	/*APB1 = 16Mz Bydefault*/
	TIM2->PSC = 0;
	/*Set auto-reload value*/
	/*16 000 00 / 100 = 16 000 .ie 16KHz */
	TIM2->ARR = 100 - 1;
	/*Reset the timer count*/
	TIM2->CNT = 0;
	/*Set PA1 (ch2) and PA0 (ch1) to 110 : PWM mode 1*/
	/*CH1*/
	TIM2->CCMR1 &= ~(1U << 4U);
	TIM2->CCMR1 |= (1U << 5U);
	TIM2->CCMR1 |= (1U << 6U);
	/*CH2*/
	TIM2->CCMR1 &= ~(1U << 12U);
	TIM2->CCMR1 |= (1U << 13U);
	TIM2->CCMR1 |= (1U << 14U);
	/*Enable ch1 and ch2*/
	/*CH1 enable*/
	TIM2->CCER |= (1U << 0U);
	/*CH2 enable*/
	TIM2->CCER |= (1U << 4U);
	
	/*Enable timer*/
	TIM2->CR1 |= CR1_CEN;
}

void tim2_pa0_pa1_pwm_set_dutycycle(uint8_t ch, uint32_t dutycycle)
{
	/*Select channel*/
	switch(ch)
	{
		case 1:
		{
			TIM2->CCR1 = dutycycle;
			break;
		}
		case 2:
		{
			TIM2->CCR2 = dutycycle;
			break;
		}
		default:
		{
			break;
		}
	}
	/*Set dudtycycle*/
}

void pseudo_dly(void)
{
	for(int i = 0;1 < 17000;i++);
}

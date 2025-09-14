/*
 * adc_dma.c
 *
 *  Created on: 07-Sep-2025
 *      Author: vigneshv
 */

#include "adc_dma.h"
#include "stm32_f429xx.h"
#include "dma.h"
#include "cm4.h"

#define ADC1EN 		(1U << 8U)
#define CR1_SCAN  	(1U << 8U)
#define CR2_DMA  	(1U << 8U)
#define CR2_DDS  	(1U << 9U)
#define CR2_CONT  	(1U << 1U)
#define CR2_ADCON   (1U << 0U)
#define CR2_SWSTART (1U << 30U)

#define TIM2EN      (1U << 0U)
#define CR1_CEN      (1U << 0U)

uint16_t adc_raw_data[10] = {0};

void adc_tim_data_init(void)
{
	/*GPIO cfg*/
	/*Enable clock access to ADC GPIO pins port*/
	RCC->AHB1ENR |= (1U << 0U);//PORTA
	/*Set PA0 mode to analog mode*/
	GPIOA->MODER |= (3U << 0U);

	/*ADC cfg*/
	/*Enable clock access to ADC*/
	RCC->APB2ENR |= ADC1EN;

	/*Let default sequence length*/

	/*Let default sequence*/

	/*No need of scan mode*/

	/*Select to use DMA*/
	/*No need of continuous mode*/
	ADC1->CR2 |= CR2_DMA | CR2_DDS;

	/*Select external trigger on rising edge*/
	ADC1->CR2 |= (1U << 28U);
	ADC1->CR2 &= ~(1U << 29U);

	/*Select timer2 TRGO event*/
	ADC1->CR2 &= ~(1U << 24U);
	ADC1->CR2 |= (1U << 25U);
	ADC1->CR2 |= (1U << 26U);
	ADC1->CR2 &= ~(1U << 27U);

	/*DAM cfg*/
	/*Enable clock access to DMA*/
	RCC->AHB1ENR |= DMA2EN;
	/*Disable DMA stream*/
	DMA2_Stream0->CR &= ~DMA2_STREAM_EN;

	/*Wait till DMA is disabled*/
	while(DMA2_Stream0->CR & DMA2_STREAM_EN);
	/*Enable circular mode*/
	DMA2_Stream0->CR |= DMA_SCR_CIRC;
	/*Set memory transfer size to half-word*/
	DMA2_Stream0->CR |= (1U << 13U);
	DMA2_Stream0->CR &= ~(1U << 14U);
	/*Set peripheral transfer size to half-word*/
	DMA2_Stream0->CR |= (1U << 11U);
	DMA2_Stream0->CR &= ~(1U << 12U);
	/*Enable memory increment*/
	DMA2_Stream0->CR |= (1U << 10U);
	/*enable transfer complete interrupt*/
	DMA2_Stream0->CR |= TCIEN;
	/*Set peripheral address*/
	DMA2_Stream0->PAR = (uint32_t)(&(ADC1->DR));
	/*Set memory address*/
	DMA2_Stream0->M0AR = (uint32_t)adc_raw_data;
	/*Set number of transfer*/
	DMA2_Stream0->NDTR = (uint16_t)10;

	//enable DMA interrupt in NVIC
	NVIC_EnableIRQ(DMA2_Stream0_IRQn);

	/*TIM2 Cfg*/
	/*configure timer period to 100Hz i.e, ADC is
	 * going to be sampling at a 100Hz rate, every 10ms*/
	/*Enable clock to the TIM2*/
	RCC->APB1ENR |= TIM2EN;

	/*Set TIM prescaler value*/
	TIM2->PSC = 16000 - 1; //16000000/16000 = 1000

	/*Set TIM auto-reload value*/
	TIM2->ARR = 10 - 1; //1000/10 = 100Hz

	/*configure master mode selection bits*/
	TIM2->CR2 &= ~(1U << 4U);
	TIM2->CR2 |= (1U << 5U);
	TIM2->CR2 &= ~(1U << 6U);

	/*ADC cfg*/
	/*Enable ADC*/
	ADC1->CR2 |= CR2_ADCON;
	/*No need of SW Start ADC*/
	//ADC1->CR2 |= CR2_SWSTART;

	/*Enable DMA stream*/
	DMA2_Stream0->CR |= DMA2_STREAM_EN;

	/*Enable Timer*/
	TIM2->CR1 |= CR1_CEN;
}

void adc_init(void)
{
	/*GPIO cfg*/
	/*Enable clock access to ADC GPIO pins port*/
	RCC->AHB1ENR |= (1U << 0U);//PORTA
	/*Set PA0 and PA1 mode to analog mode*/
	GPIOA->MODER |= ((3U << 0U) | (3U << 2U));

	/*ADC cfg*/
	/*Enable clock access to ADC*/
	RCC->APB2ENR |= ADC1EN;
	/*Set sequence length*/
	ADC1->SQR1 |= (1U << 20U);
	ADC1->SQR1 &= ~(1U << 21U);
	ADC1->SQR1 &= ~(1U << 22U);
	ADC1->SQR1 &= ~(1U << 23U);
	/*Set sequence*/
	ADC1->SQR3 = (0U << 0U) | (1U << 5U);
	/*Enable scan mode*/
	ADC1->CR1 = CR1_SCAN;
	/*Select to use DMA*/
	ADC1->CR2 |= CR2_DMA | CR2_DDS | CR2_CONT;
	/*DAM cfg*/
	/*Enable clock access to DMA*/
	RCC->AHB1ENR |= DMA2EN;
	/*Disable DMA stream*/
	DMA2_Stream0->CR &= ~DMA2_STREAM_EN;

	/*Wait till DMA is disabled*/
	while(DMA2_Stream0->CR & DMA2_STREAM_EN);
	/*Enable circular mode*/
	DMA2_Stream0->CR |= DMA_SCR_CIRC;
	/*Set memory transfer size to half-word*/
	DMA2_Stream0->CR |= (1U << 13U);
	DMA2_Stream0->CR &= ~(1U << 14U);
	/*Set peripheral transfer size to half-word*/
	DMA2_Stream0->CR |= (1U << 11U);
	DMA2_Stream0->CR &= ~(1U << 12U);
	/*Enable memory increment*/
	DMA2_Stream0->CR |= (1U << 10U);
	/*Set peripheral address*/
	DMA2_Stream0->PAR = (uint32_t)(&(ADC1->DR));
	/*Set memory address*/
	DMA2_Stream0->M0AR = (uint32_t)adc_raw_data;
	/*Set number of transfer*/
	DMA2_Stream0->NDTR = (uint16_t)NUM_OF_CHANNELS;
	/*Enable DMA stream*/
	DMA2_Stream0->CR |= DMA2_STREAM_EN;

	/*ADC cfg*/
	/*Enable ADC*/
	ADC1->CR2 |= CR2_ADCON;
	/*Start ADC*/
	ADC1->CR2 |= CR2_SWSTART;
}



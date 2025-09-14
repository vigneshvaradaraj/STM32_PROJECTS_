/*
 * dma.c
 *
 *  Created on: Sep 6, 2025
 *      Author: vigneshv
 */

#include "dma.h"
#include "cm4.h"
#include "stm32_f429xx.h"

uint8_t g_transfer_complete = 0;

//only DMA2 supports M2M transfer
void dma2MemToMemCfg(void)
{
	// DMA2 lies on AHB1 bus
	// enable clock for DMA2
	RCC->AHB1ENR |= DMA2EN;

	//disable DMA stream
	DMA2_Stream0->CR = 0;

	//wait until stream is disabled
	while(DMA2_Stream0->CR & DMA2_STREAM_EN);

	//configure DMA parameters

	/*set memory data size to half word*/
	DMA2_Stream0->CR |= (1 << 13);

	/*set peripheral data size to half word*/
	DMA2_Stream0->CR |= (1 << 11);

	/*enable memory address increment*/
	DMA2_Stream0->CR |= (1 << 10);

	/*enable peripheral address increment*/
	DMA2_Stream0->CR |= (1 << 9);

	/*select data transfer direction as M2M*/
	DMA2_Stream0->CR |= (1 << 7);

	/*enable transfer complete interrupt*/
	DMA2_Stream0->CR |= TCIEN;

	/*enable transfer error interrupt*/
	DMA2_Stream0->CR |= TEIEN;

	//disable direct mode
	DMA2_Stream0->FCR |= (1 << 2);

	//set DMA FIFO threshold
	DMA2_Stream0->FCR |= (3 << 0);

	//enable DMA interrupt in NVIC
	NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

void dma_transfer_start(uint8_t* src_buff, uint8_t* dest_buff, int length)
{
	/*set peripheral address*/
	DMA2_Stream0->PAR = (uint32_t)src_buff;

	/*set memory address*/
	DMA2_Stream0->M0AR = (uint32_t)dest_buff;

	/*set transfer length*/
	DMA2_Stream0->NDTR = length;

	/*Enable DMA stream*/
	DMA2_Stream0->CR |= DMA2_STREAM_EN;
}

void DMA2_Stream0_IRQHandler(void)
{
	/*check if transfer complete occured*/
	if((DMA2->LISR) & LISR_TCIF0)
	{
		g_transfer_complete = 1;
		/*clear flag*/
		DMA2->LIFCR |= LIFCR_CTCIF0;
	}
	if((DMA2->LISR) & LISR_TEIF0)
	{
		/*clear flag*/
		DMA2->LIFCR |= LIFCR_CTEIF0;
	}
}



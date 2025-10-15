/*
 * spi_dma.c
 *
 *  Created on: 16-Sep-2025
 *      Author: vigneshv
 */
 
 #include "spi_dma.h"
 #include "stm32_f429xx.h"

 #include "cm4.h"
 
 /*
	PA5 : SPI_CLK
	PA6 : SPI_MISO
	PA7 : SPI_MOSI
 */
 
 #define GPIOAEN  (1U << 0U)
 #define SPI1EN  (1U << 12U)
 #define CR1_SSM  (1U << 9U)
 #define CR1_SSI  (1U << 8U)
 #define CR1_MSTR (1U << 2U)
 
 #define CR1_CPHA  (1U << 0U)
 #define CR1_CPOL  (1U << 1U)
 
 #define CR2_TXDMAEN (1U << 1U)
 #define CR2_RXDMAEN (1U << 0U)
 
 #define CR1_SPE (1U << 6U)
 
 #define DMA2EN  (1U << 22U)
#define DMA_SCR_EN (1U << 0U)
#define DMA_SCR_MINC (1U << 10U)
#define DMA_SCR_PINC (1U << 9U)
#define DMA_SCR_CIRC (1U << 8U)
#define DMA_SCR_TCIE (1U << 4U)
#define DMA_SCR_TEIE (1U << 2U)
#define DMA_SFCR_DMDIS (1U << 2U)

#define HIFCR_CDMEIF5 (1U << 8U)
#define HIFCR_CTEIF5 (1U << 9U)
#define HIFCR_CTCIF5 (1U << 11U)

#define HIFCR_CDMEIF6 (1U << 18U)
#define HIFCR_CTEIF6 (1U << 19U)
#define HIFCR_CTCIF6 (1U << 21U)

#define HIFSR_TCIF5 (1U << 11U)
#define HIFSR_TCIF6 (1U << 21U)

#define LIFCR_CTCIF2 (1U << 21U)
#define LIFCR_CTCIF3 (1U << 27U)
#define LIFCR_CTEIF2 (1U << 19U)
#define LIFCR_CTEIF3 (1U << 25U)
 
 void spi1_dma_init(void)
 {
	 /*GPIO Cfg*/
	 /*Enable clock to PORT of SPI pins*/
	 RCC->AHB1ENR |= GPIOAEN;
	 /*Set SPI pin modes to alternate function mode*/
	 /*PA5*/
	 GPIOA->MODER &= ~(1U << 10U);
	 GPIOA->MODER |= (1U << 11U);
	 /*PA6*/
	 GPIOA->MODER &= ~(1U << 12U);
	 GPIOA->MODER |= (1U << 13U);
	 /*PA7*/
	 GPIOA->MODER &= ~(1U << 14U);
	 GPIOA->MODER |= (1U << 15U);
	 
	 /*Set alternate type to AF5(SPI1)*/
	 /*PA5*/
	 GPIOA->AFR[0] |= (1U << 20U);
	 GPIOA->AFR[0] &= ~(1U << 21U);
	 GPIOA->AFR[0] |= (1U << 22U);
	 GPIOA->AFR[0] &= ~(1U << 23U);
	 /*PA6*/
	 GPIOA->AFR[0] |= (1U << 24U);
	 GPIOA->AFR[0] &= ~(1U << 25U);
	 GPIOA->AFR[0] |= (1U << 26U);
	 GPIOA->AFR[0] &= ~(1U << 27U);
	 /*PA7*/
	 GPIOA->AFR[0] |= (1U << 28U);
	 GPIOA->AFR[0] &= ~(1U << 29U);
	 GPIOA->AFR[0] |= (1U << 30U);
	 GPIOA->AFR[0] &= ~(1U << 31U);
	 
	 /*SPI cfg*/
	 /*Enable clock access to the SPI1 module*/
	 RCC->APB2ENR |= SPI1EN;
	 /*Set software slave management */
	 SPI1->CR1 |= CR1_SSM | CR1_SSI;
	 /*Set SPI to master mode*/
	 SPI1->CR1 |= CR1_MSTR;
	 /*Set CPHA and CPOL*/
	 SPI1->CR1 |= CR1_CPOL | CR1_CPHA;/*TODO:*/
	 /*Set clock devider : PCLK / 4*/
	 SPI1->CR1 |= (1U << 3U) ;
	 SPI1->CR1 &= ~(1U << 4U);
	 SPI1->CR1 &= ~(1U << 5U);
	 /*Select to use DMA*/
	 SPI1->CR1 |= CR2_TXDMAEN | CR2_RXDMAEN;
	 /*Enable SPI*/
	 SPI1->CR1 |= CR1_SPE;
 }
 
 void dma2_stream3_spi_tx_init(void)
 {
	 /*DMA Cfg*/
	 /*Enable clk acess to DMA2*/
	 RCC->AHB1ENR |= DMA2EN;
	 /*Disable DMA stream*/
	 DMA2_Stream3->CR = 0;
	 /*wat till DMA is disabled*/
	 while(DMA2_Stream3->CR & DMA_SCR_EN);
	 /*DMA Stream Cfg*/
	 /*Enable mem add inc*/
	 DMA2_Stream3->CR |= DMA_SCR_MINC;
	 /*Set transfer direction : Mem to Perph*/
	 DMA2_Stream3->CR |= (1U << 6U);
	 DMA2_Stream3->CR &= ~(1U << 7U);
	 /*Enable Transfer complete interrupt*/
	 DMA2_Stream3->CR |= DMA_SCR_TCIE;
	 /*Enable Transfer error interrupt*/
	 DMA2_Stream3->CR |= DMA_SCR_TEIE;
	 /*Disable direct mode*/
	 DMA2_Stream3->FCR |= DMA_SFCR_DMDIS;
	 /*Set DMA FIFO threshold*/
	 DMA2_Stream3->FCR |= (3U << 0U);
	 /*Enable DMA interrupt in NVIC*/
	 NVIC_EnableIRQ(DMA2_Stream3_IRQn);
 }
 
 void dma2_stream2_spi_rx_init(void)
 {
	/*DAM cfg*/
	/*Enable clock access to DMA*/
	RCC->AHB1ENR |= DMA2EN;
	/*Disable DMA stream*/
	DMA2_Stream2->CR &= ~DMA_SCR_EN;

	/*Wait till DMA is disabled*/
	while(DMA2_Stream2->CR & DMA_SCR_EN);
	
	 /*DMA Stream Cfg*/
	 /*Enable mem add inc*/
	 DMA2_Stream2->CR |= DMA_SCR_MINC;
	 /*Set transfer direction : Periph to Memory*/
	 DMA2_Stream2->CR &= ~(1U << 6U);
	 DMA2_Stream2->CR &= ~(1U << 7U);
	 /*Enable Transfer complete interrupt*/
	 DMA2_Stream2->CR |= DMA_SCR_TCIE;
	 /*Enable Transfer error interrupt*/
	 DMA2_Stream2->CR |= DMA_SCR_TEIE;
	 /*Disable direct mode*/
	 DMA2_Stream2->FCR |= DMA_SFCR_DMDIS;
	 /*Set DMA FIFO threshold : Full threshold*/
	 DMA2_Stream2->FCR |= (3U << 0U);
	 /*Enable DMA interrupt in NVIC*/
	 NVIC_EnableIRQ(DMA2_Stream2_IRQn);
 }
	  
 void dma2_stream3_spi_transfer(uint32_t msg_to_send, uint32_t msg_len)
 {
	 /*Clear interrupt flags*/
	 DMA2->LIFCR |= LIFCR_CTCIF3 | LIFCR_CTEIF3;
	 /*Set peripheral address*/
	 DMA2_Stream3->PAR = (uint32_t)(&(SPI1->DR));
	 /*Set memory address*/
	 DMA2_Stream3->M0AR = msg_to_send;
	 /*Set transfer lemgth*/
	 DMA2_Stream3->NDTR = msg_len;
	 /*Enable the DMA Stream*/
	 DMA2_Stream3->CR |= DMA_SCR_EN;
 }
 void dma2_stream2_spi_recieve(uint32_t received_msg, uint32_t msg_len)
 {
	 /*Clear interrupt flags*/
	 DMA2->LIFCR |= LIFCR_CTCIF2 | LIFCR_CTEIF2;
	 /*Set peripheral address*/
	 DMA2_Stream2->PAR = (uint32_t)(&(SPI1->DR));
	 /*Set memory address*/
	 DMA2_Stream2->M0AR = received_msg;
	 /*Set transfer lemgth*/
	 DMA2_Stream2->NDTR = msg_len;
	 /*Enable the DMA Stream*/
	 DMA2_Stream2->CR |= DMA_SCR_EN;
 }


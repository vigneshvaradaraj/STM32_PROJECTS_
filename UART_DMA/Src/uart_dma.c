/*
 * uart_dma.c
 *
 *  Created on: 14-Sep-2025
 *      Author: vigneshv
 */

#include "stm32_f429xx.h"

#define UART2EN  (1U << 17U)
#define GPIOAEN  (1U << 0U)

#define DMA1EN   (1U << 21U)
#define DMA1_SCR_EN (1U << 0U)
#define DMA1_SCR_MINC (1U << 10U)
#define DMA1_SCR_PINC (1U << 9U)
#define DMA1_SCR_CIRC (1U << 8U)
#define DMA1_SCR_TCIE (1U << 4U)
#define DMA1_SCR_TEIE (1U << 2U)
#define DMA1_SFCR_DMDIS (1U << 2U)

#define HIFCR_CDMEIF5 (1U << 8U)
#define HIFCR_CTEIF5 (1U << 9U)
#define HIFCR_CTCIF5 (1U << 11U)

#define HIFCR_CDMEIF6 (1U << 18U)
#define HIFCR_CTEIF6 (1U << 19U)
#define HIFCR_CTCIF6 (1U << 21U)

#define CR1_TE  (1U << 3U)
#define CR1_RE  (1U << 2U)
#define CR1_UE  (1U << 13U)
#define SR_TXE  (1U << 7U)
#define SR_TC   (1U << 6U)
#define CR1_TCIE (1U << 6U)

#define CR3_DMAT (1U << 7U)
#define CR3_DMAR (1U << 6U)

#define UART_BAUDRATE (115200)
#define CLK           (16000000)

/*To find the stream and channel of particular peripheral refer table 27*/


char uart_data_buffer[UART_DATA_BUFF_SIZE];

void uart2_rx_tx_init(void)
{
	/*Cfg UART GPIO Pin*/
	/*1.Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	/*2.Set PA2 mode to alternate function mode*/
	GPIOA->MODER &= ~(1U << 4U);
	GPIOA->MODER |= (1U << 5U);

	/*3.Set PA3 mode to alternate function mode*/
	GPIOA->MODER &= ~(1U << 6U);
	GPIOA->MODER |= (1U << 7U);

	/*4.Set PA2 alternate function type to AF7 (UART)*/
	GPIOA->AFR[0] |= (1U << 8U);
	GPIOA->AFR[0] |= (1U << 9U);
	GPIOA->AFR[0] |= (1U << 10U);
	GPIOA->AFR[0] &= ~(1U << 11U);

	/*5.Set PA2 alternate function type to AF7 (UART)*/
	GPIOA->AFR[0] |= (1U << 12U);
	GPIOA->AFR[0] |= (1U << 13U);
	GPIOA->AFR[0] |= (1U << 14U);
	GPIOA->AFR[0] &= ~(1U << 15U);

	/*Cfg UART module*/
	/*6.Enable cock acess to UART module*/
	RCC->APB1ENR |= UART2EN;

	/*7.Set baud rate*/
	uart_set_baudrate(CLK,UART_BAUDRATE);

	/*8.Select to use DMA for RX and TX*/
	USART2->CR3 |= CR3_DMAT | CR3_DMAR;

	/*9.Set transfer direction*/
	USART2->CR1 |= CR1_TE | CR1_RE;

	/*10.Clear TC flag*/
	USART2->SR &= ~SR_TC;

	/*11.Enable TCIE*/
	USART2->CR1 |= CR1_TCIE;

	/*12.Enable UART module*/
	USART2->CR1 |= CR1_UE;
}

void dma1_init(void)
{
	/*Enable clock access to DMA*/
	RCC->AHB1ENR |= DMA1EN;

	/*enable DMA Stream6 interrupt in NVIC*/
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}

void dma1_stream5_uart_rx_config(void)
{
	/*Disable DMA stream*/
	DMA1_Stream5->CR &= ~DMA2_SCR_EN;

	/*Wait till DMA is disabled*/
	while(DMA1_Stream5->CR & DMA2_SCR_EN);

	/*Clear interrupt flags for stream 5*/
	DMA1->HIFCR |= HIFCR_CDMEIF5 | HIFCR_CTEIF5 | HIFCR_CTCIF5;

	/*Set peripheral address*/
	DMA1_Stream5->PAR = (uint32_t)(&(USART2->DR));
	/*Set memory address*/
	DMA1_Stream5->M0AR = (uint32_t)uart_data_buffer;
	/*Set number of transfer*/
	DMA1_Stream5->NDTR = (uint16_t)UART_DATA_BUF_SIZE;
	/*Select channel 4*/
	DMA1_Stream5->CR &= ~(1U << 25U);
	DMA1_Stream5->CR &= ~(1U << 26U);
	DMA1_Stream5->CR |= (1U << 27U);
	/*Enable memory address increment*/
	DMA1_Stream5->CR |= DMA1_SCR_MINC;
	/*Enable transfer complete interrupt*/
	DMA1_Stream5->CR |= DMA_SCR_TCIE;
	/*Enable circular mode*/
	DMA1_Stream5->CR |= DMA_SCR_CIRC;
	/*Set transfer direction*/
	/*Peripheral to Memory*/
	DMA1_Stream5->CR &= ~(1U << 6U);
	DMA1_Stream5->CR &= ~(1U << 7U);
	/*Enable DMA stream*/
	DMA1_Stream5->CR |= DMA2_SCR_EN;
	/*enable DMA Stream5 interrupt in NVIC*/
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

void dma1_stream6_uart_tx_config(uint32_t msg_to_send, uint32_t msg_len)
{
	/*Disable DMA stream*/
	DMA1_Strea6->CR &= ~DMA2_SCR_EN;

	/*Wait till DMA is disabled*/
	while(DMA1_Stream6->CR & DMA2_SCR_EN);

	/*Clear interrupt flags for stream 6*/
	DMA1->HIFCR |= HIFCR_CDMEIF6 | HIFCR_CTEIF6 | HIFCR_CTCIF6;

	/*Set peripheral address*/
	DMA1_Stream6->PAR = (uint32_t)(&(USART2->DR));
	/*Set memory address*/
	DMA1_Stream6->M0AR = (uint32_t)msg_to_send;
	/*Set number of transfer*/
	DMA1_Stream6->NDTR = (uint16_t)msg_len;
	/*Select channel 4*/
	DMA1_Stream6->CR &= ~(1U << 25U);
	DMA1_Stream6->CR &= ~(1U << 26U);
	DMA1_Stream6->CR |= (1U << 27U);

	/*Enable memory address increment*/
	DMA1_Stream6->CR |= DMA1_SCR_MINC;

	/*Set transfer direction*/
	/*Memory to peripheral*/
	DMA1_Stream6->CR |= (1U << 6U);
	DMA1_Stream6->CR &= ~(1U << 7U);

	/*Set transfer complete interrupt*/
	DMA1_Stream6->CR |= DMA_SCR_TCIE;

	/*Enable DMA stream*/
	DMA1_Stream6->CR |= DMA2_SCR_EN;
}

static uint16_t compute_uart_bd(uint32_t peripheral_clk, uint32_t baudrate)
{
	return ((peripheral_clk + (baudrate/2U))/baudrate);
}

static void uart_set_baudrate(uint32_t peripheral_clk, uint32_t baudrate)
{
	USART2->BRR = compute_uart_bd(peripheral_clk,baudrate);
}

void DMA1_Stream6_IRQHandler(void)
{

}
void DMA1_Stream6_IRQHandler(void)
{

}

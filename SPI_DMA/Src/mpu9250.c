/*
 * mpu9250.c
 *
 *  Created on: 13-Oct-2025
 *      Author: vigneshv
 */

#include "mpu9250.h"
#include "stm32_f429xx.h"

#define USER_CTRL_I2C_IF_DIS (1U << 4U)
#define SPI_DATA_BUFF_LEN 2

double g_accel_range;

uint8_t spi_data_buff[SPI_DATA_BUFF_LEN];
uint8_t g_tx_cmplt;
uint8_t g_rx_cmplt;

void mpu925_accel_config(uint8_t mode)
{
	switch(mode)
	{
		case ACC_FULL_SCALE_2_G:
		{
			g_accel_range = 2.0;
			break;
		}
		case ACC_FULL_SCALE_4_G:
		{
			g_accel_range = 4.0;
			break;
		}
		case ACC_FULL_SCALE_8_G:
		{
			g_accel_range = 8.0;
			break;
		}
		case ACC_FULL_SCALE_16_G:
		{
			g_accel_range = 16.0;
			break;
		}
		default:
			break;
	}

	/*Set to SPI mode only*/
	spi_data_buff[0] = MPU9250_ADDR_USER_CTRL;
	spi_data_buff[1] = USER_CTRL_I2C_IF_DIS;

	dma2_stream3_spi_transfer((uint32_t)spi_data_buff,(uint32_t)SPI_DATA_BUFF_LEN);

	/*wait for transfer completion*/
	while(!g_tx_cmplt);

	/*Reset flag*/
	g_tx_cmplt = 0;

	/*Configure the ACCEL Range*/
	spi_data_buff[0] = MPU9250_ADDR_ACCELCONGF;
	spi_data_buff[1] = mode;

	dma2_stream3_spi_transfer((uint32_t)spi_data_buff,(uint32_t)SPI_DATA_BUFF_LEN);

	/*wait for transfer completion*/
	while(!g_tx_cmplt);

	/*Reset flag*/
	g_tx_cmplt = 0;
}

void DMA2_Stream3_IRQHandler(void)
{
	if((DMA2->LISR) & LISR_TCIF3)
	{
		//do something...
		g_tx_cmplt = 1;

		//clear the flag
		DMA2->LIFCR |= LIFCR_CTCIF3;
	}
	else if((DMA2->LISR) & LISR_TEIF3)
	{
		//do something...

		//Clear the flag
		DMA2->LIFCR |= LIFCR_CTEIF3;
	}
}

void DMA2_Stream2_IRQHandler(void)
{
	if((DMA2->LISR) & LISR_TCIF2)
	{
		//do something...
		g_rx_cmplt = 1;

		//clear the flag
		DMA2->LIFCR |= LIFCR_CTCIF2;
	}
	else if((DMA2->LISR) & LISR_TEIF2)
	{
		//do something...

		//Clear the flag
		DMA2->LIFCR |= LIFCR_CTEIF2;
	}
}

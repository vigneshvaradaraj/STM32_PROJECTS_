/*
 * simple_spi.h
 *
 *  Created on: 14-Jul-2025
 *      Author: vigneshv
 */

#ifndef SIMPLE_SPI_H_
#define SIMPLE_SPI_H_

extern void spi_peripheral_clock_init(void);
extern void spi_gpio_init(void);
extern void spi_init(void);
extern void spi_send(uint8_t data);
extern uint8_t spi_recieve(void);
extern void spi_interrupt_config(void);
extern void spi_send_n_bytes(const uint8_t* data, uint16_t size);
extern uint32_t HAL_GetTick(void);


#endif /* SIMPLE_SPI_H_ */

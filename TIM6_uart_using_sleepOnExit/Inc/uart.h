/*
 * uart.h
 *
 *  Created on: 28-Apr-2025
 *      Author: vigneshv
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void uart_gpio_int(void);
void uart_init(void);
void uart_send_byte(uint8_t);
void uart_send_string(char*);

#endif /* UART_H_ */

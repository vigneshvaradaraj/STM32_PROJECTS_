/*
 * led.h
 *
 *  Created on: 23-Sep-2025
 *      Author: vigneshv
 */

#ifndef LED_H_
#define LED_H_

#include "stm32_f429xx.h"
#include <stdint.h>

void led_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);


#endif /* LED_H_ */

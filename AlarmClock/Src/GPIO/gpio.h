/*
 * gpio.h
 *
 *  Created on: Aug 30, 2025
 *      Author: vigneshv
 */

#ifndef GPIO_GPIO_H_
#define GPIO_GPIO_H_
#include <stdint.h>

typedef enum
{
	BUTTON_PRESSED,
	BUTTON_RELEASED
}buttonPressState_t;

void init_gpio(void);
uint8_t readButton(void);


#endif /* GPIO_GPIO_H_ */

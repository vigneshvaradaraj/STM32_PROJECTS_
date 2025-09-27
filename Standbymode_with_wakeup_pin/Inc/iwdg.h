/*
 * iwdg.h
 *
 *  Created on: 23-Sep-2025
 *      Author: vigneshv
 */

#ifndef IWDG_H_
#define IWDG_H_
#include "stm32_f429xx.h"
#include <stdint.h>

#define IWDG_KEY_RELOAD				0x0000AAAAU

void iwdg_init(void);

#endif /* IWDG_H_ */

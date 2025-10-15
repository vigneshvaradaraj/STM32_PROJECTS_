/*
 * tim.h
 *
 *  Created on: 09-Sep-2025
 *      Author: vigneshv
 */

#ifndef TIM_H_
#define TIM_H_

#include <stdint.h>

void tim2_pa0_pa1_pwm_init(void);
void tim2_pa0_pa1_pwm_set_dutycycle(uint8_t ch, uint32_t dutycycle);
void pseudo_dly(void);

#endif /* TIM_H_ */

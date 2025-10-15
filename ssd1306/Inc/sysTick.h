/*
 * sysTick.h
 *
 *  Created on: Aug 9, 2025
 *      Author: vigneshv
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_


typedef struct
{
	volatile uint32_t CTRL;
	volatile uint32_t RLV;
	volatile uint32_t CV;
	volatile uint32_t CALIB;
}systick_t;
#define  SYST_CSR  0xE000E010

#define SYS_TICK ((systick_t*)SYST_CSR)


#endif /* SYSTICK_H_ */

/*
 * wwdg.c
 *
 *  Created on: 25-Sep-2025
 *      Author: vigneshv
 */

#include "stm32_f429xx.h"

void wwdg_init(void)
{
	/*Enable the peripheral clock WWDG*/
	RCC->APB1ENR |= RCC_APB1ENR_WWDGEN;

	/*Set prescaler to have a rollover each about ~2s*/
	WWDG->CFR |= (1U << 7U);
	WWDG->CFR |= (1U << 8U);

	/*Set windows value to same value (~2s) as down counter
	 * in order to be able to refresh the WWDG almost immediately*/
	WWDG->CFR &= ~ WWDG_CFR_W;
	WWDG->CFR |= 0x7E;

	/*Set counter*/
	WWDG->CR &= ~WWDG_CR_T;
	WWDG->CR |= 0x7E;
	/*Enable WWDG*/
	WWDG->CR |= WWDG_CR_WDGA;
}



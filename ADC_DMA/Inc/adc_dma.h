/*
 * adc_dma.h
 *
 *  Created on: 07-Sep-2025
 *      Author: vigneshv
 */

#ifndef ADC_DMA_H_
#define ADC_DMA_H_

typedef unsigned short int uint16_t;

#define NUM_OF_CHANNELS 2
extern uint16_t adc_raw_data[NUM_OF_CHANNELS];
void adc_init(void);



#endif /* ADC_DMA_H_ */

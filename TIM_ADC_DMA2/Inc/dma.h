/*
 * dma.h
 *
 *  Created on: Sep 6, 2025
 *      Author: vigneshv
 */

#ifndef DMA_H_
#define DMA_H_

#define DMA2EN  (1U << 22U)
#define DMA2_STREAM_EN (1U << 0U)

#define LISR_TCIF0 (1U << 5U)
#define LIFCR_CTCIF0 (1U << 5U)

#define LISR_TEIF0 (1U << 3U)
#define LIFCR_CTEIF0 (1U << 3U)

#define TCIEN (1U << 4U)
#define TEIEN (1U << 4U)

#define DMA_SCR_CIRC (1U << 8U)

typedef unsigned char uint8_t;
//typedef unsigned int uint32_t;

void dma2MemToMemCfg(void);
void dma_transfer_start(uint8_t* src_buff, uint8_t* dest_buff, int length);

#endif /* DMA_H_ */

/*
 * spi_dma.h
 *
 *  Created on: 16-Sep-2025
 *      Author: vigneshv
 */

#ifndef SPI_DMA_H_
#define SPI_DMA_H_

#include <stdint.h>

#define LISR_TCIF3  (1U << 27U)
#define LISR_TCIF2  (1U << 21U)

#define LISR_TEIF3  	(1U << 25U)
#define LISR_TEIF2  	(1U << 19U)

#define LIFCR_CTCIF2 	(1U << 21U)
#define LIFCR_CTCIF3 	(1U << 27U)

#define LIFCR_CTEIF2 	(1U << 19U)
#define LIFCR_CTEIF3 	(1U << 25U)

 void spi1_dma_init(void);
 void dma2_stream3_spi_tx_init(void);
 void dma2_stream2_spi_rx_init(void);
 void dma2_stream3_spi_transfer(uint32_t msg_to_send, uint32_t msg_len);
 void dma2_stream2_spi_recieve(uint32_t received_msg, uint32_t msg_len);
#endif /* SPI_DMA_H_ */

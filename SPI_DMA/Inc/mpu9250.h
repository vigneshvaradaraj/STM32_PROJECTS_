/*
 * mpu9250.h
 *
 *  Created on: 13-Oct-2025
 *      Author: vigneshv
 */

#ifndef MPU9250_H_
#define MPU9250_H_

#include <stdint.h>
#include "spi_dma.h"

#define ACC_FULL_SCALE_2_G 		0x00
#define ACC_FULL_SCALE_4_G 		0x08
#define ACC_FULL_SCALE_8_G 		0x10
#define ACC_FULL_SCALE_16_G 	0x18

#define MPU9250_ADDR_ACCELCONGF 0x1C
#define MPU9250_ADDR_USER_CTRL 	0x6A




#endif /* MPU9250_H_ */

/*
 * rtc.h
 *
 *  Created on: Sep 20, 2025
 *      Author: vigneshv
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>
#include "stm32_f429xx.h"

#define CR_ALRAIE (1U << 12U)
#define ISR_ALRAF (1U << 8U)

#define ISR_INIT (1U << 7U)

#define CR_TSIE (1U << 15U)
#define CR_TSE   (1U << 11U)
#define ISR_TSF  (1U << 11U)

void rtc_init(void);
void rtc_alarm_init(void);
void rtc_timestamp_init(void);
void rtc_tamper_detect_init(void);
uint8_t rtc_convert_bcd2bin(uint8_t value);
uint32_t rtc_date_get_day(void);
uint32_t rtc_date_get_year(void);
uint32_t rtc_date_get_month(void);
uint32_t rtc_time_get_second(void);
uint32_t rtc_time_get_minute(void);
uint32_t rtc_time_get_hour(void);
#endif /* RTC_H_ */

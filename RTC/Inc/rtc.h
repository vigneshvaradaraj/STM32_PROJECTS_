/*
 * rtc.h
 *
 *  Created on: Sep 20, 2025
 *      Author: vigneshv
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>


void rtc_init(void);
uint8_t rtc_convert_bcd2bin(uint8_t value);
uint32_t rtc_date_get_day(void);
uint32_t rtc_date_get_year(void);
uint32_t rtc_date_get_month(void);
uint32_t rtc_time_get_second(void);
uint32_t rtc_time_get_minute(void);
uint32_t rtc_time_get_hour(void);
#endif /* RTC_H_ */

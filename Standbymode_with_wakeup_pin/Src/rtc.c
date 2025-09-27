/*
 * rtc.c
 *
 *  Created on: Sep 20, 2025
 *      Author: vigneshv
 */

#include "rtc.h"
#include "stm32_f429xx.h"
#include "cm4.h"

#define GPIOCEN (1U << 2U)
#define PWREN (1U << 28U)
#define CR_DBP (1U << 8U)
#define CSR_LSION (1U << 0U)
#define CSR_LSRDY (1U << 1U)
#define BDCR_BDRST (1U << 16U)
#define BDCR_RTCEN (1U << 15U)

#define ISR_INITF (1U << 6U)

#define RTC_WRITE_PROTECTION_ENABLE_1 ((uint8_t)0xCAU)
#define RTC_WRITE_PROTECTION_ENABLE_2 ((uint8_t)0x53U)
#define RTC_INIT_MASK 0xFFFFFFFF
#define TIME_FORMATE_AM 	0x00000000U

#define CR_FMT   (1U << 6U)
#define ISR_RSF  (1U << 5U)

#define RTC_ASYNCH_PREDIV ((uint32_t)0x7F)
#define RTC_SYNCH_PREDIV ((uint32_t)0xF9)

#define CR_ALRAE (1U << 8U)
#define CR_TSEDGE (1U << 3U)






#define READ_REG(REG)  ((REG))
#define WRITE_REG(REG, VAL)  				((REG) = (VAL))
#define READ_BIT(REG, BIT) 					((REG) & BIT)
#define MODIFY_REG(REG, CLEARMASK, SETMASK) WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

static uint8_t rtc_init_seq(void);
static void rtc_time_config(uint32_t Fomate12_24,uint32_t Hours,uint32_t Minutes,uint32_t Seconds);
static void rtc_date_config(uint32_t WeekDay,uint32_t Day,uint32_t Month,uint32_t Year);
static void rtc_set_synch_prescaler(uint32_t synchPrescaler);
static void rtc_set_asynch_prescaler(uint32_t AsynchPrescaler);
static uint8_t exit_init_seq(void);
static void rtc_alarm_config_time(uint32_t Fomate12_24,uint32_t Hours,uint32_t Minutes,uint32_t Seconds);
static void rtc_alarm_set_mask(uint32_t Mask);

void rtc_tamper_detect_init(void)
{
	/*Enable clock access to PWR*/
	RCC->APB1ENR |= PWREN;

	/*Enable backup access to Cfg RTC*/
	PWR->CR |= CR_DBP;

	/*Enable the RTC*/
	RCC->BDCR |= BDCR_RTCEN;

	/*Set Tamper trigger to falling edge*/
	RTC->TAFCR |= RTC_TAFCR_TAMP1TRG;

	/*Enable tamper detection*/
	RTC->TAFCR |= RTC_TAFCR_TAMP1E;

	/*Enable tamper interrupt*/
	RTC->TAFCR |= RTC_TAFCR_TAMPIE;

	/*Enable tamper interrupt in NVIC*/
	NVIC_EnableIRQ(TAMP_STAMP_IRQn);

	/*Cfg RTC tamper EXTI interrupt*/
	EXTI->IMR |= EXTI_IMR_IM21;

	/*Enable Rising edge*/
	EXTI->RTSR |= EXTI_IMR_IM21;

	/*Clear Tamper Interrupt pending bit*/
	RTC->ISR |= ~RTC_ISR_TAMP1F;
}

void rtc_timestamp_init(void)
{
	/*Disable RTC register write protection*/
	RTC->WPR = RTC_WRITE_PROTECTION_ENABLE_1;
	RTC->WPR = RTC_WRITE_PROTECTION_ENABLE_2;

	/*Set RTC timestamp generation to rising edge*/
	RTC->CR &= ~CR_TSEDGE;

	/*Timestamp pin is connected to PC13 which is RTC_AF1 (it is not alternate function)*/

	/*Enable Timestamp*/
	RTC->CR |= CR_TSE;

	/*Enable Timestamp Interrupt*/
	RTC->CR |= CR_TSIE;

	/*Cfg RTC timestamp EXTI interrupt*/
	EXTI->IMR |= (1U << 21U);

	/*Enable rising edge*/
	EXTI->RTSR |= (1U << 21U);

	/*Enable alarm interrupt in NVIC*/
	NVIC_EnableIRQ(TAMP_STAMP_IRQn);

	/*Enter intialization mode*/
	if(rtc_init_seq() != 1)
	{
		/*TODO:*/
	}
	/*Set desired date: Fri Dec 29th 2016*/
	rtc_date_config(0x5,0x29,0x12,0x16);

	/*Set desired time: 11:59:55 PM*/
	rtc_time_config(1,0x11,0x59,0x55);

	/*Exit the initialization mode*/
	if(exit_init_seq() != 1)
	{
		//TODO:
	}

	/*Enable RTC registers write protection*/
	RTC->WPR = 0xFF;

	/*Set PC13 as input pin*/
	RCC->AHB1ENR |= GPIOCEN;//enable clk
	GPIOC->MODER &= ~(3 << 26);//as input pin

	/*These pin need to check for differnet hardware*/
}
void rtc_alarm_init(void)
{
	/*Disable RTC register write protection*/
	RTC->WPR = RTC_WRITE_PROTECTION_ENABLE_1;
	RTC->WPR = RTC_WRITE_PROTECTION_ENABLE_2;
	/*Enter the initialization mode*/
	if(rtc_init_seq() != 1)
	{
		/*TODO:*/
	}
	/*Set desired date: Fri Dec 29th 2016*/
	rtc_date_config(0x5,0x29,0x12,0x16);
	/*Set desired time: 11:59:55 PM*/
	rtc_time_config(1,0x11,0x59,0x55);
	/*Set Alarm : 12: 00: 09*/
	rtc_alarm_config_time(TIME_FORMATE_AM,0x12,0x00,0x01);
	/*Ignore weekday*/
	rtc_alarm_set_mask(RTC_ALRMAR_MSK4);

	/*Enable Alarm*/
	RTC->CR |= CR_ALRAE;

	/*Clear Alarm interrupt pending bit*/
	RTC->ISR &= ~ISR_ALRAF;

	/*Enable Alarm interrupt*/
	RTC->CR |= CR_ALRAIE;
	/*Cfg RTC Alarm interrupt ref : pgNo: 386*/
	EXTI->IMR |= (1U << 17U);

	/*Enable rising trigger selection*/
	EXTI->RTSR |= (1U << 17U);

	/*Enable alarm interrupt in NVIC*/
	NVIC_EnableIRQ(RTC_Alarm_IRQn);

	/*Exit the initialization mode*/
	if(exit_init_seq() != 1)
	{
		//TODO:
	}
	/*Enable RTC registers write protection*/
	RTC->WPR = 0xFF;
}

void rtc_init(void)
{
	/*Enable clock access to PWR*/
	RCC->APB1ENR |= PWREN;

	/*Enable backup access to Cfg RTC*/
	PWR->CR |= CR_DBP;
	/*Enable Low Speed Internal (LSI)*/
	RCC->CSR |= CSR_LSION;
	/*Wait for LSI to be ready*/
	while((RCC->CSR & CSR_LSRDY) != CSR_LSRDY);
	/*Force backup domain reset*/
	RCC->BDCR |= BDCR_BDRST;
	/*Release backup domain reset*/
	RCC->BDCR &= ~BDCR_BDRST;
	/*Set RTC clock source to LSI*/
	RCC->BDCR &= ~(1U << 8U);
	RCC->BDCR |= (1U << 9U);
	/*Enable the RTC*/
	RCC->BDCR |= BDCR_RTCEN;
	/*Disable RTC register write protection*/
	RTC->WPR = RTC_WRITE_PROTECTION_ENABLE_1;
	RTC->WPR = RTC_WRITE_PROTECTION_ENABLE_2;
	/*Enter the initialization mode*/
	if(rtc_init_seq() != 1)
	{
		/*TODO:*/
	}
	/*Set desired date: Fri Dec 29th 2016*/
	rtc_date_config(0x5,0x29,0x12,0x16);
	/*Set desired time: 11:59:55 PM*/
	rtc_time_config(1,0x11,0x59,0x55);
	/*Set hour formate*/
	RTC->CR |= CR_FMT;
	/*Set Async prescaler*/
	rtc_set_asynch_prescaler(RTC_ASYNCH_PREDIV);
	/*Set sync prescaler*/
	rtc_set_synch_prescaler(RTC_SYNCH_PREDIV);
	/*Exit the initialization mode*/
	if(exit_init_seq() != 1)
	{
		//TODO:
	}
	/*Enable RTC registers write protection*/
	RTC->WPR = 0xFF;
}

uint8_t rtc_convert_bin2bcd(uint8_t value)
{
	return (uint8_t)((((value)/10U) << 4U) | ((value) % 10U));
}

uint8_t rtc_convert_bcd2bin(uint8_t value)
{
	return (uint8_t)(((uint8_t)((value) & (uint8_t)0xF0U) >> (uint8_t)0x4U) * 10U + ((value) & (uint8_t)0x0FU));
}

uint32_t rtc_date_get_day(void)
{
	return (uint32_t)((READ_BIT(RTC->DR,(RTC_DR_DT | RTC_DR_DU))) >> RTC_DR_DU_Pos);
}

uint32_t rtc_date_get_year(void)
{
	return (uint32_t)((READ_BIT(RTC->DR,(RTC_DR_YT | RTC_DR_YU))) >> RTC_DR_YU_Pos);
}

uint32_t rtc_date_get_month(void)
{
	return (uint32_t)((READ_BIT(RTC->DR,(RTC_DR_MT | RTC_DR_MU))) >> RTC_DR_MU_Pos);
}

uint32_t rtc_time_get_second(void)
{
	return (uint32_t)((READ_BIT(RTC->TR,(RTC_TR_ST | RTC_TR_SU))) >> RTC_TR_SU_Pos);
}

uint32_t rtc_time_get_minute(void)
{
	return (uint32_t)((READ_BIT(RTC->TR,(RTC_TR_MNT | RTC_TR_MNU))) >> RTC_TR_MNU_Pos);
}

uint32_t rtc_time_get_hour(void)
{
	return (uint32_t)((READ_BIT(RTC->TR,(RTC_TR_HT | RTC_TR_HU))) >> RTC_TR_HU_Pos);
}

static void rtc_set_asynch_prescaler(uint32_t AsynchPrescaler)
{
	MODIFY_REG(RTC->PRER,RTC_PRER_PREDIV_A,AsynchPrescaler << RTC_PRER_PREDIV_A_Pos);
}
static void rtc_set_synch_prescaler(uint32_t synchPrescaler)
{
	MODIFY_REG(RTC->PRER,RTC_PRER_PREDIV_S,synchPrescaler << RTC_PRER_PREDIV_S_Pos);
}

static void rtc_date_config(uint32_t WeekDay,uint32_t Day,uint32_t Month,uint32_t Year)
{
	register uint32_t temp = 0U;

	temp = (WeekDay << RTC_DR_WDU_Pos) |\
			(((Year & 0xF0U) << (RTC_DR_YT_Pos - 4U)) | ((Year & 0x0FU) << RTC_DR_YU_Pos)) |\
		(((Month & 0xF0U) << (RTC_DR_MT_Pos - 4U)) | ((Month & 0x0FU) << RTC_DR_MU_Pos))   |\
		(((Day & 0xF0U) << (RTC_DR_DT_Pos - 4U)) | ((Day & 0x0FU) << RTC_DR_DU_Pos));
	MODIFY_REG(RTC->DR,(RTC_DR_WDU | RTC_DR_MT | RTC_DR_MU | RTC_DR_DT | RTC_DR_DU | RTC_DR_YT | RTC_DR_YU ),temp);
}

static void rtc_time_config(uint32_t Fomate12_24,uint32_t Hours,uint32_t Minutes,uint32_t Seconds)
{
	register uint32_t temp = 0U;

	temp = Fomate12_24 |\
			(((Hours & 0xF0U) << (RTC_TR_HT_Pos - 4U)) | ((Hours & 0x0FU) << RTC_TR_HU_Pos)) |\
			(((Minutes & 0xF0U) << (RTC_TR_MNT_Pos - 4U)) | ((Minutes & 0x0FU) << RTC_TR_MNU_Pos))   |\
			(((Seconds & 0xF0U) << (RTC_TR_ST_Pos - 4U)) | ((Seconds & 0x0FU) << RTC_TR_SU_Pos));
	MODIFY_REG(RTC->TR,(RTC_TR_PM | RTC_TR_HT | RTC_TR_HU | RTC_TR_MNT | RTC_TR_MNU | RTC_TR_ST | RTC_TR_SU ),temp);
}
void _rtc_enable_init_mode(void)
{
	RTC->ISR = RTC_INIT_MASK;
}

uint8_t _rtc_isActiveflag_init(void)
{
	return (uint8_t)((RTC->ISR & ISR_INITF) == ISR_INITF);
}

static uint8_t rtc_init_seq(void)
{
	/*Start init mode*/
	_rtc_enable_init_mode();

	/*Wait till we are in init mode*/
	while(_rtc_isActiveflag_init() != 1);

	return 1;
}

void _rtc_disable_init_mode(void)
{
	RTC->ISR = ~RTC_INIT_MASK;
}
uint8_t _rtc_isActiveflag_rs(void)
{
	return ((RTC->ISR & ISR_RSF) == ISR_RSF);
}
static uint8_t wait_for_synch(void)
{
	/*Clear RSF*/
	RTC->ISR &= ~ISR_RSF;
	/*Wait for register to synch*/
	while(_rtc_isActiveflag_rs() != 1);

	return 1;
}

static uint8_t exit_init_seq(void)
{
	/*Stop init mode*/
	_rtc_disable_init_mode();

	/*Wait for register to synchronise*/
	return wait_for_synch();
}

static void rtc_alarm_config_time(uint32_t Fomate12_24,uint32_t Hours,uint32_t Minutes,uint32_t Seconds)
{
	register uint32_t temp = 0U;

	temp = Fomate12_24 | (((Hours & 0xF0U) << (RTC_ALRMAR_HT_Pos - 4U)) | ((Hours & 0x0FU) << RTC_ALRMAR_HU_Pos)) |\
			(((Minutes & 0xF0U) << (RTC_ALRMAR_MNT_Pos - 4U)) | ((Minutes & 0x0FU) << RTC_ALRMAR_MNU_Pos))   |\
			(((Seconds & 0xF0U) << (RTC_ALRMAR_ST_Pos - 4U)) | ((Seconds & 0x0FU) << RTC_ALRMAR_SU_Pos));
	MODIFY_REG(RTC->ALRMAR,(RTC_ALRMAR_PM | RTC_ALRMAR_HT | RTC_ALRMAR_HU | RTC_ALRMAR_MNT | RTC_ALRMAR_MNU | RTC_ALRMAR_ST | RTC_ALRMAR_SU ),temp);
}

static void rtc_alarm_set_mask(uint32_t Mask)
{
	MODIFY_REG(RTC->ALRMAR,RTC_ALRMAR_MSK4 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK1, Mask);
}

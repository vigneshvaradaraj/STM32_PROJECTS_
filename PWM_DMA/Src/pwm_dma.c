/*
 * pwm_dma.c
 *
 *  Created on: 12-Sep-2025
 *      Author: vigneshv
 */
 #include "stm32_f429xx.h"

#define GPIOAEN   (1U << 0U)
#define TIM2EN   (1U << 0U)
#define CR1_CEN   (1U << 0U)
#define DIER_CC1DE   (1U << 9U)
#define DIER_CC2DE   (1U << 10U)
#define DIER_UDE  (1U << 8U)
#define DIER_TDE  (1U << 14U)

#define DMA1EN   (1U << 21U)
#define DMA_SCR_MINC   (1U << 10U)
#define DMA_SCR_TCIE  (1U << 4U)
#define DMA_SCR_EN   (1U << 0U)

#define SC_CIRC      (1U << 8U)

#define LOOKUP_TABLE_LEN 200

/*To know pin and alternate functionality go to datasheet*/

/*
 DMA1_Stream5_channel 3 is TIM2_CH1
 DMA1_Stream6_channel 3 is TIM2_CH2
 PA0 : TIM2_CH1
 PA1 : TIM2_CH2
*/
uint32_t ch1_duty_cycl_lookup[LOOKUP_TABLE_LEN] =
{
		400,1103,1138,623,747,1445,655,988,629,1206,836,896,73,1171,630,47,436,1063,302,906,1386,1242,1387,998,1535,1526,768,125,518,596,821,1491,996,451,1227,402,22,1100,84,1082,937,118,308,1411,1170,818,944,190,229,433,109,782,688,819,1576,1186,313,773,896,402,1404,1196,304,703,716,528,536,327,313,1550,1188,407,483,1551,688,1219,263,1455,1190,889,590,356,1413,1538,1302,180,214,1089,972,1429,299,378,711,539,1597,1554,1209,1068,372,505,1388,44,237,1066,942,1203,1305,811,1511,1354,1088,82,1062,150,152,1035,15,1256,447,752,150,214,1418,381,841,453,1314,194,1302,971,907,713,251,1596,201,298,1170,661,350,1099,14,141,1087,239,1358,862,1582,12,558,827,809,1019,311,1023,543,1055,796,716,748,825,950,873,1175,90,962,515,730,947,88,580,1178,112,1126,405,230,1427,1466,1111,861,1020,166,337,1315,1072,992,733,191,161,1079,213,1284,229,924,789,1313,650,1078,1219,1457,140
};

uint32_t ch2_duty_cycl_lookup[LOOKUP_TABLE_LEN] =
{
		409,581,1150,873,524,417,387,211,1386,373,392,1028,953,1284,70,934,197,409,991,1141,1263,508,108,481,1369,333,759,26,931,1018,1071,967,1268,865,89,1231,1386,1371,265,198,50,1390,855,1181,172,234,979,1077,1519,1006,283,305,1105,1043,1331,866,704,465,1348,125,826,660,499,1321,559,99,221,369,1391,1581,462,1015,1336,1194,55,1044,1070,1425,565,1497,1453,805,637,1241,921,1582,298,343,927,1423,737,748,166,1386,993,1123,515,528,860,1284,916,751,351,1522,635,989,333,296,637,189,1269,1351,800,1003,246,1452,593,323,1106,1216,1177,1385,519,1255,1370,494,696,894,400,1452,32,578,198,272,852,106,1475,733,288,974,108,1343,1528,1032,384,1558,923,1337,505,146,1390,306,1364,597,1105,1552,0,72,580,685,1102,1135,886,1055,619,24,1121,129,1212,103,440,958,682,190,1112,994,412,1392,746,1195,48,1029,1184,1113,33,512,771,595,230,1444,171,822,441,1402,483,511,972,1262,257,428
};
void tim2_pa0_pa1_pwm_dma_init(void)
{
	/*GPIO Cfg*/
	/*Enable clock acess to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;
	/*Set PA0 and PA1 mode to alternate function mode*/
	GPIOA->MODER |= (2U << 0);
	GPIOA->MODER |= (2U << 2);
	/*Set alternate function type to TIM2 (AF01)*/
	/*PA0*/
	GPIOA->AFR[0] |= (1U << 0U);
	GPIOA->AFR[0] &= ~(1U << 1U);
	GPIOA->AFR[0] &= ~(1U << 2U);
	GPIOA->AFR[0] &= ~(1U << 3U);
	/*PA1*/
	GPIOA->AFR[0] |= (1U << 4U);
	GPIOA->AFR[0] &= ~(1U << 5U);
	GPIOA->AFR[0] &= ~(1U << 6U);
	GPIOA->AFR[0] &= ~(1U << 7U);
	
	/*TIM2 Cfg*/
	/*Enable clock access to TIM2*/
	RCC->APB1ENR |= TIM2EN;
	/*Set timer prescaler*/
	/*APB1 = 16Mz Bydefault*/
	TIM2->PSC = 0;
	/*Set auto-reload value*/
	/*16 000 00 / 16 000 .ie 10KHz */
	TIM2->ARR = 1600 - 1;
	/*Reset the timer count*/
	TIM2->CNT = 0;
	/*Set PA1 (ch2) and PA0 (ch1) to 110 : PWM mode 1*/
	
	/*CH1*/
	TIM2->CCMR1 &= ~(1U << 4U);
	TIM2->CCMR1 |= (1U << 5U);
	TIM2->CCMR1 |= (1U << 6U);
	/*CH2*/
	TIM2->CCMR1 &= ~(1U << 12U);
	TIM2->CCMR1 |= (1U << 13U);
	TIM2->CCMR1 |= (1U << 14U);
	
	/*Cfg DMA Interrupt enable*/
	TIM2->DIER |= DIER_CC1DE|DIER_CC2DE|DIER_UDE|DIER_TDE;
	
	/*Enable ch1 and ch2*/
	/*CH1 enable*/
	TIM2->CCER |= (1U << 0U);
	/*CH2 enable*/
	TIM2->CCER |= (1U << 4U);
	
	/*Cfg DMA parameters*/
	/*Enable Clock access DMA*/
	RCC->AHB1ENR |= DMA1EN;

	/*Cfg DMA Stream 5*/
	//disable DMA stream
	DMA1_Stream5->CR = 0;

	//wait until stream is disabled
	while(DMA1_Stream5->CR & DMA_SCR_EN);

	/*Select DMA channel : CH1*/
	DMA1_Stream5->CR |= (1U << 25U);
	DMA1_Stream5->CR |= (1U << 26U);
	DMA1_Stream5->CR &= ~(1U << 27U);

	/*Enable circular mode*/
	DMA1_Stream5->CR |= SC_CIRC;
	/*Enable memory increment*/
	DMA1_Stream5->CR |= DMA_SCR_MINC;

	/*Set Memory size : 32 bits*/
	DMA1_Stream5->CR &= ~(1U << 13U);
	DMA1_Stream5->CR |= (1U << 14U);
	/*Set peripheral size : 32 bits*/
	DMA1_Stream5->CR &= ~(1U << 11U);
	DMA1_Stream5->CR |= (1U << 12U);

	/*Set transfer direction : memory to peripheral*/
	DMA1_Stream5->CR |= (1U << 6U);
	DMA1_Stream5->CR &= ~(1U << 7U);

	/*Set number of transfers*/
	DMA1_Stream5->NDTR = (uint16_t)LOOKUP_TABLE_LEN;

	/*Set peripheral address*/
	DMA1_Stream5->PAR = (uint32_t)(&TIM2->CCR1);
	/*Set memory address*/
	DMA1_Stream5->M0AR = (uint32_t)ch1_duty_cycl_lookup;

	/*Enable the DMA stream*/
	DMA1_Stream5->CR |= DMA_SCR_EN;

	/*Cfg DMA Stream 6*/
	//disable DMA stream
	DMA1_Stream6->CR = 0;

	//wait until stream is disabled
	while(DMA1_Stream6->CR & DMA_SCR_EN);

	/*Select DMA channel : CH1*/
	DMA1_Stream6->CR |= (1U << 25U);
	DMA1_Stream6->CR |= (1U << 26U);
	DMA1_Stream6->CR &= ~(1U << 27U);

	/*Enable circular mode*/
	DMA1_Stream6->CR |= SC_CIRC;

	/*Enable memory increment*/
	DMA1_Stream6->CR |= DMA_SCR_MINC;

	/*Set Memory size : 32 bits*/
	DMA1_Stream6->CR &= ~(1U << 13U);
	DMA1_Stream6->CR |= (1U << 14U);
	/*Set peripheral size : 32 bits*/
	DMA1_Stream6->CR &= ~(1U << 11U);
	DMA1_Stream6->CR |= (1U << 12U);

	/*Set transfer direction : memory to peripheral*/
	DMA1_Stream6->CR |= (1U << 6U);
	DMA1_Stream6->CR &= ~(1U << 7U);

	/*Set number of transfers*/
	DMA1_Stream6->NDTR = (uint16_t)LOOKUP_TABLE_LEN;

	/*Set peripheral address*/
	DMA1_Stream6->PAR = (uint32_t)(&TIM2->CCR1);
	/*Set memory address*/
	DMA1_Stream6->M0AR = (uint32_t)ch1_duty_cycl_lookup;

	/*Enable the DMA stream*/
	DMA1_Stream6->CR |= DMA_SCR_EN;

	/*Enable Timer*/
	TIM2->CR1 |= CR1_CEN;

}



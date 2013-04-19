
#ifndef __HARDWARE__
#define __HARDWARE__

#include "FreeRTOS.h"
#include "task.h"
#include "lpc23xx.h"
#include "cpu_setup.h"
#include <time.h>

void setup_hardware();
void init_hardware();
void gpio_init();
void gpio_int_init();
int setup_konter_onoff(unsigned int kanale, unsigned char statk);

void rtcWrite(struct tm *newTime);

#define PORT2_INPUT(kanal)	(FIO2PIN & BIT(kanal)) ? 1 : 0;
#define PORT1_INPUT(kanal)	(FIO1PIN & BIT(kanal)) ? 1 : 0;
#define PORT0_INPUT(kanal)	(FIO0PIN & BIT(kanal)) ? 1 : 0;

#ifdef BOARD_SANTER
	#ifdef BOARD_SANTER_v1_0
		#define uC	"NXP LPC 2387"
	
		#ifdef PAKAI_LED_UTAMA
			#define LED_UTAMA	BIT(18)
			
			#define setup_led_utama()	do {	\
						FIO1DIR = LED_UTAMA;	\
						FIO1CLR = LED_UTAMA;	\
					} while(0)
		#endif
		
		#ifdef PAKAI_RTC
			#define setup_rtc()	do {				\
						PCONP |= BIT(9);			\
						RTC_CCR  = BIT(4);			\
						RTC_AMR  = 0;				\
						RTC_CIIR = 0;				\
					} while(0)
			#define start_rtc()	do {				\
						RTC_CCR &= ~RTC_CCR_CTCRST;	\
						RTC_CCR |= RTC_CCR_CLKEN;	\
					} while(0)
		#endif
		
		#ifdef PAKAI_RELAY
			#define RLY_1	BIT(0)			/* P1 */
			#define RLY_2	BIT(1)
			#define RLY_3	BIT(4)			
			#define RLY_4	BIT(8)	
			#define RLY_5	BIT(9)
			#define RLY_6	BIT(10)			
			#define RLY_7	BIT(14)
			#define RLY_8	BIT(15)			
			
			#define cRelay1()		FIO1CLR = RLY_1
			#define cRelay2()		FIO1CLR = RLY_2
			#define cRelay3()		FIO1CLR = RLY_3
			#define cRelay4()		FIO1CLR = RLY_4
			#define cRelay5()		FIO1CLR = RLY_5
			#define cRelay6()		FIO1CLR = RLY_6
			#define cRelay7()		FIO1CLR = RLY_7
			#define cRelay8()		FIO1CLR = RLY_8
			
			#define sRelay1()		FIO1SET = RLY_1
			#define sRelay2()		FIO1SET = RLY_2
			#define sRelay3()		FIO1SET = RLY_3
			#define sRelay4()		FIO1SET = RLY_4
			#define sRelay5()		FIO1SET = RLY_5
			#define sRelay6()		FIO1SET = RLY_6
			#define sRelay7()		FIO1SET = RLY_7
			#define sRelay8()		FIO1SET = RLY_8

			#define setup_relay()		do {	\
						FIO1DIR = FIO1DIR | RLY_1 | RLY_2 | RLY_3 | RLY_4;		\
						FIO1DIR = FIO1DIR | RLY_5 | RLY_6 | RLY_7 | RLY_8;		\
					} while(0)	
		#endif
		
		#ifdef PAKAI_SHELL
			#define mainTX_ENABLE	( ( unsigned long ) 0x00010 )
			#define mainRX_ENABLE	( ( unsigned long ) 0x00040 )
			
			#define mainCOM_TEST_PRIORITY		( tskIDLE_PRIORITY + 2 )
			#define mainCOM_TEST_BAUD_RATE		( ( unsigned long ) 115200 )
			
			#define setup_serial0_P0()	do {				\
						PINSEL0 |= mainTX_ENABLE | mainRX_ENABLE;	\
					} while(0)
		#endif

		#ifdef PAKAI_SPI_SSP0		// SDCard
			#define CS_SDC			21
			#define cs_SDCard()		FIO1CLR |= BIT(CS_SDC)
			#define uncs_SDCard()	FIO1SET |= BIT(CS_SDC)
			#define setup_spi_ssp0()	do {	\
						PCONP		|= BIT(21);	\
						PINSEL3		|= BIT(9)  | BIT(8);	\
						PINSEL3		|= BIT(11) | BIT(10);	\
						PINSEL3		|= BIT(15) | BIT(14);	\
						PINSEL3		|= BIT(17) | BIT(16);	\
					} while(0)
					// PCONP default AKTIF
					// PINSEL SPI : SCK0 | SCK0 | MISO0 | MOSI0	
		#endif
		
		#ifdef PAKAI_SPI_SSP1		// ADC AD7708
			#define setup_spi_ssp1()	do {	\
						PCONP		|= BIT(10);	\
						PINSEL0		|= BIT(13);	\
						PINSEL0		|= BIT(15);	\
						PINSEL0		|= BIT(17);	\
						PINSEL0		|= BIT(19);	\
					} while(0)
					// PCONP default AKTIF
					// PINSEL SPI : SSEL1 | SCK1 | MISO1 | MOSI1
					// set ssel supaya tidak cs & uncs spi (auto)
		#endif	
		
		#ifdef PAKAI_SPI1_P0
			#define setup_spi1_p0()	do {	\
						PCONP		|= BIT(8);				\
						PINSEL0 	|= BIT(15) | BIT(17) | BIT(19);	\
						PCLKSEL0	|= BIT(16) | BIT(17);	\	
					} while(0)
					// PCONP defaultnya AKTIF
					// PINSEL SPI : SCK1 | MISO1 | MOSI1
					// PCLK/8 ~ 133ns
		#endif

		#ifdef PAKAI_ADC_7708
			#define RDY_AD7708		11		// P211
			#define setup_eint1()	do {			\
						PINSEL4		|= BIT(22);		\
					} while(0)

		#endif
		
		#ifdef PAKAI_SDCARD
			#define INS_SDC			2
			#define setup_sdc()		do	{					\
						PINSEL4		&= ~(BIT(5) | BIT(4));	\
						FIO2DIR 	&= ~(BIT(INS_SDC));		\
						PINMODE4	|= (BIT(5) | BIT(4));	\
					} while (0)
			
		#endif
	
		#define iKonter_1		BIT(5) 	// P25, konter_1
		#define iKonter_2		BIT(6)	// P26
		#define iKonter_3		BIT(7)	// P27
		#define iKonter_4		BIT(8)	// P28
		#define iKonter_5		BIT(9)	// P29
		
		#define iKonter_6		BIT(24)	// P024, konter_6
		#define iKonter_7		BIT(25)	// P025
		#define iKonter_8		BIT(26)	// P026
		#define iKonter_9		BIT(28)	// P027
		#define iKonter_10		BIT(28)	// P028

	#endif
#endif

#endif

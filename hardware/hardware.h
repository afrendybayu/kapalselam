
#ifndef __HARDWARE__
#define __HARDWARE__

#include "FreeRTOS.h"
#include "task.h"
#include "lpc23xx.h"
#include "cpu_setup.h"

void setup_hardware();
void init_hardware();

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
			#define mainTX_ENABLE	( ( unsigned long ) 0x0001 )
			#define mainRX_ENABLE	( ( unsigned long ) 0x0004 )
			
			#define mainCOM_TEST_PRIORITY		( tskIDLE_PRIORITY + 2 )
			#define mainCOM_TEST_BAUD_RATE		( ( unsigned long ) 115200 )
			
			#define setup_serial_P0()	do {				\
						PINSEL0 |= mainTX_ENABLE | mainRX_ENABLE;	\
					} while(0)
		#endif
	#endif
#endif

#endif



#include "hardware.h"


void setup_hardware()	{
	
	sysInit();
	gpio_init();
	setup_watchdog();

	PCLKSEL0 = 0x55555555;		// PCLK is the same as CCLK
	PCLKSEL1 = 0x55555555;
	
	PCONP   |= 0x80000000;
	FIO1DIR  = 0xFFFFFFFF;

	/* USB Power dinyalakan supaya memory USB bisa dipakai */
	//PCONP   |= 0x80000000;
	//FIO1DIR  = 0xFFFFFFFF;
	SCS 	|= (1<<0); 				// fast mode for port 0 and 1
	MEMMAP = 0x01;					// SCB
	
	#ifdef PAKAI_RELAY
		setup_relay();
	#endif
	
	#ifdef PAKAI_LED_UTAMA
		setup_led_utama();
	#endif

	#ifdef PAKAI_ADC
		setup_adc();
	#endif
	
	#ifdef CEK_BLINK
		blink_led();
	#endif

	#ifdef PAKAI_SHELL
		setup_serial0_P0();
		//vAltStartComTestTasks( mainCOM_TEST_PRIORITY, mainCOM_TEST_BAUD_RATE );
	#endif
	
	#ifdef PAKAI_SERIAL_3
		#ifdef PAKAI_SERIAL_3_P0
			setup_serial3_P0();
		#endif
	#endif

	#ifdef PAKAI_SERIAL_2			
		#ifdef PAKAI_SERIAL_2_P0
			setup_serial2_P0();
		#endif
	#endif

	
}

void init_hardware()	{
	#ifdef PAKAI_LED_UTAMA

	#endif
	
	#ifdef PAKAI_SHELL
		vAltStartComTestTasks( mainCOM_TEST_PRIORITY, mainCOM_TEST_BAUD_RATE );
	#endif
}


void gpio_init()	{
	// paksa PINSEL untuk GPIO //
	PINSEL0 = 0x00000000;
	PINSEL1 = 0x00000000;
	PINSEL2 = 0x00000000;
	PINSEL3 = 0x00000000;
	PINSEL4 = 0x00000000;
	PINSEL5 = 0x00000000;
	PINSEL6 = 0x00000000;
	PINSEL7 = 0x00000000;
	PINSEL8 = 0x00000000;
	PINSEL9 = 0x00000000;
	PINSEL10 = 0x00000000;
	
	
	// paksa untuk enable pull up //
	PINMODE0 = 0x00000000;
	PINMODE1 = 0x00000000;
	PINMODE2 = 0x00000000;
	PINMODE3 = 0x00000000;
	PINMODE4 = 0x00000000;
	PINMODE5 = 0x00000000;
	PINMODE6 = 0x00000000;
	PINMODE7 = 0x00000000;
	PINMODE8 = 0x00000000;
}

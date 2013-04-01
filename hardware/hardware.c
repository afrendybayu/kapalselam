

#include "hardware.h"

void setup_hardware()	{
	sysInit();
	
	/* USB Power dinyalakan supaya memory USB bisa dipakai */
	//PCONP   |= 0x80000000;
	//FIO1DIR  = 0xFFFFFFFF;
	//SCS 	|= (1<<0); 				// fast mode for port 0 and 1
	
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
		setup_serial_P0();
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
	//	vAltStartComTestTasks( mainCOM_TEST_PRIORITY, mainCOM_TEST_BAUD_RATE );
	#endif
}

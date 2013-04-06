

#include "FreeRTOS.h"
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
	gpio_int_init();
	
	#ifdef PAKAI_LED_UTAMA

	#endif
	
	#ifdef PAKAI_SHELL
		vAltStartComTestTasks( mainCOM_TEST_PRIORITY, mainCOM_TEST_BAUD_RATE );
	#endif
}

int setup_konter_onoff(unsigned int aaa, unsigned char statk) {
	//printf("\r\n aaa: %d, statk: %d\r\n", aaa, statk);
	int bbb = 0;
	if (statk==1) {
		/*
		if (aaa==0) {	IO2_INT_EN_R |= kont_1;		bbb = 101;	}
		if (aaa==1) {	IO2_INT_EN_R |= kont_2;		bbb = 102;	}
		//if (aaa==2) {	IO2_INT_EN_R |= kont_3;		bbb = 103;	}
		if (aaa==3) {	IO2_INT_EN_R |= kont_4;		bbb = 104;	}
		if (aaa==4) {	IO2_INT_EN_R |= kont_5;		bbb = 105;	}
		if (aaa==5) {	IO2_INT_EN_R |= kont_6;		bbb = 106;	}
		if (aaa==6) {	IO2_INT_EN_R |= kont_7;		bbb = 107;	}
		if (aaa==7) {	IO2_INT_EN_R |= kont_8;		bbb = 108;	}
		if (aaa==8) {	IO2_INT_EN_R |= kont_9;		bbb = 109;	}
		if (aaa==9) {	IO2_INT_EN_R |= kont_10;	bbb = 110;	}
		//*/
	} else if (statk==3)	{
		if (aaa==0) {	;	bbb = 201;	}
		if (aaa==1) {	;	bbb = 202;	}
		if (aaa==2) {	;	bbb = 203;	}
		if (aaa==3) {	;	bbb = 204;	}
		if (aaa==4) {	;	bbb = 205;	}
		if (aaa==5) {	;	bbb = 206;	}
		if (aaa==6) {	;	bbb = 207;	}
		if (aaa==7) {	;	bbb = 208;	}
		if (aaa==8) {	;	bbb = 209;	}
		if (aaa==9) {	;	bbb = 210;	}
	} else {
		if (aaa==0) {	IO2_INT_EN_R &= ~iKonter_1;		bbb = 1;	}
		if (aaa==1) {	IO2_INT_EN_R &= ~iKonter_2;		bbb = 2;	}
		if (aaa==2) {	IO2_INT_EN_R &= ~iKonter_3;		bbb = 3;	}
		if (aaa==3) {	IO2_INT_EN_R &= ~iKonter_4;		bbb = 4;	}
		if (aaa==4) {	IO2_INT_EN_R &= ~iKonter_5;		bbb = 5;	}
		if (aaa==5) {	IO2_INT_EN_R &= ~iKonter_6;		bbb = 6;	}
		if (aaa==6) {	IO2_INT_EN_R &= ~iKonter_7;		bbb = 7;	}
		if (aaa==7) {	IO2_INT_EN_R &= ~iKonter_8;		bbb = 8;	}
		if (aaa==8) {	IO2_INT_EN_R &= ~iKonter_9;		bbb = 9;	}
		if (aaa==9) {	IO2_INT_EN_R &= ~iKonter_10;	bbb = 10;	}
		
	}
	return bbb;
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

// fungsi gpio_int_init untuk inisialisasi input interrupt konter
void gpio_int_init()	{
	extern void ( gpio_ISR_Wrapper )( void );
	extern void ( timer1_ISR_Wrapper )( void );
	
	//reset_konter();
	portENTER_CRITICAL();

	// Timer1 untuk free running clock
	T1TCR = TxTCR_Counter_Reset;           // reset & disable timer 0

	// setup Timer 1 to count forever
	// Periode Timer 1 adalah 50 ns
	// akan overflow setiap 3.57 menit, interrupt dan
	// lakukan sesuatu pada variabel2.
	T1PR = 3 - 1;               	// set the prescale divider (60 / 3, 20 Mhz (50 ns))
	T1CCR = 0;                      // disable dulu compare registers
	T1EMR = 0;                      // disable external match register
	T1TCR = TxTCR_Counter_Enable;

	//siapkan interrupt handler untuk Timer 1
	VICIntSelect &= ~(VIC_CHAN_TO_MASK(VIC_CHAN_NUM_Timer1));
	VICIntEnClr  = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_Timer1);
	VICVectAddr5 = ( portLONG )timer1_ISR_Wrapper;
	VICVectPriority5 = 0x08;
	VICIntEnable = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_Timer1);
	T1MR0 = 0xFFFFFFFF;		// maksimum
	T1MCR = TxMCR_MR0I;     // interrupt on cmp-match0
	T1IR  = TxIR_MR0_Interrupt; // clear match0 interrupt

	// karena masih 3 menit yang akan datang, harusnya dienable dari sini gak masalah
	T1TCR = TxTCR_Counter_Enable;         // enable timer 0

	//siapkan interrupt handler untuk GPIO
	VICIntSelect    &= ~VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT3);
	VICIntEnClr      = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT3);
	VICVectAddr17 = ( portLONG )gpio_ISR_Wrapper;
	VICVectPriority17 = 0x05;
	VICIntEnable = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT3);


	// setup GPIO direction as input & interrupt
	FIO2DIR &= ~(iKonter_1 | iKonter_2 | iKonter_3 | iKonter_4 | iKonter_5);
	FIO0DIR &= ~(iKonter_6 | iKonter_7 | iKonter_8 | iKonter_9 | iKonter_10);
	
	// enable falling edge interrupt
	// inverse input	--> input MPU rising edge, masuk inverter falling edge
	
	IO2_INT_EN_F |= iKonter_1 | iKonter_2 | iKonter_3 | iKonter_4 | iKonter_5;
	IO0_INT_EN_F |= iKonter_6 | iKonter_7 | iKonter_8 | iKonter_9 | iKonter_5;

	portEXIT_CRITICAL();
}

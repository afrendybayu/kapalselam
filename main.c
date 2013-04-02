/*
    FreeRTOS V7.4.0 - Copyright (C) 2013 Real Time Engineers Ltd.

    FEATURES AND PORTS ARE ADDED TO FREERTOS ALL THE TIME.  PLEASE VISIT
    http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************

*/

/*

 *
 * "Check" hook -  This only executes every five seconds from the tick hook.
 * Its main function is to check that all the standard demo tasks are still 
 * operational.  Should any unexpected behaviour within a demo task be discovered 
 * the tick hook will write an error to the LCD (via the LCD task).  If all the 
 * demo tasks are executing with their expected behaviour then the check task 
 * writes PASS to the LCD (again via the LCD task), as described above.
 *
 * "uIP" task -  This is the task that handles the uIP stack.  All TCP/IP
 * processing is performed in this task.
 */

/* Scheduler includes. */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Demo app includes. */
//#include "LCD/portlcd.h"
#include "flash.h"
#include "cmd/sh_serial.h"

//#define mainTX_ENABLE	( ( unsigned long ) 0x0001 )
//#define mainRX_ENABLE	( ( unsigned long ) 0x0004 )

/* Demo application definitions. */
#define mainQUEUE_SIZE						( 3 )
#define mainCHECK_DELAY						( ( portTickType ) 5000 / portTICK_RATE_MS )
#define mainBASIC_WEB_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 6 )

/* Task priorities. */
#define mainQUEUE_POLL_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainFLASH_PRIORITY                  ( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
#define mainGEN_QUEUE_TASK_PRIORITY			( tskIDLE_PRIORITY ) 
#define configTIMER_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )

/* 
 * The task that handles the uIP stack.  All TCP/IP processing is performed in
 * this task.
 */


static void vLedTask( void *pvParameters );
static void vLedTask2( void *pvParameters );

/* Configure the hardware as required by the demo. */
//static void prvSetupHardware( void );

/* The queue used to send messages to the LCD task. */
//xQueueHandle xLCDQueue;
xTaskHandle hdl_led1;
xTaskHandle hdl_led2;

/*-----------------------------------------------------------*/

void dummy_del(int a)	{
	int i,j,k,z;
	for (z=0; z<a; z++)
		for (i=0; i<1000; i++)	{
			//for(j=0; j<1200; j++)		// untuk 60Mhz --> 500 ms
			for(j=0; j<200; j++)		// untuk 60Mhz --> 500 ms
				k=0;
		}
}

void led_blink()	{
	dummy_del(1);
	FIO0CLR = BIT(27);
	FIO1CLR = BIT(0);
	dummy_del(1);
	FIO0SET = BIT(27);
	FIO1SET = BIT(0);
}

void loop_led()		{
	while(1)	{
		led_blink();
	}
}
xSemaphoreHandle xButtonSemaphore;

int main( void )	{

	setup_hardware();

	vSemaphoreCreateBinary( xButtonSemaphore );		// xSemaphoreHandle xButtonSemaphore;
	xSemaphoreTake( xButtonSemaphore, 0 );

	xTaskCreate( vLedTask, ( signed portCHAR * ) "Led", configMINIMAL_STACK_SIZE*10, NULL, \
		tskIDLE_PRIORITY, ( xTaskHandle * ) &hdl_led1 );		// #define xTaskCreate(.....) 		xTaskGenericCreate( .. )
	
	xTaskCreate( vLedTask2, ( signed portCHAR * ) "Led2", configMINIMAL_STACK_SIZE*10, NULL, \
		mainCHECK_TASK_PRIORITY, ( xTaskHandle * ) &hdl_led2 );		// #define xTaskCreate(.....) 		xTaskGenericCreate( .. )


	init_hardware();

	//loop_led();
	/* Create the queue used by the LCD task.  Messages for display on the LCD
	are received via this queue. */

	/* Start the standard demo tasks. */
	//vStartComTestStringsTasks(mainCOM_TEST_PRIORITY, mainCOM_TEST_BAUD_RATE, mainCOM_TEST_LED );


	/* Start the scheduler. */
	vTaskStartScheduler();
		// ada di source/tasks.c
    /* Will only get here if there was insufficient memory to create the idle task. */
	return 0; 
}
/*-----------------------------------------------------------*/

#if 0
void vApplicationTickHook( void )
{
unsigned portBASE_TYPE uxColumn = 0;
static xLCDMessage xMessage = { 0, "PASS" };
static unsigned portLONG ulTicksSinceLastDisplay = 0;
static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	// Called from every tick interrupt.  Have enough ticks passed to make it
	// time to perform our health status check again?
	ulTicksSinceLastDisplay++;
	if( ulTicksSinceLastDisplay >= mainCHECK_DELAY )
	{
		ulTicksSinceLastDisplay = 0;
		
		// Has an error been found in any task?

        if( xAreBlockingQueuesStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR - BLOCKQ";
		}

		if( xAreBlockTimeTestTasksStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR - BLOCKTIM";
		}

		if( xAreGenericQueueTasksStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR - GENQ";
		}
		
		if( xAreQueuePeekTasksStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR - PEEKQ";
		}       
		
		if( xAreDynamicPriorityTasksStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR - DYNAMIC";
		}
        
        xMessage.xColumn++;

		// Send the message to the LCD gatekeeper for display.
		xHigherPriorityTaskWoken = pdFALSE;
		//xQueueSendToBackFromISR( xLCDQueue, &xMessage, &xHigherPriorityTaskWoken );
	}
}
#endif
/*-----------------------------------------------------------*/

void vLedTask( void *pvParameters )	{
	//
	//char s[128];
	int a = 0;
	vTaskDelay(3000);
	for( ;; )	{
		//FIO1CLR |= BIT(0) | BIT(8);
		FIO1CLR |= BIT(0);
		vTaskDelay(500);
		FIO1SET |= BIT(0);
		//FIO1SET |= BIT(0) | BIT(8);
		vTaskDelay(500);
		pll_feed();
		qsprintf("%d-kirim-queue-%d\r\n", a, a++);
	}
}

void vLedTask2( void *pvParameters )	{
	//
	
	for( ;; )	{
		FIO1CLR |= BIT(18);
		vTaskDelay(300);
		FIO1SET |= BIT(18);
		vTaskDelay(300);
		
	}
}

/*-----------------------------------------------------------*/

#if 0
void init_serial()	{
	PINSEL0 |= mainTX_ENABLE;
	PINSEL0 |= mainRX_ENABLE;
}

static void prvSetupHardware( void )
{
	#ifdef RUN_FROM_RAM
		/* Remap the interrupt vectors to RAM if we are are running from RAM. */
		SCB_MEMMAP = 2;
	#else
		
	#endif
	
	/* Disable the PLL. */
	PLLCON = 0;							// 0 = PLL dimatikan dulu
	PLLFEED = mainPLL_FEED_BYTE1;		// 0xAA
	PLLFEED = mainPLL_FEED_BYTE2;		// 0x55
	
	/* Configure clock source. */				// SCS = system control dan status register 
//	pakai internal RC saja
//	SCS |= mainOSC_ENABLE;						// mainOSC_ENABLE = 0x20, Hal 42	==> Untuk Kristal External
//	while( !( SCS & mainOSC_STAT ) );			// 
//	CLKSRCSEL = mainOSC_SELECT; 				// mainOSC_SELECT = 0x01 ==> 
	
	/* Setup the PLL to multiply the XTAL input by 4. */
	PLLCFG = ( mainPLL_MUL | mainPLL_DIV );		// set fcc jd 480 MHz
	PLLFEED = mainPLL_FEED_BYTE1;
	PLLFEED = mainPLL_FEED_BYTE2;

	/* Turn on and wait for the PLL to lock... */
	PLLCON = mainPLL_ENABLE;					// mainPLL_ENABLE = 0x01
	PLLFEED = mainPLL_FEED_BYTE1;
	PLLFEED = mainPLL_FEED_BYTE2;

	CCLKCFG = mainCPU_CLK_DIV;					// mainCPU_CLK_DIV = 8
	while( !( PLLSTAT & mainPLL_LOCK ) );
	
	/* Connecting the clock. */
	PLLCON = mainPLL_CONNECT;					// mainPLL_CONNECT = 0x02 | 0x01
	PLLFEED = mainPLL_FEED_BYTE1;
	PLLFEED = mainPLL_FEED_BYTE2;
	while( !( PLLSTAT & mainPLL_CONNECTED ) ); 
	
	/* 
	This code is commented out as the MAM does not work on the original revision
	LPC2368 chips.  If using Rev B chips then you can increase the speed though
	the use of the MAM.
	
	Setup and turn on the MAM.  Three cycle access is used due to the fast
	PLL used.  It is possible faster overall performance could be obtained by
	tuning the MAM and PLL settings.
	*/
	#if 1
	MAMCR = 0;
	//MAMTIM = mainMAM_TIM_3;			// MAMTIM=1 --> 20MHz, MAMTIM=2 --> 40MHz, MAMTIM=3 >40MHz, MAMTIM=4 >=60MHz
	MAMTIM = mainMAM_TIM_4;				// 
	MAMCR = mainMAM_MODE_FULL;
	#endif
	//*/
	
	/* Setup the led's on the MCB2300 board */
	vParTestInitialise();
	
	init_serial();
}

#endif



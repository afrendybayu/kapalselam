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


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.

    >>>>>>NOTE<<<<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details. You should have received a copy of the GNU General Public License
    and the FreeRTOS license exception along with FreeRTOS; if not itcan be
    viewed here: http://www.freertos.org/a00114.html and also obtained by
    writing to Real Time Engineers Ltd., contact details for whom are available
    on the FreeRTOS WEB site.

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************


    http://www.FreeRTOS.org - Documentation, books, training, latest versions, 
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, and our new
    fully thread aware and reentrant UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High 
    Integrity Systems, who sell the code with commercial support, 
    indemnification and middleware, under the OpenRTOS brand.
    
    http://www.SafeRTOS.com - High Integrity Systems also provide a safety 
    engineered and independently SIL3 certified version for use in safety and 
    mission critical applications that require provable dependability.
*/

/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the standard demo application tasks.
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "LCD" task - the LCD task is a 'gatekeeper' task.  It is the only task that
 * is permitted to access the display directly.  Other tasks wishing to write a
 * message to the LCD send the message on a queue to the LCD task instead of
 * accessing the LCD themselves.  The LCD task just blocks on the queue waiting
 * for messages - waking and displaying the messages as they arrive.
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
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Demo app includes. */
//#include "BlockQ.h"
//#include "death.h"
//#include "blocktim.h"
//#include "LCD/portlcd.h"
//#include "flash.h"
#include "partest.h"
//#include "GenQTest.h"
//#include "QPeek.h"
//#include "dynamic.h"

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

/* Constants to setup the PLL. */
// Untuk mendapat 60 Mhz ==> PLL Setting Fcco=480MHz, M=60, N=1. Hal 58 Contoh Setting PLL 
//#define mainPLL_MUL			( ( unsigned portLONG ) ( 8 - 1 ) )
#define PLL_MUL				60
#define mainPLL_MUL			( ( unsigned portLONG ) ( PLL_MUL - 1 ) )

//#define mainPLL_DIV			( ( unsigned portLONG ) 0x0000 )
#define PLL_DIV				1
#define mainPLL_DIV			( ( unsigned portLONG ) (PLL_DIV - 1) )

//#define mainCPU_CLK_DIV		( ( unsigned portLONG ) 0x0003 )
#define CLK_DIV				8
#define mainCPU_CLK_DIV		( ( unsigned portLONG ) (CLK_DIV-1) )
#define mainPLL_ENABLE		( ( unsigned portLONG ) 0x0001 )
#define mainPLL_CONNECT		( ( ( unsigned portLONG ) 0x0002 ) | mainPLL_ENABLE )
#define mainPLL_FEED_BYTE1	( ( unsigned portLONG ) 0xaa )
#define mainPLL_FEED_BYTE2	( ( unsigned portLONG ) 0x55 )
#define mainPLL_LOCK		( ( unsigned portLONG ) 0x4000000 )
#define mainPLL_CONNECTED	( ( unsigned portLONG ) 0x2000000 )
#define mainOSC_ENABLE		( ( unsigned portLONG ) 0x20 )
#define mainOSC_STAT		( ( unsigned portLONG ) 0x40 )
#define mainOSC_SELECT		( ( unsigned portLONG ) 0x01 )			// Kristal External 
#define intRC_SELECT		( ( unsigned portLONG ) 0x00 )			// Internal RC	==> CANnet jangan pakai ini
#define RTC_SELECT			( ( unsigned portLONG ) 0x10 )			// RTC

/* Constants to setup the MAM. */
#define mainMAM_TIM_3		( ( unsigned portCHAR ) 0x03 )
#define mainMAM_TIM_4		( ( unsigned portCHAR ) 0x04 )
#define mainMAM_MODE_FULL	( ( unsigned portCHAR ) 0x02 )

/* 
 * The task that handles the uIP stack.  All TCP/IP processing is performed in
 * this task.
 */


static void vLedTask( void *pvParameters );
static void vLedTask2( void *pvParameters );

/* Configure the hardware as required by the demo. */
static void prvSetupHardware( void );

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

void loop_led()		{
	while(1)	{
		led_blink();
	}
}

void led_blink()	{
	dummy_del(1);
	FIO0CLR = BIT(27);
	//FIO1CLR = BIT(18);
	dummy_del(1);
	FIO0SET = BIT(27);
	//FIO1SET = BIT(18);
}

int main( void )
{
	prvSetupHardware();
	int i=0;
	
	//vTaskDelay(2000);
	
	/* Create the queue used by the LCD task.  Messages for display on the LCD
	are received via this queue. */
	//xLCDQueue = xQueueCreate( mainQUEUE_SIZE, sizeof( xLCDMessage ) );

	/* Create the uIP task.  This uses the lwIP RTOS abstraction layer.*/
    //xTaskCreate( vuIP_Task, ( signed portCHAR * ) "uIP", mainBASIC_WEB_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY - 1, NULL );

	/* Start the standard demo tasks. */
	vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );			// mainBLOCK_Q_PRIORITY = tskIDLE_PRIORITY + 2 = 
		//	ada di Common/Minimal/BlockQ.c
    //vCreateBlockTimeTasks();		// 
		//	ada di Common/Minimal/blocktim.c
	#if 0
	xTaskCreate( vLedTask, ( signed portCHAR * ) "Led2",  (configMINIMAL_STACK_SIZE * 2) ,\
		 NULL, mainCHECK_TASK_PRIORITY, ( xTaskHandle * ) &hdl_led );
	#endif
	
	#if 1
	xTaskCreate( vLedTask, ( signed portCHAR * ) "Led", configMINIMAL_STACK_SIZE, NULL, \
		mainCHECK_TASK_PRIORITY, NULL );
		// #define xTaskCreate(.....) 		xTaskGenericCreate( .. )
	#endif
	
	#if 1
	xTaskCreate( vLedTask2, ( signed portCHAR * ) "Led2", configMINIMAL_STACK_SIZE, NULL, \
		mainCHECK_TASK_PRIORITY, ( xTaskHandle * ) &hdl_led2 );
		// #define xTaskCreate(.....) 		xTaskGenericCreate( .. )
	#endif

    //vStartLEDFlashTasks( mainFLASH_PRIORITY );					// mainFLASH_PRIORITY	= tskIDLE_PRIORITY + 2 = 
		//	xTaskCreate( vLEDFlashTask, ....) ada di Common/Minimal/flash.c
    //vStartGenericQueueTasks( mainGEN_QUEUE_TASK_PRIORITY );		// mainGEN_QUEUE_TASK_PRIORITY = tskIDLE_PRIORITY
		//	ada di Common/Minimal/GenQTest.c
    //vStartQueuePeekTasks();
		// ada di Common/Minimal/QPeek.c
    //vStartDynamicPriorityTasks();
		// ada di Common/Minimal/dinamic.c

	/* Start the tasks defined within this file/specific to this demo. */
	//xTaskCreate( vLCDTask, ( signed portCHAR * ) "LCD", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY - 1, NULL );

	/* Start the scheduler. */
	vTaskStartScheduler();
		// ada di source/tasks.c
	//loop_led();
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

void vLedTask( void *pvParameters )
{
	//
		
	for( ;; )	{
		FIO0CLR = BIT(27);
		vTaskDelay(100);
		FIO0SET = BIT(27);
		vTaskDelay(100);
	}
}

void vLedTask2( void *pvParameters )
{
	//
		
	for( ;; )	{
		FIO1CLR = BIT(18);
		vTaskDelay(300);
		FIO1SET = BIT(18);
		vTaskDelay(300);
	}
}

/*-----------------------------------------------------------*/

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
}





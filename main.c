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
#include "monita.h"
#include "modul/adc/ad7708.h"

/* Demo app includes. */
//#include "LCD/portlcd.h"
//#include "flash.h"
//#include "cmd/sh_serial.h"

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
//static void vLedTask2( void *pvParameters );

/* Configure the hardware as required by the demo. */
//static void prvSetupHardware( void );

/* The queue used to send messages to the LCD task. */
//xQueueHandle xLCDQueue;
xTaskHandle hdl_led;
extern struct t_st_hw st_hw;

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

xSemaphoreHandle xButtonSemaphore;




int main( void )	{
	st_hw.init = 0;
	st_hw.idle_c = 0;
	
	setup_hardware();
	init_hardware();
	
	//vSemaphoreCreateBinary( xButtonSemaphore );		// xSemaphoreHandle xButtonSemaphore;
	//xSemaphoreTake( xButtonSemaphore, 0 );

	xTaskCreate( vLedTask, ( signed portCHAR * ) "Led", configMINIMAL_STACK_SIZE*ST_LED, NULL, \
		tskIDLE_PRIORITY, ( xTaskHandle * ) &hdl_led );		// #define xTaskCreate(.....) 		xTaskGenericCreate( .. )
		
	init_ambilCepatTasks();
	


	/* Start the scheduler. */
	vTaskStartScheduler();
		// ada di source/tasks.c
    /* Will only get here if there was insufficient memory to create the idle task. */
	return 0; 
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook()	{
	st_hw.idle_c++;
}

void vApplicationTickHook( void )	{
	
unsigned portBASE_TYPE uxColumn = 0;
static xLCDMessage xMessage = { 0, "PASS" };
static unsigned portLONG ulTicksSinceLastDisplay = 0;
static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

#if 0
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
#endif
}

/*-----------------------------------------------------------*/

//extern volatile float data_f[];
extern struct t2_konter konter;
extern unsigned int giliran;

void vLedTask( void *pvParameters )	{

	char a=0, b=1, i=0;
	vTaskDelay(100);

	do	{
		vTaskDelay(100);
		FIO1PIN ^= LED_UTAMA;
		pll_feed();
	} while(st_hw.init==0);
	
	b = 5;
	for( ;; )	{
		if (a>b)	{
			FIO1PIN ^= LED_UTAMA;
			a=0;
			
			i = 1-i;
			if (i)	{
				st_hw.idle   = st_hw.idle_c;
				st_hw.idle_c = 0;
				pll_feed();
			}
			
			//qsprintf("hit: %.1f-%.1f = gh: %d, h: %d, hl: %d, p: %d, b: %d, g: %d\r\n", data_f[0], data_f[1], \
				konter.global_hit, konter.t_konter[0].hit, konter.t_konter[0].hit_lama, \
				konter.t_konter[0].last_period, konter.t_konter[0].beda, giliran);
			//qsprintf("reg ID     : %02x\r\n", m_read | reg_id);

		}
		vTaskDelay(100);
		a++;
		//qsprintf("%d-kirim-queue-%d\r\n", b, b++);
	}
}


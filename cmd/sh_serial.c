
// Afrendy Bayu
// Depok, 25 Maret 2013
// 

/* Scheduler include files. */

//#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "serial.h"
#include "serial/tinysh.h"
#include "sh_serial.h"
#include <stdarg.h>
#include "queue.h"
#include "hardware.h"
//#include <stdio.h>
#include "sh_hardware.h"
#include "sh_rtos.h"
#include "sh_data.h"
//#include <stdio.h>
#include "monita.h"

static xComPortHandle xPort;
static xQueueHandle xPrintQueue;
xTaskHandle *hdl_shell;
extern struct t_st_hw st_hw;

#if 1
void printd(int prio, const char *format, ...)	{
	va_list arg;
	int lg=0;

	if (prio>0)	{
		va_start (arg, format);
		lg = vsprintf (stdout, format, arg);	
		va_end (arg);
		vSerialPutString(xPort, format, lg);
	}
}

// qrprintf : custom printf yg dapat menyimpan data ke queue
void qsprintf(char *fmt, ...) {
	//uprintf("---> KIRIM : ");
	char str_buffer[128];
	int lg=0;
    va_list args;
    va_start(args, fmt);
    lg = vsprintf(str_buffer, fmt, args);
    lg = xQueueSend( xPrintQueue, ( void * ) str_buffer, ( portTickType ) 0 );
    va_end(args);
    //uprintf(" %d/%d >>>>> %s\r\n", lg, xPrintQueue, str_buffer);
}

// qrprintf : custom printf yg dapat mengambil data dari queue
void qrprintf(portTickType w) {
	//char *str_buffer;
char str_buffer[128];
	if (xPrintQueue != 0)	{
		if (xQueueReceive( xPrintQueue, &str_buffer, w ))	{
			//uprintf("PrintQueue: %d/%d__%s\r\n", xPrintQueue, strlen(str_buffer), str_buffer);
			//uprintf("%d__%s\r\n", strlen(str_buffer), str_buffer);
			vSerialPutString(xPort, str_buffer, strlen(str_buffer));
		}
	}
	//vTaskDelay(5);
}

// uprintf : user/custom printf ke serial debug
void uprintf(char *fmt, ...) {
	char str_buffer[128];
	int lg=0;
    va_list args;
    va_start(args, fmt);
    lg = vsprintf(str_buffer, fmt, args);
    va_end(args);
    vSerialPutString(xPort, str_buffer, lg);
	vTaskDelay(5);
}
#endif

void vAltStartComTestTasks( unsigned portBASE_TYPE uxPriority, unsigned long ulBaudRate )		{
const unsigned portBASE_TYPE uxQueueSize = 10;
const unsigned portBASE_TYPE uxQueueLength = 128;
	xPrintQueue = xQueueCreate( uxQueueSize, uxQueueLength );
	/* Initialise the com port then spawn the Rx and Tx tasks. */
	xSerialPortInitMinimal( ulBaudRate, configMINIMAL_STACK_SIZE );

	/* The Tx task is spawned with a lower priority than the Rx task. */
	xTaskCreate( vComRxTask, ( signed char * ) "Shell", comSTACK_SIZE * ST_SHELL, NULL, uxPriority, ( xTaskHandle * ) hdl_shell );
}

void init_banner()	{
//const unsigned portBASE_TYPE uxQueueSize = 10;
//const unsigned portBASE_TYPE uxQueueLength = 30;
	//xPrintQueue = xQueueCreate( uxQueueSize, ( unsigned portBASE_TYPE ) sizeof( char * ) );
	//xPrintQueue = xQueueCreate( uxQueueSize, uxQueueLength );
	//strcpy(s, "percobaan queue init_banner ...\r\n");
	//xQueueSend( xPrintQueue, ( void * ) s, ( portTickType ) 10 );
	//uprintf("xPrintQueue hasil queue: %d\r\n", xPrintQueue);
	uprintf("\r\n\r\nDaun Biru Engineering, Maret 2013\r\n");
	uprintf("==================================\r\n");
	uprintf("monita %s %s\r\n", BOARD_SANTER, BOARD_SANTER_v1_0);
	uprintf("CPU LPC 2387, %d MHz\r\n", configCPU_CLOCK_HZ/1000000);
	uprintf("FreeRTOS %s by Richard Barry\r\n", tskKERNEL_VERSION_NUMBER);		//  oleh Richard Barry
	uprintf("ARM-GCC %s : %s : %s\r\n", __VERSION__, __DATE__, __TIME__);
	
}

#if PAKAI_CONTOH_SHELL
static void display_args(int argc, char **argv)	{
	int i;
	for(i=0;i<argc;i++)
		uprintf("  argv[%d]=\"%s\"\r\n",i,argv[i]);
}

static void foo_fnt(int argc, char **argv)	{
	printf("foo command called\r\n");
	display_args(argc,argv);
}

static tinysh_cmd_t myfoocmd={0,"foo","foo command","[args]", foo_fnt,0,0,0};


static void atoxi_fnt(int argc, char **argv)	{
	int i;
	for(i=1;i<argc;i++)    {
		uprintf("\"%s\"-->%u (0x%x)\r\n",
		argv[i],tinysh_atoxi(argv[i]),tinysh_atoxi(argv[i]));
	}
}
#endif

void cmd_shell()	{
	#ifdef PAKAI_CONTOH_SHELL
	tinysh_add_command(&myfoocmd);
	#endif
	tinysh_add_command(&reset_cmd);
	tinysh_add_command(&task_list_cmd);
	tinysh_add_command(&cek_data_cmd);
	tinysh_add_command(&idle_tick_cmd);
}

static portTASK_FUNCTION( vComRxTask, pvParameters )		{
signed char cExpectedByte, cByteRxed;
portBASE_TYPE xResyncRequired = pdFALSE, xErrorOccurred = pdFALSE;
portBASE_TYPE xGotChar;
int ch;
char s[30];

	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	vTaskDelay(1);
	init_banner();
	

	
	cmd_shell();
	
	do	{
		vTaskDelay(100);
	} while (st_hw.init==0);
	
	
	sprintf(s, "%s$ ", PROMPT);
	tinysh_set_prompt(s);
	tinysh_char_in('\n');
	
	vTaskDelay(500);
	for( ;; )	{
		vTaskDelay(2);
		#if 1
		xGotChar = xSerialGetChar( xPort, &ch, 100 );
		if( xGotChar == pdTRUE )		{

			//if( xSerialGetChar( xPort, &ch, comRX_BLOCK_TIME ) )		{ // comRX_BLOCK_TIME = 0xffff
			
			tinysh_char_in((unsigned char) ch);
			FIO1PIN ^= LED_UTAMA;
			//xSerialPutChar( xPort, cByteRxed, comTX_BLOCK_TIME);
			//xSerialPutChar( xPort, (signed char) ch, 10000);
		}
		
		#endif
		qrprintf(0);
	}
}


#include "FreeRTOS.h"
#include "task.h"
#include "serial/tinysh.h"
#include "sh_serial.h"
#include "sh_rtos.h"

void task_list()	{
static signed char cListBuffer[ mainLIST_BUFFER_SIZE ];
const signed char *pcList = &( cListBuffer[ 0 ] );
const char * const pcHeader = "\r\nTask          State  Priority  Stack	#\r\n************************************************";
	qsprintf(pcHeader);
	vTaskList( cListBuffer );
	qsprintf(cListBuffer);
}

#if 0
void task_run_time()	{
static signed char cListBuffer[ mainLIST_BUFFER_SIZE ];
const signed char *pcList = &( cListBuffer[ 0 ] );
const char * const pcHeader = "\r\nTask          State  Priority  Stack	#\r\n************************************************";
	qsprintf(pcHeader);
	vTaskGetRunTimeStats( cListBuffer );
	qsprintf(cListBuffer);
}
#endif


#include "FreeRTOS.h"
#include "task.h"
#include "ambilcepat.h"


xTaskHandle *hdl_cepat;

void init_ambilCepatTasks( )		{
	xTaskCreate( sedotDataTask, ( signed portCHAR * ) "Santer", configMINIMAL_STACK_SIZE*10, NULL, \
		tskIDLE_PRIORITY+3, ( xTaskHandle * ) &hdl_cepat );
}

static portTASK_FUNCTION( sedotDataTask, pvParameters )		{
int ch;

	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	vTaskDelay(5000);
	reset_konter();

	for( ;; )	{
		vTaskDelay(100);
		hitung_rpm();
		data_frek_rpm();
	}
}

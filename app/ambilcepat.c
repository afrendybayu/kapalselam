
#include "FreeRTOS.h"
#include "task.h"
#include "ambilcepat.h"
#include "monita.h"


xTaskHandle *hdl_cepat;
extern struct t_st_hw st_hw;

void init_ambilCepatTasks( )		{
	xTaskCreate( sedotDataTask, ( signed portCHAR * ) "Santer", configMINIMAL_STACK_SIZE*ST_SANTER, NULL, \
		tskIDLE_PRIORITY+3, ( xTaskHandle * ) &hdl_cepat );
}

static portTASK_FUNCTION( sedotDataTask, pvParameters )		{
int ch;

	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	vTaskDelay(200);
	#ifdef PAKAI_ADC_7708
		init_ad7708();
	#endif
	reset_konter();
	st_hw.init = 1;
	for( ;; )	{
		vTaskDelay(100);
		hitung_rpm();
		data_frek_rpm();
	}
}

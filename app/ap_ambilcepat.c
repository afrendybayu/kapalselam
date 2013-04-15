
#include "FreeRTOS.h"
#include "task.h"
#include "ap_ambilcepat.h"
#include "monita.h"
#include "adc/ad7708.h"


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
	uprintf("  task : %s\r\n", __FUNCTION__);
	
	reset_konter();

	#ifdef PAKAI_SDCARD
		st_hw.sdc = 0;
		//init_sdc();
	#endif
	
	#ifdef PAKAI_ADC_7708
		st_hw.adc = 0;
		setup_ad7708();
		init_ad7708();
	#endif
	
	st_hw.init++;
	for( ;; )	{
		
		
		
		#ifdef PAKAI_ADC_7708
			//if (adc.ambil == 1)	{
				if (cek_adc_statusx() & ADC_RDY)	{	//	0x81 & 0x80
					ambil_data_ad7708();
				}
			//}
		
			#if 0
			if (cek_adc_statusx() & ADC_RDY)	{	//	0x81 & 0x80
				ambil_data_ad7708();
			}
			#endif
		#endif
		#if 0
		vTaskDelay(100);
		//hitung_rpm();
		//data_frek_rpm();
		#endif
		vTaskDelay(1);
	}
}

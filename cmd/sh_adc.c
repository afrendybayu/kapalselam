
#include "FreeRTOS.h"
#include "task.h"
#include "serial/tinysh.h"
#include "sh_serial.h"
#include "sh_adc.h"
#include "adc/ad7708.h"
#include "monita.h"


extern float data_f[];
extern struct t_adc adc;

void cek_adc(void)	{
	char i;
	uprintf(" ADC count = %d\r\n", adc.count);
	uprintf(" Ch | Kuantisasi |  Tegangan\r\n********************************\r\n");
	for (i=0; i<JML_KANAL_ADC; i++ ) {
		uprintf(" %2d | %10d | %8d V\r\n", \
			i+1, adc.data[i], (int) data_f[20+i]*1000);
	}
}

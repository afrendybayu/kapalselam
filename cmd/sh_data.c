
#include "FreeRTOS.h"
#include "task.h"
#include "monita.h"
#include "serial/tinysh.h"

extern float data_f[];

void cek_data(int argc, char **argv)	{
	int i=0 ;
	uprintf("\r\n    Cek data input modul    \r\n****************************************\r\n");
	//uprintf("");
	for (i=0; i<JML_TITIK_DATA; i++ ) {
		uprintf("%2d : %-16s : %-4s : %.2f\r\n", \
			i+1, "___NAMA_____", "----", data_f[i]);
	}
}


#include "FreeRTOS.h"
#include "task.h"
#include "serial/tinysh.h"
#include "sh_serial.h"
#include "sh_sdc.h"
#include "monita.h"
#include "ff9a/sdc.h"


void sh_cek_status_sdc()	{
	qsprintf("status : %s\r\n", (cek_status_sdc()==0)?"AKTIF":"MATI" );
}

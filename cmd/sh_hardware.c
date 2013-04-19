
#include "FreeRTOS.h"
#include "task.h"
#include "serial/tinysh.h"
#include "sh_hardware.h"
#include "cpu_setup.h"
#include "monita.h"
#include "iap.h"

extern struct t_st_hw st_hw;

void idle_tick()	{
	qsprintf("\r\nidle tick : %d/s, adc: %d/s\r\n", st_hw.idle, st_hw.adc_pd);
}

void cek_black_sector_rom()		{
	IAP_return_t iap_return;
	uchr i;
	for (i=0; i<JML_SECTOR_LPC; i++)	{
	iap_return = iapReadBlackSector(i, i);
		if (iap_return.ReturnCode == 0)	{
			uprintf("Sektor %2d kosong : %08X - %04X\r\n", i, iap_return.Result[0], iap_return.Result[1]);
		} else if (iap_return.ReturnCode == 2) {
			uprintf("Sektor %2d ISI[2] : %08X - %04X\r\n", i, iap_return.Result[0], iap_return.Result[1]);
		} else if (iap_return.ReturnCode == 8) {
			uprintf("Sektor %2d ISI[8] : mulai sektor %02d - %04X\r\n", i, iap_return.Result[0], iap_return.Result[1]);
		} else {
			uprintf("Error no : %d\r\n", iap_return.ReturnCode);
		}
	}
}

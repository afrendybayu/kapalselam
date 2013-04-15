
#include "FreeRTOS.h"
#include "task.h"
#include "serial/tinysh.h"
#include "sh_hardware.h"
#include "cpu_setup.h"
#include "monita.h"

extern struct t_st_hw st_hw;

void idle_tick()	{
	qsprintf("\r\nidle tick : %d/s, adc: %d/s\r\n", st_hw.idle, st_hw.adc_pd);
}


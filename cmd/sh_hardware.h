
//#include <stdio.h>
#include "FreeRTOS.h"
#include "sh_serial.h"
#include "cpu_setup.h"

#ifndef __SH_HARDWARE__
#define __SH_HARDWARE__

void idle_tick();

static tinysh_cmd_t reset_cmd={0,"reset","reset cpu","[args]", reset_cpu,0,0,0};
static tinysh_cmd_t idle_tick_cmd={ 0,"idle","lihat idle sistem","", idle_tick,0,0,0 };

#endif


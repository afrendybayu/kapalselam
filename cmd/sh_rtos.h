
#ifndef __SH_RTOS__
#define __SH_RTOS__

#include "FreeRTOS.h"
#include "task.h"
#include "sh_serial.h"
//#include "sh_rtos.h"



#define mainLIST_BUFFER_SIZE				512

void task_list();
static tinysh_cmd_t task_list_cmd={ 0,"task_list","task list FreeRTOS","", task_list,0,0,0 };

#endif

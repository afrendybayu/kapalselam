
#ifndef __SH_SDC__
#define __SH_SDC__


void sh_cek_status_sdc();
static tinysh_cmd_t cek_sdc_cmd={ 0,"cek_sdc","cek status sdc","", sh_cek_status_sdc, 0,0,0 };

#endif

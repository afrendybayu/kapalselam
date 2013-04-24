
#ifndef __SH_RTC__
#define __SH_RTC__



void set_date_kitab();
void set_date(int argc, char **argv);
void init_RTC_sh();
void get_cal();
void cek_flag_RTC_sh();
void set_date_kitab();

static tinysh_cmd_t set_date_cmd={0,"set_date","Mengeset waktu","thn bulan tgl jam menit", set_date,0,0,0};	
//static tinysh_cmd_t cek_flag_rtc_cmd={0,"frtc","","",  cek_flag_RTC_sh,0,0,0};
//static tinysh_cmd_t init_rtc_cmd={0,"irtc","","",  init_RTC_sh,0,0,0};
static tinysh_cmd_t kalender_rtc_cmd={0,"kalender","","",  get_cal,0,0,0};

#endif


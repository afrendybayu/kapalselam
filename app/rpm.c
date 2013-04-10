
#include "FreeRTOS.h"
#include "task.h"
#include "rpm.h"
#include "monita.h"
#include "math.h"

extern unsigned int giliran;
//extern volatile float data_f[];
extern struct t2_konter konter;
extern unsigned char status_konter[];

void set_konter_rpm(int st, unsigned int period)		{
	//new_period = T1TC;
	if (period > konter.t_konter[st].last_period)	{
		konter.t_konter[st].beda = (period -
			konter.t_konter[st].last_period) * 50;	// 1 clock 50 nanosecond
	}	else	{	// sudah overflow
		konter.t_konter[st].beda = (period +
			(0xFFFFFFFF - konter.t_konter[st].last_period)) * 50;	// 1 clock 50 nanosecond

	}
	konter.t_konter[st].hit++;
	konter.t_konter[st].last_period = period;
}

void set_konter_onoff(int i, int onoff) {	// hanya menambah jml counter hit tiap kanal saja
	//konter.t_konter[i].onoff = onoff;		// status dilihat dari GPIO (polling) saja
	konter.t_konter[i].hit++;
}

void reset_konter(void)	{
	int i;
	
	konter.global_hit = 0;
	konter.ovflow = 0;
	giliran = 0;
	
	for (i=0; i<JML_KANAL; i++)		{
		konter.t_konter[i].last_period = 0;
		konter.t_konter[i].beda = 0;
		konter.t_konter[i].hit = 0;
		konter.t_konter[i].hit2 = 0;
		konter.t_konter[i].hit_lama = 0;
		konter.t_konter[i].hit_lama2 = 0;
		konter.t_konter[i].onoff = 0;
		
		status_konter[i] = 0;
	}

}

void hitung_rpm(void)	{	
	//uprintf("%s() masuk ..., hit: %d, %d\r\n", __FUNCTION__, data_putaran[0], data_hit[0]);
	//struct t_env *env2;
	//env2 = (char *) ALMT_ENV;
	
	//if (env2->kalib[giliran].status==0)	
	{
		
		portENTER_CRITICAL();
		
		if (konter.t_konter[giliran].hit_lama == konter.t_konter[giliran].hit)		{
			//konter.t_konter[giliran].beda = 0;		
			data_putaran[giliran] = 0;
			
		}	else	{
			/* didapatkan frekuensi (HZ) putaran */
			//temp_rpm = konter.t_konter[i].beda; // beda msh dlm nS
			
			/* rpm */
			data_putaran[giliran] = konter.t_konter[giliran].beda;
		}
		data_hit[giliran] = konter.t_konter[giliran].hit;
		
		konter.t_konter[giliran].hit_lama = konter.t_konter[giliran].hit; 
		#ifdef PAKAI_PILIHAN_FLOW
		
		konter.t_konter[giliran].hit_lama2 = konter.t_konter[giliran].hit2; 
		#endif
		portEXIT_CRITICAL();
	}
	giliran++;
	if (giliran == JML_KANAL) giliran = 0;
	
	#if 0
	//portENTER_CRITICAL();
	for (i=0; i<KANALNYA; i++)
	{
		if (konter.t_konter[i].hit_lama == konter.t_konter[i].hit)
		{
			konter.t_konter[i].beda = 0;
		}
		
		konter.t_konter[i].hit_lama = konter.t_konter[i].hit; 
	}
	//portEXIT_CRITICAL();
	#endif
	
}


void data_frek_rpm (void) {
	//qsprintf("%s() masuk ...\r\n", __FUNCTION__);
	unsigned int i=0;
	char status;
	float temp_f;
	float temp_rpm;
	
	//struct t_env *env2;
	//env2 = (char *) ALMT_ENV;
	
	//for (i=0; i<JML_KANAL; i++)	
	{
		
		status = 0;
		//status = env2->kalib[i].status;
		
		if (status==sRPM)		{
			if (data_putaran[i])	{
				// cari frekuensi
				temp_f = (float) 1000000000.00 / data_putaran[i]; // beda msh dlm nS
				// rpm
				temp_rpm = temp_f * 60;		// ganti ke rps * 60;
				//uprintf(".");
			}
			else	{
				temp_f = 0;
				temp_rpm = 0;
			}
			//qsprintf("%s() masuk ...f: %d, rpm: %d\r\n", __FUNCTION__, (int) temp_f, (int) temp_rpm);
			
			//data_f[0] = temp_rpm;
			#if 0
			data_f[(i*2)+1] = (konter.t_konter[i].hit*env2->kalib[i].m)+env2->kalib[i].C;
			data_f[i*2] = (float) (temp_rpm*env2->kalib[i].m)+env2->kalib[i].C;
			#endif
			
			#if 0
			data_f[(i*2)+1] = (konter.t_konter[i].hit);
			data_f[i*2] = (float) (temp_rpm);
			#endif
			
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+(i*2+1)) = (int) data_f[i*2+1];	// konter.t_konter[i].hit;
			#endif
			
			#if 0
			if (data_f[(i*2)+1]>10000000) {		// reset setelah 10juta, 7 digit
			//if (data_f[(i*2)+1]>1000) {		// tes saja, reset setelah 10juta, 7 digit
				data_f[(i*2)+1] = 0;
				konter.t_konter[i].hit = 0;
			}
			#endif

			#ifdef PAKAI_RTC
			//*(&MEM_RTC0+(i*2))   = data_f[i*2];		// konter.t_konter[i].onoff;
			//*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];		// konter.t_konter[i].hit;
			#endif
			
		}	
		#if 0
		else if (status==sONOFF || status==sFLOW1 || status==sFLOW2 || status==ssFLOW2) {		// OnOff
			// 					
			data_f[i*2] = (float) konter.t_konter[i].onoff;
			data_f[i*2+1] = (float) konter.t_konter[i].hit;
			
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+(i*2))	 = data_f[i*2];		// konter.t_konter[i].onoff;
			*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];	// konter.t_konter[i].hit;
			#endif
		
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (status==nFLOW1) {		// flow_setelah_selector
			data_f[i*2]   = (float) konter.t_konter[i].hit;
			data_f[i*2+1] = (float) konter.t_konter[i].hit2;
			
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+(i*2))	 = data_f[i*2];			// konter.t_konter[i].hit;
			*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];		// konter.t_konter[i].hit2;
			#endif
			//printf("   %d : %d, %d : %d", (i*2), *(&MEM_RTC0+(i*2)), (i*2+1), *(&MEM_RTC0+(i*2+1)));
		} else if (status==nFLOW2) {		// Hitung flow_setelah_selector, dengan kalibrasi
			//data_f[i*2]   = (float) konter.t_konter[i].hit;
			//data_f[i*2+1] = (float) konter.t_konter[i].hit2;

			data_f[i*2]   = (konter.t_konter[i].hit*env2->kalib[i].m)+env2->kalib[i].C;
			data_f[i*2+1] = (konter.t_konter[i].hit2*env2->kalib[i].m)+env2->kalib[i].C;
			
			#if 1
			if (data_f[i*2]>9999999.99)	{		// 
				konter.t_konter[i].hit = 0;
			}
			
			if (data_f[i*2+1]>9999999.99)	{
				konter.t_konter[i].hit2 = 0;
			}
			#endif
			
			//*(&MEM_RTC0+(i*2))   = data_f[i*2];		// konter.t_konter[i].hit;
			//*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];		// konter.t_konter[i].hit2;
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+(i*2))	 = konter.t_konter[i].hit;		// konter.t_konter[i].hit;
			*(&MEM_RTC0+(i*2+1)) = konter.t_konter[i].hit2;		// konter.t_konter[i].hit2;
			#endif
		} else if (status==fFLOW) {		// flow_setelah_selector = 203
			data_f[i*2]   = (float) konter.global_hit;
			data_f[i*2+1] = (float) konter.ovflow;
			
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+(i*2))	 = konter.global_hit;
			*(&MEM_RTC0+(i*2+1)) = konter.ovflow;
			#endif
		#endif

		}
		#endif
	}
}


#ifndef __SH_ADC__
#define __SH_ADC__


void cek_adc(void);

static tinysh_cmd_t cek_adc_cmd={ 0,"cek_adc","cek nilai adc","", cek_adc, 0,0,0 };

#endif


#include "ad7708.h"
#include "monita.h"

extern struct t_st_hw st_hw;

unsigned char cek_reg_adc(unsigned char reg)	{
	unsigned char cmd, xx;
	
	cmd = m_read | reg;
	SSP1Send(cmd, 0);
	SSP1Receive();
	SSP1Send(cmd, 0);
	xx = SSP1Receive();
	
	return xx;
}

unsigned char set_reg_adc(unsigned char reg, unsigned char data)	{
	unsigned char cmd;
	
	cmd = m_write | reg;
	SSP1Send(cmd, 1);
	SSP1Receive();
	SSP1Send(data, 1);
	SSP1Receive();
	cek_reg_adc(reg);
	
	return cmd;
}

char init_ad7708()	{
	unsigned char idadc, adaADC=0, ch, cadc;
	short jj, kk, ww=0, gg=0;
	
	idadc = cek_adc_idx();		// cek
	vTaskDelay(1);
	ch = idadc>>4;
	//qsprintf("idadc : %02x >> ", idadc);

	if ( (ch==0x5) || (ch==0x4) )	{
		adaADC = 1;
	} else {
		return 0;
	}
	
	set_adc_ioconx(0x00);		// semua io port as input
	//uprintf("SET ioc : %02x\r\n", 0x00);
	//uprintf("ioc  : %02x \r\n", cek_adc_ioconx());
	vTaskDelay(1);
	set_adc_modex(0x01);		// mode idle
	//uprintf("SET mode : %02x\r\n", 0x01);
	//uprintf("mode : %02x \r\n", cek_adc_modex());
	adaADC = 2;
	uprintf("Kalibrasi %d kanal ADC %s, id: %02x\r\n", JML_KANAL_ADC, (ch==5)?"AD7708":"AD7718", idadc );
	for (ch=0; ch<JML_KANAL_ADC; ch++)	{		// JML_KANAL_ADC
		uprintf("  Kanal %2d : ", ch+1);
		if (ch>=8)
			cadc = ((ch+6) << 4) | unipolar | rADC2560;		// REFIN 0-2.56V
		else
			cadc = (ch << 4) | unipolar | rADC2560;			// REFIN 0-2.56V
		set_adc_kontrolx(cadc);
		vTaskDelay(1);
		set_adc_modex(CHCON_10 | M_ADC_IZS);				// ena CHOP, CHCON:10ch, ,
		vTaskDelay(1);
		jj = 0; kk = 0;
		do {
			jj++; kk++;
			cadc = cek_adc_modex() & M_ADC_IDLE;
			vTaskDelay(10);
			if (kk>100)		{
				adaADC = 3;
				break;
			}
		} while (cadc!=M_ADC_IDLE);
		
		if (adaADC==3)	{
			uprintf("GAGAL Kalibrasi [%d].\r\n", kk);
			gg++;
		} else {
			uprintf("OK : %d\r\n", jj);
			ww++;
		}
	}
	
	if (ww==JML_KANAL_ADC)	{
		uprintf("KALIBRASI KOMPLIT SUKSES %d ch\r\n", ww);
		adaADC = 4;
	} else {
		uprintf(" %d KANAL KALIBRASI ERROR.\r\n", gg);
		return adaADC;
	}
	
	set_adc_filterx(RATE_FILTER_20);
	uprintf("ADC iocon 0x%02x; adccon 0x%02x; mode 0x%02x\r\n", 	\
		cek_adc_ioconx(), cek_adc_kontrolx(), cek_adc_modex());
	set_adc_ioconx(0x00);

	adaADC = 5;
	st_hw.adc = 1;
	
	return adaADC;
}


void cek_adc_offset(void)	{
	
	unsigned char cmd;
	unsigned char xx;
	unsigned char isi[5];
	
	cmd = m_read | reg_offset;
	SSP1Send(cmd, 1);
	//isi[0] = SSP1Receive(cmd, 1);
	//isi[1] = SSP1Receive(cmd, 1);
	SSP1Terima(2);
	qsprintf("ofset adc: %02x %02x\r\n", isi[1], isi[0]);
	//qsprintf("ofset adc: %02x %02x %02x %02x\r\n", isi[3], isi[2], isi[1], isi[0]);
	//return xx;
}

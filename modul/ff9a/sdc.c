
/****************************
*		Afrendy Bayu
*   Depok, 16 April 2013
* http://www.k9spud.com/sdcard/
* http://www.ece.ucdavis.edu/vcl/vclpeople/jwwebb/measbd/ctrl_fpga/cdev/microsd_driver/html/microsd_8c-source.html
****************************/

#include "FreeRTOS.h"
#include "sdc.h"
#include "monita.h"
#include "spi_ssp.h"
#include "hardware.h"

#ifdef PAKAI_SDCARD

//#define DEBUG_SDC

extern struct t_st_hw st_hw;


inline unsigned char cek_ins_sdc()	{
	//unsigned char a = ((FIO2PIN & BIT(INS_SDC)) >> INS_SDC);
	return PORT2_INPUT(INS_SDC);
}

void SD_kirimCmd(unsigned char* cmd)	{
	int i;
	for(i=0; i<6; i++)	{
		//uprintf("%02x ", cmd[i]);
		//SSP0Send(cmd[i], 1);
		SSP0Send(*cmd, 1);
		//uprintf("%02x ", *cmd);
		cmd++;
	}
}

unsigned char respon_SDcmd(unsigned char* rspn, int length)	{
	int i=0, j=0;
	for(i = 0; i < length; i++)		{
		SSP0Send(0xff, 1);
		rspn[i] = SSP0Receive();
		//uprintf("rspn: %02x\r\n", rspn[i]);
		//uprintf("%02x ", rspn[i]);
		if (rspn[i] != 0xff)	j++;
	}
	return j;
}

uchr baca_blok_sdc(unsigned int almt, uchr *data, int len)	{
	uchr rspn;
	unsigned short crc;
	
	rspn = Microsd_SendCmd( SET_BLOCKLEN, len );
	if (rspn != R1_READY_STATE)
		return rspn;
	rspn = Microsd_SendCmd(READ_SINGLE_BLOCK, almt);
	if(rspn != R1_READY_STATE)
		return rspn;
	rspn = respon_SDcmd(data, len);
	if (rspn != START_TOKEN)	{		// START_TOKEN: 0xFE
		qsprintf("__START TOKEN salah !!__\r\n");
		return rspn;
	}
	
	rspn = SSP0byte(0xff);
	crc = rspn;
	rspn = SSP0byte(0xff);
	crc = (crc << 8) | rspn;
	
	if( crc != Microsd_crc16(data, len) ) {
		qsprintf("__ReadBlock(): CRC error, data dumped__\r\n");
		return 0x00;
	}
	
	return 0xff;
}

uchr tulis_block_sdc(unsigned int almt, uchr *data, int len)	{
	uchr rspn;
	unsigned int i;
	unsigned short crc;
	
	rspn = Microsd_SendCmd( SET_BLOCKLEN, len );
	if (rspn != R1_READY_STATE)
		return rspn;
	rspn = Microsd_SendCmd( WRITE_BLOCK, almt );
	if(rspn != R1_READY_STATE) {
		qsprintf("__START TOKEN salah__, respon:%02x !!\r\n", rspn);
		return rspn;
	}
	SSP0byte(0xff);
	SSP0byte(0xff);
	
	SSP0byte(0xfe);
	for(i = 0; i < len; i++)	{
		SSP0byte(&data[i]);
	}
	
	crc = Microsd_crc16(data, len);
	rspn = (crc >> 8) & 0xff;
	SSP0byte(rspn);
	rspn = (crc & 0xff);
	rspn = SSP0byte(rspn);
	
	if ( (rspn & 0x0f) == 0x05 )	{
		qsprintf(" BerHASIL simpan data\r\n");
	} else if ( (rspn & 0x0f) == 0x0B )	{
		qsprintf(" __CRC Error__\r\n");
	} else if ( (rspn & 0x0f) == 0x05 )	{
		qsprintf(" __Simpan Error__\r\n");
	} else {
		qsprintf(" respon aneh : %02x.\r\n", rspn);
	}
	
	do	{
		rspn = SSP0byte(0xff);
	} while (rspn == 0x00);
	
	
	return 0x00;
	
}

unsigned char cek_csd_sdc()	{
	unsigned char CMD9_CSD_STATE[] = { CMD9,0x00,0x00,0x00,0x00,0xFF };
	return SD_WriteCmd(CMD9_CSD_STATE);
}

unsigned char cek_csd_sdc2()	{
	unsigned char strRsp[2];
	Microsd_SendCmd( CMD9, 0x00000000 );
	respon_SDcmd( strRsp, R3_RESPONSE_SIZE );
	
	return strRsp[0];
}

unsigned char cek_versi2_sdc(void)		{
	int i;
	unsigned char strRsp[4];
	unsigned char versi=0, cc=0, rsp=0;
	
	unsigned char strHrp[] = { 0x00, 0x00, 0x01, 0xAA };
	Microsd_SendCmd( CMD8, 0x000001AA );
	rsp = respon_SDcmd( strRsp, R7_RESPONSE_SIZE );
	uprintf("\r\nhasil respon : %02x %02x %02x %02x\r\n", strRsp[0], strRsp[1], strRsp[2], strRsp[3]);
	
	if (rsp == R7_RESPONSE_SIZE)	{
		for (i=0; i<R7_RESPONSE_SIZE; i++)		{
			if (strRsp[i] == strHrp[i])
				cc++;
		}
		
		//uprintf("cc: %d\r\n", cc);
		if (cc==R7_RESPONSE_SIZE)	{
			uprintf("support SDC v2. ");
			versi = 2;
		} else {
			uprintf("tidak support SDC v2, cc: %d ", cc);
			versi = 0;
		}
	}
	
	return versi;
}


unsigned char SD_WriteCmd(unsigned char* cmd)	{
	unsigned int i;
	unsigned char response;
	
	//cmd[0] |= (1<<6);
	//cmd[0] &= ~(1<<7);
	cmd[0]  = (cmd[0] & 0x3F) | 0x40;
	cmd[5] |= (1<<0);
	
	#ifdef DEBUG_SDC
	uprintf("%02x %02x %02x %02x %02x %02x\r\n", cmd[0],cmd[1],cmd[2],cmd[3],cmd[4],cmd[5]);
	#endif
	
	for(i = 0; i < 6; ++i)	{
		SSP0Send(*cmd, 1);
		//uprintf("%02x ", *cmd);
		response = SSP0Receive();
		//uprintf("%02x ", response);
		cmd++;
	}
	
	// Wait for the response
	i = 0;
	do	{
		//response = SPIRead();
		SSP0Send(0xff, 1);
		response = SSP0Receive();
		//uprintf("%02x ", response);
		if(i > 50)		{
			break;
		}
		i++;
	} while(response == 0xFF);
	
	//#ifdef DEBUG_SDC
	uprintf("respon[%d]: %02x\r\n", i, response);
	//#endif
	// Following any command, the SD Card needs 8 clocks to finish up its work.
	// (from SanDisk SD Card Product Manual v1.9 section 5.1.8)
	//SPIWrite(0xFF); 
	SSP0Send(0xff, 1);

	//SD_CS = savedSD_CS;
	return (response);
}

unsigned char cmd_cid_csd_sdc(unsigned cmd)	{
	char i, status;
	unsigned char strRsp[50];
	status = Microsd_SendCmd( cmd, 0x00000000 );
	respon_SDcmd( strRsp, 20 );
	uprintf("\r\nrespon CMD%d : %02x : ", cmd, status);
	for(i=0; i<20; i++)	{
		uprintf("%02x ", strRsp[i]);
	}
}

void flush_ssp_spi()	{
	int i;
	unsigned char rsp;
	///uprintf("\r\nFlush !!!\r\n");
	for(i=0; i<8; i++)	{
		rsp = SSP0byte(0xff);
	//	uprintf("%02x ", rsp);
	}
	//uprintf("\r\n");
}

void sdc_info()		{
	int i;
	unsigned char status;
	unsigned char cid[20];			// Card Identification Register
    unsigned char csd[20];			// Card Specific Data Register
    unsigned char scr[8];			// SD card configuration register
    struct card_info c;
    
	status = Microsd_SendCmd_nocrc( CSD, 0x00000000 );
	respon_SDcmd( csd, 20 );
	flush_ssp_spi();
	
	status = Microsd_SendCmd_nocrc( CID, 0x00000000 );
	respon_SDcmd( cid, 20 );
	flush_ssp_spi();
    
    status = Microsd_SendCmd( SCR, 0x00000000 );
	respon_SDcmd( scr, 4 );
	uprintf("\r\nCMD%d : SCR : %02x : ", SCR, status);
	for(i=0; i<4; i++)	{
		uprintf("%02x ", cid[i]);
	}
	flush_ssp_spi();

	
	uprintf("\r\nCMD%d : CSD : %02x : ", CSD, status);
	for(i=0; i<20; i++)		uprintf("%02x ", csd[i]);
	for(i=0; i<20; i++)		{
		uprintf("%02x ", csd[i]);
		if (csd[i]==START_TOKEN)	{
			++i;
			break;
		}
	}
	uprintf("\r\nCID : TOKEN 0xFE i=%d\r\n", i);
	unsigned int Blocks;
    if ( (csd[i+0] & 0xc0) == 0) {		// ff fe 00
		//Type 1 csd
		unsigned int C_Size      = ((csd[i+6] & 0x03) << 10) | (csd[i+7] << 2) | ((csd[i+8] >>6) & 0x03);
		unsigned int C_Size_Mult = ((csd[i+9] & 0x03) << 1) | ((csd[i+10] >> 7) & 0x01);
		unsigned int Read_Bl_Len = csd[i+5] & 0x0f;
		Blocks = ((C_Size + 1) * (1 << (C_Size_Mult + 2)) * (1 << Read_Bl_Len)) / 512;
		uprintf("CSD tipe 1, block: %d, BL: %d\r\n", Blocks, Read_Bl_Len);
    } else {
		//Type 2 csd
		Blocks = (((csd[i+7] & 0x3F) << 16) | (csd[i+8] << 8) | csd[i+9]) * 1024;
		unsigned int C_Size = (((csd[i+7] & 0x3F) << 16) | (csd[i+8] << 8) | csd[i+9]);
		uprintf("CSD tipe 2, block: %d, Csize: %d, %06x, T: %d\r\n", Blocks, C_Size, C_Size, C_Size*512);
    }
	c.speed = csd[i+3];
	c.sector = ((csd[i+10] & 0xFC) << 2) | ((csd[i+11] & 0x1) >> 7) ;		// 
	uprintf("speed: 0x%02x, sector: %d\r\n", c.speed, c.sector);

	//c->state |= (CARD_STATE_DET & CARD_STATE_INI); 		// State - detected/initialized
	//c->type |= CARD_TYPE_SD;					// SD card
	//if (ocr[0] & 0x40) c->type |= CARD_TYPE_HC;			// std/high capacity
	//c->version = scr[0] & 0x0f;					// Version
	//c->volt = (ocr[1] << 16) + (ocr[2] << 8) + ocr[3];		// Voltage range from OCR
		
	
	
	
	//uprintf("\r\nCID : ");
	//for(i=0; i<20; i++)		uprintf("%02x ", cid[i]);
	//uprintf("\r\nCMD%d : CID : %02x : ", CID, status);
	for(i=0; i<20; i++)		{
		//uprintf("%02x ", cid[i]);
		if (cid[i]==START_TOKEN)	{	
			++i;
			break;
		}
	}
	uprintf("\r\nCID : TOKEN 0xFE i=%d\r\n", i);
	c.blocks = Blocks;						// Size in 512 byte blocks
	c.manid = cid[i+0];		 						// Manufacturer ID
	memcpy(c.name, &cid[i+3], 5);	 c.name[5] = '\0';	// Product Name
	memcpy(c.appid, &cid[i+1], 2);						// OEM/Application ID
	c.rev = cid[i+8];									// Product Revision
	c.year = 2000 + ((cid[i+13] & 0x0f) << 4) + (cid[i+14] >> 4);	// Product Date - year
	c.month = (cid[i+14] & 0x0f);					// Product Date - month
	c.serial = (cid[i+9] << 24) + (cid[i+10] << 16) + (cid[i+11] << 8) + cid[i+12]; // Serial Number
	uprintf("MID: %d, Nama: %s, Rev%d, SN: %ld, ManDate: %02d-%d\r\n", c.manid, c.name, c.rev, c.serial, c.month, c.year);


}

unsigned char cek_status_sdc(void)	{
	if (cek_ins_sdc())	{		// cek kartu ada ?
		unsigned char CMD0_GO_IDLE_STATE[] = { 0x00,0x00,0x00,0x00,0x00,0x95 };
		if  (SD_WriteCmd(CMD0_GO_IDLE_STATE) == 0x01)
			return 0;
		else return 1;
	}
	return 1;
}

unsigned char init_sdc(void)		{
	#ifdef DEBUG_SDC
	uprintf("--> %s\r\n", __FUNCTION__);
	#endif
	
	unsigned int i = 0;
	unsigned char status, v2=0;
	
	setup_spi_ssp0();
	init_ssp0(SCK_LAMA);

	for(status = 0; status < 10; ++status)	{
		for(i = 0; i; i++);
		for(i = 0; i; i++);
		for(i = 0; i; i++);
	}
	
	for(i = 0; i < 16; ++i)
		SSP0Send(0xFF, 1);			// write dummy data to pump clock signal line
	
	// Wait for the SD Card to go into IDLE state
	i = 0;	status = 0xff;
	do	{
		unsigned char CMD0_GO_IDLE_STATE[] = { 0x00,0x00,0x00,0x00,0x00,0x95 };
		status = SD_WriteCmd(CMD0_GO_IDLE_STATE);
		//status = Microsd_SendCmd( CMD0, 0x00000000);
		// fail and return
		if (i++ > 25)	{
			return 1;
		}
	} while( status != 0x01 );
	//#ifdef DEBUG_SDC
	#if 1
	uprintf("---> CMD0_GO_IDLE_STATE status[%d] : %d\r\n", i, status);
	#endif
	v2 = cek_versi2_sdc();
	//uprintf("v2: %02x\r\n", v2);
	flush_ssp_spi();
	
	i=0;
	do	{
		unsigned char CMD55_APP_CMD[] = {55,0x00,0x00,0x00,0x00,0xFF};
		status = SD_WriteCmd(CMD55_APP_CMD); // Do not check response here
		#ifdef DEBUG_SDC
		uprintf("---> CMD55_APP_CMD status[%d] : %d\r\n", i, status);
		#endif
		if (status==0x01)	{
			unsigned char cmdSDcmd41[] = { 41,0x40,0x00,0x00,0x00,0xe5 };		// kingston 4GB
			status = SD_WriteCmd(cmdSDcmd41);
			#ifdef DEBUG_SDC
			uprintf("---> CMD41_SD_SEND_OP_COND status[%d] : %02x\r\n", i, status);
			#endif
		}
		// fail and return
		if (i++ > 20)	{
			return 2;
		}
	} while( status != 0x00 );
	flush_ssp_spi();

	//vTaskDelay(50);
	init_ssp0(SCK_KILAT);
	st_hw.sdc = 1;

	vTaskDelay(50);


	#if 1
	unsigned char strRsp[50];
	status = Microsd_SendCmd( CMD58, 0x40000000 );
	respon_SDcmd( strRsp, R3_RESPONSE_SIZE );
	uprintf("---> 8 st: %02x, respon : %02x %02x %02x %02x\r\n", status, strRsp[0], strRsp[1], strRsp[2], strRsp[3]);
	#endif
	flush_ssp_spi();
	
	sdc_info();
	flush_ssp_spi();


	return 0;
}


unsigned char Microsd_crc7(unsigned char *MessBuff)	{
	int i;
	unsigned char	Upper8;
	unsigned int Lower32;

	Upper8 = MessBuff[0];
	Lower32 = ( (unsigned int) MessBuff[1] << 24 ) |
			  ( (unsigned int) MessBuff[2] << 16 ) |
			  ( (unsigned int) MessBuff[3] << 8  ) |
			  ( (unsigned int) MessBuff[4] );

	for(i = 0; i < 40; i++)		{
		if(Upper8 & 0x80) // if the MSB of the bit-frame is 1...
			Upper8 ^= G_POLY7; // xor with the generator polynomial

		/* Logical left shift of the bit-frame by 1 bit */
		Upper8 <<= 1;

		if(Lower32 & 0x80000000) // if the MSB of MessPtr->Lower32 is 1...
			Upper8 |= 0x01; // set the LSB of Upper8 to 1
			Lower32 <<= 1;
	}

	return Upper8;
}

unsigned short Microsd_crc16(unsigned char *MessBuff, unsigned int len)	{
	unsigned short crc = 0;
	int byten, bitn, xorFlag;

	if(len <= 1) {
		qsprintf("crc16() does not handle 1-byte messages.\r\n"); return 0; }

	for(byten = 0; byten < len+2; byten++) // Append 2-bytes to end of message.
		for(bitn = 0; bitn < 8; bitn++)		{
			xorFlag = crc & 0x8000;
			crc <<= 1;

			if((byten < len) && (MessBuff[byten] & (0x80 >> bitn)))
				crc |= 0x0001;

			if(xorFlag)
				crc ^= G_POLY16;

		}// end for(bitn)
	return crc;
}

unsigned char Microsd_SendCmd(unsigned char cmd_idx, unsigned int arg)	{
	int i = 0;
	unsigned char MessBuff[6], RecvBuff[6];

	//Microsd_FlushRxFifo(SDPtr);

	// Append start (zero) and transmission (one) bits to front,
	// insert argument to MessBuff[1:4], append CRC and stop bit (one) to end.
	MessBuff[0] = (cmd_idx & 0x3F) | 0x40;
	MessBuff[1] = (unsigned char) ((arg >> 24) & 0x000000FF);
	MessBuff[2] = (unsigned char) ((arg >> 16) & 0x000000FF);
	MessBuff[3] = (unsigned char) ((arg >>  8) & 0x000000FF);
	MessBuff[4] = (unsigned char) ( arg & 0x000000FF);
	MessBuff[5] = ( Microsd_crc7(MessBuff) | 0x01 );

	#if 0
	for(i=0; i<6; i++)	{
		uprintf("%02x ", MessBuff[i]);
	}
	uprintf("\r\n");
	#endif
	//Microsd_SpiTransfer(SDPtr, MessBuff, RecvBuff, 6);
	return SD_WriteCmd(MessBuff);

	//return Microsd_GetResponse(SDPtr);
}

unsigned char Microsd_SendCmd_nocrc(unsigned char cmd_idx, unsigned int arg)	{
	int i = 0;
	unsigned char MessBuff[6], RecvBuff[6];

	//Microsd_FlushRxFifo(SDPtr);

	// Append start (zero) and transmission (one) bits to front,
	// insert argument to MessBuff[1:4], append CRC and stop bit (one) to end.
	MessBuff[0] = (cmd_idx & 0x3F) | 0x40;
	MessBuff[1] = (unsigned char) ((arg >> 24) & 0x000000FF);
	MessBuff[2] = (unsigned char) ((arg >> 16) & 0x000000FF);
	MessBuff[3] = (unsigned char) ((arg >>  8) & 0x000000FF);
	MessBuff[4] = (unsigned char) ( arg & 0x000000FF);
	MessBuff[5] = ( 0xff | 0x01 );

	#if 1
	for(i=0; i<6; i++)	{
		uprintf("%02x ", MessBuff[i]);
	}
	uprintf("\r\n");
	#endif
	//Microsd_SpiTransfer(SDPtr, MessBuff, RecvBuff, 6);
	return SD_WriteCmd(MessBuff);

	//return Microsd_GetResponse(SDPtr);
}

#endif

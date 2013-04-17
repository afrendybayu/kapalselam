
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
		if (rspn[i] != 0xff)	j++;
	}
	return j;
}

unsigned char cek_versi2_sdc()	{
	int i;
	unsigned char strRsp[4];
	unsigned char versi=0, cc=0, rsp=0;
	
	unsigned char strHrp[] = { 0x00, 0x00, 0x01, 0xAA };
	Microsd_SendCmd( CMD8, 0x000001AA );
	rsp = respon_SDcmd( strRsp, R7_RESPONSE_SIZE );
	//uprintf(" respon : %02x %02x %02x %02x\r\n", strRsp[0], strRsp[1], strRsp[2], strRsp[3]);
	
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
			uprintf("tidak support SDC v2\r\n");
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
	
	//uprintf("%02x %02x %02x %02x %02x %02x\r\n", cmd[0],cmd[1],cmd[2],cmd[3],cmd[4],cmd[5]);

	for(i = 0; i < 6; ++i)	{
		SSP0Send(*cmd, 1);
		//uprintf("%02x ", *cmd);
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

	//uprintf("respon[%d]: %02x\r\n", i, response);

	// Following any command, the SD Card needs 8 clocks to finish up its work.
	// (from SanDisk SD Card Product Manual v1.9 section 5.1.8)
	//SPIWrite(0xFF); 
	SSP0Send(0xff, 1);

	//SD_CS = savedSD_CS;
	return (response);
}


unsigned char init_sdc(void)		{
	//uprintf("--> %s\r\n", __FUNCTION__);
	unsigned int i = 0;
	unsigned char status, v2=0;
	
	setup_spi_ssp0();
	init_ssp0(SCK_LAMA);

	for(status = 0; status < 10; ++status)	{
		for(i = 0; i; i++);
	}
	
	for(i = 0; i < 16; ++i)
		SSP0Send(0xFF, 1);			// write dummy data to pump clock signal line
	
	// Wait for the SD Card to go into IDLE state
	i = 0;
	do	{
		unsigned char CMD0_GO_IDLE_STATE[] = { 0x00,0x00,0x00,0x00,0x00,0x95 };
		status = SD_WriteCmd(CMD0_GO_IDLE_STATE);
		//status = Microsd_SendCmd( CMD0, 0x00000000);
		// fail and return
		if (i++ > 25)	{
			return 1;
		}
	} while( status != 0x01 );
	//uprintf("---> CMD0_GO_IDLE_STATE status[%d] : %d\r\n", i, status);
	
	v2 = cek_versi2_sdc();
	
	i=0;
	do	{
		unsigned char CMD55_APP_CMD[] = {55,0x00,0x00,0x00,0x00,0xFF};
		status = SD_WriteCmd(CMD55_APP_CMD); // Do not check response here
		//uprintf("---> CMD55_APP_CMD status[%d] : %d\r\n", i, status);

		if (status==0x01)	{
			unsigned char cmdSDcmd41[] = { 41,0x40,0x00,0x00,0x00,0xe5 };		// kingston 4GB
			status = SD_WriteCmd(cmdSDcmd41);
			//uprintf("---> CMD41_SD_SEND_OP_COND status[%d] : %02x\r\n", i, status);
		}
		// fail and return
		if (i++ > 20)	{
			return 2;
		}
	} while( status != 0x00 );
	
	unsigned char CMD58_APP_CMD[] = { 58,0x00,0x00,0x00,0x00,0xFF };
	status = SD_WriteCmd(CMD58_APP_CMD); // Do not check response here
	//uprintf("---> CMD58_APP_CMD status[%d] : %d\r\n", i, status);
	if (status == 0x01)	{
		uprintf("OCR : BLOCK ADDRESS [1]\r\n");
	} else {
		uprintf("OCR : BYTE ADDRESS  [0]\r\n");
	}
	vTaskDelay(200);
	//init_ssp0(SCK_KILAT);
	st_hw.sdc = 1;
	
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
	return 0;
}

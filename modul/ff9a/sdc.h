
#ifndef __M_SDCARD__
#define __M_SDCARD__

#define R1_IN_IDLE_STATE    (1<<0)   // The card is in idle state and running initializing process.
#define R1_ERASE_RESET      (1<<1)   // An erase sequence was cleared before executing because of an out of erase sequence command was received.
#define R1_ILLEGAL_COMMAND  (1<<2)   // An illegal command code was detected
#define R1_COM_CRC_ERROR    (1<<3)   // The CRC check of the last command failed.
#define R1_ERASE_SEQ_ERROR  (1<<4)  // An error in the sequence of erase commands occured.
#define R1_ADDRESS_ERROR    (1<<5)  // A misaligned address, which did not match the block length was used in the command.
#define R1_PARAMETER        (1<<6)  // The command's argument (e.g. address, block length) was out of the allowed range for this card

#define SD_STATUS_READY_MASK		0x01
#define SD_STATUS_LEGACY_MASK		0x02
#define SD_STATUS_HIGHCAP_MASK		0x04
#define SD_STATUS_FAT_READY_MASK	0x10
#define SD_STATUS_UNUSABLE_MASK		0x80

#define GO_IDLE_STATE		0
#define SEND_IF_COND		8
#define SET_BLOCKLEN		16
#define READ_SINGLE_BLOCK	17
#define WRITE_BLOCK			24
#define SD_SEND_OP_COND		41
#define APP_CMD				55
#define READ_OCR			58
#define CRC_ON_OFF			59


#define CMD0				0
#define CMD1				1
#define CMD8				8
#define CMD9				9
#define CMD10				10
#define CMD12				12
#define CMD16				16
#define CMD17				17
#define CMD18				18
#define CMD23				23
#define CMD24				24
#define CMD25				25
#define CMD41			    41
#define CMD55				55
#define CMD58				58


#define R1_IDLE_STATE		0x01
#define R1_READY_STATE		0x00
#define START_TOKEN 		0xFE

#define R1_RESPONSE_SIZE	1
#define R3_RESPONSE_SIZE	4
#define R7_RESPONSE_SIZE	4

#define G_POLY7  			0x89   // x^7 + x^3 + 1
#define G_POLY16 			0x1021 // (x^16) + x^12 + x^5 + 1

typedef struct {
	unsigned int	SpiBaseAddr;   // Base Address of the SPI component
	unsigned char	Status;        // microSD status, use SD_STATUS_*_MASK
} Microsd;

unsigned char init_sdc(void);
inline unsigned char cek_sdc();
void SD_kirimCmd(unsigned char* cmd);
unsigned char respon_SDcmd(unsigned char* rspn, int length);
unsigned char cek_versi2_sdc();
unsigned char Microsd_SendCmd(unsigned char cmd_idx, unsigned int arg);
unsigned char Microsd_crc7(unsigned char *MessBuff);
unsigned short Microsd_crc16(unsigned char *MessBuff, unsigned int len);
unsigned char SD_WriteCmd(unsigned char* cmd);

#endif

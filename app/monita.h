
#ifndef __APP_MONITA__
#define __APP_MONITA__

#define JML_SECTOR_LPC 		28
#define IAP_ADDRESS 		0x7FFFFFF1
typedef void (*IAP)(unsigned int [],unsigned int[]);


typedef struct	{
  unsigned int ReturnCode;
  unsigned int Result[4];
} IAP_return_t;

typedef enum IAP_STATUS_t {
   CMD_SUCCESS = 0,         // Command was executed successfully.
   INVALID_COMMAND = 1,     // Invalid command.
   SRC_ADDR_ERROR = 2,      // Source address is not on a word boundary.
   DST_ADDR_ERROR = 3,      // Destination address is not on a correct boundary.

   SRC_ADDR_NOT_MAPPED = 4, // Source address is not mapped in the memory map.
                            // Count value is taken in to consideration where
                            // applicable.

   DST_ADDR_NOT_MAPPED = 5, // Destination address is not mapped in the memory
                            // map. Count value is taken in to consideration
                            // where applicable.

   COUNT_ERROR = 6,         // Byte count is not multiple of 4 or is not a
                            // permitted value.

   INVALID_SECTOR = 7,      // Sector number is invalid.
   SECTOR_NOT_BLANK = 8,    // Sector is not blank.

   SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION = 9, // Command to prepare sector for
                                                // write operation was not
                                                // executed.

   COMPARE_ERROR = 10,      // Source and destination data is not same.
   BUSY = 11,               // Flash programming hardware interface is busy.
} IAP_STATUS_t;

//IAP_return_t iap_return;

#define  sRPM			0
#define	 sONOFF			1
#define	 sPUSHBUTTON	2
#define	 sFLOW1			3
#define	 sFLOW2			4
#define	 nFLOW1			100
#define	 ssFLOW2		201
#define	 nFLOW2			202
#define	 fFLOW			203
#define	 DAYA			6
#define  fENERGI		251
#define	 sADC_ORI		9

#define ATA		0
#define MMC		1
#define USB		2
#define ROM		3
#define SDC		4


#define JML_SUMBER		2
#define PER_SUMBER		20

#define ANGKA_PENTING	1000

#ifdef PAKAI_RELAY
	#define JML_RELAY		8
	#define JML_TITIK_DATA	(JML_SUMBER * PER_SUMBER)+JML_RELAY
#else
	#define JML_TITIK_DATA	(JML_SUMBER * PER_SUMBER)
#endif

float data_f [ JML_TITIK_DATA ];



// 32 KB
#define ALMT_SEKTOR_8	0x08000
#define ALMT_SEKTOR_9	0x10000
#define ALMT_SEKTOR_10	0x18000
#define ALMT_SEKTOR_11	0x20000
#define ALMT_SEKTOR_12	0x28000
#define ALMT_SEKTOR_13	0x30000
#define ALMT_SEKTOR_14	0x38000
#define ALMT_SEKTOR_15	0x40000
#define ALMT_SEKTOR_16	0x48000
#define ALMT_SEKTOR_17	0x50000
#define ALMT_SEKTOR_18	0x58000
#define ALMT_SEKTOR_19	0x60000
#define ALMT_SEKTOR_20	0x68000
#define ALMT_SEKTOR_21	0x70000

// 4 KB
#define ALMT_SEKTOR_22	0x78000
#define ALMT_SEKTOR_23	0x79000
#define ALMT_SEKTOR_24	0x7A000
#define ALMT_SEKTOR_25	0x7B000
#define ALMT_SEKTOR_26	0x7C000
#define ALMT_SEKTOR_27	0x7D000

#define SEKTOR_ENV		22
#define ALMT_ENV		ALMT_SEKTOR_22


#define JUM_GPIO	10
#define JML_KANAL	10

#define uchr		unsigned char

typedef struct {
	//unsigned int new_period;
	unsigned int last_period;
	unsigned int beda;			// kali 10
	unsigned int hit;
	unsigned int hit_lama;		// untuk mengetahui mati atau enggak
	unsigned char onoff;
	unsigned int hit2;
	unsigned int hit_lama2;		// untuk mengetahui mati atau enggak
} ts_konter;

struct t2_konter{
	unsigned int global_hit;
	unsigned int ovflow;		// overflow count untuk Timer 1
	ts_konter t_konter[JML_KANAL];
};

unsigned int giliran;
struct t2_konter konter;
unsigned char status_konter[JML_KANAL];

struct t_st_hw  {
	unsigned char init;
	long int idle;
	long int idle_c;
	unsigned char adc;
	long int adc_pd;
	long int adc_c;
	unsigned char rtc;
	unsigned char sdc;
};

struct t_st_hw st_hw;

struct t_kalib {
	float m;
	float C;
	char status;		// [status lihat define di atas !!!] //
	char ket[32];
	char adc;
	char formula[32];
};

struct t_adc {
	unsigned char cur_kanal;
	unsigned char count;
	unsigned char ambil;
	unsigned short data[JML_KANAL];
	//float flt_data[JML_KANAL];
};

struct t_adc adc;


struct t_env {
	char nama_board[32];
	unsigned char IP0;
	unsigned char IP1;
	unsigned char IP2;
	unsigned char IP3;
	unsigned char GW0;
	unsigned char GW1;
	unsigned char GW2;
	unsigned char GW3;
	struct t_kalib kalib[JML_KANAL];
	int magic1;
	int magic2;
	int mmc_serial;
	char SN[20];
	char berkas[32];
	char statusWebClient;
	char banyak_sumber;
	char burst;
	unsigned char wIP0;
	unsigned char wIP1;
	unsigned char wIP2;
	unsigned char wIP3;
	char statusWebClientI;
	unsigned char k1;
	unsigned char k2;
	unsigned char uRTC;
	char	statusSerClient;
	int		intKirim;
	int		intReset;
	float	intTole;
	int		netTot;
	char passwd[15];
	char nohp[18];
	char cron;
	unsigned char almtSlave;
	char statusSlave;
};
struct t_env env;


#ifdef PAKAI_RTC
	unsigned char flagRTCc;
	unsigned char flagRTCs;
	unsigned char flagRTCm;
	unsigned char flagRTCh;
	unsigned char flagRTCd;
	
typedef struct __attribute__ ((packed)) {
	union  {
		struct	{
			unsigned int counter   : 14;
			unsigned int rsvd15_31 : 18;
		};
		unsigned int i;
	};
} rtcCTC_t;

typedef struct __attribute__ ((packed))	{
	union	{
		struct	{
			unsigned int seconds   : 6;
			unsigned int rsvd7_6   : 2;
			unsigned int minutes   : 6;
			unsigned int rsvd14_15 : 2;
			unsigned int hours     : 5;
			unsigned int rsvd21_23 : 3;
			unsigned int dow       : 3;
			unsigned int rsvd27_31 : 5;
		};
		unsigned int i;
	};
} rtcCTIME0_t;

typedef struct __attribute__ ((packed))	{
	union	{
		struct	{
			unsigned int dom       : 5;
			unsigned int rsvd5_7   : 3;
			unsigned int month     : 4;
			unsigned int rsvd12_15 : 4;
			unsigned int year      : 12;
			unsigned int rsvd28_31 : 4;
		};
		unsigned int i;
	};
} rtcCTIME1_t;

typedef struct __attribute__ ((packed))	{
	union	{
		struct 	{
			unsigned int doy       : 12;
			unsigned int rsvd12_31 : 20;
		};
		unsigned int i;
	};
} rtcCTIME2_t;

struct rtc_time {
  int	tm_sec;
  int	tm_min;
  int	tm_hour;
  int	tm_mday;	// 1 - 31 /
  int	tm_mon;		// 0 - 11 //
  int	tm_year;
  int	tm_wday;
  int	tm_yday;
  int	tm_isdst;
};

#endif

#endif

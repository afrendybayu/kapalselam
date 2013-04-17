
#ifndef __APP_MONITA__
#define __APP_MONITA__

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

#define IAP_LOCATION 0x7FFFFFF1
typedef void (*IAP)(unsigned int [],unsigned int[]);

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

#endif

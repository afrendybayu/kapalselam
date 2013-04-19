
#include "iap.h"
#include "monita.h"
 
unsigned int param_table[5];

//typedef void (*IAP)(unsigned int [],unsigned int[]);

void iap_entry(unsigned int param_tab[], unsigned int result_tab[])		{
	IAP_return_t iap_return;
	void (*iap)(unsigned int[], unsigned int[]);
	iap = (void (*)(unsigned int[], unsigned int[]))IAP_ADDRESS;
	iap(param_tab,result_tab);
}

IAP_return_t iapReadSerialNumber(void)	{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = IAP_CMD_READPARTID;
	iap_entry(param_table,(unsigned int*)(&iap_return));
	return iap_return;
}

IAP_return_t iapReadBootVersion(void)	{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = IAP_CMD_READBOOTCODEVERSION;
	iap_entry(param_table,(unsigned int*)(&iap_return));
	return iap_return;
}

IAP_return_t iapReadBlackSector(uchr awal, uchr akhir)		{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = IAP_CMD_BLANKCHECKSECTOR;
	param_table[1] = awal;
	param_table[2] = akhir;
	iap_entry(param_table,(unsigned int*)(&iap_return));
	return iap_return;
}

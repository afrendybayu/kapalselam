#ifndef _IAP_H_
#define _IAP_H_

#include "FreeRTOS.h"
#include "monita.h"

#define IAP_CMD_PREPARESECTORFORWRITE (50)
#define IAP_CMD_COPYRAMTOFLASH        (51)
#define CAP_CMD_ERASESECTORS          (52)
#define IAP_CMD_BLANKCHECKSECTOR      (53)
#define IAP_CMD_READPARTID            (54)
#define IAP_CMD_READBOOTCODEVERSION   (55)
#define IAP_CMD_COMPARE               (56)
#define IAP_CMD_REINVOKEISP           (57)
#define IAP_CMD_READUID               (58)

IAP_return_t iapReadBootVersion(void);
IAP_return_t iapReadSerialNumber(void);
IAP_return_t iapReadBlackSector(uchr awal, uchr akhir)	;

#endif

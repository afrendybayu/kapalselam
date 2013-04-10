
#ifndef __SPI_SSP__
#define __SPI_SSP__

#include "lpc23xx.h"

#define SPI_BitE	BIT(2)
#define SPI_CPHA	BIT(3)
#define SPI_CPOL	BIT(4)
#define SPI_MSTR	BIT(5)
#define SPI_LSBF	BIT(6)
#define SPI_SPIE	BIT(7)

void init_spi0();

#endif

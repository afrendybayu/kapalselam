
#include "FreeRTOS.h"
#include "lpc23xx.h"
#include "sspi_ssp.h"

void init_spi0()	{
	// SPI master, 8bit/transfer default, MSB first default
	// SPI INT enable, Fist clock edge SCK, CPOL aktif high
	S0SPCR = MSTR;
}

U8 spiPut (char valueIn)	{
  SPI_SPDR = valueIn;

  while (!(SPI_SPSR & SPI_SPIF));

  return SPI_SPDR;
}

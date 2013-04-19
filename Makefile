#/*
#    FreeRTOS V7.4.0 - Copyright (C) 2013 Real Time Engineers Ltd.
#	
#
#    ***************************************************************************
#     *                                                                       *
#     *    FreeRTOS tutorial books are available in pdf and paperback.        *
#     *    Complete, revised, and edited pdf reference manuals are also       *
#     *    available.                                                         *
#     *                                                                       *
#     *    Purchasing FreeRTOS documentation will not only help you, by       *
#     *    ensuring you get running as quickly as possible and with an        *
#     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
#     *    the FreeRTOS project to continue with its mission of providing     *
#     *    professional grade, cross platform, de facto standard solutions    *
#     *    for microcontrollers - completely free of charge!                  *
#     *                                                                       *
#     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
#     *                                                                       *
#     *    Thank you for using FreeRTOS, and thank you for your support!      *
#     *                                                                       *
#    ***************************************************************************
#
#
#    This file is part of the FreeRTOS distribution.
#
#    FreeRTOS is free software; you can redistribute it and/or modify it under
#    the terms of the GNU General Public License (version 2) as published by the
#    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
#    >>>NOTE<<< The modification to the GPL is included to allow you to
#    distribute a combined work that includes FreeRTOS without being obliged to
#    provide the source code for proprietary components outside of the FreeRTOS
#    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
#    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#    more details. You should have received a copy of the GNU General Public
#    License and the FreeRTOS license exception along with FreeRTOS; if not it
#    can be viewed here: http://www.freertos.org/a00114.html and also obtained
#    by writing to Richard Barry, contact details for whom are available on the
#    FreeRTOS WEB site.
#
#    1 tab == 4 spaces!
#
#    http://www.FreeRTOS.org - Documentation, latest information, license and
#    contact details.
#
#    http://www.SafeRTOS.com - A version that is certified for use in safety
#    critical systems.
#
#    http://www.OpenRTOS.com - Commercial support, development, porting,
#    licensing and training services.
#*/

NAMA_FILE=rtosdemo


PENULIS=../../../../../../atinom/modul/Penulis/lpc_dbe
RTOS_SOURCE_DIR=modul/source
DEMO_COMMON_DIR=modul/minimal
DEMO_INCLUDE_DIR=modul/includ
UIP_COMMON_DIR=../../Common/ethernet/uIP/uip-1.0/uip
GCC=../../../../../../atinom/modul/TOOLCHAIN/bin/
CC=$(GCC)arm-elf-gcc
#CC=arm-elf-gcc
OBJCOPY=$(GCC)arm-elf-objcopy
UKURAN=$(GCC)arm-elf-size
LDSCRIPT=hardware/lpc2368.ld

LINKER_FLAGS=-mthumb -nostartfiles -Xlinker -o$(NAMA_FILE).elf -Xlinker -M -Xlinker -Map=hasil/$(NAMA_FILE).map

DEBUG=-g
OPTIM=-O0		
#	kalo OPTIM=-O1 --> kedip kecil trus ngehang


CFLAGS= $(DEBUG) \
		$(OPTIM) \
		-T$(LDSCRIPT) \
		-I . \
		-I ./modul 		\
		-I ./hardware 	\
		-I ./cmd		 \
		-I ./app		\
		-I $(RTOS_SOURCE_DIR)/include \
		-I $(RTOS_SOURCE_DIR)/portable/GCC/ARM7_LPC23xx \
		-I $(DEMO_INCLUDE_DIR) \
		-D ROWLEY_LPC23xx \
		-D THUMB_INTERWORK \
		-mcpu=arm7tdmi \
		-D PACK_STRUCT_END=__attribute\(\(packed\)\) \
		-D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) \
		-fomit-frame-pointer \
		-mthumb-interwork \
#		-fno-strict-aliasing

#		-I $(UIP_COMMON_DIR) \
#		-I ./webserver \
#		-fno-dwarf2-cfi-asm \



THUMB_SOURCE= \
		main.c \
		hardware/hardware.c		\
		hardware/cpu_setup.c	\
		hardware/syscalls.c		\
		hardware/spi_ssp.c		\
		hardware/iap.c			\
		cmd/sh_hardware.c		\
		cmd/sh_rtos.c			\
		cmd/sh_data.c			\
		app/ap_rpm.c				\
		app/ap_ambilcepat.c		\
		$(RTOS_SOURCE_DIR)/list.c \
		$(RTOS_SOURCE_DIR)/queue.c \
		$(RTOS_SOURCE_DIR)/tasks.c \
		$(RTOS_SOURCE_DIR)/portable/GCC/ARM7_LPC23xx/port.c \
		$(RTOS_SOURCE_DIR)/portable/MemMang/heap_2.c \
#		$(DEMO_COMMON_DIR)/BlockQ.c \
#		$(DEMO_COMMON_DIR)/integer.c \
#		$(DEMO_COMMON_DIR)/flash.c \



SERIAL_SOURCE=	\
		cmd/sh_serial.c			\
		modul/serial/serial.c 	\
		modul/serial/tinysh.c	\

ADC_SOURCE= \
		cmd/sh_adc.c		\
		modul/adc/ad7708.c	\
		app/ap_adc.c		\

RTC_SOURCE= \
		cmd/sh_rtc.c		\

SDC_SOURCE=	\
		modul/ff9a/sdc.c		\
		modul/ff9a/src/diskio.c	\
		modul/ff9a/src/ff.c		\

THUMB_SOURCE += $(SERIAL_SOURCE)
THUMB_SOURCE += $(ADC_SOURCE)
THUMB_SOURCE += $(SDC_SOURCE)
THUMB_SOURCE += $(RTC_SOURCE)
		
ADC_IRQ_SOURCE= \
		modul/adc/adcISR.c	\

ARM_SOURCE= \
		$(RTOS_SOURCE_DIR)/portable/GCC/ARM7_LPC23xx/portISR.c \
		modul/serial/serialISR.c	\
		hardware/hwISR.c			\

ARM_SOURCE += $(ADC_IRQ_SOURCE)

THUMB_OBJS = $(THUMB_SOURCE:.c=.o)
ARM_OBJS = $(ARM_SOURCE:.c=.o)

LIBS = -lc -lgcc -lm -Llib

all: RTOSDemo.bin sizebefore

RTOSDemo.bin : RTOSDemo.hex
	$(OBJCOPY) $(NAMA_FILE).elf -O binary hasil/$(NAMA_FILE).bin
	 
RTOSDemo.hex : RTOSDemo.elf
	$(OBJCOPY) $(NAMA_FILE).elf -O ihex $(NAMA_FILE).hex

RTOSDemo.elf : $(THUMB_OBJS) $(ARM_OBJS) hardware/boot.s Makefile
	$(CC) $(CFLAGS) $(ARM_OBJS) $(THUMB_OBJS) $(LIBS) hardware/boot.s $(LINKER_FLAGS)

$(THUMB_OBJS) : %.o : %.c Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) -mthumb $< -o $@

$(ARM_OBJS) : %.o : %.c Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@

clean :
	rm $(THUMB_OBJS)
	rm $(ARM_OBJS)
	rm $(NAMA_FILE).elf
	rm hasil/$(NAMA_FILE).bin
	rm $(NAMA_FILE).hex
	rm hasil/$(NAMA_FILE).map
#	touch Makefile

tulis:
	sudo $(PENULIS) -hex $(NAMA_FILE).hex /dev/ttyUSB0 115200 14748

tulis1 :
	sudo $(PENULIS) -hex $(NAMA_FILE).hex /dev/ttyUSB1 115200 14748


MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after:

HEXSIZE = $(UKURAN) --target=$(FORMAT) $(NAMA_FILE).hex
ELFSIZE = $(UKURAN) -A $(NAMA_FILE).elf
sizebefore:
	@if [ -f $(NAMA_FILE).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); echo; fi
	
	





#  Copyright (C) 2015 Joerg Hoener
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.


TCPREFIX  = ../../gcc/gcc-arm-none-eabi/
BINPREFIX = bin/arm-none-eabi-
CC      = $(TCPREFIX)$(BINPREFIX)gcc
AS      = $(TCPREFIX)$(BINPREFIX)as 
LD      = $(TCPREFIX)$(BINPREFIX)ld -v
CP      = $(TCPREFIX)$(BINPREFIX)objcopy
OD      = $(TCPREFIX)$(BINPREFIX)objdump
GDB     = $(TCPREFIX)$(BINPREFIX)gdb

STM32FLASH = tools/flash

# -mfix-cortex-m3-ldrd should be enabled by default for Cortex M3.
# CFLAGS -H show header files
AFLAGS  = -Isrc -Isrc/spl/CMSIS -Isrc/spl/inc -c -g -mcpu=cortex-m3 -mthumb 
CFLAGS  = -Isrc -Isrc/spl/CMSIS -Isrc/spl/inc -mfloat-abi=soft -msoft-float -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -c -fno-common -O0 -g -mcpu=cortex-m3 -mthumb
LFLAGS  = -Tsrc/stm32_flash.ld -L$(TCPREFIX)lib/gcc/arm-none-eabi/current -lgcc -nostartfiles
CPFLAGS = -Obinary
ODFLAGS = -S

#OBJS=$(SRCS:.c=.o)
#OBS = $(patsubst %.c, %.o, $(wildcard *.c))

SOURCES=$(shell find src -type f -iname '*.c')
#SOURCES=$(shell find src -type f -iregex '.*\.\(c\|s\)')
OBJECTS=$(foreach x, $(basename $(SOURCES)), $(x).o)


all: main.bin 


clean: 
	rm -f main.lst main.elf main.bin
	find -name *.o | xargs rm

flash: main.bin 
	$(STM32FLASH) src/main.bin

main.bin: main.elf
	@echo "...copying"
	$(CP) $(CPFLAGS) src/main.elf src/main.bin
	$(OD) $(ODFLAGS) src/main.elf> src/main.lst

main.elf: $(OBJECTS) src/startup_stm32f10x_md.o
	@echo "..linking"
	$(LD)  $^ $(LFLAGS) -o src/$@

%.o: %.c
	@echo ".compiling"
	$(CC) $(CFLAGS) $< -o $@

startup_stm32f10x_md.o:
	$(AS) $(ASFLAGS) src/startup_stm32f10x_md.s -o src/startup_stm32f10x_md.o

debug:
	$(GDB) --command=tools/debug.gdb src/main.elf


TCPREFIX = ../gcc/gcc-arm-none-eabi/bin/arm-none-eabi-
CC      = $(TCPREFIX)gcc
LD      = $(TCPREFIX)ld -v
CP      = $(TCPREFIX)objcopy
OD      = $(TCPREFIX)objdump
GDB     = $(TCPREFIX)gdb

STM32FLASH = .../tools/stm32_flash.pl

# -mfix-cortex-m3-ldrd should be enabled by default for Cortex M3.
# CFLAGS -H show header files
CFLAGS  = -Isrc -Isrc/spl/CMSIS -Isrc/spl/inc -mfloat-abi=soft -msoft-float -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -c -fno-common -O0 -g -mcpu=cortex-m3 -mthumb
LFLAGS  = -Tsrc/stm32_flash.ld -L../gcc/gcc-arm-none-eabi/lib/gcc/arm-none-eabi/current -lgcc -nostartfiles
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
	$(STM32FLASH) main.bin

main.bin: main.elf
	@echo "...copying"
	$(CP) $(CPFLAGS) src/main.elf src/main.bin
	$(OD) $(ODFLAGS) src/main.elf> src/main.lst

main.elf: $(OBJECTS) src/startup_stm32f10x_md.o
	@echo "..linking"
	$(LD)  $^ $(LFLAGS) -o src/$@

%.o: %.c
	@echo ".compiling"
	$(CC) $(CFLAGS) src/startup_stm32f10x_md.s -o src/startup_stm32f10x_md.o
	$(CC) $(CFLAGS) $< -o $@

debug:
	$(GDB) --batch --command=debug.gdb main.elf


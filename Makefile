N64_INST = /home/joeldipops/Projects/tools/n64inst
ROOTDIR = $(N64_INST)
GCCN64PREFIX = $(ROOTDIR)/bin/mips64-elf-
CHKSUM64PATH = /home/joeldipops/Projects/tools/libdragon/tools/chksum64
MKDFSPATH = /home/joeldipops/Projects/tools/libdragon/tools/mkdfs/mkdfs
HEADERPATH = $(ROOTDIR)/mips64-elf/lib
N64TOOL = /home/joeldipops/Projects/tools/libdragon/tools/n64tool
HEADERNAME = header
PROG_NAME = just_add_water

OPTIMISATION_FLAGS = -O2
CFLAGS = -std=gnu99 -march=vr4300 -mtune=vr4300 $(OPTIMISATION_FLAGS) -Wall -Wno-unused -Werror -I$(CURDIR) -I$(ROOTDIR)/mips64-elf/include 

ifdef ntsc
CFLAGS += -DIS_NTSC=1
PROG_NAME = just_add_water.ntsc
endif

LD_FILE = $(PROG_NAME).ld
LINK_FLAGS = -L$(ROOTDIR)/mips64-elf/lib -ldragon -lm -lc -ldragonsys -T./$(LD_FILE)

ASFLAGS = -mtune=vr4300 -march=vr4300
CC = $(GCCN64PREFIX)gcc
AS = $(GCCN64PREFIX)as
LD = $(GCCN64PREFIX)ld

OBJCOPY = $(GCCN64PREFIX)objcopy

ifeq ($(N64_BYTE_SWAP),true)
ROM_EXTENSION = .v64
N64_FLAGS = -b -l 2M -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME)$(ROM_EXTENSION) $(PROG_NAME).bin
else
ROM_EXTENSION = .z64
N64_FLAGS = -l 2M -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME)$(ROM_EXTENSION) $(PROG_NAME).bin
endif

all: $(PROG_NAME)$(ROM_EXTENSION)

$(PROG_NAME)$(ROM_EXTENSION): $(PROG_NAME).elf $(PROG_NAME).dfs
	$(OBJCOPY) $(PROG_NAME).elf $(PROG_NAME).bin -O binary
	rm -f $(PROG_NAME)$(ROM_EXTENSION)
	$(N64TOOL) $(N64_FLAGS) -t "$(PROG_NAME)" -s 1M $(PROG_NAME).dfs
	$(CHKSUM64PATH) $(PROG_NAME)$(ROM_EXTENSION)

LD_OFILES += $(CURDIR)/obj/weather.o
LD_OFILES += $(CURDIR)/obj/cloth.o
LD_OFILES += $(CURDIR)/obj/line.o
LD_OFILES += $(CURDIR)/obj/clothManager.o
LD_OFILES += $(CURDIR)/obj/resources.o
LD_OFILES += $(CURDIR)/obj/text.o
LD_OFILES += $(CURDIR)/obj/player.o
LD_OFILES += $(CURDIR)/obj/day.o
LD_OFILES += $(CURDIR)/obj/fps.o
LD_OFILES += $(CURDIR)/obj/animation.o
LD_OFILES += $(CURDIR)/obj/title.o
LD_OFILES += $(CURDIR)/obj/renderer.o

# Produces the disassembly, with symbols included.
$(PROG_NAME).dsm: $(PROG_NAME).elf
	mips-linux-gnu-objdump $(PROG_NAME).elf -m mips -D > $(PROG_NAME).dsm

$(PROG_NAME).elf : $(PROG_NAME).o $(LD_FILE)
	$(CC) $(CFLAGS) -c -o $(CURDIR)/obj/resources.o $(CURDIR)/resources.c
	$(CC) $(CFLAGS) -c -o $(CURDIR)/obj/text.o $(CURDIR)/text.c
	$(CC) $(CFLAGS) -c -o $(CURDIR)/obj/weather.o $(CURDIR)/weather.c
	$(CC) $(CFLAGS) -c -o $(CURDIR)/obj/cloth.o $(CURDIR)/cloth.c
	$(CC) $(CFLAGS) -c -o $(CURDIR)/obj/line.o $(CURDIR)/line.c
	$(CC) $(CFLAGS) -c -o $(CURDIR)/obj/clothManager.o $(CURDIR)/clothManager.c
	$(CC) $(CFLAGS) -c -o $(CURDIR)/obj/player.o $(CURDIR)/player.c
	$(CC) $(CFLAGS) -c -o $(CURDIR)/obj/day.o $(CURDIR)/day.c
	$(CC) $(CFLAGS) -c -o $(CURDIR)/obj/fps.o $(CURDIR)/fps.c
	$(CC) $(CFLAGS) -c -o $(CURDIR)/obj/renderer.o $(CURDIR)/renderer.c
	$(CC) $(CFLAGS) -c -o $(CURDIR)/obj/animation.o $(CURDIR)/animation.c
	$(CC) $(CFLAGS) -c -o $(CURDIR)/obj/title.o $(CURDIR)/title.c
	$(CC) $(CFLAGS) -c -o $(CURDIR)/obj/$(PROG_NAME).o $(CURDIR)/$(PROG_NAME).c

	$(LD) -o $(PROG_NAME).elf $(CURDIR)/obj/$(PROG_NAME).o $(LD_OFILES) $(LINK_FLAGS)

$(PROG_NAME).dfs: timer.sprite font.sprite sprites.sprite
	$(MKDFSPATH) $(PROG_NAME).dfs ./filesystem/

timer.sprite:
	mksprite 16 16 4 ./assets/timer.png ./filesystem/timer.sprite

font.sprite:
	mksprite 16 12 8 ./assets/font.png ./filesystem/font.sprite

%.sprite: ./assets/%.png
	mksprite 16 32 3 $^ ./filesystem/$@

# TODO
./assets/%.png:
	xcf2png $(basename $@).xcf -o $@

clean:
	rm -f *.v64 *.z64 *.elf *.o *.bin *.dfs
	rm -f ./filesystem/*
	rm -f ./obj/*.o

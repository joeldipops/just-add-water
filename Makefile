N64_INST = /home/joeldipops/Projects/tools/n64inst
ROOTDIR = $(N64_INST)
GCCN64PREFIX = $(ROOTDIR)/bin/mips64-elf-
CHKSUM64PATH = /home/joeldipops/Projects/tools/libdragon/tools/chksum64
MKDFSPATH = /home/joeldipops/Projects/tools/libdragon/tools/mkdfs/mkdfs
HEADERPATH = $(ROOTDIR)/mips64-elf/lib
N64TOOL = /home/joeldipops/Projects/tools/libdragon/tools/n64tool
HEADERNAME = header
PROG_NAME = just_add_water

LD_FILE = $(PROG_NAME).ld
LINK_FLAGS = -L$(ROOTDIR)/mips64-elf/lib -ldragon -lm -lc -ldragonsys -T./$(LD_FILE)

OPTIMISATION_FLAGS = -O3
CFLAGS = -std=gnu99 -march=vr4300 -mtune=vr4300 $(OPTIMISATION_FLAGS) -Wall -Wno-unused -Werror -I$(CURDIR) -I$(ROOTDIR)/mips64-elf/include 
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

#LD_OFILES += $(CURDIR)/obj/$(PROG_NAME).o

# Produces the disassembly, with symbols included.
$(PROG_NAME).dsm: $(PROG_NAME).elf
	mips-linux-gnu-objdump $(PROG_NAME).elf -m mips -D > $(PROG_NAME).dsm

$(PROG_NAME).elf : $(PROG_NAME).o $(LD_FILE)
	$(CC) $(CFLAGS) -c -o $(CURDIR)/obj/$(PROG_NAME).o $(CURDIR)/$(PROG_NAME).c

	$(LD) -o $(PROG_NAME).elf $(CURDIR)/obj/$(PROG_NAME).o $(LD_OFILES) $(LINK_FLAGS)

$(PROG_NAME).dfs:
	#mksprite 16 8 2 ./assets/spriteSheet.png ./filesystem/spriteSheet.sprite
	$(MKDFSPATH) $(PROG_NAME).dfs ./filesystem/

clean:
	rm -f *.v64 *.z64 *.elf *.o *.bin *.dfs $(LD_FILE)
	rm -f ./obj/*.o
	$(MAKE) -C ./rsp/ clean

##############################################################################
##
##  Makefile with automatic dependencies generation
##
##  (C) 2008, Alexey Presniakov (me@klad.me)
##
##############################################################################
SOURCES = \
	src/OS/stm32/stmlib/stm32f1xx_hal.c 		\
	src/OS/stm32/stmlib/startup_stm32f103xb.s 	\
	src/main.cpp								\
	src/lib/String.cpp							\
	src/OS/stm32/stmlib/stm32f1xx_hal_rcc.c		\
	src/OS/stm32/stmlib/stm32f1xx_hal_rcc_ex.c		\
	src/OS/stm32/stmlib/stm32f1xx_hal_gpio.c	\
	src/OS/stm32/stmlib/stm32f1xx_hal_adc.c		\
	src/OS/stm32/stmlib/stm32f1xx_hal_adc_ex.c		\
	src/OS/stm32/stmlib/stm32f1xx_hal_iwdg.c	\
	src/OS/stm32/stmlib/stm32f1xx_hal_cortex.c	\
	src/OS/stm32/startup.c						\
	src/OS/stm32/sys_fault.c					\
	src/OS/stm32/wdt.c							\
	src/OS/stm32/adc.c							\
	src/OS/stm32/pwr.c							\
	src/OS/stm32/stmlib/system_stm32f1xx.c		\
	src/OS/stm32/usb/usb_lib.cpp				\
	src/OS/stm32/usb/usb_descr.cpp				\
	src/OS/stm32/usb/usb_dev.cpp				\
	src/lib/crc32.c								\
	src/lib/crc8.c								\

	
##############################################################################

##############################################################################
##  Directories and file names
##############################################################################
OUT=AM
SRCDIR=src
OUTDIR=out
OBJDIR=.obj.stm32
DEPDIR=.dep.stm32

##############################################################################

##############################################################################

##############################################################################
##  Includes, Defines etc
##############################################################################
DEFINES+= -DSTM32F103xB '-DHSE_VALUE=16000000' -DUSE_HAL_DRIVER


INCLUDES = -Isrc/OS/stm32/ -Isrc/OS/stm32/stmlib/ -Isrc/OS/stm32/usb/
INCLUDES+= -Isrc/ -Isrc/lib/

LIBS= -lstdc++
######################################
# Настройки проекта
######################################

##############################################################################
##  Compiler information
##############################################################################
CC=arm-none-eabi-gcc
CPP=arm-none-eabi-g++
OBJCOPY=arm-none-eabi-objcopy
SIZE=arm-none-eabi-size

CPUFLAGS=-mcpu=cortex-m3 -g3
CFLAGS=$(CPUFLAGS) -Wall -Wstrict-prototypes -Wshadow -O2 \
	-fmerge-all-constants -ffunction-sections -fdata-sections \
	-fno-split-wide-types -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-math-errno -fshort-enums \
	-fno-common -flto=auto $(INCLUDES) $(DEFINES)
CPPFLAGS=$(CPUFLAGS) -Wall -Wshadow -O2 \
	-fmerge-all-constants -ffunction-sections -fdata-sections \
	-fno-tree-scev-cprop -fno-split-wide-types -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-math-errno -fshort-enums\
	-fno-common -fno-rtti -fno-exceptions -flto=auto $(INCLUDES) $(DEFINES)
LDFLAGS=$(LIBS) --specs=nano.specs --specs=nosys.specs -Tsrc/OS/stm32/stmlib/STM32F103XB_FLASH.ld -flto=auto -Wl,--relax,--gc-sections,--sort-section=alignment,--build-id=none
##############################################################################



# Target ALL
all: $(OUTDIR)/$(OUT).bin size

# Target for making Binary
$(OUTDIR)/$(OUT).bin: $(OUTDIR)/$(OUT).elf
	@echo "Making Binary Firmware..."; \
	$(OBJCOPY) -O binary $< $@

# Target for linker
$(OUTDIR)/$(OUT).elf: $(SOURCES:%=$(OBJDIR)/%.o) | $(OUTDIR)
	@echo "Linking..."; \
	$(CPP) $(CPPFLAGS) -o $@ -Wl,-Map,$(OUTDIR)/$(OUT).map $+ $(LDFLAGS)

$(OUTDIR):
	mkdir -p $@

# Target for clean
clean:
	rm -f $(OUTDIR)/$(OUT).elf $(OUTDIR)/$(OUT).map
	rm -rf $(DEPDIR)
	rm -rf $(OBJDIR)

# Target for distclean
distclean:
	rm -f $(OUTDIR)/$(OUT).elf $(OUTDIR)/$(OUT).map $(OUTDIR)/$(OUT).bin
	rm -rf $(DEPDIR)
	rm -rf $(OBJDIR)

size: $(OUTDIR)/$(OUT).elf
	@echo "Firmware size:"
	$(SIZE) $<

# PHONY
.PHONY: all clean

# Rule for compiling ASM files
$(OBJDIR)/%.s.o: %.s
	@echo "Compiling $<"; \
	mkdir -p $(dir $(DEPDIR)/$<) $(dir $(OBJDIR)/$<); \
	$(CC) $(CFLAGS) -MM -MF $(DEPDIR)/$<.d -MT $(OBJDIR)/$<.o -MP $< || rm -f $(DEPDIR)/$<.d; \
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/$<.o $<

# Rule for compiling C files
$(OBJDIR)/%.c.o: %.c
	@echo "Compiling $<"; \
	mkdir -p $(dir $(DEPDIR)/$<) $(dir $(OBJDIR)/$<); \
	$(CC) $(CFLAGS) -MM -MF $(DEPDIR)/$<.d -MT $(OBJDIR)/$<.o -MP $< || rm -f $(DEPDIR)/$<.d; \
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/$<.o $<

# Rule for compiling C++ files
$(OBJDIR)/%.cpp.o: %.cpp
	@echo "Compiling $<"; \
	mkdir -p $(dir $(DEPDIR)/$<) $(dir $(OBJDIR)/$<); \
	$(CPP) $(CPPFLAGS) -MM -MF $(DEPDIR)/$<.d -MT $(OBJDIR)/$<.o -MP $< || rm -f $(DEPDIR)/$<.d; \
	$(CPP) $(CPPFLAGS) -c -o $@ $<


# Rule for resource
$(OBJDIR)/%.o: %
	@echo "Embedding $<"; \
	mkdir -p $(dir $(OBJDIR)/$<); \
	$(LD) -r -b binary -o $@ $< && $(OBJCOPY) --rename-section .data=.rodata,alloc,load,readonly,data,contents $@


# Including dependencies infomation
-include $(SOURCES:%=$(DEPDIR)/%.d)


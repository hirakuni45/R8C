#=======================================================#
#														#
#	R8C Makefile 				        				#
#														#
#=======================================================#
TARGET		=	sd_test

BUILD		=	release

VPATH		=	../

ASOURCES	=	common/start.s

CSOURCES	=	common/vect.c \
				common/init.c \
				pfatfs/src/pff.c \
				pfatfs/src/diskio.c

PSOURCES	=	main.cpp \
				common/trb_io.cpp \
				common/adc_io.cpp

USER_LIBS	=	stdc++

LDSCRIPT	=	../M120AN/m120an.ld

USER_DEFS	=	F_CLK=20000000

MCU_TARGET	=	-mcpu=r8c

#INC_SYS		=	/usr/boost_1_53_0
INC_SYS		=

INC_APP		=	. ../ ../M120AN ../fatfs/src

OPTIMIZE	=	-O2

CP_OPT		=	-Wall -Werror \
				-Wno-unused-variable \
				-fno-exceptions

CC_OPT		=	-Wall -Werror \
				-Wno-unused-variable \
				-fno-exceptions

SYSINCS		=	$(addprefix -I, $(INC_SYS))
APPINCS		=	$(addprefix -I, $(INC_APP))
AINCS		=	$(SYSINCS) $(APPINCS)
CINCS		=	$(SYSINCS) $(APPINCS)
PINCS		=	$(SYSINCS) $(APPINCS)
LIBINCS		=	$(addprefix -L, $(LIB_ROOT))
DEFS		=	$(addprefix -D, $(USER_DEFS))
LIBS		=	$(addprefix -l, $(USER_LIBS))

# You should not have to change anything below here.
AS			=	m32c-elf-as
CC			=	m32c-elf-gcc
CP			=	m32c-elf-g++
AR			=	m32c-elf-ar
LD			=	m32c-elf-ld
OBJCOPY		=	m32c-elf-objcopy
OBJDUMP		=	m32c-elf-objdump

# AFLAGS        = -Wa,-adhlns=$(<:.s=.lst),-gstabs
# AFLAGS        =	-Wa,-adhlns=$(<:.s=.lst)
# ALL_ASFLAGS    = -x assembler-with-cpp $(ASFLAGS) $(DEFS)
ALL_ASFLAGS    = $(AFLAGS) $(MCU_TARGET) $(DEFS)

# Override is only needed by avr-lib build system.

CFLAGS		=	-std=gnu99 $(CC_OPT) $(OPTIMIZE) $(MCU_TARGET) $(DEFS)
PFLAGS		=	-std=c++0x $(CP_OPT) $(OPTIMIZE) $(MCU_TARGET) $(DEFS)
# override LDFLAGS	= $(MCU_TARGET) -nostartfiles -Wl,-Map,$(TARGET).map,-fdata-sections,-ffunction-sections,-falign-jumps,-fno-function-cse,-funit-at-a-time --select-lib=newlib -T $(LDSCRIPT)
# override LDFLAGS	= $(MCU_TARGET) -nostartfiles -Wl,-Map,$(TARGET).map,--cref,--gc-sections -T $(LDSCRIPT)

override LDFLAGS = $(MCU_TARGET) -nostartfiles -Wl,-Map,$(TARGET).map -T $(LDSCRIPT)

OBJCOPY_OPT	=	--srec-forceS3 --srec-len 32

OBJECTS	=	$(addprefix $(BUILD)/,$(patsubst %.s,%.o,$(ASOURCES))) \
			$(addprefix $(BUILD)/,$(patsubst %.c,%.o,$(CSOURCES))) \
			$(addprefix $(BUILD)/,$(patsubst %.cpp,%.o,$(PSOURCES)))

DEPENDS =   $(patsubst %.o,%.d, $(OBJECTS))

.PHONY: all clean
.SUFFIXES :
.SUFFIXES : .rc .hpp .h .c .cpp .o

all: $(BUILD) $(TARGET).elf text

$(TARGET).elf: $(OBJECTS) $(LDSCRIPT) Makefile
	$(CC) $(LDFLAGS) $(LIBINCS) -o $@ $(OBJECTS) $(LIBS)

$(BUILD)/%.o: %.s
	mkdir -p $(dir $@); \
	$(AS) -c $(AOPT) $(AFLAGS) $(AINCS) -o $@ $<

$(BUILD)/%.o : %.c
	mkdir -p $(dir $@); \
	$(CC) -c $(COPT) $(CFLAGS) $(CINCS) $(CCWARN) -o $@ $<

$(BUILD)/%.o : %.cpp
	mkdir -p $(dir $@); \
	$(CP) -c $(POPT) $(PFLAGS) $(PINCS) $(CPWARN) -o $@ $<

$(BUILD)/%.d: %.s
	mkdir -p $(dir $@); \
	$(CC) -MM -DDEPEND_ESCAPE $(AOPT) $(AFLAGS) $(APPINCS) $< \
	| sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; \
	[ -s $@ ] || rm -f $@

$(BUILD)/%.d: %.c
	mkdir -p $(dir $@); \
	$(CC) -MM -DDEPEND_ESCAPE $(COPT) $(CFLAGS) $(APPINCS) $< \
	| sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; \
	[ -s $@ ] || rm -f $@

$(BUILD)/%.d: %.cpp
	mkdir -p $(dir $@); \
	$(CP) -MM -DDEPEND_ESCAPE $(POPT) $(PFLAGS) $(APPINCS) $< \
	| sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@ ; \
	[ -s $@ ] || rm -f $@

clean:
	rm -rf $(BUILD) $(TARGET).elf

clean_depend:
	rm -f $(DEPENDS)

lst:  $(TARGET).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: mot lst

bin: $(TARGET).bin
mot: $(TARGET).mot
lst: $(TARGET).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.mot: %.elf
	$(OBJCOPY) $(OBJCOPY_OPT) -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@
#	$(OBJCOPY) -j .vects -j .text -j .data -O binary $< $@

tarball:
	tar cfvz $(subst .exe,,$(TARGET))_$(shell date +%Y%m%d%H).tgz \
	*.[hc]pp Makefile ../common/*/*.[hc]pp ../common/*/*.[hc]

bin_zip:
	$(LK) $(LFLAGS) $(LIBS) $(OBJECTS) $(ICON_OBJ) $(LIBN) -mwindows -o $(TARGET) 
	rm -f $(subst .exe,,$(TARGET))_$(shell date +%Y%m%d%H)_bin.zip
	zip $(subst .exe,,$(TARGET))_$(shell date +%Y%m%d%H)_bin.zip *.exe *.dll res/*.*

run:
	r8cprog -d R5F2M120 -P COM5 -s 57600 -e -w -v $(TARGET).mot

verify:
	r8cprog -d R5F2M120 -P COM5 -s 57600 -v $(TARGET).mot

-include $(DEPENDS)



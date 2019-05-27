
COMPILER = x86_64-w64-mingw32-gcc

# OUTPUT
DIR_BUILD = bin/
DIR_OBJECT = obj/
TARGET = BOOTX64.EFI #defined name standard in EFI
BUILDFOLDERS = . lib

#CFLAGS = -target x86_64-unknown-windows -ffreestanding -nostdlib -std=c89 -fshort-wchar -mno-red-zone
#LDFLAGS = -target x86_64-unknown-windows -nostdlib -Wl,-entry:efi_main -Wl,-subsystem:efi_application -fuse-ld=lld-link

CFLAGS = -ffreestanding
LDFLAGS = -nostdlib -Wl,-dll -shared -Wl,--subsystem,10 -e efi_main -lgcc

EFIINC = /usr/include/efi
EFIINCS = -I$(EFIINC) -I$(EFIINC)/x86_64 -I$(EFIINC)/protocol



SRC := $(foreach sdir, $(BUILDFOLDERS), $(wildcard $(sdir)/*.c))
OBJECTS := $(patsubst %.c, $(DIR_OBJECT)%.o, $(SRC))

all: $(TARGET)

$(TARGET): $(DIR_BUILD) $(OBJECTS)
	$(COMPILER) $(LDFLAGS) $(OBJECTS) -o $(DIR_BUILD)$(TARGET)

$(OBJECTS): $(DIR_OBJECT)
	@mkdir -p $(@D)
	$(COMPILER) $(CFLAGS) $(EFIINCS) -c $(patsubst $(DIR_OBJECT)%.o, %.c, $@) -o $(patsubst .$(@D)%.o, $(DIR_OBJECT)%.o, $@)


$(DIR_BUILD):
	mkdir $@

$(DIR_OBJECT):
	mkdir $@

clean:
	rm -rf $(DIR_BUILD)
	rm -rf $(DIR_OBJECT)

#COMPILER = x86_64-w64-mingw32-gcc
COMPILER = clang
LINKER = clang #lld-link

# OUTPUT
DIR_BUILD = bin/
DIR_OBJECT = obj/
TARGET = BOOTX64.EFI #defined name standard in EFI
BUILDFOLDERS = . util graphics

#You can use DWARF with COFF. To make clang-cl to emit DWARF debug info, add `-gdwarf` to clang-cl command line.
CFLAGS = -target x86_64-win32-coff -g -gdwarf -ffreestanding -fno-builtin -nostdinc -W -Wall -fno-stack-protector -fshort-wchar -mno-red-zone -Wall -Wno-macro-redefined -Wno-incompatible-library-redeclaration #-DHAVE_USE_MS_ABI  #-D__GNUC__
LDFLAGS = -target x86_64-win32-coff -g -gdwarf -ffreestanding -fno-builtin -nostdlib -disable-simplify-libcalls -dll -W -Wall -Wl,-entry:efi_main -Wl,-subsystem:efi_application -fuse-ld=lld -Wno-msvc-not-found 

EFIINC = efi
EFIINCS = -I$(EFIINC) -I$(EFIINC)/x86_64 -I$(EFIINC)/protocol

.PHONY: $(TARGET) $(OBJECTS)

SRC := $(foreach sdir, $(BUILDFOLDERS), $(wildcard $(sdir)/*.c))
OBJECTS := $(patsubst %.c, $(DIR_OBJECT)%.o, $(SRC))

all: clean $(TARGET)

$(TARGET): $(DIR_BUILD) $(OBJECTS)
	$(LINKER) $(LDFLAGS) $(OBJECTS) -o $(DIR_BUILD)$(TARGET)

$(OBJECTS): $(DIR_OBJECT)
	@mkdir -p $(@D)
	$(COMPILER) $(EFIINCS) $(CFLAGS) -c $(patsubst $(DIR_OBJECT)%.o, %.c, $@) -o $(patsubst .$(@D)%.o, $(DIR_OBJECT)%.o, $@)

$(DIR_BUILD):
	mkdir $@

$(DIR_OBJECT):
	mkdir $@

clean:
	rm -rf $(DIR_BUILD)
	rm -rf $(DIR_OBJECT)
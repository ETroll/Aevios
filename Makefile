# Ævi OS 2019 Makefile

SOURCE := src
TARGETS := bootloader/uefi kernel applications
BOOTLOADER := bootloader/uefi 
KERNEL := kernel
USERLAND := applications
DESTINATION := bin

IMAGE = bin/AeviOS.img
OMVF = toolchain/ovmf-x64

.PHONY: $(BOOTLOADER) $(KERNEL) $(USERLAND)

all: $(BOOTLOADER) #$(KERNEL)

rebuild: clean all

qemu:
	qemu-system-x86_64.exe -nographic -cpu qemu64 \
		-drive if=pflash,format=raw,unit=0,file=$(OMVF)/OVMF_CODE-pure-efi.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=$(OMVF)/OVMF_VARS-pure-efi.fd \
		-usb -drive if=none,id=stick,format=raw,file=$(IMAGE) -device nec-usb-xhci,id=xhci \
		-device usb-storage,bus=xhci.0,drive=stick -net none

debug:
	qemu-system-x86_64.exe -nographic -cpu qemu64 \
		-drive if=pflash,format=raw,unit=0,file=$(OMVF)/OVMF_CODE-pure-efi.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=$(OMVF)/OVMF_VARS-pure-efi.fd \
		-usb -drive if=none,id=stick,format=raw,file=$(IMAGE) -device nec-usb-xhci,id=xhci \
		-device usb-storage,bus=xhci.0,drive=stick -net none -s -S


$(IMAGE): $(DESTINATION)
	dd if=/dev/zero of=$@ bs=1k count=1440
	mformat -i $@ -f 1440 ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT

$(KERNEL):
	$(MAKE) -C ./$(SOURCE)/$@
	mcopy -i $(IMAGE) ./$(SOURCE)/$@/bin/* ::

$(BOOTLOADER): $(IMAGE)
	$(MAKE) -C ./$(SOURCE)/$@
	mcopy -D o -i  $(IMAGE) ./$(SOURCE)/$@/bin/*.EFI ::/EFI/BOOT

clean:
	rm -rf $(DESTINATION)
	$(MAKE) -C $(SOURCE)/$(BOOTLOADER) clean
	$(MAKE) -C $(SOURCE)/$(KERNEL) clean
	$(MAKE) -C $(SOURCE)/$(USERLAND) clean
	

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

$(DESTINATION):
	mkdir $@

# Ævi OS 2019 Makefile

SOURCE := src
TARGETS := bootloader/uefi kernel #applications
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
	qemu-system-x86_64 -cpu qemu64 \
		-drive if=pflash,format=raw,unit=0,file=$(OMVF)/OVMF_CODE-pure-efi.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=$(OMVF)/OVMF_VARS-pure-efi.fd \
		-usb -drive if=none,id=stick,format=raw,file=$(IMAGE) -device nec-usb-xhci,id=xhci \
		-device usb-storage,bus=xhci.0,drive=stick -net none -m 256 -d cpu_reset
		#-debugcon file:efi-debug.log -global isa-debugcon.iobase=0x402 
		#-serial stdio \
		#-nographic

debug: $(BOOTLOADER)
	qemu-system-x86_64 -cpu qemu64 \
		-drive if=pflash,format=raw,unit=0,file=$(OMVF)/OVMF_CODE-pure-efi.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=$(OMVF)/OVMF_VARS-pure-efi.fd \
		-usb -drive if=none,id=stick,format=raw,file=$(IMAGE) -device nec-usb-xhci,id=xhci \
		-device usb-storage,bus=xhci.0,drive=stick -net none -m 256 -d cpu_reset -s -S 
		#-debugcon file:efi-debug.log -global isa-debugcon.iobase=0x402 
		#-serial stdio \
		#-nographic

#https://support.microsoft.com/en-us/help/140365/default-cluster-size-for-ntfs-fat-and-exfat
$(IMAGE): $(DESTINATION)
	dd if=/dev/zero of=$@ bs=32M count=2
	mkfs.vfat -F 32 -S 512 -s 1 $@
	# mformat -i $@ -F ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ ./toolchain/tros.elf :: 

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
	#$(MAKE) -C $(SOURCE)/$(USERLAND) clean
	

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

$(DESTINATION):
	mkdir $@

Date: 13.04.2019

----

# Naming
Choose name: Ævi (Aevi) from norse "life". (https://en.wiktionary.org/wiki/%C3%A6vi) This fits with old naming scheme of TrOS. Website: www.aevios.org (Ævi OS)

#Initial design decisions

Have decided to create this OS for modern X86-64 systems with a focus on multiple CPU's and most modern requirements. Not support any 80's stuff like I did in TrOS. I will also from beginning create the OS to support multiple architectures. First the x86-64 and the Arm64 architectures. (For Arm64, the default arm64 implementation in Qemu will be used.)

The OS will run initialy from a USB stick or a SD card. No support for install in early development versions.

Use EFI as a minimal hardware abstraction layer

# Tools

 - Qemu (3.1.0)
 - VS Code
 - LLVM (Want to distance myself from GCC and GNU as much as I can)

 # Links

 - https://wiki.osdev.org/UEFI_Bare_Bones
 - https://wiki.osdev.org/UEFI#Emulation_with_QEMU_and_OVMF
 - https://www.kraxel.org/repos/jenkins/edk2/
 - https://fabianlee.org/2018/09/12/kvm-building-the-latest-ovmf-firmware-for-virtual-machines/
 - https://dvdhrm.github.io/2019/01/31/goodbye-gnuefi/
 - https://clang.llvm.org/docs/CrossCompilation.html
 - https://uefi.org/specifications

#Bootloader

Have decided to create a bootloader instead of using existing bootloaders. This is for learning purposes mostly, but also because most bootloaders available have some sort of GPL-like license. I want a MIT based license on my system. I will create a UEFI bootloader to put the machine in the desired state. Previously I wanted to be multiboot compatable. But for this iteration I will skip this.

The desiered state the bootloader puts the system in is as follows:
 - Kernel loaded into memory at <Address>
 - Kernel Modules and drivers loaded into memory at <Address>
 - Set up display or serial device
 - The following are passed to kernel-main:
    - Addresses of kernel itself, the modules loaded and the size of occupied space
    - Hardware information / System information
    - Address to display device or serial device (depending on availablility)

## Getting started creating the bootloader for x86-64

First I need a cross compiler. This compiler needs to compile to a PE binary format. So lets choose to set up GCC x86_64-w64-mingw32. 

The LLVM toolchain (clang compiler plus lld linker) combines all supported targets in a single toolchain, offering a target selector --target to let LLVM know what to compile for. So as long as you have clang and lld installed, you can compile native UEFI binaries just like normal local compilation:

We also need a copy of the x64 OVMF firmware. A copy can be found here: https://www.kraxel.org/repos/jenkins/edk2/ choose the edk2.git-ovmf-x64-0-<date and commit>.noarch.rpm and you can find the OMVF file located at \usr\share\edk2.git\ inside the RPM. Move the ovmf-x64 directory to a known location. (like toochains folder in project)

``` Bash
# First, lets get the cross compiler. No need to build this ourselves.
sudo apt-get install binutils-mingw-w64 gcc-mingw-w64 gnu-efi mtools
```

``` Bash
# Using clang
sudo apt-get install mtools clang lld gnu-efi 
```

Setting up Qemu interop for WSL. Instead of running Qemu in WSL and host a xserver on Windows like I have done before, I have decided to try out using the new interop functionality for WSL. So first I need to add the qemu exe files to $PATH for convenience.

##### ~/.bash_profile

``` Shell
PATH="/mnt/c/Program Files/qemu/":$PATH
```

### Testing the emulator

``` Bash
qemu-system-x86_64.exe -L toolchain/ovmf-x64/ -bios OVMF-pure-efi.fd
```
Expected result:

![UEFI Shell](images/Uefi-shell.png)

### Building the hello world

I tried using clang and ran into troubles. Most likely because of my lack of knowledge using the clang compiler. Even though I set -ffreestanding -nostdlib the variable `__STDC_VERSION__` was still set, and some typedefs that would define the types needed later was never called. So I went with `x86_64-w64-mingw32-gcc` for now. Still want to move over to pure llvm based tools later.

> To avoid bloat and unnecessary dependencies on the rest of gnu-efi, you will need to edit it to remove the references to 'LibStubStriCmp', 'LibStubMetaiMatch' and 'LibStubStrLwrUpr' (simply make all the members of the LibStubUnicodeInterface structure be NULL). 

```
obj/lib/data.o:data.c:(.data+0x20): undefined reference to `LibStubStriCmp'
obj/lib/data.o:data.c:(.data+0x28): undefined reference to `LibStubMetaiMatch'
obj/lib/data.o:data.c:(.data+0x30): undefined reference to `LibStubStrLwrUpr'
obj/lib/data.o:data.c:(.data+0x38): undefined reference to `LibStubStrLwrUpr'
```

Creating a disk image:

``` Bash
dd if=/dev/zero of=fat.img bs=1k count=1440
mformat -i fat.img -f 1440 ::
mmd -i fat.img ::/EFI
mmd -i fat.img ::/EFI/BOOT
mcopy -i fat.img BOOTX64.EFI ::/EFI/BOOT
```

Setting up qemu to use the **new** way of using OVMF for the `pflash` parameter and a USB drive containing our image.

``` Bash
qemu-system-x86_64.exe -nographic -cpu qemu64 \
-drive if=pflash,format=raw,unit=0,file=toolchain/ovmf-x64/OVMF_CODE-pure-efi.fd,readonly=on \
-drive if=pflash,format=raw,unit=1,file=toolchain/ovmf-x64/OVMF_VARS-pure-efi.fd \
-usb -drive if=none,id=stick,format=raw,file=fat.img -device nec-usb-xhci,id=xhci \
-device usb-storage,bus=xhci.0,drive=stick -net none
```





https://www.developerfiles.com/debugging-c-with-clang-compiler-and-lldb/

https://github.com/yoppeh/efi-clang

http://www.rodsbooks.com/efi-programming/index.html


https://iweb.dl.sourceforge.net/project/gnu-efi/gnu-efi-3.0.12.tar.bz2

tar xvjf gnu-efi-3.0.12.tar.bz2 


https://dvdhrm.github.io/2019/01/31/goodbye-gnuefi/


https://uefi.org/sites/default/files/resources/ACPI_6_3_May16.pdf


cat efi-debug.log | grep Loading | grep -i BOOTX64
> Loading driver at 0x0000693C000 EntryPoint=0x0000693F000 BOOTX64.efi

add-symbol-file ${workspaceFolder}/src/bootloader/uefi/bin/BOOTX64.EFI 0x0000693F000


https://github.com/vathpela/gnu-efi/
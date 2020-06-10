#ifndef INCLUDE_BOOTGRAPHICS_H
#define INCLUDE_BOOTGRAPHICS_H

#include "../util/util.h"
#include "../efi/efi.h"
#include "../efi/protocol/efi-stop.h"
#include "../efi/protocol/efi-gop.h"



EFI_GRAPHICS_OUTPUT_PROTOCOL* efi_graphics_getOutput(EFI_BOOT_SERVICES* bs, EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *console);








#endif
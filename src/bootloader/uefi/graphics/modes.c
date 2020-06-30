#include "graphics.h"



static void _fill_boxes(UINT32 *pixelBuffer, UINT32 width, UINT32 height)
{
    UINT32 y, x = 0;
    //Assume BGRR - But check for BRGR
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL white = {0xff, 0xff, 0xff, 0},
                                  black = {0, 0 , 0, 0},
                                  *color;

    for (y = 0; y < height; y++) {
        color = ((y / 32) % 2 == 0) ? &white : &black;
        for (x = 0; x < width; x++) {
            if (x % 32 == 0 && x != 0) {
                color = (color == &white) ? &black : &white;
            }
            pixelBuffer[y * width + x] = *(UINT32 *)color;
        }
    }
}

static void _draw_boxes(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop, EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *console, EFI_BOOT_SERVICES* bs)
{
    int i, imax;
    EFI_STATUS rc;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN numPixels;
    UINT32 *pixelBuffer;
    UINT32 bufferSize;

    if (gop->Mode) {
        imax = gop->Mode->MaxMode;
    } else {
        efi_util_printf(console, L"gop->Mode is NULL\n");
        return;
    }

    for (i = 0; i < imax; i++) {
        UINTN sizeOfInfo;

        rc = gop->QueryMode(gop, i, &sizeOfInfo, &info);
        if (EFI_ERROR(rc) && rc == EFI_NOT_STARTED) {
            efi_util_printf(console, L"gop->QueryMode() returned %r\n", rc);
            efi_util_printf(console, L"Trying to start GOP with SetMode().\n");
            rc = gop->SetMode(gop, gop->Mode ? gop->Mode->Mode : 0);
            rc = gop->QueryMode(gop, i, &sizeOfInfo, &info);
        }

        if (EFI_ERROR(rc)) {
            efi_util_printf(console, L"%d: Bad response from QueryMode: %r (%d)\n", i, rc, rc);
            continue;
        }

        if (CompareMem(info, gop->Mode->Info, sizeof (*info))) {
            continue;
        }

        efi_util_printf(console, L"Found graphics mode %d with resolution %dx%d\r\n", i, info->HorizontalResolution, info->VerticalResolution);

        numPixels = info->VerticalResolution * info->HorizontalResolution;
        bufferSize = numPixels * sizeof(UINT32);

        bs->AllocatePool(EfiLoaderData, bufferSize, (void**)&pixelBuffer);
        
        if (!pixelBuffer) {
            efi_util_printf(console, L"Allocation of 0x%08lx bytes failed.\n", sizeof(UINT32) * numPixels);
            return;
        }

        _fill_boxes(pixelBuffer, info->HorizontalResolution, info->VerticalResolution);

        gop->Blt(gop,
                (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)pixelBuffer,
                EfiBltBufferToVideo,
                0, 0, 0, 0,
                info->HorizontalResolution,
                info->VerticalResolution,
                0);
        return;
    }
    efi_util_printf(console, L"Never found the active video mode?\n");
}




EFI_GRAPHICS_OUTPUT_PROTOCOL* efi_graphics_getOutputProtocol(EFI_BOOT_SERVICES* bs, EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *console) 
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL *output;
    EFI_GUID graphicsProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

    EFI_STATUS status = bs->LocateProtocol(&graphicsProtocolGuid, NULL, (void **)&output);

    if(!EFI_ERROR(status))
    {
        _draw_boxes(output, console, bs);
        return output;
    }
    else
    {
        CHAR16 tmp[100];
        efi_util_statusToString(tmp, status);
        efi_util_printf(console, L"Error locating Graphics output. Status: %lX - %s\r\n", status, tmp);
    }
    return NULL;
}
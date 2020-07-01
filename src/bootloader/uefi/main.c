#include "efi/efi.h"
#include "efi/protocol/efi-lip.h"
#include "efi/protocol/efi-gop.h"

#include "util/util.h"
#include "graphics/graphics.h"

 
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    EFI_INPUT_KEY Key;
    CHAR16 tempBuffer[100];
 
    /* Store the system table for future use in other functions */
    EFI_SYSTEM_TABLE *ST = SystemTable;

    ST->ConOut->ClearScreen(ST->ConOut);
 
    /* Say hi */
    Status = ST->ConOut->OutputString(ST->ConOut, L"Hello World\n\r");
    if (EFI_ERROR(Status)) 
    {
        return Status;
    }

    EFI_LOADED_IMAGE_PROTOCOL *loaded_image = 0;
    EFI_GUID loadedImageGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

    Status = ST->BootServices->HandleProtocol(ImageHandle, &loadedImageGuid, (void **)&loaded_image);

    // Status = ST->ConOut->OutputString(ST->ConOut, loaded_image->ImageBase);
    
    if (EFI_ERROR(Status)) 
    {
        ST->ConOut->OutputString(ST->ConOut, L"Error loading image base\n\r");
    }
    else 
    {
        ST->ConOut->OutputString(ST->ConOut, L"Loaded image base\n\r");
    }
    // efiprint(ST->ConOut, "Test test\n\r");

    efi_util_printf(ST->ConOut, L"Image base at %H0x%lX%N\n\r", loaded_image->ImageBase);

    UINTN MemMapSize = 0;
    UINTN MemMapKey, MemMapDescriptorSize = NULL;
    UINT32 MemMapDescriptorVersion;
    EFI_MEMORY_DESCRIPTOR *MemMap = NULL;

    EFI_STATUS memstatus = ST->BootServices->GetMemoryMap(&MemMapSize, MemMap, &MemMapKey, &MemMapDescriptorSize, &MemMapDescriptorVersion);

    efi_util_statusToString(tempBuffer, memstatus);

    efi_util_printf(ST->ConOut,L"memstatus: %H0x%lX%N - %s\n\r", memstatus, tempBuffer);
    if(memstatus == EFI_BUFFER_TOO_SMALL)
    {
        efi_util_printf(ST->ConOut, L"MemMap buffer was too small. MemMapSize = %ld, MemMapDescriptorSize = %ld\n\r", MemMapSize, MemMapDescriptorSize);
        MemMapSize += MemMapDescriptorSize;
        memstatus = ST->BootServices->AllocatePool(EfiLoaderData, MemMapSize, (void **)&MemMap); // Allocate pool for MemMap (it should always be resident in memory)
        if(EFI_ERROR(memstatus))
        {
            efi_util_printf(ST->ConOut, L"MemMap AllocatePool error. 0x%lX\n\r", memstatus);
            return memstatus;
        }
        memstatus = ST->BootServices->GetMemoryMap(&MemMapSize, MemMap, &MemMapKey, &MemMapDescriptorSize, &MemMapDescriptorVersion);
    }

    efi_util_printf(ST->ConOut, L"MemMap - MemMapSize = %ld, MemMapDescriptorSize = %ld\n\r", MemMapSize, MemMapDescriptorSize);
    efi_util_printf(ST->ConOut,L"Num: Type, PhysicalStart, VirtualStart, NumberOfPages, Attribute\n\r");
    //for(int i = 0; i<=(MemMapSize/MemMapDescriptorSize); i++) {
    for(int i = 0; i<=10; i++) {
        efi_util_printf(ST->ConOut, L"%d: %lX 0x%lX 0x%lX %lu %lu\n\r", i, MemMap[i].Type, MemMap[i].PhysicalStart, MemMap[i].VirtualStart, MemMap[i].NumberOfPages, MemMap[i].Attribute);
    }

    // EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = efi_graphics_getOutputProtocol(ST->BootServices, ST->ConOut);
    // if(gop == NULL) {
    //     efi_util_printf(ST->ConOut, L"Error getting graphics output\n\r");
    // }

    ui_context* ui_ctx = efi_ui_context_new(ST);

    efi_ui_window_new(ST, ui_ctx, 10, 10, 300, 200);
    efi_ui_window_new(ST, ui_ctx, 100, 150, 400, 400);
    efi_ui_window_new(ST, ui_ctx, 200, 100, 200, 600);

    efi_ui_context_paint(ui_ctx);

    efi_util_readKey(ST->ConIn, &Key);
 
    return Status;
}
#include "efi/efi.h"
#include "efi/protocol/efi-lip.h"
#include "efi/protocol/efi-spp.h"

#include "util/util.h"
#include "graphics/graphics.h"

 
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    EFI_INPUT_KEY Key;
    // CHAR16 tempBuffer[100];
 
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

    efi_util_printf(ST->ConOut,L"memstatus: %H0x%lX%N - %r\n\r", memstatus, memstatus);
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

    //
    // UI and main Bootloader application here
    //

    ui_context* ui_ctx = efi_ui_context_new(ST);

    efi_ui_window_new(ST, ui_ctx, 10, 10, 300, 200);
    efi_ui_window_new(ST, ui_ctx, 100, 150, 400, 400);
    efi_ui_window_new(ST, ui_ctx, 200, 100, 200, 600);

    efi_ui_context_paint(ui_ctx);

    UINTN eventindex;
    EFI_EVENT timerEvent;
    EFI_EVENT waitList[2];
    UINT64 timeout = 1000000; //10ms

    // Status = efi_util_connectAllHandles(ST);
    // efi_util_printf(ST->ConOut,L"Connect status: %r\n\r", Status);

    // return Status;

    Status = ST->BootServices->CreateEventEx(EVT_TIMER, 0, NULL, NULL, NULL, &timerEvent);
    if (!EFI_ERROR(Status)) 
    {
        ST->BootServices->SetTimer(timerEvent, TimerPeriodic, timeout);

        // Find the simple pointer protocol and hook up to the events

        EFI_GUID simplePointerProtocolGuid = EFI_SIMPLE_POINTER_PROTOCOL_GUID;
        EFI_SIMPLE_POINTER_PROTOCOL *spp;


        EFI_HANDLE* handle_buffer;
        UINTN handle_count = 0;
        ST->BootServices->LocateHandleBuffer(ByProtocol,
            &simplePointerProtocolGuid,
            NULL,
            &handle_count,
            &handle_buffer);
        efi_util_printf(ST->ConOut,L"Found %ld mice connected\n\r", handle_count);

        // Status = ST->BootServices->LocateProtocol(&simplePointerProtocolGuid, NULL, (void **)&spp);
        Status = ST->BootServices->HandleProtocol( handle_buffer[0], &simplePointerProtocolGuid, (void **)&spp );
        if(!EFI_ERROR(Status))
        {
            // waitList[0] = Event; //Change to mouse event for now
            waitList[0] = spp->WaitForInput;
            waitList[1] = ST->ConIn->WaitForKey;
            // waitList[1] = timerEvent;

            efi_util_printf(ST->ConOut, L"Mouse - ResX %ld - ResY %ld - ResZ: %ld - LeftBtn: %d - RightBtn: %d\n\r", 
                spp->Mode->ResolutionX,
                spp->Mode->ResolutionY,
                spp->Mode->ResolutionZ,
                spp->Mode->LeftButton,
                spp->Mode->RightButton);

            EFI_SIMPLE_POINTER_STATE ps;
            Status = spp->GetState(spp, &ps);

            efi_util_printf(ST->ConOut,L"Mouse status: %r\n\r", Status);

            // Empty the console input buffer
            ST->ConIn->Reset(ST->ConIn, EFI_FALSE);

            while(TRUE)
            {
                Status = ST->BootServices->WaitForEvent(2, waitList, &eventindex);
                if (EFI_ERROR(Status)) 
                {
                    efi_util_printf(ST->ConOut,L"Error in main event loop: %r\n\r", Status);
                }
                else 
                {
                    if(eventindex == 1)
                    {
                        ST->ConIn->ReadKeyStroke(ST->ConIn, &Key);
                        ST->ConIn->Reset(ST->ConIn, EFI_FALSE);
                        efi_util_printf(ST->ConOut, L"%c", Key.UnicodeChar);
                    }
                    else 
                    {
                        efi_util_printf(ST->ConOut, L"Event for index %d\n\r", eventindex);
                    }
                }
            }
        }
        efi_util_printf(ST->ConOut, L"Could not set up Simple Pointer Protocol\n\r");
        ST->BootServices->CloseEvent(timerEvent);
    }

    efi_util_printf(ST->ConOut,L"Error setting up event loop: %H0x%lX%N - %r\n\r", Status, Status);

    efi_util_readKey(ST->ConIn, &Key);
 
    return Status;
}
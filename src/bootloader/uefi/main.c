#include "efi/efi.h"
#include "efi/protocol/efi-lip.h"

#include "util/util.h"


 
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    EFI_INPUT_KEY Key;
 
    /* Store the system table for future use in other functions */
    EFI_SYSTEM_TABLE *ST = SystemTable;

    ST->ConOut->ClearScreen(ST->ConOut);
 
    /* Say hi */
    Status = ST->ConOut->OutputString(ST->ConOut, L"Hello World3\n\r");
    if (EFI_ERROR(Status)) 
    {
        return Status;
    }

    EFI_LOADED_IMAGE_PROTOCOL *loaded_image = 0;
    EFI_GUID loadedImageGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

    Status = SystemTable->BootServices->HandleProtocol(ImageHandle, &loadedImageGuid, (void **)&loaded_image);

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

    Print(ST->ConOut, L"Image base at %H0x%x%N\n\r", loaded_image->ImageBase);

    UINTN MemMapSize, MemMapKey, MemMapDescriptorSize = 0;
    UINT32 MemMapDescriptorVersion;
    EFI_MEMORY_DESCRIPTOR *MemMap = NULL;

    EFI_STATUS memstatus = ST->BootServices->GetMemoryMap(&MemMapSize, MemMap, &MemMapKey, &MemMapDescriptorSize, &MemMapDescriptorVersion);
    if(memstatus == EFI_BUFFER_TOO_SMALL)
    {
        Print(ST->ConOut, L"MemMap buffer was too small. MemMapSize = %d, MemMapDescriptorSize = %d\n\r", MemMapSize, MemMapDescriptorSize);
        MemMapSize += MemMapDescriptorSize;
        memstatus = ST->BootServices->AllocatePool(EfiLoaderData, MemMapSize, (void **)&MemMap); // Allocate pool for MemMap (it should always be resident in memory)
        if(EFI_ERROR(memstatus))
        {
            Print(ST->ConOut, L"MemMap AllocatePool error. 0x%llx\n\r", memstatus);
            return memstatus;
        }
        memstatus = ST->BootServices->GetMemoryMap(&MemMapSize, MemMap, &MemMapKey, &MemMapDescriptorSize, &MemMapDescriptorVersion);
    }
    Print(ST->ConOut, L"MemMap - MemMapSize = %d, MemMapDescriptorSize = %d\n\r", MemMapSize, MemMapDescriptorSize);
    Print(ST->ConOut,L"Num: Type, PhysicalStart, VirtualStart, NumberOfPages, Attribute\n\r");
    // for(int i = 0; i<=(MemMapSize/MemMapDescriptorSize); i++) {
    for(int i = 0; i<=20; i++) {
        Print(ST->ConOut, L"%d: %u 0x%x 0x%x %u %u\n\r", i, MemMap[i].Type, MemMap[i].PhysicalStart, MemMap[i].VirtualStart, MemMap[i].NumberOfPages, MemMap[i].Attribute);
    }


    /* Now wait for a keystroke before continuing, otherwise your
       message will flash off the screen before you see it.
 
       First, we need to empty the console input buffer to flush
       out any keystrokes entered before this point */
    Status = ST->ConIn->Reset(ST->ConIn, EFI_FALSE);
    if (EFI_ERROR(Status)) {
        return Status;
    }
        
 
    /* Now wait until a key becomes available.  This is a simple
       polling implementation.  You could try and use the WaitForKey
       event instead if you like */
    while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY) ;
 
    return Status;
}
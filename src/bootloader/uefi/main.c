#include "efi/efi.h"
#include "efi/protocol/efi-lip.h"

#include "util/util.h"

#define FALSE 0
#define EFI_ERROR(a) (((INT64) a) < 0)
 
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

    Print(ST->ConOut, L"Hello World in unicode. Image base at %x\n\r", loaded_image->ImageBase);


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
#include "util.h"

EFI_STATUS efi_util_readKey(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *console, EFI_INPUT_KEY *key) 
{
    EFI_STATUS Status;

    /* Now wait for a keystroke before continuing, otherwise your
    message will flash off the screen before you see it.

    First, we need to empty the console input buffer to flush
    out any keystrokes entered before this point */
    Status = console->Reset(console, EFI_FALSE);
    if (EFI_ERROR(Status)) {
        return Status;
    }
        
 
    /* Now wait until a key becomes available.  This is a simple
       polling implementation.  You could try and use the WaitForKey
       event instead if you like */
    while ((Status = console->ReadKeyStroke(console, key)) == EFI_NOT_READY);
    return Status;
}
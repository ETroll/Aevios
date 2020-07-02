#include "util.h"

EFI_STATUS efi_util_connectAllHandles(EFI_SYSTEM_TABLE* ST)
{
    //
    // Connect All Handles Example
    //  The following example recursively connects all controllers in a platform.
    //
    EFI_STATUS           Status;
    EFI_BOOT_SERVICES    *gBS = ST->BootServices;
    UINTN                HandleCount;
    EFI_HANDLE           *HandleBuffer;
    UINTN                HandleIndex;

    //
    // Retrieve the list of all handles from the handle database
    //
    Status = gBS->LocateHandleBuffer (
        AllHandles,
        NULL,
        NULL,
        &HandleCount,
        &HandleBuffer);
    efi_util_printf(ST->ConOut,L"LocateHandleBuffer status: %r\n\r", Status);

        if (!EFI_ERROR (Status)) {
            for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
                Status = gBS->ConnectController (
                    HandleBuffer[HandleIndex],
                    NULL,
                    NULL,
                    TRUE
                    );
                efi_util_printf(ST->ConOut,L"ConnectController status: %r\n\r", Status);
            }
            gBS->FreePool(HandleBuffer);
        }
    return Status;
}

EFI_STATUS efi_util_connectMinimumHandles(EFI_SYSTEM_TABLE* ST)
{
    //
    // Connect Device Path Example
    //  The following example walks the device path nodes of a device path, and
    //  connects only the drivers required to force a handle with that device path
    //  to be present in the handle database. This algorithms guarantees that
    //  only the minimum number of devices and drivers are initialized.
    //
    EFI_STATUS               Status;
    EFI_BOOT_SERVICES        *gBS = ST->BootServices;
    EFI_DEVICE_PATH_PROTOCOL *DevicePath = NULL;
    EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath;
    EFI_HANDLE               Handle;
    EFI_GUID gEfiDevicePathProtocolGuid = EFI_DEVICE_PATH_PROTOCOL_GUID;

    do { 
        //
        // Find the handle that best matches the Device Path. If it is only a
        // partial match the remaining part of the device path is returned in
        // RemainingDevicePath.
        // 
        RemainingDevicePath = DevicePath; 
        Status = gBS->LocateDevicePath (
            &gEfiDevicePathProtocolGuid,
            &RemainingDevicePath,
            &Handle);
        efi_util_printf(ST->ConOut,L"LocateDevicePath status: %r\n\r", Status);
        
        if (EFI_ERROR(Status)) { 
            return EFI_NOT_FOUND; 
        }
        //
        // Connect all drivers that apply to Handle and RemainingDevicePath
        // If no drivers are connected Handle, then return EFI_NOT_FOUND
        // The Recursive flag is FALSE so only one level will be expanded.
        //
        Status = gBS->ConnectController (
            Handle,
            NULL,
            RemainingDevicePath,
            FALSE
            );
        efi_util_printf(ST->ConOut,L"ConnectController status: %r\n\r", Status);
        
        if (EFI_ERROR(Status)) {
            return EFI_NOT_FOUND;
        }
        
        //
        // Loop until RemainingDevicePath is an empty device path 
        //
        
    } while (!IsDevicePathEnd (RemainingDevicePath));
    
    //
    // A handle with DevicePath exists in the handle database
    //
    return EFI_SUCCESS;

}
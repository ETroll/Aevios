/*
 * efi-spp.h
 *
 * UEFI simple pointer protocol.
 */


#ifndef __EFI_SPP_H
#define __EFI_SPP_H

#include <efi.h>

#define EFI_SIMPLE_POINTER_PROTOCOL_GUID    {0x31878c87, 0xb75, 0x11d5, {0x9a, 0x4f, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}}

typedef struct {
    INT32 RelativeMovementX;
    INT32 RelativeMovementY;
    INT32 RelativeMovementZ;
    BOOLEAN LeftButton;
    BOOLEAN RightButton;
} EFI_SIMPLE_POINTER_STATE;

typedef struct {
    UINT64 ResolutionX;
    UINT64 ResolutionY;
    UINT64 ResolutionZ;
    BOOLEAN LeftButton;
    BOOLEAN RightButton;
} EFI_SIMPLE_POINTER_MODE;

struct EFI_SIMPLE_POINTER_PROTOCOL;

typedef EFI_STATUS (*EFI_SIMPLE_POINTER_RESET)(struct EFI_SIMPLE_POINTER_PROTOCOL *This, BOOLEAN ExtendedVerification);
typedef EFI_STATUS (*EFI_SIMPLE_POINTER_GET_STATE)(struct EFI_SIMPLE_POINTER_PROTOCOL *This, EFI_SIMPLE_POINTER_STATE *State);

typedef struct EFI_SIMPLE_POINTER_PROTOCOL {
	EFI_SIMPLE_POINTER_RESET Reset;
	EFI_SIMPLE_POINTER_GET_STATE GetState;
	EFI_EVENT WaitForInput;
	EFI_SIMPLE_POINTER_MODE *Mode;
} EFI_SIMPLE_POINTER_PROTOCOL;

#endif
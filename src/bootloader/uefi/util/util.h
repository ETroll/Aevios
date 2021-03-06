#ifndef INCLUDE_BOOTUTIL_H
#define INCLUDE_BOOTUTIL_H

#include "../efi/efi.h"
#include "../efi/protocol/efi-stop.h"

// typedef int* va_list;

// #define va_start(ap, v) (ap = (va_list) &v)
// #define va_end(ap) ((void) (ap = 0))
// #define va_arg(ap) (*++(ap))

// static int32_t _fltused = 0;


#define EFI_DP_TYPE_MASK                    0x7F
#define EFI_DP_TYPE_UNPACKED                0x80

#define END_DEVICE_PATH_TYPE                0x7f

#define END_ENTIRE_DEVICE_PATH_SUBTYPE      0xff
#define END_INSTANCE_DEVICE_PATH_SUBTYPE    0x01
#define END_DEVICE_PATH_LENGTH              (sizeof(EFI_DEVICE_PATH_PROTOCOL))

#define DevicePathType(a)           ( ((a)->Type) & EFI_DP_TYPE_MASK )
#define DevicePathSubType(a)        ( (a)->SubType )
#define DevicePathNodeLength(a)     ( ((a)->Length[0]) | ((a)->Length[1] << 8) )
#define NextDevicePathNode(a)       ( (EFI_DEVICE_PATH_PROTOCOL *) ( ((UINT8 *) (a)) + DevicePathNodeLength(a)))
#define IsDevicePathEndType(a)      ( DevicePathType(a) == END_DEVICE_PATH_TYPE )
#define IsDevicePathEndSubType(a)   ( (a)->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE )
#define IsDevicePathEnd(a)          ( IsDevicePathEndType(a) && IsDevicePathEndSubType(a) )
#define IsDevicePathUnpacked(a)     ( (a)->Type & EFI_DP_TYPE_UNPACKED )


typedef __builtin_va_list va_list;

#define va_start(v,l)	__builtin_va_start(v,l)
#define va_end(v)	__builtin_va_end(v)
#define va_arg(v,l)	__builtin_va_arg(v,l)
#define va_copy(d,s)	__builtin_va_copy(d,s)

#define EFI_ERROR(a) (((INT64) a) < 0)

#ifndef NULL
#define NULL	0
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif


typedef struct listnode_struct 
{  
    void* data;
    struct listnode_struct* prev;
    struct listnode_struct* next;
} util_listnode;

typedef struct list_struct 
{
    unsigned int count;
    util_listnode* root;
} util_list;



util_list* efi_util_list_new(EFI_SYSTEM_TABLE* ST);
void efi_util_list_delete(EFI_SYSTEM_TABLE* ST, util_list* list);
int efi_util_list_add(EFI_SYSTEM_TABLE* ST, util_list* list, void* data);
void* efi_util_list_getAt(util_list* list, unsigned int index);

util_listnode* efi_util_listnode_new(EFI_SYSTEM_TABLE* ST, void* data);
void efi_util_listnode_delete(EFI_SYSTEM_TABLE* ST, util_listnode* node);



// void efiprint(struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *TOP, char* str, ...);

void ZeroMem (void *Buffer, UINTN Size);
void SetMem (void *Buffer, UINTN Size, UINT8 Value);
void CopyMem (void *Dest, const void *Src, UINTN len);
INTN CompareMem (const void *Dest, const void *Src, UINTN len);


INTN StrCmp (const CHAR16 *s1, const CHAR16 *s2);
INTN StrnCmp (const CHAR16 *s1, const CHAR16 *s2, UINTN len);
void StrCpy (CHAR16 *Dest, const CHAR16 *Src);
void StrnCpy (CHAR16 *Dest, const CHAR16 *Src, UINTN Len);
CHAR16* StpCpy (CHAR16 *Dest, const CHAR16 *Src);
CHAR16* StpnCpy (CHAR16 *Dest, const CHAR16 *Src, UINTN Len);
void StrCat (CHAR16 *Dest, const CHAR16 *Src);
void StrnCat (CHAR16 *Dest, const CHAR16 *Src, UINTN Len);
UINTN StrnLen (const CHAR16 *s1, UINTN Len);
UINTN StrLen (const CHAR16 *s1);
UINTN StrSize (const CHAR16 *s1);
UINTN strlena (const CHAR8 *s1);
UINTN strcmpa (const CHAR8 *s1, const CHAR8 *s2);
UINTN strncmpa (const CHAR8 *s1, const CHAR8 *s2, UINTN len);
UINTN xtoi (const CHAR16 *str);
UINTN Atoi (const CHAR16 *str);
BOOLEAN MetaMatch (CHAR16 *String, CHAR16 *Pattern);

void efi_util_statusToString (CHAR16 *Buffer, EFI_STATUS Status);

UINTN efi_util_printf (struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *TOP, CONST CHAR16 *fmt, ...);
UINTN efi_util_sprintf (CHAR16 *Str, UINTN StrSize, const CHAR16 *fmt, ...);

EFI_STATUS efi_util_readKey(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *console, EFI_INPUT_KEY *key);

EFI_STATUS efi_util_connectAllHandles(EFI_SYSTEM_TABLE* ST);
EFI_STATUS efi_util_connectMinimumHandles(EFI_SYSTEM_TABLE* ST);


#endif

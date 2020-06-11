#include "util.h"

// C Compiler in freestanding mode requires the existence of the symbols memset, memcpy, memmove and memcmp. 
// We *must* supply these functions yourself as described in the C standard.
void* memset(void* ptr, UINT8 value, UINTN len ) {
    SetMem(ptr, len, value);
    return ptr;
}

void ZeroMem (void *Buffer, UINTN Size)
{
    INT8 *pt;

    pt = Buffer;
    while (Size--) {
        *(pt++) = 0;
    }
}

void SetMem (void *Buffer, UINTN Size, UINT8 Value)
{
    INT8 *pt;

    pt = Buffer;
    while (Size--) {
        *(pt++) = Value;
    }
}


void CopyMem (void *Dest, const void *Src, UINTN len)
{
    CHAR8 *d;
    const CHAR8 *s = Src;
    d = Dest;
    while (len--) {
        *(d++) = *(s++);
    }
}

INTN CompareMem (const void *Dest, const void *Src, UINTN len)
{
    const CHAR8 *d = Dest, *s = Src;
    while (len--) {
        if (*d != *s) {
            return *d - *s;
        }

        d += 1;
        s += 1;
    }

    return 0;
}
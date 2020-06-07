#include "util.h"

INTN StrCmp (const CHAR16 *s1, const CHAR16 *s2)
{
    while (*s1) {
        if (*s1 != *s2) {
            break;
        }

        s1 += 1;
        s2 += 1;
    }

    return *s1 - *s2;
}

INTN StrnCmp (const CHAR16 *s1, const CHAR16 *s2, UINTN len)
{
    while (*s1  &&  len) {
        if (*s1 != *s2) {
            break;
        }

        s1  += 1;
        s2  += 1;
        len -= 1;
    }

    return len ? *s1 - *s2 : 0;
}

void StrCpy (CHAR16 *Dest, const CHAR16 *Src)
{
    while (*Src) {
        *(Dest++) = *(Src++);
    }
    *Dest = 0;
}

void StrnCpy (CHAR16 *Dest, const CHAR16 *Src, UINTN Len)
{
    UINTN Size = StrnLen(Src, Len);
    if (Size != Len) {
        SetMem(Dest + Size, (Len - Size) * sizeof(CHAR16), '\0');
    }

    CopyMem(Dest, Src, Size * sizeof(CHAR16));
}

CHAR16* StpCpy (CHAR16 *Dest, const CHAR16 *Src)
{
    while (*Src) {
        *(Dest++) = *(Src++);
    }
    *Dest = 0;
    return Dest;
}

CHAR16* StpnCpy (CHAR16 *Dest, const CHAR16 *Src, UINTN Len)
{
    UINTN Size = StrnLen(Src, Len);
    if (Size != Len) {
        SetMem(Dest + Size, (Len - Size) * sizeof(CHAR16), '\0');
    }
    CopyMem(Dest, Src, Size * sizeof(CHAR16));
    return Dest + Size;
}

void StrCat (CHAR16 *Dest, const CHAR16 *Src)
{
    StrCpy(Dest+StrLen(Dest), Src);
}

void StrnCat (CHAR16 *Dest, const CHAR16 *Src, UINTN Len)
{
    UINTN DestSize, Size;

    DestSize = StrLen(Dest);
    Size = StrnLen(Src, Len);
    CopyMem(Dest + DestSize, Src, Size * sizeof(CHAR16));
    Dest[DestSize + Size] = '\0';
}


UINTN StrnLen (const CHAR16 *s1, UINTN Len)
{
    UINTN i;
    for (i = 0; *s1 && i < Len; i++) {
        s1++;
    }
    return i;
}

UINTN StrLen (const CHAR16 *s1)
{
    UINTN len;
    for (len=0; *s1; s1+=1, len+=1);
    return len;
}

UINTN StrSize (const CHAR16 *s1)
{
    UINTN len;
    for (len=0; *s1; s1+=1, len+=1) ;
    return (len + 1) * sizeof(CHAR16);
}

// duplicate a string
// TODO:
// CHAR16* StrDuplicate (const CHAR16 *Src)
// {
//     CHAR16 *Dest;
//     UINTN Size;

//     Size = StrSize(Src);
//     Dest = AllocatePool (Size);
//     if (Dest) {
//         CopyMem (Dest, Src, Size);
//     }
//     return Dest;
// }

UINTN strlena (const CHAR8 *s1)
{
    UINTN len;
    for (len=0; *s1; s1+=1, len+=1) ;
    return len;
}

UINTN strcmpa (const CHAR8 *s1, const CHAR8 *s2)
{
    while (*s1) {
        if (*s1 != *s2) {
            break;
        }

        s1 += 1;
        s2 += 1;
    }

    return *s1 - *s2;
}

UINTN strncmpa (const CHAR8 *s1, const CHAR8 *s2, UINTN len)
{
    while (*s1  &&  len) {
        if (*s1 != *s2) {
            break;
        }

        s1  += 1;
        s2  += 1;
        len -= 1;
    }

    return len ? *s1 - *s2 : 0;
}


// convert hex string to uint
UINTN xtoi (const CHAR16 *str)
{
    UINTN u;
    CHAR16 c;

    // skip preceeding white space
    while (*str && *str == ' ') {
        str += 1;
    }

    // convert hex digits
    u = 0;
    while ((c = *(str++))) {
        if (c >= 'a'  &&  c <= 'f') {
            c -= 'a' - 'A';
        }

        if ((c >= '0'  &&  c <= '9')  ||  (c >= 'A'  &&  c <= 'F')) {
            u = (u << 4)  |  (c - (c >= 'A' ? 'A'-10 : '0'));
        } else {
            break;
        }
    }

    return u;
}

// convert string to uint
UINTN Atoi (const CHAR16 *str)
{
    UINTN u;
    CHAR16 c;

    // skip preceeding white space
    while (*str && *str == ' ') {
        str += 1;
    }

    // convert digits
    u = 0;
    while ((c = *(str++))) {
        if (c >= '0' && c <= '9') {
            u = (u * 10) + c - '0';
        } else {
            break;
        }
    }

    return u;
}

BOOLEAN MetaMatch (CHAR16 *String, CHAR16 *Pattern)
{
    CHAR16  c, p, l;

    for (; ;) {
        p = *Pattern;
        Pattern += 1;

        switch (p) {
        case 0:
            // End of pattern.  If end of string, TRUE match
            return *String ? FALSE : TRUE;

        case '*':
            // Match zero or more chars
            while (*String) {
                if (MetaMatch (String, Pattern)) {
                    return TRUE;
                }
                String += 1;
            }
            return MetaMatch (String, Pattern);

        case '?':
            // Match any one char
            if (!*String) {
                return FALSE;
            }
            String += 1;
            break;

        case '[':
            // Match char set
            c = *String;
            if (!c) {
                return FALSE;                       // syntax problem
            }

            l = 0;
            while ((p = *Pattern++)) {
                if (p == ']') {
                    return FALSE;
                }

                if (p == '-') {                     // if range of chars,
                    p = *Pattern;                   // get high range
                    if (p == 0 || p == ']') {
                        return FALSE;               // syntax problem
                    }

                    if (c >= l && c <= p) {         // if in range,
                        break;                      // it's a match
                    }
                }

                l = p;
                if (c == p) {                       // if char matches
                    break;                          // move on
                }
            }

            // skip to end of match char set
            while (p && p != ']') {
                p = *Pattern;
                Pattern += 1;
            }

            String += 1;
            break;

        default:
            c = *String;
            if (c != p) {
                return FALSE;
            }

            String += 1;
            break;
        }
    }
}

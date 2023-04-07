#include <assert.h>
#include "MkString.h"

MkString_MemAlloc_Cb MkString_MemAlloc;
MkString_MemRealloc_Cb MkString_MemRealloc;
MkString_MemFree_Cb MkString_MemFree;
MkString_MemCopy_Cb MkString_MemCopy;

MkStringW * MkStringW_CopyC(const wchar_t * cstring)
{
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(cstring);

    MkStringW * string = MkString_MemAlloc(sizeof(MkStringW));
    if (!string)
    {
        return NULL;
    }
    
    string->length = wcslen(cstring);
    string->chars = MkString_MemAlloc((string->length + 1) * sizeof(wchar_t));
    if (!string->chars)
    {
        MkString_MemFree(string);
        return NULL;
    }

    MkString_MemCopy(string->chars, cstring, string->length * sizeof(wchar_t));
    string->chars[string->length] = L'\0';
    return string;
}

MkStringW * MkStringW_Copy(const MkStringW * string)
{
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(string);

    MkStringW * copy = MkString_MemAlloc(sizeof(MkStringW));
    if (!copy)
    {
        return NULL;
    }

    copy->chars = MkString_MemAlloc((string->length + 1) * sizeof(wchar_t));
    if (!copy->chars)
    {
        MkString_MemFree(copy);
        return NULL;
    }
    copy->length = string->length;

    MkString_MemCopy(copy->chars, string->chars, string->length * sizeof(wchar_t));
    copy->chars[copy->length] = L'\0';
    return copy;
}

MkStringW * MkStringW_ConcatC(const wchar_t * cstringA, const wchar_t * cstringB)
{
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(cstringA);
    assert(cstringB);

    const size_t lengthA = wcslen(cstringA);
    const size_t lengthB = wcslen(cstringB);

    MkStringW * string = MkString_MemAlloc(sizeof(MkStringW));
    if (!string)
    {
        return NULL;
    }

    string->length = lengthA + lengthB;
    string->chars = MkString_MemAlloc((string->length + 1) * sizeof(wchar_t));
    if (!string->chars)
    {
        MkString_MemFree(string);
        return NULL;
    }

    MkString_MemCopy(string->chars, cstringA, lengthA * sizeof(wchar_t));
    MkString_MemCopy(string->chars + lengthA, cstringB, lengthB * sizeof(wchar_t));
    string->chars[string->length] = L'\0';
    return string;
}

void MkStringW_Destroy(MkStringW * string)
{
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(string);
    assert(string->chars);

    MkString_MemFree(string->chars);
    MkString_MemFree(string);
}

int MkStringW_AppendC(MkStringW * dest, const wchar_t * source)
{
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(dest);
    assert(source);

    const size_t sourceLength = wcslen(source);
    const size_t newLength = dest->length + sourceLength;

    wchar_t * newChars = MkString_MemRealloc(dest->chars, (newLength + 1) * sizeof(wchar_t));
    if (!newChars)
    {
        return 0;
    }
    MkString_MemCopy(newChars + dest->length, source, sourceLength * sizeof(wchar_t));
    newChars[newLength] = L'\0';

    dest->chars = newChars;
    dest->length = newLength;
    return 1;
}

int MkStringW_AppendWin32PathC(MkStringW * win32Path, const wchar_t * element)
{
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(win32Path);
    assert(element);

    const size_t offset = win32Path->length == 0 || win32Path->chars[win32Path->length - 1] == L'\\' ? 0 : 1;
    const size_t elementLength = wcslen(element);
    const size_t newLength = win32Path->length + elementLength + offset;

    wchar_t * newChars = MkString_MemRealloc(win32Path->chars, (newLength + 1) * sizeof(wchar_t));
    if (!newChars)
    {
        return 0;
    }
    if (offset == 1)
    {
        newChars[win32Path->length] = L'\\';
        win32Path->length++;
    }
    MkString_MemCopy(newChars + win32Path->length, element, elementLength * sizeof(wchar_t));
    newChars[newLength] = L'\0';

    win32Path->chars = newChars;
    win32Path->length = newLength;
    return 1;
}

byte * MkStringW_ToUtf8(const MkStringW * string, size_t * count)
{
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(string);
    assert(count);

    byte * utf8String = MkString_MemAlloc(4 * string->length);
    if (!utf8String)
    {
        return NULL;
    }

    *count = 0;
    for (size_t i = 0; i != string->length; i++)
    {
        wchar_t c = string->chars[i];
        if (c <= 0x007f)
        {
            utf8String[*count++] = c;
        }
        else if (c <= 0x07ff)
        {
            utf8String[*count++] = (c >> 6) + 0b11000000;
            utf8String[*count++] = (c & 0b00111111) + 0b10000000;
        }
        else if (c <= 0xffff)
        {
            utf8String[*count++] = (c >> 12) + 0b11100000;
            utf8String[*count++] = (c >> 6 & 0b00111111) + 0b10000000;
            utf8String[*count++] = (c & 0b00111111) + 0b10000000;
        }
        else if (c <= 0x0010ffff)
        {
            utf8String[*count++] = (c >> 18) + 0b11110000;
            utf8String[*count++] = (c >> 12 & 0b00111111) + 0b10000000;
            utf8String[*count++] = (c >> 6 & 0b00111111) + 0b10000000;
            utf8String[*count++] = (c & 0b00111111) + 0b10000000;
        }
        else
        {
            utf8String[*count++] = 0b11101111;
            utf8String[*count++] = 0b10111111;
            utf8String[*count++] = 0b10111101;
        }
    }

    utf8String = MkString_MemRealloc(utf8String, *count);
    return utf8String;
}

typedef enum
{
    UTF8_START,
    UTF8_END_OK,
    UTF8_END_ERROR,
    UTF8_READ_1,
    UTF8_READ_2,
    UTF8_READ_3,
    UTF8_READ_ERROR,
} Utf8State;

MkStringW * MkStringW_FromUtf8(const byte * stream, const size_t count)
{
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(stream);

    MkStringW * string = MkString_MemAlloc(sizeof(MkStringW));
    if (!string)
    {
        return NULL;
    }

    wchar_t * chars = MkString_MemAlloc((count + 1) * sizeof(wchar_t));
    if (!chars)
    {
        MkString_MemFree(string);
        return NULL;
    }
    size_t length = 0;

    int limited = sizeof(wchar_t) < 4;

    Utf8State state = UTF8_START;
    size_t i = 0;
    u32 c = 0;
    size_t readCount = 0;
    while (i != count || state == UTF8_END_OK || state == UTF8_END_ERROR)
    {
        switch (state)
        {
            case UTF8_START:
            {
                if ((stream[i] & 0b10000000) == 0b00000000)
                {
                    c = stream[i++];
                    readCount = 0;
                    state = UTF8_END_OK;
                }
                else if ((stream[i] & 0b11000000) == 0b10000000)
                {
                    i++;
                    readCount = 0;
                    state = UTF8_READ_ERROR;
                }
                else if ((stream[i] & 0b11100000) == 0b11000000)
                {
                    c = (stream[i++] & 0b00011111) << 6;
                    readCount = 1;
                    state = UTF8_READ_1;
                }
                else if ((stream[i] & 0b11110000) == 0b11100000)
                {
                    c = (stream[i++] & 0b00001111) << 12;
                    readCount = 2;
                    state = UTF8_READ_2;
                }
                else if ((stream[i] & 0b11111000) == 0b11110000)
                {
                    c = (stream[i++] & 0b00000111) << 18;
                    readCount = 3;
                    state = UTF8_READ_3;
                }
                else
                {
                    i++;
                    readCount = 0;
                    state = UTF8_READ_ERROR;
                }

                if (readCount > count - i)
                {
                    i = count;
                    state = UTF8_END_ERROR;
                }
                break;
            }

            case UTF8_END_OK:
            {
                chars[length++] = c > 0xffff && limited ? 0xfffd : c;
                state = UTF8_START;
                break;
            }

            case UTF8_END_ERROR:
            {
                chars[length++] = 0xfffd;
                state = UTF8_START;
                break;
            }

            case UTF8_READ_1:
            {
                if ((stream[i] & 0b11000000) == 0b10000000)
                {
                    c += stream[i++] & 0b00111111;
                    state = UTF8_END_OK;
                }
                else
                {
                    i++;
                    state = UTF8_END_ERROR;
                }
                break;
            }

            case UTF8_READ_2:
            {
                if ((stream[i] & 0b11000000) == 0b10000000)
                {
                    c += (stream[i++] & 0b00111111) << 6;
                    state = UTF8_READ_1;
                }
                else
                {
                    i++;
                    state = UTF8_END_ERROR;
                }
                break;
            }

            case UTF8_READ_3:
            {
                if ((stream[i] & 0b11000000) == 0b10000000)
                {
                    c += (stream[i++] & 0b00111111) << 12;
                    state = UTF8_READ_2;
                }
                else
                {
                    i++;
                    state = UTF8_END_ERROR;
                }
                break;
            }

            case UTF8_READ_ERROR:
            {
                if ((stream[i++] & 0b11000000) != 0b10000000)
                {
                    state = UTF8_END_ERROR;
                }
                break;
            }
        }
    }

    chars[length] = L'\0';
    string->chars = MkString_MemRealloc(chars, (length + 1) * sizeof(wchar_t));
    string->length = length;
    return string;
}
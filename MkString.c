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
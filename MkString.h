#ifndef MK_STRING
#define MK_STRING

// NOTE: This library uses assertions to check function parameters.

#ifndef __cplusplus
#include <wchar.h>
#endif

typedef void * (*MkString_MemAlloc_Cb)(size_t size);
typedef void * (*MkString_MemRealloc_Cb)(void * pointer, size_t size);
typedef void (*MkString_MemFree_Cb)(void * pointer);
typedef void (*MkString_MemCopy_Cb)(void * dest, const void * source, size_t size);

#ifdef __cplusplus
extern "C"
{
#endif

    extern MkString_MemAlloc_Cb MkString_MemAlloc;
    extern MkString_MemRealloc_Cb MkString_MemRealloc;
    extern MkString_MemFree_Cb MkString_MemFree;
    extern MkString_MemCopy_Cb MkString_MemCopy;

    // A safer wide string type.
    typedef struct
    {
        wchar_t * chars;
        size_t length;
    } MkStringW;

    // Create a safe string by copying a C string.
    // WARNING: Make sure the C string is valid (null-terminated)! If it isn't the usual problems will arise.
    // Returns NULL if memory allocation failed.
    MkStringW * MkStringW_CopyC(const wchar_t * cstring);

    // Create a copy.
    // Returns NULL if memory allocation failed.
    MkStringW * MkStringW_Copy(const MkStringW * string);

    // Create a safe string by concatenating two C strings.
    // WARNING: Make sure the C strings are valid!
    // Returns NULL if memory allocation failed.
    MkStringW * MkStringW_ConcatC(const wchar_t * cstringA, const wchar_t * cstringB);

    // Destroy a safe string.
    void MkStringW_Destroy(MkStringW * string);

    // Append source C string to the end of dest.
    // WARNING: Make sure the C string is valid!
    // Returns 0 if memory allocation failed.
    int MkStringW_AppendC(MkStringW * dest, const wchar_t * source);

    // Append a C string element to a Win32 folder path.
    // WARNING: Make sure the C string is valid!
    // Returns 0 if memory allocation failed.
    int MkStringW_AppendWin32PathC(MkStringW * win32Path, const wchar_t * element);

#ifdef __cplusplus
}
#endif

#endif
#ifndef MK_STRING
#define MK_STRING

// NOTE: This library uses assertions to check function parameters.

#ifndef __cplusplus
#include <wchar.h>
#endif

typedef unsigned char byte;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef char i8;
typedef short i16;
typedef long i32;
typedef long long i64;

typedef void * (*MkString_MemAlloc_Cb)(size_t size);
typedef void * (*MkString_MemRealloc_Cb)(void * pointer, size_t size);
typedef void (*MkString_MemFree_Cb)(void * pointer);
typedef void (*MkString_MemCopy_Cb)(void * dest, const void * source, size_t size);

#ifdef __cplusplus
extern "C"
{
#endif
    // NOTE: You must set these!
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

    // Convert a string into a UTF-8 byte stream.
    // Returns NULL if memory allocation failed.
    byte * MkStringW_ToUtf8(const MkStringW * string, size_t * count);

    // Parse a UTF-8 byte stream into a string. Errors will be marked by the replacement char 0xFFFD.
    // Returns NULL if memory allocation failed.
    MkStringW * MkStringW_FromUtf8(const byte * stream, const size_t count);

#ifdef __cplusplus
}
#endif

#endif
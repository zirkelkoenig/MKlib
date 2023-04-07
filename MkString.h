/*
Safe string types and additional useful string functionality.

Usage:
- You must define and set pointers to memory management functions.
- None of the functions accept NULL pointers, except where explicitly stated.
- Assertions are used to check input parameters.
*/

//----------------
// DEFINITIONS
//----------------

#ifndef MK_STRING
#define MK_STRING

#include <wchar.h>

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

//-------------------
// MEMORY MANAGEMENT
//-------------------

extern MkString_MemAlloc_Cb MkString_MemAlloc;
extern MkString_MemRealloc_Cb MkString_MemRealloc;
extern MkString_MemFree_Cb MkString_MemFree;
extern MkString_MemCopy_Cb MkString_MemCopy;

//-------------------
// PUBLIC INTERFACE
//-------------------

// Convert a C string into a UTF-8 byte array.
// WARNING: Make sure the C string is valid!
// Returns NULL if memory allocation failed.
byte * MkString_CwToUtf8(const wchar_t * cwString, size_t * utf8Length);

// Parse a UTF-8 byte array into a C string. Errors will be marked by the replacement char 0xFFFD.
// Returns NULL if memory allocation failed.
wchar_t * MkString_CwFromUtf8(const byte * utf8, const size_t utf8Length);

// A safer wide string type.
typedef struct
{
    wchar_t * chars;
    size_t length;
} MkStringW;

// Create a safe string by copying a C string.
// WARNING: Make sure the C string is valid (null-terminated)! If it isn't the usual problems will arise.
// Returns NULL if memory allocation failed.
MkStringW * MkStringW_Copy_Cw(const wchar_t * cwString);

// Create a copy.
// Returns NULL if memory allocation failed.
MkStringW * MkStringW_Copy(const MkStringW * string);

// Create a safe string by concatenating two C strings.
// WARNING: Make sure the C strings are valid!
// Returns NULL if memory allocation failed.
MkStringW * MkStringW_Concat_Cw(const wchar_t * cwStringA, const wchar_t * cwStringB);

// Destroy a safe string.
void MkStringW_Destroy(MkStringW * string);

// Append source C string to the end of dest.
// WARNING: Make sure the C string is valid!
// Returns 0 if memory allocation failed.
int MkStringW_Append_Cw(MkStringW * dest, const wchar_t * source);

// Append a C string to a Win32 folder path, inserting a separator ('\\') if necessary.
// WARNING: Make sure the C string is valid!
// Returns 0 if memory allocation failed.
int MkStringW_AppendWin32Path_Cw(MkStringW * win32Path, const wchar_t * component);

// Convert a string into a UTF-8 byte array.
// Returns NULL if memory allocation failed.
byte * MkStringW_ToUtf8(const MkStringW * string, size_t * utf8Length);

// Parse a UTF-8 byte array into a string. Errors will be marked by the replacement char 0xFFFD.
// Returns NULL if memory allocation failed.
MkStringW * MkStringW_FromUtf8(const byte * utf8, const size_t utf8Length);

#endif

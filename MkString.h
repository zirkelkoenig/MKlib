/*
Safe string types and additional useful string functionality.

Usage:
- You must define and set pointers to memory management functions (see MkLib.h).
- None of the functions accept NULL pointers, except where explicitly stated.
- Assertions are used to check input parameters.
*/

#ifndef MK_STRING_HEADER
#define MK_STRING_HEADER

#include <wchar.h>
#include "MkLib.h"

// Convert a C string into a UTF-8 byte array.
// WARNING: Make sure the C string is valid!
// Returns NULL if memory allocation failed.
byte * MkLib_CwStringToUtf8(const wchar_t * string, size_t * utf8Length);

// Parse a UTF-8 byte array into a C string. Errors will be marked by the replacement char 0xFFFD.
// Returns NULL if memory allocation failed.
wchar_t * MkLib_CwStringFromUtf8(const byte * utf8, const size_t utf8Length);

// Parse a UTF-8 byte array into one dynamic array per line (which won't be 0-terminated). Errors will be marked by the
// replacement char 0xFFFD.
// Returns NULL if memory allocation failed.
wchar_t ** MkLib_CwDynArrayLinesFromUtf8(const byte * utf8, const size_t utf8Length);

// Find the first occurence of a character in a wide C string, starting at the given index.
// Returns SIZE_MAX if the character was not found.
size_t MkLib_CwStringFindChar(const wchar_t * string, const size_t length, const size_t start, const wchar_t c);

// A safer wide string type.
typedef struct {
    size_t length;
    wchar_t chars[1];
} MkLib_StringW;

// Create a safe string by copying a C string.
// WARNING: Make sure the C string is valid (null-terminated)! If it isn't the usual problems will arise.
// Returns NULL if memory allocation failed.
MkLib_StringW * MkLib_StringW_FromCwString(const wchar_t * cwString);

// Create a copy.
// Returns NULL if memory allocation failed.
MkLib_StringW * MkLib_StringW_Copy(const MkLib_StringW * string);

// Create a safe string by concatenating two C strings.
// WARNING: Make sure the C strings are valid!
// Returns NULL if memory allocation failed.
MkLib_StringW * MkLib_StringW_FromConcatCwStrings(const wchar_t * cwStringA, const wchar_t * cwStringB);

// Destroy a safe string.
void MkLib_StringW_Destroy(MkLib_StringW * string);

// Append source C string to the end of dest.
// WARNING: Make sure the C string is valid!
// Returns 0 if memory allocation failed.
int MkLib_StringW_AppendCwString(MkLib_StringW ** dest, const wchar_t * source);

// Append a C string to a Win32 folder path, inserting a separator ('\\') if necessary.
// WARNING: Make sure the C string is valid!
// Returns 0 if memory allocation failed.
int MkLib_StringW_AppendCwWin32Path(MkLib_StringW ** win32Path, const wchar_t * component);

// Convert a string into a UTF-8 byte array.
// Returns NULL if memory allocation failed.
byte * MkLib_StringW_ToUtf8(const MkLib_StringW * string, size_t * utf8Length);

// Parse a UTF-8 byte array into a string. Errors will be marked by the replacement char 0xFFFD.
// Returns NULL if memory allocation failed.
MkLib_StringW * MkLib_StringW_FromUtf8(const byte * utf8, const size_t utf8Length);

// Parse a UTF-8 byte array into one string per line. Errors will be marked by the replacement char 0xFFFD.
// Returns NULL if memory allocation failed.
MkLib_StringW ** MkLib_StringW_LinesFromUtf8(const byte * utf8, const size_t utf8Length, size_t * lineCount);

#endif

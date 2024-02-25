#if !defined(_MKLIB_STRING_H)
#define _MKLIB_STRING_H

#include "MkList.h"

typedef unsigned char byte;
typedef unsigned long ulong;


//---------------------
// C STRING FUNCTIONS

// Return a pointer to the first occurrence of a given character in a string.
// Returns NULL if the character was not found.
wchar_t * MkWcsFindChar(wchar_t * wcs, size_t length, wchar_t wc);

// Return the index of the first occurrence of any of the given characters.
// Returns SIZE_MAX if none of the characters were found.
size_t MkWcsFindCharsIndex(const wchar_t * wcs, size_t length, const wchar_t * chars, size_t count);

// Return the index of the first occurrence of a given NULL-terminated substring.
// Returns SIZE_MAX if the substring was not found.
size_t MkWcsFindSubstringIndex(const wchar_t * wcs, size_t length, const wchar_t * substring);

// Check whether a string starts with the given NULL-terminated prefix.
bool MkWcsIsPrefix(const wchar_t * wcs, size_t length, const wchar_t * prefix);


//----------------
// SAFE STRING

// A safe string type.
struct MkWstr {
    size_t length;
    wchar_t * wcs;
};

// Set a safe string to refer to another string.
static void MkWstrSet(MkWstr * wstrPtr, wchar_t * wcs, size_t length) {
    assert(wstrPtr);
    assert(wcs);
    assert(length);

    wstrPtr->length = length;
    wstrPtr->wcs = wcs;
}

// Check whether two safe strings are equal.
bool MkWstrsAreEqual(const MkWstr * a, const MkWstr * b);


//----------------
// UTF-8 FUNCTIONS

// Read input from a byte stream.
// Returns FALSE when no more bytes can be returned and sets a status variable.
typedef bool (*MkReadByteStream)(void * stream, ulong count, byte * buffer, void * status);

// Write output to a byte stream.
// Returns FALSE on error and sets a status variable.
typedef bool (*MkWriteByteStream)(void * stream, const byte * buffer, ulong count, void * status);

enum MkUtf8Options {
    MKUTF8_NO_OPTIONS = 0,
    MKUTF8_CRLF = 1 << 0, // convert newlines to CR+LF while writing
    MKUTF8_STOP_AT_NULL = 1 << 1, // stop at a NULL byte
};

// Read a UTF-8 byte stream and append the resulting characters to a list.
// The output list must be properly initialized to take WCHAR_T elements.
// Returns FALSE on memory allocation failure.
bool MkReadUtf8Stream(MkReadByteStream readCallback, void * stream, void * status, MkUtf8Options options, MkList * listPtr);

// Write text to a UTF-8 byte stream.
// Returns FALSE on failure.
bool MkWriteUtf8Stream(MkWriteByteStream writeCallback, void * stream, void * status, MkUtf8Options options, const wchar_t * wcs, size_t count);

#endif
/*
MIT License

Copyright (c) 2024 Marvin Kipping

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#if !defined(_MKLIB_STRING_H)
#define _MKLIB_STRING_H

#include <assert.h>
#include <stdint.h>
#include <wchar.h>

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;

//-----------------------
// C STRING FUNCTIONS

// Finds the first occurrence of the given character in a string and returns its index.
// Returns SIZE_MAX if the character is not in the string.
size_t MkWcsFindCharIndex(const wchar_t * wcs, size_t wcsMax, wchar_t c);

// Finds the first occurrence of one of the given characters in a string and returns the index.
// Returns SIZE_MAX if none of the characters are in the string.
size_t MkWcsFindCharsIndex(const wchar_t * wcs, size_t wcsMax, const wchar_t * chars, size_t count);

// Finds the first occurrence of the given NULL-terminated substring in another string and returns its index.
// Returns SIZE_MAX if the substring was not found.
size_t MkWcsFindSubstrIndex(const wchar_t * wcs, size_t wcsMax, const wchar_t * substr);

// Checks whether a NULL-terminated string is the prefix of another string.
bool MkWcsIsPrefix(const wchar_t * wcs, size_t wcsMax, const wchar_t * prefix);

// Checks whether two strings are equal.
bool MkWcsAreEqual(const wchar_t * a, size_t aMax, const wchar_t * b, size_t bMax);

//---------------
// STRING REF

// a string reference type
struct MkWstr {
    size_t length;
    const wchar_t * wcs;
};

// Sets up a string reference.
static void MkWstrSet(MkWstr * strRef, const wchar_t * wcs, size_t length) {
    assert(strRef);
    assert(wcs || length == 0);

    strRef->length = length;
    strRef->wcs = wcs;
}

//-----------------
// UTF-8 FUNCTIONS

// Reads a number of elements from a stream and puts them into a buffer.
// Returns FALSE on failure and sets a status variable.
typedef bool (*MkStreamRead)(void * stream, void * buffer, ulong count, void * status);

// Takes a number of elements from a buffer and writes them to a stream.
// Returns FALSE on failure and sets a status variable.
typedef bool (*MkStreamWrite)(void * stream, const void * buffer, ulong count, void * status);

// Reads a stream of UTF-8 bytes and writes them to a wide char stream.
// Returns FALSE if a write operation failed.
bool MkUtf8Read(
    MkStreamRead readBytesCallback, void * byteStream, void * readStatus,
    MkStreamWrite writeCharsCallback, void * charStream, void * writeStatus);

// Writes a wide char string to a UTF-8 byte stream. Stops when encountering a NULL char.
// Returns FALSE if a write operation failed.
bool MkUtf8WriteWcs(
    const wchar_t * wcs, size_t wcsMax, bool toCrlf,
    MkStreamWrite writeBytesCallback, void * byteStream, void * writeStatus);

#endif
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

#include "MkList.h"

typedef unsigned char byte;
typedef unsigned long ulong;


//---------------------
// C STRING FUNCTIONS

// Return a pointer to the first occurrence of a given character in a string.
// Returns NULL if the character was not found.
wchar_t * MkWcsFindChar(wchar_t * wcs, ulong length, wchar_t wc);

// Return the index of the first occurrence of any of the given characters.
// Returns ULONG_MAX if none of the characters were found.
ulong MkWcsFindCharsIndex(const wchar_t * wcs, ulong length, const wchar_t * chars, ulong count);

// Return the index of the first occurrence of a given NULL-terminated substring.
// Returns ULONG_MAX if the substring was not found.
ulong MkWcsFindSubstringIndex(const wchar_t * wcs, ulong length, const wchar_t * substring);

// Check whether a string starts with the given NULL-terminated prefix.
bool MkWcsIsPrefix(const wchar_t * wcs, ulong length, const wchar_t * prefix);


//----------------
// SAFE STRING

// A safe string type.
struct MkWstr {
    ulong length;
    wchar_t * wcs;
};

// Set a safe string to refer to another string.
static void MkWstrSet(MkWstr * wstrPtr, wchar_t * wcs, ulong length) {
    assert(wstrPtr);
    assert(wcs || length == 0);

    wstrPtr->length = length;
    wstrPtr->wcs = wcs;
}

// Check whether two safe strings are equal.
bool MkWstrsAreEqual(const MkWstr * a, const MkWstr * b);


//----------------
// UTF-8 FUNCTIONS

// Read the UTF-8 input stream and append the resulting wide chars to the output list.
// Returns FALSE on memory allocation failure.
bool MkReadUtf8Stream(const byte * input, ulong inputSize, MkList * output);

// Read the UTF-8 input stream and append the resulting lines of wide chars to the output list.
// Returns FALSE on memory allocation failure.
bool MkReadUtf8StreamToLines(const byte * input, ulong inputSize, MkList * outputLines);

// Write bytes to a stream.
// Returns FALSE on failure and sets the status variable.
typedef bool (*MkByteStreamWriteCallback)(void * stream, const byte * input, ulong inputCount, void * status);

// Write wide chars to a UTF-8 stream. Stops when encountering a NULL-terminator.
// Returns FALSE on output error.
bool MkWriteUtf8Stream(MkByteStreamWriteCallback writeCallback, void * stream, void * status, const wchar_t * str, ulong strLength, bool toCrlf);

#endif
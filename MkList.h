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

#if !defined(_MKLIB_LIST_H)
#define _MKLIB_LIST_H

#include <assert.h>
#include <limits.h>

typedef unsigned char byte;
typedef unsigned long ulong;

// A dynamic array.
struct MkList {
    ulong count;
    ulong capacity;
    ulong growCount;
    ulong elemSize;
    byte * elems;
};

// Initialize a list.
void MkListInit(MkList * listPtr, ulong growCount, ulong elemSize);

// Clear a list, freeing its allocated memory.
void MkListClear(MkList * listPtr);

// Explicitly set the capacity of a list.
// Returns FALSE on memory allocation failure.
bool MkListSetCapacity(MkList * listPtr, ulong capacity);

// Insert a number of new element slots at the given index, shifting the successive elements. Pass ULONG_MAX as the index to append the new elements at the end of the list.
// Returns a pointer to the first new element slot or NULL on memory allocation failure.
byte * MkListInsert(MkList * listPtr, ulong index, ulong count);

// Check if a list is valid. Will only perform in debug builds.
static void MkListAssert(MkList * listPtr) {
    assert(listPtr);
    assert(listPtr->count <= listPtr->capacity);
    assert(listPtr->growCount != 0);
    assert(listPtr->elemSize != 0);
    assert(listPtr->elems != nullptr || listPtr->capacity == 0);
}

// Get a pointer the the element slot at the given index.
static byte * MkListGet(MkList * listPtr, ulong index) {
    MkListAssert(listPtr);
    assert(index < listPtr->count);
    return listPtr->elems + (index * listPtr->elemSize);
}

#endif
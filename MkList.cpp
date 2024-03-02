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

#include <stdlib.h>

#include "MkList.h"

void MkListInit(MkList * listPtr, ulong growCount, ulong elemSize) {
    assert(listPtr);
    assert(growCount != 0);
    assert(elemSize != 0);

    listPtr->count = 0;
    listPtr->capacity = 0;
    listPtr->growCount = growCount;
    listPtr->elemSize = elemSize;
    listPtr->elems = nullptr;
}

void MkListClear(MkList * listPtr) {
    MkListAssert(listPtr);
    listPtr->count = 0;
    listPtr->capacity = 0;
    if (listPtr->elems) {
        free(listPtr->elems);
        listPtr->elems = nullptr;
    }
}

bool MkListSetCapacity(MkList * listPtr, ulong capacity) {
    MkListAssert(listPtr);
    
    byte * newElems = (byte *)realloc(listPtr->elems, capacity * listPtr->elemSize);
    if (!newElems && capacity != 0) {
        return false;
    }
    if (capacity < listPtr->count) {
        listPtr->count = capacity;
    }
    listPtr->capacity = capacity;
    listPtr->elems = newElems;
    return true;
}

void * MkListInsert(MkList * listPtr, ulong index, ulong count) {
    MkListAssert(listPtr);
    assert(index < listPtr->count || index == ULONG_MAX);

    ulong newCount = listPtr->count + count;
    if (newCount > listPtr->capacity) {
        ulong newCapacity = ((newCount / listPtr->growCount) + 1) * listPtr->growCount;
        if (!MkListSetCapacity(listPtr, newCapacity)) {
            return nullptr;
        }
    }

    if (index == ULONG_MAX) {
        index = listPtr->count;
    }
    listPtr->count = newCount;

    ulong offset = count * listPtr->elemSize;
    ulong end = (index + count) * listPtr->elemSize;
    byte * bytes = static_cast<byte *>(listPtr->elems);
    for (ulong i = listPtr->count * listPtr->elemSize - 1; i >= end; i--) {
        bytes[i] = bytes[i - offset];
    }

    return MkListGet(listPtr, index);
}
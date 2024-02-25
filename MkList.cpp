#include <stdlib.h>

#include "MkList.h"

void MkListInit(MkList * listPtr, size_t growCount, size_t elemSize) {
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

bool MkListSetCapacity(MkList * listPtr, size_t capacity) {
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

byte * MkListInsert(MkList * listPtr, size_t index, size_t count) {
    MkListAssert(listPtr);
    assert(index < listPtr->count || index == SIZE_MAX);

    size_t newCount = listPtr->count + count;
    if (newCount > listPtr->capacity) {
        size_t newCapacity = ((newCount / listPtr->growCount) + 1) * listPtr->growCount;
        if (!MkListSetCapacity(listPtr, newCapacity)) {
            return nullptr;
        }
    }

    if (index == SIZE_MAX) {
        index = listPtr->count;
    }
    listPtr->count = newCount;

    size_t offset = count * listPtr->elemSize;
    size_t end = (index + count) * listPtr->elemSize;
    for (size_t i = listPtr->count * listPtr->elemSize - 1; i >= end; i--) {
        listPtr->elems[i] = listPtr->elems[i - offset];
    }

    return MkListGet(listPtr, index);
}
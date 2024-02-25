#if !defined(_MKLIB_LIST_H)
#define _MKLIB_LIST_H

#include <assert.h>
#include <limits.h>

typedef unsigned char byte;

// A dynamic array.
struct MkList {
    size_t count;
    size_t capacity;
    size_t growCount;
    size_t elemSize;
    byte * elems;
};

// Initialize a list.
void MkListInit(MkList * listPtr, size_t growCount, size_t elemSize);

// Clear a list, freeing its allocated memory.
void MkListClear(MkList * listPtr);

// Explicitly set the capacity of a list.
// Returns FALSE on memory allocation failure.
bool MkListSetCapacity(MkList * listPtr, size_t capacity);

// Insert a number of new element slots at the given index, shifting the successive elements. Pass SIZE_MAX as the index to append the new elements at the end of the list.
// Returns a pointer to the first new element slot or NULL on memory allocation failure.
byte * MkListInsert(MkList * listPtr, size_t index, size_t count);

// Check if a list is valid. Will only perform in debug builds.
static void MkListAssert(MkList * listPtr) {
    assert(listPtr);
    assert(listPtr->count <= listPtr->capacity);
    assert(listPtr->growCount != 0);
    assert(listPtr->elemSize != 0);
    assert(listPtr->elems != nullptr || listPtr->capacity == 0);
}

// Get a pointer the the element slot at the given index.
static byte * MkListGet(MkList * listPtr, size_t index) {
    MkListAssert(listPtr);
    assert(index < listPtr->count);
    return listPtr->elems + (index * listPtr->elemSize);
}

#endif
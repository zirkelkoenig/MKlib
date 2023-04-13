#include <assert.h>
#include "MkDynArray.h"

typedef struct {
    size_t count;
    size_t capacity;
    size_t elemSize;
    size_t growCount;
} Head;

void * MkLib_DynArray_Create_Impl(const size_t elemSize, const size_t capacity, const size_t growCount) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(elemSize != 0);
    assert(capacity != 0);

    const size_t size = sizeof(Head) + capacity * elemSize;
    Head * head = MkLib_MemAlloc(size);
    if (!head) {
        return NULL;
    }
    head->count = 0;
    head->capacity = capacity;
    head->elemSize = elemSize;
    head->growCount = growCount;

    return head + 1;
}

size_t MkLib_DynArray_Count(void * array) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(array);

    Head * head = (Head *)array - 1;
    assert(head->count <= head->capacity);

    return head->count;
}

void MkLib_DynArray_Destroy(void * array) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(array);

    Head * head = (Head *)array - 1;
    MkLib_MemFree(head);
}

int MkLib_DynArray_Increment_Impl(void ** array) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(array);
    assert(*array);

    Head * head = (Head *)*array - 1;
    assert(head->count <= head->capacity);

    if (head->count == head->capacity) {
        size_t newCapacity = head->capacity + head->growCount;
        size_t size = sizeof(Head) + newCapacity * head->elemSize;
        head = MkLib_MemRealloc(head, size);
        if (!head) {
            return 0;
        }
        head->capacity = newCapacity;
        *array = head + 1;
    }

    head->count++;
    return 1;
}

int MkLib_DynArray_Shift_Impl(void ** array, const size_t index) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(array);
    assert(*array);

    if (!MkLib_DynArray_Increment_Impl(array)) {
        return 0;
    }

    Head * head = (Head *)*array - 1;
    assert(head->count <= head->capacity);
    assert(index < head->count);

    byte * location = (byte *)*array + index * head->elemSize;
    MkLib_MemMove(location + head->elemSize, location, (head->count - 1 - index) * head->elemSize);
}

void MkLib_DynArray_Remove(void * array, size_t index) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(array);

    Head * head = (Head *)array - 1;
    assert(head->count <= head->capacity);
    assert(index < head->count);

    head->count--;
    byte * location = (byte *)array + index * head->elemSize;
    MkLib_MemMove(location, location + head->elemSize, (head->count - index) * head->elemSize);
}

int MkLib_DynArray_SetCount(void ** array, const size_t count) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(array);
    assert(*array);

    Head * head = (Head *)*array - 1;
    assert(head->count <= head->capacity);

    if (count > head->capacity) {
        size_t newCapacity = ((count / head->growCount) + 1) * head->growCount;
        head = MkLib_MemRealloc(head, sizeof(Head) + newCapacity * head->elemSize);
        if (!head) {
            return 0;
        }
        head->capacity = newCapacity;
        *array = head + 1;
    }

    head->count = count;
    return 1;
}
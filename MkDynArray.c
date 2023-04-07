#include <assert.h>
#include "MkDynArray.h"

typedef struct
{
    size_t count;
    size_t capacity;
    size_t elemSize;
    size_t growCount;
} Head;

void * MkDynArray_Create_Impl(const size_t elemSize, const size_t capacity)
{
    assert(MkDynArray_MemAlloc);
    assert(MkDynArray_MemRealloc);
    assert(MkDynArray_MemFree);
    assert(MkDynArray_MemCopy);

    assert(elemSize != 0);
    assert(capacity != 0);

    const size_t size = sizeof(Head) + capacity * elemSize;
    Head * head = MkDynArray_MemAlloc(size);
    if (!head)
    {
        return NULL;
    }
    head->count = 0;
    head->capacity = capacity;
    head->elemSize = elemSize;
    head->growCount = capacity;

    return head + 1;
}

size_t MkDynArray_Count(void * array)
{
    assert(MkDynArray_MemAlloc);
    assert(MkDynArray_MemRealloc);
    assert(MkDynArray_MemFree);
    assert(MkDynArray_MemCopy);

    assert(array);

    Head * head = (Head *)array - 1;
    return head->count;
}

void MkDynArray_Destroy(void * array)
{
    assert(MkDynArray_MemAlloc);
    assert(MkDynArray_MemRealloc);
    assert(MkDynArray_MemFree);
    assert(MkDynArray_MemCopy);

    assert(array);

    Head * head = (Head *)array - 1;
    MkDynArray_MemFree(head);
}

int MkDynArray_Increment_Impl(void ** array)
{
    assert(MkDynArray_MemAlloc);
    assert(MkDynArray_MemRealloc);
    assert(MkDynArray_MemFree);
    assert(MkDynArray_MemCopy);

    assert(array);
    assert(*array);

    Head * head = (Head *)*array - 1;
    assert(head->count <= head->capacity);

    if (head->count == head->capacity)
    {
        size_t newCapacity = head->capacity + head->growCount;
        size_t size = sizeof(Head) + newCapacity * head->elemSize;
        head = MkDynArray_MemRealloc(head, size);
        if (!head)
        {
            return 0;
        }
        head->capacity = newCapacity;
        *array = head + 1;
    }

    head->count++;
    return 1;
}

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
#include <stdint.h>
#include <stdlib.h>

// a dynamic array
template<class T>
struct MkList {
    size_t count; // the current number of elements
    size_t capacity; // the current number of available element slots
    size_t growCount; // the number of element slots to add when expanding the array
    T * elems; // the actual element slots

    // Initializes the array.
    void Init(size_t newGrowCount) {
        assert(newGrowCount != 0);

        count = 0;
        capacity = 0;
        growCount = newGrowCount;
        elems = nullptr;
    }

    void Assert() {
        assert(count <= capacity);
        assert(growCount != 0);
        assert(elems || capacity == 0);
    }

    // Clears the array, freeing all allocated memory.
    void Clear() {
        Assert();

        count = 0;
        capacity = 0;
        if (elems) {
            free(elems);
            elems = nullptr;
        }
    }

    // Explicitly sets the number of element slots.
    // Returns FALSE on memory allocation failure.
    bool SetCapacity(size_t newCapacity) {
        Assert();

        T * newElems = static_cast<T *>(realloc(elems, newCapacity * sizeof(T)));
        if (!(newElems || newCapacity == 0)) {
            return false;
        }

        if (count > newCapacity) {
            count = newCapacity;
        }
        capacity = newCapacity;
        elems = newElems;
        return true;
    }

    // Creates space for a given number of elements at the given index, shifting the existing successive elements.
    // If the index is SIZE_MAX, the new space is created at the end of the array.
    // Returns a pointer to the first new element slot or NULL on memory allocation failure.
    T * Insert(size_t index, size_t insertCount) {
        Assert();
        assert(index <= count || index == SIZE_MAX);

        size_t newCount = count + insertCount;
        if (newCount > capacity) {
            size_t newCapacity = ((newCount / growCount) + 1) * growCount;
            if (!SetCapacity(newCapacity)) {
                return nullptr;
            }
        }

        if (index == SIZE_MAX) {
            index = count;
        }
        count = newCount;

        size_t shiftEnd = index + insertCount - 1;
        for (size_t i = count - 1; i != shiftEnd; i--) {
            elems[i] = elems[i - insertCount];
        }

        return &elems[index];
    }

    // Removes a given number of elements from the given index, unshifting the successive elements.
    void Remove(size_t index, size_t removeCount) {
        Assert();
        assert(index < count);
        assert(removeCount <= count - index);

        size_t shiftEnd = count - removeCount;
        for (size_t i = index; i != shiftEnd; i++) {
            elems[i] = elems[i + removeCount];
        }
        count -= removeCount;
    }
};

#endif
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

//typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;

// A dynamic array.
template<class T>
struct MkList2 {
    ulong count; // the current number of elements in the array
    ulong capacity; // the current array capacity
    ulong growCount; // the number of element slots to add when expanding the array
    T * elems; // the actual element slots

    // Initialize the array.
    MkList2(ulong growCount) {
        Init(growCount);
    }

    // Initialize the array.
    void Init(ulong growCount) {
        assert(growCount != 0);

        count = 0;
        capacity = 0;
        this->growCount = growCount;
        elems = nullptr;
    }

    void Assert() {
        assert(count <= capacity);
        assert(growCount != 0);
        assert(elems || capacity == 0);
    }

    // Clear the array, freeing all its allocated memory.
    void Clear() {
        Assert();

        count = 0;
        capacity = 0;
        if (elems) {
            free(elems);
        }
    }

    // Explicitly set the array capacity.
    // Returns FALSE on memory allocation failure.
    bool SetCapacity(ulong newCapacity) {
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

    // Create space for a number of new elements, shifting the successive elements.
    // Pass ULONG_MAX as the index to append the new elements to the end of the array.
    // Returns a pointer to the first new element slot or NULL on memory allocation failure.
    T * Insert(ulong index, ulong shiftCount) {
        Assert();
        assert(index <= count || index == ULONG_MAX);

        ulong newCount = count + shiftCount;
        if (newCount > capacity) {
            ulong newCapacity = ((newCount / growCount) + 1) * growCount;
            if (!SetCapacity(newCapacity)) {
                return nullptr;
            }
        }

        if (index == ULONG_MAX) {
            index = count;
        }
        count = newCount;

        ulong shiftEnd = index + shiftCount;
        for (ulong i = count - 1; i >= shiftEnd; i--) {
            elems[i] = elems[i - shiftCount];
        }

        return &elems[index];
    }

    // Remove elements from the array, unshifting the successive elements.
    void Remove(ulong index, ulong unshiftCount) {
        Assert();
        assert(index < count);
        assert(unshiftCount <= count - index);

        ulong shiftEnd = count - unshiftCount;
        for (ulong i = index; i != shiftEnd; i++) {
            elems[i] = elems[i + unshiftCount];
        }
        count -= unshiftCount;
    }
};

#endif
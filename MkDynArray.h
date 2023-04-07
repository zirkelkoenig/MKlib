/*
A convenient dynamic array that can be treated just like a normal C array by the usage code.

Usage:
- You must define and set pointers to memory management functions.
- None of the functions accept NULL pointers, except where explicitly stated.
- Assertions are used to check input parameters.
- DO NOT PASS NORMAL C ARRAYS! Doing so will most certainly cause memory corruption.
- Do not try to free a dynamic array yourself. Doing so will probably crash your program or lead to other undefined
  behaviour.
*/

//----------------
// DEFINITIONS
//----------------

#ifndef MK_DYN_ARRAY
#define MK_DYN_ARRAY

#ifndef __cplusplus
#include <stddef.h>
#endif

typedef void * (*MkDynArray_MemAlloc_Cb)(size_t size);
typedef void * (*MkDynArray_MemRealloc_Cb)(void * pointer, size_t size);
typedef void (*MkDynArray_MemFree_Cb)(void * pointer);
typedef void (*MkDynArray_MemCopy_Cb)(void * dest, const void * source, size_t size);

#ifdef __cplusplus
extern "C"
{
#endif

    //-------------------
    // MEMORY MANAGEMENT
    //-------------------

    extern MkDynArray_MemAlloc_Cb MkDynArray_MemAlloc;
    extern MkDynArray_MemRealloc_Cb MkDynArray_MemRealloc;
    extern MkDynArray_MemFree_Cb MkDynArray_MemFree;
    extern MkDynArray_MemCopy_Cb MkDynArray_MemCopy;

    //-------------------
    // PUBLIC INTERFACE
    //-------------------

    // T * MkDynArray_Create(T, const size_t capacity)
    //
    // Create a dynamic array of a given type and an initial capacity. The capacity will also act as the grow count.
    // NOTE: You are not allowed to pass 0 as the capacity!
    // Returns NULL if memory allocation failed.
    #define MkDynArray_Create(T, capacity) (T *)MkDynArray_Create_Impl(sizeof(T), capacity);

    // void MkDynArray_Destroy(T * array)
    // 
    // Destroy a dynamic array.
    void MkDynArray_Destroy(void * array);

    // size_t MkDynArray_Count(T * array)
    // 
    // Get the current element count of the array.
    size_t MkDynArray_Count(void * array);

    // int MkDynArray_Add(T ** array, T elem)
    //
    // Add an element to the end of the dynamic array, resizing it if necessary.
    // Returns 0 if memory allocation failed.
    #define MkDynArray_Add(array, elem) MkDynArray_Increment_Impl(array) ? ((*(array))[MkDynArray_Count(*(array)) - 1] = (elem), 1) : 0

    //--------------------------
    // IMPLEMENTATION
    //--------------------------

    void * MkDynArray_Create_Impl(const size_t elemSize, const size_t capacity);
    int MkDynArray_Increment_Impl(void ** array);

#ifdef __cplusplus
}
#endif

#endif

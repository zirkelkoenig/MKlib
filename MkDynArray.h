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

#ifndef MK_DYN_ARRAY_HEADER
#define MK_DYN_ARRAY_HEADER

#include <stddef.h>
#include "MkLib.h"

// T * MkLib_DynArray_Create(T, const size_t capacity, const size_t growCount)
//
// Create a dynamic array of a given type, an initial capacity and a grow count.
// NOTE: You are not allowed to pass 0 for capacity or grow count!
// Returns NULL if memory allocation failed.
#define MkLib_DynArray_Create(T, capacity, growCount) (T *)MkLib_DynArray_Create_Impl(sizeof(T), capacity, growCount);

// void MkLib_DynArray_Destroy(T * array)
// 
// Destroy a dynamic array.
void MkLib_DynArray_Destroy(void * array);

// size_t MkLib_DynArray_Count(T * array)
// 
// Get the current element count of the array.
size_t MkLib_DynArray_Count(void * array);

// int MkLib_DynArray_SetCount(T ** array, const size_t count)
//
// Set the element count of the array and allocate more memory if necessary.
// Returns 0 if memory allocation failed.
int MkLib_DynArray_SetCount(void ** array, const size_t count);

// int MkLib_DynArray_Push(T ** array, T elem)
//
// Add an element to the end of the dynamic array, resizing it if necessary.
// Returns 0 if memory allocation failed.
#define MkLib_DynArray_Push(array, elem) (MkLib_DynArray_Increment_Impl(array) ? ((*(array))[MkLib_DynArray_Count(*(array)) - 1] = (elem), 1) : 0)

// int MkLib_DynArray_Insert(T ** array, const size_t index, T elem)
//
// Insert an element at the given position of the dynamic array, resizing it if necessary.
// Returns 0 if memory allocation failed.
#define MkLib_DynArray_Insert(array, index, elem) (MkLib_DynArray_Shift_Impl(array, index) ? ((*(array))[index] = (elem), 1) : 0)

// void MkLib_DynArray_Remove(T * array, const size_t index)
//
// Removes an element from the given position of the dynamic array.
void MkLib_DynArray_Remove(void * array, const size_t index);

//--------------------------
// IMPLEMENTATION
//--------------------------

void * MkLib_DynArray_Create_Impl(const size_t elemSize, const size_t capacity, const size_t growCount);
int MkLib_DynArray_Increment_Impl(void ** array);
int MkLib_DynArray_Shift_Impl(void ** array, const size_t index);

#endif

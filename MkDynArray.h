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

// T * MkDynArray_Create(T, const size_t capacity)
//
// Create a dynamic array of a given type and an initial capacity. The capacity will also act as the grow count.
// NOTE: You are not allowed to pass 0 as the capacity!
// Returns NULL if memory allocation failed.
#define MkLib_DynArray_Create(T, capacity) (T *)MkLib_DynArray_Create_Impl(sizeof(T), capacity);

// void MkDynArray_Destroy(T * array)
// 
// Destroy a dynamic array.
void MkLib_DynArray_Destroy(void * array);

// size_t MkDynArray_Count(T * array)
// 
// Get the current element count of the array.
size_t MkLib_DynArray_Count(void * array);

// int MkDynArray_Add(T ** array, T elem)
//
// Add an element to the end of the dynamic array, resizing it if necessary.
// Returns 0 if memory allocation failed.
#define MkLib_DynArray_Add(array, elem) MkLib_DynArray_Increment_Impl(array) ? ((*(array))[MkLib_DynArray_Count(*(array)) - 1] = (elem), 1) : 0

//--------------------------
// IMPLEMENTATION
//--------------------------

void * MkLib_DynArray_Create_Impl(const size_t elemSize, const size_t capacity);
int MkLib_DynArray_Increment_Impl(void ** array);

#endif

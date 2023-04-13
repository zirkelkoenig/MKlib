/*
General definitions used by all library modules.

NOTE: You must define and set pointers to memory management functions.
*/

#ifndef MK_LIB_HEADER
#define MK_LIB_HEADER

#include <stddef.h>

typedef unsigned char byte;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef char i8;
typedef short i16;
typedef long i32;
typedef long long i64;

typedef void * (*MkLib_MemAlloc_Cb)(size_t size);
typedef void * (*MkLib_MemRealloc_Cb)(void * pointer, size_t size);
typedef void (*MkLib_MemFree_Cb)(void * pointer);
typedef void (*MkLib_MemCopy_Cb)(void * dest, const void * source, size_t size);
typedef void (*MkLib_MemMove_Cb)(void * dest, const void * source, size_t size);

//----------------------
// MEMORY MANAGEMENT
//----------------------
extern MkLib_MemAlloc_Cb MkLib_MemAlloc;
extern MkLib_MemRealloc_Cb MkLib_MemRealloc;
extern MkLib_MemFree_Cb MkLib_MemFree;
extern MkLib_MemCopy_Cb MkLib_MemCopy;
extern MkLib_MemMove_Cb MkLib_MemMove;

#endif
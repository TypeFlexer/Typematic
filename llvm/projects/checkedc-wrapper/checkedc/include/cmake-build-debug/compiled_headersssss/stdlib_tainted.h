//---------------------------------------------------------------------//
// Bounds-safe interfaces for functions in stdlib.h that               //
// take pointer arguments.                                             //
//                                                                     //
// These are listed in the same order that they occur in the C11       //
// specification.                                                      //
/////////////////////////////////////////////////////////////////////////

//will later change this to tainted -c
#ifdef __checkedc
#ifndef __STDLIB_TAINTED_H
#define __STDLIB_TAINTED_H


#include <stddef.h>
static void* c_malloc(size_t size);
static void* c_realloc(_TArray_ptr<void> pointer, size_t size);
static void c_free(_TArray_ptr<void>pointer);
static int c_isTaintedPointerToTaintedMem(_TArray_ptr<void> pointer);
static int c_isPointerToTaintedMem(void* pointer);
static void* c_fetch_sandbox_address();
static unsigned int c_fetch_pointer_offset(_TArray_ptr<void> pointer_name);
static unsigned int c_fetch_pointer_from_offset(unsigned long pointer_offset);
static unsigned long c_fetch_sandbox_heap_address();

// TODO: express alignment constraints once where clauses have been added.
_Itype_for_any(T) _TArray_ptr<T> t_malloc(size_t size)
{
        void* temp = (void*)c_malloc(size);
        return (_TArray_ptr<T>) temp;
};

_Itype_for_any(T) void t_free(_TArray_ptr<T> pointer)
{
      return c_free(pointer);
};

_Itype_for_any(T) _TArray_ptr<T> t_realloc(_TArray_ptr<T> pointer, size_t size)
{
  return (_TArray_ptr<T>)c_realloc(pointer, size);
};


#endif  // guard
#endif  // Tainted C

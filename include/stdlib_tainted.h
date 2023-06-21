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

#pragma TLIB_SCOPE push
#pragma TLIB_SCOPE on

#include <stddef.h>

// TODO: express alignment constraints once where clauses have been added.
//#ifdef WASM_SBX
_Itype_for_any(T) _TArray_ptr<T> t_malloc(size_t size);

_Itype_for_any(T) void t_free(_TPtr<T> pointer);

_Itype_for_any(T) _TArray_ptr<T> t_realloc(_TPtr<T> pointer, size_t size);

_Itype_for_any(T) _TArray_ptr<T> t_calloc(size_t nmemb, size_t size);
//#elif HEAP_SBX

extern _Itype_for_any(T) _TPtr<T> hoard_malloc(size_t size);
extern _Itype_for_any(T) void hoard_free(_TPtr<T> ptr);
extern _Itype_for_any(T) _TArray_ptr<T> hoard_realloc(_TPtr<T> pointer, size_t size);

extern _Itype_for_any(T) _TArray_ptr<T> hoard_calloc(size_t nmemb, size_t size);
extern int isPointerinHeap(void *p);
extern int CacheUpdateandCheck(void *p);
extern _Tainted void registerCallback(_TPtr<void> callbackFunc);
extern _Tainted void unregisterCallback(_TPtr<void> callbackFunc);
//#endif

/* Convert a string to a floating-point number.  */
extern double t_strtod (_TPtr<char> __restrict __nptr,
                       _Ptr<_TPtr<char>>__restrict __endptr);
_TLIB int t_system(const char *s : itype(_TPtr<const char>));
#pragma TLIB_SCOPE pop
#endif  // guard
#endif  // Tainted C

//--------------------------------------------------------------------------//
// Additional bounds-safe interface options for functions that have         //
// different interfaces for array_ptr and nt_array_ptr args. Programmer can //
// choose to use these over the default bounds-safe interfaces.             //
//////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <stdlib_checked.h>
#include <string_checked.h>
#include "stdio_tainted.h"
#include "stdlib_tainted.h"
#include "string_tainted.h"

#  define SIZE_MAX		(18446744073709551615UL)
#ifdef __checkcbox

#ifndef __TAINTED_EXTENSIONS_H
#define __TAINTED_EXTENSIONS_H

#pragma TLIB_SCOPE push
#pragma TLIB_SCOPE on
int c_isPointerToTaintedMem(void* pointer);

void* c_fetch_pointer_from_offset(const unsigned int pointer_offset);

void* c_ConditionalTaintedOff2Ptr(const unsigned long pointer_offset);

extern int c_isTaintedPointerToTaintedMem(void* pointer);

void* c_fetch_sandbox_address();

unsigned int c_fetch_pointer_offset(void* pointer);

unsigned long c_fetch_sandbox_heap_address();

void* c_fetch_function_pointer(const char* const function_name);

unsigned long c_fetch_function_pointer_offset(const char* const function_name);

void* c_fetch_pointer_from_sandbox(unsigned int);
int sbx_register_callback(const void* chosen_interceptor, int no_of_args,
                          int does_return, int arg_types[]);


#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE on

// default strncmp has a bounds-safe interface tnt_array_ptr;
// this option is for tarray_ptr
 int __attribute__((__always___))
t_strncmp_array_ptr(_TArray_ptr<const char> src : count(n), _TArray_ptr<const char> s2 : count(n), size_t n);

// default snprintf assumes nt_array_ptr for bounds-safe interface
// this option is for array_ptr
// clang does not  functions that use va_list/va_start/va_end to
// access variable number of arguments.
_Unchecked  static int
t_snprintf_array_ptr( _TArray_ptr<char> restrict s : count(n),
                       size_t n,
                       _TNt_array_ptr<const char> restrict format,
                       ...);
_TLIB  static _TNt_array_ptr<char> string_tainted_malloc(size_t sz) : count(sz) _Unchecked{
#ifdef WASM_SBX
  if(sz >= SIZE_MAX)
    return NULL;
  _TArray_ptr<char> p : count(sz+1) = (_TArray_ptr<char>)t_malloc(sz + 1);
  if (p != NULL)
    p[sz] = 0;
  return _Tainted_Assume_bounds_cast<_TNt_array_ptr<char>>(p, count(sz));
#elif HEAP_SBX
  if(sz >= SIZE_MAX)
  return NULL;
  _TArray_ptr<char> p : count(sz+1) = (_TArray_ptr<char>)hoard_malloc(sz + 1);
  if (p != NULL)
  p[sz] = 0;
  return _Tainted_Assume_bounds_cast<_TNt_array_ptr<char>>(p, count(sz));
#else
  return (_TNt_array_ptr<char>)malloc(sz);
#endif
}

_TLIB  static _Ptr<char> StrMalloc(size_t sz)
     _Unchecked {
  if (sz >= SIZE_MAX)
    return NULL;
  char *p = (char *)malloc<char>(sz + 1);
  if (p != NULL)
    p[sz] = 0;
  return _Assume_bounds_cast<_Nt_array_ptr<char>>(p, count(sz));
}

static _TPtr<char> TNtStrRealloc(_TPtr<char> old_mem, size_t sz)
    _Unchecked {
#ifdef WASM_SBX
  if (sz >= SIZE_MAX)
    return NULL;
  _TPtr<char> p = t_realloc<char>(old_mem, sz + 1);
  if (p != NULL)
    p[sz] = 0;
  return p;
#elif HEAP_SBX
  if (sz >= SIZE_MAX)
    return NULL;
  _TPtr<char> p = hoard_realloc<char>(old_mem, sz + 1);
  if (p != NULL)
    p[sz] = 0;
  return p;
#else
        return (_TPtr<char>)realloc((void*)old_mem, sz);
#endif
}
static _TPtr<char> GlobalTaintedAdaptorStr = NULL;

_TLIB  static _TPtr<char> StaticStrToTStr(char* Ip)
{
#ifdef WASM_SBX
        size_t len = strlen(Ip);
        GlobalTaintedAdaptorStr = string_tainted_malloc(len);
        t_strcpy(GlobalTaintedAdaptorStr, Ip);
        return GlobalTaintedAdaptorStr;
#elif HEAP_SBX
        size_t len = strlen(Ip);
        GlobalTaintedAdaptorStr = string_tainted_malloc(len);
        t_strcpy(GlobalTaintedAdaptorStr, Ip);
        return GlobalTaintedAdaptorStr;
#else
        return (_TPtr<char>)Ip;
#endif
}

_TLIB _Unchecked
     static _TPtr<char>
        StaticCheckedToTStrAdaptor(_Nt_array_ptr<char> Ip )
{
#ifdef WASM_SBX
  int Iplen = strlen((const char*)Ip);
//  BytesMarshalled += Iplen;
//  printf("BytesMarshalled = %f", BytesMarshalled);
  _TPtr<char> retVal = TNtStrRealloc(GlobalTaintedAdaptorStr, Iplen);
  t_strcpy(c_fetch_pointer_from_offset((int)retVal), (const char*)Ip);
  return retVal;
#elif HEAP_SBX
  int Iplen = strlen((const char*)Ip);
  _TPtr<char> retVal = TNtStrRealloc(GlobalTaintedAdaptorStr, Iplen);
  t_strcpy(retVal, (const char*)Ip);
  return retVal;
#else
        return (_TPtr<char>)Ip;
#endif
}

_TLIB _Unchecked
     static _TPtr<char>
    StaticUncheckedToTStrAdaptor(char* Ip, size_t len)
{
#ifdef WASM_SBX
  if (len < 0)
    return NULL;
  int Iplen = len;
  _TPtr<char> Tptr = NULL;
//  BytesMarshalled += Iplen;
//  printf("BytesMarshalled = %f", BytesMarshalled);
  Tptr = string_tainted_malloc(Iplen);
  t_memcpy(c_fetch_pointer_from_offset((int)Tptr), Ip, len);
  return Tptr;
#elif HEAP_SBX
  if (len < 0)
          return NULL;
  int Iplen = len;
  _TPtr<char> Tptr = NULL;
  Tptr = string_tainted_malloc(Iplen);
  t_memcpy(Tptr, Ip, len);
  return Tptr;
#else
        return ( _TPtr<char>)Ip;
#endif
}


_TLIB  static _TNt_array_ptr<char> TNtStrMalloc(size_t sz) : count(sz) _Unchecked{
#ifdef WASM_SBX
  if(sz >= SIZE_MAX)
    return NULL;
  _TArray_ptr<char> p : count(sz+1) = (_TArray_ptr<char>)t_malloc<char>(sz + 1);
  if (p != NULL)
    p[sz] = 0;
  return _Tainted_Assume_bounds_cast<_TNt_array_ptr<char>>(p, count(sz));
#elif HEAP_SBX
  if(sz >= SIZE_MAX)
    return NULL;
  _TArray_ptr<char> p : count(sz+1) = (_TArray_ptr<char>)hoard_malloc(sz + 1);
  if (p != NULL)
    p[sz] = 0;
  return _Tainted_Assume_bounds_cast<_TNt_array_ptr<char>>(p, count(sz));
#else
        return (_TNt_array_ptr<char> )malloc(sz);
#endif
}

_TLIB _Unchecked  static _TPtr<char> CheckedToTaintedStrAdaptor(const char* Ip :
                                                               itype(_Nt_array_ptr<const char>))
{
#ifdef WASM_SBX
  int Iplen = strlen(Ip);
  _TPtr<char> RetPtr = string_tainted_malloc(Iplen*sizeof(char));
//  BytesMarshalled += Iplen;
//  printf("BytesMarshalled = %f", BytesMarshalled);
  t_strcpy(RetPtr, (const char*)Ip);
  return RetPtr;
#elif HEAP_SBX
  int Iplen = strlen(Ip);
  _TPtr<char> RetPtr = string_tainted_malloc(Iplen*sizeof(char));
  t_strcpy(RetPtr, (const char*)Ip);
  return RetPtr;
#else
        return (_TPtr<char>)Ip;
#endif
}


_TLIB  static _Ptr<char> TaintedToCheckedStrAdaptor(_TPtr<char> Ip, size_t len)
{
#ifdef WASM_SBX
  int Iplen = len;
  if (Iplen == 0)
    return NULL;
  _Ptr<char> RetPtr = (_Ptr<char>)malloc<char>(Iplen*sizeof(char));
  t_strcpy((char*)RetPtr, Ip);
  return RetPtr;
#elif HEAP_SBX
  int Iplen = len;
  if (Iplen == 0)
    return NULL;
  _Ptr<char> RetPtr = (_Ptr<char>)malloc<char>(Iplen*sizeof(char));
  t_strcpy((char*)RetPtr, Ip);
  return RetPtr;
#else
        return (_Ptr<char>)Ip;
#endif
}
#pragma CHECKED_SCOPE pop
#pragma TLIB_SCOPE pop


#endif // guard 
#endif // TAINTED C

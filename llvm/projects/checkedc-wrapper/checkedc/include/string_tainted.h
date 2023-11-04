//---------------------------------------------------------------------//
// Bounds-safe interfaces for functions in string.h that               //
// take pointer arguments.                                             //
//                                                                     //
// These are listed in the same order that they occur in the C11       //
// specification.                                                      //
//                                                                     //
// TODO: revise string types after support for pointers to             //
// null-terminated arrays is added to C.                               //
//                                                                     //
// TODO: Better Support for _FORTIFY_SOURCE > 0                        //
/////////////////////////////////////////////////////////////////////////

#ifdef __checkcbox
#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE off
#endif

#include_next <string.h>

#ifdef __checkcbox
#pragma CHECKED_SCOPE pop
#endif

#ifdef __checkcbox
#ifndef __STRING_TAINTED_H
#define __STRING_TAINTED_H

#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE on

// GCC has macros that it uses as part of its string implementation to optimize cases
// where one or both strings are compile-time constants.  I'm not sure
// why they put this logic into macros instead of the compiler because the
// compiler can recognize these cases in more contexts than a macro, but they
// did.
//
// For now, undefine the various macros.  GCC has a #define for turning off
// this feature, but that must be set before string.h is included and we don't
// control when that happens (string.h may already have been included before
// this file is ever included).
#if defined(__GNUC__)
#undef t_strchr
#undef t_strcmp
#undef t_strcspn
#undef t_strncmp
#undef t_strncmp
#undef t_strpbrk
#undef t_strspn
#undef t_strdup
#endif

// We wrap each definition in a complex conditional, there two boolean values:
// - we are fortifying, or we're not (_FORTIFY_SOURCE==0 is not fortifying)
// - there is or there isn't a macro hash-defining this symbol (defined(symbol))
// Cases:
// - Fortifying,     Macro Exists: this is expected, we don't need the definition
// - Not Fortifying, Macro Exists: we need the definition, we need to undef macro
// - Fortifying,     No Macro:     we need the definition
// - Not Fortifying, No Macro:     we need the definition

#if _FORTIFY_SOURCE == 0 || !defined(t_memcpy)
#undef t_memcpy
_Itype_for_any(T) _TArray_ptr<T> t_memcpy(_TArray_ptr<T> restrict dest : byte_count(n),_TArray_ptr<const T> restrict src : byte_count(n),
             size_t n) : byte_count(n) ;
#endif

#if _FORTIFY_SOURCE == 0 || !defined(t_memmove)
#undef t_memmove
_Itype_for_any(T) _TArray_ptr<T> t_memmove(_TArray_ptr<T> dest : byte_count(n),
                  _TArray_ptr<const T> src : byte_count(n),
              size_t n) : byte_count(n);
#endif

#if _FORTIFY_SOURCE == 0 || !defined(t_memset)
#undef t_memset
_Itype_for_any(T) _TArray_ptr<T> t_memset(_TArray_ptr<T> dest : byte_count(n),
                                            int c,
                                            size_t n) : byte_count(n);
#endif

#if _FORTIFY_SOURCE == 0 || !defined(t_strcpy)
#undef t_strcpy
_TArray_ptr<char> t_strcpy(_TArray_ptr<char> restrict dest,
               const char* restrict src : itype(restrict _TArray_ptr<const char>));
#endif

#if _FORTIFY_SOURCE == 0 || !defined(t_strncpy)
#undef t_strncpy
_TArray_ptr<char> t_strncpy(_TArray_ptr<char> dest : byte_count(n),
                const char* src : itype(_TArray_ptr<const char>) byte_count(n),
              size_t n) : count(n);
#endif

#if _FORTIFY_SOURCE == 0 || !defined(t_strcat)
#undef t_strcat
_TArray_ptr<char> t_strcat(_TArray_ptr<char> restrict dest,
                             _TArray_ptr<const char> restrict src);
#endif

#if _FORTIFY_SOURCE == 0 || !defined(t_strncat)
#undef t_strncat
_TArray_ptr<char> t_strncat(_TArray_ptr<char> restrict dest,
                          _TArray_ptr<const char> restrict src : count(n),
              size_t n);
#endif

_Itype_for_any(T) int t_memcmp(_TArray_ptr<const T> src1 : byte_count(n), _TArray_ptr<const T> src2 : byte_count(n),
           size_t n);

int t_strcmp(_TNt_array_ptr<const char> src1,
           _TNt_array_ptr<const char> src2);

int t_strcoll(_TNt_array_ptr<const char> src1,
            _TNt_array_ptr<const  char> src2);

// strncmp takes possibly null-terminated strings as arguments and checks
// up to n characters. For a bounds-safe interface, this means each string
// needs to be EITHER null-terminated OR have bounds greater than or equal to
// n. We cannot express them both in a single interface, so this is the
// interface for null-terminated strings (assumed to be the most common case).
// In the checkedc_extensions.h header there is a bounds-safe interface for
// use of _TArray_ptr rather than _TNt_array_ptr.
int t_strncmp(_TNt_array_ptr<const char> src,
            _TNt_array_ptr<const char> s2,
            size_t n);

size_t t_strxfrm(_TArray_ptr<char> restrict dest : count(n),
                 _TArray_ptr<const char> restrict src,
               size_t n);

_Itype_for_any(T) _TArray_ptr<T> t_memchr(_TArray_ptr<T> s : byte_count(n), int c, size_t n) :
  byte_count(n);

_TNt_array_ptr<char> t_strchr(_TNt_array_ptr<const char> s, int c);

size_t strcspn(_TNt_array_ptr<const char> s1,
               _TNt_array_ptr<const char> s2);

_TNt_array_ptr<char> t_strpbrk(_TNt_array_ptr<const char> s1,
                              _TNt_array_ptr<const char> s2);

_TNt_array_ptr<char> t_strrchr(_TNt_array_ptr<const char> s, int c);
size_t t_strspn(_TNt_array_ptr<const char> s1,
              _TNt_array_ptr<const char> s2);

_TNt_array_ptr<char> t_strstr(_TNt_array_ptr<const char>s1,
                             _TNt_array_ptr<const char> s2);
_TNt_array_ptr<char> t_strtok(_TNt_array_ptr<char> restrict s1,
                           _TNt_array_ptr<const char> s2);

_TNt_array_ptr<char> t_strerror(int errnum);
size_t strlen(_TNt_array_ptr<const char> s);

_TNt_array_ptr<char> t_strdup(_TNt_array_ptr<const char> s);

#include "_builtin_string_tainted.h"

#pragma CHECKED_SCOPE pop

#endif // guard
#endif // Tainted C

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
#undef tc_strcpy
#undef ct_strcpy
#undef t_strcpy
#undef t_strcat
#undef t_strncat
#undef t_strncpy
#endif

// We wrap each definition in a complex conditional, there two boolean values:
// - we are fortifying, or we're not (_FORTIFY_SOURCE==0 is not fortifying)
// - there is or there isn't a macro hash-defining this symbol (defined(symbol))
// Cases:
// - Fortifying,     Macro Exists: this is expected, we don't need the definition
// - Not Fortifying, Macro Exists: we need the definition, we need to undef macro
// - Fortifying,     No Macro:     we need the definition
// - Not Fortifying, No Macro:     we need the definition
#pragma TLIB_SCOPE push
#pragma TLIB_SCOPE on

#if _FORTIFY_SOURCE == 0 || !defined(t_memcpy)
#undef t_memcpy
_TLIB _Itype_for_any(T) _TPtr<T> t_memcpy(void* dest : itype(_TPtr<T>),void* src : itype(_TPtr<const T>),
                                            size_t n) ;
#endif

#if _FORTIFY_SOURCE == 0 || !defined(t_memmove)
#undef t_memmove
_Itype_for_any(T) _TPtr<T> t_memmove(void* dest : itype(_TPtr<T>),
                                           const void* src : itype(_TPtr<const T>),
                                             size_t n);
#endif

_TLIB static int t_strncasecmp(const char *src : itype(_TPtr<const char>),
                  const char *s2 : itype(_TPtr<const char>),
                  size_t n){return strncasecmp(src,s2,n);};


#if _FORTIFY_SOURCE == 0 || !defined(t_memset)
#undef t_memset
_TLIB _Itype_for_any(T) _TArray_ptr<T> t_memset(_TPtr<T> dest,
                                            int c,
                                            size_t n) : byte_count(n);
#endif

#if _FORTIFY_SOURCE == 0 || !defined(t_strcpy)
#undef t_strcpy
_TLIB _TPtr<char> t_strcpy(char* restrict dest :itype(restrict _TPtr<char>),
                           const char* restrict src : itype(restrict _TPtr<const char>));
#endif

#if _FORTIFY_SOURCE == 0 || !defined(tc_strcpy)
#undef tc_strcpy
_TArray_ptr<char> tc_strcpy(_TNt_array_ptr<char> restrict dest :itype(restrict _Nt_array_ptr<char> ),
                              _Nt_array_ptr<const char> restrict src : itype(restrict _TNt_array_ptr<const char>));
#endif

#if _FORTIFY_SOURCE == 0 || !defined(ct_strcpy)
#undef ct_strcpy
_TArray_ptr<char> ct_strcpy(_Nt_array_ptr<char> restrict dest,
                              _TNt_array_ptr<const char> restrict src);
#endif

#if _FORTIFY_SOURCE == 0 || !defined(t_strncpy)
#undef t_strncpy
_TLIB _TArray_ptr<char> t_strncpy(char* dest : itype(_TPtr<char>),
                              const char* src : itype(_TPtr<const char>),
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

_TLIB _Itype_for_any(T) int t_memcmp(const void* src1 : itype(_TPtr<const T>), const void* src2 : itype(_TPtr<const T>)  ,
                                 size_t n);

_TLIB int t_strcmp(const char *src1 : itype(_TPtr<const char>),
             const char *src2 : itype(_TPtr<const char>));

_TLIB int t_strcoll(_TNt_array_ptr<const char> src1,
              _TNt_array_ptr<const  char> src2);

// strncmp takes possibly null-terminated strings as arguments and checks
// up to n characters. For a bounds-safe interface, this means each string
// needs to be EITHER null-terminated OR have bounds greater than or equal to
// n. We cannot express them both in a single interface, so this is the
// interface for null-terminated strings (assumed to be the most common case).
// In the checkedc_extensions.h header there is a bounds-safe interface for
// use of _TArray_ptr rather than _TNt_array_ptr.
int t_strncmp(const char *src : itype(_TPtr<const char>),
                const char *s2 : itype(_TPtr<const char>),
              size_t n);

size_t t_strxfrm(_TArray_ptr<char> restrict dest : count(n),
                   _TArray_ptr<const char> restrict src,
                   size_t n);

_Itype_for_any(T) _TPtr<T> t_memchr(_TPtr<T> s , int c, size_t n);

_TPtr<char> t_strchr(const char* s : itype(_TPtr<const char>), int c);

size_t t_strcspn( const char* s1  : itype(_TPtr<const char>),
                   const char* s2  : itype(_TPtr<const char>)) ;
extern _TPtr<char> t_strndup (_TPtr<const char> __string, size_t __n);

_TPtr<char> t_strpbrk(const char *s1 : itype(_TPtr<const char>),
                        const char *s2 : itype(_TPtr<const char>));

_TPtr<char> t_strrchr(_TPtr<const char> s, int c);
size_t t_strspn(const char *s1 : itype(_TPtr<const char>),
                                        const char *s2 : itype(_TPtr<const char>));

_TPtr<char> t_strstr(const char *s1 : itype(_TPtr<const char>),
                              const char *s2 : itype(_TPtr<const char>));
_TPtr<char> t_strtok(const char  * restrict s1 : itype(_TPtr<const char> restrict),
                              const char  * restrict s2 : itype(_TPtr<const char> restrict));

_TNt_array_ptr<char> t_strerror(int errnum);

size_t t_strlen(_TPtr<const char> s);

_TPtr<char> t_strdup(_TPtr<const char> s);
#pragma TLIB_SCOPE pop
#include "_builtin_string_tainted.h"

#pragma CHECKED_SCOPE pop

#endif // guard
#endif // Tainted C

//---------------------------------------------------------------------//
// Bounds-safe interfaces for compiler-defined builtin functions       //
// corresponding to string.h functions                                 //
//                                                                     //
// These are given in the order they appear in clang's Builtins.def.   //
// Functions that do not appear can not have checked interfaces        //
// defined.                                                            //
//                                                                     //
// These are based on the types as declared within clang               //
// and https://gcc.gnu.org/onlinedocs/gcc/Object-Size-Checking.html    //
/////////////////////////////////////////////////////////////////////////

#include "_builtin_common.h"

#if _USE_FORTIFY_LEVEL > 0

#ifndef __has_builtin
#define _undef__has_builtin
#define __has_builtin(x) 0
#endif

#if __has_builtin(__builtin___memcpy_chkcbx) || defined(__GNUC__)
_Itype_for_any(T) _TArray_ptr<T> __t_builtin___memcpy_chk(_TArray_ptr<T> restrict dest : byte_count(n),
                                                          _TArray_ptr<const T> restrict src : byte_count(n),
                             size_t n,
                             size_t obj_size) : byte_count(n);
#endif

#if __has_builtin(__builtin___memmove_chkcbx) || defined(__GNUC__)
_Itype_for_any(T) _TArray_ptr<T> __t_builtin___memmove_chk(_TArray_ptr<T> restrict dest : byte_count(n),
                                                             _TArray_ptr<const T> restrict src : byte_count(n),
                             size_t n,
                             size_t obj_size) : byte_count(n);
#endif

#if __has_builtin(__builtin___memset_chkcbx) || defined(__GNUC__)
_Itype_for_any(T) _TArray_ptr<T> __t_builtin___memset_chk(_TArray_ptr<T> s : byte_count(n),
                            int c,
                            size_t n,
                            size_t obj_size) : byte_count(n);
#endif

#if __has_builtin(__builtin___strcat_chkcbx) || defined(__GNUC__)
_TArray_ptr<char> __t_builtin___strcat_chk(_TNt_array_ptr<char> restrict dest,
                                           _Nt_array_ptr<const char> restrict src,
                             size_t obj_size);
#endif

#if __has_builtin(__builtin___strcpy_chk) || defined(__GNUC__)
_TArray_ptr<char> __t_builtin___strcpy_chk(_TNt_array_ptr<char> restrict dest,
                                           _Nt_array_ptr<const char> restrict src,
                             size_t obj_size);
#endif

#if __has_builtin(__builtin___strncat_chk) || defined(__GNUC__)
_TArray_ptr<char> __t_builtin___strncat_chk(_TNt_array_ptr<char> restrict dest,
                                            _Nt_array_ptr<const char> restrict src
                                            : count(n),
                              size_t n,
                              size_t obj_size);
#endif

#if __has_builtin(__builtin___strncpy_chk) || defined(__GNUC__)
char *__t_builtin___strncpy_chk(_TNt_array_ptr<char> restrict dest : count(n),
                                  _TNt_array_ptr<char> restrict src : count(n),
                              size_t n,
                              size_t obj_size) : count(n);
#endif

#ifdef _undef__has_builtin
#undef _undef__has_builtin
#undef __has_builtin
#endif

#endif

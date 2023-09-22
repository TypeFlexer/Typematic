//---------------------------------------------------------------------//
// Bounds-safe interfaces for compiler-defined builtin functions       //
// corresponding to stdio.h functions                                  //
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


#if __has_builtin(__builtin___sprintf_chkcbx) || defined(__GNUC__)
// sprintf
_TLIB int __t_sprintf_chkcbx(  _TNt_array_ptr<char> restrict buffer,
                  int flag,
                  size_t obj_size,
                    _TNt_array_ptr<const char> restrict format,
                  ...);

_TLIB int __t_builtin___sprintf_chkcbx( _TNt_array_ptr<char> restrict buffer,
                            int flag,
                            size_t obj_size,
                            restrict _TNt_array_ptr<const char> format,
                            ...);
#endif

#if __has_builtin(__builtin___snprintf_chkcbx) || defined(__GNUC__)
// snprintf
_TLIB int __t_snprintf_chkcbx(_TNt_array_ptr<char> restrict buffer : count(maxlen),
                   size_t maxlen,
                   int flag,
                   size_t obj_size,
                   _TNt_array_ptr<const char> restrict format,
                   ...);

_TLIB int __t_builtin___snprintf_chkcbx(_TNt_array_ptr<char> restrict buffer : count(maxlen),
                             size_t maxlen,
                             int flag,
                             size_t obj_size,
                             _TNt_array_ptr<const char> restrict format,
                             ...);
#endif

#if __has_builtin(__builtin___vsprintf_chkcbx) || defined(__GNUC__)
// vsprintf

_TLIB int __t_vsprintf_chkcbx(_TNt_array_ptr<char> restrict buffer,
                   int flag,
                   size_t obj_size,
                   restrict _TNt_array_ptr<const char> restrict format,
                   va_list);



_TLIB int __t_builtin___vsprintf_chkcbx(_TNt_array_ptr<char> restrict buffer,
                             int flag,
                             size_t obj_size,
                             _TNt_array_ptr<const char> restrict format,
                             va_list arg);
#endif


#if __has_builtin(__builtin___vsnprintf_chkcbx) || defined(__GNUC__)
// vsnprintf
_TLIB int __t_vsnprintf_chkcbx(_TNt_array_ptr<char> restrict buffer : count(maxlen),
                    size_t maxlen,
                    int flag,
                    size_t obj_size,
                    _TNt_array_ptr<const char> restrict format,
                    va_list);

_TLIB int __t_builtin___vsnprintf_chkcbx(_TNt_array_ptr<char> restrict buffer : count(maxlen),
                              size_t maxlen,
                              int flag,
                              size_t obj_size,
                              _TNt_array_ptr<const char> restrict format ,
                              va_list arg);
#endif

#if __has_builtin(__builtin___fprintf_chkcbx) || defined(__GNUC__)
// fprintf
_TLIB int __t_fprintf_chkcbx(_Ptr<FILE> restrict stream,
                  int flag,
                  _TNt_array_ptr<const char> restrict format,
                  ...);

_TLIB int __t_builtin___fprintf_chkcbx(_Ptr<FILE> restrict stream,
                            int flag,
                            _TNt_array_ptr<const char> restrict format,
                            ...);
#endif

#if __has_builtin(__builtin___printf_chkcbx) || defined(__GNUC__)
// printf
_TLIB int __t_printf_chkcbx(int flag,
                    _TNt_array_ptr<const char> restrict format,
                 ...);

_TLIB int __t_builtin___printf_chkcbx(int flag,
                              _TNt_array_ptr<const char> restrict format,
                           ...);
#endif

#if __has_builtin(__builtin___vfprintf_chkcbx) || defined(__GNUC__)
// vfprintf
_TLIB int __t_vfprintf_chkcbx(_TPtr<FILE> restrict stream,
                   int flag,
                   _TNt_array_ptr<const char> restrict format,
                   va_list);

_TLIB int __t_builtin___vfprintf_chkcbx(_TPtr<FILE> restrict stream,
                             int flag,
                             _TNt_array_ptr<const char> restrict format,
                             va_list);
#endif

#if __has_builtin(__builtin___vprintf_chkcbx) || defined(__GNUC__)
// vprintf
_TLIB int __t_vprintf_chk(int flag,
                  _TNt_array_ptr<const char> restrict format,
                  va_list);

_TLIB int __t_builtin___vprintf_chk(int flag,
                            _TNt_array_ptr<const char> restrict format,
                            va_list);
#endif

#ifdef _undef__has_builtin
#undef _undef__has_builtin
#undef __has_builtin
#endif

#endif

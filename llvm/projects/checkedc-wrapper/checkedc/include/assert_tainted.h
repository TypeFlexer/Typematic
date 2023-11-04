//---------------------------------------------------------------------//
// Bounds-safe interfaces for some functions in assert.h that          //
// take pointer arguments.                                             //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include_next <assert.h>
#include <stddef.h> // define wchar_t for wcstoimax and wcstoumax

#ifdef __checkedc
#ifndef __ASSERT_TAINTED_H
#define __ASSERT_TAINTED_H

#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE on

#if defined(_WIN32) || defined(_WIN64)
_ACRTIMP void __cdecl _t_wassert(_In_z_ _TNt_array_ptr<const wchar_t> _Message,
                               _In_z_ _TNt_array_ptr<const wchar_t> _File,
                               _In_ unsigned _Line);
#else

#ifdef __APPLE__
#define __THROW
extern void __t_assert_rtn(_TNt_array_ptr<const char> msg,
                         _TNt_array_ptr<const char> file,
			 int line,
                         _TNt_checked<const char>fn);
#else
extern void __t_assert(_TNt_array_ptr<const char> msg,
                       _TNt_array_ptr<const char> file,
					 int line);
#endif

#undef __t_assert_fail
extern void __t_assert_fail (_TNt_array_ptr<const char> __assertion, _TNt_array_ptr<const char> __file,
        unsigned int __line, _TNt_array_ptr<const char> __function)
__THROW __attribute__ ((__noreturn__));

#endif

#pragma CHECKED_SCOPE pop

#endif  // guard
#endif  // Tainted C

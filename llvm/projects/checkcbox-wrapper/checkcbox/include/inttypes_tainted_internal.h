//---------------------------------------------------------------------//
// Bounds-safe interfaces for functions in inttypes.h that             //
// take pointer arguments.                                             //
//                                                                     //
// These are listed in the same order that they occur in the C11       //
// specification.                                                      //
/////////////////////////////////////////////////////////////////////////

#ifdef __checkcbox
#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE off
#endif

#include <stddef.h> // define wchar_t for wcstoimax and wcstoumax
#include <stdint.h>
#ifdef __checkcbox
#pragma CHECKED_SCOPE pop
#endif

#ifdef __checkcbox
#ifndef __INTTYPES_TAINTED_H
#define __INTTYPES_TAINTED_H

#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE on

_TLIB intmax_t t_strtoimax(_TNt_array_ptr<const char> restrict nptr,
                     _TPtr<_TNt_array_ptr<char>>restrict endptr,
                   int base);

_TLIB uintmax_t t_strtoumax( _TNt_array_ptr<const char> restrict nptr,
                      _TPtr<_TNt_array_ptr<char >> restrict endptr,
                    int base);

_TLIB intmax_t t_wcstoimax(_TNt_array_ptr<const wchar_t> restrict nptr,
                     _TPtr<_TNt_array_ptr<wchar_t>> restrict endptr,
                   int base);

_TLIB uintmax_t t_wcstoumax(_TNt_array_ptr<const wchar_t> restrict nptr,
                    _TPtr<_TNt_array_ptr<wchar_t>> restrict endptr,
                    int base);

#pragma CHECKED_SCOPE pop

#endif // guard
#endif // Tainted C

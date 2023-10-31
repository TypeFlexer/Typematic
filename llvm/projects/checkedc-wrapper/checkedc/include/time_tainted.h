//---------------------------------------------------------------------//
// Bounds-safe interfaces for functions in time.h that                 //
// take pointer arguments.                                             //
//                                                                     //
// These are listed in the same order that they occur in the C11       //
// specification.                                                      //
/////////////////////////////////////////////////////////////////////////

#ifdef __checkcbox
#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE off
#endif

#include_next <time.h>

#ifdef __checkcbox
#pragma CHECKED_SCOPE pop
#endif

#ifdef __checkcbox
#ifndef __TIME_TAINTED_H
#define __TIME_TAINTED_H

#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE on

time_t t_mktime(_TPtr<struct tm> timeptr);

int t_timespec_get(_TPtr<struct timespec> ts,
                 int base);

_Nt_array_ptr<char> t_asctime(_TPtr<const struct tm> timeptr);

_Nt_array_ptr<char> t_ctime(_TPtr<const time_t> timer);

_TPtr<struct tm> t_gmtime(_TPtr<const time_t> timer);

_TPtr<struct tm> t_localtime(_TPtr<const time_t> timer);

size_t t_strftime(_TArray_ptr<char> restrict output : count(maxsize),
                size_t maxsize,
                  _TNt_array_ptr<const char> restrict format,
                  _TPtr<const struct tm> restrict timeptr);

#pragma CHECKED_SCOPE pop

#endif // guard
#endif // Tainted C

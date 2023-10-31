//--------------------------------------------------------------------//
// Bounds-safe interfaces for functions in errno.h                    //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifdef __checkcbox
#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE off
#endif

#include_next <errno.h>

#ifdef __checkcbox
#ifndef __ERRNO_TAINTED_H
#define __ERRNO_TAINTED_H

#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE on

#if defined(_WIN32) || defined(_WIN64)
__declspec(dllimport) _TPtr<int> __cdecl _t_errno(void);
#elif defined(__APPLE__)
extern _TPtr<int> __t_errno_location(void);
#else
extern _TPtr<int> __t_errno_location(void)  __THROW __attribute_const__;
#endif

#pragma CHECKED_SCOPE pop

#endif // guards
#endif // Tainted C

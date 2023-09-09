//---------------------------------------------------------------------//
// Bounds-safe interfaces for some functions in arpa/inet.h that       //
// take pointer arguments.                                             //
//                                                                     //
// These are POSIX-only                                                //
/////////////////////////////////////////////////////////////////////////

// The Windows environment may not have arpa/inet.h
#if defined __has_include_next
#if __has_include_next(<arpa/inet.h>)

#ifdef __checkcbox
#endif

#include_next <arpa/inet.h>

#ifdef __checkcbox
#endif

#ifdef __checkcbox
#ifndef __INET_TAINTED_H
#define __INET_TAINTED_H


extern in_addr_t t_inet_addr (_TNt_array_ptr<const char>) __THROW;


#endif // guard
#endif // Checkcbox C

#endif // has inet.h
#endif // defined __has_include_next

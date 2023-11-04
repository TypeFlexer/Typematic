//---------------------------------------------------------------------//
// Bounds-safe interfaces for functions in unistd.h that               //
// take pointer arguments.                                             //
//                                                                     //
// These are POSIX-only                                                //
/////////////////////////////////////////////////////////////////////////


// Uses clang-specific __has_include macro to detect unistd.h
// which is required by Posix Standard.
// The Windows environment also may not have unistd.h
#if defined __has_include_next
#if __has_include_next(<unistd.h>)

#ifdef __checkcbox
#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE off
#endif

#include_next <unistd.h>

#ifdef __checkcbox
#pragma CHECKED_SCOPE pop
#endif

#ifdef __checkcbox
#ifndef __UNISTD_TAINTED_H
#define __UNISTD_TAINTED_H

#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE on

#if _POSIX_VERSION >= 200112L

extern _Nt_array_ptr<_Nt_array_ptr<char>> environ;

#ifdef __APPLE__

extern int access (_Nt_array_ptr<const char> __name, int __type);
_Itype_for_any(T) extern ssize_t t_read (int __fd, _TArray_ptr<const T> __buf : byte_count(__nbytes), size_t __nbytes);
_Itype_for_any(T) extern ssize_t t_write (int __fd, _TArray_ptr<const T> __buf : byte_count(__n), size_t __n);

#else

extern int access (_Nt_array_ptr<const char> __name, int __type) __THROW __nonnull ((1));
_Itype_for_any(T) extern ssize_t t_read (int __fd, _TArray_ptr<T> __buf : byte_count(__nbytes), size_t __nbytes) __wur;
_Itype_for_any(T) extern ssize_t t_write (int __fd, _TArray_ptr<const T> __buf : byte_count(__n), size_t __n) __wur;

#endif
#endif

#pragma CHECKED_SCOPE pop

#endif // guard
#endif // Tainted C

#endif // has unistd.h
#endif // defined __has_include_next

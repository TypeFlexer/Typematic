//---------------------------------------------------------------------//
// Bounds-safe interfaces for functions in stdio.h that                //
// take pointer arguments.                                             //
//                                                                     //
// These are listed in the same order that they occur in the C11       //
// specification.                                                      //
//                                                                     //
// TODO: Better Support for _FORTIFY_SOURCE > 0                        //
/////////////////////////////////////////////////////////////////////////

#ifdef __checkcbox
#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE off
#endif

#include_next <stdio.h>

#ifdef __checkcbox
#pragma CHECKED_SCOPE pop
#endif

#ifdef __checkcbox
#ifndef __STDIO_TAINTED_H
#define __STDIO_TAINTED_H

#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE on

/*
 * Any STDLIB function that returns a FILE
 * pointer is NOT supported for tainted use in checked region as
 * these functions perform malloc operations under the hood that leads to
 * return of tainted pointers that point to checked region
 *
 * Hence to achieve this functionality, please call a generic C-stdlib function
 * from within a tainted/mirror region.
 */
//#if defined(_WIN32) || defined(_WIN64)
//// stdin, stdout, and stderr only have to be expressions that have
//// type FILE *.  On Windows, they are function calls, so we need to change
//// the return type of the function being called.
//_TPtr<FILE> __cdecl __t_acrt_iob_func(unsigned);
//#else
//_TLIB extern _TPtr<FILE> t_stdin;
//_TLIB extern _TPtr<FILE> t_stdout ;
//_TLIB extern _TPtr<FILE> t_stderr;
//#endif

_TLIB int t_remove(_TPtr<const char> name);
_TLIB int t_rename(_TPtr<const char> from,
           _TPtr<const char> to);
//_TLIB _TPtr<FILE> t_tmpfile(void);
//_TLIB _TPtr<char> t_tmpnam(_TPtr<char> source);
_TLIB int t_fclose(_TPtr<void> stream);
//_TLIB int t_fflush(_TPtr<FILE> stream);
_TLIB _Unchecked static FILE* t_fopen(_TPtr<const char> restrict filename,
            const char* restrict mode : itype(restrict _TPtr<const char>));
//_TLIB _Ptr<FILE> t_freopen(_TNt_array_ptr<const char> restrict filename,
//              _TNt_array_ptr<const char> restrict mode,
//              _Ptr<FILE> restrict stream);

//_TLIB void t_setbuf(_TPtr<FILE> restrict stream,
//              _TArray_ptr<char> restrict buf : count(BUFSIZ));
//_TLIB int t_setvbuf(_TPtr<FILE> restrict stream,
//              _TArray_ptr<char> restrict buf : count(size),
//            int mode, size_t size);


#if _FORTIFY_SOURCE == 0 || !defined(t_fprintf)
#undef t_fprintf
_TLIB _Unchecked
int t_fprintf(FILE * restrict stream,
              const char* format : itype(_TPtr<const char> ), ...);
#endif

//_TLIB _Unchecked
//int t_fscanf(_TPtr<FILE> restrict stream,
//             _TNt_array_ptr<const char> restrict format, ...);

#if _FORTIFY_SOURCE == 0 || !defined(t_printf)
#undef t_printf
_TLIB _Unchecked
int t_printf(const char * restrict format : itype(restrict _TPtr<const char>), ...);
#endif

_TLIB
_Unchecked int t_scanf(const char * restrict format : itype (restrict _TPtr<const char>), ...);

#if _FORTIFY_SOURCE == 0 || !defined(t_sprintf)
#undef t_sprintf
_TLIB _Unchecked
int t_sprintf(char* restrict s : itype(_TPtr<char> restrict),
              const char* restrict format : itype(_TPtr<const char> restrict), ...);
#endif
_TLIB _Unchecked
int t_sscanf(_TPtr<const char> restrict s,
             const char* format : itype(_TPtr<const char>), ...);

#if _FORTIFY_SOURCE == 0 || !defined(t_snprintf)
#undef t_snprintf

// Since snprintf automatically adds the null terminator
// and counts that number in n, s only needs count(n-1) per the 
// definition of _Nt types. Additional declaration for arrays 
// available in checkedc_extensions.h
_TLIB _Unchecked
int t_snprintf(char* s : itype(_TPtr<char>),
             size_t n _Where n > 0,
             const char* restrict format : itype(_TPtr<const char > restrict), ...);
#endif

#if _FORTIFY_SOURCE == 0 || !defined(t_vfprintf)
#undef t_vfprintf
_TLIB _Unchecked
int t_vfprintf(FILE* restrict stream,
               const char*  restrict format : itype(_TPtr<const char> restrict),
             va_list arg);
#endif
//_TLIB _Unchecked
//int t_vfscanf(_TPtr<FILE> restrict stream,
//              _TNt_array_ptr<const char> restrict format,
//            va_list arg);

#if _FORTIFY_SOURCE == 0 || !defined(t_vprintf)
#undef t_vprintf
_TLIB _Unchecked
int t_vprintf(_TPtr<const char> restrict format,
             va_list arg);
#endif
_TLIB _Unchecked
int t_vscanf(_TPtr<const char> restrict format,
            va_list arg);

#if _FORTIFY_SOURCE == 0 || !defined(t_vsnprintf)
#undef t_vsnprintf
_TLIB _Unchecked
int t_vsnprintf(_TPtr<char> restrict s, size_t n,
                _TPtr<char> restrict format,
                va_list arg);
#endif

#if _FORTIFY_SOURCE == 0 || !defined(t_vsprintf)
#undef t_vsprintf
_TLIB _Unchecked
int t_vsprintf(_TNt_array_ptr<char> restrict s,
               _TNt_array_ptr<char> restrict format,
               va_list arg);
#endif
_TLIB _Unchecked
int t_vsscanf(_TNt_array_ptr<const char> restrict s,
              _TNt_array_ptr<const char> restrict format,
              va_list arg);

//_TLIB int t_fgetc(_TPtr<FILE> stream);
//_TLIB int t_fputc(int c, _TPtr<FILE> stream);
//_TLIB _TArray_ptr<char> t_fgets(_TArray_ptr<char> restrict s : count(n), int n,
//                   _TPtr<FILE> restrict stream) :
//  bounds(s, s + n);
//_TLIB int t_fputs(_TPtr<const char> restrict s,
//            FILE* restrict stream : itype(restrict _TPtr<FILE>));
//_TLIB int t_getc(_TPtr<FILE> stream);
//_TLIB int t_putc(int c, _TPtr<FILE> stream);
_TLIB static int t_puts(_TPtr<const char>str);

//_TLIB int t_ungetc(int c, _TPtr<FILE> stream);
//
_TLIB _Itype_for_any(T) size_t t_fread(_TArray_ptr<T> restrict pointer : byte_count(size * nmemb),
             size_t size, size_t nmemb,
             _TPtr<void> restrict stream);
//
//_TLIB _Itype_for_any(T) size_t t_fwrite(_TArray_ptr<T> restrict pointer : byte_count(size * nmemb),
//              size_t size, size_t nmemb,
//              _TPtr<FILE> restrict stream);
//
//_TLIB int t_fgetpos(_TPtr<FILE> restrict stream,
//              _TPtr<fpos_t> restrict pos);
//
_TLIB int t_fseek(_TPtr<void> stream, long int offset, int whence);
//_TLIB int t_fsetpos(_TPtr<FILE> stream,
//              _TPtr<const fpos_t> pos);
//
_TLIB long int t_ftell(_TPtr<void> stream);
_TLIB void t_rewind(_TPtr<void> stream);
//
//_TLIB void t_clearerr(_TPtr<FILE> stream);
//_TLIB int t_feof(_TPtr<FILE> stream);
_TLIB int t_ferror(_TPtr<void> stream);

#include "_builtin_stdio_checked.h"

#pragma CHECKED_SCOPE pop

#endif // guard
#endif // Tainted C

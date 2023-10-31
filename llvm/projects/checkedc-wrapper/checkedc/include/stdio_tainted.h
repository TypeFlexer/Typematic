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
#ifndef __STDIO_TAINTED_H
#define __STDIO_TAINTED_H

#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE on

#if defined(_WIN32) || defined(_WIN64)
// stdin, stdout, and stderr only have to be expressions that have
// type FILE *.  On Windows, they are function calls, so we need to change
// the return type of the function being called.
_TPtr<FILE> __cdecl __t_acrt_iob_func(unsigned);
#else
extern _TPtr<FILE> t_stdin;
extern _TPtr<FILE> t_stdout ;
extern _TPtr<FILE> t_stderr;
#endif

int t_remove(_TNt_array_ptr<const char> name);
int t_rename(_TNt_array_ptr<const char> from,
           _TNt_array_ptr<const char> to);
_TPtr<FILE> t_tmpfile(void);
_TNt_array_ptr<char> t_tmpnam(_TNt_array_ptr<char> source);
int t_fclose(_TPtr<FILE> stream);
int t_fflush(_TPtr<FILE> stream);
_TPtr<FILE> t_fopen(_TNt_array_ptr<const char> restrict filename,
            _TNt_array_ptr<const char> restrict mode);
_TPtr<FILE> t_freopen(_TNt_array_ptr<const char> restrict filename,
              _TNt_array_ptr<const char> restrict mode,
              _Ptr<FILE> restrict stream);

void t_setbuf(_TPtr<FILE> restrict stream,
              _TArray_ptr<char> restrict buf : count(BUFSIZ));
int t_setvbuf(_TPtr<FILE> restrict stream,
              _TArray_ptr<char> restrict buf : count(size),
            int mode, size_t size);

//
// TODO: printing and scanning functions are still mostly
// unchecked because of the use of varargs.
// * There may not be enough arguments for the format string.
// * Any pointer arguments may not meet the requirements of the
//  format string.
//

// We wrap each definition in a complex conditional, there two boolean values:
// - we are fortifying, or we're not (_FORTIFY_SOURCE==0 is not fortifying)
// - there is or there isn't a macro hash-defining this symbol (defined(symbol))
// Cases:
// - Fortifying,     Macro Exists: this is expected, we don't need the definition
// - Not Fortifying, Macro Exists: we need the definition, we need to undef macro
// - Fortifying,     No Macro:     we need the definition
// - Not Fortifying, No Macro:     we need the definition


#if _FORTIFY_SOURCE == 0 || !defined(t_fprintf)
#undef t_fprintf
_Unchecked
int t_fprintf(_TPtr<FILE> restrict stream,
              _TNt_array_ptr<const char> restrict format, ...);
#endif

_Unchecked
int t_fscanf(_TPtr<FILE> restrict stream,
             _TNt_array_ptr<const char> restrict format, ...);

#if _FORTIFY_SOURCE == 0 || !defined(t_printf)
#undef t_printf
_Unchecked
int t_printf(_TNt_array_ptr<const char> restrict format, ...);
#endif

int t_scanf(_TNt_array_ptr<const char> restrict format);

#if _FORTIFY_SOURCE == 0 || !defined(t_sprintf)
#undef t_sprintf
_Unchecked
int t_sprintf(_TArray_ptr<char> restrict s,
              _TNt_array_ptr<const char> restrict format, ...);
#endif
_Unchecked
int t_sscanf(_TNt_array_ptr<const char> restrict s,
             _TNt_array_ptr<const char> restrict format, ...);

#if _FORTIFY_SOURCE == 0 || !defined(t_snprintf)
#undef t_snprintf

// Since snprintf automatically adds the null terminator
// and counts that number in n, s only needs count(n-1) per the 
// definition of _Nt types. Additional declaration for arrays 
// available in checkedc_extensions.h
_Unchecked
int t_snprintf(_TNt_array_ptr<char> restrict s : count(n-1),
             size_t n _Where n > 0,
             _TNt_array_ptr<const char> restrict format, ...);
#endif

#if _FORTIFY_SOURCE == 0 || !defined(t_vfprintf)
#undef t_vfprintf
_Unchecked
int t_vfprintf(_TPtr<FILE> restrict stream,
               _TNt_array_ptr<const char> restrict format,
             va_list arg);
#endif
_Unchecked
int t_vfscanf(_TPtr<FILE> restrict stream,
              _TNt_array_ptr<const char> restrict format,
            va_list arg);

#if _FORTIFY_SOURCE == 0 || !defined(t_vprintf)
#undef t_vprintf
_Unchecked
int t_vprintf(_TNt_array_ptr<const char> restrict format,
             va_list arg);
#endif
_Unchecked
int t_vscanf(_TNt_array_ptr<const char> restrict format,
            va_list arg);

#if _FORTIFY_SOURCE == 0 || !defined(t_vsnprintf)
#undef t_vsnprintf
_Unchecked
int t_vsnprintf(_TNt_array_ptr<char> restrict s : count(n), size_t n,
                _TNt_array_ptr<char> restrict format,
                va_list arg);
#endif

#if _FORTIFY_SOURCE == 0 || !defined(t_vsprintf)
#undef t_vsprintf
_Unchecked
int t_vsprintf(_TNt_array_ptr<char> restrict s,
               _TNt_array_ptr<char> restrict format,
               va_list arg);
#endif
_Unchecked
int t_vsscanf(_TNt_array_ptr<const char> restrict s,
              _TNt_array_ptr<const char> restrict format,
              va_list arg);

int t_fgetc(_TPtr<FILE> stream);
int t_fputc(int c, _TPtr<FILE> stream);
_TArray_ptr<char> t_fgets(_TArray_ptr<char> restrict s : count(n), int n,
                   _TPtr<FILE> restrict stream) :
  bounds(s, s + n);
int t_fputs(_TNt_array_ptr<const char> restrict s,
            _TPtr<FILE> restrict stream);
int t_getc(_TPtr<FILE> stream);
int t_putc(int c, _TPtr<FILE> stream);
int t_puts(_TNt_array_ptr<const char>str);
int t_ungetc(int c, _TPtr<FILE> stream);

_Itype_for_any(T) size_t t_fread(_TArray_ptr<T> restrict pointer : byte_count(size * nmemb),
             size_t size, size_t nmemb,
             _TPtr<FILE> restrict stream);

_Itype_for_any(T) size_t t_fwrite(_TArray_ptr<T> restrict pointer : byte_count(size * nmemb),
              size_t size, size_t nmemb,
              _TPtr<FILE> restrict stream);

int t_fgetpos(_TPtr<FILE> restrict stream,
              _TPtr<fpos_t> restrict pos);

int t_fseek(_TPtr<FILE> stream, long int offset, int whence);
int t_fsetpos(_TPtr<FILE> stream,
              _TPtr<const fpos_t> pos);

long int t_ftell(_TPtr<FILE> stream);
void t_rewind(_TPtr<FILE> stream);

void t_clearerr(_TPtr<FILE> stream);
int t_feof(_TPtr<FILE> stream);
int t_ferror(_TPtr<FILE> stream);
void t_perror(_TNt_array_ptr<const char>s);

#include "_builtin_stdio_checked.h"

#pragma CHECKED_SCOPE pop

#endif // guard
#endif // Tainted C

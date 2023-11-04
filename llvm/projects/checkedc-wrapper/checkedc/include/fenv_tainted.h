//--------------------------------------------------------------------//
// Bounds-safe interfaces for functions in fenv.h that                //
// take pointer arguments.                                            //
//                                                                    //
// These are listed in the same order that they occur in the C11      //
// specification.                                                     //
////////////////////////////////////////////////////////////////////////

#include_next <fenv.h>

#ifdef __checkcbox
#pragma CHECKED_SCOPE pop
#endif

#ifdef __checkedc
#ifndef __FENV_TAINTED_H
#define __FENV_TAINTED_H

#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE on

int t_fesetexceptflag(_TPtr<const fexcept_t> flagp,
                    int excepts);
int t_fegetenv(_TPtr<fenv_t> envp);
int t_feholdexcept(_TPtr<fenv_t> envp);
int t_fesetenv(_TPtr<const fenv_t> envp);
int t_feupdateenv(_TPtr<const fenv_t> envp);

#pragma CHECKED_SCOPE pop

#endif // guard
#endif // Tainted C

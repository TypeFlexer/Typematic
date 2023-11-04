//---------------------------------------------------------------------//
// Bounds-safe interfaces for a functions in signal.h that             //
// take pointer arguments.                                             //
/////////////////////////////////////////////////////////////////////////

#include_next <signal.h>

#ifdef __checkcbox
#ifndef __SIGNAL_TAINTED_H
#define __SIGNAL_TAINTED_H

_TPtr<void (int)> t_signal // bounds-safe interface for signal return
     (int);

#endif // guard
#endif // Tainted C

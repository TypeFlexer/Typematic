//---------------------------------------------------------------------//
// Bounds-safe interfaces for functions in math.h that                 //
// take pointer arguments.                                             //
//                                                                     //
// These are listed in the same order that they occur in the C11       //
// specification.                                                      //
/////////////////////////////////////////////////////////////////////////


#pragma TLIB_SCOPE push
#pragma TLIB_SCOPE on

#include_next <math.h>

#ifdef __checkcbox
#ifndef __MATH_TAINTED_H
#define __MATH_TAINTED_H

double t_frexp(double value, _TPtr<int> exp);
float t_frexpf(float value, _TPtr<int> exp);
long double t_frexpl(long double value, _TPtr<int> exp);
double t_modf(double value, _TPtr<double> iptr);
float t_modff(float value, _TPtr<float> iptr);
long double t_modfl(long double value,
                  _TPtr<long double> iptr);

double t_remquo(double x, double y, _TPtr<int> quo);
float t_remquof(float x, float y, _TPtr<int> quo);
long double t_remquol(long double x, long double y, _TPtr<int> quo);

double t_nan(_TNt_array_ptr<const char> t);
float t_nanf(_TNt_array_ptr<const char> t);
long double t_nanl(_TNt_array_ptr<const char>t);

int t_isnan(double __x);
int t_isinf(double __x);

#pragma TLIB_SCOPE pop
#endif // guard
#endif // Tainted C

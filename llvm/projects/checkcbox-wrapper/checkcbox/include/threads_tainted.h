//---------------------------------------------------------------------//
// Bounds-safe interfaces for functions in threads.h that              //
// take pointer arguments.                                             //
//                                                                     //
// These are listed in the same order that they occur in the C11       //
// specification.                                                      //
/////////////////////////////////////////////////////////////////////////



#ifdef _TAINTED_MOCKUP_THREADS
// C implementations may not support the C11 threads package or even the
// macro that says C11 threads are not supported.  This mocks up
// the types needed by threads so that we can test that the declarations
// below compile.
typedef struct __once_flag_struct once_flag;
typedef struct __cnd_struct cnd_t;
typedef struct __mtx_struct mtx_t;
typedef struct __thread_struct thrd_t;
typedef int (*thrd_start_t)(void *);
typedef struct __thread_specific_storage_struct tss_t;
typedef void (tss_dtor_t)(void *);
struct timespec;
#else

#if defined __has_include_next
#if __has_include_next(<threads.h>)

#include_next <threads.h>

#endif // has threads.h
#endif // defined __has_include_next
#endif // _TAINTED_MOCKUP_THREADS


#if defined _TAINTED_MOCKUP_THREADS || \
    (defined __has_include_next && __has_include_next(<threads.h>))

#ifdef __checkcbox
#ifndef __THREADS_CHECKED_H
#define __THREADS_CHECKED_H

#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE on

_TLIB void t_call_once(_TPtr<once_flag> flag,
               _TPtr<void (void)>fn);

_TLIB int t_cnd_broadcast(_TPtr<cnd_t> condition);
_TLIB void t_cnd_destroy(_TPtr<cnd_t> condition);
_TLIB void t_cnd_init(_TPtr<cnd_t> condition);
_TLIB int t_cnd_signal(_TPtr<cnd_t> condition);
_TLIB int t_cnd_timedwait(_TPtr<cnd_t> restrict cond,
                  _TPtr<mtx_t> restrict mutex,
                  _TPtr<const struct timespec> restrict spec);
_TLIB int t_cnd_wait(_TPtr<cnd_t> condition,
             _TPtr<mtx_t> mutex);
_TLIB void t_mtx_destroy(_TPtr<mtx_t> mutex);
_TLIB int t_mtx_init(_TPtr<mtx_t> mutex, int type);
_TLIB int t_mtx_lock(_TPtr<mtx_t> mutex);
_TLIB int t_mtx_timedlock(_TPtr<mtx_t> restrict mutex,
                  _TPtr<const struct timespec> restrict ts);
_TLIB int t_mtx_trylock(_TPtr<mtx_t> mtex);
_TLIB int t_mtx_unlock(_TPtr<mtx_t> mtex);

_TLIB int t_thrd_create(_TPtr<thrd_t> thr,
                _TPtr<int (_TPtr<void>)> func,
                _TPtr<void> arg);

_TLIB int t_thrd_join(thrd_t thr, _TPtr<int> res);
_TLIB int t_thrd_sleep(_TPtr<const struct timespec> duration,
               _TPtr<struct timespec> remaining);
_TLIB int t_tss_create(_TPtr<tss_t> key,
               _TPtr<void (_TPtr<void>)> dtor);

_TLIB _TPtr<void> t_tss_get(tss_t key);
_TLIB int t_tss_set(tss_t key, _TPtr<void> value);

#pragma CHECKED_SCOPE pop

#endif // guard
#endif // Tainted C

#endif

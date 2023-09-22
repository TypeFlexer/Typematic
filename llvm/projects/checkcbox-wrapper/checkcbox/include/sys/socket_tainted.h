//---------------------------------------------------------------------//
// Bounds-safe interfaces for functions in POSIX socket.h.             //
//                                                                     //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

// The Windows environment may not have sys/socket.h
#if defined __has_include_next
#if __has_include_next(<sys/socket.h>)

#ifdef __checkcbox
#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE off
#endif

#include_next <sys/socket.h>

#ifdef __checkcbox
#pragma CHECKED_SCOPE pop
#endif

#ifdef __checkcbox
#ifndef __SOCKET_TAINTED_H
#define __SOCKET_TAINTED_H

#pragma CHECKED_SCOPE push
#pragma CHECKED_SCOPE on

#ifdef __APPLE__
// Seems not to be a thing for Mac
#define __THROW

#endif

extern int t_socketpair (int __domain, int __type, int __protocol,
    _TArray_ptr<int>  __fds : count(2)) __THROW;

extern int t_bind (
    int __fd, 
    _TPtr<const struct sockaddr> __addr,
    socklen_t __len)
     __THROW;

extern int t_getsockname (
    int __fd, 
    _TPtr<struct sockaddr> __restrict __addr,
    _TPtr<socklen_t> __restrict __len) __THROW;

extern int t_connect (
    int __fd, 
    _TPtr<const struct sockaddr> __addr,
    socklen_t __len);

extern int t_getpeername (
    int __fd, 
    _TPtr<struct sockaddr> __restrict __addr,
    _TPtr<socklen_t> __restrict __len) __THROW;

extern ssize_t t_send(
    int __fd, 
    _TArray_ptr<const void> __buf : byte_count(__n),
    size_t __n, int __flags);

extern ssize_t t_recv (
    int __fd, 
    _TArray_ptr<void> __buf : byte_count(__n),
    size_t __n, int __flags);

extern ssize_t t_sendto (
    int __fd, 
    _TArray_ptr<const void> __buf : byte_count(__n),
    size_t __n, 
    int __flags, 
    _TPtr<const struct sockaddr> __addr,
    socklen_t __addr_len);

extern ssize_t t_recvfrom (
    int __fd, 
    _TArray_ptr<void> __restrict __buf : byte_count(__n),
    size_t __n, int __flags, 
    _TPtr<struct sockaddr> __restrict __addr,
    _TPtr<socklen_t> __restrict__restrict __addr_len);

extern ssize_t t_sendmsg (
    int __fd, 
    _TPtr<const struct msghdr> __message,
    int __flags);

#ifdef __USE_GNU
extern int t_sendmmsg (
    int __fd, 
    _TArray_ptr<struct mmsghdr> __vmessages : count(__vlen),
    unsigned int __vlen, 
    int __flags);
#endif

extern ssize_t t_recvmsg (
    int __fd, 
    _TPtr<struct msghdr> __message,
    int __flags);

#ifdef __USE_GNU
extern int t_recvmmsg (
    int __fd, 
    _TArray_ptr<struct mmsghdr> __vmessages : count(__vlen),
    unsigned int __vlen, 
    int __flags, 
    _TPtr<struct timespec> __tmo);
#endif

extern int t_getsockopt (
    int __fd, int __level, int __optname, 
    _TArray_ptr<void> __restrict __optval : byte_count(*__optlen),
    _TPtr<socklen_t> __restrict __optlen) __THROW;

extern int t_setsockopt (
    int __fd, int __level, int __optname, 
    _TArray_ptr<const void> __optval : byte_count(__optlen),
    socklen_t __optlen) __THROW;

extern int t_accept (
    int __fd, 
    _TPtr<struct sockaddr> __restrict __addr,
    _TPtr<socklen_t> __restrict __addr_len);

#ifdef __USE_GNU
extern int t_accept4 (
    int __fd, 
    _TPtr<struct sockaddr> __addr,
    _TPtr<socklen_t> __restrict __addr_len : itype(),
    int __flags);
#endif

#pragma CHECKED_SCOPE pop

#endif // guard
#endif // Tainted C

#endif // has socket.h
#endif // defined __has_include_next

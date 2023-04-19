/*===---- stdtainted.h - Standard header for __tainted__ macro ---------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __STDTAINTED_H
#define __STDTAINTED_H

#define tainted _Tainted
#define __tainted__is_defined 1
#define tainted_callback _Callback
#define __tainted_callback__is_defined 1
#define tainted_mirror _Mirror
#define __tainted_mirror__is_defined 1

#endif /* __STDTAINTED_H */
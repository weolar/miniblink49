/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2013 Apple Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

/* This prefix file is for use on Mac OS X only. This prefix file should contain only:
 *    1) files to precompile for faster builds
 *    2) in one case at least: OS-X-specific performance bug workarounds
 *    3) the special trick to catch us using new or delete without including "config.h"
 * The project should be able to build without this header, although we rarely test that.
 */

/* Things that need to be defined globally should go into "config.h". */

#ifdef __cplusplus
#define NULL __null
#else
#define NULL ((void *)0)
#endif

#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>
#include <regex.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef __cplusplus
#include <ciso646>

#if defined(_LIBCPP_VERSION)

// Add work around for a bug in libc++ that caused standard heap
// APIs to not compile <rdar://problem/10858112>.

#include <type_traits>

namespace blink {
class TimerHeapReference;
}

_LIBCPP_BEGIN_NAMESPACE_STD

inline _LIBCPP_INLINE_VISIBILITY
const blink::TimerHeapReference& move(const blink::TimerHeapReference& t)
{
    return t;
}

_LIBCPP_END_NAMESPACE_STD

#endif // defined(_LIBCPP_VERSION)

#include <algorithm>
#include <cstddef>
#include <new>

#endif // __cplusplus

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#endif

#ifdef __cplusplus
#define new ("if you use new/delete make sure to include config.h at the top of the file"())
#define delete ("if you use new/delete make sure to include config.h at the top of the file"())
#endif

/* When C++ exceptions are disabled, the C++ library defines |try| and |catch|
 * to allow C++ code that expects exceptions to build. These definitions
 * interfere with Objective-C++ uses of Objective-C exception handlers, which
 * use |@try| and |@catch|. As a workaround, undefine these macros. */
#ifdef __OBJC__
#undef try
#undef catch
#endif

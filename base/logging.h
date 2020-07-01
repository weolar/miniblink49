// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_LOGGING_H_
#define BASE_LOGGING_H_

#ifndef CHECK
#define CHECK(condition)  \
  do {                    \
    if ((!(condition))) { \
      DebugBreak();       \
    }                     \
  } while (0)
#endif

// The DCHECK macro is equivalent to CHECK except that it only
// generates code in debug builds.
#ifdef DEBUG
#  define DCHECK(condition)      CHECK(condition)
#else
#  ifndef DCHECK
#    define DCHECK(condition)      ((void) 0)
#  endif
#endif

#if defined(NDEBUG) && !defined(DCHECK_ALWAYS_ON)
#define DCHECK_IS_ON() 0
#else
#define DCHECK_IS_ON() 1
#endif

#endif // BASE_LOGGING_H_
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STRINGS_STRING_UTIL_WIN_H_
#define BASE_STRINGS_STRING_UTIL_WIN_H_

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>

#include "base/logging.h"

namespace base {

// Chromium code style is to not use malloc'd strings; this is only for use
// for interaction with APIs that require it.
inline char* strdup(const char* str)
{
    return _strdup(str);
}

int vsnprintf(char* buffer, size_t size, const char* format, va_list arguments);
int vswprintf(wchar_t* buffer, size_t size, const wchar_t* format, va_list arguments);

} // namespace base

#endif // BASE_STRINGS_STRING_UTIL_WIN_H_

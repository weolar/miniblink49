// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/file_util.h"

#include <io.h>
#include <psapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <time.h>
#include <windows.h>

#include <algorithm>
#include <limits>
#include <string>

#include "base/files/file_enumerator.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/metrics/histogram.h"
#include "base/process/process_handle.h"
#include "base/rand_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread_restrictions.h"
#include "base/time/time.h"
#include "base/win/scoped_handle.h"
#include "base/win/windows_version.h"

namespace base {

// Gets the current working directory for the process.
bool GetCurDirectory(FilePath* dir)
{
    ThreadRestrictions::AssertIOAllowed();

    wchar_t system_buffer[MAX_PATH];
    system_buffer[0] = 0;
    DWORD len = ::GetCurrentDirectory(MAX_PATH, system_buffer);
    if (len == 0 || len > MAX_PATH)
        return false;
    // TODO(evanm): the old behavior of this function was to always strip the
    // trailing slash.  We duplicate this here, but it shouldn't be necessary
    // when everyone is using the appropriate FilePath APIs.
    std::wstring dir_str(system_buffer);
    *dir = FilePath(dir_str).StripTrailingSeparators();
    return true;
}

bool SetCurDirectory(const FilePath& directory)
{
    ThreadRestrictions::AssertIOAllowed();
    BOOL ret = ::SetCurrentDirectory(directory.value().c_str());
    return ret != 0;
}

}
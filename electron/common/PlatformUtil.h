// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_COMMON_PLATFORM_UTIL_H_
#define ATOM_COMMON_PLATFORM_UTIL_H_

#include "build/build_config.h"

#if defined(OS_WIN)
#include "base/strings/string16.h"
#endif

class GURL;

namespace base {
class FilePath;
}

namespace platform_util {

// Show the given file in a file manager. If possible, select the file.
// Must be called from the UI thread.
void showItemInFolder(const base::FilePath& full_path);

// Open the given file in the desktop's default manner.
// Must be called from the UI thread.
void openItem(const base::FilePath& full_path);

// Open the given external protocol URL in the desktop's default manner.
// (For example, mailto: URLs in the default mail user agent.)
bool openExternal(
#if defined(OS_WIN)
    const base::string16& url,
#else
    const GURL& url,
#endif
    bool activate);

// Move a file to trash.
bool moveItemToTrash(const base::FilePath& full_path);

void beep();

}  // namespace platform_util

#endif  // ATOM_COMMON_PLATFORM_UTIL_H_

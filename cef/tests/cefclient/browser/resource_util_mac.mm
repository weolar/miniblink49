// Copyright (c) 2013 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cefclient/browser/resource_util.h"

#import <Foundation/Foundation.h>
#include <mach-o/dyld.h>
#include <stdio.h>

#include "include/base/cef_logging.h"

namespace client {

namespace {

bool AmIBundled() {
  // Implementation adapted from Chromium's base/mac/foundation_util.mm
  ProcessSerialNumber psn = {0, kCurrentProcess};

  FSRef fsref;
  OSStatus pbErr;
  if ((pbErr = GetProcessBundleLocation(&psn, &fsref)) != noErr) {
    NOTREACHED();
    return false;
  }

  FSCatalogInfo info;
  OSErr fsErr;
  if ((fsErr = FSGetCatalogInfo(&fsref, kFSCatInfoNodeFlags, &info,
                                NULL, NULL, NULL)) != noErr) {
    NOTREACHED();
    return false;
  }

  return (info.nodeFlags & kFSNodeIsDirectoryMask);
}

}  // namespace

bool GetResourceDir(std::string& dir) {
  // Implementation adapted from Chromium's base/base_path_mac.mm
  if (AmIBundled()) {
    // Retrieve the executable directory.
    uint32_t pathSize = 0;
    _NSGetExecutablePath(NULL, &pathSize);
    if (pathSize > 0) {
      dir.resize(pathSize);
      _NSGetExecutablePath(const_cast<char*>(dir.c_str()), &pathSize);
    }

    // Trim executable name up to the last separator
    std::string::size_type last_separator = dir.find_last_of("/");
    dir.resize(last_separator);
    dir.append("/../Resources");
    return true;
  } else {
    // TODO: Provide unbundled path
    NOTIMPLEMENTED();
    return false;
  }
}

}  // namespace client

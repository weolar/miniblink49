// Copyright (c) 2013 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cefclient/browser/resource_util.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

namespace client {

bool GetResourceDir(std::string& dir) {
  char buff[1024];

  // Retrieve the executable path.
  ssize_t len = readlink("/proc/self/exe", buff, sizeof(buff)-1);
  if (len == -1)
    return false;

  buff[len] = 0;

  // Remove the executable name from the path.
  char* pos = strrchr(buff, '/');
  if (!pos)
    return false;

  // Add "files" to the path.
  strcpy(pos+1, "files");  // NOLINT(runtime/printf)
  dir = std::string(buff);
  return true;
}

}  // namespace client

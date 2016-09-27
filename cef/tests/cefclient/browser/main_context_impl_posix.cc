// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefclient/browser/main_context_impl.h"

#include <unistd.h>

namespace client {

std::string MainContextImpl::GetDownloadPath(const std::string& file_name) {
  return std::string();
}

std::string MainContextImpl::GetAppWorkingDirectory() {
  char szWorkingDir[256];
  if (getcwd(szWorkingDir, sizeof(szWorkingDir) - 1) == NULL) {
    szWorkingDir[0] = 0;
  } else {
    // Add trailing path separator.
    size_t len = strlen(szWorkingDir);
    szWorkingDir[len] = '/';
    szWorkingDir[len + 1] = 0;
  }
  return szWorkingDir;
}

}  // namespace client

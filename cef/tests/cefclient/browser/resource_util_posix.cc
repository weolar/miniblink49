// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefclient/browser/resource_util.h"

#include <stdio.h>

namespace client {

namespace {

bool FileExists(const char* path) {
  FILE* f = fopen(path, "rb");
  if (f) {
    fclose(f);
    return true;
  }
  return false;
}

bool ReadFileToString(const char* path, std::string& data) {
  // Implementation adapted from base/file_util.cc
  FILE* file = fopen(path, "rb");
  if (!file)
    return false;

  char buf[1 << 16];
  size_t len;
  while ((len = fread(buf, 1, sizeof(buf), file)) > 0)
    data.append(buf, len);
  fclose(file);

  return true;
}

}  // namespace

bool LoadBinaryResource(const char* resource_name, std::string& resource_data) {
  std::string path;
  if (!GetResourceDir(path))
    return false;

  path.append("/");
  path.append(resource_name);

  return ReadFileToString(path.c_str(), resource_data);
}

CefRefPtr<CefStreamReader> GetBinaryResourceReader(const char* resource_name) {
  std::string path;
  if  (!GetResourceDir(path))
    return NULL;

  path.append("/");
  path.append(resource_name);

  if (!FileExists(path.c_str()))
    return NULL;

  return CefStreamReader::CreateForFile(path);
}

}  // namespace client

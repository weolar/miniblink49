// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFCLIENT_BROWSER_RESOURCE_UTIL_H_
#define CEF_TESTS_CEFCLIENT_BROWSER_RESOURCE_UTIL_H_
#pragma once

#include <string>
#include "include/cef_stream.h"

#if defined(OS_WIN)
#include "include/wrapper/cef_resource_manager.h"
#endif

namespace client {

#if defined(OS_POSIX)
// Returns the directory containing resource files.
bool GetResourceDir(std::string& dir);
#endif

// Retrieve a resource as a string.
bool LoadBinaryResource(const char* resource_name, std::string& resource_data);

// Retrieve a resource as a steam reader.
CefRefPtr<CefStreamReader> GetBinaryResourceReader(const char* resource_name);

#if defined(OS_WIN)
// Create a new provider for loading binary resources.
CefResourceManager::Provider* CreateBinaryResourceProvider(
    const std::string& url_path);
#endif

}  // namespace client

#endif  // CEF_TESTS_CEFCLIENT_BROWSER_RESOURCE_UTIL_H_

// Copyright (c) 2009 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_DLL_CTOCPP_BASE_CTOCPP_H_
#define CEF_LIBCEF_DLL_CTOCPP_BASE_CTOCPP_H_
#pragma once

#include "include/cef_base.h"
#include "include/capi/cef_base_capi.h"
#include "libcef_dll/ctocpp/ctocpp.h"

#ifndef BUILDING_CEF_SHARED
#pragma message("Warning: "__FILE__" may be accessed DLL-side only")
#else  // BUILDING_CEF_SHARED

// Wrap a C structure with a C++ class.
class CefBaseCToCpp
    : public CefCToCpp<CefBaseCToCpp, CefBase, cef_base_t> {
 public:
  CefBaseCToCpp();
};

#endif  // BUILDING_CEF_SHARED
#endif  // CEF_LIBCEF_DLL_CTOCPP_BASE_CTOCPP_H_

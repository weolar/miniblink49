// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libcef_dll/ctocpp/base_ctocpp.h"

CefBaseCToCpp::CefBaseCToCpp() {
}

template<> cef_base_t* CefCToCpp<CefBaseCToCpp, CefBase, cef_base_t>::
    UnwrapDerived(CefWrapperType type, CefBase* c) {
  NOTREACHED();
  return NULL;
}

#ifndef NDEBUG
template<> base::AtomicRefCount CefCToCpp<CefBaseCToCpp, CefBase,
    cef_base_t>::DebugObjCt = 0;
#endif

template<> CefWrapperType CefCToCpp<CefBaseCToCpp, CefBase,cef_base_t>::
    kWrapperType = WT_BASE;

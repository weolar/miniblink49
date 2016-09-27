// Copyright 2015 The Chromium Embedded Framework Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_RENDERER_EXTENSIONS_EXTENSIONS_DISPATCHER_DELEGATE_H_
#define CEF_LIBCEF_RENDERER_EXTENSIONS_EXTENSIONS_DISPATCHER_DELEGATE_H_

#include "base/macros.h"
#include "extensions/renderer/dispatcher_delegate.h"

namespace extensions {

class CefExtensionsDispatcherDelegate : public DispatcherDelegate {
 public:
  CefExtensionsDispatcherDelegate();
  ~CefExtensionsDispatcherDelegate() override;

 private:
  DISALLOW_COPY_AND_ASSIGN(CefExtensionsDispatcherDelegate);
};

}  // namespace extensions

#endif  // CEF_LIBCEF_RENDERER_EXTENSIONS_EXTENSIONS_DISPATCHER_DELEGATE_H_

// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_RENDERER_EXTENSIONS_EXTENSIONS_RENDERER_CLIENT_H_
#define CEF_LIBCEF_RENDERER_EXTENSIONS_EXTENSIONS_RENDERER_CLIENT_H_

#include "base/macros.h"
#include "extensions/renderer/extensions_renderer_client.h"

namespace extensions {

class CefExtensionsRendererClient : public ExtensionsRendererClient {
 public:
  CefExtensionsRendererClient();
  ~CefExtensionsRendererClient() override;

  // ExtensionsRendererClient implementation.
  bool IsIncognitoProcess() const override;
  int GetLowestIsolatedWorldId() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(CefExtensionsRendererClient);
};

}  // namespace extensions

#endif  // CEF_LIBCEF_RENDERER_EXTENSIONS_EXTENSIONS_RENDERER_CLIENT_H_

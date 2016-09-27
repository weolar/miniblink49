// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/renderer/extensions/extensions_renderer_client.h"

#include "libcef/renderer/render_process_observer.h"

namespace extensions {

CefExtensionsRendererClient::CefExtensionsRendererClient() {
}

CefExtensionsRendererClient::~CefExtensionsRendererClient() {
}

bool CefExtensionsRendererClient::IsIncognitoProcess() const {
  return CefRenderProcessObserver::is_incognito_process();
}

int CefExtensionsRendererClient::GetLowestIsolatedWorldId() const {
  // CEF doesn't need to reserve world IDs for anything other than extensions,
  // so we always return 1. Note that 0 is reserved for the global world.
  return 1;
}

}  // namespace extensions

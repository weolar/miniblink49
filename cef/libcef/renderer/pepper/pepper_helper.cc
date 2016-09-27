// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/renderer/pepper/pepper_helper.h"

#include "libcef/renderer/pepper/renderer_pepper_host_factory.h"

#include "content/public/renderer/renderer_ppapi_host.h"
#include "ppapi/host/ppapi_host.h"

CefPepperHelper::CefPepperHelper(content::RenderFrame* render_frame)
    : RenderFrameObserver(render_frame) {}

CefPepperHelper::~CefPepperHelper() {}

void CefPepperHelper::DidCreatePepperPlugin(content::RendererPpapiHost* host) {
  // TODO(brettw) figure out how to hook up the host factory. It needs some
  // kind of filter-like system to allow dynamic additions.
  host->GetPpapiHost()->AddHostFactoryFilter(
      scoped_ptr<ppapi::host::HostFactory>(
          new CefRendererPepperHostFactory(host)));
}

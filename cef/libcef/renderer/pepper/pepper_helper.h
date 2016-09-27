// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_RENDERER_PEPPER_PEPPER_HELPER_H_
#define CEF_LIBCEF_RENDERER_PEPPER_PEPPER_HELPER_H_

#include "base/compiler_specific.h"
#include "content/public/renderer/render_frame_observer.h"

// This class listens for Pepper creation events from the RenderFrame and
// attaches the parts required for Chrome-specific plugin support.
class CefPepperHelper : public content::RenderFrameObserver {
 public:
  explicit CefPepperHelper(content::RenderFrame* render_frame);
  ~CefPepperHelper() override;

  // RenderFrameObserver.
  void DidCreatePepperPlugin(content::RendererPpapiHost* host) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(CefPepperHelper);
};

#endif  // CEF_LIBCEF_RENDERER_PEPPER_PEPPER_HELPER_H_

// Copyright 2014 The Chromium Embedded Framework Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef LIBCEF_RENDERER_RENDER_FRAME_OBSERVER_H_
#define LIBCEF_RENDERER_RENDER_FRAME_OBSERVER_H_

#include "content/public/renderer/render_frame_observer.h"

namespace content {
class RenderFrame;
}

class CefRenderFrameObserver : public content::RenderFrameObserver {
 public:
  explicit CefRenderFrameObserver(content::RenderFrame* render_frame);
  ~CefRenderFrameObserver() override;

  void DidCreateScriptContext(v8::Handle<v8::Context> context,
                              int extension_group,
                              int world_id) override;
  void WillReleaseScriptContext(v8::Handle<v8::Context> context,
                                int world_id) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(CefRenderFrameObserver);
};

#endif  // LIBCEF_RENDERER_RENDER_FRAME_OBSERVER_H_

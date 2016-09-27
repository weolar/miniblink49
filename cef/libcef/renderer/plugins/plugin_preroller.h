// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_RENDERER_PLUGINS_PLUGIN_PREROLLER_H_
#define CEF_LIBCEF_RENDERER_PLUGINS_PLUGIN_PREROLLER_H_

#include "base/macros.h"
#include "content/public/common/webplugininfo.h"
#include "content/public/renderer/plugin_instance_throttler.h"
#include "content/public/renderer/render_frame_observer.h"
#include "third_party/WebKit/public/web/WebPluginParams.h"
#include "url/gurl.h"

namespace blink {
class WebLocalFrame;
class WebPlugin;
}

class SkBitmap;

// This class manages a plugin briefly for the purposes of keyframe extraction.
// Once a keyframe has been extracted, this class will replace the plugin with
// a ChromePluginPlaceholder. The actual plugin will continue to live in a
// throttled state. This class manages its own lifetime.
class CefPluginPreroller : public content::PluginInstanceThrottler::Observer,
                           public content::RenderFrameObserver {
 public:
  // Does not take ownership of either |plugin| or |throttler|.
  CefPluginPreroller(content::RenderFrame* render_frame,
                     blink::WebLocalFrame* frame,
                     const blink::WebPluginParams& params,
                     const content::WebPluginInfo& info,
                     const std::string& identifier,
                     const base::string16& name,
                     const base::string16& message,
                     content::PluginInstanceThrottler* throttler);

  ~CefPluginPreroller() override;

 private:
  // content::PluginInstanceThrottler::Observer methods:
  void OnKeyframeExtracted(const SkBitmap* bitmap) override;
  void OnThrottleStateChange() override;
  void OnThrottlerDestroyed() override;

  blink::WebLocalFrame* frame_;
  blink::WebPluginParams params_;
  content::WebPluginInfo info_;
  std::string identifier_;
  base::string16 name_;
  base::string16 message_;

  content::PluginInstanceThrottler* throttler_;

  GURL keyframe_data_url_;

  DISALLOW_COPY_AND_ASSIGN(CefPluginPreroller);
};

#endif  // CEF_LIBCEF_RENDERER_PLUGINS_PLUGIN_PREROLLER_H_

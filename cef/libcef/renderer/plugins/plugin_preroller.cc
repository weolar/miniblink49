// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/renderer/plugins/plugin_preroller.h"

#include "libcef/renderer/plugins/cef_plugin_placeholder.h"

#include "base/base64.h"
#include "chrome/grit/generated_resources.h"
#include "third_party/WebKit/public/platform/WebRect.h"
#include "third_party/WebKit/public/web/WebElement.h"
#include "third_party/WebKit/public/web/WebPlugin.h"
#include "third_party/WebKit/public/web/WebPluginContainer.h"
#include "ui/gfx/codec/png_codec.h"

CefPluginPreroller::CefPluginPreroller(
    content::RenderFrame* render_frame,
    blink::WebLocalFrame* frame,
    const blink::WebPluginParams& params,
    const content::WebPluginInfo& info,
    const std::string& identifier,
    const base::string16& name,
    const base::string16& message,
    content::PluginInstanceThrottler* throttler)
    : RenderFrameObserver(render_frame),
      frame_(frame),
      params_(params),
      info_(info),
      identifier_(identifier),
      name_(name),
      message_(message),
      throttler_(throttler) {
  DCHECK(throttler);
  throttler_->AddObserver(this);
}

CefPluginPreroller::~CefPluginPreroller() {
  if (throttler_)
    throttler_->RemoveObserver(this);
}

void CefPluginPreroller::OnKeyframeExtracted(const SkBitmap* bitmap) {
  std::vector<unsigned char> png_data;
  if (!gfx::PNGCodec::EncodeBGRASkBitmap(*bitmap, false, &png_data)) {
    DLOG(ERROR) << "Provided keyframe could not be encoded as PNG.";
    return;
  }

  base::StringPiece png_as_string(reinterpret_cast<char*>(&png_data[0]),
                                  png_data.size());

  std::string data_url_header = "data:image/png;base64,";
  std::string data_url_body;
  base::Base64Encode(png_as_string, &data_url_body);
  keyframe_data_url_ = GURL(data_url_header + data_url_body);
}

void CefPluginPreroller::OnThrottleStateChange() {
  if (!throttler_->IsThrottled())
    return;

  PlaceholderPosterInfo poster_info;
  poster_info.poster_attribute = keyframe_data_url_.spec();
  poster_info.custom_poster_size = throttler_->GetSize();

  CefPluginPlaceholder* placeholder =
      CefPluginPlaceholder::CreateBlockedPlugin(
          render_frame(), frame_, params_, info_, identifier_, name_,
          IDR_PLUGIN_POSTER_HTML, message_, poster_info);
  placeholder->SetPremadePlugin(throttler_);
  placeholder->set_power_saver_enabled(true);
  placeholder->AllowLoading();

  blink::WebPluginContainer* container =
      throttler_->GetWebPlugin()->container();
  container->setPlugin(placeholder->plugin());

  bool success = placeholder->plugin()->initialize(container);
  DCHECK(success);

  container->invalidate();
  container->reportGeometry();

  delete this;
}

void CefPluginPreroller::OnThrottlerDestroyed() {
  throttler_ = nullptr;
  delete this;
}

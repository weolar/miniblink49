// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_RENDERER_EXTENSIONS_PRINT_WEB_VIEW_HELPER_DELEGATE_H_
#define CEF_LIBCEF_RENDERER_EXTENSIONS_PRINT_WEB_VIEW_HELPER_DELEGATE_H_

#include "components/printing/renderer/print_web_view_helper.h"

namespace extensions {

class CefPrintWebViewHelperDelegate
    : public printing::PrintWebViewHelper::Delegate {
 public:
  ~CefPrintWebViewHelperDelegate() override;

  bool CancelPrerender(content::RenderView* render_view,
                       int routing_id) override;
  blink::WebElement GetPdfElement(blink::WebLocalFrame* frame) override;
  bool IsPrintPreviewEnabled() override;
  bool OverridePrint(blink::WebLocalFrame* frame) override;
};

}  // namespace extensions

#endif  // CEF_LIBCEF_RENDERER_EXTENSIONS_PRINT_WEB_VIEW_HELPER_DELEGATE_H_

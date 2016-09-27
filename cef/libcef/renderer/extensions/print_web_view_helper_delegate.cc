// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/renderer/extensions/print_web_view_helper_delegate.h"

#include <vector>

#include "chrome/common/chrome_switches.h"
#include "chrome/common/extensions/extension_constants.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/renderer/render_view.h"
#include "extensions/common/constants.h"
#include "extensions/renderer/guest_view/mime_handler_view/mime_handler_view_container.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebElement.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"

namespace extensions {

CefPrintWebViewHelperDelegate::~CefPrintWebViewHelperDelegate(){
}

bool CefPrintWebViewHelperDelegate::CancelPrerender(
    content::RenderView* render_view, int routing_id) {
  return false;
}

// Return the PDF object element if |frame| is the out of process PDF extension.
blink::WebElement CefPrintWebViewHelperDelegate::GetPdfElement(
        blink::WebLocalFrame* frame) {
  GURL url = frame->document().url();
  if (url.SchemeIs(extensions::kExtensionScheme) &&
      url.host() == extension_misc::kPdfExtensionId) {
    // <object> with id="plugin" is created in
    // chrome/browser/resources/pdf/pdf.js.
    auto plugin_element = frame->document().getElementById("plugin");
    if (!plugin_element.isNull()) {
      return plugin_element;
    }
    NOTREACHED();
  }
  return blink::WebElement();
}

bool CefPrintWebViewHelperDelegate::IsPrintPreviewEnabled() {
  return false;
}

bool CefPrintWebViewHelperDelegate::OverridePrint(
    blink::WebLocalFrame* frame) {
  if (!frame->document().isPluginDocument())
    return false;

  std::vector<extensions::MimeHandlerViewContainer*> mime_handlers =
      extensions::MimeHandlerViewContainer::FromRenderFrame(
          content::RenderFrame::FromWebFrame(frame));
  if (!mime_handlers.empty()) {
    // This message is handled in chrome/browser/resources/pdf/pdf.js and
    // instructs the PDF plugin to print. This is to make window.print() on a
    // PDF plugin document correctly print the PDF. See
    // https://crbug.com/448720.
    base::DictionaryValue message;
    message.SetString("type", "print");
    mime_handlers.front()->PostMessageFromValue(message);
    return true;
  }
  return false;
}

}  // namespace extensions

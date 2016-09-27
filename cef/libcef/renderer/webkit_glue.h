// Copyright (c) 2012 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_RENDERER_WEBKIT_GLUE_H_
#define CEF_LIBCEF_RENDERER_WEBKIT_GLUE_H_

#include <string>
#include "base/basictypes.h"
#include "third_party/skia/include/core/SkColor.h"

#include "include/internal/cef_types.h"

namespace blink {
class WebFrame;
class WebNode;
class WebString;
class WebView;
}

namespace webkit_glue {

extern const int64 kInvalidFrameId;

bool CanGoBack(blink::WebView* view);
bool CanGoForward(blink::WebView* view);
void GoBack(blink::WebView* view);
void GoForward(blink::WebView* view);

// Returns the text of the document element.
std::string DumpDocumentText(blink::WebFrame* frame);

// Expose additional actions on WebNode.
cef_dom_node_type_t GetNodeType(const blink::WebNode& node);
blink::WebString GetNodeName(const blink::WebNode& node);
blink::WebString CreateNodeMarkup(const blink::WebNode& node);
bool SetNodeValue(blink::WebNode& node, const blink::WebString& value);

int64 GetIdentifier(blink::WebFrame* frame);

// Find the frame with the specified |unique_name| relative to
// |relative_to_frame| in the frame hierarchy.
blink::WebFrame* FindFrameByUniqueName(const blink::WebString& unique_name,
                                       blink::WebFrame* relative_to_frame);

bool ParseCSSColor(const blink::WebString& string, bool strict, SkColor& color);

}  // webkit_glue

#endif  // CEF_LIBCEF_RENDERER_WEBKIT_GLUE_H_

// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef CEF_LIBCEF_RENDERER_FRAME_IMPL_H_
#define CEF_LIBCEF_RENDERER_FRAME_IMPL_H_
#pragma once

#include <string>
#include "include/cef_frame.h"
#include "include/cef_v8.h"

class CefBrowserImpl;

namespace blink {
class WebFrame;
}

// Implementation of CefFrame. CefFrameImpl objects are owned by the
// CefBrowerImpl and will be detached when the browser is notified that the
// associated renderer WebFrame will close.
class CefFrameImpl : public CefFrame {
 public:
  CefFrameImpl(CefBrowserImpl* browser,
               blink::WebFrame* frame);
  ~CefFrameImpl() override;

  // CefFrame implementation.
  bool IsValid() override;
  void Undo() override;
  void Redo() override;
  void Cut() override;
  void Copy() override;
  void Paste() override;
  void Delete() override;
  void SelectAll() override;
  void ViewSource() override;
  void GetSource(CefRefPtr<CefStringVisitor> visitor) override;
  void GetText(CefRefPtr<CefStringVisitor> visitor) override;
  void LoadRequest(CefRefPtr<CefRequest> request) override;
  void LoadURL(const CefString& url) override;
  void LoadString(const CefString& string,
                  const CefString& url) override;
  void ExecuteJavaScript(const CefString& jsCode,
                         const CefString& scriptUrl,
                         int startLine) override;
  bool IsMain() override;
  bool IsFocused() override;
  CefString GetName() override;
  int64 GetIdentifier() override;
  CefRefPtr<CefFrame> GetParent() override;
  CefString GetURL() override;
  CefRefPtr<CefBrowser> GetBrowser() override;
  CefRefPtr<CefV8Context> GetV8Context() override;
  void VisitDOM(CefRefPtr<CefDOMVisitor> visitor) override;

  void Detach();

  blink::WebFrame* web_frame() const { return frame_; }

 protected:
  CefBrowserImpl* browser_;
  blink::WebFrame* frame_;
  int64 frame_id_;

  IMPLEMENT_REFCOUNTING(CefFrameImpl);
  DISALLOW_EVIL_CONSTRUCTORS(CefFrameImpl);
};

#endif  // CEF_LIBCEF_RENDERER_FRAME_IMPL_H_

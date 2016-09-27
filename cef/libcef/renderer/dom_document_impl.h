// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_DOM_DOCUMENT_IMPL_H_
#define CEF_LIBCEF_DOM_DOCUMENT_IMPL_H_
#pragma once

#include <map>
#include "include/cef_dom.h"

namespace blink {
class WebFrame;
class WebNode;
};

class CefBrowserImpl;

class CefDOMDocumentImpl : public CefDOMDocument {
 public:
  CefDOMDocumentImpl(CefBrowserImpl* browser,
                     blink::WebFrame* frame);
  ~CefDOMDocumentImpl() override;

  // CefDOMDocument methods.
  Type GetType() override;
  CefRefPtr<CefDOMNode> GetDocument() override;
  CefRefPtr<CefDOMNode> GetBody() override;
  CefRefPtr<CefDOMNode> GetHead() override;
  CefString GetTitle() override;
  CefRefPtr<CefDOMNode> GetElementById(const CefString& id) override;
  CefRefPtr<CefDOMNode> GetFocusedNode() override;
  bool HasSelection() override;
  int GetSelectionStartOffset() override;
  int GetSelectionEndOffset() override;
  CefString GetSelectionAsMarkup() override;
  CefString GetSelectionAsText() override;
  CefString GetBaseURL() override;
  CefString GetCompleteURL(const CefString& partialURL) override;

  CefBrowserImpl* GetBrowser() { return browser_; }
  blink::WebFrame* GetFrame() { return frame_; }

  // The document maintains a map of all existing node objects.
  CefRefPtr<CefDOMNode> GetOrCreateNode(const blink::WebNode& node);
  void RemoveNode(const blink::WebNode& node);

  // Must be called before the object is destroyed.
  void Detach();

  // Verify that the object exists and is being accessed on the UI thread.
  bool VerifyContext();

 protected:
  CefBrowserImpl* browser_;
  blink::WebFrame* frame_;

  typedef std::map<blink::WebNode, CefDOMNode*> NodeMap;
  NodeMap node_map_;

  IMPLEMENT_REFCOUNTING(CefDOMDocumentImpl);
};

#endif  // CEF_LIBCEF_DOM_DOCUMENT_IMPL_H_

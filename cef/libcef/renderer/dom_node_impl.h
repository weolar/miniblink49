// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_DOM_NODE_IMPL_H_
#define CEF_LIBCEF_DOM_NODE_IMPL_H_
#pragma once

#include "include/cef_dom.h"
#include "third_party/WebKit/public/web/WebNode.h"

class CefDOMDocumentImpl;

class CefDOMNodeImpl : public CefDOMNode {
 public:
  CefDOMNodeImpl(CefRefPtr<CefDOMDocumentImpl> document,
                 const blink::WebNode& node);
  ~CefDOMNodeImpl() override;

  // CefDOMNode methods.
  Type GetType() override;
  bool IsText() override;
  bool IsElement() override;
  bool IsEditable() override;
  bool IsFormControlElement() override;
  CefString GetFormControlElementType() override;
  bool IsSame(CefRefPtr<CefDOMNode> that) override;
  CefString GetName() override;
  CefString GetValue() override;
  bool SetValue(const CefString& value) override;
  CefString GetAsMarkup() override;
  CefRefPtr<CefDOMDocument> GetDocument() override;
  CefRefPtr<CefDOMNode> GetParent() override;
  CefRefPtr<CefDOMNode> GetPreviousSibling() override;
  CefRefPtr<CefDOMNode> GetNextSibling() override;
  bool HasChildren() override;
  CefRefPtr<CefDOMNode> GetFirstChild() override;
  CefRefPtr<CefDOMNode> GetLastChild() override;
  CefString GetElementTagName() override;
  bool HasElementAttributes() override;
  bool HasElementAttribute(const CefString& attrName) override;
  CefString GetElementAttribute(const CefString& attrName) override;
  void GetElementAttributes(AttributeMap& attrMap) override;
  bool SetElementAttribute(const CefString& attrName,
                           const CefString& value) override;
  CefString GetElementInnerText() override;

  // Will be called from CefDOMDocumentImpl::Detach().
  void Detach();

  // Verify that the object exists and is being accessed on the UI thread.
  bool VerifyContext();

 protected:
  CefRefPtr<CefDOMDocumentImpl> document_;
  blink::WebNode node_;

  IMPLEMENT_REFCOUNTING(CefDOMNodeImpl);
};

#endif  // CEF_LIBCEF_DOM_NODE_IMPL_H_

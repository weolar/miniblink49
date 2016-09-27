// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libcef/renderer/dom_node_impl.h"
#include "libcef/common/tracker.h"
#include "libcef/renderer/browser_impl.h"
#include "libcef/renderer/dom_document_impl.h"
#include "libcef/renderer/thread_util.h"
#include "libcef/renderer/webkit_glue.h"

#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebDOMEvent.h"
#include "third_party/WebKit/public/web/WebElement.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebFormControlElement.h"
#include "third_party/WebKit/public/web/WebInputElement.h"
#include "third_party/WebKit/public/web/WebNode.h"
#include "third_party/WebKit/public/web/WebSelectElement.h"

using blink::WebDocument;
using blink::WebDOMEvent;
using blink::WebElement;
using blink::WebFrame;
using blink::WebFormControlElement;
using blink::WebInputElement;
using blink::WebNode;
using blink::WebSelectElement;
using blink::WebString;

CefDOMNodeImpl::CefDOMNodeImpl(CefRefPtr<CefDOMDocumentImpl> document,
                               const blink::WebNode& node)
    : document_(document),
      node_(node) {
}

CefDOMNodeImpl::~CefDOMNodeImpl() {
  CEF_REQUIRE_RT();

  if (document_.get() && !node_.isNull()) {
    // Remove the node from the document.
    document_->RemoveNode(node_);
  }
}

CefDOMNodeImpl::Type CefDOMNodeImpl::GetType() {
  if (!VerifyContext())
    return DOM_NODE_TYPE_UNSUPPORTED;

  return webkit_glue::GetNodeType(node_);
}

bool CefDOMNodeImpl::IsText() {
  if (!VerifyContext())
    return false;

  return node_.isTextNode();
}

bool CefDOMNodeImpl::IsElement() {
  if (!VerifyContext())
    return false;

  return node_.isElementNode();
}

// Logic copied from RenderViewImpl::IsEditableNode.
bool CefDOMNodeImpl::IsEditable() {
  if (!VerifyContext())
    return false;

  if (node_.isContentEditable())
    return true;

  if (node_.isElementNode()) {
    const WebElement& element = node_.toConst<WebElement>();
    if (element.isTextFormControlElement())
      return true;

    // Also return true if it has an ARIA role of 'textbox'.
    for (unsigned i = 0; i < element.attributeCount(); ++i) {
      if (base::LowerCaseEqualsASCII(element.attributeLocalName(i).utf8(),
                                     "role")) {
        if (base::LowerCaseEqualsASCII(element.attributeValue(i).utf8(),
                                       "textbox")) {
          return true;
        }
        break;
      }
    }
  }

  return false;
}

bool CefDOMNodeImpl::IsFormControlElement() {
  if (!VerifyContext())
    return false;

  if (node_.isElementNode()) {
    const WebElement& element = node_.toConst<WebElement>();
    return element.isFormControlElement();
  }

  return false;
}

CefString CefDOMNodeImpl::GetFormControlElementType() {
  CefString str;
  if (!VerifyContext())
    return str;

  if (node_.isElementNode()) {
    const WebElement& element = node_.toConst<WebElement>();
    if (element.isFormControlElement()) {
      // Retrieve the type from the form control element.
      const WebFormControlElement& formElement =
          node_.toConst<WebFormControlElement>();

      const base::string16& form_control_type = formElement.formControlType();
      str = form_control_type;
    }
  }

  return str;
}

bool CefDOMNodeImpl::IsSame(CefRefPtr<CefDOMNode> that) {
  if (!VerifyContext())
    return false;

  CefDOMNodeImpl* impl = static_cast<CefDOMNodeImpl*>(that.get());
  if (!impl || !impl->VerifyContext())
    return false;

  return node_.equals(impl->node_);
}

CefString CefDOMNodeImpl::GetName() {
  CefString str;
  if (!VerifyContext())
    return str;

  const WebString& name = webkit_glue::GetNodeName(node_);
  if (!name.isNull())
    str = name;

  return str;
}

CefString CefDOMNodeImpl::GetValue() {
  CefString str;
  if (!VerifyContext())
    return str;

  if (node_.isElementNode()) {
    const WebElement& element = node_.toConst<WebElement>();
    if (element.isFormControlElement()) {
      // Retrieve the value from the form control element.
      const WebFormControlElement& formElement =
          node_.toConst<WebFormControlElement>();

      base::string16 value;
      const base::string16& form_control_type = formElement.formControlType();
      if (form_control_type == base::ASCIIToUTF16("text")) {
        const WebInputElement& input_element =
            formElement.toConst<WebInputElement>();
        value = input_element.value();
      } else if (form_control_type == base::ASCIIToUTF16("select-one")) {
        const WebSelectElement& select_element =
            formElement.toConst<WebSelectElement>();
        value = select_element.value();
      }

      base::TrimWhitespace(value, base::TRIM_LEADING, &value);
      str = value;
    }
  }

  if (str.empty()) {
    const WebString& value = node_.nodeValue();
    if (!value.isNull())
      str = value;
  }

  return str;
}

bool CefDOMNodeImpl::SetValue(const CefString& value) {
  if (!VerifyContext())
    return false;

  if (node_.isElementNode())
    return false;

  return webkit_glue::SetNodeValue(node_, base::string16(value));
}

CefString CefDOMNodeImpl::GetAsMarkup() {
  CefString str;
  if (!VerifyContext())
    return str;

  const WebString& markup = webkit_glue::CreateNodeMarkup(node_);
  if (!markup.isNull())
    str = markup;

  return str;
}

CefRefPtr<CefDOMDocument> CefDOMNodeImpl::GetDocument() {
  if (!VerifyContext())
    return NULL;

  return document_.get();
}

CefRefPtr<CefDOMNode> CefDOMNodeImpl::GetParent() {
  if (!VerifyContext())
    return NULL;

  return document_->GetOrCreateNode(node_.parentNode());
}

CefRefPtr<CefDOMNode> CefDOMNodeImpl::GetPreviousSibling() {
  if (!VerifyContext())
    return NULL;

  return document_->GetOrCreateNode(node_.previousSibling());
}

CefRefPtr<CefDOMNode> CefDOMNodeImpl::GetNextSibling() {
  if (!VerifyContext())
    return NULL;

  return document_->GetOrCreateNode(node_.nextSibling());
}

bool CefDOMNodeImpl::HasChildren() {
  if (!VerifyContext())
    return false;

  return node_.hasChildNodes();
}

CefRefPtr<CefDOMNode> CefDOMNodeImpl::GetFirstChild() {
  if (!VerifyContext())
    return NULL;

  return document_->GetOrCreateNode(node_.firstChild());
}

CefRefPtr<CefDOMNode> CefDOMNodeImpl::GetLastChild() {
  if (!VerifyContext())
    return NULL;

  return document_->GetOrCreateNode(node_.lastChild());
}

CefString CefDOMNodeImpl::GetElementTagName() {
  CefString str;
  if (!VerifyContext())
    return str;

  if (!node_.isElementNode()) {
    NOTREACHED();
    return str;
  }

  const WebElement& element = node_.toConst<blink::WebElement>();
  const WebString& tagname = element.tagName();
  if (!tagname.isNull())
    str = tagname;

  return str;
}

bool CefDOMNodeImpl::HasElementAttributes() {
  if (!VerifyContext())
    return false;

  if (!node_.isElementNode()) {
    NOTREACHED();
    return false;
  }

  const WebElement& element = node_.toConst<blink::WebElement>();
  return (element.attributeCount() > 0);
}

bool CefDOMNodeImpl::HasElementAttribute(const CefString& attrName) {
  if (!VerifyContext())
    return false;

  if (!node_.isElementNode()) {
    NOTREACHED();
    return false;
  }

  const WebElement& element = node_.toConst<blink::WebElement>();
  return element.hasAttribute(base::string16(attrName));
}

CefString CefDOMNodeImpl::GetElementAttribute(const CefString& attrName) {
  CefString str;
  if (!VerifyContext())
    return str;

  if (!node_.isElementNode()) {
    NOTREACHED();
    return str;
  }

  const WebElement& element = node_.toConst<blink::WebElement>();
  const WebString& attr = element.getAttribute(base::string16(attrName));
  if (!attr.isNull())
    str = attr;

  return str;
}

void CefDOMNodeImpl::GetElementAttributes(AttributeMap& attrMap) {
  if (!VerifyContext())
    return;

  if (!node_.isElementNode()) {
    NOTREACHED();
    return;
  }

  const WebElement& element = node_.toConst<blink::WebElement>();
  unsigned int len = element.attributeCount();
  if (len == 0)
    return;

  for (unsigned int i = 0; i < len; ++i) {
    base::string16 name = element.attributeLocalName(i);
    base::string16 value = element.attributeValue(i);
    attrMap.insert(std::make_pair(name, value));
  }
}

bool CefDOMNodeImpl::SetElementAttribute(const CefString& attrName,
                                         const CefString& value) {
  if (!VerifyContext())
    return false;

  if (!node_.isElementNode()) {
    NOTREACHED();
    return false;
  }

  WebElement element = node_.to<blink::WebElement>();
  return element.setAttribute(base::string16(attrName),
                              base::string16(value));
}

CefString CefDOMNodeImpl::GetElementInnerText() {
  CefString str;
  if (!VerifyContext())
    return str;

  if (!node_.isElementNode()) {
    NOTREACHED();
    return str;
  }

  WebElement element = node_.to<blink::WebElement>();
  const WebString& text = element.textContent();
  if (!text.isNull())
    str = text;

  return str;
}

void CefDOMNodeImpl::Detach() {
  document_ = NULL;
  node_.assign(WebNode());
}

bool CefDOMNodeImpl::VerifyContext() {
  if (!document_.get()) {
    NOTREACHED();
    return false;
  }
  if (!document_->VerifyContext())
    return false;
  if (node_.isNull()) {
    NOTREACHED();
    return false;
  }
  return true;
}

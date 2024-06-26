// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/xml/cfx_xmlelement.h"

#include <utility>

#include "core/fxcrt/cfx_widetextbuf.h"
#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/xml/cfx_xmlchardata.h"
#include "core/fxcrt/xml/cfx_xmldocument.h"
#include "core/fxcrt/xml/cfx_xmltext.h"

CFX_XMLElement::CFX_XMLElement(const WideString& wsTag) : name_(wsTag) {
  ASSERT(!name_.IsEmpty());
}

CFX_XMLElement::~CFX_XMLElement() = default;

FX_XMLNODETYPE CFX_XMLElement::GetType() const {
  return FX_XMLNODE_Element;
}

CFX_XMLNode* CFX_XMLElement::Clone(CFX_XMLDocument* doc) {
  auto* node = doc->CreateNode<CFX_XMLElement>(name_);
  node->attrs_ = attrs_;

  // TODO(dsinclair): This clone is wrong. It doesn't clone all child nodes just
  // text nodes?
  for (CFX_XMLNode* pChild = GetFirstChild(); pChild;
       pChild = pChild->GetNextSibling()) {
    if (pChild->GetType() == FX_XMLNODE_Text)
      node->AppendChild(pChild->Clone(doc));
  }
  return node;
}

WideString CFX_XMLElement::GetLocalTagName() const {
  auto pos = name_.Find(L':');
  return pos.has_value() ? name_.Right(name_.GetLength() - pos.value() - 1)
                         : name_;
}

WideString CFX_XMLElement::GetNamespacePrefix() const {
  auto pos = name_.Find(L':');
  return pos.has_value() ? name_.Left(pos.value()) : WideString();
}

WideString CFX_XMLElement::GetNamespaceURI() const {
  WideString attr(L"xmlns");
  WideString wsPrefix = GetNamespacePrefix();
  if (!wsPrefix.IsEmpty()) {
    attr += L":";
    attr += wsPrefix;
  }
  const CFX_XMLNode* pNode = this;
  while (pNode) {
    if (pNode->GetType() != FX_XMLNODE_Element)
      break;

    auto* pElement = static_cast<const CFX_XMLElement*>(pNode);
    if (!pElement->HasAttribute(attr)) {
      pNode = pNode->GetParent();
      continue;
    }
    return pElement->GetAttribute(attr);
  }
  return WideString();
}

WideString CFX_XMLElement::GetTextData() const {
  CFX_WideTextBuf buffer;
  for (CFX_XMLNode* pChild = GetFirstChild(); pChild;
       pChild = pChild->GetNextSibling()) {
    CFX_XMLText* pText = ToXMLText(pChild);
    if (pText)
      buffer << pText->GetText();
  }
  return buffer.MakeString();
}

void CFX_XMLElement::Save(
    const RetainPtr<IFX_SeekableWriteStream>& pXMLStream) {
  ByteString bsNameEncoded = name_.ToUTF8();

  pXMLStream->WriteString("<");
  pXMLStream->WriteString(bsNameEncoded.AsStringView());

  for (auto it : attrs_) {
    // Note, the space between attributes is added by AttributeToString which
    // writes a blank as the first character.
    pXMLStream->WriteString(
        AttributeToString(it.first, it.second).ToUTF8().AsStringView());
  }

  if (!GetFirstChild()) {
    pXMLStream->WriteString(" />\n");
    return;
  }

  pXMLStream->WriteString(">\n");

  for (CFX_XMLNode* pChild = GetFirstChild(); pChild;
       pChild = pChild->GetNextSibling()) {
    pChild->Save(pXMLStream);
  }
  pXMLStream->WriteString("</");
  pXMLStream->WriteString(bsNameEncoded.AsStringView());
  pXMLStream->WriteString(">\n");
}

CFX_XMLElement* CFX_XMLElement::GetFirstChildNamed(WideStringView name) const {
  return GetNthChildNamed(name, 0);
}

CFX_XMLElement* CFX_XMLElement::GetNthChildNamed(WideStringView name,
                                                 size_t idx) const {
  for (auto* child = GetFirstChild(); child; child = child->GetNextSibling()) {
    CFX_XMLElement* elem = ToXMLElement(child);
    if (!elem || elem->name_ != name)
      continue;
    if (idx == 0)
      return elem;

    --idx;
  }
  return nullptr;
}

bool CFX_XMLElement::HasAttribute(const WideString& name) const {
  return attrs_.find(name) != attrs_.end();
}

WideString CFX_XMLElement::GetAttribute(const WideString& name) const {
  auto it = attrs_.find(name);
  return it != attrs_.end() ? it->second : WideString();
}

void CFX_XMLElement::SetAttribute(const WideString& name,
                                  const WideString& value) {
  attrs_[name] = value;
}

void CFX_XMLElement::RemoveAttribute(const WideString& name) {
  attrs_.erase(name);
}

WideString CFX_XMLElement::AttributeToString(const WideString& name,
                                             const WideString& value) {
  WideString ret = L" ";
  ret += name;
  ret += L"=\"";
  ret += EncodeEntities(value);
  ret += L"\"";
  return ret;
}

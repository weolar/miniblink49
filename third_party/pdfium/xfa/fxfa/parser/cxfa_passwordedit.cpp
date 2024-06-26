// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_passwordedit.h"

#include "fxjs/xfa/cjx_passwordedit.h"

namespace {

const CXFA_Node::PropertyData kPasswordEditPropertyData[] = {
    {XFA_Element::Margin, 1, 0},
    {XFA_Element::Border, 1, 0},
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kPasswordEditAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::PasswordChar, XFA_AttributeType::CData, (void*)L"*"},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::HScrollPolicy, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Auto},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_PasswordEdit::CXFA_PasswordEdit(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::PasswordEdit,
                kPasswordEditPropertyData,
                kPasswordEditAttributeData) {}

CXFA_PasswordEdit::~CXFA_PasswordEdit() = default;

XFA_FFWidgetType CXFA_PasswordEdit::GetDefaultFFWidgetType() const {
  return XFA_FFWidgetType::kPasswordEdit;
}

WideString CXFA_PasswordEdit::GetPasswordChar() {
  return JSObject()->GetCData(XFA_Attribute::PasswordChar);
}

// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_button.h"

#include "fxjs/xfa/cjx_button.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kButtonPropertyData[] = {
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kButtonAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Highlight, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Inverted},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Button::CXFA_Button(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Button,
                kButtonPropertyData,
                kButtonAttributeData,
                pdfium::MakeUnique<CJX_Button>(this)) {}

CXFA_Button::~CXFA_Button() = default;

XFA_FFWidgetType CXFA_Button::GetDefaultFFWidgetType() const {
  return XFA_FFWidgetType::kButton;
}

XFA_AttributeValue CXFA_Button::GetHighlight() {
  return JSObject()->GetEnum(XFA_Attribute::Highlight);
}

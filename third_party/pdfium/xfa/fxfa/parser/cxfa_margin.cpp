// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_margin.h"

#include "fxjs/xfa/cjx_margin.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kMarginPropertyData[] = {
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kMarginAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::LeftInset, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::BottomInset, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::TopInset, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::RightInset, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Margin::CXFA_Margin(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Margin,
                kMarginPropertyData,
                kMarginAttributeData,
                pdfium::MakeUnique<CJX_Margin>(this)) {}

CXFA_Margin::~CXFA_Margin() = default;

float CXFA_Margin::GetLeftInset() const {
  return TryLeftInset().value_or(0);
}

float CXFA_Margin::GetTopInset() const {
  return TryTopInset().value_or(0);
}

float CXFA_Margin::GetRightInset() const {
  return TryRightInset().value_or(0);
}

float CXFA_Margin::GetBottomInset() const {
  return TryBottomInset().value_or(0);
}

Optional<float> CXFA_Margin::TryLeftInset() const {
  return JSObject()->TryMeasureAsFloat(XFA_Attribute::LeftInset);
}

Optional<float> CXFA_Margin::TryTopInset() const {
  return JSObject()->TryMeasureAsFloat(XFA_Attribute::TopInset);
}

Optional<float> CXFA_Margin::TryRightInset() const {
  return JSObject()->TryMeasureAsFloat(XFA_Attribute::RightInset);
}

Optional<float> CXFA_Margin::TryBottomInset() const {
  return JSObject()->TryMeasureAsFloat(XFA_Attribute::BottomInset);
}

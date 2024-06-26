// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_color.h"

#include "fxjs/xfa/cjx_color.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::PropertyData kColorPropertyData[] = {
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kColorAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::CSpace, XFA_AttributeType::CData, (void*)L"SRGB"},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Value, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Color::CXFA_Color(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Color,
                kColorPropertyData,
                kColorAttributeData,
                pdfium::MakeUnique<CJX_Color>(this)) {}

CXFA_Color::~CXFA_Color() = default;

FX_ARGB CXFA_Color::GetValue() {
  Optional<WideString> val = JSObject()->TryCData(XFA_Attribute::Value, false);
  return val ? StringToFXARGB(val->AsStringView()) : 0xFF000000;
}

FX_ARGB CXFA_Color::GetValueOrDefault(FX_ARGB defaultValue) {
  Optional<WideString> val = JSObject()->TryCData(XFA_Attribute::Value, false);
  return val ? StringToFXARGB(val->AsStringView()) : defaultValue;
}

void CXFA_Color::SetValue(FX_ARGB color) {
  int a;
  int r;
  int g;
  int b;
  std::tie(a, r, g, b) = ArgbDecode(color);
  JSObject()->SetCData(XFA_Attribute::Value,
                       WideString::Format(L"%d,%d,%d", r, g, b), false, false);
}

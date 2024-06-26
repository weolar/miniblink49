// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_font.h"

#include "fxjs/xfa/cjx_font.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_fill.h"
#include "xfa/fxfa/parser/cxfa_measurement.h"

namespace {

const CXFA_Node::PropertyData kFontPropertyData[] = {
    {XFA_Element::Fill, 1, 0},
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kFontAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::LineThrough, XFA_AttributeType::Integer, (void*)0},
    {XFA_Attribute::Typeface, XFA_AttributeType::CData, (void*)L"Courier"},
    {XFA_Attribute::FontHorizontalScale, XFA_AttributeType::CData,
     (void*)L"100%"},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::KerningMode, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::None},
    {XFA_Attribute::Underline, XFA_AttributeType::Integer, (void*)0},
    {XFA_Attribute::BaselineShift, XFA_AttributeType::Measure, (void*)L"0in"},
    {XFA_Attribute::OverlinePeriod, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::All},
    {XFA_Attribute::LetterSpacing, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::LineThroughPeriod, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::All},
    {XFA_Attribute::FontVerticalScale, XFA_AttributeType::CData,
     (void*)L"100%"},
    {XFA_Attribute::PsName, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Size, XFA_AttributeType::Measure, (void*)L"10pt"},
    {XFA_Attribute::Posture, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Normal},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Weight, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Normal},
    {XFA_Attribute::UnderlinePeriod, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::All},
    {XFA_Attribute::Overline, XFA_AttributeType::Integer, (void*)0},
    {XFA_Attribute::GenericFamily, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Serif},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Font::CXFA_Font(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(
          doc,
          packet,
          (XFA_XDPPACKET_Template | XFA_XDPPACKET_Config | XFA_XDPPACKET_Form),
          XFA_ObjectType::Node,
          XFA_Element::Font,
          kFontPropertyData,
          kFontAttributeData,
          pdfium::MakeUnique<CJX_Font>(this)) {}

CXFA_Font::~CXFA_Font() = default;

float CXFA_Font::GetBaselineShift() const {
  return JSObject()
      ->GetMeasure(XFA_Attribute::BaselineShift)
      .ToUnit(XFA_Unit::Pt);
}

float CXFA_Font::GetHorizontalScale() {
  WideString wsValue = JSObject()->GetCData(XFA_Attribute::FontHorizontalScale);
  int32_t iScale = FXSYS_wtoi(wsValue.c_str());
  return iScale > 0 ? (float)iScale : 100.0f;
}

float CXFA_Font::GetVerticalScale() {
  WideString wsValue = JSObject()->GetCData(XFA_Attribute::FontVerticalScale);
  int32_t iScale = FXSYS_wtoi(wsValue.c_str());
  return iScale > 0 ? (float)iScale : 100.0f;
}

float CXFA_Font::GetLetterSpacing() {
  WideString wsValue = JSObject()->GetCData(XFA_Attribute::LetterSpacing);
  CXFA_Measurement ms(wsValue.AsStringView());
  if (ms.GetUnit() == XFA_Unit::Em)
    return ms.GetValue() * GetFontSize();
  return ms.ToUnit(XFA_Unit::Pt);
}

int32_t CXFA_Font::GetLineThrough() {
  return JSObject()->GetInteger(XFA_Attribute::LineThrough);
}

int32_t CXFA_Font::GetUnderline() {
  return JSObject()->GetInteger(XFA_Attribute::Underline);
}

XFA_AttributeValue CXFA_Font::GetUnderlinePeriod() {
  return JSObject()
      ->TryEnum(XFA_Attribute::UnderlinePeriod, true)
      .value_or(XFA_AttributeValue::All);
}

float CXFA_Font::GetFontSize() const {
  return JSObject()->GetMeasure(XFA_Attribute::Size).ToUnit(XFA_Unit::Pt);
}

WideString CXFA_Font::GetTypeface() {
  return JSObject()->GetCData(XFA_Attribute::Typeface);
}

bool CXFA_Font::IsBold() {
  return JSObject()->GetEnum(XFA_Attribute::Weight) == XFA_AttributeValue::Bold;
}

bool CXFA_Font::IsItalic() {
  return JSObject()->GetEnum(XFA_Attribute::Posture) ==
         XFA_AttributeValue::Italic;
}

void CXFA_Font::SetColor(FX_ARGB color) {
  CXFA_Fill* node =
      JSObject()->GetOrCreateProperty<CXFA_Fill>(0, XFA_Element::Fill);
  if (!node)
    return;

  node->SetColor(color);
}

FX_ARGB CXFA_Font::GetColor() {
  CXFA_Fill* fill = GetChild<CXFA_Fill>(0, XFA_Element::Fill, false);
  return fill ? fill->GetColor(true) : 0xFF000000;
}

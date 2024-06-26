// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_stipple.h"

#include "fxjs/xfa/cjx_stipple.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_color.h"

namespace {

const CXFA_Node::PropertyData kStipplePropertyData[] = {
    {XFA_Element::Color, 1, 0},
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kStippleAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Rate, XFA_AttributeType::Integer, (void*)50},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Stipple::CXFA_Stipple(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Stipple,
                kStipplePropertyData,
                kStippleAttributeData,
                pdfium::MakeUnique<CJX_Stipple>(this)) {}

CXFA_Stipple::~CXFA_Stipple() = default;

CXFA_Color* CXFA_Stipple::GetColorIfExists() {
  return GetChild<CXFA_Color>(0, XFA_Element::Color, false);
}

int32_t CXFA_Stipple::GetRate() {
  return JSObject()
      ->TryInteger(XFA_Attribute::Rate, true)
      .value_or(GetDefaultRate());
}

void CXFA_Stipple::Draw(CXFA_Graphics* pGS,
                        CXFA_GEPath* fillPath,
                        const CFX_RectF& rtFill,
                        const CFX_Matrix& matrix) {
  int32_t iRate = GetRate();
  if (iRate == 0)
    iRate = 100;

  CXFA_Color* pColor = GetColorIfExists();
  FX_ARGB crColor = pColor ? pColor->GetValue() : CXFA_Color::kBlackColor;

  int32_t alpha;
  FX_COLORREF colorref;
  std::tie(alpha, colorref) = ArgbToAlphaAndColorRef(crColor);
  FX_ARGB cr = AlphaAndColorRefToArgb(iRate * alpha / 100, colorref);

  pGS->SaveGraphState();
  pGS->SetFillColor(CXFA_GEColor(cr));
  pGS->FillPath(fillPath, FXFILL_WINDING, &matrix);
  pGS->RestoreGraphState();
}

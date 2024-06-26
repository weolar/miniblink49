// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_radial.h"

#include <utility>

#include "fxjs/xfa/cjx_radial.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_color.h"
#include "xfa/fxgraphics/cxfa_geshading.h"

namespace {

const CXFA_Node::PropertyData kRadialPropertyData[] = {
    {XFA_Element::Color, 1, 0},
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kRadialAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Type, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::ToEdge},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Radial::CXFA_Radial(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Radial,
                kRadialPropertyData,
                kRadialAttributeData,
                pdfium::MakeUnique<CJX_Radial>(this)) {}

CXFA_Radial::~CXFA_Radial() = default;

bool CXFA_Radial::IsToEdge() {
  auto value = JSObject()->TryEnum(XFA_Attribute::Type, true);
  return !value.has_value() || value.value() == XFA_AttributeValue::ToEdge;
}

CXFA_Color* CXFA_Radial::GetColorIfExists() {
  return GetChild<CXFA_Color>(0, XFA_Element::Color, false);
}

void CXFA_Radial::Draw(CXFA_Graphics* pGS,
                       CXFA_GEPath* fillPath,
                       FX_ARGB crStart,
                       const CFX_RectF& rtFill,
                       const CFX_Matrix& matrix) {
  CXFA_Color* pColor = GetColorIfExists();
  FX_ARGB crEnd = pColor ? pColor->GetValue() : CXFA_Color::kBlackColor;
  if (!IsToEdge())
    std::swap(crStart, crEnd);

  float endRadius = sqrt(rtFill.Width() * rtFill.Width() +
                         rtFill.Height() * rtFill.Height()) /
                    2;
  CXFA_GEShading shading(rtFill.Center(), rtFill.Center(), 0, endRadius, true,
                         true, crStart, crEnd);

  pGS->SaveGraphState();
  pGS->SetFillColor(CXFA_GEColor(&shading));
  pGS->FillPath(fillPath, FXFILL_WINDING, &matrix);
  pGS->RestoreGraphState();
}

// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_pattern.h"

#include "fxjs/xfa/cjx_pattern.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_color.h"
#include "xfa/fxgraphics/cxfa_gepattern.h"

namespace {

const CXFA_Node::PropertyData kPatternPropertyData[] = {
    {XFA_Element::Color, 1, 0},
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kPatternAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Type, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::CrossHatch},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Pattern::CXFA_Pattern(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Pattern,
                kPatternPropertyData,
                kPatternAttributeData,
                pdfium::MakeUnique<CJX_Pattern>(this)) {}

CXFA_Pattern::~CXFA_Pattern() = default;

CXFA_Color* CXFA_Pattern::GetColorIfExists() {
  return GetChild<CXFA_Color>(0, XFA_Element::Color, false);
}

XFA_AttributeValue CXFA_Pattern::GetType() {
  return JSObject()->GetEnum(XFA_Attribute::Type);
}

void CXFA_Pattern::Draw(CXFA_Graphics* pGS,
                        CXFA_GEPath* fillPath,
                        FX_ARGB crStart,
                        const CFX_RectF& rtFill,
                        const CFX_Matrix& matrix) {
  CXFA_Color* pColor = GetColorIfExists();
  FX_ARGB crEnd = pColor ? pColor->GetValue() : CXFA_Color::kBlackColor;

  FX_HatchStyle iHatch = FX_HatchStyle::Cross;
  switch (GetType()) {
    case XFA_AttributeValue::CrossDiagonal:
      iHatch = FX_HatchStyle::DiagonalCross;
      break;
    case XFA_AttributeValue::DiagonalLeft:
      iHatch = FX_HatchStyle::ForwardDiagonal;
      break;
    case XFA_AttributeValue::DiagonalRight:
      iHatch = FX_HatchStyle::BackwardDiagonal;
      break;
    case XFA_AttributeValue::Horizontal:
      iHatch = FX_HatchStyle::Horizontal;
      break;
    case XFA_AttributeValue::Vertical:
      iHatch = FX_HatchStyle::Vertical;
      break;
    default:
      break;
  }

  CXFA_GEPattern pattern(iHatch, crEnd, crStart);

  pGS->SaveGraphState();
  pGS->SetFillColor(CXFA_GEColor(&pattern, 0x0));
  pGS->FillPath(fillPath, FXFILL_WINDING, &matrix);
  pGS->RestoreGraphState();
}

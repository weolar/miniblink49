// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_fill.h"

#include "fxjs/xfa/cjx_fill.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_color.h"
#include "xfa/fxfa/parser/cxfa_linear.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/cxfa_pattern.h"
#include "xfa/fxfa/parser/cxfa_radial.h"
#include "xfa/fxfa/parser/cxfa_stipple.h"

namespace {

const CXFA_Node::PropertyData kFillPropertyData[] = {
    {XFA_Element::Pattern, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Solid, 1,
     XFA_PROPERTYFLAG_OneOf | XFA_PROPERTYFLAG_DefaultOneOf},
    {XFA_Element::Stipple, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Color, 1, 0},
    {XFA_Element::Linear, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Radial, 1, XFA_PROPERTYFLAG_OneOf},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kFillAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Presence, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::Visible},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Fill::CXFA_Fill(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Fill,
                kFillPropertyData,
                kFillAttributeData,
                pdfium::MakeUnique<CJX_Fill>(this)) {}

CXFA_Fill::~CXFA_Fill() = default;

bool CXFA_Fill::IsVisible() {
  return JSObject()
             ->TryEnum(XFA_Attribute::Presence, true)
             .value_or(XFA_AttributeValue::Visible) ==
         XFA_AttributeValue::Visible;
}

void CXFA_Fill::SetColor(FX_ARGB color) {
  CXFA_Color* pColor =
      JSObject()->GetOrCreateProperty<CXFA_Color>(0, XFA_Element::Color);
  if (!pColor)
    return;

  pColor->SetValue(color);
}

FX_ARGB CXFA_Fill::GetColor(bool bText) {
  CXFA_Color* pColor = GetChild<CXFA_Color>(0, XFA_Element::Color, false);
  if (!pColor)
    return bText ? 0xFF000000 : 0xFFFFFFFF;
  return pColor->GetValueOrDefault(bText ? 0xFF000000 : 0xFFFFFFFF);
}

XFA_Element CXFA_Fill::GetType() const {
  CXFA_Node* pChild = GetFirstChild();
  while (pChild) {
    XFA_Element eType = pChild->GetElementType();
    if (eType != XFA_Element::Color && eType != XFA_Element::Extras)
      return eType;

    pChild = pChild->GetNextSibling();
  }
  return XFA_Element::Solid;
}

void CXFA_Fill::Draw(CXFA_Graphics* pGS,
                     CXFA_GEPath* fillPath,
                     const CFX_RectF& rtWidget,
                     const CFX_Matrix& matrix) {
  pGS->SaveGraphState();

  switch (GetType()) {
    case XFA_Element::Radial:
      DrawRadial(pGS, fillPath, rtWidget, matrix);
      break;
    case XFA_Element::Pattern:
      DrawPattern(pGS, fillPath, rtWidget, matrix);
      break;
    case XFA_Element::Linear:
      DrawLinear(pGS, fillPath, rtWidget, matrix);
      break;
    case XFA_Element::Stipple:
      DrawStipple(pGS, fillPath, rtWidget, matrix);
      break;
    default:
      pGS->SetFillColor(CXFA_GEColor(GetColor(false)));
      pGS->FillPath(fillPath, FXFILL_WINDING, &matrix);
      break;
  }

  pGS->RestoreGraphState();
}

void CXFA_Fill::DrawStipple(CXFA_Graphics* pGS,
                            CXFA_GEPath* fillPath,
                            const CFX_RectF& rtWidget,
                            const CFX_Matrix& matrix) {
  CXFA_Stipple* stipple =
      JSObject()->GetOrCreateProperty<CXFA_Stipple>(0, XFA_Element::Stipple);
  if (stipple)
    stipple->Draw(pGS, fillPath, rtWidget, matrix);
}

void CXFA_Fill::DrawRadial(CXFA_Graphics* pGS,
                           CXFA_GEPath* fillPath,
                           const CFX_RectF& rtWidget,
                           const CFX_Matrix& matrix) {
  CXFA_Radial* radial =
      JSObject()->GetOrCreateProperty<CXFA_Radial>(0, XFA_Element::Radial);
  if (radial)
    radial->Draw(pGS, fillPath, GetColor(false), rtWidget, matrix);
}

void CXFA_Fill::DrawLinear(CXFA_Graphics* pGS,
                           CXFA_GEPath* fillPath,
                           const CFX_RectF& rtWidget,
                           const CFX_Matrix& matrix) {
  CXFA_Linear* linear =
      JSObject()->GetOrCreateProperty<CXFA_Linear>(0, XFA_Element::Linear);
  if (linear)
    linear->Draw(pGS, fillPath, GetColor(false), rtWidget, matrix);
}

void CXFA_Fill::DrawPattern(CXFA_Graphics* pGS,
                            CXFA_GEPath* fillPath,
                            const CFX_RectF& rtWidget,
                            const CFX_Matrix& matrix) {
  CXFA_Pattern* pattern =
      JSObject()->GetOrCreateProperty<CXFA_Pattern>(0, XFA_Element::Pattern);
  if (pattern)
    pattern->Draw(pGS, fillPath, GetColor(false), rtWidget, matrix);
}

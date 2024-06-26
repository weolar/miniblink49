// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_linear.h"

#include "fxjs/xfa/cjx_linear.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_color.h"
#include "xfa/fxgraphics/cxfa_geshading.h"

namespace {

const CXFA_Node::PropertyData kLinearPropertyData[] = {
    {XFA_Element::Color, 1, 0},
    {XFA_Element::Extras, 1, 0},
    {XFA_Element::Unknown, 0, 0}};

const CXFA_Node::AttributeData kLinearAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Type, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::ToRight},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Unknown, XFA_AttributeType::Integer, nullptr}};

}  // namespace

CXFA_Linear::CXFA_Linear(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                XFA_ObjectType::Node,
                XFA_Element::Linear,
                kLinearPropertyData,
                kLinearAttributeData,
                pdfium::MakeUnique<CJX_Linear>(this)) {}

CXFA_Linear::~CXFA_Linear() = default;

XFA_AttributeValue CXFA_Linear::GetType() {
  return JSObject()
      ->TryEnum(XFA_Attribute::Type, true)
      .value_or(XFA_AttributeValue::ToRight);
}

CXFA_Color* CXFA_Linear::GetColorIfExists() {
  return GetChild<CXFA_Color>(0, XFA_Element::Color, false);
}

void CXFA_Linear::Draw(CXFA_Graphics* pGS,
                       CXFA_GEPath* fillPath,
                       FX_ARGB crStart,
                       const CFX_RectF& rtFill,
                       const CFX_Matrix& matrix) {
  CXFA_Color* pColor = GetColorIfExists();
  FX_ARGB crEnd = pColor ? pColor->GetValue() : CXFA_Color::kBlackColor;

  CFX_PointF ptStart;
  CFX_PointF ptEnd;
  switch (GetType()) {
    case XFA_AttributeValue::ToRight:
      ptStart = CFX_PointF(rtFill.left, rtFill.top);
      ptEnd = CFX_PointF(rtFill.right(), rtFill.top);
      break;
    case XFA_AttributeValue::ToBottom:
      ptStart = CFX_PointF(rtFill.left, rtFill.top);
      ptEnd = CFX_PointF(rtFill.left, rtFill.bottom());
      break;
    case XFA_AttributeValue::ToLeft:
      ptStart = CFX_PointF(rtFill.right(), rtFill.top);
      ptEnd = CFX_PointF(rtFill.left, rtFill.top);
      break;
    case XFA_AttributeValue::ToTop:
      ptStart = CFX_PointF(rtFill.left, rtFill.bottom());
      ptEnd = CFX_PointF(rtFill.left, rtFill.top);
      break;
    default:
      break;
  }

  CXFA_GEShading shading(ptStart, ptEnd, false, false, crStart, crEnd);

  pGS->SaveGraphState();
  pGS->SetFillColor(CXFA_GEColor(&shading));
  pGS->FillPath(fillPath, FXFILL_WINDING, &matrix);
  pGS->RestoreGraphState();
}

// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_stroke.h"

#include <utility>

#include "fxjs/xfa/cjx_object.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/parser/cxfa_color.h"
#include "xfa/fxfa/parser/cxfa_measurement.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/xfa_utils.h"
#include "xfa/fxgraphics/cxfa_graphics.h"

void XFA_StrokeTypeSetLineDash(CXFA_Graphics* pGraphics,
                               XFA_AttributeValue iStrokeType,
                               XFA_AttributeValue iCapType) {
  switch (iStrokeType) {
    case XFA_AttributeValue::DashDot: {
      float dashArray[] = {4, 1, 2, 1};
      if (iCapType != XFA_AttributeValue::Butt) {
        dashArray[1] = 2;
        dashArray[3] = 2;
      }
      pGraphics->SetLineDash(0, dashArray, FX_ArraySize(dashArray));
      break;
    }
    case XFA_AttributeValue::DashDotDot: {
      float dashArray[] = {4, 1, 2, 1, 2, 1};
      if (iCapType != XFA_AttributeValue::Butt) {
        dashArray[1] = 2;
        dashArray[3] = 2;
        dashArray[5] = 2;
      }
      pGraphics->SetLineDash(0, dashArray, FX_ArraySize(dashArray));
      break;
    }
    case XFA_AttributeValue::Dashed: {
      float dashArray[] = {5, 1};
      if (iCapType != XFA_AttributeValue::Butt)
        dashArray[1] = 2;

      pGraphics->SetLineDash(0, dashArray, FX_ArraySize(dashArray));
      break;
    }
    case XFA_AttributeValue::Dotted: {
      float dashArray[] = {2, 1};
      if (iCapType != XFA_AttributeValue::Butt)
        dashArray[1] = 2;

      pGraphics->SetLineDash(0, dashArray, FX_ArraySize(dashArray));
      break;
    }
    default:
      pGraphics->SetSolidLineDash();
      break;
  }
}

CXFA_Stroke::CXFA_Stroke(CXFA_Document* pDoc,
                         XFA_PacketType ePacket,
                         uint32_t validPackets,
                         XFA_ObjectType oType,
                         XFA_Element eType,
                         const PropertyData* properties,
                         const AttributeData* attributes,
                         std::unique_ptr<CJX_Object> js_node)
    : CXFA_Node(pDoc,
                ePacket,
                validPackets,
                oType,
                eType,
                properties,
                attributes,
                std::move(js_node)) {}

CXFA_Stroke::~CXFA_Stroke() = default;

bool CXFA_Stroke::IsVisible() {
  XFA_AttributeValue presence = JSObject()
                                    ->TryEnum(XFA_Attribute::Presence, true)
                                    .value_or(XFA_AttributeValue::Visible);
  return presence == XFA_AttributeValue::Visible;
}

XFA_AttributeValue CXFA_Stroke::GetCapType() {
  return JSObject()->GetEnum(XFA_Attribute::Cap);
}

XFA_AttributeValue CXFA_Stroke::GetStrokeType() {
  return JSObject()->GetEnum(XFA_Attribute::Stroke);
}

float CXFA_Stroke::GetThickness() const {
  return GetMSThickness().ToUnit(XFA_Unit::Pt);
}

CXFA_Measurement CXFA_Stroke::GetMSThickness() const {
  return JSObject()->GetMeasure(XFA_Attribute::Thickness);
}

void CXFA_Stroke::SetMSThickness(CXFA_Measurement msThinkness) {
  JSObject()->SetMeasure(XFA_Attribute::Thickness, msThinkness, false);
}

FX_ARGB CXFA_Stroke::GetColor() {
  CXFA_Color* pNode = GetChild<CXFA_Color>(0, XFA_Element::Color, false);
  if (!pNode)
    return 0xFF000000;

  return StringToFXARGB(
      pNode->JSObject()->GetCData(XFA_Attribute::Value).AsStringView());
}

void CXFA_Stroke::SetColor(FX_ARGB argb) {
  CXFA_Color* pNode =
      JSObject()->GetOrCreateProperty<CXFA_Color>(0, XFA_Element::Color);
  if (!pNode)
    return;

  int a;
  int r;
  int g;
  int b;
  std::tie(a, r, g, b) = ArgbDecode(argb);
  pNode->JSObject()->SetCData(XFA_Attribute::Value,
                              WideString::Format(L"%d,%d,%d", r, g, b), false,
                              false);
}

XFA_AttributeValue CXFA_Stroke::GetJoinType() {
  return JSObject()->GetEnum(XFA_Attribute::Join);
}

bool CXFA_Stroke::IsInverted() {
  return JSObject()->GetBoolean(XFA_Attribute::Inverted);
}

float CXFA_Stroke::GetRadius() const {
  return JSObject()
      ->TryMeasure(XFA_Attribute::Radius, true)
      .value_or(CXFA_Measurement(0, XFA_Unit::In))
      .ToUnit(XFA_Unit::Pt);
}

bool CXFA_Stroke::SameStyles(CXFA_Stroke* stroke, uint32_t dwFlags) {
  if (this == stroke)
    return true;
  if (fabs(GetThickness() - stroke->GetThickness()) >= 0.01f)
    return false;
  if ((dwFlags & XFA_STROKE_SAMESTYLE_NoPresence) == 0 &&
      IsVisible() != stroke->IsVisible()) {
    return false;
  }
  if (GetStrokeType() != stroke->GetStrokeType())
    return false;
  if (GetColor() != stroke->GetColor())
    return false;
  if ((dwFlags & XFA_STROKE_SAMESTYLE_Corner) != 0 &&
      fabs(GetRadius() - stroke->GetRadius()) >= 0.01f) {
    return false;
  }
  return true;
}

void CXFA_Stroke::Stroke(CXFA_GEPath* pPath,
                         CXFA_Graphics* pGS,
                         const CFX_Matrix& matrix) {
  if (!IsVisible())
    return;

  float fThickness = GetThickness();
  if (fThickness < 0.001f)
    return;

  pGS->SaveGraphState();
  if (IsCorner() && fThickness > 2 * GetRadius())
    fThickness = 2 * GetRadius();

  pGS->SetLineWidth(fThickness);
  pGS->EnableActOnDash();
  pGS->SetLineCap(CFX_GraphStateData::LineCapButt);
  XFA_StrokeTypeSetLineDash(pGS, GetStrokeType(), XFA_AttributeValue::Butt);
  pGS->SetStrokeColor(CXFA_GEColor(GetColor()));
  pGS->StrokePath(pPath, &matrix);
  pGS->RestoreGraphState();
}

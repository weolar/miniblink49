// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_box.h"

#include <algorithm>
#include <utility>

#include "fxjs/xfa/cjx_object.h"
#include "xfa/fxfa/parser/cxfa_corner.h"
#include "xfa/fxfa/parser/cxfa_edge.h"
#include "xfa/fxfa/parser/cxfa_fill.h"
#include "xfa/fxfa/parser/cxfa_margin.h"
#include "xfa/fxfa/parser/cxfa_measurement.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/cxfa_rectangle.h"
#include "xfa/fxgraphics/cxfa_gepath.h"
#include "xfa/fxgraphics/cxfa_gepattern.h"
#include "xfa/fxgraphics/cxfa_geshading.h"
#include "xfa/fxgraphics/cxfa_graphics.h"

namespace {

std::pair<XFA_AttributeValue, CXFA_Stroke*> Style3D(
    const std::vector<CXFA_Stroke*>& strokes) {
  if (strokes.empty())
    return {XFA_AttributeValue::Unknown, nullptr};

  CXFA_Stroke* stroke = strokes[0];
  for (size_t i = 1; i < strokes.size(); i++) {
    CXFA_Stroke* find = strokes[i];
    if (!find)
      continue;
    if (!stroke)
      stroke = find;
    else if (stroke->GetStrokeType() != find->GetStrokeType())
      stroke = find;
    break;
  }

  XFA_AttributeValue iType = stroke->GetStrokeType();
  if (iType == XFA_AttributeValue::Lowered ||
      iType == XFA_AttributeValue::Raised ||
      iType == XFA_AttributeValue::Etched ||
      iType == XFA_AttributeValue::Embossed) {
    return {iType, stroke};
  }
  return {XFA_AttributeValue::Unknown, stroke};
}

CXFA_Rectangle* ToRectangle(CXFA_Box* box) {
  return static_cast<CXFA_Rectangle*>(box);
}

}  // namespace

CXFA_Box::CXFA_Box(CXFA_Document* pDoc,
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

CXFA_Box::~CXFA_Box() = default;

XFA_AttributeValue CXFA_Box::GetHand() {
  return JSObject()->GetEnum(XFA_Attribute::Hand);
}

XFA_AttributeValue CXFA_Box::GetPresence() {
  return JSObject()
      ->TryEnum(XFA_Attribute::Presence, true)
      .value_or(XFA_AttributeValue::Visible);
}

int32_t CXFA_Box::CountEdges() {
  return CountChildren(XFA_Element::Edge, false);
}

CXFA_Edge* CXFA_Box::GetEdgeIfExists(int32_t nIndex) {
  if (nIndex == 0)
    return JSObject()->GetOrCreateProperty<CXFA_Edge>(nIndex,
                                                      XFA_Element::Edge);
  return JSObject()->GetProperty<CXFA_Edge>(nIndex, XFA_Element::Edge);
}

std::vector<CXFA_Stroke*> CXFA_Box::GetStrokes() {
  return GetStrokesInternal(false);
}

bool CXFA_Box::IsCircular() {
  return JSObject()->GetBoolean(XFA_Attribute::Circular);
}

Optional<int32_t> CXFA_Box::GetStartAngle() {
  return JSObject()->TryInteger(XFA_Attribute::StartAngle, false);
}

Optional<int32_t> CXFA_Box::GetSweepAngle() {
  return JSObject()->TryInteger(XFA_Attribute::SweepAngle, false);
}

CXFA_Fill* CXFA_Box::GetOrCreateFillIfPossible() {
  return JSObject()->GetOrCreateProperty<CXFA_Fill>(0, XFA_Element::Fill);
}

std::tuple<XFA_AttributeValue, bool, float> CXFA_Box::Get3DStyle() {
  if (GetElementType() == XFA_Element::Arc)
    return {XFA_AttributeValue::Unknown, false, 0.0f};

  std::vector<CXFA_Stroke*> strokes = GetStrokesInternal(true);
  CXFA_Stroke* stroke;
  XFA_AttributeValue iType;

  std::tie(iType, stroke) = Style3D(strokes);
  if (iType == XFA_AttributeValue::Unknown)
    return {XFA_AttributeValue::Unknown, false, 0.0f};

  return {iType, stroke->IsVisible(), stroke->GetThickness()};
}

std::vector<CXFA_Stroke*> CXFA_Box::GetStrokesInternal(bool bNull) {
  std::vector<CXFA_Stroke*> strokes;
  strokes.resize(8);

  for (int32_t i = 0, j = 0; i < 4; i++) {
    CXFA_Corner* corner;
    if (i == 0) {
      corner =
          JSObject()->GetOrCreateProperty<CXFA_Corner>(i, XFA_Element::Corner);
    } else {
      corner = JSObject()->GetProperty<CXFA_Corner>(i, XFA_Element::Corner);
    }

    // TODO(dsinclair): If i == 0 and GetOrCreateProperty failed, we can end up
    // with a null corner in the first position.
    if (corner || i == 0) {
      strokes[j] = corner;
    } else if (!bNull) {
      if (i == 1 || i == 2)
        strokes[j] = strokes[0];
      else
        strokes[j] = strokes[2];
    }
    j++;

    CXFA_Edge* edge;
    if (i == 0)
      edge = JSObject()->GetOrCreateProperty<CXFA_Edge>(i, XFA_Element::Edge);
    else
      edge = JSObject()->GetProperty<CXFA_Edge>(i, XFA_Element::Edge);

    // TODO(dsinclair): If i == 0 and GetOrCreateProperty failed, we can end up
    // with a null edge in the first position.
    if (edge || i == 0) {
      strokes[j] = edge;
    } else if (!bNull) {
      if (i == 1 || i == 2)
        strokes[j] = strokes[1];
      else
        strokes[j] = strokes[3];
    }
    j++;
  }
  return strokes;
}

void CXFA_Box::Draw(CXFA_Graphics* pGS,
                    const CFX_RectF& rtWidget,
                    const CFX_Matrix& matrix,
                    bool forceRound) {
  if (GetPresence() != XFA_AttributeValue::Visible)
    return;

  XFA_Element eType = GetElementType();
  if (eType != XFA_Element::Arc && eType != XFA_Element::Border &&
      eType != XFA_Element::Rectangle) {
    return;
  }
  std::vector<CXFA_Stroke*> strokes;
  if (!forceRound && eType != XFA_Element::Arc)
    strokes = GetStrokes();

  DrawFill(strokes, pGS, rtWidget, matrix, forceRound);
  XFA_Element type = GetElementType();
  if (type == XFA_Element::Arc || forceRound) {
    StrokeArcOrRounded(pGS, rtWidget, matrix, forceRound);
  } else if (type == XFA_Element::Rectangle || type == XFA_Element::Border) {
    ToRectangle(this)->Draw(strokes, pGS, rtWidget, matrix);
  } else {
    NOTREACHED();
  }
}

void CXFA_Box::DrawFill(const std::vector<CXFA_Stroke*>& strokes,
                        CXFA_Graphics* pGS,
                        CFX_RectF rtWidget,
                        const CFX_Matrix& matrix,
                        bool forceRound) {
  CXFA_Fill* fill = JSObject()->GetProperty<CXFA_Fill>(0, XFA_Element::Fill);
  if (!fill || !fill->IsVisible())
    return;

  pGS->SaveGraphState();

  CXFA_GEPath fillPath;
  XFA_Element type = GetElementType();
  if (type == XFA_Element::Arc || forceRound) {
    CXFA_Edge* edge = GetEdgeIfExists(0);
    float fThickness = std::fmax(0.0, edge ? edge->GetThickness() : 0);
    float fHalf = fThickness / 2;
    XFA_AttributeValue iHand = GetHand();
    if (iHand == XFA_AttributeValue::Left)
      rtWidget.Inflate(fHalf, fHalf);
    else if (iHand == XFA_AttributeValue::Right)
      rtWidget.Deflate(fHalf, fHalf);

    GetPathArcOrRounded(rtWidget, forceRound, &fillPath);
  } else if (type == XFA_Element::Rectangle || type == XFA_Element::Border) {
    ToRectangle(this)->GetFillPath(strokes, rtWidget, &fillPath);
  } else {
    NOTREACHED();
  }
  fillPath.Close();

  fill->Draw(pGS, &fillPath, rtWidget, matrix);
  pGS->RestoreGraphState();
}

void CXFA_Box::GetPathArcOrRounded(CFX_RectF rtDraw,
                                   bool forceRound,
                                   CXFA_GEPath* fillPath) {
  float a, b;
  a = rtDraw.width / 2.0f;
  b = rtDraw.height / 2.0f;
  if (IsCircular() || forceRound)
    a = b = std::min(a, b);

  CFX_PointF center = rtDraw.Center();
  rtDraw.left = center.x - a;
  rtDraw.top = center.y - b;
  rtDraw.width = a + a;
  rtDraw.height = b + b;
  Optional<int32_t> startAngle = GetStartAngle();
  Optional<int32_t> sweepAngle = GetSweepAngle();
  if (!startAngle && !sweepAngle) {
    fillPath->AddEllipse(rtDraw);
    return;
  }

  fillPath->AddArc(rtDraw.TopLeft(), rtDraw.Size(),
                   -startAngle.value_or(0) * FX_PI / 180.0f,
                   -sweepAngle.value_or(360) * FX_PI / 180.0f);
}

void CXFA_Box::StrokeArcOrRounded(CXFA_Graphics* pGS,
                                  CFX_RectF rtWidget,
                                  const CFX_Matrix& matrix,
                                  bool forceRound) {
  CXFA_Edge* edge = GetEdgeIfExists(0);
  if (!edge || !edge->IsVisible())
    return;

  bool bVisible;
  float fThickness;
  XFA_AttributeValue i3DType;
  std::tie(i3DType, bVisible, fThickness) = Get3DStyle();
  bool lowered3d = false;
  if (i3DType != XFA_AttributeValue::Unknown) {
    if (bVisible && fThickness >= 0.001f)
      lowered3d = true;
  }

  float fHalf = edge->GetThickness() / 2;
  if (fHalf < 0) {
    fHalf = 0;
  }

  XFA_AttributeValue iHand = GetHand();
  if (iHand == XFA_AttributeValue::Left) {
    rtWidget.Inflate(fHalf, fHalf);
  } else if (iHand == XFA_AttributeValue::Right) {
    rtWidget.Deflate(fHalf, fHalf);
  }
  if (!forceRound || !lowered3d) {
    if (fHalf < 0.001f)
      return;

    CXFA_GEPath arcPath;
    GetPathArcOrRounded(rtWidget, forceRound, &arcPath);
    if (edge)
      edge->Stroke(&arcPath, pGS, matrix);
    return;
  }
  pGS->SaveGraphState();
  pGS->SetLineWidth(fHalf);

  float a, b;
  a = rtWidget.width / 2.0f;
  b = rtWidget.height / 2.0f;
  if (forceRound) {
    a = std::min(a, b);
    b = a;
  }

  CFX_PointF center = rtWidget.Center();
  rtWidget.left = center.x - a;
  rtWidget.top = center.y - b;
  rtWidget.width = a + a;
  rtWidget.height = b + b;

  CXFA_GEPath arcPath;
  arcPath.AddArc(rtWidget.TopLeft(), rtWidget.Size(), 3.0f * FX_PI / 4.0f,
                 FX_PI);

  pGS->SetStrokeColor(CXFA_GEColor(0xFF808080));
  pGS->StrokePath(&arcPath, &matrix);
  arcPath.Clear();
  arcPath.AddArc(rtWidget.TopLeft(), rtWidget.Size(), -1.0f * FX_PI / 4.0f,
                 FX_PI);

  pGS->SetStrokeColor(CXFA_GEColor(0xFFFFFFFF));
  pGS->StrokePath(&arcPath, &matrix);
  rtWidget.Deflate(fHalf, fHalf);
  arcPath.Clear();
  arcPath.AddArc(rtWidget.TopLeft(), rtWidget.Size(), 3.0f * FX_PI / 4.0f,
                 FX_PI);

  pGS->SetStrokeColor(CXFA_GEColor(0xFF404040));
  pGS->StrokePath(&arcPath, &matrix);
  arcPath.Clear();
  arcPath.AddArc(rtWidget.TopLeft(), rtWidget.Size(), -1.0f * FX_PI / 4.0f,
                 FX_PI);

  pGS->SetStrokeColor(CXFA_GEColor(0xFFC0C0C0));
  pGS->StrokePath(&arcPath, &matrix);
  pGS->RestoreGraphState();
}

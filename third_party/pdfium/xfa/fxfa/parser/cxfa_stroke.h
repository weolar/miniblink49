// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_STROKE_H_
#define XFA_FXFA_PARSER_CXFA_STROKE_H_

#include <memory>

#include "core/fxge/fx_dib.h"
#include "xfa/fxfa/fxfa_basic.h"
#include "xfa/fxfa/parser/cxfa_node.h"

enum StrokeSameStyle {
  XFA_STROKE_SAMESTYLE_NoPresence = 1,
  XFA_STROKE_SAMESTYLE_Corner = 2
};

class CXFA_GEPath;
class CXFA_Graphics;
class CXFA_Node;

void XFA_StrokeTypeSetLineDash(CXFA_Graphics* pGraphics,
                               XFA_AttributeValue iStrokeType,
                               XFA_AttributeValue iCapType);

class CXFA_Stroke : public CXFA_Node {
 public:
  ~CXFA_Stroke() override;

  bool IsCorner() const { return GetElementType() == XFA_Element::Corner; }
  bool IsVisible();
  bool IsInverted();

  XFA_AttributeValue GetCapType();
  XFA_AttributeValue GetStrokeType();
  XFA_AttributeValue GetJoinType();
  float GetRadius() const;
  float GetThickness() const;

  CXFA_Measurement GetMSThickness() const;
  void SetMSThickness(CXFA_Measurement msThinkness);

  FX_ARGB GetColor();
  void SetColor(FX_ARGB argb);

  bool SameStyles(CXFA_Stroke* stroke, uint32_t dwFlags);

  void Stroke(CXFA_GEPath* pPath, CXFA_Graphics* pGS, const CFX_Matrix& matrix);

 protected:
  CXFA_Stroke(CXFA_Document* pDoc,
              XFA_PacketType ePacket,
              uint32_t validPackets,
              XFA_ObjectType oType,
              XFA_Element eType,
              const PropertyData* properties,
              const AttributeData* attributes,
              std::unique_ptr<CJX_Object> js_node);
};

#endif  // XFA_FXFA_PARSER_CXFA_STROKE_H_

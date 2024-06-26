// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_RECTANGLE_H_
#define XFA_FXFA_PARSER_CXFA_RECTANGLE_H_

#include <memory>
#include <vector>

#include "xfa/fxfa/parser/cxfa_box.h"
#include "xfa/fxgraphics/cxfa_gepath.h"

class CXFA_Rectangle : public CXFA_Box {
 public:
  CXFA_Rectangle(CXFA_Document* doc, XFA_PacketType packet);
  ~CXFA_Rectangle() override;

  void GetFillPath(const std::vector<CXFA_Stroke*>& strokes,
                   const CFX_RectF& rtWidget,
                   CXFA_GEPath* fillPath);
  void Draw(const std::vector<CXFA_Stroke*>& strokes,
            CXFA_Graphics* pGS,
            CFX_RectF rtWidget,
            const CFX_Matrix& matrix);

 protected:
  CXFA_Rectangle(CXFA_Document* pDoc,
                 XFA_PacketType ePacket,
                 uint32_t validPackets,
                 XFA_ObjectType oType,
                 XFA_Element eType,
                 const PropertyData* properties,
                 const AttributeData* attributes,
                 std::unique_ptr<CJX_Object> js_node);

 private:
  void Stroke(const std::vector<CXFA_Stroke*>& strokes,
              CXFA_Graphics* pGS,
              CFX_RectF rtWidget,
              const CFX_Matrix& matrix);
  void StrokeEmbossed(CXFA_Graphics* pGS,
                      CFX_RectF rt,
                      float fThickness,
                      const CFX_Matrix& matrix);
  void StrokeLowered(CXFA_Graphics* pGS,
                     CFX_RectF rt,
                     float fThickness,
                     const CFX_Matrix& matrix);
  void StrokeRaised(CXFA_Graphics* pGS,
                    CFX_RectF rt,
                    float fThickness,
                    const CFX_Matrix& matrix);
  void StrokeEtched(CXFA_Graphics* pGS,
                    CFX_RectF rt,
                    float fThickness,
                    const CFX_Matrix& matrix);
  void StrokeRect(CXFA_Graphics* pGraphic,
                  const CFX_RectF& rt,
                  float fLineWidth,
                  const CFX_Matrix& matrix,
                  FX_ARGB argbTopLeft,
                  FX_ARGB argbBottomRight);
  void GetPath(const std::vector<CXFA_Stroke*>& strokes,
               CFX_RectF rtWidget,
               CXFA_GEPath& path,
               int32_t nIndex,
               bool bStart,
               bool bCorner);
};

#endif  // XFA_FXFA_PARSER_CXFA_RECTANGLE_H_

// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_BOX_H_
#define XFA_FXFA_PARSER_CXFA_BOX_H_

#include <memory>
#include <tuple>
#include <vector>

#include "core/fxcrt/fx_coordinates.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxgraphics/cxfa_gepath.h"

class CXFA_Edge;
class CXFA_Fill;
class CXFA_Graphics;
class CXFA_Margin;
class CXFA_Stroke;

class CXFA_Box : public CXFA_Node {
 public:
  ~CXFA_Box() override;

  XFA_AttributeValue GetPresence();
  std::tuple<XFA_AttributeValue, bool, float> Get3DStyle();

  int32_t CountEdges();
  CXFA_Edge* GetEdgeIfExists(int32_t nIndex);
  CXFA_Fill* GetOrCreateFillIfPossible();

  std::vector<CXFA_Stroke*> GetStrokes();

  void Draw(CXFA_Graphics* pGS,
            const CFX_RectF& rtWidget,
            const CFX_Matrix& matrix,
            bool forceRound);

 protected:
  CXFA_Box(CXFA_Document* pDoc,
           XFA_PacketType ePacket,
           uint32_t validPackets,
           XFA_ObjectType oType,
           XFA_Element eType,
           const PropertyData* properties,
           const AttributeData* attributes,
           std::unique_ptr<CJX_Object> js_node);

  XFA_AttributeValue GetHand();

 private:
  bool IsCircular();
  Optional<int32_t> GetStartAngle();
  Optional<int32_t> GetSweepAngle();

  std::vector<CXFA_Stroke*> GetStrokesInternal(bool bNull);
  void DrawFill(const std::vector<CXFA_Stroke*>& strokes,
                CXFA_Graphics* pGS,
                CFX_RectF rtWidget,
                const CFX_Matrix& matrix,
                bool forceRound);
  void StrokeArcOrRounded(CXFA_Graphics* pGS,
                          CFX_RectF rtWidget,
                          const CFX_Matrix& matrix,
                          bool forceRound);
  void GetPathArcOrRounded(CFX_RectF rtDraw,
                           bool forceRound,
                           CXFA_GEPath* fillPath);
};

#endif  // XFA_FXFA_PARSER_CXFA_BOX_H_

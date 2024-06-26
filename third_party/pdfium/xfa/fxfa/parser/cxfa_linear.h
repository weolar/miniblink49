// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_LINEAR_H_
#define XFA_FXFA_PARSER_CXFA_LINEAR_H_

#include "core/fxcrt/fx_coordinates.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxgraphics/cxfa_gepath.h"

class CXFA_Color;
class CXFA_Graphics;

class CXFA_Linear final : public CXFA_Node {
 public:
  static constexpr XFA_AttributeValue kDefaultType =
      XFA_AttributeValue::ToRight;

  CXFA_Linear(CXFA_Document* doc, XFA_PacketType packet);
  ~CXFA_Linear() override;

  void Draw(CXFA_Graphics* pGS,
            CXFA_GEPath* fillPath,
            FX_ARGB crStart,
            const CFX_RectF& rtFill,
            const CFX_Matrix& matrix);

 private:
  XFA_AttributeValue GetType();
  CXFA_Color* GetColorIfExists();
};

#endif  // XFA_FXFA_PARSER_CXFA_LINEAR_H_

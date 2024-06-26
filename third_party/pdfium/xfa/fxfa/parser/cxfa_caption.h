// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_CAPTION_H_
#define XFA_FXFA_PARSER_CXFA_CAPTION_H_

#include "xfa/fxfa/parser/cxfa_node.h"

class CXFA_Font;
class CXFA_Margin;
class CXFA_Value;

class CXFA_Caption final : public CXFA_Node {
 public:
  static constexpr XFA_AttributeValue kDefaultPlacementType =
      XFA_AttributeValue::Left;

  CXFA_Caption(CXFA_Document* doc, XFA_PacketType packet);
  ~CXFA_Caption() override;

  bool IsVisible();
  bool IsHidden();
  XFA_AttributeValue GetPlacementType();
  float GetReserve() const;
  CXFA_Margin* GetMarginIfExists();
  CXFA_Font* GetFontIfExists();
  CXFA_Value* GetValueIfExists();
};

#endif  // XFA_FXFA_PARSER_CXFA_CAPTION_H_

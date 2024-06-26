// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_MARGIN_H_
#define XFA_FXFA_PARSER_CXFA_MARGIN_H_

#include "xfa/fxfa/parser/cxfa_node.h"

class CXFA_Margin final : public CXFA_Node {
 public:
  CXFA_Margin(CXFA_Document* doc, XFA_PacketType packet);
  ~CXFA_Margin() override;

  float GetLeftInset() const;
  float GetTopInset() const;
  float GetRightInset() const;
  float GetBottomInset() const;

  Optional<float> TryLeftInset() const;
  Optional<float> TryTopInset() const;
  Optional<float> TryRightInset() const;
  Optional<float> TryBottomInset() const;
};

#endif  // XFA_FXFA_PARSER_CXFA_MARGIN_H_

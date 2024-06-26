// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_BUTTON_H_
#define XFA_FXFA_PARSER_CXFA_BUTTON_H_

#include "xfa/fxfa/parser/cxfa_node.h"

class CXFA_Button final : public CXFA_Node {
 public:
  CXFA_Button(CXFA_Document* doc, XFA_PacketType packet);
  ~CXFA_Button() override;

  XFA_FFWidgetType GetDefaultFFWidgetType() const override;

  XFA_AttributeValue GetHighlight();
};

#endif  // XFA_FXFA_PARSER_CXFA_BUTTON_H_

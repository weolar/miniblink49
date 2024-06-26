// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_ALWAYSEMBED_H_
#define XFA_FXFA_PARSER_CXFA_ALWAYSEMBED_H_

#include "xfa/fxfa/parser/cxfa_node.h"

class CXFA_AlwaysEmbed final : public CXFA_Node {
 public:
  CXFA_AlwaysEmbed(CXFA_Document* doc, XFA_PacketType packet);
  ~CXFA_AlwaysEmbed() override;
};

#endif  // XFA_FXFA_PARSER_CXFA_ALWAYSEMBED_H_

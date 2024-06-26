// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_PAGINATIONOVERRIDE_H_
#define XFA_FXFA_PARSER_CXFA_PAGINATIONOVERRIDE_H_

#include "xfa/fxfa/parser/cxfa_node.h"

class CXFA_PaginationOverride final : public CXFA_Node {
 public:
  CXFA_PaginationOverride(CXFA_Document* doc, XFA_PacketType packet);
  ~CXFA_PaginationOverride() override;
};

#endif  // XFA_FXFA_PARSER_CXFA_PAGINATIONOVERRIDE_H_

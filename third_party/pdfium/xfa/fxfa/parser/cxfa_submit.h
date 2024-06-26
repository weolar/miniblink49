// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_SUBMIT_H_
#define XFA_FXFA_PARSER_CXFA_SUBMIT_H_

#include "core/fxcrt/widestring.h"
#include "xfa/fxfa/parser/cxfa_node.h"

class CXFA_Submit final : public CXFA_Node {
 public:
  CXFA_Submit(CXFA_Document* doc, XFA_PacketType packet);
  ~CXFA_Submit() override;

  bool IsSubmitEmbedPDF();
  XFA_AttributeValue GetSubmitFormat();
  WideString GetSubmitTarget();
  WideString GetSubmitXDPContent();
};

#endif  // XFA_FXFA_PARSER_CXFA_SUBMIT_H_

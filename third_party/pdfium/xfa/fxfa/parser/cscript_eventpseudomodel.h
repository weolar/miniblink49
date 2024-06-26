// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CSCRIPT_EVENTPSEUDOMODEL_H_
#define XFA_FXFA_PARSER_CSCRIPT_EVENTPSEUDOMODEL_H_

#include "xfa/fxfa/parser/cxfa_object.h"

class CXFA_Document;

class CScript_EventPseudoModel final : public CXFA_Object {
 public:
  explicit CScript_EventPseudoModel(CXFA_Document* pDocument);
  ~CScript_EventPseudoModel() override;
};

#endif  // XFA_FXFA_PARSER_CSCRIPT_EVENTPSEUDOMODEL_H_

// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_DELTAS_H_
#define XFA_FXFA_PARSER_CXFA_DELTAS_H_

#include "xfa/fxfa/parser/cxfa_list.h"

class CXFA_Deltas : public CXFA_List {
 public:
  explicit CXFA_Deltas(CXFA_Document* doc);
  ~CXFA_Deltas() override;
};

#endif  // XFA_FXFA_PARSER_CXFA_DELTAS_H_

// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_TREELIST_H_
#define XFA_FXFA_PARSER_CXFA_TREELIST_H_

#include "core/fxcrt/fx_string.h"
#include "xfa/fxfa/parser/cxfa_list.h"

class CXFA_Node;

class CXFA_TreeList : public CXFA_List {
 public:
  explicit CXFA_TreeList(CXFA_Document* pDocument);
  ~CXFA_TreeList() override;

  CXFA_Node* NamedItem(WideStringView wsName);
};

#endif  // XFA_FXFA_PARSER_CXFA_TREELIST_H_

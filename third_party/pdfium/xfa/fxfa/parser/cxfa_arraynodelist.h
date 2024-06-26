// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_ARRAYNODELIST_H_
#define XFA_FXFA_PARSER_CXFA_ARRAYNODELIST_H_

#include <vector>

#include "xfa/fxfa/parser/cxfa_treelist.h"

class CXFA_Document;
class CXFA_Node;

class CXFA_ArrayNodeList final : public CXFA_TreeList {
 public:
  explicit CXFA_ArrayNodeList(CXFA_Document* pDocument);
  ~CXFA_ArrayNodeList() override;

  // From CXFA_TreeList.
  size_t GetLength() override;
  void Append(CXFA_Node* pNode) override;
  void Insert(CXFA_Node* pNewNode, CXFA_Node* pBeforeNode) override;
  void Remove(CXFA_Node* pNode) override;
  CXFA_Node* Item(size_t iIndex) override;

  void SetArrayNodeList(std::vector<CXFA_Node*> srcArray);

 private:
  std::vector<CXFA_Node*> m_array;
};

#endif  // XFA_FXFA_PARSER_CXFA_ARRAYNODELIST_H_

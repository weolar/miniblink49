// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_LIST_H_
#define XFA_FXFA_PARSER_CXFA_LIST_H_

#include <memory>

#include "xfa/fxfa/parser/cxfa_object.h"

class CXFA_Document;

class CXFA_List : public CXFA_Object {
 public:
  ~CXFA_List() override;

  virtual size_t GetLength() = 0;
  virtual void Append(CXFA_Node* pNode) = 0;
  virtual void Insert(CXFA_Node* pNewNode, CXFA_Node* pBeforeNode) = 0;
  virtual void Remove(CXFA_Node* pNode) = 0;
  virtual CXFA_Node* Item(size_t iIndex) = 0;

 protected:
  CXFA_List(CXFA_Document* doc, std::unique_ptr<CJX_Object> js_obj);
  CXFA_List(CXFA_Document* pDocument,
            XFA_ObjectType objectType,
            XFA_Element eType,
            std::unique_ptr<CJX_Object> obj);
};

#endif  // XFA_FXFA_PARSER_CXFA_LIST_H_

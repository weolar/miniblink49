// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_CONTAINERLAYOUTITEM_H_
#define XFA_FXFA_PARSER_CXFA_CONTAINERLAYOUTITEM_H_

#include "xfa/fxfa/parser/cxfa_layoutitem.h"

class CXFA_ContainerLayoutItem : public CXFA_LayoutItem {
 public:
  explicit CXFA_ContainerLayoutItem(CXFA_Node* pNode);
  ~CXFA_ContainerLayoutItem() override;

  CXFA_LayoutProcessor* GetLayout() const;
  int32_t GetPageIndex() const;
  CFX_SizeF GetPageSize() const;
  CXFA_Node* GetMasterPage() const;

  UnownedPtr<CXFA_Node> m_pOldSubform;
};

#endif  // XFA_FXFA_PARSER_CXFA_CONTAINERLAYOUTITEM_H_

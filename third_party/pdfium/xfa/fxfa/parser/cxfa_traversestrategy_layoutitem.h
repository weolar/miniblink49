// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_TRAVERSESTRATEGY_LAYOUTITEM_H_
#define XFA_FXFA_PARSER_CXFA_TRAVERSESTRATEGY_LAYOUTITEM_H_

#include "xfa/fxfa/parser/cxfa_layoutitem.h"
#include "xfa/fxfa/parser/cxfa_nodeiteratortemplate.h"

class CXFA_TraverseStrategy_LayoutItem {
 public:
  static CXFA_LayoutItem* GetFirstChild(CXFA_LayoutItem* pLayoutItem) {
    return pLayoutItem->m_pFirstChild;
  }
  static CXFA_LayoutItem* GetNextSibling(CXFA_LayoutItem* pLayoutItem) {
    return pLayoutItem->m_pNextSibling;
  }
  static CXFA_LayoutItem* GetParent(CXFA_LayoutItem* pLayoutItem) {
    return pLayoutItem->m_pParent;
  }
};

using CXFA_LayoutItemIterator =
    CXFA_NodeIteratorTemplate<CXFA_LayoutItem,
                              CXFA_TraverseStrategy_LayoutItem>;

#endif  // XFA_FXFA_PARSER_CXFA_TRAVERSESTRATEGY_LAYOUTITEM_H_

// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_TRAVERSESTRATEGY_CONTENTAREACONTAINERLAYOUTITEM_H_
#define XFA_FXFA_PARSER_CXFA_TRAVERSESTRATEGY_CONTENTAREACONTAINERLAYOUTITEM_H_

#include "xfa/fxfa/parser/cxfa_containerlayoutitem.h"

class CXFA_TraverseStrategy_ContentAreaContainerLayoutItem {
 public:
  static CXFA_ContainerLayoutItem* GetFirstChild(
      CXFA_ContainerLayoutItem* pLayoutItem) {
    for (CXFA_LayoutItem* pChildItem = pLayoutItem->m_pFirstChild; pChildItem;
         pChildItem = pChildItem->m_pNextSibling) {
      if (CXFA_ContainerLayoutItem* pContainer =
              pChildItem->AsContainerLayoutItem()) {
        return pContainer;
      }
    }
    return nullptr;
  }

  static CXFA_ContainerLayoutItem* GetNextSibling(
      CXFA_ContainerLayoutItem* pLayoutItem) {
    for (CXFA_LayoutItem* pChildItem = pLayoutItem->m_pNextSibling; pChildItem;
         pChildItem = pChildItem->m_pNextSibling) {
      if (CXFA_ContainerLayoutItem* pContainer =
              pChildItem->AsContainerLayoutItem()) {
        return pContainer;
      }
    }
    return nullptr;
  }

  static CXFA_ContainerLayoutItem* GetParent(
      CXFA_ContainerLayoutItem* pLayoutItem) {
    return ToContainerLayoutItem(pLayoutItem->m_pParent);
  }
};

#endif  // XFA_FXFA_PARSER_CXFA_TRAVERSESTRATEGY_CONTENTAREACONTAINERLAYOUTITEM_H_

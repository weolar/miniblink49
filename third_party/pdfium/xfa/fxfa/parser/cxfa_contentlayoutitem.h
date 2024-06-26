// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_CONTENTLAYOUTITEM_H_
#define XFA_FXFA_PARSER_CXFA_CONTENTLAYOUTITEM_H_

#include "xfa/fxfa/parser/cxfa_layoutitem.h"

class CXFA_FFWidget;

class CXFA_ContentLayoutItem : public CXFA_LayoutItem {
 public:
  explicit CXFA_ContentLayoutItem(CXFA_Node* pNode);
  ~CXFA_ContentLayoutItem() override;

  virtual CXFA_FFWidget* AsFFWidget();

  CXFA_ContentLayoutItem* GetFirst();
  CXFA_ContentLayoutItem* GetLast();
  CXFA_ContentLayoutItem* GetPrev() const { return m_pPrev; }
  CXFA_ContentLayoutItem* GetNext() const { return m_pNext; }

  CFX_RectF GetRect(bool bRelative) const;
  int32_t GetIndex() const;
  int32_t GetCount() const;

  CXFA_ContentLayoutItem* m_pPrev = nullptr;
  CXFA_ContentLayoutItem* m_pNext = nullptr;
  CFX_PointF m_sPos;
  CFX_SizeF m_sSize;
  mutable uint32_t m_dwStatus = 0;
};

inline CXFA_FFWidget* ToFFWidget(CXFA_ContentLayoutItem* item) {
  return item ? item->AsFFWidget() : nullptr;
}

#endif  // XFA_FXFA_PARSER_CXFA_CONTENTLAYOUTITEM_H_

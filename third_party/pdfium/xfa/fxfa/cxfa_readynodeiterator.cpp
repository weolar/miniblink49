// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_readynodeiterator.h"

#include "xfa/fxfa/parser/cxfa_node.h"

CXFA_ReadyNodeIterator::CXFA_ReadyNodeIterator(CXFA_Node* pTravelRoot)
    : m_ContentIterator(pTravelRoot) {}

CXFA_ReadyNodeIterator::~CXFA_ReadyNodeIterator() {}

CXFA_Node* CXFA_ReadyNodeIterator::MoveToNext() {
  CXFA_Node* pItem = m_pCurNode ? m_ContentIterator.MoveToNext()
                                : m_ContentIterator.GetCurrent();
  while (pItem) {
    m_pCurNode = pItem->IsWidgetReady() ? pItem : nullptr;
    if (m_pCurNode)
      return m_pCurNode.Get();
    pItem = m_ContentIterator.MoveToNext();
  }
  return nullptr;
}

void CXFA_ReadyNodeIterator::SkipTree() {
  m_ContentIterator.SkipChildrenAndMoveToNext();
  m_pCurNode = nullptr;
}

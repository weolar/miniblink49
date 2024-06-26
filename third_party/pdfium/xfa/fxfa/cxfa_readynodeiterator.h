// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_READYNODEITERATOR_H_
#define XFA_FXFA_CXFA_READYNODEITERATOR_H_

#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fxfa/parser/cxfa_traversestrategy_xfacontainernode.h"

class CXFA_Node;

class CXFA_ReadyNodeIterator {
 public:
  explicit CXFA_ReadyNodeIterator(CXFA_Node* pTravelRoot);
  ~CXFA_ReadyNodeIterator();

  CXFA_Node* MoveToNext();
  void SkipTree();

 private:
  CXFA_ContainerIterator m_ContentIterator;
  UnownedPtr<CXFA_Node> m_pCurNode;
};

#endif  // XFA_FXFA_CXFA_READYNODEITERATOR_H_

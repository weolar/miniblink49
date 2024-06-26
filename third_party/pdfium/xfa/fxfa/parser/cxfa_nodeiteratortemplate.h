// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_NODEITERATORTEMPLATE_H_
#define XFA_FXFA_PARSER_CXFA_NODEITERATORTEMPLATE_H_

#include "core/fxcrt/unowned_ptr.h"

template <class NodeType, class TraverseStrategy>
class CXFA_NodeIteratorTemplate {
 public:
  explicit CXFA_NodeIteratorTemplate(NodeType* pRoot)
      : m_pRoot(pRoot), m_pCurrent(pRoot) {}

  NodeType* GetRoot() const { return m_pRoot.Get(); }
  NodeType* GetCurrent() const { return m_pCurrent.Get(); }

  void Reset() { m_pCurrent = m_pRoot; }
  bool SetCurrent(NodeType* pNode) {
    if (!RootReachableFromNode(pNode)) {
      m_pCurrent = nullptr;
      return false;
    }
    m_pCurrent = pNode;
    return true;
  }

  NodeType* MoveToPrev() {
    if (!m_pRoot)
      return nullptr;
    if (!m_pCurrent) {
      m_pCurrent = LastDescendant(m_pRoot.Get());
      return m_pCurrent.Get();
    }
    NodeType* pSibling = PreviousSiblingWithinSubtree(m_pCurrent.Get());
    if (pSibling) {
      m_pCurrent = LastDescendant(pSibling);
      return m_pCurrent.Get();
    }
    NodeType* pParent = ParentWithinSubtree(m_pCurrent.Get());
    if (pParent) {
      m_pCurrent = pParent;
      return pParent;
    }
    return nullptr;
  }

  NodeType* MoveToNext() {
    if (!m_pRoot || !m_pCurrent)
      return nullptr;
    NodeType* pChild = TraverseStrategy::GetFirstChild(m_pCurrent.Get());
    if (pChild) {
      m_pCurrent = pChild;
      return pChild;
    }
    return SkipChildrenAndMoveToNext();
  }

  NodeType* SkipChildrenAndMoveToNext() {
    if (!m_pRoot)
      return nullptr;
    NodeType* pNode = m_pCurrent.Get();
    while (pNode) {
      NodeType* pSibling = NextSiblingWithinSubtree(pNode);
      if (pSibling) {
        m_pCurrent = pSibling;
        return pSibling;
      }
      pNode = ParentWithinSubtree(pNode);
    }
    m_pCurrent = nullptr;
    return nullptr;
  }

 private:
  bool RootReachableFromNode(NodeType* pNode) {
    if (!pNode)
      return false;
    if (pNode == m_pRoot)
      return true;
    return RootReachableFromNode(TraverseStrategy::GetParent(pNode));
  }

  NodeType* ParentWithinSubtree(NodeType* pNode) {
    if (!pNode || pNode == m_pRoot)
      return nullptr;
    return TraverseStrategy::GetParent(pNode);
  }

  NodeType* NextSiblingWithinSubtree(NodeType* pNode) {
    if (pNode == m_pRoot)
      return nullptr;
    return TraverseStrategy::GetNextSibling(pNode);
  }

  NodeType* PreviousSiblingWithinSubtree(NodeType* pNode) {
    NodeType* pParent = ParentWithinSubtree(pNode);
    if (!pParent)
      return nullptr;
    NodeType* pCurrent = TraverseStrategy::GetFirstChild(pParent);
    NodeType* pPrevious = nullptr;
    while (pCurrent != pNode) {
      pPrevious = pCurrent;
      pCurrent = TraverseStrategy::GetNextSibling(pCurrent);
    }
    return pPrevious;
  }

  NodeType* LastChild(NodeType* pNode) {
    NodeType* pPrevious = nullptr;
    NodeType* pChild = TraverseStrategy::GetFirstChild(pNode);
    while (pChild) {
      pPrevious = pChild;
      pChild = NextSiblingWithinSubtree(pChild);
    }
    return pPrevious;
  }

  NodeType* LastDescendant(NodeType* pNode) {
    NodeType* pChild = LastChild(pNode);
    return pChild ? LastDescendant(pChild) : pNode;
  }

  UnownedPtr<NodeType> m_pRoot;
  UnownedPtr<NodeType> m_pCurrent;
};

#endif  // XFA_FXFA_PARSER_CXFA_NODEITERATORTEMPLATE_H_

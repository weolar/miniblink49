// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_document_datamerger_imp.h"

#include "xfa/fxfa/parser/cxfa_node.h"

bool XFA_DataMerge_NeedGenerateForm(CXFA_Node* pTemplateChild,
                                    bool bUseInstanceManager) {
  XFA_Element eType = pTemplateChild->GetElementType();
  if (eType == XFA_Element::Variables)
    return true;
  if (pTemplateChild->IsContainerNode())
    return false;
  if (eType == XFA_Element::Proto ||
      (bUseInstanceManager && eType == XFA_Element::Occur)) {
    return false;
  }
  return true;
}


CXFA_Node* XFA_DataMerge_FindFormDOMInstance(CXFA_Document* pDocument,
                                             XFA_Element eType,
                                             uint32_t dwNameHash,
                                             CXFA_Node* pFormParent) {
  CXFA_Node* pFormChild = pFormParent->GetFirstChild();
  for (; pFormChild; pFormChild = pFormChild->GetNextSibling()) {
    if (pFormChild->GetElementType() == eType &&
        pFormChild->GetNameHash() == dwNameHash && pFormChild->IsUnusedNode()) {
      return pFormChild;
    }
  }
  return nullptr;
}

CXFA_Node* XFA_NodeMerge_CloneOrMergeContainer(
    CXFA_Document* pDocument,
    CXFA_Node* pFormParent,
    CXFA_Node* pTemplateNode,
    bool bRecursive,
    std::vector<CXFA_Node*>* pSubformArray) {
  CXFA_Node* pExistingNode = nullptr;
  if (!pSubformArray) {
    pExistingNode = XFA_DataMerge_FindFormDOMInstance(
        pDocument, pTemplateNode->GetElementType(),
        pTemplateNode->GetNameHash(), pFormParent);
  } else if (!pSubformArray->empty()) {
    pExistingNode = pSubformArray->front();
    pSubformArray->erase(pSubformArray->begin());
  }
  if (pExistingNode) {
    if (pSubformArray) {
      pFormParent->InsertChild(pExistingNode, nullptr);
    } else if (pExistingNode->IsContainerNode()) {
      pFormParent->RemoveChild(pExistingNode, true);
      pFormParent->InsertChild(pExistingNode, nullptr);
    }
    pExistingNode->ClearFlag(XFA_NodeFlag_UnusedNode);
    pExistingNode->SetTemplateNode(pTemplateNode);
    if (bRecursive && pExistingNode->GetElementType() != XFA_Element::Items) {
      for (CXFA_Node* pTemplateChild = pTemplateNode->GetFirstChild();
           pTemplateChild; pTemplateChild = pTemplateChild->GetNextSibling()) {
        if (XFA_DataMerge_NeedGenerateForm(pTemplateChild, true)) {
          XFA_NodeMerge_CloneOrMergeContainer(
              pDocument, pExistingNode, pTemplateChild, bRecursive, nullptr);
        }
      }
    }
    pExistingNode->SetFlagAndNotify(XFA_NodeFlag_Initialized);
    return pExistingNode;
  }

  CXFA_Node* pNewNode = pTemplateNode->CloneTemplateToForm(false);
  pFormParent->InsertChild(pNewNode, nullptr);
  if (bRecursive) {
    for (CXFA_Node* pTemplateChild = pTemplateNode->GetFirstChild();
         pTemplateChild; pTemplateChild = pTemplateChild->GetNextSibling()) {
      if (XFA_DataMerge_NeedGenerateForm(pTemplateChild, true)) {
        CXFA_Node* pNewChild = pTemplateChild->CloneTemplateToForm(true);
        pNewNode->InsertChild(pNewChild, nullptr);
      }
    }
  }
  return pNewNode;
}

CXFA_Node* XFA_DataMerge_FindDataScope(CXFA_Node* pParentFormNode) {
  for (CXFA_Node* pRootBoundNode = pParentFormNode;
       pRootBoundNode && pRootBoundNode->IsContainerNode();
       pRootBoundNode = pRootBoundNode->GetParent()) {
    CXFA_Node* pDataScope = pRootBoundNode->GetBindData();
    if (pDataScope)
      return pDataScope;
  }
  return ToNode(
      pParentFormNode->GetDocument()->GetXFAObject(XFA_HASHCODE_Data));
}


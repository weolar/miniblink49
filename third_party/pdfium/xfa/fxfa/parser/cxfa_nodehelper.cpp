// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_nodehelper.h"

#include <utility>

#include "core/fxcrt/fx_extension.h"
#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cjx_object.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_localemgr.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/xfa_basic_data.h"
#include "xfa/fxfa/parser/xfa_resolvenode_rs.h"
#include "xfa/fxfa/parser/xfa_utils.h"

CXFA_NodeHelper::CXFA_NodeHelper() = default;

CXFA_NodeHelper::~CXFA_NodeHelper() = default;

CXFA_Node* CXFA_NodeHelper::GetOneChildNamed(CXFA_Node* parent,
                                             WideStringView wsName) {
  if (!parent)
    return nullptr;

  return FindFirstSiblingNamed(parent, FX_HashCode_GetW(wsName, false));
}

CXFA_Node* CXFA_NodeHelper::GetOneChildOfClass(CXFA_Node* parent,
                                               WideStringView wsClass) {
  if (!parent)
    return nullptr;

  XFA_Element element = XFA_GetElementByName(wsClass);
  if (element == XFA_Element::Unknown)
    return nullptr;

  return FindFirstSiblingOfClass(parent, element);
}

CXFA_Node* CXFA_NodeHelper::FindFirstSiblingNamed(CXFA_Node* parent,
                                                  uint32_t dNameHash) {
  CXFA_Node* result =
      FindFirstSiblingNamedInList(parent, dNameHash, XFA_NODEFILTER_Properties);
  if (result)
    return result;

  return FindFirstSiblingNamedInList(parent, dNameHash,
                                     XFA_NODEFILTER_Children);
}

CXFA_Node* CXFA_NodeHelper::FindFirstSiblingNamedInList(CXFA_Node* parent,
                                                        uint32_t dNameHash,
                                                        uint32_t dwFilter) {
  for (CXFA_Node* child : parent->GetNodeList(dwFilter, XFA_Element::Unknown)) {
    if (child->GetNameHash() == dNameHash)
      return child;

    CXFA_Node* result = FindFirstSiblingNamed(child, dNameHash);
    if (result)
      return result;
  }
  return nullptr;
}

CXFA_Node* CXFA_NodeHelper::FindFirstSiblingOfClass(CXFA_Node* parent,
                                                    XFA_Element element) {
  CXFA_Node* result =
      FindFirstSiblingOfClassInList(parent, element, XFA_NODEFILTER_Properties);
  if (result)
    return result;

  return FindFirstSiblingOfClassInList(parent, element,
                                       XFA_NODEFILTER_Children);
}

CXFA_Node* CXFA_NodeHelper::FindFirstSiblingOfClassInList(CXFA_Node* parent,
                                                          XFA_Element element,
                                                          uint32_t dwFilter) {
  for (CXFA_Node* child : parent->GetNodeList(dwFilter, XFA_Element::Unknown)) {
    if (child->GetElementType() == element)
      return child;

    CXFA_Node* result = FindFirstSiblingOfClass(child, element);
    if (result)
      return result;
  }
  return nullptr;
}

int32_t CXFA_NodeHelper::CountSiblings(CXFA_Node* pNode,
                                       XFA_LOGIC_TYPE eLogicType,
                                       std::vector<CXFA_Node*>* pSiblings,
                                       bool bIsClassName) {
  if (!pNode)
    return 0;
  CXFA_Node* parent = GetParent(pNode, XFA_LOGIC_NoTransparent);
  if (!parent)
    return 0;
  if (!parent->HasProperty(pNode->GetElementType()) &&
      eLogicType == XFA_LOGIC_Transparent) {
    parent = GetParent(pNode, XFA_LOGIC_Transparent);
    if (!parent)
      return 0;
  }
  if (bIsClassName) {
    return TraverseSiblings(parent, pNode->GetClassHashCode(), pSiblings,
                            eLogicType, bIsClassName, true);
  }
  return TraverseSiblings(parent, pNode->GetNameHash(), pSiblings, eLogicType,
                          bIsClassName, true);
}

int32_t CXFA_NodeHelper::TraverseSiblings(CXFA_Node* parent,
                                          uint32_t dNameHash,
                                          std::vector<CXFA_Node*>* pSiblings,
                                          XFA_LOGIC_TYPE eLogicType,
                                          bool bIsClassName,
                                          bool bIsFindProperty) {
  if (!parent || !pSiblings)
    return 0;

  int32_t nCount = 0;
  if (bIsFindProperty) {
    for (CXFA_Node* child :
         parent->GetNodeList(XFA_NODEFILTER_Properties, XFA_Element::Unknown)) {
      if (bIsClassName) {
        if (child->GetClassHashCode() == dNameHash) {
          pSiblings->push_back(child);
          nCount++;
        }
      } else {
        if (child->GetNameHash() == dNameHash) {
          if (child->GetElementType() != XFA_Element::PageSet &&
              child->GetElementType() != XFA_Element::Extras &&
              child->GetElementType() != XFA_Element::Items) {
            pSiblings->push_back(child);
            nCount++;
          }
        }
      }
      if (child->IsUnnamed() &&
          child->GetElementType() == XFA_Element::PageSet) {
        nCount += TraverseSiblings(child, dNameHash, pSiblings, eLogicType,
                                   bIsClassName, false);
      }
    }
    if (nCount > 0)
      return nCount;
  }
  for (CXFA_Node* child :
       parent->GetNodeList(XFA_NODEFILTER_Children, XFA_Element::Unknown)) {
    if (child->GetElementType() == XFA_Element::Variables)
      continue;

    if (bIsClassName) {
      if (child->GetClassHashCode() == dNameHash) {
        pSiblings->push_back(child);
        nCount++;
      }
    } else {
      if (child->GetNameHash() == dNameHash) {
        pSiblings->push_back(child);
        nCount++;
      }
    }
    if (eLogicType == XFA_LOGIC_NoTransparent)
      continue;

    if (NodeIsTransparent(child) &&
        child->GetElementType() != XFA_Element::PageSet) {
      nCount += TraverseSiblings(child, dNameHash, pSiblings, eLogicType,
                                 bIsClassName, false);
    }
  }
  return nCount;
}

CXFA_Node* CXFA_NodeHelper::GetParent(CXFA_Node* pNode,
                                      XFA_LOGIC_TYPE eLogicType) {
  if (!pNode) {
    return nullptr;
  }
  if (eLogicType == XFA_LOGIC_NoTransparent) {
    return pNode->GetParent();
  }
  CXFA_Node* parent;
  CXFA_Node* node = pNode;
  while (true) {
    parent = GetParent(node, XFA_LOGIC_NoTransparent);
    if (!parent) {
      break;
    }
    XFA_Element parentType = parent->GetElementType();
    if ((!parent->IsUnnamed() && parentType != XFA_Element::SubformSet) ||
        parentType == XFA_Element::Variables) {
      break;
    }
    node = parent;
  }
  return parent;
}

int32_t CXFA_NodeHelper::GetIndex(CXFA_Node* pNode,
                                  XFA_LOGIC_TYPE eLogicType,
                                  bool bIsProperty,
                                  bool bIsClassIndex) {
  CXFA_Node* parent = GetParent(pNode, XFA_LOGIC_NoTransparent);
  if (!parent)
    return 0;

  if (!bIsProperty && eLogicType == XFA_LOGIC_Transparent) {
    parent = GetParent(pNode, XFA_LOGIC_Transparent);
    if (!parent)
      return 0;
  }
  uint32_t dwHashName = pNode->GetNameHash();
  if (bIsClassIndex) {
    dwHashName = pNode->GetClassHashCode();
  }
  std::vector<CXFA_Node*> siblings;
  int32_t iSize = TraverseSiblings(parent, dwHashName, &siblings, eLogicType,
                                   bIsClassIndex, true);
  for (int32_t i = 0; i < iSize; ++i) {
    CXFA_Node* child = siblings[i];
    if (child == pNode) {
      return i;
    }
  }
  return 0;
}

WideString CXFA_NodeHelper::GetNameExpression(CXFA_Node* refNode,
                                              bool bIsAllPath) {
  WideString wsName;
  if (bIsAllPath) {
    wsName = GetNameExpression(refNode, false);
    CXFA_Node* parent = GetParent(refNode, XFA_LOGIC_NoTransparent);
    while (parent) {
      WideString wsParent = GetNameExpression(parent, false);
      wsParent += L".";
      wsParent += wsName;
      wsName = std::move(wsParent);
      parent = GetParent(parent, XFA_LOGIC_NoTransparent);
    }
    return wsName;
  }

  WideString ws;
  bool bIsProperty = NodeIsProperty(refNode);
  if (refNode->IsUnnamed() ||
      (bIsProperty && refNode->GetElementType() != XFA_Element::PageSet)) {
    ws = WideString::FromASCII(refNode->GetClassName());
    return WideString::Format(
        L"#%ls[%d]", ws.c_str(),
        GetIndex(refNode, XFA_LOGIC_Transparent, bIsProperty, true));
  }
  ws = refNode->JSObject()->GetCData(XFA_Attribute::Name);
  ws.Replace(L".", L"\\.");
  return WideString::Format(
      L"%ls[%d]", ws.c_str(),
      GetIndex(refNode, XFA_LOGIC_Transparent, bIsProperty, false));
}

bool CXFA_NodeHelper::NodeIsTransparent(CXFA_Node* refNode) {
  if (!refNode)
    return false;

  XFA_Element refNodeType = refNode->GetElementType();
  return (refNode->IsUnnamed() && refNode->IsContainerNode()) ||
         refNodeType == XFA_Element::SubformSet ||
         refNodeType == XFA_Element::Area || refNodeType == XFA_Element::Proto;
}

bool CXFA_NodeHelper::CreateNodeForCondition(const WideString& wsCondition) {
  size_t szLen = wsCondition.GetLength();
  WideString wsIndex(L"0");
  bool bAll = false;
  if (szLen == 0) {
    m_iCreateFlag = XFA_ResolveNode_RSType_CreateNodeOne;
    return false;
  }
  if (wsCondition[0] != '[')
    return false;

  size_t i = 1;
  for (; i < szLen; ++i) {
    wchar_t ch = wsCondition[i];
    if (ch == ' ')
      continue;

    if (ch == '*')
      bAll = true;
    break;
  }
  if (bAll) {
    wsIndex = L"1";
    m_iCreateFlag = XFA_ResolveNode_RSType_CreateNodeAll;
  } else {
    m_iCreateFlag = XFA_ResolveNode_RSType_CreateNodeOne;
    wsIndex = wsCondition.Mid(i, szLen - 1 - i);
  }
  m_iCreateCount = wsIndex.GetInteger();
  return true;
}

bool CXFA_NodeHelper::CreateNode(const WideString& wsName,
                                 const WideString& wsCondition,
                                 bool bLastNode,
                                 CFXJSE_Engine* pScriptContext) {
  ASSERT(!wsName.IsEmpty());

  if (!m_pCreateParent)
    return false;

  WideStringView wsNameView = wsName.AsStringView();
  bool bIsClassName = false;
  bool bResult = false;
  if (wsNameView[0] == '!') {
    wsNameView = wsNameView.Right(wsNameView.GetLength() - 1);
    m_pCreateParent = ToNode(
        pScriptContext->GetDocument()->GetXFAObject(XFA_HASHCODE_Datasets));
  }
  if (wsNameView[0] == '#') {
    bIsClassName = true;
    wsNameView = wsNameView.Right(wsNameView.GetLength() - 1);
  }
  if (m_iCreateCount == 0)
    CreateNodeForCondition(wsCondition);

  if (bIsClassName) {
    XFA_Element eType = XFA_GetElementByName(wsNameView);
    if (eType == XFA_Element::Unknown)
      return false;

    for (int32_t i = 0; i < m_iCreateCount; ++i) {
      CXFA_Node* pNewNode = m_pCreateParent->CreateSamePacketNode(eType);
      if (pNewNode) {
        m_pCreateParent->InsertChild(pNewNode, nullptr);
        if (i == m_iCreateCount - 1) {
          m_pCreateParent = pNewNode;
        }
        bResult = true;
      }
    }
  } else {
    XFA_Element eClassType = XFA_Element::DataGroup;
    if (bLastNode) {
      eClassType = m_eLastCreateType;
    }
    for (int32_t i = 0; i < m_iCreateCount; ++i) {
      CXFA_Node* pNewNode = m_pCreateParent->CreateSamePacketNode(eClassType);
      if (pNewNode) {
        pNewNode->JSObject()->SetAttribute(XFA_Attribute::Name, wsNameView,
                                           false);
        pNewNode->CreateXMLMappingNode();
        m_pCreateParent->InsertChild(pNewNode, nullptr);
        if (i == m_iCreateCount - 1) {
          m_pCreateParent = pNewNode;
        }
        bResult = true;
      }
    }
  }
  if (!bResult)
    m_pCreateParent = nullptr;

  return bResult;
}

void CXFA_NodeHelper::SetCreateNodeType(CXFA_Node* refNode) {
  if (!refNode)
    return;

  if (refNode->GetElementType() == XFA_Element::Subform) {
    m_eLastCreateType = XFA_Element::DataGroup;
  } else if (refNode->GetElementType() == XFA_Element::Field) {
    m_eLastCreateType = XFA_FieldIsMultiListBox(refNode)
                            ? XFA_Element::DataGroup
                            : XFA_Element::DataValue;
  } else if (refNode->GetElementType() == XFA_Element::ExclGroup) {
    m_eLastCreateType = XFA_Element::DataValue;
  }
}

bool CXFA_NodeHelper::NodeIsProperty(CXFA_Node* refNode) {
  if (!refNode)
    return false;

  CXFA_Node* parent = GetParent(refNode, XFA_LOGIC_NoTransparent);
  return parent && parent->HasProperty(refNode->GetElementType());
}

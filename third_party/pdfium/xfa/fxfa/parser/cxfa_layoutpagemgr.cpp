// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_layoutpagemgr.h"

#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cjx_object.h"
#include "third_party/base/stl_util.h"
#include "xfa/fxfa/cxfa_ffnotify.h"
#include "xfa/fxfa/parser/cxfa_containerlayoutitem.h"
#include "xfa/fxfa/parser/cxfa_contentarea.h"
#include "xfa/fxfa/parser/cxfa_contentlayoutitem.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_itemlayoutprocessor.h"
#include "xfa/fxfa/parser/cxfa_layoutprocessor.h"
#include "xfa/fxfa/parser/cxfa_localemgr.h"
#include "xfa/fxfa/parser/cxfa_measurement.h"
#include "xfa/fxfa/parser/cxfa_medium.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/cxfa_nodeiteratortemplate.h"
#include "xfa/fxfa/parser/cxfa_object.h"
#include "xfa/fxfa/parser/cxfa_occur.h"
#include "xfa/fxfa/parser/cxfa_pageset.h"
#include "xfa/fxfa/parser/cxfa_subform.h"
#include "xfa/fxfa/parser/cxfa_traversestrategy_contentareacontainerlayoutitem.h"
#include "xfa/fxfa/parser/cxfa_traversestrategy_layoutitem.h"
#include "xfa/fxfa/parser/cxfa_traversestrategy_xfacontainernode.h"
#include "xfa/fxfa/parser/cxfa_traversestrategy_xfanode.h"
#include "xfa/fxfa/parser/xfa_document_datamerger_imp.h"
#include "xfa/fxfa/parser/xfa_resolvenode_rs.h"

namespace {

class TraverseStrategy_PageSet {
 public:
  static CXFA_ContainerLayoutItem* GetFirstChild(
      CXFA_ContainerLayoutItem* pLayoutItem) {
    if (pLayoutItem->GetFormNode()->GetElementType() != XFA_Element::PageSet)
      return nullptr;

    for (CXFA_LayoutItem* pChildItem = pLayoutItem->m_pFirstChild; pChildItem;
         pChildItem = pChildItem->m_pNextSibling) {
      CXFA_ContainerLayoutItem* pContainer =
          pChildItem->AsContainerLayoutItem();
      if (pContainer &&
          pContainer->GetFormNode()->GetElementType() == XFA_Element::PageSet) {
        return pContainer;
      }
    }
    return nullptr;
  }

  static CXFA_ContainerLayoutItem* GetNextSibling(
      CXFA_ContainerLayoutItem* pLayoutItem) {
    for (CXFA_LayoutItem* pChildItem = pLayoutItem->m_pNextSibling; pChildItem;
         pChildItem = pChildItem->m_pNextSibling) {
      CXFA_ContainerLayoutItem* pContainer =
          pChildItem->AsContainerLayoutItem();
      if (pContainer &&
          pContainer->GetFormNode()->GetElementType() == XFA_Element::PageSet) {
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

using PageSetIterator = CXFA_NodeIteratorTemplate<CXFA_ContainerLayoutItem,
                                                  TraverseStrategy_PageSet>;

uint32_t GetRelevant(CXFA_Node* pFormItem, uint32_t dwParentRelvant) {
  uint32_t dwRelevant = XFA_WidgetStatus_Viewable | XFA_WidgetStatus_Printable;
  WideString wsRelevant =
      pFormItem->JSObject()->GetCData(XFA_Attribute::Relevant);
  if (!wsRelevant.IsEmpty()) {
    if (wsRelevant.EqualsASCII("+print") || wsRelevant.EqualsASCII("print"))
      dwRelevant &= ~XFA_WidgetStatus_Viewable;
    else if (wsRelevant.EqualsASCII("-print"))
      dwRelevant &= ~XFA_WidgetStatus_Printable;
  }
  if (!(dwParentRelvant & XFA_WidgetStatus_Viewable) &&
      (dwRelevant != XFA_WidgetStatus_Viewable)) {
    dwRelevant &= ~XFA_WidgetStatus_Viewable;
  }
  if (!(dwParentRelvant & XFA_WidgetStatus_Printable) &&
      (dwRelevant != XFA_WidgetStatus_Printable)) {
    dwRelevant &= ~XFA_WidgetStatus_Printable;
  }
  return dwRelevant;
}

void SyncContainer(CXFA_FFNotify* pNotify,
                   CXFA_LayoutProcessor* pDocLayout,
                   CXFA_LayoutItem* pContainerItem,
                   uint32_t dwRelevant,
                   bool bVisible,
                   int32_t nPageIndex) {
  bool bVisibleItem = false;
  uint32_t dwStatus = 0;
  uint32_t dwRelevantContainer = 0;
  if (bVisible) {
    XFA_AttributeValue eAttributeValue =
        pContainerItem->GetFormNode()
            ->JSObject()
            ->TryEnum(XFA_Attribute::Presence, true)
            .value_or(XFA_AttributeValue::Visible);
    if (eAttributeValue == XFA_AttributeValue::Visible)
      bVisibleItem = true;

    dwRelevantContainer =
        GetRelevant(pContainerItem->GetFormNode(), dwRelevant);
    dwStatus =
        (bVisibleItem ? XFA_WidgetStatus_Visible : 0) | dwRelevantContainer;
  }
  pNotify->OnLayoutItemAdded(pDocLayout, pContainerItem, nPageIndex, dwStatus);
  for (CXFA_LayoutItem* pChild = pContainerItem->m_pFirstChild; pChild;
       pChild = pChild->m_pNextSibling) {
    if (pChild->IsContentLayoutItem()) {
      SyncContainer(pNotify, pDocLayout, pChild, dwRelevantContainer,
                    bVisibleItem, nPageIndex);
    }
  }
}

void ReorderLayoutItemToTail(CXFA_ContainerLayoutItem* pLayoutItem) {
  CXFA_ContainerLayoutItem* pParentLayoutItem =
      ToContainerLayoutItem(pLayoutItem->m_pParent);
  if (!pParentLayoutItem)
    return;

  pParentLayoutItem->RemoveChild(pLayoutItem);
  pParentLayoutItem->AddChild(pLayoutItem);
}

void RemoveLayoutItem(CXFA_ContainerLayoutItem* pLayoutItem) {
  CXFA_ContainerLayoutItem* pParentLayoutItem =
      ToContainerLayoutItem(pLayoutItem->m_pParent);
  if (!pParentLayoutItem)
    return;

  pParentLayoutItem->RemoveChild(pLayoutItem);
}

CXFA_Node* ResolveBreakTarget(CXFA_Node* pPageSetRoot,
                              bool bNewExprStyle,
                              WideString& wsTargetAll) {
  if (!pPageSetRoot)
    return nullptr;

  CXFA_Document* pDocument = pPageSetRoot->GetDocument();
  if (wsTargetAll.IsEmpty())
    return nullptr;

  wsTargetAll.Trim();
  int32_t iSplitIndex = 0;
  bool bTargetAllFind = true;
  while (iSplitIndex != -1) {
    WideString wsExpr;
    Optional<size_t> iSplitNextIndex = 0;
    if (!bTargetAllFind) {
      iSplitNextIndex = wsTargetAll.Find(' ', iSplitIndex);
      if (!iSplitNextIndex.has_value())
        return nullptr;
      wsExpr =
          wsTargetAll.Mid(iSplitIndex, iSplitNextIndex.value() - iSplitIndex);
    } else {
      wsExpr = wsTargetAll;
    }
    if (wsExpr.IsEmpty())
      return nullptr;

    bTargetAllFind = false;
    if (wsExpr[0] == '#') {
      CXFA_Node* pNode = pDocument->GetNodeByID(
          ToNode(pDocument->GetXFAObject(XFA_HASHCODE_Template)),
          wsExpr.Right(wsExpr.GetLength() - 1).AsStringView());
      if (pNode)
        return pNode;
    } else if (bNewExprStyle) {
      WideString wsProcessedTarget = wsExpr;
      if (wsExpr.Left(4).EqualsASCII("som(") && wsExpr.Last() == L')')
        wsProcessedTarget = wsExpr.Mid(4, wsExpr.GetLength() - 5);

      XFA_RESOLVENODE_RS rs;
      bool iRet = pDocument->GetScriptContext()->ResolveObjects(
          pPageSetRoot, wsProcessedTarget.AsStringView(), &rs,
          XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Properties |
              XFA_RESOLVENODE_Attributes | XFA_RESOLVENODE_Siblings |
              XFA_RESOLVENODE_Parent,
          nullptr);
      if (iRet && rs.objects.front()->IsNode())
        return rs.objects.front()->AsNode();
    }
    iSplitIndex = iSplitNextIndex.value();
  }
  return nullptr;
}

void SetLayoutGeneratedNodeFlag(CXFA_Node* pNode) {
  pNode->SetFlag(XFA_NodeFlag_LayoutGeneratedNode);
  pNode->ClearFlag(XFA_NodeFlag_UnusedNode);
}

bool CheckContentAreaNotUsed(
    CXFA_ContainerLayoutItem* pPageAreaLayoutItem,
    CXFA_Node* pContentArea,
    CXFA_ContainerLayoutItem*& pContentAreaLayoutItem) {
  for (CXFA_LayoutItem* pChild = pPageAreaLayoutItem->m_pFirstChild;
       pChild; pChild = pChild->m_pNextSibling) {
    CXFA_ContainerLayoutItem* pLayoutItem = pChild->AsContainerLayoutItem();
    if (pLayoutItem && pLayoutItem->GetFormNode() == pContentArea) {
      if (!pLayoutItem->m_pFirstChild) {
        pContentAreaLayoutItem = pLayoutItem;
        return true;
      }
      return false;
    }
  }
  return true;
}

void SyncRemoveLayoutItem(CXFA_LayoutItem* pParentLayoutItem,
                          CXFA_FFNotify* pNotify,
                          CXFA_LayoutProcessor* pDocLayout) {
  CXFA_LayoutItem* pNextLayoutItem;
  CXFA_LayoutItem* pCurLayoutItem = pParentLayoutItem->m_pFirstChild;
  while (pCurLayoutItem) {
    pNextLayoutItem = pCurLayoutItem->m_pNextSibling;
    if (pCurLayoutItem->m_pFirstChild)
      SyncRemoveLayoutItem(pCurLayoutItem, pNotify, pDocLayout);

    pNotify->OnLayoutItemRemoving(pDocLayout, pCurLayoutItem);
    delete pCurLayoutItem;
    pCurLayoutItem = pNextLayoutItem;
  }
}

bool RunBreakTestScript(CXFA_Script* pTestScript) {
  WideString wsExpression = pTestScript->JSObject()->GetContent(false);
  if (wsExpression.IsEmpty())
    return true;
  return pTestScript->GetDocument()->GetNotify()->RunScript(
      pTestScript, pTestScript->GetContainerParent());
}

}  // namespace

class CXFA_ContainerRecord {
 public:
  CXFA_ContainerRecord(CXFA_ContainerLayoutItem* pPageSet = nullptr,
                       CXFA_ContainerLayoutItem* pPageArea = nullptr,
                       CXFA_ContainerLayoutItem* pContentArea = nullptr)
      : pCurPageSet(pPageSet),
        pCurPageArea(pPageArea),
        pCurContentArea(pContentArea) {}

  CXFA_ContainerLayoutItem* pCurPageSet;
  CXFA_ContainerLayoutItem* pCurPageArea;
  CXFA_ContainerLayoutItem* pCurContentArea;
};

CXFA_LayoutPageMgr::CXFA_LayoutPageMgr(CXFA_LayoutProcessor* pLayoutProcessor)
    : m_pLayoutProcessor(pLayoutProcessor),
      m_pTemplatePageSetRoot(nullptr),
      m_pPageSetLayoutItemRoot(nullptr),
      m_pPageSetCurRoot(nullptr),
      m_CurrentContainerRecordIter(m_ProposedContainerRecords.end()),
      m_pCurPageArea(nullptr),
      m_nAvailPages(0),
      m_nCurPageCount(0),
      m_ePageSetMode(XFA_AttributeValue::OrderedOccurrence),
      m_bCreateOverFlowPage(false) {}

CXFA_LayoutPageMgr::~CXFA_LayoutPageMgr() {
  ClearData();
  CXFA_LayoutItem* pLayoutItem = GetRootLayoutItem();
  CXFA_LayoutItem* pNextLayout = nullptr;
  for (; pLayoutItem; pLayoutItem = pNextLayout) {
    pNextLayout = pLayoutItem->m_pNextSibling;
    XFA_ReleaseLayoutItem(pLayoutItem);
  }
}

bool CXFA_LayoutPageMgr::InitLayoutPage(CXFA_Node* pFormNode) {
  PrepareLayout();
  CXFA_Node* pTemplateNode = pFormNode->GetTemplateNodeIfExists();
  if (!pTemplateNode)
    return false;

  m_pTemplatePageSetRoot =
      pTemplateNode->JSObject()->GetOrCreateProperty<CXFA_PageSet>(
          0, XFA_Element::PageSet);
  ASSERT(m_pTemplatePageSetRoot);

  if (m_pPageSetLayoutItemRoot) {
    m_pPageSetLayoutItemRoot->m_pParent = nullptr;
    m_pPageSetLayoutItemRoot->m_pFirstChild = nullptr;
    m_pPageSetLayoutItemRoot->m_pNextSibling = nullptr;
    m_pPageSetLayoutItemRoot->SetFormNode(m_pTemplatePageSetRoot);
  } else {
    m_pPageSetLayoutItemRoot =
        new CXFA_ContainerLayoutItem(m_pTemplatePageSetRoot);
  }
  m_pPageSetCurRoot = m_pPageSetLayoutItemRoot;
  m_pTemplatePageSetRoot->JSObject()->SetLayoutItem(m_pPageSetLayoutItemRoot);

  XFA_AttributeValue eRelation =
      m_pTemplatePageSetRoot->JSObject()->GetEnum(XFA_Attribute::Relation);
  if (eRelation != XFA_AttributeValue::Unknown)
    m_ePageSetMode = eRelation;

  InitPageSetMap();
  CXFA_Node* pPageArea = nullptr;
  int32_t iCount = 0;
  for (pPageArea = m_pTemplatePageSetRoot->GetFirstChild(); pPageArea;
       pPageArea = pPageArea->GetNextSibling()) {
    if (pPageArea->GetElementType() == XFA_Element::PageArea) {
      iCount++;
      if (pPageArea->GetFirstChildByClass<CXFA_ContentArea>(
              XFA_Element::ContentArea))
        return true;
    }
  }
  if (iCount > 0)
    return false;

  CXFA_Document* pDocument = pTemplateNode->GetDocument();
  pPageArea = m_pTemplatePageSetRoot->GetChild<CXFA_Node>(
      0, XFA_Element::PageArea, false);
  if (!pPageArea) {
    pPageArea = pDocument->CreateNode(m_pTemplatePageSetRoot->GetPacketType(),
                                      XFA_Element::PageArea);
    if (!pPageArea)
      return false;

    m_pTemplatePageSetRoot->InsertChild(pPageArea, nullptr);
    pPageArea->SetFlagAndNotify(XFA_NodeFlag_Initialized);
  }
  CXFA_ContentArea* pContentArea =
      pPageArea->GetChild<CXFA_ContentArea>(0, XFA_Element::ContentArea, false);
  if (!pContentArea) {
    pContentArea = static_cast<CXFA_ContentArea*>(pDocument->CreateNode(
        pPageArea->GetPacketType(), XFA_Element::ContentArea));
    if (!pContentArea)
      return false;

    pPageArea->InsertChild(pContentArea, nullptr);
    pContentArea->SetFlagAndNotify(XFA_NodeFlag_Initialized);
    pContentArea->JSObject()->SetMeasure(
        XFA_Attribute::X, CXFA_Measurement(0.25f, XFA_Unit::In), false);
    pContentArea->JSObject()->SetMeasure(
        XFA_Attribute::Y, CXFA_Measurement(0.25f, XFA_Unit::In), false);
    pContentArea->JSObject()->SetMeasure(
        XFA_Attribute::W, CXFA_Measurement(8.0f, XFA_Unit::In), false);
    pContentArea->JSObject()->SetMeasure(
        XFA_Attribute::H, CXFA_Measurement(10.5f, XFA_Unit::In), false);
  }
  CXFA_Medium* pMedium =
      pPageArea->GetChild<CXFA_Medium>(0, XFA_Element::Medium, false);
  if (!pMedium) {
    pMedium = static_cast<CXFA_Medium*>(
        pDocument->CreateNode(pPageArea->GetPacketType(), XFA_Element::Medium));
    if (!pContentArea)
      return false;

    pPageArea->InsertChild(pMedium, nullptr);
    pMedium->SetFlagAndNotify(XFA_NodeFlag_Initialized);
    pMedium->JSObject()->SetMeasure(
        XFA_Attribute::Short, CXFA_Measurement(8.5f, XFA_Unit::In), false);
    pMedium->JSObject()->SetMeasure(
        XFA_Attribute::Long, CXFA_Measurement(11.0f, XFA_Unit::In), false);
  }
  return true;
}

bool CXFA_LayoutPageMgr::PrepareFirstPage(CXFA_Node* pRootSubform) {
  bool bProBreakBefore = false;
  CXFA_Node* pBreakBeforeNode = nullptr;
  while (pRootSubform) {
    for (CXFA_Node* pBreakNode = pRootSubform->GetFirstChild(); pBreakNode;
         pBreakNode = pBreakNode->GetNextSibling()) {
      XFA_Element eType = pBreakNode->GetElementType();
      if (eType == XFA_Element::BreakBefore ||
          (eType == XFA_Element::Break &&
           pBreakNode->JSObject()->GetEnum(XFA_Attribute::Before) !=
               XFA_AttributeValue::Auto)) {
        bProBreakBefore = true;
        pBreakBeforeNode = pBreakNode;
        break;
      }
    }
    if (bProBreakBefore)
      break;

    bProBreakBefore = true;
    pRootSubform =
        pRootSubform->GetFirstChildByClass<CXFA_Subform>(XFA_Element::Subform);
    while (pRootSubform && !pRootSubform->PresenceRequiresSpace()) {
      pRootSubform = pRootSubform->GetNextSameClassSibling<CXFA_Subform>(
          XFA_Element::Subform);
    }
  }
  CXFA_Node* pLeader;
  CXFA_Node* pTrailer;
  if (pBreakBeforeNode &&
      ExecuteBreakBeforeOrAfter(pBreakBeforeNode, true, pLeader, pTrailer)) {
    m_CurrentContainerRecordIter = m_ProposedContainerRecords.begin();
    return true;
  }
  return AppendNewPage(true);
}

bool CXFA_LayoutPageMgr::AppendNewPage(bool bFirstTemPage) {
  if (m_CurrentContainerRecordIter != GetTailPosition())
    return true;

  CXFA_Node* pPageNode = GetNextAvailPageArea(nullptr, nullptr, false, false);
  if (!pPageNode)
    return false;

  if (bFirstTemPage &&
      m_CurrentContainerRecordIter == m_ProposedContainerRecords.end()) {
    m_CurrentContainerRecordIter = m_ProposedContainerRecords.begin();
  }
  return !bFirstTemPage ||
         m_CurrentContainerRecordIter != m_ProposedContainerRecords.end();
}

void CXFA_LayoutPageMgr::RemoveLayoutRecord(CXFA_ContainerRecord* pNewRecord,
                                            CXFA_ContainerRecord* pPrevRecord) {
  if (!pNewRecord || !pPrevRecord)
    return;
  if (pNewRecord->pCurPageSet != pPrevRecord->pCurPageSet) {
    RemoveLayoutItem(pNewRecord->pCurPageSet);
    return;
  }
  if (pNewRecord->pCurPageArea != pPrevRecord->pCurPageArea) {
    RemoveLayoutItem(pNewRecord->pCurPageArea);
    return;
  }
  if (pNewRecord->pCurContentArea != pPrevRecord->pCurContentArea) {
    RemoveLayoutItem(pNewRecord->pCurContentArea);
    return;
  }
}

void CXFA_LayoutPageMgr::ReorderPendingLayoutRecordToTail(
    CXFA_ContainerRecord* pNewRecord,
    CXFA_ContainerRecord* pPrevRecord) {
  if (!pNewRecord || !pPrevRecord)
    return;
  if (pNewRecord->pCurPageSet != pPrevRecord->pCurPageSet) {
    ReorderLayoutItemToTail(pNewRecord->pCurPageSet);
    return;
  }
  if (pNewRecord->pCurPageArea != pPrevRecord->pCurPageArea) {
    ReorderLayoutItemToTail(pNewRecord->pCurPageArea);
    return;
  }
  if (pNewRecord->pCurContentArea != pPrevRecord->pCurContentArea) {
    ReorderLayoutItemToTail(pNewRecord->pCurContentArea);
    return;
  }
}

void CXFA_LayoutPageMgr::SubmitContentItem(
    CXFA_ContentLayoutItem* pContentLayoutItem,
    XFA_ItemLayoutProcessorResult eStatus) {
  if (pContentLayoutItem) {
    GetCurrentContainerRecord()->pCurContentArea->AddChild(pContentLayoutItem);
    m_bCreateOverFlowPage = false;
  }

  if (eStatus != XFA_ItemLayoutProcessorResult::Done) {
    if (eStatus == XFA_ItemLayoutProcessorResult::PageFullBreak &&
        m_CurrentContainerRecordIter == GetTailPosition()) {
      AppendNewPage(false);
    }
    m_CurrentContainerRecordIter = GetTailPosition();
    m_pCurPageArea = GetCurrentContainerRecord()->pCurPageArea->GetFormNode();
  }
}

float CXFA_LayoutPageMgr::GetAvailHeight() {
  CXFA_ContainerLayoutItem* pLayoutItem =
      GetCurrentContainerRecord()->pCurContentArea;
  if (!pLayoutItem || !pLayoutItem->GetFormNode())
    return 0.0f;

  float fAvailHeight = pLayoutItem->GetFormNode()
                           ->JSObject()
                           ->GetMeasure(XFA_Attribute::H)
                           .ToUnit(XFA_Unit::Pt);
  if (fAvailHeight >= kXFALayoutPrecision)
    return fAvailHeight;
  if (m_CurrentContainerRecordIter == m_ProposedContainerRecords.begin())
    return 0.0f;
  return FLT_MAX;
}

CXFA_ContainerRecord* CXFA_LayoutPageMgr::CreateContainerRecord(
    CXFA_Node* pPageNode,
    bool bCreateNew) {
  CXFA_ContainerRecord* pNewRecord = new CXFA_ContainerRecord();
  if (m_CurrentContainerRecordIter != m_ProposedContainerRecords.end()) {
    if (!IsPageSetRootOrderedOccurrence() || !pPageNode) {
      *pNewRecord = *GetCurrentContainerRecord();
      m_ProposedContainerRecords.push_back(pNewRecord);
      return pNewRecord;
    }
    CXFA_Node* pPageSet = pPageNode->GetParent();
    if (!bCreateNew) {
      if (pPageSet == m_pTemplatePageSetRoot) {
        pNewRecord->pCurPageSet = m_pPageSetCurRoot;
      } else {
        CXFA_ContainerLayoutItem* pParentLayoutItem =
            ToContainerLayoutItem(pPageSet->JSObject()->GetLayoutItem());
        if (!pParentLayoutItem)
          pParentLayoutItem = m_pPageSetCurRoot;

        pNewRecord->pCurPageSet = pParentLayoutItem;
      }
    } else {
      CXFA_ContainerLayoutItem* pParentPageSetLayout = nullptr;
      if (pPageSet == GetCurrentContainerRecord()->pCurPageSet->GetFormNode()) {
        pParentPageSetLayout = ToContainerLayoutItem(
            GetCurrentContainerRecord()->pCurPageSet->m_pParent);
      } else {
        pParentPageSetLayout = ToContainerLayoutItem(
            pPageSet->GetParent()->JSObject()->GetLayoutItem());
      }
      auto* pPageSetLayoutItem = new CXFA_ContainerLayoutItem(pPageSet);
      pPageSet->JSObject()->SetLayoutItem(pPageSetLayoutItem);
      if (!pParentPageSetLayout) {
        CXFA_ContainerLayoutItem* pPrePageSet = m_pPageSetLayoutItemRoot;
        while (pPrePageSet->m_pNextSibling) {
          pPrePageSet = pPrePageSet->m_pNextSibling->AsContainerLayoutItem();
        }
        pPrePageSet->m_pNextSibling = pPageSetLayoutItem;
        m_pPageSetCurRoot = pPageSetLayoutItem;
      } else {
        pParentPageSetLayout->AddChild(pPageSetLayoutItem);
      }
      pNewRecord->pCurPageSet = pPageSetLayoutItem;
    }
  } else {
    if (pPageNode) {
      CXFA_Node* pPageSet = pPageNode->GetParent();
      if (pPageSet == m_pTemplatePageSetRoot) {
        pNewRecord->pCurPageSet = m_pPageSetLayoutItemRoot;
      } else {
        CXFA_ContainerLayoutItem* pPageSetLayoutItem =
            new CXFA_ContainerLayoutItem(pPageSet);
        pPageSet->JSObject()->SetLayoutItem(pPageSetLayoutItem);
        m_pPageSetLayoutItemRoot->AddChild(pPageSetLayoutItem);
        pNewRecord->pCurPageSet = pPageSetLayoutItem;
      }
    } else {
      pNewRecord->pCurPageSet = m_pPageSetLayoutItemRoot;
    }
  }
  m_ProposedContainerRecords.push_back(pNewRecord);
  return pNewRecord;
}

void CXFA_LayoutPageMgr::AddPageAreaLayoutItem(CXFA_ContainerRecord* pNewRecord,
                                               CXFA_Node* pNewPageArea) {
  CXFA_ContainerLayoutItem* pNewPageAreaLayoutItem = nullptr;
  if (pdfium::IndexInBounds(m_PageArray, m_nAvailPages)) {
    CXFA_ContainerLayoutItem* pContainerItem = m_PageArray[m_nAvailPages];
    pContainerItem->SetFormNode(pNewPageArea);
    m_nAvailPages++;
    pNewPageAreaLayoutItem = pContainerItem;
  } else {
    CXFA_FFNotify* pNotify = pNewPageArea->GetDocument()->GetNotify();
    auto* pContainerItem =
        pNotify->OnCreateContainerLayoutItem(pNewPageArea).release();
    m_PageArray.push_back(pContainerItem);
    m_nAvailPages++;
    pNotify->OnPageEvent(pContainerItem, XFA_PAGEVIEWEVENT_PostRemoved);
    pNewPageAreaLayoutItem = pContainerItem;
  }
  pNewRecord->pCurPageSet->AddChild(pNewPageAreaLayoutItem);
  pNewRecord->pCurPageArea = pNewPageAreaLayoutItem;
  pNewRecord->pCurContentArea = nullptr;
}

void CXFA_LayoutPageMgr::AddContentAreaLayoutItem(
    CXFA_ContainerRecord* pNewRecord,
    CXFA_Node* pContentArea) {
  if (!pContentArea) {
    pNewRecord->pCurContentArea = nullptr;
    return;
  }
  CXFA_ContainerLayoutItem* pNewContentAreaLayoutItem =
      new CXFA_ContainerLayoutItem(pContentArea);
  ASSERT(pNewRecord->pCurPageArea);
  pNewRecord->pCurPageArea->AddChild(pNewContentAreaLayoutItem);
  pNewRecord->pCurContentArea = pNewContentAreaLayoutItem;
}

void CXFA_LayoutPageMgr::FinishPaginatedPageSets() {
  for (CXFA_ContainerLayoutItem* pRootPageSetLayoutItem =
           m_pPageSetLayoutItemRoot;
       pRootPageSetLayoutItem;
       pRootPageSetLayoutItem =
           ToContainerLayoutItem(pRootPageSetLayoutItem->m_pNextSibling)) {
    PageSetIterator sIterator(pRootPageSetLayoutItem);
    for (CXFA_ContainerLayoutItem* pPageSetLayoutItem = sIterator.GetCurrent();
         pPageSetLayoutItem; pPageSetLayoutItem = sIterator.MoveToNext()) {
      XFA_AttributeValue ePageRelation =
          pPageSetLayoutItem->GetFormNode()->JSObject()->GetEnum(
              XFA_Attribute::Relation);
      switch (ePageRelation) {
        case XFA_AttributeValue::OrderedOccurrence:
        default: { ProcessLastPageSet(); } break;
        case XFA_AttributeValue::SimplexPaginated:
        case XFA_AttributeValue::DuplexPaginated: {
          CXFA_LayoutItem* pLastPageAreaLayoutItem = nullptr;
          int32_t nPageAreaCount = 0;
          for (CXFA_LayoutItem* pPageAreaLayoutItem =
                   pPageSetLayoutItem->m_pFirstChild;
               pPageAreaLayoutItem;
               pPageAreaLayoutItem = pPageAreaLayoutItem->m_pNextSibling) {
            if (pPageAreaLayoutItem->GetFormNode()->GetElementType() !=
                XFA_Element::PageArea) {
              continue;
            }
            nPageAreaCount++;
            pLastPageAreaLayoutItem = pPageAreaLayoutItem;
          }
          if (!pLastPageAreaLayoutItem)
            break;

          if (!FindPageAreaFromPageSet_SimplexDuplex(
                  pPageSetLayoutItem->GetFormNode(), nullptr, nullptr, nullptr,
                  true, true,
                  nPageAreaCount == 1 ? XFA_AttributeValue::Only
                                      : XFA_AttributeValue::Last) &&
              (nPageAreaCount == 1 &&
               !FindPageAreaFromPageSet_SimplexDuplex(
                   pPageSetLayoutItem->GetFormNode(), nullptr, nullptr, nullptr,
                   true, true, XFA_AttributeValue::Last))) {
            break;
          }
          CXFA_Node* pNode = m_pCurPageArea;
          XFA_AttributeValue eCurChoice =
              pNode->JSObject()->GetEnum(XFA_Attribute::PagePosition);
          if (eCurChoice == XFA_AttributeValue::Last) {
            XFA_AttributeValue eOddOrEven =
                pNode->JSObject()->GetEnum(XFA_Attribute::OddOrEven);
            XFA_AttributeValue eLastChoice =
                pLastPageAreaLayoutItem->GetFormNode()->JSObject()->GetEnum(
                    XFA_Attribute::PagePosition);
            if (eLastChoice == XFA_AttributeValue::First &&
                (ePageRelation == XFA_AttributeValue::SimplexPaginated ||
                 eOddOrEven != XFA_AttributeValue::Odd)) {
              CXFA_ContainerRecord* pRecord =
                  CreateContainerRecord(nullptr, false);
              AddPageAreaLayoutItem(pRecord, pNode);
              break;
            }
          }
          bool bUsable = true;
          std::vector<float> rgUsedHeights;
          for (CXFA_LayoutItem* pChildLayoutItem =
                   pLastPageAreaLayoutItem->m_pFirstChild;
               pChildLayoutItem;
               pChildLayoutItem = pChildLayoutItem->m_pNextSibling) {
            if (pChildLayoutItem->GetFormNode()->GetElementType() !=
                XFA_Element::ContentArea) {
              continue;
            }
            float fUsedHeight = 0;
            for (CXFA_LayoutItem* pContentChildLayoutItem =
                     pChildLayoutItem->m_pFirstChild;
                 pContentChildLayoutItem;
                 pContentChildLayoutItem =
                     pContentChildLayoutItem->m_pNextSibling) {
              if (CXFA_ContentLayoutItem* pContent =
                      pContentChildLayoutItem->AsContentLayoutItem()) {
                fUsedHeight += pContent->m_sSize.height;
              }
            }
            rgUsedHeights.push_back(fUsedHeight);
          }
          int32_t iCurContentAreaIndex = -1;
          for (CXFA_Node* pContentAreaNode = pNode->GetFirstChild();
               pContentAreaNode;
               pContentAreaNode = pContentAreaNode->GetNextSibling()) {
            if (pContentAreaNode->GetElementType() !=
                XFA_Element::ContentArea) {
              continue;
            }
            iCurContentAreaIndex++;
            const float fHeight = pContentAreaNode->JSObject()
                                      ->GetMeasure(XFA_Attribute::H)
                                      .ToUnit(XFA_Unit::Pt) +
                                  kXFALayoutPrecision;
            if (rgUsedHeights[iCurContentAreaIndex] > fHeight) {
              bUsable = false;
              break;
            }
          }
          if (bUsable) {
            CXFA_LayoutItem* pChildLayoutItem =
                pLastPageAreaLayoutItem->m_pFirstChild;
            CXFA_Node* pContentAreaNode = pNode->GetFirstChild();
            pLastPageAreaLayoutItem->SetFormNode(pNode);
            while (pChildLayoutItem && pContentAreaNode) {
              if (pChildLayoutItem->GetFormNode()->GetElementType() !=
                  XFA_Element::ContentArea) {
                pChildLayoutItem = pChildLayoutItem->m_pNextSibling;
                continue;
              }
              if (pContentAreaNode->GetElementType() !=
                  XFA_Element::ContentArea) {
                pContentAreaNode = pContentAreaNode->GetNextSibling();
                continue;
              }
              pChildLayoutItem->SetFormNode(pContentAreaNode);
              pChildLayoutItem = pChildLayoutItem->m_pNextSibling;
              pContentAreaNode = pContentAreaNode->GetNextSibling();
            }
          } else if (pNode->JSObject()->GetEnum(XFA_Attribute::PagePosition) ==
                     XFA_AttributeValue::Last) {
            CXFA_ContainerRecord* pRecord =
                CreateContainerRecord(nullptr, false);
            AddPageAreaLayoutItem(pRecord, pNode);
          }
        } break;
      }
    }
  }
}

int32_t CXFA_LayoutPageMgr::GetPageCount() const {
  return pdfium::CollectionSize<int32_t>(m_PageArray);
}

CXFA_ContainerLayoutItem* CXFA_LayoutPageMgr::GetPage(int32_t index) const {
  if (!pdfium::IndexInBounds(m_PageArray, index))
    return nullptr;
  return m_PageArray[index];
}

int32_t CXFA_LayoutPageMgr::GetPageIndex(
    const CXFA_ContainerLayoutItem* pPage) const {
  auto it = std::find(m_PageArray.begin(), m_PageArray.end(), pPage);
  return it != m_PageArray.end() ? it - m_PageArray.begin() : -1;
}

bool CXFA_LayoutPageMgr::RunBreak(XFA_Element eBreakType,
                                  XFA_AttributeValue eTargetType,
                                  CXFA_Node* pTarget,
                                  bool bStartNew) {
  bool bRet = false;
  switch (eTargetType) {
    case XFA_AttributeValue::ContentArea:
      if (pTarget && pTarget->GetElementType() != XFA_Element::ContentArea)
        pTarget = nullptr;
      if (!pTarget ||
          m_CurrentContainerRecordIter == m_ProposedContainerRecords.end() ||
          pTarget !=
              GetCurrentContainerRecord()->pCurContentArea->GetFormNode() ||
          bStartNew) {
        CXFA_Node* pPageArea = nullptr;
        if (pTarget)
          pPageArea = pTarget->GetParent();

        pPageArea = GetNextAvailPageArea(pPageArea, pTarget, false, false);
        bRet = !!pPageArea;
      }
      break;
    case XFA_AttributeValue::PageArea:
      if (pTarget && pTarget->GetElementType() != XFA_Element::PageArea)
        pTarget = nullptr;
      if (!pTarget ||
          m_CurrentContainerRecordIter == m_ProposedContainerRecords.end() ||
          pTarget != GetCurrentContainerRecord()->pCurPageArea->GetFormNode() ||
          bStartNew) {
        CXFA_Node* pPageArea =
            GetNextAvailPageArea(pTarget, nullptr, true, false);
        bRet = !!pPageArea;
      }
      break;
    case XFA_AttributeValue::PageOdd:
      if (pTarget && pTarget->GetElementType() != XFA_Element::PageArea)
        pTarget = nullptr;
      break;
    case XFA_AttributeValue::PageEven:
      if (pTarget && pTarget->GetElementType() != XFA_Element::PageArea)
        pTarget = nullptr;
      break;
    case XFA_AttributeValue::Auto:
    default:
      break;
  }
  return bRet;
}

bool CXFA_LayoutPageMgr::ExecuteBreakBeforeOrAfter(
    CXFA_Node* pCurNode,
    bool bBefore,
    CXFA_Node*& pBreakLeaderTemplate,
    CXFA_Node*& pBreakTrailerTemplate) {
  XFA_Element eType = pCurNode->GetElementType();
  switch (eType) {
    case XFA_Element::BreakBefore:
    case XFA_Element::BreakAfter: {
      WideString wsBreakLeader;
      WideString wsBreakTrailer;
      CXFA_Node* pFormNode = pCurNode->GetContainerParent();
      CXFA_Node* pContainer = pFormNode->GetTemplateNodeIfExists();
      bool bStartNew =
          pCurNode->JSObject()->GetInteger(XFA_Attribute::StartNew) != 0;
      CXFA_Script* pScript =
          pCurNode->GetFirstChildByClass<CXFA_Script>(XFA_Element::Script);
      if (pScript && !RunBreakTestScript(pScript))
        return false;

      WideString wsTarget =
          pCurNode->JSObject()->GetCData(XFA_Attribute::Target);
      CXFA_Node* pTarget =
          ResolveBreakTarget(m_pTemplatePageSetRoot, true, wsTarget);
      wsBreakTrailer = pCurNode->JSObject()->GetCData(XFA_Attribute::Trailer);
      wsBreakLeader = pCurNode->JSObject()->GetCData(XFA_Attribute::Leader);
      pBreakLeaderTemplate =
          ResolveBreakTarget(pContainer, true, wsBreakLeader);
      pBreakTrailerTemplate =
          ResolveBreakTarget(pContainer, true, wsBreakTrailer);
      if (RunBreak(eType,
                   pCurNode->JSObject()->GetEnum(XFA_Attribute::TargetType),
                   pTarget, bStartNew)) {
        return true;
      }
      if (!m_ProposedContainerRecords.empty() &&
          m_CurrentContainerRecordIter == m_ProposedContainerRecords.begin() &&
          eType == XFA_Element::BreakBefore) {
        CXFA_Node* pParentNode = pFormNode->GetContainerParent();
        if (!pParentNode ||
            pFormNode != pParentNode->GetFirstContainerChild()) {
          break;
        }
        pParentNode = pParentNode->GetParent();
        if (!pParentNode ||
            pParentNode->GetElementType() != XFA_Element::Form) {
          break;
        }
        return true;
      }
      break;
    }
    case XFA_Element::Break: {
      bool bStartNew =
          pCurNode->JSObject()->GetInteger(XFA_Attribute::StartNew) != 0;
      WideString wsTarget = pCurNode->JSObject()->GetCData(
          bBefore ? XFA_Attribute::BeforeTarget : XFA_Attribute::AfterTarget);
      CXFA_Node* pTarget =
          ResolveBreakTarget(m_pTemplatePageSetRoot, true, wsTarget);
      if (RunBreak(bBefore ? XFA_Element::BreakBefore : XFA_Element::BreakAfter,
                   pCurNode->JSObject()->GetEnum(
                       bBefore ? XFA_Attribute::Before : XFA_Attribute::After),
                   pTarget, bStartNew)) {
        return true;
      }
      break;
    }
    default:
      break;
  }
  return false;
}

bool CXFA_LayoutPageMgr::ProcessBreakBeforeOrAfter(
    CXFA_Node* pBreakNode,
    bool bBefore,
    CXFA_Node*& pBreakLeaderNode,
    CXFA_Node*& pBreakTrailerNode,
    bool& bCreatePage) {
  CXFA_Node* pLeaderTemplate = nullptr;
  CXFA_Node* pTrailerTemplate = nullptr;
  CXFA_Node* pFormNode = pBreakNode->GetContainerParent();
  if (pFormNode->PresenceRequiresSpace()) {
    bCreatePage = ExecuteBreakBeforeOrAfter(pBreakNode, bBefore,
                                            pLeaderTemplate, pTrailerTemplate);
    CXFA_Document* pDocument = pBreakNode->GetDocument();
    CXFA_Node* pDataScope = nullptr;
    pFormNode = pFormNode->GetContainerParent();
    if (pLeaderTemplate) {
      if (!pDataScope)
        pDataScope = XFA_DataMerge_FindDataScope(pFormNode);

      pBreakLeaderNode = pDocument->DataMerge_CopyContainer(
          pLeaderTemplate, pFormNode, pDataScope, true, true, true);
      pDocument->DataMerge_UpdateBindingRelations(pBreakLeaderNode);
      SetLayoutGeneratedNodeFlag(pBreakLeaderNode);
    }
    if (pTrailerTemplate) {
      if (!pDataScope)
        pDataScope = XFA_DataMerge_FindDataScope(pFormNode);

      pBreakTrailerNode = pDocument->DataMerge_CopyContainer(
          pTrailerTemplate, pFormNode, pDataScope, true, true, true);
      pDocument->DataMerge_UpdateBindingRelations(pBreakTrailerNode);
      SetLayoutGeneratedNodeFlag(pBreakTrailerNode);
    }
    return true;
  }
  return false;
}

bool CXFA_LayoutPageMgr::ProcessBookendLeaderOrTrailer(
    CXFA_Node* pBookendNode,
    bool bLeader,
    CXFA_Node*& pBookendAppendNode) {
  CXFA_Node* pLeaderTemplate = nullptr;
  CXFA_Node* pFormNode = pBookendNode->GetContainerParent();
  if (ResolveBookendLeaderOrTrailer(pBookendNode, bLeader, pLeaderTemplate)) {
    CXFA_Document* pDocument = pBookendNode->GetDocument();
    CXFA_Node* pDataScope = nullptr;
    if (pLeaderTemplate) {
      if (!pDataScope)
        pDataScope = XFA_DataMerge_FindDataScope(pFormNode);

      pBookendAppendNode = pDocument->DataMerge_CopyContainer(
          pLeaderTemplate, pFormNode, pDataScope, true, true, true);
      pDocument->DataMerge_UpdateBindingRelations(pBookendAppendNode);
      SetLayoutGeneratedNodeFlag(pBookendAppendNode);
      return true;
    }
  }
  return false;
}

CXFA_Node* CXFA_LayoutPageMgr::BreakOverflow(CXFA_Node* pOverflowNode,
                                             CXFA_Node*& pLeaderTemplate,
                                             CXFA_Node*& pTrailerTemplate,
                                             bool bCreatePage) {
  CXFA_Node* pContainer =
      pOverflowNode->GetContainerParent()->GetTemplateNodeIfExists();
  if (pOverflowNode->GetElementType() == XFA_Element::Break) {
    WideString wsOverflowLeader =
        pOverflowNode->JSObject()->GetCData(XFA_Attribute::OverflowLeader);
    WideString wsOverflowTarget =
        pOverflowNode->JSObject()->GetCData(XFA_Attribute::OverflowTarget);
    WideString wsOverflowTrailer =
        pOverflowNode->JSObject()->GetCData(XFA_Attribute::OverflowTrailer);
    if (wsOverflowTarget.IsEmpty() && wsOverflowLeader.IsEmpty() &&
        wsOverflowTrailer.IsEmpty()) {
      return nullptr;
    }

    if (!wsOverflowTarget.IsEmpty() && bCreatePage && !m_bCreateOverFlowPage) {
      CXFA_Node* pTarget =
          ResolveBreakTarget(m_pTemplatePageSetRoot, true, wsOverflowTarget);
      if (pTarget) {
        m_bCreateOverFlowPage = true;
        switch (pTarget->GetElementType()) {
          case XFA_Element::PageArea:
            RunBreak(XFA_Element::Overflow, XFA_AttributeValue::PageArea,
                     pTarget, true);
            break;
          case XFA_Element::ContentArea:
            RunBreak(XFA_Element::Overflow, XFA_AttributeValue::ContentArea,
                     pTarget, true);
            break;
          default:
            break;
        }
      }
    }
    if (!bCreatePage) {
      pLeaderTemplate = ResolveBreakTarget(pContainer, true, wsOverflowLeader);
      pTrailerTemplate =
          ResolveBreakTarget(pContainer, true, wsOverflowTrailer);
    }
    return pOverflowNode;
  }

  if (pOverflowNode->GetElementType() != XFA_Element::Overflow)
    return nullptr;

  WideString wsOverflowTarget =
      pOverflowNode->JSObject()->GetCData(XFA_Attribute::Target);
  if (!wsOverflowTarget.IsEmpty() && bCreatePage && !m_bCreateOverFlowPage) {
    CXFA_Node* pTarget =
        ResolveBreakTarget(m_pTemplatePageSetRoot, true, wsOverflowTarget);
    if (pTarget) {
      m_bCreateOverFlowPage = true;
      switch (pTarget->GetElementType()) {
        case XFA_Element::PageArea:
          RunBreak(XFA_Element::Overflow, XFA_AttributeValue::PageArea, pTarget,
                   true);
          break;
        case XFA_Element::ContentArea:
          RunBreak(XFA_Element::Overflow, XFA_AttributeValue::ContentArea,
                   pTarget, true);
          break;
        default:
          break;
      }
    }
  }
  if (!bCreatePage) {
    WideString wsLeader =
        pOverflowNode->JSObject()->GetCData(XFA_Attribute::Leader);
    WideString wsTrailer =
        pOverflowNode->JSObject()->GetCData(XFA_Attribute::Trailer);
    pLeaderTemplate = ResolveBreakTarget(pContainer, true, wsLeader);
    pTrailerTemplate = ResolveBreakTarget(pContainer, true, wsTrailer);
  }
  return pOverflowNode;
}

bool CXFA_LayoutPageMgr::ProcessOverflow(CXFA_Node* pFormNode,
                                         CXFA_Node*& pLeaderNode,
                                         CXFA_Node*& pTrailerNode,
                                         bool bDataMerge,
                                         bool bCreatePage) {
  if (!pFormNode)
    return false;

  CXFA_Node* pLeaderTemplate = nullptr;
  CXFA_Node* pTrailerTemplate = nullptr;
  bool bIsOverflowNode = false;
  if (pFormNode->GetElementType() == XFA_Element::Overflow ||
      pFormNode->GetElementType() == XFA_Element::Break) {
    bIsOverflowNode = true;
  }
  for (CXFA_Node* pCurNode = bIsOverflowNode ? pFormNode
                                             : pFormNode->GetFirstChild();
       pCurNode; pCurNode = pCurNode->GetNextSibling()) {
    if (BreakOverflow(pCurNode, pLeaderTemplate, pTrailerTemplate,
                      bCreatePage)) {
      if (bIsOverflowNode)
        pFormNode = pCurNode->GetParent();

      CXFA_Document* pDocument = pCurNode->GetDocument();
      CXFA_Node* pDataScope = nullptr;
      if (pLeaderTemplate) {
        if (!pDataScope)
          pDataScope = XFA_DataMerge_FindDataScope(pFormNode);

        pLeaderNode = pDocument->DataMerge_CopyContainer(
            pLeaderTemplate, pFormNode, pDataScope, true, true, true);
        pDocument->DataMerge_UpdateBindingRelations(pLeaderNode);
        SetLayoutGeneratedNodeFlag(pLeaderNode);
      }
      if (pTrailerTemplate) {
        if (!pDataScope)
          pDataScope = XFA_DataMerge_FindDataScope(pFormNode);

        pTrailerNode = pDocument->DataMerge_CopyContainer(
            pTrailerTemplate, pFormNode, pDataScope, true, true, true);
        pDocument->DataMerge_UpdateBindingRelations(pTrailerNode);
        SetLayoutGeneratedNodeFlag(pTrailerNode);
      }
      return true;
    }
    if (bIsOverflowNode) {
      break;
    }
  }
  return false;
}

bool CXFA_LayoutPageMgr::ResolveBookendLeaderOrTrailer(
    CXFA_Node* pBookendNode,
    bool bLeader,
    CXFA_Node*& pBookendAppendTemplate) {
  CXFA_Node* pContainer =
      pBookendNode->GetContainerParent()->GetTemplateNodeIfExists();
  if (pBookendNode->GetElementType() == XFA_Element::Break) {
    WideString leader = pBookendNode->JSObject()->GetCData(
        bLeader ? XFA_Attribute::BookendLeader : XFA_Attribute::BookendTrailer);
    if (!leader.IsEmpty()) {
      pBookendAppendTemplate = ResolveBreakTarget(pContainer, false, leader);
      return true;
    }
    return false;
  }

  if (pBookendNode->GetElementType() == XFA_Element::Bookend) {
    WideString leader = pBookendNode->JSObject()->GetCData(
        bLeader ? XFA_Attribute::Leader : XFA_Attribute::Trailer);
    pBookendAppendTemplate = ResolveBreakTarget(pContainer, true, leader);
    return true;
  }
  return false;
}

bool CXFA_LayoutPageMgr::FindPageAreaFromPageSet(CXFA_Node* pPageSet,
                                                 CXFA_Node* pStartChild,
                                                 CXFA_Node* pTargetPageArea,
                                                 CXFA_Node* pTargetContentArea,
                                                 bool bNewPage,
                                                 bool bQuery) {
  if (!pPageSet && !pStartChild)
    return false;

  if (IsPageSetRootOrderedOccurrence()) {
    return FindPageAreaFromPageSet_Ordered(pPageSet, pStartChild,
                                           pTargetPageArea, pTargetContentArea,
                                           bNewPage, bQuery);
  }
  XFA_AttributeValue ePreferredPosition =
      m_CurrentContainerRecordIter != m_ProposedContainerRecords.end()
          ? XFA_AttributeValue::Rest
          : XFA_AttributeValue::First;
  return FindPageAreaFromPageSet_SimplexDuplex(
      pPageSet, pStartChild, pTargetPageArea, pTargetContentArea, bNewPage,
      bQuery, ePreferredPosition);
}

bool CXFA_LayoutPageMgr::FindPageAreaFromPageSet_Ordered(
    CXFA_Node* pPageSet,
    CXFA_Node* pStartChild,
    CXFA_Node* pTargetPageArea,
    CXFA_Node* pTargetContentArea,
    bool bNewPage,
    bool bQuery) {
  int32_t iPageSetCount = 0;
  if (!pStartChild && !bQuery) {
    auto it = m_pPageSetMap.find(pPageSet);
    if (it != m_pPageSetMap.end())
      iPageSetCount = it->second;
    int32_t iMax = -1;
    CXFA_Node* pOccurNode =
        pPageSet->GetFirstChildByClass<CXFA_Occur>(XFA_Element::Occur);
    if (pOccurNode) {
      Optional<int32_t> ret =
          pOccurNode->JSObject()->TryInteger(XFA_Attribute::Max, false);
      if (ret)
        iMax = *ret;
    }
    if (iMax >= 0 && iMax <= iPageSetCount)
      return false;
  }

  bool bRes = false;
  CXFA_Node* pCurrentNode =
      pStartChild ? pStartChild->GetNextSibling() : pPageSet->GetFirstChild();
  for (; pCurrentNode; pCurrentNode = pCurrentNode->GetNextSibling()) {
    if (pCurrentNode->GetElementType() == XFA_Element::PageArea) {
      if ((pTargetPageArea == pCurrentNode || !pTargetPageArea)) {
        if (!pCurrentNode->GetFirstChildByClass<CXFA_ContentArea>(
                XFA_Element::ContentArea)) {
          if (pTargetPageArea == pCurrentNode) {
            CreateMinPageRecord(pCurrentNode, true, false);
            pTargetPageArea = nullptr;
          }
          continue;
        }
        if (!bQuery) {
          CXFA_ContainerRecord* pNewRecord =
              CreateContainerRecord(pCurrentNode, !pStartChild);
          AddPageAreaLayoutItem(pNewRecord, pCurrentNode);
          if (!pTargetContentArea) {
            pTargetContentArea =
                pCurrentNode->GetFirstChildByClass<CXFA_ContentArea>(
                    XFA_Element::ContentArea);
          }
          AddContentAreaLayoutItem(pNewRecord, pTargetContentArea);
        }
        m_pCurPageArea = pCurrentNode;
        m_nCurPageCount = 1;
        bRes = true;
        break;
      }
      if (!bQuery)
        CreateMinPageRecord(pCurrentNode, false, false);
    } else if (pCurrentNode->GetElementType() == XFA_Element::PageSet) {
      if (FindPageAreaFromPageSet_Ordered(pCurrentNode, nullptr,
                                          pTargetPageArea, pTargetContentArea,
                                          bNewPage, bQuery)) {
        bRes = true;
        break;
      }
      if (!bQuery)
        CreateMinPageSetRecord(pCurrentNode, true);
    }
  }
  if (!pStartChild && bRes && !bQuery)
    m_pPageSetMap[pPageSet] = ++iPageSetCount;
  return bRes;
}

bool CXFA_LayoutPageMgr::FindPageAreaFromPageSet_SimplexDuplex(
    CXFA_Node* pPageSet,
    CXFA_Node* pStartChild,
    CXFA_Node* pTargetPageArea,
    CXFA_Node* pTargetContentArea,
    bool bNewPage,
    bool bQuery,
    XFA_AttributeValue ePreferredPosition) {
  const XFA_AttributeValue eFallbackPosition = XFA_AttributeValue::Any;
  CXFA_Node* pPreferredPageArea = nullptr;
  CXFA_Node* pFallbackPageArea = nullptr;
  CXFA_Node* pCurrentNode = nullptr;
  if (!pStartChild || pStartChild->GetElementType() == XFA_Element::PageArea)
    pCurrentNode = pPageSet->GetFirstChild();
  else
    pCurrentNode = pStartChild->GetNextSibling();

  for (; pCurrentNode; pCurrentNode = pCurrentNode->GetNextSibling()) {
    if (pCurrentNode->GetElementType() == XFA_Element::PageArea) {
      if (!MatchPageAreaOddOrEven(pCurrentNode))
        continue;

      XFA_AttributeValue eCurPagePosition =
          pCurrentNode->JSObject()->GetEnum(XFA_Attribute::PagePosition);
      if (ePreferredPosition == XFA_AttributeValue::Last) {
        if (eCurPagePosition != ePreferredPosition)
          continue;
        if (m_ePageSetMode == XFA_AttributeValue::SimplexPaginated ||
            pCurrentNode->JSObject()->GetEnum(XFA_Attribute::OddOrEven) ==
                XFA_AttributeValue::Any) {
          pPreferredPageArea = pCurrentNode;
          break;
        }
        CXFA_ContainerRecord* pNewRecord =
            CreateContainerRecord(nullptr, false);
        AddPageAreaLayoutItem(pNewRecord, pCurrentNode);
        AddContentAreaLayoutItem(
            pNewRecord, pCurrentNode->GetFirstChildByClass<CXFA_ContentArea>(
                            XFA_Element::ContentArea));
        pPreferredPageArea = pCurrentNode;
        return false;
      }
      if (ePreferredPosition == XFA_AttributeValue::Only) {
        if (eCurPagePosition != ePreferredPosition)
          continue;
        if (m_ePageSetMode != XFA_AttributeValue::DuplexPaginated ||
            pCurrentNode->JSObject()->GetEnum(XFA_Attribute::OddOrEven) ==
                XFA_AttributeValue::Any) {
          pPreferredPageArea = pCurrentNode;
          break;
        }
        return false;
      }
      if ((pTargetPageArea == pCurrentNode || !pTargetPageArea)) {
        if (!pCurrentNode->GetFirstChildByClass<CXFA_ContentArea>(
                XFA_Element::ContentArea)) {
          if (pTargetPageArea == pCurrentNode) {
            CXFA_ContainerRecord* pNewRecord =
                CreateContainerRecord(nullptr, false);
            AddPageAreaLayoutItem(pNewRecord, pCurrentNode);
            pTargetPageArea = nullptr;
          }
          continue;
        }
        if ((ePreferredPosition == XFA_AttributeValue::Rest &&
             eCurPagePosition == XFA_AttributeValue::Any) ||
            eCurPagePosition == ePreferredPosition) {
          pPreferredPageArea = pCurrentNode;
          break;
        }
        if (eCurPagePosition == eFallbackPosition && !pFallbackPageArea) {
          pFallbackPageArea = pCurrentNode;
        }
      } else if (pTargetPageArea && !MatchPageAreaOddOrEven(pTargetPageArea)) {
        CXFA_ContainerRecord* pNewRecord =
            CreateContainerRecord(nullptr, false);
        AddPageAreaLayoutItem(pNewRecord, pCurrentNode);
        AddContentAreaLayoutItem(
            pNewRecord, pCurrentNode->GetFirstChildByClass<CXFA_ContentArea>(
                            XFA_Element::ContentArea));
      }
    } else if (pCurrentNode->GetElementType() == XFA_Element::PageSet) {
      if (FindPageAreaFromPageSet_SimplexDuplex(
              pCurrentNode, nullptr, pTargetPageArea, pTargetContentArea,
              bNewPage, bQuery, ePreferredPosition)) {
        break;
      }
    }
  }

  CXFA_Node* pCurPageArea = nullptr;
  if (pPreferredPageArea)
    pCurPageArea = pPreferredPageArea;
  else if (pFallbackPageArea)
    pCurPageArea = pFallbackPageArea;

  if (!pCurPageArea)
    return false;

  if (!bQuery) {
    CXFA_ContainerRecord* pNewRecord = CreateContainerRecord(nullptr, false);
    AddPageAreaLayoutItem(pNewRecord, pCurPageArea);
    if (!pTargetContentArea) {
      pTargetContentArea = pCurPageArea->GetFirstChildByClass<CXFA_ContentArea>(
          XFA_Element::ContentArea);
    }
    AddContentAreaLayoutItem(pNewRecord, pTargetContentArea);
  }
  m_pCurPageArea = pCurPageArea;
  return true;
}

bool CXFA_LayoutPageMgr::MatchPageAreaOddOrEven(CXFA_Node* pPageArea) {
  if (m_ePageSetMode != XFA_AttributeValue::DuplexPaginated)
    return true;

  Optional<XFA_AttributeValue> ret =
      pPageArea->JSObject()->TryEnum(XFA_Attribute::OddOrEven, true);
  if (!ret || *ret == XFA_AttributeValue::Any)
    return true;

  int32_t iPageLast = GetPageCount() % 2;
  return *ret == XFA_AttributeValue::Odd ? iPageLast == 0 : iPageLast == 1;
}

CXFA_Node* CXFA_LayoutPageMgr::GetNextAvailPageArea(
    CXFA_Node* pTargetPageArea,
    CXFA_Node* pTargetContentArea,
    bool bNewPage,
    bool bQuery) {
  if (!m_pCurPageArea) {
    FindPageAreaFromPageSet(m_pTemplatePageSetRoot, nullptr, pTargetPageArea,
                            pTargetContentArea, bNewPage, bQuery);
    ASSERT(m_pCurPageArea);
    return m_pCurPageArea;
  }

  if (!pTargetPageArea || pTargetPageArea == m_pCurPageArea) {
    if (!bNewPage && GetNextContentArea(pTargetContentArea))
      return m_pCurPageArea;

    if (IsPageSetRootOrderedOccurrence()) {
      int32_t iMax = -1;
      CXFA_Node* pOccurNode =
          m_pCurPageArea->GetFirstChildByClass<CXFA_Occur>(XFA_Element::Occur);
      if (pOccurNode) {
        Optional<int32_t> ret =
            pOccurNode->JSObject()->TryInteger(XFA_Attribute::Max, false);
        if (ret)
          iMax = *ret;
      }
      if ((iMax < 0 || m_nCurPageCount < iMax)) {
        if (!bQuery) {
          CXFA_ContainerRecord* pNewRecord =
              CreateContainerRecord(m_pCurPageArea, false);
          AddPageAreaLayoutItem(pNewRecord, m_pCurPageArea);
          if (!pTargetContentArea) {
            pTargetContentArea =
                m_pCurPageArea->GetFirstChildByClass<CXFA_ContentArea>(
                    XFA_Element::ContentArea);
          }
          AddContentAreaLayoutItem(pNewRecord, pTargetContentArea);
        }
        m_nCurPageCount++;
        return m_pCurPageArea;
      }
    }
  }

  if (!bQuery && IsPageSetRootOrderedOccurrence())
    CreateMinPageRecord(m_pCurPageArea, false, true);
  if (FindPageAreaFromPageSet(m_pCurPageArea->GetParent(), m_pCurPageArea,
                              pTargetPageArea, pTargetContentArea, bNewPage,
                              bQuery)) {
    return m_pCurPageArea;
  }

  CXFA_Node* pPageSet = m_pCurPageArea->GetParent();
  while (true) {
    if (FindPageAreaFromPageSet(pPageSet, nullptr, pTargetPageArea,
                                pTargetContentArea, bNewPage, bQuery)) {
      return m_pCurPageArea;
    }
    if (!bQuery && IsPageSetRootOrderedOccurrence())
      CreateMinPageSetRecord(pPageSet, false);
    if (FindPageAreaFromPageSet(nullptr, pPageSet, pTargetPageArea,
                                pTargetContentArea, bNewPage, bQuery)) {
      return m_pCurPageArea;
    }
    if (pPageSet == m_pTemplatePageSetRoot)
      break;

    pPageSet = pPageSet->GetParent();
  }
  return nullptr;
}

bool CXFA_LayoutPageMgr::GetNextContentArea(CXFA_Node* pContentArea) {
  CXFA_Node* pCurContentNode =
      GetCurrentContainerRecord()->pCurContentArea->GetFormNode();
  if (!pContentArea) {
    pContentArea = pCurContentNode->GetNextSameClassSibling<CXFA_ContentArea>(
        XFA_Element::ContentArea);
    if (!pContentArea)
      return false;
  } else {
    if (pContentArea->GetParent() != m_pCurPageArea)
      return false;

    CXFA_ContainerLayoutItem* pContentAreaLayout = nullptr;
    if (!CheckContentAreaNotUsed(GetCurrentContainerRecord()->pCurPageArea,
                                 pContentArea, pContentAreaLayout)) {
      return false;
    }
    if (pContentAreaLayout) {
      if (pContentAreaLayout->GetFormNode() != pCurContentNode) {
        CXFA_ContainerRecord* pNewRecord =
            CreateContainerRecord(nullptr, false);
        pNewRecord->pCurContentArea = pContentAreaLayout;
        return true;
      }
      return false;
    }
  }

  CXFA_ContainerRecord* pNewRecord = CreateContainerRecord(nullptr, false);
  AddContentAreaLayoutItem(pNewRecord, pContentArea);
  return true;
}

void CXFA_LayoutPageMgr::InitPageSetMap() {
  if (!IsPageSetRootOrderedOccurrence())
    return;

  CXFA_NodeIterator sIterator(m_pTemplatePageSetRoot);
  for (CXFA_Node* pPageSetNode = sIterator.GetCurrent(); pPageSetNode;
       pPageSetNode = sIterator.MoveToNext()) {
    if (pPageSetNode->GetElementType() == XFA_Element::PageSet) {
      XFA_AttributeValue eRelation =
          pPageSetNode->JSObject()->GetEnum(XFA_Attribute::Relation);
      if (eRelation == XFA_AttributeValue::OrderedOccurrence)
        m_pPageSetMap[pPageSetNode] = 0;
    }
  }
}

int32_t CXFA_LayoutPageMgr::CreateMinPageRecord(CXFA_Node* pPageArea,
                                                bool bTargetPageArea,
                                                bool bCreateLast) {
  if (!pPageArea)
    return 0;

  int32_t iMin = 0;
  Optional<int32_t> ret;
  CXFA_Node* pOccurNode =
      pPageArea->GetFirstChildByClass<CXFA_Occur>(XFA_Element::Occur);
  if (pOccurNode) {
    ret = pOccurNode->JSObject()->TryInteger(XFA_Attribute::Min, false);
    if (ret)
      iMin = *ret;
  }

  if (!ret && !bTargetPageArea)
    return iMin;

  CXFA_Node* pContentArea = pPageArea->GetFirstChildByClass<CXFA_ContentArea>(
      XFA_Element::ContentArea);
  if (iMin < 1 && bTargetPageArea && !pContentArea)
    iMin = 1;

  int32_t i = 0;
  if (bCreateLast)
    i = m_nCurPageCount;

  for (; i < iMin; i++) {
    CXFA_ContainerRecord* pNewRecord = CreateContainerRecord(nullptr, false);
    AddPageAreaLayoutItem(pNewRecord, pPageArea);
    AddContentAreaLayoutItem(pNewRecord, pContentArea);
  }
  return iMin;
}

void CXFA_LayoutPageMgr::CreateMinPageSetRecord(CXFA_Node* pPageSet,
                                                bool bCreateAll) {
  if (!pPageSet)
    return;

  auto it = m_pPageSetMap.find(pPageSet);
  if (it == m_pPageSetMap.end())
    return;

  int32_t iCurSetCount = it->second;
  if (bCreateAll)
    iCurSetCount = 0;

  CXFA_Node* pOccurNode =
      pPageSet->GetFirstChildByClass<CXFA_Occur>(XFA_Element::Occur);
  if (!pOccurNode)
    return;

  Optional<int32_t> iMin =
      pOccurNode->JSObject()->TryInteger(XFA_Attribute::Min, false);
  if (!iMin || iCurSetCount >= *iMin)
    return;

  for (int32_t i = 0; i < *iMin - iCurSetCount; i++) {
    for (CXFA_Node* node = pPageSet->GetFirstChild(); node;
         node = node->GetNextSibling()) {
      if (node->GetElementType() == XFA_Element::PageArea)
        CreateMinPageRecord(node, false, false);
      else if (node->GetElementType() == XFA_Element::PageSet)
        CreateMinPageSetRecord(node, true);
    }
  }
  m_pPageSetMap[pPageSet] = *iMin;
}

void CXFA_LayoutPageMgr::CreateNextMinRecord(CXFA_Node* pRecordNode) {
  if (!pRecordNode)
    return;

  for (CXFA_Node* pCurrentNode = pRecordNode->GetNextSibling(); pCurrentNode;
       pCurrentNode = pCurrentNode->GetNextSibling()) {
    if (pCurrentNode->GetElementType() == XFA_Element::PageArea)
      CreateMinPageRecord(pCurrentNode, false, false);
    else if (pCurrentNode->GetElementType() == XFA_Element::PageSet)
      CreateMinPageSetRecord(pCurrentNode, true);
  }
}

void CXFA_LayoutPageMgr::ProcessLastPageSet() {
  CreateMinPageRecord(m_pCurPageArea, false, true);
  CreateNextMinRecord(m_pCurPageArea);
  CXFA_Node* pPageSet = m_pCurPageArea->GetParent();
  while (true) {
    CreateMinPageSetRecord(pPageSet, false);
    if (pPageSet == m_pTemplatePageSetRoot)
      break;

    CreateNextMinRecord(pPageSet);
    pPageSet = pPageSet->GetParent();
  }
}

bool CXFA_LayoutPageMgr::GetNextAvailContentHeight(float fChildHeight) {
  CXFA_Node* pCurContentNode =
      GetCurrentContainerRecord()->pCurContentArea->GetFormNode();
  if (!pCurContentNode)
    return false;

  pCurContentNode = pCurContentNode->GetNextSameClassSibling<CXFA_ContentArea>(
      XFA_Element::ContentArea);
  if (pCurContentNode) {
    float fNextContentHeight = pCurContentNode->JSObject()
                                   ->GetMeasure(XFA_Attribute::H)
                                   .ToUnit(XFA_Unit::Pt);
    return fNextContentHeight > fChildHeight;
  }

  CXFA_Node* pPageNode =
      GetCurrentContainerRecord()->pCurPageArea->GetFormNode();
  CXFA_Node* pOccurNode =
      pPageNode->GetFirstChildByClass<CXFA_Occur>(XFA_Element::Occur);
  int32_t iMax = 0;
  Optional<int32_t> ret;
  if (pOccurNode) {
    ret = pOccurNode->JSObject()->TryInteger(XFA_Attribute::Max, false);
    if (ret)
      iMax = *ret;
  }
  if (ret) {
    if (m_nCurPageCount == iMax) {
      CXFA_Node* pSrcPage = m_pCurPageArea;
      int32_t nSrcPageCount = m_nCurPageCount;
      auto psSrcIter = GetTailPosition();
      CXFA_Node* pNextPage =
          GetNextAvailPageArea(nullptr, nullptr, false, true);
      m_pCurPageArea = pSrcPage;
      m_nCurPageCount = nSrcPageCount;
      CXFA_ContainerRecord* pPrevRecord = *psSrcIter++;
      while (psSrcIter != m_ProposedContainerRecords.end()) {
        auto psSaveIter = psSrcIter;
        CXFA_ContainerRecord* pInsertRecord = *psSrcIter++;
        RemoveLayoutRecord(pInsertRecord, pPrevRecord);
        delete pInsertRecord;
        m_ProposedContainerRecords.erase(psSaveIter);
      }
      if (pNextPage) {
        CXFA_Node* pContentArea =
            pNextPage->GetFirstChildByClass<CXFA_ContentArea>(
                XFA_Element::ContentArea);
        if (pContentArea) {
          float fNextContentHeight = pContentArea->JSObject()
                                         ->GetMeasure(XFA_Attribute::H)
                                         .ToUnit(XFA_Unit::Pt);
          if (fNextContentHeight > fChildHeight)
            return true;
        }
      }
      return false;
    }
  }

  CXFA_Node* pContentArea = pPageNode->GetFirstChildByClass<CXFA_ContentArea>(
      XFA_Element::ContentArea);
  if (!pContentArea)
    return false;

  float fNextContentHeight = pContentArea->JSObject()
                                 ->GetMeasure(XFA_Attribute::H)
                                 .ToUnit(XFA_Unit::Pt);
  if (fNextContentHeight < kXFALayoutPrecision)
    return true;
  if (fNextContentHeight > fChildHeight)
    return true;
  return false;
}

void CXFA_LayoutPageMgr::ClearData() {
  if (!m_pTemplatePageSetRoot)
    return;

  auto sPos = m_ProposedContainerRecords.begin();
  while (sPos != m_ProposedContainerRecords.end()) {
    CXFA_ContainerRecord* pRecord = *sPos++;
    delete pRecord;
  }
  m_ProposedContainerRecords.clear();
  m_CurrentContainerRecordIter = m_ProposedContainerRecords.end();
  m_pCurPageArea = nullptr;
  m_nCurPageCount = 0;
  m_bCreateOverFlowPage = false;
  m_pPageSetMap.clear();
}

void CXFA_LayoutPageMgr::SaveLayoutItem(CXFA_LayoutItem* pParentLayoutItem) {
  CXFA_LayoutItem* pNextLayoutItem;
  CXFA_LayoutItem* pCurLayoutItem = pParentLayoutItem->m_pFirstChild;
  while (pCurLayoutItem) {
    pNextLayoutItem = pCurLayoutItem->m_pNextSibling;
    if (pCurLayoutItem->IsContentLayoutItem()) {
      if (pCurLayoutItem->GetFormNode()->HasRemovedChildren()) {
        CXFA_FFNotify* pNotify =
            m_pTemplatePageSetRoot->GetDocument()->GetNotify();
        CXFA_LayoutProcessor* pDocLayout =
            m_pTemplatePageSetRoot->GetDocument()->GetLayoutProcessor();
        if (pCurLayoutItem->m_pFirstChild)
          SyncRemoveLayoutItem(pCurLayoutItem, pNotify, pDocLayout);

        pNotify->OnLayoutItemRemoving(pDocLayout, pCurLayoutItem);
        delete pCurLayoutItem;
        pCurLayoutItem = pNextLayoutItem;
        continue;
      }

      if (pCurLayoutItem->GetFormNode()->IsLayoutGeneratedNode()) {
        CXFA_NodeIteratorTemplate<CXFA_Node, CXFA_TraverseStrategy_XFANode>
            sIterator(pCurLayoutItem->GetFormNode());
        for (CXFA_Node* pNode = sIterator.GetCurrent(); pNode;
             pNode = sIterator.MoveToNext()) {
          pNode->SetFlag(XFA_NodeFlag_UnusedNode);
        }
      }
    }

    if (pCurLayoutItem->m_pFirstChild)
      SaveLayoutItem(pCurLayoutItem);

    pCurLayoutItem->m_pParent = nullptr;
    pCurLayoutItem->m_pNextSibling = nullptr;
    pCurLayoutItem->m_pFirstChild = nullptr;
    if (!pCurLayoutItem->IsContentLayoutItem() &&
        pCurLayoutItem->GetFormNode()->GetElementType() !=
            XFA_Element::PageArea) {
      delete pCurLayoutItem;
    }
    pCurLayoutItem = pNextLayoutItem;
  }
}

CXFA_Node* CXFA_LayoutPageMgr::QueryOverflow(CXFA_Node* pFormNode) {
  for (CXFA_Node* pCurNode = pFormNode->GetFirstChild(); pCurNode;
       pCurNode = pCurNode->GetNextSibling()) {
    if (pCurNode->GetElementType() == XFA_Element::Break) {
      WideString wsOverflowLeader =
          pCurNode->JSObject()->GetCData(XFA_Attribute::OverflowLeader);
      WideString wsOverflowTarget =
          pCurNode->JSObject()->GetCData(XFA_Attribute::OverflowTarget);
      WideString wsOverflowTrailer =
          pCurNode->JSObject()->GetCData(XFA_Attribute::OverflowTrailer);

      if (!wsOverflowLeader.IsEmpty() || !wsOverflowTrailer.IsEmpty() ||
          !wsOverflowTarget.IsEmpty()) {
        return pCurNode;
      }
      return nullptr;
    }
    if (pCurNode->GetElementType() == XFA_Element::Overflow)
      return pCurNode;
  }
  return nullptr;
}

void CXFA_LayoutPageMgr::MergePageSetContents() {
  CXFA_Document* pDocument = m_pTemplatePageSetRoot->GetDocument();
  CXFA_FFNotify* pNotify = pDocument->GetNotify();
  CXFA_LayoutProcessor* pDocLayout = pDocument->GetLayoutProcessor();
  CXFA_ContainerLayoutItem* pRootLayout = GetRootLayoutItem();
  for (CXFA_Node* pPageNode : pDocument->m_pPendingPageSet) {
    CXFA_NodeIteratorTemplate<CXFA_Node, CXFA_TraverseStrategy_XFANode>
        sIterator(pPageNode);
    for (CXFA_Node* pNode = sIterator.GetCurrent(); pNode;
         pNode = sIterator.MoveToNext()) {
      if (pNode->IsContainerNode()) {
        CXFA_Node* pBindNode = pNode->GetBindData();
        if (pBindNode) {
          pBindNode->RemoveBindItem(pNode);
          pNode->SetBindingNode(nullptr);
        }
      }
      pNode->SetFlag(XFA_NodeFlag_UnusedNode);
    }
  }

  int32_t iIndex = 0;
  for (; pRootLayout;
       pRootLayout = ToContainerLayoutItem(pRootLayout->m_pNextSibling)) {
    CXFA_Node* pPendingPageSet = nullptr;
    CXFA_NodeIteratorTemplate<
        CXFA_ContainerLayoutItem,
        CXFA_TraverseStrategy_ContentAreaContainerLayoutItem>
        iterator(pRootLayout);
    CXFA_ContainerLayoutItem* pRootPageSetContainerItem = iterator.GetCurrent();
    ASSERT(pRootPageSetContainerItem->GetFormNode()->GetElementType() ==
           XFA_Element::PageSet);
    if (iIndex <
        pdfium::CollectionSize<int32_t>(pDocument->m_pPendingPageSet)) {
      pPendingPageSet = pDocument->m_pPendingPageSet[iIndex];
      iIndex++;
    }
    if (!pPendingPageSet) {
      if (pRootPageSetContainerItem->GetFormNode()->GetPacketType() ==
          XFA_PacketType::Template) {
        pPendingPageSet =
            pRootPageSetContainerItem->GetFormNode()->CloneTemplateToForm(
                false);
      } else {
        pPendingPageSet = pRootPageSetContainerItem->GetFormNode();
      }
    }
    if (pRootPageSetContainerItem->GetFormNode()->JSObject()->GetLayoutItem() ==
        pRootPageSetContainerItem) {
      pRootPageSetContainerItem->GetFormNode()->JSObject()->SetLayoutItem(
          nullptr);
    }
    pRootPageSetContainerItem->SetFormNode(pPendingPageSet);
    pPendingPageSet->ClearFlag(XFA_NodeFlag_UnusedNode);
    for (CXFA_ContainerLayoutItem* pContainerItem = iterator.MoveToNext();
         pContainerItem; pContainerItem = iterator.MoveToNext()) {
      CXFA_Node* pNode = pContainerItem->GetFormNode();
      if (pNode->GetPacketType() != XFA_PacketType::Template)
        continue;

      switch (pNode->GetElementType()) {
        case XFA_Element::PageSet: {
          CXFA_Node* pParentNode = pContainerItem->m_pParent->GetFormNode();
          pContainerItem->SetFormNode(XFA_NodeMerge_CloneOrMergeContainer(
              pDocument, pParentNode, pContainerItem->GetFormNode(), true,
              nullptr));
          break;
        }
        case XFA_Element::PageArea: {
          CXFA_LayoutItem* pFormLayout = pContainerItem;
          CXFA_Node* pParentNode = pContainerItem->m_pParent->GetFormNode();
          bool bIsExistForm = true;
          for (int32_t iLevel = 0; iLevel < 3; iLevel++) {
            pFormLayout = pFormLayout->m_pFirstChild;
            if (iLevel == 2) {
              while (pFormLayout &&
                     !pFormLayout->GetFormNode()->PresenceRequiresSpace()) {
                pFormLayout = pFormLayout->m_pNextSibling;
              }
            }
            if (!pFormLayout) {
              bIsExistForm = false;
              break;
            }
          }
          if (bIsExistForm) {
            CXFA_Node* pNewSubform = pFormLayout->GetFormNode();
            if (pContainerItem->m_pOldSubform &&
                pContainerItem->m_pOldSubform != pNewSubform) {
              CXFA_Node* pExistingNode = XFA_DataMerge_FindFormDOMInstance(
                  pDocument, pContainerItem->GetFormNode()->GetElementType(),
                  pContainerItem->GetFormNode()->GetNameHash(), pParentNode);
              CXFA_ContainerIterator sIterator(pExistingNode);
              for (CXFA_Node* pIter = sIterator.GetCurrent(); pIter;
                   pIter = sIterator.MoveToNext()) {
                if (pIter->GetElementType() != XFA_Element::ContentArea) {
                  CXFA_LayoutItem* pLayoutItem =
                      pIter->JSObject()->GetLayoutItem();
                  if (pLayoutItem) {
                    pNotify->OnLayoutItemRemoving(pDocLayout, pLayoutItem);
                    delete pLayoutItem;
                  }
                }
              }
              if (pExistingNode) {
                pParentNode->RemoveChild(pExistingNode, true);
              }
            }
            pContainerItem->m_pOldSubform = pNewSubform;
          }
          pContainerItem->SetFormNode(pDocument->DataMerge_CopyContainer(
              pContainerItem->GetFormNode(), pParentNode,
              ToNode(pDocument->GetXFAObject(XFA_HASHCODE_Record)), true, true,
              true));
          break;
        }
        case XFA_Element::ContentArea: {
          CXFA_Node* pParentNode = pContainerItem->m_pParent->GetFormNode();
          for (CXFA_Node* pChildNode = pParentNode->GetFirstChild(); pChildNode;
               pChildNode = pChildNode->GetNextSibling()) {
            if (pChildNode->GetTemplateNodeIfExists() !=
                pContainerItem->GetFormNode()) {
              continue;
            }
            pContainerItem->SetFormNode(pChildNode);
            break;
          }
          break;
        }
        default:
          break;
      }
    }
    if (!pPendingPageSet->GetParent()) {
      CXFA_Node* pFormToplevelSubform =
          pDocument->GetXFAObject(XFA_HASHCODE_Form)
              ->AsNode()
              ->GetFirstChildByClass<CXFA_Subform>(XFA_Element::Subform);
      if (pFormToplevelSubform)
        pFormToplevelSubform->InsertChild(pPendingPageSet, nullptr);
    }
    pDocument->DataMerge_UpdateBindingRelations(pPendingPageSet);
    pPendingPageSet->SetFlagAndNotify(XFA_NodeFlag_Initialized);
  }

  CXFA_Node* pPageSet = GetRootLayoutItem()->GetFormNode();
  while (pPageSet) {
    CXFA_Node* pNextPageSet =
        pPageSet->GetNextSameClassSibling<CXFA_PageSet>(XFA_Element::PageSet);
    CXFA_NodeIteratorTemplate<CXFA_Node, CXFA_TraverseStrategy_XFANode>
        sIterator(pPageSet);
    CXFA_Node* pNode = sIterator.GetCurrent();
    while (pNode) {
      if (pNode->IsUnusedNode()) {
        if (pNode->IsContainerNode()) {
          XFA_Element eType = pNode->GetElementType();
          if (eType == XFA_Element::PageArea || eType == XFA_Element::PageSet) {
            CXFA_ContainerIterator iteChild(pNode);
            CXFA_Node* pChildNode = iteChild.MoveToNext();
            for (; pChildNode; pChildNode = iteChild.MoveToNext()) {
              CXFA_LayoutItem* pLayoutItem =
                  pChildNode->JSObject()->GetLayoutItem();
              if (pLayoutItem) {
                pNotify->OnLayoutItemRemoving(pDocLayout, pLayoutItem);
                delete pLayoutItem;
              }
            }
          } else if (eType != XFA_Element::ContentArea) {
            CXFA_LayoutItem* pLayoutItem = pNode->JSObject()->GetLayoutItem();
            if (pLayoutItem) {
              pNotify->OnLayoutItemRemoving(pDocLayout, pLayoutItem);
              delete pLayoutItem;
            }
          }
          CXFA_Node* pNext = sIterator.SkipChildrenAndMoveToNext();
          pNode->GetParent()->RemoveChild(pNode, true);
          pNode = pNext;
        } else {
          pNode->ClearFlag(XFA_NodeFlag_UnusedNode);
          pNode->SetFlagAndNotify(XFA_NodeFlag_Initialized);
          pNode = sIterator.MoveToNext();
        }
      } else {
        pNode->SetFlagAndNotify(XFA_NodeFlag_Initialized);
        pNode = sIterator.MoveToNext();
      }
    }
    pPageSet = pNextPageSet;
  }
}

void CXFA_LayoutPageMgr::LayoutPageSetContents() {
  for (CXFA_ContainerLayoutItem* pRootLayoutItem = GetRootLayoutItem();
       pRootLayoutItem;
       pRootLayoutItem =
           ToContainerLayoutItem(pRootLayoutItem->m_pNextSibling)) {
    CXFA_NodeIteratorTemplate<
        CXFA_ContainerLayoutItem,
        CXFA_TraverseStrategy_ContentAreaContainerLayoutItem>
        iterator(pRootLayoutItem);
    for (CXFA_ContainerLayoutItem* pContainerItem = iterator.GetCurrent();
         pContainerItem; pContainerItem = iterator.MoveToNext()) {
      CXFA_Node* pNode = pContainerItem->GetFormNode();
      switch (pNode->GetElementType()) {
        case XFA_Element::PageArea:
          m_pLayoutProcessor->GetRootRootItemLayoutProcessor()
              ->DoLayoutPageArea(pContainerItem);
          break;
        default:
          break;
      }
    }
  }
}

void CXFA_LayoutPageMgr::SyncLayoutData() {
  MergePageSetContents();
  LayoutPageSetContents();
  CXFA_FFNotify* pNotify = m_pTemplatePageSetRoot->GetDocument()->GetNotify();
  int32_t nPageIdx = -1;
  for (CXFA_ContainerLayoutItem* pRootLayoutItem = GetRootLayoutItem();
       pRootLayoutItem;
       pRootLayoutItem =
           ToContainerLayoutItem(pRootLayoutItem->m_pNextSibling)) {
    CXFA_NodeIteratorTemplate<
        CXFA_ContainerLayoutItem,
        CXFA_TraverseStrategy_ContentAreaContainerLayoutItem>
        iteratorParent(pRootLayoutItem);
    for (CXFA_ContainerLayoutItem* pContainerItem = iteratorParent.GetCurrent();
         pContainerItem; pContainerItem = iteratorParent.MoveToNext()) {
      switch (pContainerItem->GetFormNode()->GetElementType()) {
        case XFA_Element::PageArea: {
          nPageIdx++;
          uint32_t dwRelevant =
              XFA_WidgetStatus_Viewable | XFA_WidgetStatus_Printable;
          CXFA_NodeIteratorTemplate<CXFA_LayoutItem,
                                    CXFA_TraverseStrategy_LayoutItem>
              iterator(pContainerItem);
          CXFA_LayoutItem* pChildLayoutItem = iterator.GetCurrent();
          while (pChildLayoutItem) {
            CXFA_ContentLayoutItem* pContentItem =
                pChildLayoutItem->AsContentLayoutItem();
            if (!pContentItem) {
              pChildLayoutItem = iterator.MoveToNext();
              continue;
            }

            XFA_AttributeValue presence =
                pContentItem->GetFormNode()
                    ->JSObject()
                    ->TryEnum(XFA_Attribute::Presence, true)
                    .value_or(XFA_AttributeValue::Visible);
            bool bVisible = presence == XFA_AttributeValue::Visible;
            uint32_t dwRelevantChild =
                GetRelevant(pContentItem->GetFormNode(), dwRelevant);
            SyncContainer(pNotify, m_pLayoutProcessor, pContentItem,
                          dwRelevantChild, bVisible, nPageIdx);
            pChildLayoutItem = iterator.SkipChildrenAndMoveToNext();
          }
          break;
        }
        default:
          break;
      }
    }
  }

  int32_t nPage = pdfium::CollectionSize<int32_t>(m_PageArray);
  for (int32_t i = nPage - 1; i >= m_nAvailPages; i--) {
    CXFA_ContainerLayoutItem* pPage = m_PageArray[i];
    m_PageArray.erase(m_PageArray.begin() + i);
    pNotify->OnPageEvent(pPage, XFA_PAGEVIEWEVENT_PostRemoved);
    delete pPage;
  }
  ClearData();
}

void XFA_ReleaseLayoutItem_NoPageArea(CXFA_LayoutItem* pLayoutItem) {
  CXFA_LayoutItem* pNode = pLayoutItem->m_pFirstChild;
  while (pNode) {
    CXFA_LayoutItem* pNext = pNode->m_pNextSibling;
    pNode->m_pParent = nullptr;
    XFA_ReleaseLayoutItem_NoPageArea(pNode);
    pNode = pNext;
  }
  if (pLayoutItem->GetFormNode()->GetElementType() != XFA_Element::PageArea)
    delete pLayoutItem;
}

void CXFA_LayoutPageMgr::PrepareLayout() {
  m_pPageSetCurRoot = nullptr;
  m_ePageSetMode = XFA_AttributeValue::OrderedOccurrence;
  m_nAvailPages = 0;
  ClearData();
  if (!m_pPageSetLayoutItemRoot)
    return;

  CXFA_ContainerLayoutItem* pRootLayoutItem = m_pPageSetLayoutItemRoot;
  if (pRootLayoutItem &&
      pRootLayoutItem->GetFormNode()->GetPacketType() == XFA_PacketType::Form) {
    CXFA_Node* pPageSetFormNode = pRootLayoutItem->GetFormNode();
    pRootLayoutItem->GetFormNode()->GetDocument()->m_pPendingPageSet.clear();
    if (pPageSetFormNode->HasRemovedChildren()) {
      XFA_ReleaseLayoutItem(pRootLayoutItem);
      m_pPageSetLayoutItemRoot = nullptr;
      pRootLayoutItem = nullptr;
      pPageSetFormNode = nullptr;
      m_PageArray.clear();
    }
    while (pPageSetFormNode) {
      CXFA_Node* pNextPageSet =
          pPageSetFormNode->GetNextSameClassSibling<CXFA_PageSet>(
              XFA_Element::PageSet);
      pPageSetFormNode->GetParent()->RemoveChild(pPageSetFormNode, false);
      pRootLayoutItem->GetFormNode()
          ->GetDocument()
          ->m_pPendingPageSet.push_back(pPageSetFormNode);
      pPageSetFormNode = pNextPageSet;
    }
  }
  pRootLayoutItem = m_pPageSetLayoutItemRoot;
  CXFA_ContainerLayoutItem* pNextLayout = nullptr;
  for (; pRootLayoutItem; pRootLayoutItem = pNextLayout) {
    pNextLayout = ToContainerLayoutItem(pRootLayoutItem->m_pNextSibling);
    SaveLayoutItem(pRootLayoutItem);
    delete pRootLayoutItem;
  }
  m_pPageSetLayoutItemRoot = nullptr;
}

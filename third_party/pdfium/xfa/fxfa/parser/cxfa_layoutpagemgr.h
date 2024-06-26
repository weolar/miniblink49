// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_LAYOUTPAGEMGR_H_
#define XFA_FXFA_PARSER_CXFA_LAYOUTPAGEMGR_H_

#include <iterator>
#include <list>
#include <map>
#include <vector>

#include "xfa/fxfa/parser/cxfa_itemlayoutprocessor.h"

class CXFA_ContainerRecord;
class CXFA_LayoutItem;
class CXFA_Node;

class CXFA_LayoutPageMgr {
 public:
  explicit CXFA_LayoutPageMgr(CXFA_LayoutProcessor* pLayoutProcessor);
  ~CXFA_LayoutPageMgr();

  bool InitLayoutPage(CXFA_Node* pFormNode);
  bool PrepareFirstPage(CXFA_Node* pRootSubform);
  float GetAvailHeight();
  bool GetNextAvailContentHeight(float fChildHeight);
  void SubmitContentItem(CXFA_ContentLayoutItem* pContentLayoutItem,
                         XFA_ItemLayoutProcessorResult eStatus);
  void FinishPaginatedPageSets();
  void SyncLayoutData();
  int32_t GetPageCount() const;
  CXFA_ContainerLayoutItem* GetPage(int32_t index) const;
  int32_t GetPageIndex(const CXFA_ContainerLayoutItem* pPage) const;
  inline CXFA_ContainerLayoutItem* GetRootLayoutItem() const {
    return m_pPageSetLayoutItemRoot;
  }
  bool ProcessBreakBeforeOrAfter(CXFA_Node* pBreakNode,
                                 bool bBefore,
                                 CXFA_Node*& pBreakLeaderNode,
                                 CXFA_Node*& pBreakTrailerNode,
                                 bool& bCreatePage);
  bool ProcessOverflow(CXFA_Node* pFormNode,
                       CXFA_Node*& pLeaderNode,
                       CXFA_Node*& pTrailerNode,
                       bool bDataMerge,
                       bool bCreatePage);
  CXFA_Node* QueryOverflow(CXFA_Node* pFormNode);
  bool ProcessBookendLeaderOrTrailer(CXFA_Node* pBookendNode,
                                     bool bLeader,
                                     CXFA_Node*& pBookendAppendNode);

 private:
  bool AppendNewPage(bool bFirstTemPage);
  void ReorderPendingLayoutRecordToTail(CXFA_ContainerRecord* pNewRecord,
                                        CXFA_ContainerRecord* pPrevRecord);
  void RemoveLayoutRecord(CXFA_ContainerRecord* pNewRecord,
                          CXFA_ContainerRecord* pPrevRecord);
  CXFA_ContainerRecord* GetCurrentContainerRecord() {
    return *m_CurrentContainerRecordIter;
  }
  std::list<CXFA_ContainerRecord*>::iterator GetTailPosition() {
    auto iter = m_ProposedContainerRecords.end();
    return !m_ProposedContainerRecords.empty() ? std::prev(iter) : iter;
  }
  CXFA_ContainerRecord* CreateContainerRecord(CXFA_Node* pPageNode,
                                              bool bCreateNew);
  void AddPageAreaLayoutItem(CXFA_ContainerRecord* pNewRecord,
                             CXFA_Node* pNewPageArea);
  void AddContentAreaLayoutItem(CXFA_ContainerRecord* pNewRecord,
                                CXFA_Node* pContentArea);
  bool RunBreak(XFA_Element eBreakType,
                XFA_AttributeValue eTargetType,
                CXFA_Node* pTarget,
                bool bStartNew);
  CXFA_Node* BreakOverflow(CXFA_Node* pOverflowNode,
                           CXFA_Node*& pLeaderTemplate,
                           CXFA_Node*& pTrailerTemplate,
                           bool bCreatePage);
  bool ResolveBookendLeaderOrTrailer(CXFA_Node* pBookendNode,
                                     bool bLeader,
                                     CXFA_Node*& pBookendAppendTemplate);
  bool ExecuteBreakBeforeOrAfter(CXFA_Node* pCurNode,
                                 bool bBefore,
                                 CXFA_Node*& pBreakLeaderTemplate,
                                 CXFA_Node*& pBreakTrailerTemplate);

  int32_t CreateMinPageRecord(CXFA_Node* pPageArea,
                              bool bTargetPageArea,
                              bool bCreateLast);
  void CreateMinPageSetRecord(CXFA_Node* pPageSet, bool bCreateAll);
  void CreateNextMinRecord(CXFA_Node* pRecordNode);
  bool FindPageAreaFromPageSet(CXFA_Node* pPageSet,
                               CXFA_Node* pStartChild,
                               CXFA_Node* pTargetPageArea,
                               CXFA_Node* pTargetContentArea,
                               bool bNewPage,
                               bool bQuery);
  bool FindPageAreaFromPageSet_Ordered(CXFA_Node* pPageSet,
                                       CXFA_Node* pStartChild,
                                       CXFA_Node* pTargetPageArea,
                                       CXFA_Node* pTargetContentArea,
                                       bool bNewPage,
                                       bool bQuery);
  bool FindPageAreaFromPageSet_SimplexDuplex(
      CXFA_Node* pPageSet,
      CXFA_Node* pStartChild,
      CXFA_Node* pTargetPageArea = nullptr,
      CXFA_Node* pTargetContentArea = nullptr,
      bool bNewPage = false,
      bool bQuery = false,
      XFA_AttributeValue ePreferredPosition = XFA_AttributeValue::First);
  bool MatchPageAreaOddOrEven(CXFA_Node* pPageArea);
  CXFA_Node* GetNextAvailPageArea(CXFA_Node* pTargetPageArea,
                                  CXFA_Node* pTargetContentArea,
                                  bool bNewPage,
                                  bool bQuery);
  bool GetNextContentArea(CXFA_Node* pTargetContentArea);
  void InitPageSetMap();
  void ProcessLastPageSet();
  bool IsPageSetRootOrderedOccurrence() const {
    return m_ePageSetMode == XFA_AttributeValue::OrderedOccurrence;
  }
  void ClearData();
  void MergePageSetContents();
  void LayoutPageSetContents();
  void PrepareLayout();
  void SaveLayoutItem(CXFA_LayoutItem* pParentLayoutItem);

  CXFA_LayoutProcessor* m_pLayoutProcessor;
  CXFA_Node* m_pTemplatePageSetRoot;
  CXFA_ContainerLayoutItem* m_pPageSetLayoutItemRoot;
  CXFA_ContainerLayoutItem* m_pPageSetCurRoot;
  std::list<CXFA_ContainerRecord*> m_ProposedContainerRecords;
  std::list<CXFA_ContainerRecord*>::iterator m_CurrentContainerRecordIter;
  CXFA_Node* m_pCurPageArea;
  int32_t m_nAvailPages;
  int32_t m_nCurPageCount;
  XFA_AttributeValue m_ePageSetMode;
  bool m_bCreateOverFlowPage;
  std::map<CXFA_Node*, int32_t> m_pPageSetMap;
  std::vector<CXFA_ContainerLayoutItem*> m_PageArray;
};

#endif  // XFA_FXFA_PARSER_CXFA_LAYOUTPAGEMGR_H_

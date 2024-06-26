// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_ITEMLAYOUTPROCESSOR_H_
#define XFA_FXFA_PARSER_CXFA_ITEMLAYOUTPROCESSOR_H_

#include <float.h>

#include <list>
#include <map>
#include <vector>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fxfa/fxfa_basic.h"

constexpr float kXFALayoutPrecision = 0.0005f;

class CXFA_ContainerLayoutItem;
class CXFA_ContentLayoutItem;
class CXFA_ItemLayoutProcessor;
class CXFA_LayoutContext;
class CXFA_LayoutPageMgr;
class CXFA_LayoutProcessor;
class CXFA_Node;

enum class XFA_ItemLayoutProcessorResult {
  Done,
  PageFullBreak,
  RowFullBreak,
  ManualBreak,
};

enum class XFA_ItemLayoutProcessorStages {
  None,
  BookendLeader,
  BreakBefore,
  Keep,
  Container,
  BreakAfter,
  BookendTrailer,
  Done,
};

class CXFA_ItemLayoutProcessor {
 public:
  CXFA_ItemLayoutProcessor(CXFA_Node* pNode, CXFA_LayoutPageMgr* pPageMgr);
  ~CXFA_ItemLayoutProcessor();

  XFA_ItemLayoutProcessorResult DoLayout(bool bUseBreakControl,
                                         float fHeightLimit,
                                         float fRealHeight,
                                         CXFA_LayoutContext* pContext);
  void DoLayoutPageArea(CXFA_ContainerLayoutItem* pPageAreaLayoutItem);

  CXFA_Node* GetFormNode() { return m_pFormNode; }
  CXFA_ContentLayoutItem* ExtractLayoutItem();

 private:
  CFX_SizeF GetCurrentComponentSize();
  bool HasLayoutItem() const { return !!m_pLayoutItem; }
  void SplitLayoutItem(float fSplitPos);

  float FindSplitPos(float fProposedSplitPos);

  bool ProcessKeepForSplit(
      CXFA_ItemLayoutProcessor* pChildProcessor,
      XFA_ItemLayoutProcessorResult eRetValue,
      std::vector<CXFA_ContentLayoutItem*>* rgCurLineLayoutItem,
      float* fContentCurRowAvailWidth,
      float* fContentCurRowHeight,
      float* fContentCurRowY,
      bool* bAddedItemInRow,
      bool* bForceEndPage,
      XFA_ItemLayoutProcessorResult* result);
  void ProcessUnUseOverFlow(CXFA_Node* pLeaderNode,
                            CXFA_Node* pTrailerNode,
                            CXFA_ContentLayoutItem* pTrailerItem,
                            CXFA_Node* pFormNode);
  bool IsAddNewRowForTrailer(CXFA_ContentLayoutItem* pTrailerItem);
  bool JudgeLeaderOrTrailerForOccur(CXFA_Node* pFormNode);

  CXFA_ContentLayoutItem* CreateContentLayoutItem(CXFA_Node* pFormNode);

  void SetCurrentComponentPos(const CFX_PointF& pos);
  void SetCurrentComponentSize(const CFX_SizeF& size);

  void SplitLayoutItem(CXFA_ContentLayoutItem* pLayoutItem,
                       CXFA_ContentLayoutItem* pSecondParent,
                       float fSplitPos);
  float InsertKeepLayoutItems();
  bool CalculateRowChildPosition(
      std::vector<CXFA_ContentLayoutItem*> (&rgCurLineLayoutItems)[3],
      XFA_AttributeValue eFlowStrategy,
      bool bContainerHeightAutoSize,
      bool bContainerWidthAutoSize,
      float* fContentCalculatedWidth,
      float* fContentCalculatedHeight,
      float* fContentCurRowY,
      float fContentCurRowHeight,
      float fContentWidthLimit,
      bool bRootForceTb);
  void ProcessUnUseBinds(CXFA_Node* pFormNode);
  bool JudgePutNextPage(CXFA_ContentLayoutItem* pParentLayoutItem,
                        float fChildHeight,
                        std::vector<CXFA_ContentLayoutItem*>* pKeepItems);

  void DoLayoutPositionedContainer(CXFA_LayoutContext* pContext);
  void DoLayoutTableContainer(CXFA_Node* pLayoutNode);
  XFA_ItemLayoutProcessorResult DoLayoutFlowedContainer(
      bool bUseBreakControl,
      XFA_AttributeValue eFlowStrategy,
      float fHeightLimit,
      float fRealHeight,
      CXFA_LayoutContext* pContext,
      bool bRootForceTb);
  void DoLayoutField();

  void GotoNextContainerNodeSimple(bool bUsePageBreak);
  void GotoNextContainerNode(CXFA_Node** pCurActionNode,
                             XFA_ItemLayoutProcessorStages* nCurStage,
                             CXFA_Node* pParentContainer,
                             bool bUsePageBreak);

  bool ProcessKeepNodesForCheckNext(CXFA_Node** pCurActionNode,
                                    XFA_ItemLayoutProcessorStages* nCurStage,
                                    CXFA_Node** pNextContainer,
                                    bool* pLastKeepNode);

  bool ProcessKeepNodesForBreakBefore(CXFA_Node** pCurActionNode,
                                      XFA_ItemLayoutProcessorStages* nCurStage,
                                      CXFA_Node* pContainerNode);

  CXFA_Node* GetSubformSetParent(CXFA_Node* pSubformSet);

  void UpdatePendingItemLayout(CXFA_ContentLayoutItem* pLayoutItem);
  void AddTrailerBeforeSplit(float fSplitPos,
                             CXFA_ContentLayoutItem* pTrailerLayoutItem,
                             bool bUseInherited);
  void AddLeaderAfterSplit(CXFA_ContentLayoutItem* pLeaderLayoutItem);
  void AddPendingNode(CXFA_Node* pPendingNode, bool bBreakPending);
  float InsertPendingItems(CXFA_Node* pCurChildNode);
  XFA_ItemLayoutProcessorResult InsertFlowedItem(
      CXFA_ItemLayoutProcessor* pProcessor,
      bool bContainerWidthAutoSize,
      bool bContainerHeightAutoSize,
      float fContainerHeight,
      XFA_AttributeValue eFlowStrategy,
      uint8_t* uCurHAlignState,
      std::vector<CXFA_ContentLayoutItem*> (&rgCurLineLayoutItems)[3],
      bool bUseBreakControl,
      float fAvailHeight,
      float fRealHeight,
      float fContentWidthLimit,
      float* fContentCurRowY,
      float* fContentCurRowAvailWidth,
      float* fContentCurRowHeight,
      bool* bAddedItemInRow,
      bool* bForceEndPage,
      CXFA_LayoutContext* pLayoutContext,
      bool bNewRow);

  CXFA_Node* m_pFormNode;
  CXFA_ContentLayoutItem* m_pLayoutItem = nullptr;
  CXFA_Node* m_pCurChildNode = nullptr;
  float m_fUsedSize = 0;
  UnownedPtr<CXFA_LayoutPageMgr> m_pPageMgr;
  std::list<CXFA_Node*> m_PendingNodes;
  bool m_bBreakPending = true;
  std::vector<float> m_rgSpecifiedColumnWidths;
  std::vector<CXFA_ContentLayoutItem*> m_arrayKeepItems;
  float m_fLastRowWidth = 0;
  float m_fLastRowY = 0;
  bool m_bUseInheriated = false;
  XFA_ItemLayoutProcessorResult m_ePreProcessRs =
      XFA_ItemLayoutProcessorResult::Done;
  bool m_bKeepBreakFinish = false;
  bool m_bIsProcessKeep = false;
  CXFA_Node* m_pKeepHeadNode = nullptr;
  CXFA_Node* m_pKeepTailNode = nullptr;
  CXFA_ContentLayoutItem* m_pOldLayoutItem = nullptr;
  CXFA_ItemLayoutProcessor* m_pCurChildPreprocessor = nullptr;
  XFA_ItemLayoutProcessorStages m_nCurChildNodeStage =
      XFA_ItemLayoutProcessorStages::None;
  std::map<CXFA_Node*, int32_t> m_PendingNodesCount;
  float m_fWidthLimite = 0;
  bool m_bHasAvailHeight = true;
};

#endif  // XFA_FXFA_PARSER_CXFA_ITEMLAYOUTPROCESSOR_H_

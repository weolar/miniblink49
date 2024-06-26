// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_layoutprocessor.h"

#include "fxjs/xfa/cjx_object.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "xfa/fxfa/parser/cxfa_contentlayoutitem.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_itemlayoutprocessor.h"
#include "xfa/fxfa/parser/cxfa_layoutpagemgr.h"
#include "xfa/fxfa/parser/cxfa_localemgr.h"
#include "xfa/fxfa/parser/cxfa_measurement.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/cxfa_subform.h"
#include "xfa/fxfa/parser/xfa_document_datamerger_imp.h"
#include "xfa/fxfa/parser/xfa_utils.h"

CXFA_LayoutProcessor::CXFA_LayoutProcessor(CXFA_Document* pDocument)
    : m_pDocument(pDocument), m_nProgressCounter(0), m_bNeedLayout(true) {}

CXFA_LayoutProcessor::~CXFA_LayoutProcessor() {}

CXFA_Document* CXFA_LayoutProcessor::GetDocument() const {
  return m_pDocument.Get();
}

int32_t CXFA_LayoutProcessor::StartLayout(bool bForceRestart) {
  if (!bForceRestart && !IsNeedLayout())
    return 100;

  m_pRootItemLayoutProcessor.reset();
  m_nProgressCounter = 0;
  CXFA_Node* pFormPacketNode =
      ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Form));
  if (!pFormPacketNode)
    return -1;

  CXFA_Subform* pFormRoot =
      pFormPacketNode->GetFirstChildByClass<CXFA_Subform>(XFA_Element::Subform);
  if (!pFormRoot)
    return -1;

  if (!m_pLayoutPageMgr)
    m_pLayoutPageMgr = pdfium::MakeUnique<CXFA_LayoutPageMgr>(this);
  if (!m_pLayoutPageMgr->InitLayoutPage(pFormRoot))
    return -1;

  if (!m_pLayoutPageMgr->PrepareFirstPage(pFormRoot))
    return -1;

  m_pRootItemLayoutProcessor = pdfium::MakeUnique<CXFA_ItemLayoutProcessor>(
      pFormRoot, m_pLayoutPageMgr.get());
  m_nProgressCounter = 1;
  return 0;
}

int32_t CXFA_LayoutProcessor::DoLayout() {
  if (m_nProgressCounter < 1)
    return -1;

  XFA_ItemLayoutProcessorResult eStatus;
  CXFA_Node* pFormNode = m_pRootItemLayoutProcessor->GetFormNode();
  float fPosX =
      pFormNode->JSObject()->GetMeasure(XFA_Attribute::X).ToUnit(XFA_Unit::Pt);
  float fPosY =
      pFormNode->JSObject()->GetMeasure(XFA_Attribute::Y).ToUnit(XFA_Unit::Pt);
  do {
    float fAvailHeight = m_pLayoutPageMgr->GetAvailHeight();
    eStatus = m_pRootItemLayoutProcessor->DoLayout(true, fAvailHeight,
                                                   fAvailHeight, nullptr);
    if (eStatus != XFA_ItemLayoutProcessorResult::Done)
      m_nProgressCounter++;

    CXFA_ContentLayoutItem* pLayoutItem =
        m_pRootItemLayoutProcessor->ExtractLayoutItem();
    if (pLayoutItem)
      pLayoutItem->m_sPos = CFX_PointF(fPosX, fPosY);

    m_pLayoutPageMgr->SubmitContentItem(pLayoutItem, eStatus);
  } while (eStatus != XFA_ItemLayoutProcessorResult::Done);

  if (eStatus == XFA_ItemLayoutProcessorResult::Done) {
    m_pLayoutPageMgr->FinishPaginatedPageSets();
    m_pLayoutPageMgr->SyncLayoutData();
    m_bNeedLayout = false;
    m_rgChangedContainers.clear();
  }
  return 100 * (eStatus == XFA_ItemLayoutProcessorResult::Done
                    ? m_nProgressCounter
                    : m_nProgressCounter - 1) /
         m_nProgressCounter;
}

bool CXFA_LayoutProcessor::IncrementLayout() {
  if (m_bNeedLayout) {
    StartLayout(true);
    return DoLayout() == 100;
  }
  return m_rgChangedContainers.empty();
}

int32_t CXFA_LayoutProcessor::CountPages() const {
  return m_pLayoutPageMgr ? m_pLayoutPageMgr->GetPageCount() : 0;
}

CXFA_ContainerLayoutItem* CXFA_LayoutProcessor::GetPage(int32_t index) const {
  return m_pLayoutPageMgr ? m_pLayoutPageMgr->GetPage(index) : nullptr;
}

CXFA_LayoutItem* CXFA_LayoutProcessor::GetLayoutItem(CXFA_Node* pFormItem) {
  return pFormItem->JSObject()->GetLayoutItem();
}

void CXFA_LayoutProcessor::AddChangedContainer(CXFA_Node* pContainer) {
  if (!pdfium::ContainsValue(m_rgChangedContainers, pContainer))
    m_rgChangedContainers.push_back(pContainer);
}

CXFA_ContainerLayoutItem* CXFA_LayoutProcessor::GetRootLayoutItem() const {
  return m_pLayoutPageMgr ? m_pLayoutPageMgr->GetRootLayoutItem() : nullptr;
}

bool CXFA_LayoutProcessor::IsNeedLayout() {
  return m_bNeedLayout || !m_rgChangedContainers.empty();
}

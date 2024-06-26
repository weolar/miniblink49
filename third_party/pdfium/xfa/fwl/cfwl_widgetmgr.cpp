// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_widgetmgr.h"

#include <utility>

#include "third_party/base/ptr_util.h"
#include "xfa/fwl/cfwl_app.h"
#include "xfa/fwl/cfwl_notedriver.h"
#include "xfa/fxfa/cxfa_ffapp.h"
#include "xfa/fxfa/cxfa_fwladapterwidgetmgr.h"

CFWL_WidgetMgr::CFWL_WidgetMgr(CXFA_FFApp* pAdapterNative)
    //: m_pAdapter(pAdapterNative->GetFWLAdapterWidgetMgr()) 
{
  m_mapWidgetItem[nullptr] = pdfium::MakeUnique<Item>();
}

CFWL_WidgetMgr::~CFWL_WidgetMgr() {}

CFWL_Widget* CFWL_WidgetMgr::GetParentWidget(const CFWL_Widget* pWidget) const {
  Item* pItem = GetWidgetMgrItem(pWidget);
  return pItem && pItem->pParent ? pItem->pParent->pWidget : nullptr;
}

CFWL_Widget* CFWL_WidgetMgr::GetOwnerWidget(const CFWL_Widget* pWidget) const {
  Item* pItem = GetWidgetMgrItem(pWidget);
  return pItem && pItem->pOwner ? pItem->pOwner->pWidget : nullptr;
}

CFWL_Widget* CFWL_WidgetMgr::GetFirstSiblingWidget(CFWL_Widget* pWidget) const {
  Item* pItem = GetWidgetMgrItem(pWidget);
  if (!pItem)
    return nullptr;

  pItem = pItem->pPrevious;
  while (pItem && pItem->pPrevious)
    pItem = pItem->pPrevious;
  return pItem ? pItem->pWidget : nullptr;
}

CFWL_Widget* CFWL_WidgetMgr::GetPriorSiblingWidget(CFWL_Widget* pWidget) const {
  Item* pItem = GetWidgetMgrItem(pWidget);
  return pItem && pItem->pPrevious ? pItem->pPrevious->pWidget : nullptr;
}

CFWL_Widget* CFWL_WidgetMgr::GetNextSiblingWidget(CFWL_Widget* pWidget) const {
  Item* pItem = GetWidgetMgrItem(pWidget);
  return pItem && pItem->pNext ? pItem->pNext->pWidget : nullptr;
}

CFWL_Widget* CFWL_WidgetMgr::GetFirstChildWidget(CFWL_Widget* pWidget) const {
  Item* pItem = GetWidgetMgrItem(pWidget);
  return pItem && pItem->pChild ? pItem->pChild->pWidget : nullptr;
}

CFWL_Widget* CFWL_WidgetMgr::GetLastChildWidget(CFWL_Widget* pWidget) const {
  Item* pItem = GetWidgetMgrItem(pWidget);
  if (!pItem)
    return nullptr;

  pItem = pItem->pChild;
  while (pItem && pItem->pNext)
    pItem = pItem->pNext;
  return pItem ? pItem->pWidget : nullptr;
}

CFWL_Widget* CFWL_WidgetMgr::GetSystemFormWidget(CFWL_Widget* pWidget) const {
  Item* pItem = GetWidgetMgrItem(pWidget);
  while (pItem) {
    if (IsAbleNative(pItem->pWidget))
      return pItem->pWidget;
    pItem = pItem->pParent;
  }
  return nullptr;
}

void CFWL_WidgetMgr::AppendWidget(CFWL_Widget* pWidget) {
  Item* pItem = GetWidgetMgrItem(pWidget);
  if (!pItem)
    return;
  if (!pItem->pParent)
    return;

  Item* pChild = pItem->pParent->pChild;
  int32_t i = 0;
  while (pChild) {
    if (pChild == pItem) {
      if (pChild->pPrevious)
        pChild->pPrevious->pNext = pChild->pNext;
      if (pChild->pNext)
        pChild->pNext->pPrevious = pChild->pPrevious;
      if (pItem->pParent->pChild == pItem)
        pItem->pParent->pChild = pItem->pNext;

      pItem->pNext = nullptr;
      pItem->pPrevious = nullptr;
      break;
    }
    if (!pChild->pNext)
      break;

    pChild = pChild->pNext;
    ++i;
  }

  pChild = pItem->pParent->pChild;
  if (pChild) {
    while (pChild->pNext)
      pChild = pChild->pNext;

    pChild->pNext = pItem;
    pItem->pPrevious = pChild;
  } else {
    pItem->pParent->pChild = pItem;
    pItem->pPrevious = nullptr;
  }
  pItem->pNext = nullptr;
}

void CFWL_WidgetMgr::RepaintWidget(CFWL_Widget* pWidget,
                                   const CFX_RectF& rect) {
  CFWL_Widget* pNative = pWidget;
  CFX_RectF transformedRect = rect;
  CFWL_Widget* pOuter = pWidget->GetOuter();
  while (pOuter) {
    CFX_RectF rtTemp = pNative->GetWidgetRect();
    transformedRect.left += rtTemp.left;
    transformedRect.top += rtTemp.top;
    pNative = pOuter;
    pOuter = pOuter->GetOuter();
  }
  AddRedrawCounts(pNative);
  //m_pAdapter->RepaintWidget(pNative);
  DebugBreak();
}

void CFWL_WidgetMgr::InsertWidget(CFWL_Widget* pParent, CFWL_Widget* pChild) {
  Item* pParentItem = GetWidgetMgrItem(pParent);
  if (!pParentItem) {
    auto item = pdfium::MakeUnique<Item>(pParent);
    pParentItem = item.get();
    m_mapWidgetItem[pParent] = std::move(item);

    pParentItem->pParent = GetWidgetMgrItem(nullptr);
    AppendWidget(pParent);
  }

  Item* pItem = GetWidgetMgrItem(pChild);
  if (!pItem) {
    auto item = pdfium::MakeUnique<Item>(pChild);
    pItem = item.get();
    m_mapWidgetItem[pChild] = std::move(item);
  }
  if (pItem->pParent && pItem->pParent != pParentItem) {
    if (pItem->pPrevious)
      pItem->pPrevious->pNext = pItem->pNext;
    if (pItem->pNext)
      pItem->pNext->pPrevious = pItem->pPrevious;
    if (pItem->pParent->pChild == pItem)
      pItem->pParent->pChild = pItem->pNext;
  }
  pItem->pParent = pParentItem;
  AppendWidget(pChild);
}

void CFWL_WidgetMgr::RemoveWidget(CFWL_Widget* pWidget) {
  Item* pItem = GetWidgetMgrItem(pWidget);
  if (!pItem)
    return;
  if (pItem->pPrevious)
    pItem->pPrevious->pNext = pItem->pNext;
  if (pItem->pNext)
    pItem->pNext->pPrevious = pItem->pPrevious;
  if (pItem->pParent && pItem->pParent->pChild == pItem)
    pItem->pParent->pChild = pItem->pNext;

  Item* pChild = pItem->pChild;
  while (pChild) {
    Item* pNext = pChild->pNext;
    RemoveWidget(pChild->pWidget);
    pChild = pNext;
  }
  m_mapWidgetItem.erase(pWidget);
}

void CFWL_WidgetMgr::SetOwner(CFWL_Widget* pOwner, CFWL_Widget* pOwned) {
  Item* pParentItem = GetWidgetMgrItem(pOwner);
  if (!pParentItem) {
    auto item = pdfium::MakeUnique<Item>(pOwner);
    pParentItem = item.get();
    m_mapWidgetItem[pOwner] = std::move(item);

    pParentItem->pParent = GetWidgetMgrItem(nullptr);
    AppendWidget(pOwner);
  }

  Item* pItem = GetWidgetMgrItem(pOwned);
  if (!pItem) {
    auto item = pdfium::MakeUnique<Item>(pOwned);
    pItem = item.get();
    m_mapWidgetItem[pOwned] = std::move(item);
  }
  pItem->pOwner = pParentItem;
}
void CFWL_WidgetMgr::SetParent(CFWL_Widget* pParent, CFWL_Widget* pChild) {
  Item* pParentItem = GetWidgetMgrItem(pParent);
  Item* pItem = GetWidgetMgrItem(pChild);
  if (!pItem)
    return;
  if (pItem->pParent && pItem->pParent != pParentItem) {
    if (pItem->pPrevious)
      pItem->pPrevious->pNext = pItem->pNext;
    if (pItem->pNext)
      pItem->pNext->pPrevious = pItem->pPrevious;
    if (pItem->pParent->pChild == pItem)
      pItem->pParent->pChild = pItem->pNext;

    pItem->pNext = nullptr;
    pItem->pPrevious = nullptr;
  }
  pItem->pParent = pParentItem;
  AppendWidget(pChild);
}

CFWL_Widget* CFWL_WidgetMgr::GetWidgetAtPoint(CFWL_Widget* parent,
                                              const CFX_PointF& point) const {
  if (!parent)
    return nullptr;

  CFWL_Widget* child = GetLastChildWidget(parent);
  while (child) {
    if (child->IsVisible()) {
      CFX_PointF pos = parent->GetMatrix().GetInverse().Transform(point);
      CFX_RectF bounds = child->GetWidgetRect();
      if (bounds.Contains(pos)) {
        pos -= bounds.TopLeft();
        return GetWidgetAtPoint(child, pos);
      }
    }
    child = GetPriorSiblingWidget(child);
  }
  return parent;
}

CFWL_Widget* CFWL_WidgetMgr::NextTab(CFWL_Widget* parent,
                                     CFWL_Widget* focus,
                                     bool& bFind) {
  CFWL_WidgetMgr* pMgr = parent->GetOwnerApp()->GetWidgetMgr();
  CFWL_Widget* child = pMgr->GetFirstChildWidget(parent);
  while (child) {
    if (focus == child)
      bFind = true;

    CFWL_Widget* bRet = NextTab(child, focus, bFind);
    if (bRet)
      return bRet;

    child = pMgr->GetNextSiblingWidget(child);
  }
  return nullptr;
}

int32_t CFWL_WidgetMgr::CountRadioButtonGroup(CFWL_Widget* pFirst) const {
  int32_t iRet = 0;
  CFWL_Widget* pChild = pFirst;
  while (pChild) {
    pChild = GetNextSiblingWidget(pChild);
    ++iRet;
  }
  return iRet;
}

CFWL_Widget* CFWL_WidgetMgr::GetRadioButtonGroupHeader(
    CFWL_Widget* pRadioButton) const {
  CFWL_Widget* pNext = pRadioButton;
  if (pNext && (pNext->GetStyles() & FWL_WGTSTYLE_Group))
    return pNext;
  return nullptr;
}

std::vector<CFWL_Widget*> CFWL_WidgetMgr::GetSameGroupRadioButton(
    CFWL_Widget* pRadioButton) const {
  CFWL_Widget* pFirst = GetFirstSiblingWidget(pRadioButton);
  if (!pFirst)
    pFirst = pRadioButton;

  if (CountRadioButtonGroup(pFirst) < 2)
    return std::vector<CFWL_Widget*>();

  std::vector<CFWL_Widget*> group;
  group.push_back(GetRadioButtonGroupHeader(pRadioButton));
  return group;
}

CFWL_Widget* CFWL_WidgetMgr::GetDefaultButton(CFWL_Widget* pParent) const {
  if ((pParent->GetClassID() == FWL_Type::PushButton) &&
      (pParent->GetStates() & (1 << (FWL_WGTSTATE_MAX + 2)))) {
    return pParent;
  }

  CFWL_Widget* child =
      pParent->GetOwnerApp()->GetWidgetMgr()->GetFirstChildWidget(pParent);
  while (child) {
    if ((child->GetClassID() == FWL_Type::PushButton) &&
        (child->GetStates() & (1 << (FWL_WGTSTATE_MAX + 2)))) {
      return child;
    }
    if (CFWL_Widget* find = GetDefaultButton(child))
      return find;

    child = child->GetOwnerApp()->GetWidgetMgr()->GetNextSiblingWidget(child);
  }
  return nullptr;
}

void CFWL_WidgetMgr::AddRedrawCounts(CFWL_Widget* pWidget) {
  GetWidgetMgrItem(pWidget)->iRedrawCounter++;
}

void CFWL_WidgetMgr::ResetRedrawCounts(CFWL_Widget* pWidget) {
  GetWidgetMgrItem(pWidget)->iRedrawCounter = 0;
}

CFWL_WidgetMgr::Item* CFWL_WidgetMgr::GetWidgetMgrItem(
    const CFWL_Widget* pWidget) const {
  auto it = m_mapWidgetItem.find(pWidget);
  return it != m_mapWidgetItem.end() ? it->second.get() : nullptr;
}

bool CFWL_WidgetMgr::IsAbleNative(CFWL_Widget* pWidget) const {
  if (!pWidget || !pWidget->IsForm())
    return false;

  return pWidget->IsOverLapper() || pWidget->IsPopup();
}

void CFWL_WidgetMgr::GetAdapterPopupPos(CFWL_Widget* pWidget,
                                        float fMinHeight,
                                        float fMaxHeight,
                                        const CFX_RectF& rtAnchor,
                                        CFX_RectF* pPopupRect) const {
  //m_pAdapter->GetPopupPos(pWidget, fMinHeight, fMaxHeight, rtAnchor, pPopupRect);
  DebugBreak();
}

void CFWL_WidgetMgr::OnProcessMessageToForm(CFWL_Message* pMessage) {
  if (!pMessage)
    return;

  CFWL_Widget* pDstWidget = pMessage->GetDstTarget();
  if (!pDstWidget)
    return;

  CFWL_NoteDriver* pNoteDriver = pDstWidget->GetOwnerApp()->GetNoteDriver();
  pNoteDriver->ProcessMessage(pMessage->Clone());

#if (_FX_OS_ == _FX_OS_MACOSX_)
  CFWL_NoteLoop* pTopLoop = pNoteDriver->GetTopLoop();
  if (pTopLoop)
    pNoteDriver->UnqueueMessageAndProcess(pTopLoop);
#endif
}

void CFWL_WidgetMgr::OnDrawWidget(CFWL_Widget* pWidget,
                                  CXFA_Graphics* pGraphics,
                                  const CFX_Matrix& matrix) {
  if (!pWidget || !pGraphics)
    return;

  CFX_RectF clipCopy(0, 0, pWidget->GetWidgetRect().Size());
  CFX_RectF clipBounds;

  pWidget->GetDelegate()->OnDrawWidget(pGraphics, matrix);
  clipBounds = pGraphics->GetClipRect();
  clipCopy = clipBounds;

  if (!clipBounds.IsEmpty())
    DrawChild(pWidget, clipBounds, pGraphics, &matrix);

  GetWidgetMgrItem(pWidget)->iRedrawCounter = 0;
  ResetRedrawCounts(pWidget);
}

void CFWL_WidgetMgr::DrawChild(CFWL_Widget* parent,
                               const CFX_RectF& rtClip,
                               CXFA_Graphics* pGraphics,
                               const CFX_Matrix* pMatrix) {
  if (!parent)
    return;

  CFWL_Widget* pNextChild = GetFirstChildWidget(parent);
  while (pNextChild) {
    CFWL_Widget* child = pNextChild;
    pNextChild = GetNextSiblingWidget(child);
    if (!child->IsVisible())
      continue;

    CFX_RectF rtWidget = child->GetWidgetRect();
    if (rtWidget.IsEmpty())
      continue;

    CFX_Matrix widgetMatrix;
    CFX_RectF clipBounds(rtWidget);
    if (pMatrix)
      widgetMatrix.Concat(*pMatrix);

    widgetMatrix.TranslatePrepend(rtWidget.left, rtWidget.top);

    if (IFWL_WidgetDelegate* pDelegate = child->GetDelegate())
      pDelegate->OnDrawWidget(pGraphics, widgetMatrix);

    DrawChild(child, clipBounds, pGraphics, &widgetMatrix);
  }
}

CFWL_WidgetMgr::Item::Item() : CFWL_WidgetMgr::Item(nullptr) {}

CFWL_WidgetMgr::Item::Item(CFWL_Widget* widget)
    : pParent(nullptr),
      pOwner(nullptr),
      pChild(nullptr),
      pPrevious(nullptr),
      pNext(nullptr),
      pWidget(widget),
      iRedrawCounter(0) {}

CFWL_WidgetMgr::Item::~Item() {}

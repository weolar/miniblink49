// Copyrig 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFPAGEVIEW_H_
#define XFA_FXFA_CXFA_FFPAGEVIEW_H_

#include <memory>
#include <vector>

#include "xfa/fxfa/parser/cxfa_containerlayoutitem.h"
#include "xfa/fxfa/parser/cxfa_contentlayoutitem.h"
#include "xfa/fxfa/parser/cxfa_traversestrategy_layoutitem.h"

class CXFA_FFWidget;
class CXFA_FFDocView;

class CXFA_FFPageView final : public CXFA_ContainerLayoutItem {
 public:
  CXFA_FFPageView(CXFA_FFDocView* pDocView, CXFA_Node* pPageArea);
  ~CXFA_FFPageView() override;

  CXFA_FFDocView* GetDocView() const;
  CFX_RectF GetPageViewRect() const;
  CFX_Matrix GetDisplayMatrix(const FX_RECT& rtDisp, int32_t iRotate) const;
  std::unique_ptr<IXFA_WidgetIterator> CreateWidgetIterator(
      uint32_t dwTraverseWay,
      uint32_t dwWidgetFilter);

 private:
  UnownedPtr<CXFA_FFDocView> const m_pDocView;
};

class CXFA_FFPageWidgetIterator final : public IXFA_WidgetIterator {
 public:
  CXFA_FFPageWidgetIterator(CXFA_FFPageView* pPageView, uint32_t dwFilter);
  ~CXFA_FFPageWidgetIterator() override;

  void Reset() override;
  CXFA_FFWidget* MoveToFirst() override;
  CXFA_FFWidget* MoveToLast() override;
  CXFA_FFWidget* MoveToNext() override;
  CXFA_FFWidget* MoveToPrevious() override;
  CXFA_FFWidget* GetCurrentWidget() override;
  bool SetCurrentWidget(CXFA_FFWidget* hWidget) override;

 private:
  CXFA_FFWidget* GetWidget(CXFA_LayoutItem* pLayoutItem);

  UnownedPtr<CXFA_FFPageView> m_pPageView;
  UnownedPtr<CXFA_FFWidget> m_hCurWidget;
  uint32_t m_dwFilter;
  bool m_bIgnorerelevant;
  CXFA_LayoutItemIterator m_sIterator;
};

class CXFA_TabParam {
 public:
  explicit CXFA_TabParam(CXFA_FFWidget* pWidget);
  ~CXFA_TabParam();

  void AppendTabParam(CXFA_TabParam* pParam);
  void ClearChildren();
  CXFA_FFWidget* GetWidget() const { return m_pWidget.Get(); }
  const std::vector<CXFA_FFWidget*>& GetChildren() const { return m_Children; }

 private:
  UnownedPtr<CXFA_FFWidget> const m_pWidget;
  std::vector<CXFA_FFWidget*> m_Children;
};

class CXFA_FFTabOrderPageWidgetIterator final : public IXFA_WidgetIterator {
 public:
  CXFA_FFTabOrderPageWidgetIterator(CXFA_FFPageView* pPageView,
                                    uint32_t dwFilter);
  ~CXFA_FFTabOrderPageWidgetIterator() override;

  void Reset() override;
  CXFA_FFWidget* MoveToFirst() override;
  CXFA_FFWidget* MoveToLast() override;
  CXFA_FFWidget* MoveToNext() override;
  CXFA_FFWidget* MoveToPrevious() override;
  CXFA_FFWidget* GetCurrentWidget() override;
  bool SetCurrentWidget(CXFA_FFWidget* hWidget) override;

 private:
  CXFA_FFWidget* GetTraverseWidget(CXFA_FFWidget* pWidget);
  CXFA_FFWidget* FindWidgetByName(const WideString& wsWidgetName,
                                  CXFA_FFWidget* pRefWidget);
  void CreateTabOrderWidgetArray();
  void CreateSpaceOrderWidgetArray(std::vector<CXFA_FFWidget*>* WidgetArray);
  CXFA_FFWidget* GetWidget(CXFA_LayoutItem* pLayoutItem);
  void OrderContainer(CXFA_LayoutItemIterator* sIterator,
                      CXFA_LayoutItem* pContainerItem,
                      CXFA_TabParam* pContainer,
                      bool* bCurrentItem,
                      bool* bContentArea,
                      bool bMasterPage);

  std::vector<UnownedPtr<CXFA_FFWidget>> m_TabOrderWidgetArray;
  UnownedPtr<CXFA_FFPageView> m_pPageView;
  uint32_t m_dwFilter;
  int32_t m_iCurWidget;
  bool m_bIgnorerelevant;
};

#endif  // XFA_FXFA_CXFA_FFPAGEVIEW_H_

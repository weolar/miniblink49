// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_PWL_CPWL_LIST_IMPL_H_
#define FPDFSDK_PWL_CPWL_LIST_IMPL_H_

#include <map>
#include <memory>
#include <vector>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/unowned_ptr.h"

class CPWL_EditImpl;
class CPWL_EditImpl_Iterator;
class CPWL_List_Notify;
class IPVT_FontMap;

class CPLST_Select {
 public:
  enum State { DESELECTING = -1, NORMAL = 0, SELECTING = 1 };
  using const_iterator = std::map<int32_t, State>::const_iterator;

  CPLST_Select();
  virtual ~CPLST_Select();

  void Add(int32_t nItemIndex);
  void Add(int32_t nBeginIndex, int32_t nEndIndex);
  void Sub(int32_t nItemIndex);
  void Sub(int32_t nBeginIndex, int32_t nEndIndex);
  void DeselectAll();
  void Done();

  const_iterator begin() const { return m_Items.begin(); }
  const_iterator end() const { return m_Items.end(); }

 private:
  std::map<int32_t, State> m_Items;
};

class CPWL_ListCtrl {
 public:
  CPWL_ListCtrl();
  ~CPWL_ListCtrl();

  void SetNotify(CPWL_List_Notify* pNotify) { m_pNotify = pNotify; }
  void OnMouseDown(const CFX_PointF& point, bool bShift, bool bCtrl);
  void OnMouseMove(const CFX_PointF& point, bool bShift, bool bCtrl);
  void OnVK_UP(bool bShift, bool bCtrl);
  void OnVK_DOWN(bool bShift, bool bCtrl);
  void OnVK_LEFT(bool bShift, bool bCtrl);
  void OnVK_RIGHT(bool bShift, bool bCtrl);
  void OnVK_HOME(bool bShift, bool bCtrl);
  void OnVK_END(bool bShift, bool bCtrl);
  bool OnChar(uint16_t nChar, bool bShift, bool bCtrl);

  void SetScrollPos(const CFX_PointF& point);
  void ScrollToListItem(int32_t nItemIndex);
  CFX_FloatRect GetItemRect(int32_t nIndex) const;
  int32_t GetCaret() const { return m_nCaretIndex; }
  int32_t GetSelect() const { return m_nSelItem; }
  int32_t GetTopItem() const;
  void SetContentRect(const CFX_FloatRect& rect) { m_rcContent = rect; }
  CFX_FloatRect GetContentRect() const;

  int32_t GetItemIndex(const CFX_PointF& point) const;
  void AddString(const WideString& str);
  void SetTopItem(int32_t nIndex);
  void Select(int32_t nItemIndex);
  void SetCaret(int32_t nItemIndex);
  void Empty();
  void Cancel();
  WideString GetText() const;

  void SetFontMap(IPVT_FontMap* pFontMap) { m_pFontMap = pFontMap; }
  void SetFontSize(float fFontSize) { m_fFontSize = fFontSize; }
  CFX_FloatRect GetPlateRect() const { return m_rcPlate; }
  void SetPlateRect(const CFX_FloatRect& rect);

  float GetFontSize() const { return m_fFontSize; }
  CPWL_EditImpl* GetItemEdit(int32_t nIndex) const;
  int32_t GetCount() const;
  bool IsItemSelected(int32_t nIndex) const;
  float GetFirstHeight() const;
  void SetMultipleSel(bool bMultiple) { m_bMultiple = bMultiple; }
  bool IsMultipleSel() const { return m_bMultiple; }
  int32_t FindNext(int32_t nIndex, wchar_t nChar) const;
  int32_t GetFirstSelected() const;

 private:
  class Item {
   public:
    Item();
    ~Item();

    void SetFontMap(IPVT_FontMap* pFontMap);
    CPWL_EditImpl* GetEdit() const { return m_pEdit.get(); }

    void SetRect(const CFX_FloatRect& rect) { m_rcListItem = rect; }
    void SetSelect(bool bSelected) { m_bSelected = bSelected; }
    void SetText(const WideString& text);
    void SetFontSize(float fFontSize);
    WideString GetText() const;

    CFX_FloatRect GetRect() const { return m_rcListItem; }
    bool IsSelected() const { return m_bSelected; }
    float GetItemHeight() const;
    uint16_t GetFirstChar() const;

   private:
    CPWL_EditImpl_Iterator* GetIterator() const;

    std::unique_ptr<CPWL_EditImpl> m_pEdit;
    bool m_bSelected;
    CFX_FloatRect m_rcListItem;
  };

  CFX_PointF InToOut(const CFX_PointF& point) const;
  CFX_PointF OutToIn(const CFX_PointF& point) const;
  CFX_FloatRect InToOut(const CFX_FloatRect& rect) const;
  CFX_FloatRect OutToIn(const CFX_FloatRect& rect) const;

  CFX_PointF InnerToOuter(const CFX_PointF& point) const;
  CFX_PointF OuterToInner(const CFX_PointF& point) const;
  CFX_FloatRect InnerToOuter(const CFX_FloatRect& rect) const;
  CFX_FloatRect OuterToInner(const CFX_FloatRect& rect) const;

  void OnVK(int32_t nItemIndex, bool bShift, bool bCtrl);
  bool IsValid(int32_t nItemIndex) const;

  void ReArrange(int32_t nItemIndex);
  CFX_FloatRect GetItemRectInternal(int32_t nIndex) const;
  CFX_FloatRect GetContentRectInternal() const;
  void SetMultipleSelect(int32_t nItemIndex, bool bSelected);
  void SetSingleSelect(int32_t nItemIndex);
  void InvalidateItem(int32_t nItemIndex);
  void SelectItems();
  bool IsItemVisible(int32_t nItemIndex) const;
  void SetScrollInfo();
  void SetScrollPosY(float fy);
  void AddItem(const WideString& str);
  WideString GetItemText(int32_t nIndex) const;
  void SetItemSelect(int32_t nItemIndex, bool bSelected);
  int32_t GetLastSelected() const;
  CFX_PointF GetBTPoint() const {
    return CFX_PointF(m_rcPlate.left, m_rcPlate.top);
  }

  CFX_FloatRect m_rcPlate;
  CFX_FloatRect m_rcContent;
  UnownedPtr<CPWL_List_Notify> m_pNotify;
  bool m_bNotifyFlag;
  CFX_PointF m_ptScrollPos;
  CPLST_Select m_aSelItems;  // for multiple
  int32_t m_nSelItem;        // for single
  int32_t m_nFootIndex;      // for multiple
  bool m_bCtrlSel;           // for multiple
  int32_t m_nCaretIndex;     // for multiple
  std::vector<std::unique_ptr<Item>> m_ListItems;
  float m_fFontSize;
  UnownedPtr<IPVT_FontMap> m_pFontMap;
  bool m_bMultiple;
};

#endif  // FPDFSDK_PWL_CPWL_LIST_IMPL_H_

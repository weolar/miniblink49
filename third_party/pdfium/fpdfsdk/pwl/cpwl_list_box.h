// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_PWL_CPWL_LIST_BOX_H_
#define FPDFSDK_PWL_CPWL_LIST_BOX_H_

#include <memory>

#include "core/fxcrt/unowned_ptr.h"
#include "fpdfsdk/pwl/cpwl_wnd.h"

class CPWL_ListCtrl;
class CPWL_List_Notify;
class CPWL_ListBox;
class IPWL_Filler_Notify;
struct CPVT_WordPlace;

class CPWL_List_Notify {
 public:
  explicit CPWL_List_Notify(CPWL_ListBox* pList);
  ~CPWL_List_Notify();

  void IOnSetScrollInfoY(float fPlateMin,
                         float fPlateMax,
                         float fContentMin,
                         float fContentMax,
                         float fSmallStep,
                         float fBigStep);
  void IOnSetScrollPosY(float fy);
  void IOnInvalidateRect(CFX_FloatRect* pRect);

 private:
  UnownedPtr<CPWL_ListBox> m_pList;
};

class CPWL_ListBox : public CPWL_Wnd {
 public:
  CPWL_ListBox(const CreateParams& cp,
               std::unique_ptr<PrivateData> pAttachedData);
  ~CPWL_ListBox() override;

  // CPWL_Wnd
  void OnCreated() override;
  void OnDestroy() override;
  void DrawThisAppearance(CFX_RenderDevice* pDevice,
                          const CFX_Matrix& mtUser2Device) override;
  bool OnKeyDown(uint16_t nChar, uint32_t nFlag) override;
  bool OnChar(uint16_t nChar, uint32_t nFlag) override;
  bool OnLButtonDown(const CFX_PointF& point, uint32_t nFlag) override;
  bool OnLButtonUp(const CFX_PointF& point, uint32_t nFlag) override;
  bool OnMouseMove(const CFX_PointF& point, uint32_t nFlag) override;
  bool OnMouseWheel(short zDelta,
                    const CFX_PointF& point,
                    uint32_t nFlag) override;
  WideString GetText() override;
  void SetScrollInfo(const PWL_SCROLL_INFO& info) override;
  void SetScrollPosition(float pos) override;
  void ScrollWindowVertically(float pos) override;
  bool RePosChildWnd() override;
  CFX_FloatRect GetFocusRect() const override;
  void SetFontSize(float fFontSize) override;
  float GetFontSize() const override;

  bool OnNotifySelectionChanged(bool bKeyDown, uint32_t nFlag);

  void AddString(const WideString& str);
  void SetTopVisibleIndex(int32_t nItemIndex);
  void ScrollToListItem(int32_t nItemIndex);
  void ResetContent();
  void Reset();
  void Select(int32_t nItemIndex);
  void SetCaret(int32_t nItemIndex);
  void SetHoverSel(bool bHoverSel);

  int32_t GetCount() const;
  bool IsMultipleSel() const;
  int32_t GetCaretIndex() const;
  int32_t GetCurSel() const;
  bool IsItemSelected(int32_t nItemIndex) const;
  int32_t GetTopVisibleIndex() const;
  int32_t FindNext(int32_t nIndex, wchar_t nChar) const;
  CFX_FloatRect GetContentRect() const;
  float GetFirstHeight() const;
  CFX_FloatRect GetListRect() const;

  void SetFillerNotify(IPWL_Filler_Notify* pNotify) {
    m_pFillerNotify = pNotify;
  }

  void AttachFFLData(CFFL_FormFiller* pData) { m_pFormFiller = pData; }

 protected:
  bool m_bMouseDown = false;
  bool m_bHoverSel = false;
  std::unique_ptr<CPWL_ListCtrl> m_pList;
  std::unique_ptr<CPWL_List_Notify> m_pListNotify;
  UnownedPtr<IPWL_Filler_Notify> m_pFillerNotify;

 private:
  UnownedPtr<CFFL_FormFiller> m_pFormFiller;
};

#endif  // FPDFSDK_PWL_CPWL_LIST_BOX_H_

// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FORMFILLER_CFFL_INTERACTIVEFORMFILLER_H_
#define FPDFSDK_FORMFILLER_CFFL_INTERACTIVEFORMFILLER_H_

#include <map>
#include <memory>
#include <utility>

#include "core/fxcrt/unowned_ptr.h"
#include "fpdfsdk/cpdfsdk_annot.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "fpdfsdk/pwl/cpwl_edit.h"

class CFFL_FormFiller;
class CPDFSDK_FormFillEnvironment;
class CPDFSDK_PageView;
class CPDFSDK_Widget;

class CFFL_InteractiveFormFiller final : public IPWL_Filler_Notify {
 public:
  explicit CFFL_InteractiveFormFiller(
      CPDFSDK_FormFillEnvironment* pFormFillEnv);
  ~CFFL_InteractiveFormFiller() override;

  bool Annot_HitTest(CPDFSDK_PageView* pPageView,
                     CPDFSDK_Annot* pAnnot,
                     const CFX_PointF& point);
  FX_RECT GetViewBBox(CPDFSDK_PageView* pPageView, CPDFSDK_Annot* pAnnot);
  void OnDraw(CPDFSDK_PageView* pPageView,
              CPDFSDK_Annot* pAnnot,
              CFX_RenderDevice* pDevice,
              const CFX_Matrix& mtUser2Device);

  void OnDelete(CPDFSDK_Annot* pAnnot);

  void OnMouseEnter(CPDFSDK_PageView* pPageView,
                    CPDFSDK_Annot::ObservedPtr* pAnnot,
                    uint32_t nFlag);
  void OnMouseExit(CPDFSDK_PageView* pPageView,
                   CPDFSDK_Annot::ObservedPtr* pAnnot,
                   uint32_t nFlag);
  bool OnLButtonDown(CPDFSDK_PageView* pPageView,
                     CPDFSDK_Annot::ObservedPtr* pAnnot,
                     uint32_t nFlags,
                     const CFX_PointF& point);
  bool OnLButtonUp(CPDFSDK_PageView* pPageView,
                   CPDFSDK_Annot::ObservedPtr* pAnnot,
                   uint32_t nFlags,
                   const CFX_PointF& point);
  bool OnLButtonDblClk(CPDFSDK_PageView* pPageView,
                       CPDFSDK_Annot::ObservedPtr* pAnnot,
                       uint32_t nFlags,
                       const CFX_PointF& point);
  bool OnMouseMove(CPDFSDK_PageView* pPageView,
                   CPDFSDK_Annot::ObservedPtr* pAnnot,
                   uint32_t nFlags,
                   const CFX_PointF& point);
  bool OnMouseWheel(CPDFSDK_PageView* pPageView,
                    CPDFSDK_Annot::ObservedPtr* pAnnot,
                    uint32_t nFlags,
                    short zDelta,
                    const CFX_PointF& point);
  bool OnRButtonDown(CPDFSDK_PageView* pPageView,
                     CPDFSDK_Annot::ObservedPtr* pAnnot,
                     uint32_t nFlags,
                     const CFX_PointF& point);
  bool OnRButtonUp(CPDFSDK_PageView* pPageView,
                   CPDFSDK_Annot::ObservedPtr* pAnnot,
                   uint32_t nFlags,
                   const CFX_PointF& point);

  bool OnKeyDown(CPDFSDK_Annot* pAnnot, uint32_t nKeyCode, uint32_t nFlags);
  bool OnChar(CPDFSDK_Annot* pAnnot, uint32_t nChar, uint32_t nFlags);

  bool OnSetFocus(CPDFSDK_Annot::ObservedPtr* pAnnot, uint32_t nFlag);
  bool OnKillFocus(CPDFSDK_Annot::ObservedPtr* pAnnot, uint32_t nFlag);

  CFFL_FormFiller* GetFormFiller(CPDFSDK_Annot* pAnnot, bool bRegister);

  WideString GetText(CPDFSDK_Annot* pAnnot);
  WideString GetSelectedText(CPDFSDK_Annot* pAnnot);
  void ReplaceSelection(CPDFSDK_Annot* pAnnot, const WideString& text);

  bool CanUndo(CPDFSDK_Annot* pAnnot);
  bool CanRedo(CPDFSDK_Annot* pAnnot);
  bool Undo(CPDFSDK_Annot* pAnnot);
  bool Redo(CPDFSDK_Annot* pAnnot);

  static bool IsVisible(CPDFSDK_Widget* pWidget);
  static bool IsReadOnly(CPDFSDK_Widget* pWidget);
  static bool IsFillingAllowed(CPDFSDK_Widget* pWidget);
  static bool IsValidAnnot(CPDFSDK_PageView* pPageView, CPDFSDK_Annot* pAnnot);

  bool OnKeyStrokeCommit(CPDFSDK_Annot::ObservedPtr* pWidget,
                         CPDFSDK_PageView* pPageView,
                         uint32_t nFlag);
  bool OnValidate(CPDFSDK_Annot::ObservedPtr* pAnnot,
                  CPDFSDK_PageView* pPageView,
                  uint32_t nFlag);
  void OnCalculate(CPDFSDK_Annot::ObservedPtr* pAnnot,
                   CPDFSDK_PageView* pPageView,
                   uint32_t nFlag);
  void OnFormat(CPDFSDK_Annot::ObservedPtr* pAnnot,
                CPDFSDK_PageView* pPageView,
                uint32_t nFlag);
  bool OnButtonUp(CPDFSDK_Annot::ObservedPtr* pAnnot,
                  CPDFSDK_PageView* pPageView,
                  uint32_t nFlag);
#ifdef PDF_ENABLE_XFA
  bool OnClick(CPDFSDK_Annot::ObservedPtr* pAnnot,
               CPDFSDK_PageView* pPageView,
               uint32_t nFlag);
  bool OnFull(CPDFSDK_Annot::ObservedPtr* pAnnot,
              CPDFSDK_PageView* pPageView,
              uint32_t nFlag);
  bool OnPreOpen(CPDFSDK_Annot::ObservedPtr* pAnnot,
                 CPDFSDK_PageView* pPageView,
                 uint32_t nFlag);
  bool OnPostOpen(CPDFSDK_Annot::ObservedPtr* pAnnot,
                  CPDFSDK_PageView* pPageView,
                  uint32_t nFlag);
#endif  // PDF_ENABLE_XFA

 private:
  using CFFL_Widget2Filler =
      std::map<CPDFSDK_Annot*, std::unique_ptr<CFFL_FormFiller>>;

  // IPWL_Filler_Notify:
  void QueryWherePopup(const CPWL_Wnd::PrivateData* pAttached,
                       float fPopupMin,
                       float fPopupMax,
                       bool* bBottom,
                       float* fPopupRet) override;
  // Returns {bRC, bExit}.
  std::pair<bool, bool> OnBeforeKeyStroke(
      const CPWL_Wnd::PrivateData* pAttached,
      WideString& strChange,
      const WideString& strChangeEx,
      int nSelStart,
      int nSelEnd,
      bool bKeyDown,
      uint32_t nFlag) override;
#ifdef PDF_ENABLE_XFA
  bool OnPopupPreOpen(const CPWL_Wnd::PrivateData* pAttached,
                      uint32_t nFlag) override;
  bool OnPopupPostOpen(const CPWL_Wnd::PrivateData* pAttached,
                       uint32_t nFlag) override;
  void SetFocusAnnotTab(CPDFSDK_Annot* pWidget, bool bSameField, bool bNext);
#endif  // PDF_ENABLE_XFA

  void UnRegisterFormFiller(CPDFSDK_Annot* pAnnot);

  UnownedPtr<CPDFSDK_FormFillEnvironment> const m_pFormFillEnv;
  CFFL_Widget2Filler m_Maps;
  bool m_bNotifying;
};

class CFFL_PrivateData final : public CPWL_Wnd::PrivateData {
 public:
  CFFL_PrivateData();
  CFFL_PrivateData(const CFFL_PrivateData& that);
  ~CFFL_PrivateData() override;

  // CPWL_Wnd::PrivateData:
  std::unique_ptr<CPWL_Wnd::PrivateData> Clone() const override;

  CPDFSDK_Widget* pWidget = nullptr;
  CPDFSDK_PageView* pPageView = nullptr;
  uint32_t nWidgetAppearanceAge = 0;
  uint32_t nWidgetValueAge = 0;
};

#endif  // FPDFSDK_FORMFILLER_CFFL_INTERACTIVEFORMFILLER_H_

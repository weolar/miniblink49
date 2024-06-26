// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_PWL_CPWL_EDIT_H_
#define FPDFSDK_PWL_CPWL_EDIT_H_

#include <memory>
#include <utility>

#include "core/fpdfdoc/cpvt_wordrange.h"
#include "core/fxcrt/unowned_ptr.h"
#include "fpdfsdk/pwl/cpwl_edit_ctrl.h"

class IPWL_Filler_Notify {
 public:
  virtual ~IPWL_Filler_Notify() = default;

  // Must write to |bBottom| and |fPopupRet|.
  virtual void QueryWherePopup(const CPWL_Wnd::PrivateData* pAttached,
                               float fPopupMin,
                               float fPopupMax,
                               bool* bBottom,
                               float* fPopupRet) = 0;
  virtual std::pair<bool, bool> OnBeforeKeyStroke(
      const CPWL_Wnd::PrivateData* pAttached,
      WideString& strChange,
      const WideString& strChangeEx,
      int nSelStart,
      int nSelEnd,
      bool bKeyDown,
      uint32_t nFlag) = 0;

#ifdef PDF_ENABLE_XFA
  virtual bool OnPopupPreOpen(const CPWL_Wnd::PrivateData* pAttached,
                              uint32_t nFlag) = 0;
  virtual bool OnPopupPostOpen(const CPWL_Wnd::PrivateData* pAttached,
                               uint32_t nFlag) = 0;
#endif  // PDF_ENABLE_XFA
};

class CPWL_Edit final : public CPWL_EditCtrl {
 public:
  CPWL_Edit(const CreateParams& cp, std::unique_ptr<PrivateData> pAttachedData);
  ~CPWL_Edit() override;

  // CPWL_EditCtrl
  void OnCreated() override;
  bool RePosChildWnd() override;
  CFX_FloatRect GetClientRect() const override;
  void DrawThisAppearance(CFX_RenderDevice* pDevice,
                          const CFX_Matrix& mtUser2Device) override;
  bool OnLButtonDown(const CFX_PointF& point, uint32_t nFlag) override;
  bool OnLButtonDblClk(const CFX_PointF& point, uint32_t nFlag) override;
  bool OnRButtonUp(const CFX_PointF& point, uint32_t nFlag) override;
  bool OnMouseWheel(short zDelta,
                    const CFX_PointF& point,
                    uint32_t nFlag) override;
  bool OnKeyDown(uint16_t nChar, uint32_t nFlag) override;
  bool OnChar(uint16_t nChar, uint32_t nFlag) override;
  CFX_FloatRect GetFocusRect() const override;
  void OnSetFocus() override;
  void OnKillFocus() override;

  void SetAlignFormatVerticalCenter();
  void SetCharArray(int32_t nCharArray);
  void SetLimitChar(int32_t nLimitChar);
  void SetCharSpace(float fCharSpace);

  bool CanSelectAll() const;
  bool CanCopy() const;
  bool CanCut() const;

  void CutText();

  void SetText(const WideString& csText);
  void ReplaceSel(const WideString& csText);

  bool IsTextFull() const;

  static float GetCharArrayAutoFontSize(const CPDF_Font* pFont,
                                        const CFX_FloatRect& rcPlate,
                                        int32_t nCharArray);

  void SetFillerNotify(IPWL_Filler_Notify* pNotify) {
    m_pFillerNotify = pNotify;
  }

  void AttachFFLData(CFFL_FormFiller* pData) { m_pFormFiller = pData; }

  void OnInsertWord(const CPVT_WordPlace& place,
                    const CPVT_WordPlace& oldplace);
  void OnInsertReturn(const CPVT_WordPlace& place,
                      const CPVT_WordPlace& oldplace);
  void OnBackSpace(const CPVT_WordPlace& place, const CPVT_WordPlace& oldplace);
  void OnDelete(const CPVT_WordPlace& place, const CPVT_WordPlace& oldplace);
  void OnClear(const CPVT_WordPlace& place, const CPVT_WordPlace& oldplace);
  void OnInsertText(const CPVT_WordPlace& place,
                    const CPVT_WordPlace& oldplace);

 private:
  // In case of implementation swallow the OnKeyDown event. If the event is
  // swallowed, implementation may do other unexpected things, which is not the
  // control means to do.
  static bool IsProceedtoOnChar(uint16_t nKeyCode, uint32_t nFlag);

  CPVT_WordRange GetSelectWordRange() const;
  bool IsVScrollBarVisible() const;
  void SetParamByFlag();

  CFX_PointF GetWordRightBottomPoint(const CPVT_WordPlace& wpWord);

  CPVT_WordRange CombineWordRange(const CPVT_WordRange& wr1,
                                  const CPVT_WordRange& wr2);
  CPVT_WordRange GetLatinWordsRange(const CFX_PointF& point) const;
  CPVT_WordRange GetLatinWordsRange(const CPVT_WordPlace& place) const;
  CPVT_WordRange GetSameWordsRange(const CPVT_WordPlace& place,
                                   bool bLatin,
                                   bool bArabic) const;

  bool m_bFocus = false;
  CFX_FloatRect m_rcOldWindow;
  UnownedPtr<IPWL_Filler_Notify> m_pFillerNotify;
  UnownedPtr<CFFL_FormFiller> m_pFormFiller;
};

#endif  // FPDFSDK_PWL_CPWL_EDIT_H_

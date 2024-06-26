// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_PWL_CPWL_EDIT_CTRL_H_
#define FPDFSDK_PWL_CPWL_EDIT_CTRL_H_

#include <memory>

#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/fx_string.h"
#include "fpdfsdk/pwl/cpwl_wnd.h"

class CPWL_EditImpl;
class CPWL_Caret;
struct CPVT_WordPlace;

enum PWL_EDIT_ALIGNFORMAT_H { PEAH_LEFT = 0, PEAH_MIDDLE, PEAH_RIGHT };

enum PWL_EDIT_ALIGNFORMAT_V { PEAV_TOP = 0, PEAV_CENTER, PEAV_BOTTOM };

class CPWL_EditCtrl : public CPWL_Wnd {
 public:
  CPWL_EditCtrl(const CreateParams& cp,
                std::unique_ptr<PrivateData> pAttachedData);
  ~CPWL_EditCtrl() override;

  void SetSelection(int32_t nStartChar, int32_t nEndChar);
  void GetSelection(int32_t& nStartChar, int32_t& nEndChar) const;
  void ClearSelection();
  void SelectAll();

  CFX_PointF GetScrollPos() const;
  void SetScrollPos(const CFX_PointF& point);

  void SetCharSet(uint8_t nCharSet) { m_nCharSet = nCharSet; }
  int32_t GetCharSet() const;

  bool CanUndo() override;
  bool CanRedo() override;
  bool Undo() override;
  bool Redo() override;

  void SetReadyToInput();

  // CPWL_Wnd:
  void OnCreated() override;
  bool OnKeyDown(uint16_t nChar, uint32_t nFlag) override;
  bool OnChar(uint16_t nChar, uint32_t nFlag) override;
  bool OnLButtonDown(const CFX_PointF& point, uint32_t nFlag) override;
  bool OnLButtonUp(const CFX_PointF& point, uint32_t nFlag) override;
  bool OnMouseMove(const CFX_PointF& point, uint32_t nFlag) override;
  void SetScrollInfo(const PWL_SCROLL_INFO& info) override;
  void SetScrollPosition(float pos) override;
  void ScrollWindowVertically(float pos) override;
  void CreateChildWnd(const CreateParams& cp) override;
  bool RePosChildWnd() override;
  void SetFontSize(float fFontSize) override;
  float GetFontSize() const override;
  void SetCursor() override;
  WideString GetText() override;
  WideString GetSelectedText() override;
  void ReplaceSelection(const WideString& text) override;

  bool SetCaret(bool bVisible,
                const CFX_PointF& ptHead,
                const CFX_PointF& ptFoot);

 protected:
  void CopyText();
  void PasteText();
  void CutText();
  void InsertWord(uint16_t word, int32_t nCharset);
  void InsertReturn();
  bool IsWndHorV();
  void Delete();
  void Backspace();
  void GetCaretInfo(CFX_PointF* ptHead, CFX_PointF* ptFoot) const;
  void SetEditCaret(bool bVisible);

  std::unique_ptr<CPWL_EditImpl> const m_pEdit;
  CPWL_Caret* m_pEditCaret = nullptr;
  bool m_bMouseDown = false;

 private:
  void CreateEditCaret(const CreateParams& cp);

  int32_t m_nCharSet = FX_CHARSET_Default;
};

#endif  // FPDFSDK_PWL_CPWL_EDIT_CTRL_H_

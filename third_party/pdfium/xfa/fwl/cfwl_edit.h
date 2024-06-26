// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_EDIT_H_
#define XFA_FWL_CFWL_EDIT_H_

#include <memory>
#include <utility>

#include "xfa/fde/cfde_texteditengine.h"
#include "xfa/fwl/cfwl_event.h"
#include "xfa/fwl/cfwl_scrollbar.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fxgraphics/cxfa_gepath.h"

#define FWL_STYLEEXT_EDT_ReadOnly (1L << 0)
#define FWL_STYLEEXT_EDT_MultiLine (1L << 1)
#define FWL_STYLEEXT_EDT_WantReturn (1L << 2)
#define FWL_STYLEEXT_EDT_AutoHScroll (1L << 4)
#define FWL_STYLEEXT_EDT_AutoVScroll (1L << 5)
#define FWL_STYLEEXT_EDT_Validate (1L << 7)
#define FWL_STYLEEXT_EDT_Password (1L << 8)
#define FWL_STYLEEXT_EDT_Number (1L << 9)
#define FWL_STYLEEXT_EDT_CombText (1L << 17)
#define FWL_STYLEEXT_EDT_HNear 0
#define FWL_STYLEEXT_EDT_HCenter (1L << 18)
#define FWL_STYLEEXT_EDT_HFar (2L << 18)
#define FWL_STYLEEXT_EDT_VNear 0
#define FWL_STYLEEXT_EDT_VCenter (1L << 20)
#define FWL_STYLEEXT_EDT_VFar (2L << 20)
#define FWL_STYLEEXT_EDT_Justified (1L << 22)
#define FWL_STYLEEXT_EDT_HAlignMask (3L << 18)
#define FWL_STYLEEXT_EDT_VAlignMask (3L << 20)
#define FWL_STYLEEXT_EDT_HAlignModeMask (3L << 22)
#define FWL_STYLEEXT_EDT_ShowScrollbarFocus (1L << 25)
#define FWL_STYLEEXT_EDT_OuterScrollbar (1L << 26)

class CFWL_Edit;
class CFWL_MessageMouse;
class CFWL_WidgetProperties;
class CFWL_Caret;

class CFWL_Edit : public CFWL_Widget, public CFDE_TextEditEngine::Delegate {
 public:
  CFWL_Edit(const CFWL_App* app,
            std::unique_ptr<CFWL_WidgetProperties> properties,
            CFWL_Widget* pOuter);
  ~CFWL_Edit() override;

  // CFWL_Widget:
  FWL_Type GetClassID() const override;
  CFX_RectF GetAutosizedWidgetRect() override;
  CFX_RectF GetWidgetRect() override;
  void Update() override;
  FWL_WidgetHit HitTest(const CFX_PointF& point) override;
  void SetStates(uint32_t dwStates) override;
  void DrawWidget(CXFA_Graphics* pGraphics, const CFX_Matrix& matrix) override;
  void SetThemeProvider(IFWL_ThemeProvider* pThemeProvider) override;
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CXFA_Graphics* pGraphics,
                    const CFX_Matrix& matrix) override;

  virtual void SetText(const WideString& wsText,
                       CFDE_TextEditEngine::RecordOperation op =
                           CFDE_TextEditEngine::RecordOperation::kInsertRecord);

  int32_t GetTextLength() const;
  WideString GetText() const;
  void ClearText();

  void SelectAll();
  void ClearSelection();
  bool HasSelection() const;
  // Returns <start, count> of the selection.
  std::pair<size_t, size_t> GetSelection() const;

  int32_t GetLimit() const;
  void SetLimit(int32_t nLimit);
  void SetAliasChar(wchar_t wAlias);
  Optional<WideString> Copy();
  Optional<WideString> Cut();
  bool Paste(const WideString& wsPaste);
  bool Undo();
  bool Redo();
  bool CanUndo();
  bool CanRedo();

  void SetOuter(CFWL_Widget* pOuter);

  // CFDE_TextEditEngine::Delegate
  void NotifyTextFull() override;
  void OnCaretChanged() override;
  void OnTextWillChange(CFDE_TextEditEngine::TextChange* change) override;
  void OnTextChanged() override;
  void OnSelChanged() override;
  bool OnValidate(const WideString& wsText) override;
  void SetScrollOffset(float fScrollOffset) override;

 protected:
  void ShowCaret(CFX_RectF* pRect);
  void HideCaret(CFX_RectF* pRect);
  const CFX_RectF& GetRTClient() const { return m_rtClient; }
  CFDE_TextEditEngine* GetTxtEdtEngine() { return &m_EdtEngine; }

 private:
  void RenderText(CFX_RenderDevice* pRenderDev,
                  const CFX_RectF& clipRect,
                  const CFX_Matrix& mt);
  void DrawTextBk(CXFA_Graphics* pGraphics,
                  IFWL_ThemeProvider* pTheme,
                  const CFX_Matrix* pMatrix);
  void DrawContent(CXFA_Graphics* pGraphics,
                   IFWL_ThemeProvider* pTheme,
                   const CFX_Matrix* pMatrix);

  void UpdateEditEngine();
  void UpdateEditParams();
  void UpdateEditLayout();
  bool UpdateOffset();
  bool UpdateOffset(CFWL_ScrollBar* pScrollBar, float fPosChanged);
  void UpdateVAlignment();
  void UpdateCaret();
  CFWL_ScrollBar* UpdateScroll();
  void Layout();
  void LayoutScrollBar();
  CFX_PointF DeviceToEngine(const CFX_PointF& pt);
  void InitVerticalScrollBar();
  void InitHorizontalScrollBar();
  void InitEngine();
  void InitCaret();
  bool ValidateNumberChar(wchar_t cNum);
  bool IsShowScrollBar(bool bVert);
  bool IsContentHeightOverflow();
  void SetCursorPosition(size_t position);
  void UpdateCursorRect();

  void DoRButtonDown(CFWL_MessageMouse* pMsg);
  void OnFocusChanged(CFWL_Message* pMsg, bool bSet);
  void OnLButtonDown(CFWL_MessageMouse* pMsg);
  void OnLButtonUp(CFWL_MessageMouse* pMsg);
  void OnButtonDoubleClick(CFWL_MessageMouse* pMsg);
  void OnMouseMove(CFWL_MessageMouse* pMsg);
  void OnKeyDown(CFWL_MessageKey* pMsg);
  void OnChar(CFWL_MessageKey* pMsg);
  bool OnScroll(CFWL_ScrollBar* pScrollBar,
                CFWL_EventScroll::Code dwCode,
                float fPos);

  CFX_RectF m_rtClient;
  CFX_RectF m_rtEngine;
  CFX_RectF m_rtStatic;
  CFX_RectF m_rtCaret;
  bool m_bLButtonDown = false;
  bool m_bSetRange = false;
  int32_t m_nLimit = -1;
  int32_t m_iMax = 0xFFFFFFF;
  float m_fVAlignOffset = 0.0f;
  float m_fScrollOffsetX = 0.0f;
  float m_fScrollOffsetY = 0.0f;
  float m_fFontSize = 0.0f;
  size_t m_CursorPosition = 0;
  CFDE_TextEditEngine m_EdtEngine;
  std::unique_ptr<CFWL_ScrollBar> m_pVertScrollBar;
  std::unique_ptr<CFWL_ScrollBar> m_pHorzScrollBar;
  std::unique_ptr<CFWL_Caret> m_pCaret;
  WideString m_wsCache;
  WideString m_wsFont;
};

#endif  // XFA_FWL_CFWL_EDIT_H_

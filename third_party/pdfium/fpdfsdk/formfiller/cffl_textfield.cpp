// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/formfiller/cffl_textfield.h"

#include <utility>

#include "fpdfsdk/cpdfsdk_common.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fpdfsdk/cpdfsdk_widget.h"
#include "fpdfsdk/formfiller/cba_fontmap.h"
#include "public/fpdf_fwlevent.h"
#include "third_party/base/ptr_util.h"

CFFL_TextField::CFFL_TextField(CPDFSDK_FormFillEnvironment* pApp,
                               CPDFSDK_Widget* pWidget)
    : CFFL_TextObject(pApp, pWidget) {}

CFFL_TextField::~CFFL_TextField() {
  for (const auto& it : m_Maps)
    it.second->InvalidateFocusHandler(this);

  // See comment in cffl_formfiller.h.
  // The font map should be stored somewhere more appropriate so it will live
  // until the PWL_Edit is done with it. pdfium:566
  DestroyWindows();
}

CPWL_Wnd::CreateParams CFFL_TextField::GetCreateParam() {
  CPWL_Wnd::CreateParams cp = CFFL_TextObject::GetCreateParam();
  int nFlags = m_pWidget->GetFieldFlags();
  if (nFlags & FIELDFLAG_PASSWORD)
    cp.dwFlags |= PES_PASSWORD;

  if (nFlags & FIELDFLAG_MULTILINE) {
    cp.dwFlags |= PES_MULTILINE | PES_AUTORETURN | PES_TOP;
    if (!(nFlags & FIELDFLAG_DONOTSCROLL))
      cp.dwFlags |= PWS_VSCROLL | PES_AUTOSCROLL;
  } else {
    cp.dwFlags |= PES_CENTER;
    if (!(nFlags & FIELDFLAG_DONOTSCROLL))
      cp.dwFlags |= PES_AUTOSCROLL;
  }

  if (nFlags & FIELDFLAG_COMB)
    cp.dwFlags |= PES_CHARARRAY;

  if (nFlags & FIELDFLAG_RICHTEXT)
    cp.dwFlags |= PES_RICH;

  cp.dwFlags |= PES_UNDO;

  switch (m_pWidget->GetAlignment()) {
    default:
    case BF_ALIGN_LEFT:
      cp.dwFlags |= PES_LEFT;
      break;
    case BF_ALIGN_MIDDLE:
      cp.dwFlags |= PES_MIDDLE;
      break;
    case BF_ALIGN_RIGHT:
      cp.dwFlags |= PES_RIGHT;
      break;
  }
  cp.pFontMap = MaybeCreateFontMap();
  cp.pFocusHandler = this;
  return cp;
}

std::unique_ptr<CPWL_Wnd> CFFL_TextField::NewPWLWindow(
    const CPWL_Wnd::CreateParams& cp,
    std::unique_ptr<CPWL_Wnd::PrivateData> pAttachedData) {
  auto pWnd = pdfium::MakeUnique<CPWL_Edit>(cp, std::move(pAttachedData));
  pWnd->AttachFFLData(this);
  pWnd->Realize();
  pWnd->SetFillerNotify(m_pFormFillEnv->GetInteractiveFormFiller());

  int32_t nMaxLen = m_pWidget->GetMaxLen();
  WideString swValue = m_pWidget->GetValue();
  if (nMaxLen > 0) {
    if (pWnd->HasFlag(PES_CHARARRAY)) {
      pWnd->SetCharArray(nMaxLen);
      pWnd->SetAlignFormatVerticalCenter();
    } else {
      pWnd->SetLimitChar(nMaxLen);
    }
  }
  pWnd->SetText(swValue);
  return std::move(pWnd);
}

bool CFFL_TextField::OnChar(CPDFSDK_Annot* pAnnot,
                            uint32_t nChar,
                            uint32_t nFlags) {
  switch (nChar) {
    case FWL_VKEY_Return: {
      if (m_pWidget->GetFieldFlags() & FIELDFLAG_MULTILINE)
        break;

      CPDFSDK_PageView* pPageView = GetCurPageView(true);
      ASSERT(pPageView);
      m_bValid = !m_bValid;
      m_pFormFillEnv->Invalidate(pAnnot->GetPage(),
                                 pAnnot->GetRect().GetOuterRect());

      if (m_bValid) {
        if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, true))
          pWnd->SetFocus();
        break;
      }

      if (!CommitData(pPageView, nFlags))
        return false;

      DestroyPDFWindow(pPageView);
      return true;
    }
    case FWL_VKEY_Escape: {
      CPDFSDK_PageView* pPageView = GetCurPageView(true);
      ASSERT(pPageView);
      EscapeFiller(pPageView, true);
      return true;
    }
  }

  return CFFL_TextObject::OnChar(pAnnot, nChar, nFlags);
}

bool CFFL_TextField::IsDataChanged(CPDFSDK_PageView* pPageView) {
  CPWL_Edit* pEdit = GetEdit(pPageView, false);
  return pEdit && pEdit->GetText() != m_pWidget->GetValue();
}

void CFFL_TextField::SaveData(CPDFSDK_PageView* pPageView) {
  CPWL_Edit* pWnd = GetEdit(pPageView, false);
  if (!pWnd)
    return;

  WideString sOldValue = m_pWidget->GetValue();
  WideString sNewValue = pWnd->GetText();
  CPDFSDK_Widget::ObservedPtr observed_widget(m_pWidget.Get());
  CFFL_TextField::ObservedPtr observed_this(this);
  m_pWidget->SetValue(sNewValue, NotificationOption::kDoNotNotify);
  if (!observed_widget)
    return;

  m_pWidget->ResetFieldAppearance(true);
  if (!observed_widget)
    return;

  m_pWidget->UpdateField();
  if (!observed_widget || !observed_this)
    return;

  SetChangeMark();
}

void CFFL_TextField::GetActionData(CPDFSDK_PageView* pPageView,
                                   CPDF_AAction::AActionType type,
                                   CPDFSDK_FieldAction& fa) {
  switch (type) {
    case CPDF_AAction::kKeyStroke:
      if (CPWL_Edit* pWnd = GetEdit(pPageView, false)) {
        fa.bFieldFull = pWnd->IsTextFull();

        fa.sValue = pWnd->GetText();

        if (fa.bFieldFull) {
          fa.sChange.clear();
          fa.sChangeEx.clear();
        }
      }
      break;
    case CPDF_AAction::kValidate:
      if (CPWL_Edit* pWnd = GetEdit(pPageView, false)) {
        fa.sValue = pWnd->GetText();
      }
      break;
    case CPDF_AAction::kLoseFocus:
    case CPDF_AAction::kGetFocus:
      fa.sValue = m_pWidget->GetValue();
      break;
    default:
      break;
  }
}

void CFFL_TextField::SetActionData(CPDFSDK_PageView* pPageView,
                                   CPDF_AAction::AActionType type,
                                   const CPDFSDK_FieldAction& fa) {
  switch (type) {
    case CPDF_AAction::kKeyStroke:
      if (CPWL_Edit* pEdit = GetEdit(pPageView, false)) {
        pEdit->SetFocus();
        pEdit->SetSelection(fa.nSelStart, fa.nSelEnd);
        pEdit->ReplaceSel(fa.sChange);
      }
      break;
    default:
      break;
  }
}

bool CFFL_TextField::IsActionDataChanged(CPDF_AAction::AActionType type,
                                         const CPDFSDK_FieldAction& faOld,
                                         const CPDFSDK_FieldAction& faNew) {
  switch (type) {
    case CPDF_AAction::kKeyStroke:
      return (!faOld.bFieldFull && faOld.nSelEnd != faNew.nSelEnd) ||
             faOld.nSelStart != faNew.nSelStart ||
             faOld.sChange != faNew.sChange;
    default:
      break;
  }

  return false;
}

void CFFL_TextField::SaveState(CPDFSDK_PageView* pPageView) {
  ASSERT(pPageView);

  CPWL_Edit* pWnd = GetEdit(pPageView, false);
  if (!pWnd)
    return;

  pWnd->GetSelection(m_State.nStart, m_State.nEnd);
  m_State.sValue = pWnd->GetText();
}

void CFFL_TextField::RestoreState(CPDFSDK_PageView* pPageView) {
  ASSERT(pPageView);

  CPWL_Edit* pWnd = GetEdit(pPageView, true);
  if (!pWnd)
    return;

  pWnd->SetText(m_State.sValue);
  pWnd->SetSelection(m_State.nStart, m_State.nEnd);
}

#ifdef PDF_ENABLE_XFA
bool CFFL_TextField::IsFieldFull(CPDFSDK_PageView* pPageView) {
  CPWL_Edit* pWnd = GetEdit(pPageView, false);
  return pWnd && pWnd->IsTextFull();
}
#endif  // PDF_ENABLE_XFA

void CFFL_TextField::OnSetFocus(CPWL_Edit* pEdit) {
  pEdit->SetCharSet(FX_CHARSET_ChineseSimplified);
  pEdit->SetReadyToInput();

  WideString wsText = pEdit->GetText();
  int nCharacters = wsText.GetLength();
  ByteString bsUTFText = wsText.ToUTF16LE();
  auto* pBuffer = reinterpret_cast<const unsigned short*>(bsUTFText.c_str());
  m_pFormFillEnv->OnSetFieldInputFocus(pBuffer, nCharacters, true);
}

CPWL_Edit* CFFL_TextField::GetEdit(CPDFSDK_PageView* pPageView, bool bNew) {
  return static_cast<CPWL_Edit*>(GetPDFWindow(pPageView, bNew));
}

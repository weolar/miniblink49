// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FORMFILLER_CFFL_COMBOBOX_H_
#define FPDFSDK_FORMFILLER_CFFL_COMBOBOX_H_

#include <memory>

#include "core/fxcrt/fx_string.h"
#include "fpdfsdk/formfiller/cffl_textobject.h"

class CBA_FontMap;

struct FFL_ComboBoxState {
  int nIndex;
  int nStart;
  int nEnd;
  WideString sValue;
};

class CFFL_ComboBox final : public CFFL_TextObject,
                            public CPWL_Wnd::FocusHandlerIface {
 public:
  CFFL_ComboBox(CPDFSDK_FormFillEnvironment* pApp, CPDFSDK_Widget* pWidget);
  ~CFFL_ComboBox() override;

  // CFFL_TextObject:
  CPWL_Wnd::CreateParams GetCreateParam() override;
  std::unique_ptr<CPWL_Wnd> NewPWLWindow(
      const CPWL_Wnd::CreateParams& cp,
      std::unique_ptr<CPWL_Wnd::PrivateData> pAttachedData) override;
  bool OnChar(CPDFSDK_Annot* pAnnot, uint32_t nChar, uint32_t nFlags) override;
  bool IsDataChanged(CPDFSDK_PageView* pPageView) override;
  void SaveData(CPDFSDK_PageView* pPageView) override;
  void GetActionData(CPDFSDK_PageView* pPageView,
                     CPDF_AAction::AActionType type,
                     CPDFSDK_FieldAction& fa) override;
  void SetActionData(CPDFSDK_PageView* pPageView,
                     CPDF_AAction::AActionType type,
                     const CPDFSDK_FieldAction& fa) override;
  bool IsActionDataChanged(CPDF_AAction::AActionType type,
                           const CPDFSDK_FieldAction& faOld,
                           const CPDFSDK_FieldAction& faNew) override;
  void SaveState(CPDFSDK_PageView* pPageView) override;
  void RestoreState(CPDFSDK_PageView* pPageView) override;
#ifdef PDF_ENABLE_XFA
  bool IsFieldFull(CPDFSDK_PageView* pPageView) override;
#endif

  // CPWL_Wnd::FocusHandlerIface:
  void OnSetFocus(CPWL_Edit* pEdit) override;

 private:
  WideString GetSelectExportText();

  FFL_ComboBoxState m_State;
};

#endif  // FPDFSDK_FORMFILLER_CFFL_COMBOBOX_H_

// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_CPDFSDK_WIDGET_H_
#define FPDFSDK_CPDFSDK_WIDGET_H_

#include "core/fpdfdoc/cpdf_aaction.h"
#include "core/fpdfdoc/cpdf_action.h"
#include "core/fpdfdoc/cpdf_annot.h"
#include "core/fpdfdoc/cpdf_formfield.h"
#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/unowned_ptr.h"
#include "core/fxge/cfx_color.h"
#include "fpdfsdk/cpdfsdk_baannot.h"
#include "fpdfsdk/cpdfsdk_fieldaction.h"
#include "third_party/base/optional.h"

class CFX_RenderDevice;
class CPDF_Annot;
class CPDF_Dictionary;
class CPDF_FormControl;
class CPDF_FormField;
class CPDF_RenderOptions;
class CPDF_Stream;
class CPDFSDK_InteractiveForm;
class CPDFSDK_PageView;

#ifdef PDF_ENABLE_XFA
class CXFA_FFWidget;
class CXFA_FFWidgetHandler;
#endif  // PDF_ENABLE_XFA

class CPDFSDK_Widget final : public CPDFSDK_BAAnnot {
 public:
#ifdef PDF_ENABLE_XFA
  CXFA_FFWidget* GetMixXFAWidget() const;
  CXFA_FFWidgetHandler* GetXFAWidgetHandler() const;

  bool HasXFAAAction(PDFSDK_XFAAActionType eXFAAAT) const;
  bool OnXFAAAction(PDFSDK_XFAAActionType eXFAAAT,
                    CPDFSDK_FieldAction* data,
                    CPDFSDK_PageView* pPageView);

  void Synchronize(bool bSynchronizeElse);
#endif  // PDF_ENABLE_XFA

  CPDFSDK_Widget(CPDF_Annot* pAnnot,
                 CPDFSDK_PageView* pPageView,
                 CPDFSDK_InteractiveForm* pInteractiveForm);
  ~CPDFSDK_Widget() override;

  bool IsSignatureWidget() const override;
  CPDF_Action GetAAction(CPDF_AAction::AActionType eAAT) override;
  bool IsAppearanceValid() override;

  int GetLayoutOrder() const override;

  FormFieldType GetFieldType() const;
  int GetFieldFlags() const;
  int GetRotate() const;

  Optional<FX_COLORREF> GetFillColor() const;
  Optional<FX_COLORREF> GetBorderColor() const;
  Optional<FX_COLORREF> GetTextColor() const;
  float GetFontSize() const;

  int GetSelectedIndex(int nIndex) const;
  WideString GetValue() const;
  WideString GetDefaultValue() const;
  WideString GetOptionLabel(int nIndex) const;
  int CountOptions() const;
  bool IsOptionSelected(int nIndex) const;
  int GetTopVisibleIndex() const;
  bool IsChecked() const;
  int GetAlignment() const;
  int GetMaxLen() const;
  WideString GetAlternateName() const;

  void SetCheck(bool bChecked, NotificationOption notify);
  void SetValue(const WideString& sValue, NotificationOption notify);
  void SetOptionSelection(int index, bool bSelected, NotificationOption notify);
  void ClearSelection(NotificationOption notify);
  void SetTopVisibleIndex(int index);

#ifdef PDF_ENABLE_XFA
  void ResetAppearance(bool bValueChanged);
#endif  // PDF_ENABLE_XFA
  void ResetAppearance(Optional<WideString> sValue, bool bValueChanged);
  void ResetFieldAppearance(bool bValueChanged);
  void UpdateField();
  Optional<WideString> OnFormat();

  bool OnAAction(CPDF_AAction::AActionType type,
                 CPDFSDK_FieldAction* data,
                 CPDFSDK_PageView* pPageView);

  CPDFSDK_InteractiveForm* GetInteractiveForm() const {
    return m_pInteractiveForm.Get();
  }
  CPDF_FormField* GetFormField() const;
  CPDF_FormControl* GetFormControl() const;

  void DrawShadow(CFX_RenderDevice* pDevice, CPDFSDK_PageView* pPageView);

  void SetAppModified();
  void ClearAppModified();
  bool IsAppModified() const;

  uint32_t GetAppearanceAge() const { return m_nAppearanceAge; }
  uint32_t GetValueAge() const { return m_nValueAge; }

  bool IsWidgetAppearanceValid(CPDF_Annot::AppearanceMode mode);
  void DrawAppearance(CFX_RenderDevice* pDevice,
                      const CFX_Matrix& mtUser2Device,
                      CPDF_Annot::AppearanceMode mode,
                      const CPDF_RenderOptions* pOptions) override;

  CFX_Matrix GetMatrix() const;
  CFX_FloatRect GetClientRect() const;
  CFX_FloatRect GetRotatedRect() const;
  CFX_Color GetTextPWLColor() const;
  CFX_Color GetBorderPWLColor() const;
  CFX_Color GetFillPWLColor() const;

 private:
#ifdef PDF_ENABLE_XFA
  CXFA_FFWidget* GetGroupMixXFAWidget() const;
  WideString GetName() const;
#endif  // PDF_ENABLE_XFA

  UnownedPtr<CPDFSDK_InteractiveForm> const m_pInteractiveForm;
  bool m_bAppModified = false;
  uint32_t m_nAppearanceAge = 0;
  uint32_t m_nValueAge = 0;

#ifdef PDF_ENABLE_XFA
  mutable UnownedPtr<CXFA_FFWidgetHandler> m_pWidgetHandler;
#endif  // PDF_ENABLE_XFA
};

inline CPDFSDK_Widget* ToCPDFSDKWidget(CPDFSDK_Annot* pAnnot) {
  return pAnnot && pAnnot->GetAnnotSubtype() == CPDF_Annot::Subtype::WIDGET
             ? static_cast<CPDFSDK_Widget*>(pAnnot)
             : nullptr;
}

#endif  // FPDFSDK_CPDFSDK_WIDGET_H_

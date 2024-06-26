// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_CPDFSDK_INTERACTIVEFORM_H_
#define FPDFSDK_CPDFSDK_INTERACTIVEFORM_H_

#include <map>
#include <memory>
#include <vector>

#include "core/fpdfdoc/cpdf_action.h"
#include "core/fpdfdoc/ipdf_formnotify.h"
#include "core/fxcrt/unowned_ptr.h"
#include "core/fxge/fx_dib.h"
#include "fpdfsdk/cpdfsdk_widget.h"
#include "third_party/base/optional.h"

class CPDF_Dictionary;
class CPDF_FormControl;
class CPDF_FormField;
class CPDF_InteractiveForm;
class CPDF_Object;
class CPDFSDK_FormFillEnvironment;

#ifdef PDF_ENABLE_XFA
class CPDFSDK_XFAWidget;
class CXFA_FFWidget;
#endif  // PDF_ENABLE_XFA

class CPDFSDK_InteractiveForm final : public IPDF_FormNotify {
 public:
  explicit CPDFSDK_InteractiveForm(CPDFSDK_FormFillEnvironment* pFormFillEnv);
  ~CPDFSDK_InteractiveForm() override;

  CPDF_InteractiveForm* GetInteractiveForm() const {
    return m_pInteractiveForm.get();
  }
  CPDFSDK_FormFillEnvironment* GetFormFillEnv() const {
    return m_pFormFillEnv.Get();
  }

  CPDFSDK_Widget* GetSibling(CPDFSDK_Widget* pWidget, bool bNext) const;
  CPDFSDK_Widget* GetWidget(CPDF_FormControl* pControl) const;
  void GetWidgets(const WideString& sFieldName,
                  std::vector<CPDFSDK_Annot::ObservedPtr>* widgets) const;
  void GetWidgets(CPDF_FormField* pField,
                  std::vector<CPDFSDK_Annot::ObservedPtr>* widgets) const;

  void AddMap(CPDF_FormControl* pControl, CPDFSDK_Widget* pWidget);
  void RemoveMap(CPDF_FormControl* pControl);

  void EnableCalculate(bool bEnabled);
  bool IsCalculateEnabled() const;

#ifdef PDF_ENABLE_XFA
  void AddXFAMap(CXFA_FFWidget* hWidget, CPDFSDK_XFAWidget* pWidget);
  void RemoveXFAMap(CXFA_FFWidget* hWidget);
  CPDFSDK_XFAWidget* GetXFAWidget(CXFA_FFWidget* hWidget);
  void XfaEnableCalculate(bool bEnabled);
  bool IsXfaCalculateEnabled() const;
  bool IsXfaValidationsEnabled();
  void XfaSetValidationsEnabled(bool bEnabled);
  void SynchronizeField(CPDF_FormField* pFormField);
#endif  // PDF_ENABLE_XFA

  bool OnKeyStrokeCommit(CPDF_FormField* pFormField, const WideString& csValue);
  bool OnValidate(CPDF_FormField* pFormField, const WideString& csValue);
  void OnCalculate(CPDF_FormField* pFormField);
  Optional<WideString> OnFormat(CPDF_FormField* pFormField);

  void ResetFieldAppearance(CPDF_FormField* pFormField,
                            Optional<WideString> sValue,
                            bool bValueChanged);
  void UpdateField(CPDF_FormField* pFormField);

  bool DoAction_Hide(const CPDF_Action& action);
  bool DoAction_SubmitForm(const CPDF_Action& action);
  void DoAction_ResetForm(const CPDF_Action& action);

  std::vector<CPDF_FormField*> GetFieldFromObjects(
      const std::vector<const CPDF_Object*>& objects) const;
  bool IsValidField(CPDF_Dictionary* pFieldDict);
  bool SubmitFields(const WideString& csDestination,
                    const std::vector<CPDF_FormField*>& fields,
                    bool bIncludeOrExclude,
                    bool bUrlEncoded);
  bool SubmitForm(const WideString& sDestination, bool bUrlEncoded);
  ByteString ExportFormToFDFTextBuf();
  ByteString ExportFieldsToFDFTextBuf(
      const std::vector<CPDF_FormField*>& fields,
      bool bIncludeOrExclude);

  bool IsNeedHighLight(FormFieldType fieldType) const;
  void RemoveAllHighLights();
  void SetHighlightAlpha(uint8_t alpha) { m_HighlightAlpha = alpha; }
  uint8_t GetHighlightAlpha() { return m_HighlightAlpha; }
  void SetHighlightColor(FX_COLORREF clr, FormFieldType fieldType);
  void SetAllHighlightColors(FX_COLORREF clr);
  FX_COLORREF GetHighlightColor(FormFieldType fieldType);

 private:
  // IPDF_FormNotify:
  bool BeforeValueChange(CPDF_FormField* pField,
                         const WideString& csValue) override;
  void AfterValueChange(CPDF_FormField* pField) override;
  bool BeforeSelectionChange(CPDF_FormField* pField,
                             const WideString& csValue) override;
  void AfterSelectionChange(CPDF_FormField* pField) override;
  void AfterCheckedStatusChange(CPDF_FormField* pField) override;
  void AfterFormReset(CPDF_InteractiveForm* pForm) override;

  int GetPageIndexByAnnotDict(CPDF_Document* pDocument,
                              CPDF_Dictionary* pAnnotDict) const;

  UnownedPtr<CPDFSDK_FormFillEnvironment> const m_pFormFillEnv;
  std::unique_ptr<CPDF_InteractiveForm> const m_pInteractiveForm;
  std::map<CPDF_FormControl*, CPDFSDK_Widget*> m_Map;
#ifdef PDF_ENABLE_XFA
  std::map<CXFA_FFWidget*, CPDFSDK_XFAWidget*> m_XFAMap;
  bool m_bXfaCalculate = true;
  bool m_bXfaValidationsEnabled = true;
#endif  // PDF_ENABLE_XFA
  bool m_bCalculate = true;
  bool m_bBusy = false;

  uint8_t m_HighlightAlpha = 0;
  FX_COLORREF m_HighlightColor[kFormFieldTypeCount];
  bool m_NeedsHighlight[kFormFieldTypeCount];
};

#endif  // FPDFSDK_CPDFSDK_INTERACTIVEFORM_H_

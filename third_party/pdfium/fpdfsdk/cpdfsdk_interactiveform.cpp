// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/cpdfsdk_interactiveform.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/parser/cfdf_document.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfdoc/cpdf_actionfields.h"
#include "core/fpdfdoc/cpdf_interactiveform.h"
#include "core/fxcrt/autorestorer.h"
#include "core/fxge/cfx_graphstatedata.h"
#include "core/fxge/cfx_pathdata.h"
#include "fpdfsdk/cpdfsdk_actionhandler.h"
#include "fpdfsdk/cpdfsdk_annot.h"
#include "fpdfsdk/cpdfsdk_annotiterator.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "fpdfsdk/cpdfsdk_pageview.h"
#include "fpdfsdk/cpdfsdk_widget.h"
#include "fpdfsdk/formfiller/cffl_formfiller.h"
#include "fpdfsdk/ipdfsdk_annothandler.h"
// #include "fxjs/ijs_event_context.h"
// #include "fxjs/ijs_runtime.h"
#include "third_party/base/ptr_util.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/cpdfsdk_xfawidget.h"
#include "fpdfsdk/fpdfxfa/cpdfxfa_context.h"
#include "fpdfsdk/fpdfxfa/cxfa_fwladaptertimermgr.h"
#include "xfa/fxfa/cxfa_eventparam.h"
#include "xfa/fxfa/cxfa_ffdocview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/cxfa_ffwidgethandler.h"
#endif  // PDF_ENABLE_XFA

namespace {

constexpr uint32_t kWhiteBGR = FXSYS_BGR(255, 255, 255);

bool IsFormFieldTypeComboOrText(FormFieldType fieldType) {
  switch (fieldType) {
    case FormFieldType::kComboBox:
    case FormFieldType::kTextField:
      return true;
    default:
      return false;
  }
}

#ifdef PDF_ENABLE_XFA
bool IsFormFieldTypeXFA(FormFieldType fieldType) {
  switch (fieldType) {
    case FormFieldType::kXFA:
    case FormFieldType::kXFA_CheckBox:
    case FormFieldType::kXFA_ComboBox:
    case FormFieldType::kXFA_ImageField:
    case FormFieldType::kXFA_ListBox:
    case FormFieldType::kXFA_PushButton:
    case FormFieldType::kXFA_Signature:
    case FormFieldType::kXFA_TextField:
      return true;
    default:
      return false;
  }
}
#endif  // PDF_ENABLE_XFA

bool FDFToURLEncodedData(std::vector<uint8_t>* pBuffer) {
  std::unique_ptr<CFDF_Document> pFDF = CFDF_Document::ParseMemory(*pBuffer);
  if (!pFDF)
    return true;

  CPDF_Dictionary* pMainDict = pFDF->GetRoot()->GetDictFor("FDF");
  if (!pMainDict)
    return false;

  CPDF_Array* pFields = pMainDict->GetArrayFor("Fields");
  if (!pFields)
    return false;

  std::ostringstream fdfEncodedData;
  for (uint32_t i = 0; i < pFields->size(); i++) {
    CPDF_Dictionary* pField = pFields->GetDictAt(i);
    if (!pField)
      continue;
    WideString name;
    name = pField->GetUnicodeTextFor("T");
    ByteString name_b = name.ToDefANSI();
    ByteString csBValue = pField->GetStringFor("V");
    WideString csWValue = PDF_DecodeText(csBValue.AsRawSpan());
    ByteString csValue_b = csWValue.ToDefANSI();
    fdfEncodedData << name_b << "=" << csValue_b;
    if (i != pFields->size() - 1)
      fdfEncodedData << "&";
  }

  size_t nBufSize = fdfEncodedData.tellp();
  if (nBufSize <= 0)
    return false;

  pBuffer->resize(nBufSize);
  memcpy(pBuffer->data(), fdfEncodedData.str().c_str(), nBufSize);
  return true;
}

}  // namespace

CPDFSDK_InteractiveForm::CPDFSDK_InteractiveForm(
    CPDFSDK_FormFillEnvironment* pFormFillEnv)
    : m_pFormFillEnv(pFormFillEnv),
      m_pInteractiveForm(pdfium::MakeUnique<CPDF_InteractiveForm>(
          m_pFormFillEnv->GetPDFDocument())) {
  ASSERT(m_pFormFillEnv);
  m_pInteractiveForm->SetFormNotify(this);
  RemoveAllHighLights();
}

CPDFSDK_InteractiveForm::~CPDFSDK_InteractiveForm() {
  m_Map.clear();
#ifdef PDF_ENABLE_XFA
  m_XFAMap.clear();
#endif  // PDF_ENABLE_XFA
}

CPDFSDK_Widget* CPDFSDK_InteractiveForm::GetSibling(CPDFSDK_Widget* pWidget,
                                                    bool bNext) const {
  auto pIterator = pdfium::MakeUnique<CPDFSDK_AnnotIterator>(
      pWidget->GetPageView(), CPDF_Annot::Subtype::WIDGET);

  return ToCPDFSDKWidget(bNext ? pIterator->GetNextAnnot(pWidget)
                               : pIterator->GetPrevAnnot(pWidget));
}

CPDFSDK_Widget* CPDFSDK_InteractiveForm::GetWidget(
    CPDF_FormControl* pControl) const {
  if (!pControl)
    return nullptr;

  CPDFSDK_Widget* pWidget = nullptr;
  const auto it = m_Map.find(pControl);
  if (it != m_Map.end())
    pWidget = it->second;
  if (pWidget)
    return pWidget;

  CPDF_Dictionary* pControlDict = pControl->GetWidget();
  CPDF_Document* pDocument = m_pFormFillEnv->GetPDFDocument();
  CPDFSDK_PageView* pPage = nullptr;

  if (CPDF_Dictionary* pPageDict = pControlDict->GetDictFor("P")) {
    int nPageIndex = pDocument->GetPageIndex(pPageDict->GetObjNum());
    if (nPageIndex >= 0)
      pPage = m_pFormFillEnv->GetPageView(nPageIndex);
  }

  if (!pPage) {
    int nPageIndex = GetPageIndexByAnnotDict(pDocument, pControlDict);
    if (nPageIndex >= 0)
      pPage = m_pFormFillEnv->GetPageView(nPageIndex);
  }

  return pPage ? ToCPDFSDKWidget(pPage->GetAnnotByDict(pControlDict)) : nullptr;
}

void CPDFSDK_InteractiveForm::GetWidgets(
    const WideString& sFieldName,
    std::vector<CPDFSDK_Annot::ObservedPtr>* widgets) const {
  for (int i = 0, sz = m_pInteractiveForm->CountFields(sFieldName); i < sz;
       ++i) {
    CPDF_FormField* pFormField = m_pInteractiveForm->GetField(i, sFieldName);
    ASSERT(pFormField);
    GetWidgets(pFormField, widgets);
  }
}

void CPDFSDK_InteractiveForm::GetWidgets(
    CPDF_FormField* pField,
    std::vector<CPDFSDK_Annot::ObservedPtr>* widgets) const {
  for (int i = 0, sz = pField->CountControls(); i < sz; ++i) {
    CPDF_FormControl* pFormCtrl = pField->GetControl(i);
    ASSERT(pFormCtrl);
    CPDFSDK_Widget* pWidget = GetWidget(pFormCtrl);
    if (pWidget)
      widgets->emplace_back(pWidget);
  }
}

int CPDFSDK_InteractiveForm::GetPageIndexByAnnotDict(
    CPDF_Document* pDocument,
    CPDF_Dictionary* pAnnotDict) const {
  ASSERT(pAnnotDict);

  for (int i = 0, sz = pDocument->GetPageCount(); i < sz; i++) {
    if (CPDF_Dictionary* pPageDict = pDocument->GetPageDictionary(i)) {
      if (CPDF_Array* pAnnots = pPageDict->GetArrayFor("Annots")) {
        for (int j = 0, jsz = pAnnots->size(); j < jsz; j++) {
          CPDF_Object* pDict = pAnnots->GetDirectObjectAt(j);
          if (pAnnotDict == pDict)
            return i;
        }
      }
    }
  }

  return -1;
}

void CPDFSDK_InteractiveForm::AddMap(CPDF_FormControl* pControl,
                                     CPDFSDK_Widget* pWidget) {
  m_Map[pControl] = pWidget;
}

void CPDFSDK_InteractiveForm::RemoveMap(CPDF_FormControl* pControl) {
  m_Map.erase(pControl);
}

void CPDFSDK_InteractiveForm::EnableCalculate(bool bEnabled) {
  m_bCalculate = bEnabled;
}

bool CPDFSDK_InteractiveForm::IsCalculateEnabled() const {
  return m_bCalculate;
}

#ifdef PDF_ENABLE_XFA
void CPDFSDK_InteractiveForm::AddXFAMap(CXFA_FFWidget* hWidget,
                                        CPDFSDK_XFAWidget* pWidget) {
  ASSERT(hWidget);
  m_XFAMap[hWidget] = pWidget;
}

void CPDFSDK_InteractiveForm::RemoveXFAMap(CXFA_FFWidget* hWidget) {
  ASSERT(hWidget);
  m_XFAMap.erase(hWidget);
}

CPDFSDK_XFAWidget* CPDFSDK_InteractiveForm::GetXFAWidget(
    CXFA_FFWidget* hWidget) {
  ASSERT(hWidget);
  auto it = m_XFAMap.find(hWidget);
  return it != m_XFAMap.end() ? it->second : nullptr;
}

void CPDFSDK_InteractiveForm::XfaEnableCalculate(bool bEnabled) {
  m_bXfaCalculate = bEnabled;
}
bool CPDFSDK_InteractiveForm::IsXfaCalculateEnabled() const {
  return m_bXfaCalculate;
}

bool CPDFSDK_InteractiveForm::IsXfaValidationsEnabled() {
  return m_bXfaValidationsEnabled;
}
void CPDFSDK_InteractiveForm::XfaSetValidationsEnabled(bool bEnabled) {
  m_bXfaValidationsEnabled = bEnabled;
}

void CPDFSDK_InteractiveForm::SynchronizeField(CPDF_FormField* pFormField) {
//   for (int i = 0, sz = pFormField->CountControls(); i < sz; i++) {
//     CPDF_FormControl* pFormCtrl = pFormField->GetControl(i);
//     if (CPDFSDK_Widget* pWidget = GetWidget(pFormCtrl))
//       pWidget->Synchronize(false);
//   }
  DebugBreak();
}
#endif  // PDF_ENABLE_XFA

void CPDFSDK_InteractiveForm::OnCalculate(CPDF_FormField* pFormField) {
//   if (!m_pFormFillEnv->IsJSPlatformPresent())
//     return;
// 
//   if (m_bBusy)
//     return;
// 
//   AutoRestorer<bool> restorer(&m_bBusy);
//   m_bBusy = true;
// 
//   if (!IsCalculateEnabled())
//     return;
// 
//   IJS_Runtime* pRuntime = m_pFormFillEnv->GetIJSRuntime();
//   int nSize = m_pInteractiveForm->CountFieldsInCalculationOrder();
//   for (int i = 0; i < nSize; i++) {
//     CPDF_FormField* pField = m_pInteractiveForm->GetFieldInCalculationOrder(i);
//     if (!pField)
//       continue;
// 
//     FormFieldType fieldType = pField->GetFieldType();
//     if (!IsFormFieldTypeComboOrText(fieldType))
//       continue;
// 
//     CPDF_AAction aAction = pField->GetAdditionalAction();
//     if (!aAction.GetDict() || !aAction.ActionExist(CPDF_AAction::kCalculate))
//       continue;
// 
//     CPDF_Action action = aAction.GetAction(CPDF_AAction::kCalculate);
//     if (!action.GetDict())
//       continue;
// 
//     WideString csJS = action.GetJavaScript();
//     if (csJS.IsEmpty())
//       continue;
// 
//     WideString sOldValue = pField->GetValue();
//     WideString sValue = sOldValue;
//     bool bRC = true;
//     IJS_Runtime::ScopedEventContext pContext(pRuntime);
//     pContext->OnField_Calculate(pFormField, pField, &sValue, &bRC);
// 
//     Optional<IJS_Runtime::JS_Error> err = pContext->RunScript(csJS);
//     if (!err && bRC && sValue.Compare(sOldValue) != 0)
//       pField->SetValue(sValue, NotificationOption::kNotify);
//   }
  DebugBreak(); // weolar
}

Optional<WideString> CPDFSDK_InteractiveForm::OnFormat(
    CPDF_FormField* pFormField) {
//   if (!m_pFormFillEnv->IsJSPlatformPresent())
//     return {};
// 
//   WideString sValue = pFormField->GetValue();
//   IJS_Runtime* pRuntime = m_pFormFillEnv->GetIJSRuntime();
//   if (pFormField->GetFieldType() == FormFieldType::kComboBox &&
//       pFormField->CountSelectedItems() > 0) {
//     int index = pFormField->GetSelectedIndex(0);
//     if (index >= 0)
//       sValue = pFormField->GetOptionLabel(index);
//   }
// 
//   CPDF_AAction aAction = pFormField->GetAdditionalAction();
//   if (aAction.GetDict() && aAction.ActionExist(CPDF_AAction::kFormat)) {
//     CPDF_Action action = aAction.GetAction(CPDF_AAction::kFormat);
//     if (action.GetDict()) {
//       WideString script = action.GetJavaScript();
//       if (!script.IsEmpty()) {
//         IJS_Runtime::ScopedEventContext pContext(pRuntime);
//         pContext->OnField_Format(pFormField, &sValue, true);
//         Optional<IJS_Runtime::JS_Error> err = pContext->RunScript(script);
//         if (!err)
//           return sValue;
//       }
//     }
//   }
  DebugBreak(); // weolar
  return {};
}

void CPDFSDK_InteractiveForm::ResetFieldAppearance(CPDF_FormField* pFormField,
                                                   Optional<WideString> sValue,
                                                   bool bValueChanged) {
//   for (int i = 0, sz = pFormField->CountControls(); i < sz; i++) {
//     CPDF_FormControl* pFormCtrl = pFormField->GetControl(i);
//     ASSERT(pFormCtrl);
//     if (CPDFSDK_Widget* pWidget = GetWidget(pFormCtrl))
//       pWidget->ResetAppearance(sValue, bValueChanged);
//   }
  DebugBreak();
}

void CPDFSDK_InteractiveForm::UpdateField(CPDF_FormField* pFormField) {
//   auto* formfiller = m_pFormFillEnv->GetInteractiveFormFiller();
//   for (int i = 0, sz = pFormField->CountControls(); i < sz; i++) {
//     CPDF_FormControl* pFormCtrl = pFormField->GetControl(i);
//     ASSERT(pFormCtrl);
// 
//     CPDFSDK_Widget* pWidget = GetWidget(pFormCtrl);
//     if (!pWidget)
//       continue;
// 
//     IPDF_Page* pPage = pWidget->GetPage();
//     FX_RECT rect = formfiller->GetViewBBox(
//         m_pFormFillEnv->GetPageView(pPage, false), pWidget);
//     m_pFormFillEnv->Invalidate(pPage, rect);
//   }
  DebugBreak();
}

bool CPDFSDK_InteractiveForm::OnKeyStrokeCommit(CPDF_FormField* pFormField,
                                                const WideString& csValue) {
  CPDF_AAction aAction = pFormField->GetAdditionalAction();
  if (!aAction.GetDict() || !aAction.ActionExist(CPDF_AAction::kKeyStroke))
    return true;

  CPDF_Action action = aAction.GetAction(CPDF_AAction::kKeyStroke);
  if (!action.GetDict())
    return true;

  CPDFSDK_FieldAction fa;
  fa.bModifier = false;
  fa.bShift = false;
  fa.sValue = csValue;
  m_pFormFillEnv->GetActionHandler()->DoAction_FieldJavaScript(
      action, CPDF_AAction::kKeyStroke, m_pFormFillEnv.Get(), pFormField, &fa);
  return fa.bRC;
}

bool CPDFSDK_InteractiveForm::OnValidate(CPDF_FormField* pFormField,
                                         const WideString& csValue) {
  CPDF_AAction aAction = pFormField->GetAdditionalAction();
  if (!aAction.GetDict() || !aAction.ActionExist(CPDF_AAction::kValidate))
    return true;

  CPDF_Action action = aAction.GetAction(CPDF_AAction::kValidate);
  if (!action.GetDict())
    return true;

  CPDFSDK_FieldAction fa;
  fa.bModifier = false;
  fa.bShift = false;
  fa.sValue = csValue;
  m_pFormFillEnv->GetActionHandler()->DoAction_FieldJavaScript(
      action, CPDF_AAction::kValidate, m_pFormFillEnv.Get(), pFormField, &fa);
  return fa.bRC;
}

bool CPDFSDK_InteractiveForm::DoAction_Hide(const CPDF_Action& action) {
  ASSERT(action.GetDict());

  CPDF_ActionFields af(&action);
  std::vector<const CPDF_Object*> fieldObjects = af.GetAllFields();
  std::vector<CPDF_FormField*> fields = GetFieldFromObjects(fieldObjects);

  bool bHide = action.GetHideStatus();
  bool bChanged = false;

  for (CPDF_FormField* pField : fields) {
    for (int i = 0, sz = pField->CountControls(); i < sz; ++i) {
      CPDF_FormControl* pControl = pField->GetControl(i);
      ASSERT(pControl);

      if (CPDFSDK_Widget* pWidget = GetWidget(pControl)) {
        uint32_t nFlags = pWidget->GetFlags();
        nFlags &= ~ANNOTFLAG_INVISIBLE;
        nFlags &= ~ANNOTFLAG_NOVIEW;
        if (bHide)
          nFlags |= ANNOTFLAG_HIDDEN;
        else
          nFlags &= ~ANNOTFLAG_HIDDEN;
        pWidget->SetFlags(nFlags);
        pWidget->GetPageView()->UpdateView(pWidget);
        bChanged = true;
      }
    }
  }

  return bChanged;
}

bool CPDFSDK_InteractiveForm::DoAction_SubmitForm(const CPDF_Action& action) {
  WideString sDestination = action.GetFilePath();
  if (sDestination.IsEmpty())
    return false;

  const CPDF_Dictionary* pActionDict = action.GetDict();
  if (pActionDict->KeyExist("Fields")) {
    CPDF_ActionFields af(&action);
    uint32_t dwFlags = action.GetFlags();
    std::vector<const CPDF_Object*> fieldObjects = af.GetAllFields();
    std::vector<CPDF_FormField*> fields = GetFieldFromObjects(fieldObjects);
    if (!fields.empty()) {
      bool bIncludeOrExclude = !(dwFlags & 0x01);
      if (!m_pInteractiveForm->CheckRequiredFields(&fields, bIncludeOrExclude))
        return false;

      return SubmitFields(sDestination, fields, bIncludeOrExclude, false);
    }
  }
  if (!m_pInteractiveForm->CheckRequiredFields(nullptr, true))
    return false;

  return SubmitForm(sDestination, false);
}

bool CPDFSDK_InteractiveForm::SubmitFields(
    const WideString& csDestination,
    const std::vector<CPDF_FormField*>& fields,
    bool bIncludeOrExclude,
    bool bUrlEncoded) {
  ByteString textBuf = ExportFieldsToFDFTextBuf(fields, bIncludeOrExclude);
  if (textBuf.IsEmpty())
    return false;

  std::vector<uint8_t> buffer(textBuf.begin(), textBuf.end());
  if (bUrlEncoded && !FDFToURLEncodedData(&buffer))
    return false;

  m_pFormFillEnv->JS_docSubmitForm(buffer.data(), buffer.size(), csDestination);
  return true;
}

ByteString CPDFSDK_InteractiveForm::ExportFieldsToFDFTextBuf(
    const std::vector<CPDF_FormField*>& fields,
    bool bIncludeOrExclude) {
  std::unique_ptr<CFDF_Document> pFDF = m_pInteractiveForm->ExportToFDF(
      m_pFormFillEnv->JS_docGetFilePath(), fields, bIncludeOrExclude, false);

  return pFDF ? pFDF->WriteToString() : ByteString();
}

bool CPDFSDK_InteractiveForm::SubmitForm(const WideString& sDestination,
                                         bool bUrlEncoded) {
  if (sDestination.IsEmpty())
    return false;

  std::unique_ptr<CFDF_Document> pFDFDoc = m_pInteractiveForm->ExportToFDF(
      m_pFormFillEnv->JS_docGetFilePath(), false);
  if (!pFDFDoc)
    return false;

  ByteString fdfBuffer = pFDFDoc->WriteToString();
  if (fdfBuffer.IsEmpty())
    return false;

  std::vector<uint8_t> buffer(fdfBuffer.begin(), fdfBuffer.end());
  if (bUrlEncoded && !FDFToURLEncodedData(&buffer))
    return false;

  m_pFormFillEnv->JS_docSubmitForm(buffer.data(), buffer.size(), sDestination);
  return true;
}

ByteString CPDFSDK_InteractiveForm::ExportFormToFDFTextBuf() {
  std::unique_ptr<CFDF_Document> pFDF = m_pInteractiveForm->ExportToFDF(
      m_pFormFillEnv->JS_docGetFilePath(), false);

  return pFDF ? pFDF->WriteToString() : ByteString();
}

void CPDFSDK_InteractiveForm::DoAction_ResetForm(const CPDF_Action& action) {
  ASSERT(action.GetDict());
  const CPDF_Dictionary* pActionDict = action.GetDict();
  if (!pActionDict->KeyExist("Fields")) {
    m_pInteractiveForm->ResetForm(NotificationOption::kNotify);
    return;
  }
  CPDF_ActionFields af(&action);
  uint32_t dwFlags = action.GetFlags();
  std::vector<const CPDF_Object*> fieldObjects = af.GetAllFields();
  std::vector<CPDF_FormField*> fields = GetFieldFromObjects(fieldObjects);
  m_pInteractiveForm->ResetForm(fields, !(dwFlags & 0x01),
                                NotificationOption::kNotify);
}

std::vector<CPDF_FormField*> CPDFSDK_InteractiveForm::GetFieldFromObjects(
    const std::vector<const CPDF_Object*>& objects) const {
  std::vector<CPDF_FormField*> fields;
  for (const CPDF_Object* pObject : objects) {
    if (!pObject || !pObject->IsString())
      continue;

    WideString csName = pObject->GetUnicodeText();
    CPDF_FormField* pField = m_pInteractiveForm->GetField(0, csName);
    if (pField)
      fields.push_back(pField);
  }
  return fields;
}

bool CPDFSDK_InteractiveForm::BeforeValueChange(CPDF_FormField* pField,
                                                const WideString& csValue) {
  FormFieldType fieldType = pField->GetFieldType();
  if (!IsFormFieldTypeComboOrText(fieldType))
    return true;
  if (!OnKeyStrokeCommit(pField, csValue))
    return false;
  return OnValidate(pField, csValue);
}

void CPDFSDK_InteractiveForm::AfterValueChange(CPDF_FormField* pField) {
#ifdef PDF_ENABLE_XFA
  SynchronizeField(pField);
#endif  // PDF_ENABLE_XFA

  FormFieldType fieldType = pField->GetFieldType();
  if (!IsFormFieldTypeComboOrText(fieldType))
    return;

  OnCalculate(pField);
  ResetFieldAppearance(pField, OnFormat(pField), true);
  UpdateField(pField);
}

bool CPDFSDK_InteractiveForm::BeforeSelectionChange(CPDF_FormField* pField,
                                                    const WideString& csValue) {
  if (pField->GetFieldType() != FormFieldType::kListBox)
    return true;
  if (!OnKeyStrokeCommit(pField, csValue))
    return false;
  return OnValidate(pField, csValue);
}

void CPDFSDK_InteractiveForm::AfterSelectionChange(CPDF_FormField* pField) {
  if (pField->GetFieldType() != FormFieldType::kListBox)
    return;

  OnCalculate(pField);
  ResetFieldAppearance(pField, pdfium::nullopt, true);
  UpdateField(pField);
}

void CPDFSDK_InteractiveForm::AfterCheckedStatusChange(CPDF_FormField* pField) {
  FormFieldType fieldType = pField->GetFieldType();
  if (fieldType != FormFieldType::kCheckBox &&
      fieldType != FormFieldType::kRadioButton)
    return;

  OnCalculate(pField);
  UpdateField(pField);
}

void CPDFSDK_InteractiveForm::AfterFormReset(CPDF_InteractiveForm* pForm) {
  OnCalculate(nullptr);
}

bool CPDFSDK_InteractiveForm::IsNeedHighLight(FormFieldType fieldType) const {
  if (fieldType == FormFieldType::kUnknown)
    return false;

#ifdef PDF_ENABLE_XFA
  // For the XFA fields, we need to return if the specific field type has
  // highlight enabled or if the general XFA field type has it enabled.
  if (IsFormFieldTypeXFA(fieldType)) {
    if (!m_NeedsHighlight[static_cast<size_t>(fieldType)])
      return m_NeedsHighlight[static_cast<size_t>(FormFieldType::kXFA)];
  }
#endif  // PDF_ENABLE_XFA
  return m_NeedsHighlight[static_cast<size_t>(fieldType)];
}

void CPDFSDK_InteractiveForm::RemoveAllHighLights() {
  std::fill(m_HighlightColor, m_HighlightColor + kFormFieldTypeCount,
            kWhiteBGR);
  std::fill(m_NeedsHighlight, m_NeedsHighlight + kFormFieldTypeCount, false);
}

void CPDFSDK_InteractiveForm::SetHighlightColor(FX_COLORREF clr,
                                                FormFieldType fieldType) {
  if (fieldType == FormFieldType::kUnknown)
    return;

  m_HighlightColor[static_cast<size_t>(fieldType)] = clr;
  m_NeedsHighlight[static_cast<size_t>(fieldType)] = true;
}

void CPDFSDK_InteractiveForm::SetAllHighlightColors(FX_COLORREF clr) {
  for (auto type : kFormFieldTypes) {
    m_HighlightColor[static_cast<size_t>(type)] = clr;
    m_NeedsHighlight[static_cast<size_t>(type)] = true;
  }
}

FX_COLORREF CPDFSDK_InteractiveForm::GetHighlightColor(
    FormFieldType fieldType) {
  if (fieldType == FormFieldType::kUnknown)
    return kWhiteBGR;

#ifdef PDF_ENABLE_XFA
  // For the XFA fields, we need to return the specific field type highlight
  // colour or the general XFA field type colour if present.
  if (IsFormFieldTypeXFA(fieldType)) {
    if (!m_NeedsHighlight[static_cast<size_t>(fieldType)] &&
        m_NeedsHighlight[static_cast<size_t>(FormFieldType::kXFA)]) {
      return m_HighlightColor[static_cast<size_t>(FormFieldType::kXFA)];
    }
  }
#endif  // PDF_ENABLE_XFA
  return m_HighlightColor[static_cast<size_t>(fieldType)];
}

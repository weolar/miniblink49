// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/cpdfsdk_widget.h"

#include <memory>
#include <sstream>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfdoc/cpdf_defaultappearance.h"
#include "core/fpdfdoc/cpdf_formcontrol.h"
#include "core/fpdfdoc/cpdf_formfield.h"
#include "core/fpdfdoc/cpdf_iconfit.h"
#include "core/fpdfdoc/cpdf_interactiveform.h"
#include "core/fxge/cfx_graphstatedata.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/cfx_renderdevice.h"
#include "fpdfsdk/cpdfsdk_actionhandler.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "fpdfsdk/cpdfsdk_interactiveform.h"
#include "fpdfsdk/cpdfsdk_pageview.h"
#include "fpdfsdk/formfiller/cba_fontmap.h"
#include "fpdfsdk/pwl/cpwl_appstream.h"
#include "fpdfsdk/pwl/cpwl_edit.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/cpdfxfa_context.h"
#include "xfa/fxfa/cxfa_eventparam.h"
#include "xfa/fxfa/cxfa_ffdocview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/cxfa_ffwidgethandler.h"
//#include "xfa/fxfa/parser/cxfa_node.h"
#endif  // PDF_ENABLE_XFA

CPDFSDK_Widget::CPDFSDK_Widget(CPDF_Annot* pAnnot,
                               CPDFSDK_PageView* pPageView,
                               CPDFSDK_InteractiveForm* pInteractiveForm)
    : CPDFSDK_BAAnnot(pAnnot, pPageView),
      m_pInteractiveForm(pInteractiveForm) {}

CPDFSDK_Widget::~CPDFSDK_Widget() = default;

#ifdef PDF_ENABLE_XFA
CXFA_FFWidget* CPDFSDK_Widget::GetMixXFAWidget() const {
  CPDFXFA_Context* pContext = m_pPageView->GetFormFillEnv()->GetXFAContext();
  if (pContext->GetFormType() != FormType::kXFAForeground)
    return nullptr;

  CXFA_FFDocView* pDocView = pContext->GetXFADocView();
  if (!pDocView)
    return nullptr;

  WideString sName;
  if (GetFieldType() == FormFieldType::kRadioButton) {
    sName = GetAnnotName();
    if (sName.IsEmpty())
      sName = GetName();
  } else {
    sName = GetName();
  }

  if (sName.IsEmpty())
    return nullptr;

  return pDocView->GetWidgetByName(sName, nullptr);
}

CXFA_FFWidget* CPDFSDK_Widget::GetGroupMixXFAWidget() const {
  CPDFXFA_Context* pContext = m_pPageView->GetFormFillEnv()->GetXFAContext();
  if (pContext->GetFormType() != FormType::kXFAForeground)
    return nullptr;

  CXFA_FFDocView* pDocView = pContext->GetXFADocView();
  if (!pDocView)
    return nullptr;

  WideString sName = GetName();
  return !sName.IsEmpty() ? pDocView->GetWidgetByName(sName, nullptr) : nullptr;
}

CXFA_FFWidgetHandler* CPDFSDK_Widget::GetXFAWidgetHandler() const {
  CPDFXFA_Context* pContext = m_pPageView->GetFormFillEnv()->GetXFAContext();
  if (pContext->GetFormType() != FormType::kXFAForeground)
    return nullptr;

  if (!m_pWidgetHandler) {
    CXFA_FFDocView* pDocView = pContext->GetXFADocView();
    if (pDocView)
      m_pWidgetHandler = pDocView->GetWidgetHandler();
  }
  return m_pWidgetHandler.Get();
}

static XFA_EVENTTYPE GetXFAEventType(PDFSDK_XFAAActionType eXFAAAT) {
  XFA_EVENTTYPE eEventType = XFA_EVENT_Unknown;

  switch (eXFAAAT) {
    case PDFSDK_XFA_Click:
      eEventType = XFA_EVENT_Click;
      break;
    case PDFSDK_XFA_Full:
      eEventType = XFA_EVENT_Full;
      break;
    case PDFSDK_XFA_PreOpen:
      eEventType = XFA_EVENT_PreOpen;
      break;
    case PDFSDK_XFA_PostOpen:
      eEventType = XFA_EVENT_PostOpen;
      break;
  }

  return eEventType;
}

static XFA_EVENTTYPE GetXFAEventType(CPDF_AAction::AActionType eAAT,
                                     bool bWillCommit) {
  XFA_EVENTTYPE eEventType = XFA_EVENT_Unknown;

  switch (eAAT) {
    case CPDF_AAction::kCursorEnter:
      eEventType = XFA_EVENT_MouseEnter;
      break;
    case CPDF_AAction::kCursorExit:
      eEventType = XFA_EVENT_MouseExit;
      break;
    case CPDF_AAction::kButtonDown:
      eEventType = XFA_EVENT_MouseDown;
      break;
    case CPDF_AAction::kButtonUp:
      eEventType = XFA_EVENT_MouseUp;
      break;
    case CPDF_AAction::kGetFocus:
      eEventType = XFA_EVENT_Enter;
      break;
    case CPDF_AAction::kLoseFocus:
      eEventType = XFA_EVENT_Exit;
      break;
    case CPDF_AAction::kPageOpen:
    case CPDF_AAction::kPageClose:
    case CPDF_AAction::kPageVisible:
    case CPDF_AAction::kPageInvisible:
      break;
    case CPDF_AAction::kKeyStroke:
      if (!bWillCommit)
        eEventType = XFA_EVENT_Change;
      break;
    case CPDF_AAction::kValidate:
      eEventType = XFA_EVENT_Validate;
      break;
    case CPDF_AAction::kOpenPage:
    case CPDF_AAction::kClosePage:
    case CPDF_AAction::kFormat:
    case CPDF_AAction::kCalculate:
    case CPDF_AAction::kCloseDocument:
    case CPDF_AAction::kSaveDocument:
    case CPDF_AAction::kDocumentSaved:
    case CPDF_AAction::kPrintDocument:
    case CPDF_AAction::kDocumentPrinted:
      break;
    case CPDF_AAction::kDocumentOpen:
    case CPDF_AAction::kNumberOfActions:
      NOTREACHED();
      break;
  }

  return eEventType;
}

bool CPDFSDK_Widget::HasXFAAAction(PDFSDK_XFAAActionType eXFAAAT) const {
//   CXFA_FFWidget* hWidget = GetMixXFAWidget();
//   if (!hWidget)
//     return false;
// 
//   CXFA_FFWidgetHandler* pXFAWidgetHandler = GetXFAWidgetHandler();
//   if (!pXFAWidgetHandler)
//     return false;
// 
//   XFA_EVENTTYPE eEventType = GetXFAEventType(eXFAAAT);
// 
//   if ((eEventType == XFA_EVENT_Click || eEventType == XFA_EVENT_Change) &&
//       GetFieldType() == FormFieldType::kRadioButton) {
//     if (CXFA_FFWidget* hGroupWidget = GetGroupMixXFAWidget()) {
//       CXFA_Node* node = hGroupWidget->GetNode();
//       if (node->IsWidgetReady()) {
//         if (pXFAWidgetHandler->HasEvent(node, eEventType))
//           return true;
//       }
//     }
//   }
//   CXFA_Node* node = hWidget->GetNode();
//   if (!node->IsWidgetReady())
//     return false;
//   return pXFAWidgetHandler->HasEvent(node, eEventType);
  DebugBreak();
  return false;
}

bool CPDFSDK_Widget::OnXFAAAction(PDFSDK_XFAAActionType eXFAAAT,
                                  CPDFSDK_FieldAction* data,
                                  CPDFSDK_PageView* pPageView) {
  DebugBreak();
  return false;
//   CPDFXFA_Context* pContext = m_pPageView->GetFormFillEnv()->GetXFAContext();
// 
//   CXFA_FFWidget* hWidget = GetMixXFAWidget();
//   if (!hWidget)
//     return false;
// 
//   XFA_EVENTTYPE eEventType = GetXFAEventType(eXFAAAT);
//   if (eEventType == XFA_EVENT_Unknown)
//     return false;
// 
//   CXFA_FFWidgetHandler* pXFAWidgetHandler = GetXFAWidgetHandler();
//   if (!pXFAWidgetHandler)
//     return false;
// 
//   CXFA_EventParam param;
//   param.m_eType = eEventType;
//   param.m_wsChange = data->sChange;
//   param.m_iCommitKey = 0;
//   param.m_bShift = data->bShift;
//   param.m_iSelStart = data->nSelStart;
//   param.m_iSelEnd = data->nSelEnd;
//   param.m_wsFullText = data->sValue;
//   param.m_bKeyDown = data->bKeyDown;
//   param.m_bModifier = data->bModifier;
//   param.m_wsPrevText = data->sValue;
//   if ((eEventType == XFA_EVENT_Click || eEventType == XFA_EVENT_Change) &&
//       GetFieldType() == FormFieldType::kRadioButton) {
//     if (CXFA_FFWidget* hGroupWidget = GetGroupMixXFAWidget()) {
//       CXFA_Node* node = hGroupWidget->GetNode();
//       if (!node->IsWidgetReady())
//         return false;
// 
//       param.m_pTarget = node;
//       if (pXFAWidgetHandler->ProcessEvent(node, &param) !=
//           XFA_EVENTERROR_Success) {
//         return false;
//       }
//     }
//   }
// 
//   int32_t nRet = XFA_EVENTERROR_NotExist;
//   CXFA_Node* node = hWidget->GetNode();
//   if (node->IsWidgetReady()) {
//     param.m_pTarget = node;
//     nRet = pXFAWidgetHandler->ProcessEvent(node, &param);
//   }
//   if (CXFA_FFDocView* pDocView = pContext->GetXFADocView())
//     pDocView->UpdateDocView();
// 
//   return nRet == XFA_EVENTERROR_Success;
}

void CPDFSDK_Widget::Synchronize(bool bSynchronizeElse) {
//   CXFA_FFWidget* hWidget = GetMixXFAWidget();
//   if (!hWidget)
//     return;
// 
//   CXFA_Node* node = hWidget->GetNode();
//   if (!node->IsWidgetReady())
//     return;
// 
//   CPDF_FormField* pFormField = GetFormField();
//   switch (GetFieldType()) {
//     case FormFieldType::kCheckBox:
//     case FormFieldType::kRadioButton: {
//       CPDF_FormControl* pFormCtrl = GetFormControl();
//       XFA_CHECKSTATE eCheckState =
//           pFormCtrl->IsChecked() ? XFA_CHECKSTATE_On : XFA_CHECKSTATE_Off;
//       node->SetCheckState(eCheckState, true);
//       break;
//     }
//     case FormFieldType::kTextField:
//       node->SetValue(XFA_VALUEPICTURE_Edit, pFormField->GetValue());
//       break;
//     case FormFieldType::kComboBox:
//     case FormFieldType::kListBox: {
//       node->ClearAllSelections();
// 
//       for (int i = 0; i < pFormField->CountSelectedItems(); ++i) {
//         int nIndex = pFormField->GetSelectedIndex(i);
//         if (nIndex > -1 && nIndex < node->CountChoiceListItems(false))
//           node->SetItemState(nIndex, true, false, false, true);
//       }
//       if (GetFieldType() == FormFieldType::kComboBox)
//         node->SetValue(XFA_VALUEPICTURE_Edit, pFormField->GetValue());
//       break;
//     }
//     default:
//       break;
//   }
// 
//   if (bSynchronizeElse) {
//     CPDFXFA_Context* context = m_pPageView->GetFormFillEnv()->GetXFAContext();
//     context->GetXFADocView()->ProcessValueChanged(node);
//   }
  DebugBreak();
}
#endif  // PDF_ENABLE_XFA

bool CPDFSDK_Widget::IsWidgetAppearanceValid(CPDF_Annot::AppearanceMode mode) {
  CPDF_Dictionary* pAP = GetAnnotDict()->GetDictFor("AP");
  if (!pAP)
    return false;

  // Choose the right sub-ap
  const char* ap_entry = "N";
  if (mode == CPDF_Annot::Down)
    ap_entry = "D";
  else if (mode == CPDF_Annot::Rollover)
    ap_entry = "R";
  if (!pAP->KeyExist(ap_entry))
    ap_entry = "N";

  // Get the AP stream or subdirectory
  CPDF_Object* psub = pAP->GetDirectObjectFor(ap_entry);
  if (!psub)
    return false;

  FormFieldType fieldType = GetFieldType();
  switch (fieldType) {
    case FormFieldType::kPushButton:
    case FormFieldType::kComboBox:
    case FormFieldType::kListBox:
    case FormFieldType::kTextField:
    case FormFieldType::kSignature:
      return psub->IsStream();
    case FormFieldType::kCheckBox:
    case FormFieldType::kRadioButton:
      if (CPDF_Dictionary* pSubDict = psub->AsDictionary()) {
        return !!pSubDict->GetStreamFor(GetAppState());
      }
      return false;
    default:
      return true;
  }
}

FormFieldType CPDFSDK_Widget::GetFieldType() const {
  CPDF_FormField* pField = GetFormField();
  return pField ? pField->GetFieldType() : FormFieldType::kUnknown;
}

bool CPDFSDK_Widget::IsAppearanceValid() {
#ifdef PDF_ENABLE_XFA
  CPDFXFA_Context* pContext = m_pPageView->GetFormFillEnv()->GetXFAContext();
  FormType formType = pContext->GetFormType();
  if (formType == FormType::kXFAFull)
    return true;
#endif  // PDF_ENABLE_XFA
  return CPDFSDK_BAAnnot::IsAppearanceValid();
}

int CPDFSDK_Widget::GetLayoutOrder() const {
  return 2;
}

int CPDFSDK_Widget::GetFieldFlags() const {
  CPDF_InteractiveForm* pPDFInteractiveForm =
      m_pInteractiveForm->GetInteractiveForm();
  CPDF_FormControl* pFormControl =
      pPDFInteractiveForm->GetControlByDict(GetAnnotDict());
  CPDF_FormField* pFormField = pFormControl->GetField();
  return pFormField->GetFieldFlags();
}

bool CPDFSDK_Widget::IsSignatureWidget() const {
  return GetFieldType() == FormFieldType::kSignature;
}

CPDF_FormField* CPDFSDK_Widget::GetFormField() const {
  CPDF_FormControl* pControl = GetFormControl();
  return pControl ? pControl->GetField() : nullptr;
}

CPDF_FormControl* CPDFSDK_Widget::GetFormControl() const {
  CPDF_InteractiveForm* pPDFInteractiveForm =
      m_pInteractiveForm->GetInteractiveForm();
  return pPDFInteractiveForm->GetControlByDict(GetAnnotDict());
}

int CPDFSDK_Widget::GetRotate() const {
  CPDF_FormControl* pCtrl = GetFormControl();
  return pCtrl->GetRotation() % 360;
}

#ifdef PDF_ENABLE_XFA
WideString CPDFSDK_Widget::GetName() const {
  return GetFormField()->GetFullName();
}
#endif  // PDF_ENABLE_XFA

Optional<FX_COLORREF> CPDFSDK_Widget::GetFillColor() const {
  CPDF_FormControl* pFormCtrl = GetFormControl();
  int iColorType = 0;
  FX_COLORREF color = ArgbToColorRef(pFormCtrl->GetBackgroundColor(iColorType));
  if (iColorType == CFX_Color::kTransparent)
    return {};
  return color;
}

Optional<FX_COLORREF> CPDFSDK_Widget::GetBorderColor() const {
  CPDF_FormControl* pFormCtrl = GetFormControl();
  int iColorType = 0;
  FX_COLORREF color = ArgbToColorRef(pFormCtrl->GetBorderColor(iColorType));
  if (iColorType == CFX_Color::kTransparent)
    return {};
  return color;
}

Optional<FX_COLORREF> CPDFSDK_Widget::GetTextColor() const {
  CPDF_FormControl* pFormCtrl = GetFormControl();
  CPDF_DefaultAppearance da = pFormCtrl->GetDefaultAppearance();
  FX_ARGB argb;
  Optional<CFX_Color::Type> iColorType;
  std::tie(iColorType, argb) = da.GetColor();
  if (!iColorType.has_value())
    return {};

  FX_COLORREF color = ArgbToColorRef(argb);
  if (iColorType.value() == CFX_Color::kTransparent)
    return {};
  return color;
}

float CPDFSDK_Widget::GetFontSize() const {
  CPDF_FormControl* pFormCtrl = GetFormControl();
  CPDF_DefaultAppearance pDa = pFormCtrl->GetDefaultAppearance();
  float fFontSize;
  pDa.GetFont(&fFontSize);
  return fFontSize;
}

int CPDFSDK_Widget::GetSelectedIndex(int nIndex) const {
// #ifdef PDF_ENABLE_XFA
//   if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
//     CXFA_Node* node = hWidget->GetNode();
//     if (node->IsWidgetReady()) {
//       if (nIndex < node->CountSelectedItems())
//         return node->GetSelectedItem(nIndex);
//     }
//   }
// #endif  // PDF_ENABLE_XFA
//   CPDF_FormField* pFormField = GetFormField();
//   return pFormField->GetSelectedIndex(nIndex);
  DebugBreak();
  return 0;
}

WideString CPDFSDK_Widget::GetValue() const {
// #ifdef PDF_ENABLE_XFA
//   if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
//     CXFA_Node* node = hWidget->GetNode();
//     if (node->IsWidgetReady())
//       return node->GetValue(XFA_VALUEPICTURE_Display);
//   }
// #endif  // PDF_ENABLE_XFA
  DebugBreak();
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->GetValue();
}

WideString CPDFSDK_Widget::GetDefaultValue() const {
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->GetDefaultValue();
}

WideString CPDFSDK_Widget::GetOptionLabel(int nIndex) const {
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->GetOptionLabel(nIndex);
}

int CPDFSDK_Widget::CountOptions() const {
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->CountOptions();
}

bool CPDFSDK_Widget::IsOptionSelected(int nIndex) const {
// #ifdef PDF_ENABLE_XFA
//   if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
//     CXFA_Node* node = hWidget->GetNode();
//     if (node->IsWidgetReady()) {
//       if (nIndex > -1 && nIndex < node->CountChoiceListItems(false))
//         return node->GetItemState(nIndex);
// 
//       return false;
//     }
//   }
// #endif  // PDF_ENABLE_XFA
//   CPDF_FormField* pFormField = GetFormField();
//   return pFormField->IsItemSelected(nIndex);
  DebugBreak();
  return false;
}

int CPDFSDK_Widget::GetTopVisibleIndex() const {
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->GetTopVisibleIndex();
}

bool CPDFSDK_Widget::IsChecked() const {
// #ifdef PDF_ENABLE_XFA
//   if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
//     CXFA_Node* node = hWidget->GetNode();
//     if (node->IsWidgetReady())
//       return node->GetCheckState() == XFA_CHECKSTATE_On;
//   }
// #endif  // PDF_ENABLE_XFA
//   CPDF_FormControl* pFormCtrl = GetFormControl();
//   return pFormCtrl->IsChecked();
  DebugBreak();
  return false;
}

int CPDFSDK_Widget::GetAlignment() const {
  CPDF_FormControl* pFormCtrl = GetFormControl();
  return pFormCtrl->GetControlAlignment();
}

int CPDFSDK_Widget::GetMaxLen() const {
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->GetMaxLen();
}

void CPDFSDK_Widget::SetCheck(bool bChecked, NotificationOption notify) {
  CPDF_FormControl* pFormCtrl = GetFormControl();
  CPDF_FormField* pFormField = pFormCtrl->GetField();
  pFormField->CheckControl(pFormField->GetControlIndex(pFormCtrl), bChecked,
                           notify);
#ifdef PDF_ENABLE_XFA
  if (!IsWidgetAppearanceValid(CPDF_Annot::Normal))
    ResetAppearance(true);
  if (notify == NotificationOption::kDoNotNotify)
    Synchronize(true);
#endif  // PDF_ENABLE_XFA
}

void CPDFSDK_Widget::SetValue(const WideString& sValue,
                              NotificationOption notify) {
  CPDF_FormField* pFormField = GetFormField();
  pFormField->SetValue(sValue, notify);
#ifdef PDF_ENABLE_XFA
  if (notify == NotificationOption::kDoNotNotify)
    Synchronize(true);
#endif  // PDF_ENABLE_XFA
}

void CPDFSDK_Widget::SetOptionSelection(int index,
                                        bool bSelected,
                                        NotificationOption notify) {
  CPDF_FormField* pFormField = GetFormField();
  pFormField->SetItemSelection(index, bSelected, notify);
#ifdef PDF_ENABLE_XFA
  if (notify == NotificationOption::kDoNotNotify)
    Synchronize(true);
#endif  // PDF_ENABLE_XFA
}

void CPDFSDK_Widget::ClearSelection(NotificationOption notify) {
  CPDF_FormField* pFormField = GetFormField();
  pFormField->ClearSelection(notify);
#ifdef PDF_ENABLE_XFA
  if (notify == NotificationOption::kDoNotNotify)
    Synchronize(true);
#endif  // PDF_ENABLE_XFA
}

void CPDFSDK_Widget::SetTopVisibleIndex(int index) {}

void CPDFSDK_Widget::SetAppModified() {
  m_bAppModified = true;
}

void CPDFSDK_Widget::ClearAppModified() {
  m_bAppModified = false;
}

bool CPDFSDK_Widget::IsAppModified() const {
  return m_bAppModified;
}

#ifdef PDF_ENABLE_XFA
void CPDFSDK_Widget::ResetAppearance(bool bValueChanged) {
  switch (GetFieldType()) {
    case FormFieldType::kTextField:
    case FormFieldType::kComboBox: {
      ResetAppearance(OnFormat(), true);
      break;
    }
    default:
      ResetAppearance(pdfium::nullopt, false);
      break;
  }
}
#endif  // PDF_ENABLE_XFA

void CPDFSDK_Widget::ResetAppearance(Optional<WideString> sValue,
                                     bool bValueChanged) {
  SetAppModified();

  m_nAppearanceAge++;
  if (bValueChanged)
    m_nValueAge++;

  CPWL_AppStream appStream(this, GetAPDict());
  switch (GetFieldType()) {
    case FormFieldType::kPushButton:
      appStream.SetAsPushButton();
      break;
    case FormFieldType::kCheckBox:
      appStream.SetAsCheckBox();
      break;
    case FormFieldType::kRadioButton:
      appStream.SetAsRadioButton();
      break;
    case FormFieldType::kComboBox:
      appStream.SetAsComboBox(sValue);
      break;
    case FormFieldType::kListBox:
      appStream.SetAsListBox();
      break;
    case FormFieldType::kTextField:
      appStream.SetAsTextField(sValue);
      break;
    default:
      break;
  }

  m_pAnnot->ClearCachedAP();
}

Optional<WideString> CPDFSDK_Widget::OnFormat() {
  CPDF_FormField* pFormField = GetFormField();
  ASSERT(pFormField);
  return m_pInteractiveForm->OnFormat(pFormField);
}

void CPDFSDK_Widget::ResetFieldAppearance(bool bValueChanged) {
  CPDF_FormField* pFormField = GetFormField();
  ASSERT(pFormField);
  m_pInteractiveForm->ResetFieldAppearance(pFormField, pdfium::nullopt,
                                           bValueChanged);
}

void CPDFSDK_Widget::DrawAppearance(CFX_RenderDevice* pDevice,
                                    const CFX_Matrix& mtUser2Device,
                                    CPDF_Annot::AppearanceMode mode,
                                    const CPDF_RenderOptions* pOptions) {
  FormFieldType fieldType = GetFieldType();

  if ((fieldType == FormFieldType::kCheckBox ||
       fieldType == FormFieldType::kRadioButton) &&
      mode == CPDF_Annot::Normal &&
      !IsWidgetAppearanceValid(CPDF_Annot::Normal)) {
    CFX_GraphStateData gsd;
    gsd.m_LineWidth = 0.0f;

    CFX_PathData pathData;
    pathData.AppendRect(GetRect());
    pDevice->DrawPath(&pathData, &mtUser2Device, &gsd, 0, 0xFFAAAAAA,
                      FXFILL_ALTERNATE);
  } else {
    CPDFSDK_BAAnnot::DrawAppearance(pDevice, mtUser2Device, mode, pOptions);
  }
}

void CPDFSDK_Widget::UpdateField() {
  CPDF_FormField* pFormField = GetFormField();
  ASSERT(pFormField);
  m_pInteractiveForm->UpdateField(pFormField);
}

void CPDFSDK_Widget::DrawShadow(CFX_RenderDevice* pDevice,
                                CPDFSDK_PageView* pPageView) {
  FormFieldType fieldType = GetFieldType();
  if (!m_pInteractiveForm->IsNeedHighLight(fieldType))
    return;

  CFX_Matrix page2device = pPageView->GetCurrentMatrix();
  CFX_FloatRect rcDevice = GetRect();
  CFX_PointF tmp =
      page2device.Transform(CFX_PointF(rcDevice.left, rcDevice.bottom));
  rcDevice.left = tmp.x;
  rcDevice.bottom = tmp.y;

  tmp = page2device.Transform(CFX_PointF(rcDevice.right, rcDevice.top));
  rcDevice.right = tmp.x;
  rcDevice.top = tmp.y;
  rcDevice.Normalize();

  pDevice->FillRect(
      rcDevice.ToFxRect(),
      AlphaAndColorRefToArgb(
          static_cast<int>(m_pInteractiveForm->GetHighlightAlpha()),
          m_pInteractiveForm->GetHighlightColor(fieldType)));
}

CFX_FloatRect CPDFSDK_Widget::GetClientRect() const {
  CFX_FloatRect rcWindow = GetRotatedRect();
  float fBorderWidth = GetBorderWidth();
  switch (GetBorderStyle()) {
    case BorderStyle::BEVELED:
    case BorderStyle::INSET:
      fBorderWidth *= 2.0f;
      break;
    default:
      break;
  }
  return rcWindow.GetDeflated(fBorderWidth, fBorderWidth);
}

CFX_FloatRect CPDFSDK_Widget::GetRotatedRect() const {
  CFX_FloatRect rectAnnot = GetRect();
  float fWidth = rectAnnot.Width();
  float fHeight = rectAnnot.Height();

  CPDF_FormControl* pControl = GetFormControl();
  CFX_FloatRect rcPDFWindow;
  switch (abs(pControl->GetRotation() % 360)) {
    case 0:
    case 180:
    default:
      rcPDFWindow = CFX_FloatRect(0, 0, fWidth, fHeight);
      break;
    case 90:
    case 270:
      rcPDFWindow = CFX_FloatRect(0, 0, fHeight, fWidth);
      break;
  }

  return rcPDFWindow;
}

CFX_Matrix CPDFSDK_Widget::GetMatrix() const {
  CFX_Matrix mt;
  CPDF_FormControl* pControl = GetFormControl();
  CFX_FloatRect rcAnnot = GetRect();
  float fWidth = rcAnnot.Width();
  float fHeight = rcAnnot.Height();

  switch (abs(pControl->GetRotation() % 360)) {
    default:
    case 0:
      break;
    case 90:
      mt = CFX_Matrix(0, 1, -1, 0, fWidth, 0);
      break;
    case 180:
      mt = CFX_Matrix(-1, 0, 0, -1, fWidth, fHeight);
      break;
    case 270:
      mt = CFX_Matrix(0, -1, 1, 0, 0, fHeight);
      break;
  }

  return mt;
}

CFX_Color CPDFSDK_Widget::GetTextPWLColor() const {
  CFX_Color crText = CFX_Color(CFX_Color::kGray, 0);

  CPDF_FormControl* pFormCtrl = GetFormControl();
  CPDF_DefaultAppearance da = pFormCtrl->GetDefaultAppearance();

  float fc[4];
  Optional<CFX_Color::Type> iColorType = da.GetColor(fc);
  if (iColorType)
    crText = CFX_Color(*iColorType, fc[0], fc[1], fc[2], fc[3]);

  return crText;
}

CFX_Color CPDFSDK_Widget::GetBorderPWLColor() const {
  CFX_Color crBorder;

  CPDF_FormControl* pFormCtrl = GetFormControl();
  int32_t iColorType;
  float fc[4];
  pFormCtrl->GetOriginalBorderColor(iColorType, fc);
  if (iColorType > 0)
    crBorder = CFX_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  return crBorder;
}

CFX_Color CPDFSDK_Widget::GetFillPWLColor() const {
  CFX_Color crFill;

  CPDF_FormControl* pFormCtrl = GetFormControl();
  int32_t iColorType;
  float fc[4];
  pFormCtrl->GetOriginalBackgroundColor(iColorType, fc);
  if (iColorType > 0)
    crFill = CFX_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  return crFill;
}

bool CPDFSDK_Widget::OnAAction(CPDF_AAction::AActionType type,
                               CPDFSDK_FieldAction* data,
                               CPDFSDK_PageView* pPageView) {
//   CPDFSDK_FormFillEnvironment* pFormFillEnv = pPageView->GetFormFillEnv();
// 
// #ifdef PDF_ENABLE_XFA
//   CPDFXFA_Context* pContext = pFormFillEnv->GetXFAContext();
//   if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
//     XFA_EVENTTYPE eEventType = GetXFAEventType(type, data->bWillCommit);
// 
//     if (eEventType != XFA_EVENT_Unknown) {
//       if (CXFA_FFWidgetHandler* pXFAWidgetHandler = GetXFAWidgetHandler()) {
//         CXFA_EventParam param;
//         param.m_eType = eEventType;
//         param.m_wsChange = data->sChange;
//         param.m_iCommitKey = 0;
//         param.m_bShift = data->bShift;
//         param.m_iSelStart = data->nSelStart;
//         param.m_iSelEnd = data->nSelEnd;
//         param.m_wsFullText = data->sValue;
//         param.m_bKeyDown = data->bKeyDown;
//         param.m_bModifier = data->bModifier;
//         param.m_wsPrevText = data->sValue;
// 
//         int32_t nRet = XFA_EVENTERROR_NotExist;
//         CXFA_Node* node = hWidget->GetNode();
//         if (node->IsWidgetReady()) {
//           param.m_pTarget = node;
//           nRet = pXFAWidgetHandler->ProcessEvent(node, &param);
//         }
// 
//         if (CXFA_FFDocView* pDocView = pContext->GetXFADocView())
//           pDocView->UpdateDocView();
// 
//         if (nRet == XFA_EVENTERROR_Success)
//           return true;
//       }
//     }
//   }
// #endif  // PDF_ENABLE_XFA
// 
//   CPDF_Action action = GetAAction(type);
//   if (action.GetType() != CPDF_Action::Unknown) {
//     pFormFillEnv->GetActionHandler()->DoAction_Field(action, type, pFormFillEnv,
//                                                      GetFormField(), data);
//   }
//   return false;

  DebugBreak();
  return false;
}

CPDF_Action CPDFSDK_Widget::GetAAction(CPDF_AAction::AActionType eAAT) {
  switch (eAAT) {
    case CPDF_AAction::kCursorEnter:
    case CPDF_AAction::kCursorExit:
    case CPDF_AAction::kButtonDown:
    case CPDF_AAction::kButtonUp:
    case CPDF_AAction::kGetFocus:
    case CPDF_AAction::kLoseFocus:
    case CPDF_AAction::kPageOpen:
    case CPDF_AAction::kPageClose:
    case CPDF_AAction::kPageVisible:
    case CPDF_AAction::kPageInvisible:
      return CPDFSDK_BAAnnot::GetAAction(eAAT);

    case CPDF_AAction::kKeyStroke:
    case CPDF_AAction::kFormat:
    case CPDF_AAction::kValidate:
    case CPDF_AAction::kCalculate: {
      CPDF_FormField* pField = GetFormField();
      if (pField->GetAdditionalAction().GetDict())
        return pField->GetAdditionalAction().GetAction(eAAT);
      return CPDFSDK_BAAnnot::GetAAction(eAAT);
    }
    default:
      break;
  }

  return CPDF_Action(nullptr);
}

WideString CPDFSDK_Widget::GetAlternateName() const {
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->GetAlternateName();
}

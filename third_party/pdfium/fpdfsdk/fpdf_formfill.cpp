// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_formfill.h"

#include <memory>
#include <vector>

#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/render/cpdf_renderoptions.h"
#include "core/fpdfdoc/cpdf_formcontrol.h"
#include "core/fpdfdoc/cpdf_formfield.h"
#include "core/fpdfdoc/cpdf_interactiveform.h"
#include "core/fpdfdoc/cpdf_occontext.h"
#include "core/fxge/cfx_defaultrenderdevice.h"
#include "fpdfsdk/cpdfsdk_actionhandler.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "fpdfsdk/cpdfsdk_interactiveform.h"
#include "fpdfsdk/cpdfsdk_pageview.h"
#include "public/fpdfview.h"
#include "third_party/base/ptr_util.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/cpdfxfa_context.h"
#include "fpdfsdk/fpdfxfa/cpdfxfa_page.h"
#include "xfa/fxfa/cxfa_ffdocview.h"
#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"

static_assert(static_cast<int>(AlertButton::kDefault) ==
                  JSPLATFORM_ALERT_BUTTON_DEFAULT,
              "Default alert button types must match");
static_assert(static_cast<int>(AlertButton::kOK) == JSPLATFORM_ALERT_BUTTON_OK,
              "OK alert button types must match");
static_assert(static_cast<int>(AlertButton::kOKCancel) ==
                  JSPLATFORM_ALERT_BUTTON_OKCANCEL,
              "OKCancel alert button types must match");
static_assert(static_cast<int>(AlertButton::kYesNo) ==
                  JSPLATFORM_ALERT_BUTTON_YESNO,
              "YesNo alert button types must match");
static_assert(static_cast<int>(AlertButton::kYesNoCancel) ==
                  JSPLATFORM_ALERT_BUTTON_YESNOCANCEL,
              "YesNoCancel alert button types must match");

static_assert(static_cast<int>(AlertIcon::kDefault) ==
                  JSPLATFORM_ALERT_ICON_DEFAULT,
              "Default alert icon types must match");
static_assert(static_cast<int>(AlertIcon::kError) ==
                  JSPLATFORM_ALERT_ICON_ERROR,
              "Error alert icon types must match");
static_assert(static_cast<int>(AlertIcon::kWarning) ==
                  JSPLATFORM_ALERT_ICON_WARNING,
              "Warning alert icon types must match");
static_assert(static_cast<int>(AlertIcon::kQuestion) ==
                  JSPLATFORM_ALERT_ICON_QUESTION,
              "Question alert icon types must match");
static_assert(static_cast<int>(AlertIcon::kStatus) ==
                  JSPLATFORM_ALERT_ICON_STATUS,
              "Status alert icon types must match");
static_assert(static_cast<int>(AlertIcon::kAsterisk) ==
                  JSPLATFORM_ALERT_ICON_ASTERISK,
              "Asterisk alert icon types must match");

static_assert(static_cast<int>(AlertReturn::kOK) == JSPLATFORM_ALERT_RETURN_OK,
              "OK alert return types must match");
static_assert(static_cast<int>(AlertReturn::kCancel) ==
                  JSPLATFORM_ALERT_RETURN_CANCEL,
              "Cancel alert return types must match");
static_assert(static_cast<int>(AlertReturn::kNo) == JSPLATFORM_ALERT_RETURN_NO,
              "No alert return types must match");
static_assert(static_cast<int>(AlertReturn::kYes) ==
                  JSPLATFORM_ALERT_RETURN_YES,
              "Yes alert return types must match");

static_assert(static_cast<int>(FormType::kNone) == FORMTYPE_NONE,
              "None form types must match");
static_assert(static_cast<int>(FormType::kAcroForm) == FORMTYPE_ACRO_FORM,
              "AcroForm form types must match");
static_assert(static_cast<int>(FormType::kXFAFull) == FORMTYPE_XFA_FULL,
              "XFA full form types must match");
static_assert(static_cast<int>(FormType::kXFAForeground) ==
                  FORMTYPE_XFA_FOREGROUND,
              "XFA foreground form types must match");
#endif  // PDF_ENABLE_XFA

static_assert(static_cast<int>(FormFieldType::kUnknown) ==
                  FPDF_FORMFIELD_UNKNOWN,
              "Unknown form field types must match");
static_assert(static_cast<int>(FormFieldType::kPushButton) ==
                  FPDF_FORMFIELD_PUSHBUTTON,
              "PushButton form field types must match");
static_assert(static_cast<int>(FormFieldType::kCheckBox) ==
                  FPDF_FORMFIELD_CHECKBOX,
              "CheckBox form field types must match");
static_assert(static_cast<int>(FormFieldType::kRadioButton) ==
                  FPDF_FORMFIELD_RADIOBUTTON,
              "RadioButton form field types must match");
static_assert(static_cast<int>(FormFieldType::kComboBox) ==
                  FPDF_FORMFIELD_COMBOBOX,
              "ComboBox form field types must match");
static_assert(static_cast<int>(FormFieldType::kListBox) ==
                  FPDF_FORMFIELD_LISTBOX,
              "ListBox form field types must match");
static_assert(static_cast<int>(FormFieldType::kTextField) ==
                  FPDF_FORMFIELD_TEXTFIELD,
              "TextField form field types must match");
static_assert(static_cast<int>(FormFieldType::kSignature) ==
                  FPDF_FORMFIELD_SIGNATURE,
              "Signature form field types must match");
#ifdef PDF_ENABLE_XFA
static_assert(static_cast<int>(FormFieldType::kXFA) == FPDF_FORMFIELD_XFA,
              "XFA form field types must match");
static_assert(static_cast<int>(FormFieldType::kXFA_CheckBox) ==
                  FPDF_FORMFIELD_XFA_CHECKBOX,
              "XFA CheckBox form field types must match");
static_assert(static_cast<int>(FormFieldType::kXFA_ComboBox) ==
                  FPDF_FORMFIELD_XFA_COMBOBOX,
              "XFA ComboBox form field types must match");
static_assert(static_cast<int>(FormFieldType::kXFA_ImageField) ==
                  FPDF_FORMFIELD_XFA_IMAGEFIELD,
              "XFA ImageField form field types must match");
static_assert(static_cast<int>(FormFieldType::kXFA_ListBox) ==
                  FPDF_FORMFIELD_XFA_LISTBOX,
              "XFA ListBox form field types must match");
static_assert(static_cast<int>(FormFieldType::kXFA_PushButton) ==
                  FPDF_FORMFIELD_XFA_PUSHBUTTON,
              "XFA PushButton form field types must match");
static_assert(static_cast<int>(FormFieldType::kXFA_Signature) ==
                  FPDF_FORMFIELD_XFA_SIGNATURE,
              "XFA Signature form field types must match");
static_assert(static_cast<int>(FormFieldType::kXFA_TextField) ==
                  FPDF_FORMFIELD_XFA_TEXTFIELD,
              "XFA TextField form field types must match");
#endif  // PDF_ENABLE_XFA
static_assert(kFormFieldTypeCount == FPDF_FORMFIELD_COUNT,
              "Number of form field types must match");

static_assert(static_cast<int>(CPDF_AAction::kCloseDocument) ==
                  FPDFDOC_AACTION_WC,
              "CloseDocument action must match");
static_assert(static_cast<int>(CPDF_AAction::kSaveDocument) ==
                  FPDFDOC_AACTION_WS,
              "SaveDocument action must match");
static_assert(static_cast<int>(CPDF_AAction::kDocumentSaved) ==
                  FPDFDOC_AACTION_DS,
              "DocumentSaved action must match");
static_assert(static_cast<int>(CPDF_AAction::kPrintDocument) ==
                  FPDFDOC_AACTION_WP,
              "PrintDocument action must match");
static_assert(static_cast<int>(CPDF_AAction::kDocumentPrinted) ==
                  FPDFDOC_AACTION_DP,
              "DocumentPrinted action must match");

namespace {

CPDFSDK_InteractiveForm* FormHandleToInteractiveForm(FPDF_FORMHANDLE hHandle) {
  CPDFSDK_FormFillEnvironment* pFormFillEnv =
      CPDFSDKFormFillEnvironmentFromFPDFFormHandle(hHandle);
  return pFormFillEnv ? pFormFillEnv->GetInteractiveForm() : nullptr;
}

CPDFSDK_PageView* FormHandleToPageView(FPDF_FORMHANDLE hHandle,
                                       FPDF_PAGE fpdf_page) {
  IPDF_Page* pPage = IPDFPageFromFPDFPage(fpdf_page);
  if (!pPage)
    return nullptr;

  CPDFSDK_FormFillEnvironment* pFormFillEnv =
      CPDFSDKFormFillEnvironmentFromFPDFFormHandle(hHandle);
  return pFormFillEnv ? pFormFillEnv->GetPageView(pPage, true) : nullptr;
}

void FFLCommon(FPDF_FORMHANDLE hHandle,
               FPDF_BITMAP bitmap,
               FPDF_RECORDER recorder,
               FPDF_PAGE fpdf_page,
               int start_x,
               int start_y,
               int size_x,
               int size_y,
               int rotate,
               int flags) {
  if (!hHandle)
    return;

  IPDF_Page* pPage = IPDFPageFromFPDFPage(fpdf_page);
  if (!pPage)
    return;

  CPDF_Document* pPDFDoc = pPage->GetDocument();
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, fpdf_page);

  const FX_RECT rect(start_x, start_y, start_x + size_x, start_y + size_y);
  CFX_Matrix matrix = pPage->GetDisplayMatrix(rect, rotate);

  auto pDevice = pdfium::MakeUnique<CFX_DefaultRenderDevice>();
#ifdef _SKIA_SUPPORT_
  pDevice->AttachRecorder(static_cast<SkPictureRecorder*>(recorder));
#endif

  RetainPtr<CFX_DIBitmap> holder(CFXDIBitmapFromFPDFBitmap(bitmap));
  pDevice->Attach(holder, false, nullptr, false);
  {
    CFX_RenderDevice::StateRestorer restorer(pDevice.get());
    pDevice->SetClip_Rect(rect);

    CPDF_RenderOptions options;
    options.GetOptions().bClearType = !!(flags & FPDF_LCD_TEXT);

    // Grayscale output
    if (flags & FPDF_GRAYSCALE)
      options.SetColorMode(CPDF_RenderOptions::kGray);

    options.SetDrawAnnots(flags & FPDF_ANNOT);
    options.SetOCContext(
        pdfium::MakeRetain<CPDF_OCContext>(pPDFDoc, CPDF_OCContext::View));

    if (pPageView)
      pPageView->PageView_OnDraw(pDevice.get(), matrix, &options, rect);
  }

#ifdef _SKIA_SUPPORT_PATHS_
  pDevice->Flush(true);
  holder->UnPreMultiply();
#endif
}

}  // namespace

FPDF_EXPORT int FPDF_CALLCONV
FPDFPage_HasFormFieldAtPoint(FPDF_FORMHANDLE hHandle,
                             FPDF_PAGE page,
                             double page_x,
                             double page_y) {
  if (!hHandle)
    return -1;
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (pPage) {
    CPDF_InteractiveForm interactive_form(pPage->GetDocument());
    CPDF_FormControl* pFormCtrl = interactive_form.GetControlAtPoint(
        pPage,
        CFX_PointF(static_cast<float>(page_x), static_cast<float>(page_y)),
        nullptr);
    if (!pFormCtrl)
      return -1;
    CPDF_FormField* pFormField = pFormCtrl->GetField();
    return pFormField ? static_cast<int>(pFormField->GetFieldType()) : -1;
  }

#ifdef PDF_ENABLE_XFA
  CPDFXFA_Page* pXFAPage = ToXFAPage(IPDFPageFromFPDFPage(page));
  if (!pXFAPage)
    return -1;

  CXFA_FFPageView* pPageView = pXFAPage->GetXFAPageView();
  if (!pPageView)
    return -1;

  CXFA_FFDocView* pDocView = pPageView->GetDocView();
  if (!pDocView)
    return -1;

  CXFA_FFWidgetHandler* pWidgetHandler = pDocView->GetWidgetHandler();
  if (!pWidgetHandler)
    return -1;

  std::unique_ptr<IXFA_WidgetIterator> pWidgetIterator(
      pPageView->CreateWidgetIterator(XFA_TRAVERSEWAY_Form,
                                      XFA_WidgetStatus_Viewable));
  if (!pWidgetIterator)
    return -1;

  CXFA_FFWidget* pXFAAnnot;
  while ((pXFAAnnot = pWidgetIterator->MoveToNext()) != nullptr) {
    if (pXFAAnnot->GetFormFieldType() == FormFieldType::kXFA)
      continue;

    CFX_RectF rcBBox = pXFAAnnot->GetWidgetRect();
    CFX_FloatRect rcWidget(rcBBox.left, rcBBox.top, rcBBox.left + rcBBox.width,
                           rcBBox.top + rcBBox.height);
    rcWidget.Inflate(1.0f, 1.0f);
    if (rcWidget.Contains(CFX_PointF(static_cast<float>(page_x),
                                     static_cast<float>(page_y)))) {
      return static_cast<int>(pXFAAnnot->GetFormFieldType());
    }
  }
#endif  // PDF_ENABLE_XFA
  return -1;
}

FPDF_EXPORT int FPDF_CALLCONV
FPDFPage_FormFieldZOrderAtPoint(FPDF_FORMHANDLE hHandle,
                                FPDF_PAGE page,
                                double page_x,
                                double page_y) {
  if (!hHandle)
    return -1;
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage)
    return -1;
  CPDF_InteractiveForm interactive_form(pPage->GetDocument());
  int z_order = -1;
  (void)interactive_form.GetControlAtPoint(
      pPage, CFX_PointF(static_cast<float>(page_x), static_cast<float>(page_y)),
      &z_order);
  return z_order;
}

FPDF_EXPORT FPDF_FORMHANDLE FPDF_CALLCONV
FPDFDOC_InitFormFillEnvironment(FPDF_DOCUMENT document,
                                FPDF_FORMFILLINFO* formInfo) {
#ifdef PDF_ENABLE_XFA
  const int kRequiredVersion = 2;
#else   // PDF_ENABLE_XFA
  const int kRequiredVersion = 1;
#endif  // PDF_ENABLE_XFA
  if (!formInfo || formInfo->version != kRequiredVersion)
    return nullptr;

  auto* pDocument = CPDFDocumentFromFPDFDocument(document);
  if (!pDocument)
    return nullptr;

#ifdef PDF_ENABLE_XFA
  // If the CPDFXFA_Context has a FormFillEnvironment already then we've done
  // this and can just return the old Env. Otherwise, we'll end up setting a new
  // environment into the XFADocument and, that could get weird.
  auto* pContext = static_cast<CPDFXFA_Context*>(pDocument->GetExtension());
  if (pContext && pContext->GetFormFillEnv()) {
    return FPDFFormHandleFromCPDFSDKFormFillEnvironment(
        pContext->GetFormFillEnv());
  }
#endif

  auto pFormFillEnv = pdfium::MakeUnique<CPDFSDK_FormFillEnvironment>(
      CPDFDocumentFromFPDFDocument(document), formInfo);

#ifdef PDF_ENABLE_XFA
  if (pContext)
    pContext->SetFormFillEnv(pFormFillEnv.get());
#endif  // PDF_ENABLE_XFA

  return FPDFFormHandleFromCPDFSDKFormFillEnvironment(
      pFormFillEnv.release());  // Caller takes ownership.
}

FPDF_EXPORT void FPDF_CALLCONV
FPDFDOC_ExitFormFillEnvironment(FPDF_FORMHANDLE hHandle) {
  CPDFSDK_FormFillEnvironment* pFormFillEnv =
      CPDFSDKFormFillEnvironmentFromFPDFFormHandle(hHandle);
  if (!pFormFillEnv)
    return;

#ifdef PDF_ENABLE_XFA
  // Reset the focused annotations and remove the SDK document from the
  // XFA document.
  pFormFillEnv->ClearAllFocusedAnnots();
  // If the document was closed first, it's possible the XFA document
  // is now a nullptr.
  if (pFormFillEnv->GetXFAContext())
    pFormFillEnv->GetXFAContext()->SetFormFillEnv(nullptr);
#endif  // PDF_ENABLE_XFA
  delete pFormFillEnv;
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FORM_OnMouseMove(FPDF_FORMHANDLE hHandle,
                                                     FPDF_PAGE page,
                                                     int modifier,
                                                     double page_x,
                                                     double page_y) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return false;
  return pPageView->OnMouseMove(CFX_PointF(page_x, page_y), modifier);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FORM_OnFocus(FPDF_FORMHANDLE hHandle,
                                                 FPDF_PAGE page,
                                                 int modifier,
                                                 double page_x,
                                                 double page_y) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return false;
  return pPageView->OnFocus(CFX_PointF(page_x, page_y), modifier);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FORM_OnLButtonDown(FPDF_FORMHANDLE hHandle,
                                                       FPDF_PAGE page,
                                                       int modifier,
                                                       double page_x,
                                                       double page_y) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return false;
#ifdef PDF_ENABLE_CLICK_LOGGING
  fprintf(stderr, "mousedown,left,%d,%d\n", static_cast<int>(round(page_x)),
          static_cast<int>(round(page_y)));
#endif  // PDF_ENABLE_CLICK_LOGGING
  return pPageView->OnLButtonDown(CFX_PointF(page_x, page_y), modifier);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FORM_OnLButtonUp(FPDF_FORMHANDLE hHandle,
                                                     FPDF_PAGE page,
                                                     int modifier,
                                                     double page_x,
                                                     double page_y) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return false;
#ifdef PDF_ENABLE_CLICK_LOGGING
  fprintf(stderr, "mouseup,left,%d,%d\n", static_cast<int>(round(page_x)),
          static_cast<int>(round(page_y)));
#endif  // PDF_ENABLE_CLICK_LOGGING
  return pPageView->OnLButtonUp(CFX_PointF(page_x, page_y), modifier);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FORM_OnLButtonDoubleClick(FPDF_FORMHANDLE hHandle,
                          FPDF_PAGE page,
                          int modifier,
                          double page_x,
                          double page_y) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return false;
#ifdef PDF_ENABLE_CLICK_LOGGING
  fprintf(stderr, "mousedown,doubleleft,%d,%d\n",
          static_cast<int>(round(page_x)), static_cast<int>(round(page_y)));
#endif  // PDF_ENABLE_CLICK_LOGGING
  return pPageView->OnLButtonDblClk(CFX_PointF(page_x, page_y), modifier);
}

#ifdef PDF_ENABLE_XFA
FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FORM_OnRButtonDown(FPDF_FORMHANDLE hHandle,
                                                       FPDF_PAGE page,
                                                       int modifier,
                                                       double page_x,
                                                       double page_y) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return false;
#ifdef PDF_ENABLE_CLICK_LOGGING
  fprintf(stderr, "mousedown,right,%d,%d\n", static_cast<int>(round(page_x)),
          static_cast<int>(round(page_y)));
#endif  // PDF_ENABLE_CLICK_LOGGING
  return pPageView->OnRButtonDown(CFX_PointF(page_x, page_y), modifier);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FORM_OnRButtonUp(FPDF_FORMHANDLE hHandle,
                                                     FPDF_PAGE page,
                                                     int modifier,
                                                     double page_x,
                                                     double page_y) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return false;
#ifdef PDF_ENABLE_CLICK_LOGGING
  fprintf(stderr, "mouseup,right,%d,%d\n", static_cast<int>(round(page_x)),
          static_cast<int>(round(page_y)));
#endif  // PDF_ENABLE_CLICK_LOGGING
  return pPageView->OnRButtonUp(CFX_PointF(page_x, page_y), modifier);
}
#endif  // PDF_ENABLE_XFA

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FORM_OnKeyDown(FPDF_FORMHANDLE hHandle,
                                                   FPDF_PAGE page,
                                                   int nKeyCode,
                                                   int modifier) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return false;
  return pPageView->OnKeyDown(nKeyCode, modifier);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FORM_OnKeyUp(FPDF_FORMHANDLE hHandle,
                                                 FPDF_PAGE page,
                                                 int nKeyCode,
                                                 int modifier) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return false;
  return pPageView->OnKeyUp(nKeyCode, modifier);
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FORM_OnChar(FPDF_FORMHANDLE hHandle,
                                                FPDF_PAGE page,
                                                int nChar,
                                                int modifier) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return false;
  return pPageView->OnChar(nChar, modifier);
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FORM_GetFocusedText(FPDF_FORMHANDLE hHandle,
                    FPDF_PAGE page,
                    void* buffer,
                    unsigned long buflen) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return 0;

  return Utf16EncodeMaybeCopyAndReturnLength(pPageView->GetFocusedFormText(),
                                             buffer, buflen);
}

FPDF_EXPORT unsigned long FPDF_CALLCONV
FORM_GetSelectedText(FPDF_FORMHANDLE hHandle,
                     FPDF_PAGE page,
                     void* buffer,
                     unsigned long buflen) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return 0;

  return Utf16EncodeMaybeCopyAndReturnLength(pPageView->GetSelectedText(),
                                             buffer, buflen);
}

FPDF_EXPORT void FPDF_CALLCONV FORM_ReplaceSelection(FPDF_FORMHANDLE hHandle,
                                                     FPDF_PAGE page,
                                                     FPDF_WIDESTRING wsText) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return;

  pPageView->ReplaceSelection(WideStringFromFPDFWideString(wsText));
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FORM_CanUndo(FPDF_FORMHANDLE hHandle,
                                                 FPDF_PAGE page) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return false;
  return pPageView->CanUndo();
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FORM_CanRedo(FPDF_FORMHANDLE hHandle,
                                                 FPDF_PAGE page) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return false;
  return pPageView->CanRedo();
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FORM_Undo(FPDF_FORMHANDLE hHandle,
                                              FPDF_PAGE page) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return false;
  return pPageView->Undo();
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV FORM_Redo(FPDF_FORMHANDLE hHandle,
                                              FPDF_PAGE page) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return false;
  return pPageView->Redo();
}

FPDF_EXPORT FPDF_BOOL FPDF_CALLCONV
FORM_ForceToKillFocus(FPDF_FORMHANDLE hHandle) {
  CPDFSDK_FormFillEnvironment* pFormFillEnv =
      CPDFSDKFormFillEnvironmentFromFPDFFormHandle(hHandle);
  if (!pFormFillEnv)
    return false;
  return pFormFillEnv->KillFocusAnnot(0);
}

FPDF_EXPORT void FPDF_CALLCONV FPDF_FFLDraw(FPDF_FORMHANDLE hHandle,
                                            FPDF_BITMAP bitmap,
                                            FPDF_PAGE page,
                                            int start_x,
                                            int start_y,
                                            int size_x,
                                            int size_y,
                                            int rotate,
                                            int flags) {
  FFLCommon(hHandle, bitmap, nullptr, page, start_x, start_y, size_x, size_y,
            rotate, flags);
}

#ifdef _SKIA_SUPPORT_
FPDF_EXPORT void FPDF_CALLCONV FPDF_FFLRecord(FPDF_FORMHANDLE hHandle,
                                              FPDF_RECORDER recorder,
                                              FPDF_PAGE page,
                                              int start_x,
                                              int start_y,
                                              int size_x,
                                              int size_y,
                                              int rotate,
                                              int flags) {
  FFLCommon(hHandle, nullptr, recorder, page, start_x, start_y, size_x, size_y,
            rotate, flags);
}
#endif

FPDF_EXPORT void FPDF_CALLCONV
FPDF_SetFormFieldHighlightColor(FPDF_FORMHANDLE hHandle,
                                int fieldType,
                                unsigned long color) {
  CPDFSDK_InteractiveForm* pForm = FormHandleToInteractiveForm(hHandle);
  if (!pForm)
    return;

  Optional<FormFieldType> cast_input = IntToFormFieldType(fieldType);
  if (!cast_input)
    return;

  if (cast_input.value() == FormFieldType::kUnknown)
    pForm->SetAllHighlightColors(color);
  else
    pForm->SetHighlightColor(color, cast_input.value());
}

FPDF_EXPORT void FPDF_CALLCONV
FPDF_SetFormFieldHighlightAlpha(FPDF_FORMHANDLE hHandle, unsigned char alpha) {
  if (CPDFSDK_InteractiveForm* pForm = FormHandleToInteractiveForm(hHandle))
    pForm->SetHighlightAlpha(alpha);
}

FPDF_EXPORT void FPDF_CALLCONV
FPDF_RemoveFormFieldHighlight(FPDF_FORMHANDLE hHandle) {
  if (CPDFSDK_InteractiveForm* pForm = FormHandleToInteractiveForm(hHandle))
    pForm->RemoveAllHighLights();
}

FPDF_EXPORT void FPDF_CALLCONV FORM_OnAfterLoadPage(FPDF_PAGE page,
                                                    FPDF_FORMHANDLE hHandle) {
  if (CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page))
    pPageView->SetValid(true);
}

FPDF_EXPORT void FPDF_CALLCONV FORM_OnBeforeClosePage(FPDF_PAGE page,
                                                      FPDF_FORMHANDLE hHandle) {
  CPDFSDK_FormFillEnvironment* pFormFillEnv =
      CPDFSDKFormFillEnvironmentFromFPDFFormHandle(hHandle);
  if (!pFormFillEnv)
    return;

  IPDF_Page* pPage = IPDFPageFromFPDFPage(page);
  if (!pPage)
    return;

  CPDFSDK_PageView* pPageView = pFormFillEnv->GetPageView(pPage, false);
  if (pPageView) {
    pPageView->SetValid(false);
    // RemovePageView() takes care of the delete for us.
    pFormFillEnv->RemovePageView(pPage);
  }
}

FPDF_EXPORT void FPDF_CALLCONV
FORM_DoDocumentJSAction(FPDF_FORMHANDLE hHandle) {
  CPDFSDK_FormFillEnvironment* pFormFillEnv =
      CPDFSDKFormFillEnvironmentFromFPDFFormHandle(hHandle);
  if (pFormFillEnv && pFormFillEnv->IsJSPlatformPresent())
    pFormFillEnv->ProcJavascriptFun();
}

FPDF_EXPORT void FPDF_CALLCONV
FORM_DoDocumentOpenAction(FPDF_FORMHANDLE hHandle) {
  CPDFSDK_FormFillEnvironment* pFormFillEnv =
      CPDFSDKFormFillEnvironmentFromFPDFFormHandle(hHandle);
  if (pFormFillEnv && pFormFillEnv->IsJSPlatformPresent())
    pFormFillEnv->ProcOpenAction();
}

FPDF_EXPORT void FPDF_CALLCONV FORM_DoDocumentAAction(FPDF_FORMHANDLE hHandle,
                                                      int aaType) {
  CPDFSDK_FormFillEnvironment* pFormFillEnv =
      CPDFSDKFormFillEnvironmentFromFPDFFormHandle(hHandle);
  if (!pFormFillEnv)
    return;

  CPDF_Document* pDoc = pFormFillEnv->GetPDFDocument();
  const CPDF_Dictionary* pDict = pDoc->GetRoot();
  if (!pDict)
    return;

  CPDF_AAction aa(pDict->GetDictFor("AA"));
  auto type = static_cast<CPDF_AAction::AActionType>(aaType);
  if (aa.ActionExist(type)) {
    CPDF_Action action = aa.GetAction(type);
    pFormFillEnv->GetActionHandler()->DoAction_Document(action, type,
                                                        pFormFillEnv);
  }
}

FPDF_EXPORT void FPDF_CALLCONV FORM_DoPageAAction(FPDF_PAGE page,
                                                  FPDF_FORMHANDLE hHandle,
                                                  int aaType) {
  CPDFSDK_FormFillEnvironment* pFormFillEnv =
      CPDFSDKFormFillEnvironmentFromFPDFFormHandle(hHandle);
  if (!pFormFillEnv)
    return;

  IPDF_Page* pPage = IPDFPageFromFPDFPage(page);
  CPDF_Page* pPDFPage = CPDFPageFromFPDFPage(page);
  if (!pPDFPage)
    return;

  if (!pFormFillEnv->GetPageView(pPage, false))
    return;

  CPDFSDK_ActionHandler* pActionHandler = pFormFillEnv->GetActionHandler();
  CPDF_Dictionary* pPageDict = pPDFPage->GetDict();
  CPDF_AAction aa(pPageDict->GetDictFor("AA"));
  CPDF_AAction::AActionType type = aaType == FPDFPAGE_AACTION_OPEN
                                       ? CPDF_AAction::kOpenPage
                                       : CPDF_AAction::kClosePage;
  if (aa.ActionExist(type)) {
    CPDF_Action action = aa.GetAction(type);
    pActionHandler->DoAction_Page(action, type, pFormFillEnv);
  }
}

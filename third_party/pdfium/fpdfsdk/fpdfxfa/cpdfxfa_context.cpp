// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/fpdfxfa/cpdfxfa_context.h"

#include <algorithm>
#include <utility>

#include "core/fpdfapi/parser/cpdf_document.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "fpdfsdk/cpdfsdk_pageview.h"
#include "fpdfsdk/fpdfxfa/cpdfxfa_page.h"
#include "fpdfsdk/fpdfxfa/cxfa_fwladaptertimermgr.h"
// #include "fxjs/cjs_runtime.h"
// #include "fxjs/ijs_runtime.h"
#include "public/fpdf_formfill.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "xfa/fgas/font/cfgas_defaultfontmanager.h"
#include "xfa/fxfa/cxfa_eventparam.h"
#include "xfa/fxfa/cxfa_ffapp.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_ffdocview.h"
#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffwidgethandler.h"
#include "xfa/fxfa/cxfa_fontmgr.h"

namespace {

bool IsValidAlertButton(int type) {
  return type == JSPLATFORM_ALERT_BUTTON_OK ||
         type == JSPLATFORM_ALERT_BUTTON_OKCANCEL ||
         type == JSPLATFORM_ALERT_BUTTON_YESNO ||
         type == JSPLATFORM_ALERT_BUTTON_YESNOCANCEL;
}

bool IsValidAlertIcon(int type) {
  return type == JSPLATFORM_ALERT_ICON_ERROR ||
         type == JSPLATFORM_ALERT_ICON_WARNING ||
         type == JSPLATFORM_ALERT_ICON_QUESTION ||
         type == JSPLATFORM_ALERT_ICON_STATUS ||
         type == JSPLATFORM_ALERT_ICON_ASTERISK;
}

}  // namespace

CPDFXFA_Context::CPDFXFA_Context(CPDF_Document* pPDFDoc)
    : m_pPDFDoc(pPDFDoc)
    //, m_pXFAApp(pdfium::MakeUnique<CXFA_FFApp>(this))
    //, m_DocEnv(this) 
{
  ASSERT(m_pPDFDoc);
}

CPDFXFA_Context::~CPDFXFA_Context() {
  m_nLoadStatus = FXFA_LOADSTATUS_CLOSING;

  // Must happen before we remove the form fill environment.
  CloseXFADoc();

  if (m_pFormFillEnv) {
    m_pFormFillEnv->ClearAllFocusedAnnots();
    // Once we're deleted the FormFillEnvironment will point at a bad underlying
    // doc so we need to reset it ...
    m_pFormFillEnv->GetPDFDocument()->SetExtension(nullptr);
    m_pFormFillEnv.Reset();
  }

  m_nLoadStatus = FXFA_LOADSTATUS_CLOSED;
}

void CPDFXFA_Context::CloseXFADoc() {
//   if (!m_pXFADoc)
//     return;
// 
//   m_pXFADocView = nullptr;
//   m_pXFADoc->CloseDoc();
//   m_pXFADoc.reset();
  //DebugBreak();
}

void CPDFXFA_Context::SetFormFillEnv(
    CPDFSDK_FormFillEnvironment* pFormFillEnv) {
  // The layout data can have pointers back into the script context. That
  // context will be different if the form fill environment closes, so, force
  // the layout data to clear.
//   if (m_pXFADoc && m_pXFADoc->GetXFADoc()) {
//     // The CPDF_XFADocView has a pointer to the CXFA_LayoutProcessor which is
//     // owned by the CXFA_Document. The Layout Processor will be freed with the
//     // ClearLayoutData() call. Make sure the doc view has already released the
//     // pointer.
//     if (m_pXFADocView)
//       m_pXFADocView->ResetLayoutProcessor();
// 
//     m_pXFADoc->GetXFADoc()->ClearLayoutData();
//   }
// 
//   m_pFormFillEnv.Reset(pFormFillEnv);
  DebugBreak();
}

bool CPDFXFA_Context::LoadXFADoc() {
//   m_nLoadStatus = FXFA_LOADSTATUS_LOADING;
//   m_XFAPageList.clear();
// 
//   CXFA_FFApp* pApp = GetXFAApp();
//   if (!pApp)
//     return false;
// 
//   m_pXFADoc = pdfium::MakeUnique<CXFA_FFDoc>(pApp, &m_DocEnv);
//   if (!m_pXFADoc->OpenDoc(m_pPDFDoc.Get())) {
//     SetLastError(FPDF_ERR_XFALOAD);
//     return false;
//   }
// 
//   CJS_Runtime* actual_runtime = GetCJSRuntime();  // Null if a stub.
//   if (!actual_runtime) {
//     SetLastError(FPDF_ERR_XFALOAD);
//     return false;
//   }
// 
//   m_pXFADoc->GetXFADoc()->InitScriptContext(actual_runtime);
//   if (m_pXFADoc->GetFormType() == FormType::kXFAFull)
//     m_FormType = FormType::kXFAFull;
//   else
//     m_FormType = FormType::kXFAForeground;
// 
//   m_pXFADocView = m_pXFADoc->CreateDocView();
//   if (m_pXFADocView->StartLayout() < 0) {
//     CloseXFADoc();
//     SetLastError(FPDF_ERR_XFALAYOUT);
//     return false;
//   }
// 
//   m_pXFADocView->DoLayout();
//   m_pXFADocView->StopLayout();
//   m_nLoadStatus = FXFA_LOADSTATUS_LOADED;
//   return true;
  DebugBreak();
  return false;
}

int CPDFXFA_Context::GetPageCount() const {
  switch (m_FormType) {
    case FormType::kNone:
    case FormType::kAcroForm:
    case FormType::kXFAForeground:
      return m_pPDFDoc->GetPageCount();
    case FormType::kXFAFull: {
      //return m_pXFADoc ? m_pXFADocView->CountPageViews() : 0;
      DebugBreak();
      return 0;
    }
  }
  NOTREACHED();
  return 0;
}

RetainPtr<CPDFXFA_Page> CPDFXFA_Context::GetXFAPage(int page_index) {
  if (page_index < 0)
    return nullptr;

  if (pdfium::IndexInBounds(m_XFAPageList, page_index)) {
    if (m_XFAPageList[page_index])
      return m_XFAPageList[page_index];
  } else {
    m_nPageCount = GetPageCount();
    m_XFAPageList.resize(m_nPageCount);
  }

  auto pPage = pdfium::MakeRetain<CPDFXFA_Page>(this, page_index);
  if (!pPage->LoadPage())
    return nullptr;

  if (pdfium::IndexInBounds(m_XFAPageList, page_index))
    m_XFAPageList[page_index] = pPage;

  return pPage;
}

RetainPtr<CPDFXFA_Page> CPDFXFA_Context::GetXFAPage(
    CXFA_FFPageView* pPage) const {
  if (!pPage)
    return nullptr;

//   if (!m_pXFADoc)
//     return nullptr;
  DebugBreak();

  if (m_FormType != FormType::kXFAFull)
    return nullptr;

  for (auto& pTempPage : m_XFAPageList) {
    if (pTempPage && pTempPage->GetXFAPageView() == pPage)
      return pTempPage;
  }
  return nullptr;
}

CPDF_Document* CPDFXFA_Context::GetPDFDoc() const {
  return m_pPDFDoc.Get();
}

void CPDFXFA_Context::DeletePage(int page_index) {
  // Delete from the document first because, if GetPage was never called for
  // this |page_index| then |m_XFAPageList| may have size < |page_index| even
  // if it's a valid page in the document.
  m_pPDFDoc->DeletePage(page_index);

  if (pdfium::IndexInBounds(m_XFAPageList, page_index))
    m_XFAPageList[page_index].Reset();
}

uint32_t CPDFXFA_Context::GetUserPermissions() const {
  // See https://bugs.chromium.org/p/pdfium/issues/detail?id=499
  return 0xFFFFFFFF;
}

void CPDFXFA_Context::ClearChangeMark() {
  if (m_pFormFillEnv)
    m_pFormFillEnv->ClearChangeMark();
}

CJS_Runtime* CPDFXFA_Context::GetCJSRuntime() const {
//   if (!m_pFormFillEnv)
//     return nullptr;
// 
//   return m_pFormFillEnv->GetIJSRuntime()->AsCJSRuntime();
  DebugBreak();
  return nullptr;
}

WideString CPDFXFA_Context::GetAppTitle() const {
  return L"PDFium";
}

WideString CPDFXFA_Context::GetAppName() {
  return m_pFormFillEnv ? m_pFormFillEnv->FFI_GetAppName() : WideString();
}

WideString CPDFXFA_Context::GetLanguage() {
  return m_pFormFillEnv ? m_pFormFillEnv->GetLanguage() : WideString();
}

WideString CPDFXFA_Context::GetPlatform() {
  return m_pFormFillEnv ? m_pFormFillEnv->GetPlatform() : WideString();
}

void CPDFXFA_Context::Beep(uint32_t dwType) {
  if (m_pFormFillEnv)
    m_pFormFillEnv->JS_appBeep(dwType);
}

int32_t CPDFXFA_Context::MsgBox(const WideString& wsMessage,
                                const WideString& wsTitle,
                                uint32_t dwIconType,
                                uint32_t dwButtonType) {
  if (!m_pFormFillEnv || m_nLoadStatus != FXFA_LOADSTATUS_LOADED)
    return -1;

  int iconType =
      IsValidAlertIcon(dwIconType) ? dwIconType : JSPLATFORM_ALERT_ICON_DEFAULT;
  int iButtonType = IsValidAlertButton(dwButtonType)
                        ? dwButtonType
                        : JSPLATFORM_ALERT_BUTTON_DEFAULT;
  return m_pFormFillEnv->JS_appAlert(wsMessage, wsTitle, iButtonType, iconType);
}

WideString CPDFXFA_Context::Response(const WideString& wsQuestion,
                                     const WideString& wsTitle,
                                     const WideString& wsDefaultAnswer,
                                     bool bMark) {
  if (!m_pFormFillEnv)
    return WideString();

  int nLength = 2048;
  std::vector<uint8_t> pBuff(nLength);
  nLength = m_pFormFillEnv->JS_appResponse(wsQuestion, wsTitle, wsDefaultAnswer,
                                           WideString(), bMark, pBuff.data(),
                                           nLength);
  if (nLength <= 0)
    return WideString();

  nLength = std::min(2046, nLength);
  pBuff[nLength] = 0;
  pBuff[nLength + 1] = 0;
  return WideString::FromUTF16LE(reinterpret_cast<uint16_t*>(pBuff.data()),
                                 nLength / sizeof(uint16_t));
}

RetainPtr<IFX_SeekableReadStream> CPDFXFA_Context::DownloadURL(
    const WideString& wsURL) {
  return m_pFormFillEnv ? m_pFormFillEnv->DownloadFromURL(wsURL) : nullptr;
}

bool CPDFXFA_Context::PostRequestURL(const WideString& wsURL,
                                     const WideString& wsData,
                                     const WideString& wsContentType,
                                     const WideString& wsEncode,
                                     const WideString& wsHeader,
                                     WideString& wsResponse) {
  if (!m_pFormFillEnv)
    return false;

  wsResponse = m_pFormFillEnv->PostRequestURL(wsURL, wsData, wsContentType,
                                              wsEncode, wsHeader);
  return true;
}

bool CPDFXFA_Context::PutRequestURL(const WideString& wsURL,
                                    const WideString& wsData,
                                    const WideString& wsEncode) {
  return m_pFormFillEnv &&
         m_pFormFillEnv->PutRequestURL(wsURL, wsData, wsEncode);
}

std::unique_ptr<IFWL_AdapterTimerMgr> CPDFXFA_Context::NewTimerMgr() {
  if (!m_pFormFillEnv)
    return nullptr;
  return pdfium::MakeUnique<CXFA_FWLAdapterTimerMgr>(m_pFormFillEnv.Get());
}

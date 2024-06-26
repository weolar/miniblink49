// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/cpdfsdk_formfillenvironment.h"

#include <memory>
#include <utility>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfdoc/cpdf_docjsactions.h"
#include "fpdfsdk/cfx_systemhandler.h"
#include "fpdfsdk/cpdfsdk_actionhandler.h"
#include "fpdfsdk/cpdfsdk_annothandlermgr.h"
#include "fpdfsdk/cpdfsdk_interactiveform.h"
#include "fpdfsdk/cpdfsdk_pageview.h"
#include "fpdfsdk/cpdfsdk_widget.h"
#include "fpdfsdk/formfiller/cffl_interactiveformfiller.h"
//#include "fxjs/ijs_runtime.h"
#include "public/fpdf_fwlevent.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/cpdfxfa_context.h"
#endif

FPDF_WIDESTRING AsFPDFWideString(ByteString* bsUTF16LE) {
  // Force a private version of the string, since we're about to hand it off
  // to the embedder. Should the embedder modify it by accident, it won't
  // corrupt other shares of the string beyond |bsUTF16LE|.
  return reinterpret_cast<FPDF_WIDESTRING>(
      bsUTF16LE->GetBuffer(bsUTF16LE->GetLength()).data());
}

CPDFSDK_FormFillEnvironment::CPDFSDK_FormFillEnvironment(
    CPDF_Document* pDoc,
    FPDF_FORMFILLINFO* pFFinfo)
    : m_pInfo(pFFinfo),
      m_pCPDFDoc(pDoc),
      m_pSysHandler(pdfium::MakeUnique<CFX_SystemHandler>(this)) {}

CPDFSDK_FormFillEnvironment::~CPDFSDK_FormFillEnvironment() {
  m_bBeingDestroyed = true;
  ClearAllFocusedAnnots();

  // |m_PageMap| will try to access |m_pInteractiveForm| when it cleans itself
  // up. Make sure it is deleted before |m_pInteractiveForm|.
  m_PageMap.clear();

  // |m_pAnnotHandlerMgr| will try to access |m_pFormFiller| when it cleans
  // itself up. Make sure it is deleted before |m_pFormFiller|.
  //m_pAnnotHandlerMgr.reset(); // weolar

  // Must destroy the |m_pFormFiller| before the environment (|this|)
  // because any created form widgets hold a pointer to the environment.
  // Those widgets may call things like KillTimer() as they are shutdown.
  // m_pFormFiller.reset(); // weolar

  if (m_pInfo && m_pInfo->Release)
    m_pInfo->Release(m_pInfo);
}

// static
bool CPDFSDK_FormFillEnvironment::IsSHIFTKeyDown(uint32_t nFlag) {
  return !!(nFlag & FWL_EVENTFLAG_ShiftKey);
}

// static
bool CPDFSDK_FormFillEnvironment::IsCTRLKeyDown(uint32_t nFlag) {
  return !!(nFlag & FWL_EVENTFLAG_ControlKey);
}

// static
bool CPDFSDK_FormFillEnvironment::IsALTKeyDown(uint32_t nFlag) {
  return !!(nFlag & FWL_EVENTFLAG_AltKey);
}

#ifdef PDF_ENABLE_V8
int CPDFSDK_FormFillEnvironment::JS_appAlert(const WideString& Msg,
                                             const WideString& Title,
                                             int Type,
                                             int Icon) {
  if (!m_pInfo || !m_pInfo->m_pJsPlatform ||
      !m_pInfo->m_pJsPlatform->app_alert) {
    return -1;
  }

  ByteString bsMsg = Msg.ToUTF16LE();
  ByteString bsTitle = Title.ToUTF16LE();
  return m_pInfo->m_pJsPlatform->app_alert(
      m_pInfo->m_pJsPlatform, AsFPDFWideString(&bsMsg),
      AsFPDFWideString(&bsTitle), Type, Icon);
}

int CPDFSDK_FormFillEnvironment::JS_appResponse(const WideString& Question,
                                                const WideString& Title,
                                                const WideString& Default,
                                                const WideString& Label,
                                                FPDF_BOOL bPassword,
                                                void* response,
                                                int length) {
  if (!m_pInfo || !m_pInfo->m_pJsPlatform ||
      !m_pInfo->m_pJsPlatform->app_response) {
    return -1;
  }
  ByteString bsQuestion = Question.ToUTF16LE();
  ByteString bsTitle = Title.ToUTF16LE();
  ByteString bsDefault = Default.ToUTF16LE();
  ByteString bsLabel = Label.ToUTF16LE();
  return m_pInfo->m_pJsPlatform->app_response(
      m_pInfo->m_pJsPlatform, AsFPDFWideString(&bsQuestion),
      AsFPDFWideString(&bsTitle), AsFPDFWideString(&bsDefault),
      AsFPDFWideString(&bsLabel), bPassword, response, length);
}

void CPDFSDK_FormFillEnvironment::JS_appBeep(int nType) {
  if (!m_pInfo || !m_pInfo->m_pJsPlatform ||
      !m_pInfo->m_pJsPlatform->app_beep) {
    return;
  }
  m_pInfo->m_pJsPlatform->app_beep(m_pInfo->m_pJsPlatform, nType);
}

WideString CPDFSDK_FormFillEnvironment::JS_fieldBrowse() {
  if (!m_pInfo || !m_pInfo->m_pJsPlatform ||
      !m_pInfo->m_pJsPlatform->Field_browse) {
    return WideString();
  }
  const int nRequiredLen =
      m_pInfo->m_pJsPlatform->Field_browse(m_pInfo->m_pJsPlatform, nullptr, 0);
  if (nRequiredLen <= 0)
    return WideString();

  std::vector<uint8_t> pBuff(nRequiredLen);
  const int nActualLen = m_pInfo->m_pJsPlatform->Field_browse(
      m_pInfo->m_pJsPlatform, pBuff.data(), nRequiredLen);
  if (nActualLen <= 0 || nActualLen > nRequiredLen)
    return WideString();

  pBuff.resize(nActualLen);
  return WideString::FromDefANSI(ByteStringView(pBuff));
}

void CPDFSDK_FormFillEnvironment::JS_docmailForm(void* mailData,
                                                 int length,
                                                 FPDF_BOOL bUI,
                                                 const WideString& To,
                                                 const WideString& Subject,
                                                 const WideString& CC,
                                                 const WideString& BCC,
                                                 const WideString& Msg) {
  if (!m_pInfo || !m_pInfo->m_pJsPlatform ||
      !m_pInfo->m_pJsPlatform->Doc_mail) {
    return;
  }
  ByteString bsTo = To.ToUTF16LE();
  ByteString bsSubject = Subject.ToUTF16LE();
  ByteString bsCC = CC.ToUTF16LE();
  ByteString bsBcc = BCC.ToUTF16LE();
  ByteString bsMsg = Msg.ToUTF16LE();
  m_pInfo->m_pJsPlatform->Doc_mail(
      m_pInfo->m_pJsPlatform, mailData, length, bUI, AsFPDFWideString(&bsTo),
      AsFPDFWideString(&bsSubject), AsFPDFWideString(&bsCC),
      AsFPDFWideString(&bsBcc), AsFPDFWideString(&bsMsg));
}

void CPDFSDK_FormFillEnvironment::JS_docprint(FPDF_BOOL bUI,
                                              int nStart,
                                              int nEnd,
                                              FPDF_BOOL bSilent,
                                              FPDF_BOOL bShrinkToFit,
                                              FPDF_BOOL bPrintAsImage,
                                              FPDF_BOOL bReverse,
                                              FPDF_BOOL bAnnotations) {
  if (!m_pInfo || !m_pInfo->m_pJsPlatform ||
      !m_pInfo->m_pJsPlatform->Doc_print) {
    return;
  }
  m_pInfo->m_pJsPlatform->Doc_print(m_pInfo->m_pJsPlatform, bUI, nStart, nEnd,
                                    bSilent, bShrinkToFit, bPrintAsImage,
                                    bReverse, bAnnotations);
}

void CPDFSDK_FormFillEnvironment::JS_docgotoPage(int nPageNum) {
  if (!m_pInfo || !m_pInfo->m_pJsPlatform ||
      !m_pInfo->m_pJsPlatform->Doc_gotoPage) {
    return;
  }
  m_pInfo->m_pJsPlatform->Doc_gotoPage(m_pInfo->m_pJsPlatform, nPageNum);
}
#endif  // PDF_ENABLE_V8

WideString CPDFSDK_FormFillEnvironment::JS_docGetFilePath() {
  if (!m_pInfo || !m_pInfo->m_pJsPlatform ||
      !m_pInfo->m_pJsPlatform->Doc_getFilePath) {
    return WideString();
  }
  const int nRequiredLen = m_pInfo->m_pJsPlatform->Doc_getFilePath(
      m_pInfo->m_pJsPlatform, nullptr, 0);
  if (nRequiredLen <= 0)
    return WideString();

  std::vector<uint8_t> pBuff(nRequiredLen);
  const int nActualLen = m_pInfo->m_pJsPlatform->Doc_getFilePath(
      m_pInfo->m_pJsPlatform, pBuff.data(), nRequiredLen);
  if (nActualLen <= 0 || nActualLen > nRequiredLen)
    return WideString();

  pBuff.resize(nActualLen);
  return WideString::FromDefANSI(ByteStringView(pBuff));
}

void CPDFSDK_FormFillEnvironment::JS_docSubmitForm(void* formData,
                                                   int length,
                                                   const WideString& URL) {
  if (!m_pInfo || !m_pInfo->m_pJsPlatform ||
      !m_pInfo->m_pJsPlatform->Doc_submitForm) {
    return;
  }
  ByteString bsUrl = URL.ToUTF16LE();
  m_pInfo->m_pJsPlatform->Doc_submitForm(m_pInfo->m_pJsPlatform, formData,
                                         length, AsFPDFWideString(&bsUrl));
}

IJS_Runtime* CPDFSDK_FormFillEnvironment::GetIJSRuntime() {
//   if (!m_pIJSRuntime)
//     m_pIJSRuntime = IJS_Runtime::Create(this);
//   return m_pIJSRuntime.get();
  DebugBreak();
  return nullptr;
}

CPDFSDK_AnnotHandlerMgr* CPDFSDK_FormFillEnvironment::GetAnnotHandlerMgr() {
//   if (!m_pAnnotHandlerMgr)
//     m_pAnnotHandlerMgr = pdfium::MakeUnique<CPDFSDK_AnnotHandlerMgr>(this);
//   return m_pAnnotHandlerMgr.get();
  DebugBreak();
  return nullptr;
}

CPDFSDK_ActionHandler* CPDFSDK_FormFillEnvironment::GetActionHandler() {
  if (!m_pActionHandler)
    m_pActionHandler = pdfium::MakeUnique<CPDFSDK_ActionHandler>();
  return m_pActionHandler.get();
}

CFFL_InteractiveFormFiller*
CPDFSDK_FormFillEnvironment::GetInteractiveFormFiller() {
//   if (!m_pFormFiller)
//     m_pFormFiller = pdfium::MakeUnique<CFFL_InteractiveFormFiller>(this);
//   return m_pFormFiller.get();
  DebugBreak();
  return nullptr;
}

void CPDFSDK_FormFillEnvironment::Invalidate(IPDF_Page* page,
                                             const FX_RECT& rect) {
  if (m_pInfo && m_pInfo->FFI_Invalidate) {
    m_pInfo->FFI_Invalidate(m_pInfo, FPDFPageFromIPDFPage(page), rect.left,
                            rect.top, rect.right, rect.bottom);
  }
}

void CPDFSDK_FormFillEnvironment::OutputSelectedRect(
    IPDF_Page* page,
    const CFX_FloatRect& rect) {
  if (m_pInfo && m_pInfo->FFI_OutputSelectedRect) {
    m_pInfo->FFI_OutputSelectedRect(m_pInfo, FPDFPageFromIPDFPage(page),
                                    rect.left, rect.top, rect.right,
                                    rect.bottom);
  }
}

void CPDFSDK_FormFillEnvironment::SetCursor(int nCursorType) {
  if (m_pInfo && m_pInfo->FFI_SetCursor)
    m_pInfo->FFI_SetCursor(m_pInfo, nCursorType);
}

int CPDFSDK_FormFillEnvironment::SetTimer(int uElapse,
                                          TimerCallback lpTimerFunc) {
  if (m_pInfo && m_pInfo->FFI_SetTimer)
    return m_pInfo->FFI_SetTimer(m_pInfo, uElapse, lpTimerFunc);
  return CFX_SystemHandler::kInvalidTimerID;
}

void CPDFSDK_FormFillEnvironment::KillTimer(int nTimerID) {
  if (m_pInfo && m_pInfo->FFI_KillTimer)
    m_pInfo->FFI_KillTimer(m_pInfo, nTimerID);
}

void CPDFSDK_FormFillEnvironment::OnChange() {
  if (m_pInfo && m_pInfo->FFI_OnChange)
    m_pInfo->FFI_OnChange(m_pInfo);
}

#ifdef PDF_ENABLE_V8
FPDF_PAGE CPDFSDK_FormFillEnvironment::GetCurrentPage() const {
  if (m_pInfo && m_pInfo->FFI_GetCurrentPage) {
    return m_pInfo->FFI_GetCurrentPage(
        m_pInfo, FPDFDocumentFromCPDFDocument(m_pCPDFDoc.Get()));
  }
  return nullptr;
}
#endif

void CPDFSDK_FormFillEnvironment::ExecuteNamedAction(const char* namedAction) {
  if (m_pInfo && m_pInfo->FFI_ExecuteNamedAction)
    m_pInfo->FFI_ExecuteNamedAction(m_pInfo, namedAction);
}

void CPDFSDK_FormFillEnvironment::OnSetFieldInputFocus(
    FPDF_WIDESTRING focusText,
    FPDF_DWORD nTextLen,
    bool bFocus) {
  if (m_pInfo && m_pInfo->FFI_SetTextFieldFocus)
    m_pInfo->FFI_SetTextFieldFocus(m_pInfo, focusText, nTextLen, bFocus);
}

void CPDFSDK_FormFillEnvironment::DoURIAction(const char* bsURI) {
  if (m_pInfo && m_pInfo->FFI_DoURIAction)
    m_pInfo->FFI_DoURIAction(m_pInfo, bsURI);
}

void CPDFSDK_FormFillEnvironment::DoGoToAction(int nPageIndex,
                                               int zoomMode,
                                               float* fPosArray,
                                               int sizeOfArray) {
  if (m_pInfo && m_pInfo->FFI_DoGoToAction) {
    m_pInfo->FFI_DoGoToAction(m_pInfo, nPageIndex, zoomMode, fPosArray,
                              sizeOfArray);
  }
}

#ifdef PDF_ENABLE_XFA
void CPDFSDK_FormFillEnvironment::DisplayCaret(CPDFXFA_Page* page,
                                               FPDF_BOOL bVisible,
                                               double left,
                                               double top,
                                               double right,
                                               double bottom) {
  if (m_pInfo && m_pInfo->FFI_DisplayCaret) {
    m_pInfo->FFI_DisplayCaret(m_pInfo, FPDFPageFromIPDFPage(page), bVisible,
                              left, top, right, bottom);
  }
}

int CPDFSDK_FormFillEnvironment::GetCurrentPageIndex() const {
  if (!m_pInfo || !m_pInfo->FFI_GetCurrentPageIndex)
    return -1;
  return m_pInfo->FFI_GetCurrentPageIndex(
      m_pInfo, FPDFDocumentFromCPDFDocument(m_pCPDFDoc.Get()));
}

void CPDFSDK_FormFillEnvironment::SetCurrentPage(int iCurPage) {
  if (!m_pInfo || !m_pInfo->FFI_SetCurrentPage)
    return;
  m_pInfo->FFI_SetCurrentPage(
      m_pInfo, FPDFDocumentFromCPDFDocument(m_pCPDFDoc.Get()), iCurPage);
}

WideString CPDFSDK_FormFillEnvironment::GetPlatform() {
  if (!m_pInfo || !m_pInfo->FFI_GetPlatform)
    return WideString();

  int nRequiredLen = m_pInfo->FFI_GetPlatform(m_pInfo, nullptr, 0);
  if (nRequiredLen <= 0)
    return WideString();

  std::vector<uint8_t> pBuff(nRequiredLen);
  int nActualLen =
      m_pInfo->FFI_GetPlatform(m_pInfo, pBuff.data(), nRequiredLen);
  if (nActualLen <= 0 || nActualLen > nRequiredLen)
    return WideString();

  return WideString::FromUTF16LE(reinterpret_cast<uint16_t*>(pBuff.data()),
                                 nActualLen / sizeof(uint16_t));
}

void CPDFSDK_FormFillEnvironment::GotoURL(const WideString& wsURL) {
  if (!m_pInfo || !m_pInfo->FFI_GotoURL)
    return;

  ByteString bsTo = wsURL.ToUTF16LE();
  m_pInfo->FFI_GotoURL(m_pInfo, FPDFDocumentFromCPDFDocument(m_pCPDFDoc.Get()),
                       AsFPDFWideString(&bsTo));
}

FS_RECTF CPDFSDK_FormFillEnvironment::GetPageViewRect(CPDFXFA_Page* page) {
  FS_RECTF rect = {0.0f, 0.0f, 0.0f, 0.0f};
  if (!m_pInfo || !m_pInfo->FFI_GetPageViewRect)
    return rect;

  double left;
  double top;
  double right;
  double bottom;
  m_pInfo->FFI_GetPageViewRect(m_pInfo, FPDFPageFromIPDFPage(page), &left, &top,
                               &right, &bottom);

  rect.left = static_cast<float>(left);
  rect.top = static_cast<float>(top);
  rect.bottom = static_cast<float>(bottom);
  rect.right = static_cast<float>(right);
  return rect;
}

bool CPDFSDK_FormFillEnvironment::PopupMenu(CPDFXFA_Page* page,
                                            FPDF_WIDGET hWidget,
                                            int menuFlag,
                                            CFX_PointF pt) {
  return m_pInfo && m_pInfo->FFI_PopupMenu &&
         m_pInfo->FFI_PopupMenu(m_pInfo, FPDFPageFromIPDFPage(page), hWidget,
                                menuFlag, pt.x, pt.y);
}

void CPDFSDK_FormFillEnvironment::EmailTo(FPDF_FILEHANDLER* fileHandler,
                                          FPDF_WIDESTRING pTo,
                                          FPDF_WIDESTRING pSubject,
                                          FPDF_WIDESTRING pCC,
                                          FPDF_WIDESTRING pBcc,
                                          FPDF_WIDESTRING pMsg) {
  if (m_pInfo && m_pInfo->FFI_EmailTo)
    m_pInfo->FFI_EmailTo(m_pInfo, fileHandler, pTo, pSubject, pCC, pBcc, pMsg);
}

void CPDFSDK_FormFillEnvironment::UploadTo(FPDF_FILEHANDLER* fileHandler,
                                           int fileFlag,
                                           FPDF_WIDESTRING uploadTo) {
  if (m_pInfo && m_pInfo->FFI_UploadTo)
    m_pInfo->FFI_UploadTo(m_pInfo, fileHandler, fileFlag, uploadTo);
}

FPDF_FILEHANDLER* CPDFSDK_FormFillEnvironment::OpenFile(int fileType,
                                                        FPDF_WIDESTRING wsURL,
                                                        const char* mode) {
  if (m_pInfo && m_pInfo->FFI_OpenFile)
    return m_pInfo->FFI_OpenFile(m_pInfo, fileType, wsURL, mode);
  return nullptr;
}

RetainPtr<IFX_SeekableReadStream> CPDFSDK_FormFillEnvironment::DownloadFromURL(
    const WideString& url) {
  if (!m_pInfo || !m_pInfo->FFI_DownloadFromURL)
    return nullptr;

  ByteString bstrURL = url.ToUTF16LE();
  FPDF_LPFILEHANDLER fileHandler =
      m_pInfo->FFI_DownloadFromURL(m_pInfo, AsFPDFWideString(&bstrURL));

  return MakeSeekableStream(fileHandler);
}

WideString CPDFSDK_FormFillEnvironment::PostRequestURL(
    const WideString& wsURL,
    const WideString& wsData,
    const WideString& wsContentType,
    const WideString& wsEncode,
    const WideString& wsHeader) {
  if (!m_pInfo || !m_pInfo->FFI_PostRequestURL)
    return WideString();

  ByteString bsURL = wsURL.ToUTF16LE();
  ByteString bsData = wsData.ToUTF16LE();
  ByteString bsContentType = wsContentType.ToUTF16LE();
  ByteString bsEncode = wsEncode.ToUTF16LE();
  ByteString bsHeader = wsHeader.ToUTF16LE();

  FPDF_BSTR response;
  FPDF_BStr_Init(&response);
  m_pInfo->FFI_PostRequestURL(
      m_pInfo, AsFPDFWideString(&bsURL), AsFPDFWideString(&bsData),
      AsFPDFWideString(&bsContentType), AsFPDFWideString(&bsEncode),
      AsFPDFWideString(&bsHeader), &response);

  WideString wsRet =
      WideString::FromUTF16LE(reinterpret_cast<FPDF_WIDESTRING>(response.str),
                              response.len / sizeof(FPDF_WIDESTRING));

  FPDF_BStr_Clear(&response);
  return wsRet;
}

FPDF_BOOL CPDFSDK_FormFillEnvironment::PutRequestURL(
    const WideString& wsURL,
    const WideString& wsData,
    const WideString& wsEncode) {
  if (!m_pInfo || !m_pInfo->FFI_PutRequestURL)
    return false;

  ByteString bsURL = wsURL.ToUTF16LE();
  ByteString bsData = wsData.ToUTF16LE();
  ByteString bsEncode = wsEncode.ToUTF16LE();

  return m_pInfo->FFI_PutRequestURL(m_pInfo, AsFPDFWideString(&bsURL),
                                    AsFPDFWideString(&bsData),
                                    AsFPDFWideString(&bsEncode));
}

WideString CPDFSDK_FormFillEnvironment::GetLanguage() {
  if (!m_pInfo || !m_pInfo->FFI_GetLanguage)
    return WideString();

  int nRequiredLen = m_pInfo->FFI_GetLanguage(m_pInfo, nullptr, 0);
  if (nRequiredLen <= 0)
    return WideString();

  std::vector<uint8_t> pBuff(nRequiredLen);
  int nActualLen =
      m_pInfo->FFI_GetLanguage(m_pInfo, pBuff.data(), nRequiredLen);
  if (nActualLen <= 0 || nActualLen > nRequiredLen)
    return WideString();

  return WideString::FromUTF16LE(reinterpret_cast<uint16_t*>(pBuff.data()),
                                 nActualLen / sizeof(uint16_t));
}

void CPDFSDK_FormFillEnvironment::PageEvent(int iPageCount,
                                            uint32_t dwEventType) const {
  if (m_pInfo && m_pInfo->FFI_PageEvent)
    m_pInfo->FFI_PageEvent(m_pInfo, iPageCount, dwEventType);
}
#endif  // PDF_ENABLE_XFA

void CPDFSDK_FormFillEnvironment::ClearAllFocusedAnnots() {
  for (auto& it : m_PageMap) {
    if (it.second->IsValidSDKAnnot(GetFocusAnnot()))
      KillFocusAnnot(0);
  }
}

CPDFSDK_PageView* CPDFSDK_FormFillEnvironment::GetPageView(
    IPDF_Page* pUnderlyingPage,
    bool renew) {
  auto it = m_PageMap.find(pUnderlyingPage);
  if (it != m_PageMap.end())
    return it->second.get();

  if (!renew)
    return nullptr;

  auto pNew = pdfium::MakeUnique<CPDFSDK_PageView>(this, pUnderlyingPage);
  CPDFSDK_PageView* pPageView = pNew.get();
  m_PageMap[pUnderlyingPage] = std::move(pNew);

  // Delay to load all the annotations, to avoid endless loop.
  pPageView->LoadFXAnnots();
  return pPageView;
}

#ifdef PDF_ENABLE_V8
CPDFSDK_PageView* CPDFSDK_FormFillEnvironment::GetCurrentView() {
  IPDF_Page* pPage = IPDFPageFromFPDFPage(GetCurrentPage());
  return pPage ? GetPageView(pPage, true) : nullptr;
}
#endif

CPDFSDK_PageView* CPDFSDK_FormFillEnvironment::GetPageView(int nIndex) {
  IPDF_Page* pTempPage = GetPage(nIndex);
  if (!pTempPage)
    return nullptr;

  auto it = m_PageMap.find(pTempPage);
  return it != m_PageMap.end() ? it->second.get() : nullptr;
}

void CPDFSDK_FormFillEnvironment::ProcJavascriptFun() {
  CPDF_DocJSActions docJS(m_pCPDFDoc.Get());
  int iCount = docJS.CountJSActions();
  for (int i = 0; i < iCount; i++) {
    WideString csJSName;
    CPDF_Action jsAction = docJS.GetJSActionAndName(i, &csJSName);
    GetActionHandler()->DoAction_JavaScript(jsAction, csJSName, this);
  }
}

bool CPDFSDK_FormFillEnvironment::ProcOpenAction() {
  if (!m_pCPDFDoc)
    return false;

  const CPDF_Dictionary* pRoot = m_pCPDFDoc->GetRoot();
  if (!pRoot)
    return false;

  const CPDF_Object* pOpenAction = pRoot->GetDictFor("OpenAction");
  if (!pOpenAction)
    pOpenAction = pRoot->GetArrayFor("OpenAction");
  if (!pOpenAction)
    return false;

  if (pOpenAction->IsArray())
    return true;

  const CPDF_Dictionary* pDict = pOpenAction->AsDictionary();
  if (!pDict)
    return false;

  CPDF_Action action(pDict);
  GetActionHandler()->DoAction_DocOpen(action, this);
  return true;
}

void CPDFSDK_FormFillEnvironment::RemovePageView(IPDF_Page* pUnderlyingPage) {
  auto it = m_PageMap.find(pUnderlyingPage);
  if (it == m_PageMap.end())
    return;

  CPDFSDK_PageView* pPageView = it->second.get();
  if (pPageView->IsLocked() || pPageView->IsBeingDestroyed())
    return;

  // Mark the page view so we do not come into |RemovePageView| a second
  // time while we're in the process of removing.
  pPageView->SetBeingDestroyed();

  // This must happen before we remove |pPageView| from the map because
  // |KillFocusAnnot| can call into the |GetPage| method which will
  // look for this page view in the map, if it doesn't find it a new one will
  // be created. We then have two page views pointing to the same page and
  // bad things happen.
  if (pPageView->IsValidSDKAnnot(GetFocusAnnot()))
    KillFocusAnnot(0);

  // Remove the page from the map to make sure we don't accidentally attempt
  // to use the |pPageView| while we're cleaning it up.
  m_PageMap.erase(it);
}

IPDF_Page* CPDFSDK_FormFillEnvironment::GetPage(int nIndex) {
  if (!m_pInfo || !m_pInfo->FFI_GetPage)
    return nullptr;
  return IPDFPageFromFPDFPage(m_pInfo->FFI_GetPage(
      m_pInfo, FPDFDocumentFromCPDFDocument(m_pCPDFDoc.Get()), nIndex));
}

CPDFSDK_InteractiveForm* CPDFSDK_FormFillEnvironment::GetInteractiveForm() {
  if (!m_pInteractiveForm)
    m_pInteractiveForm = pdfium::MakeUnique<CPDFSDK_InteractiveForm>(this);
  return m_pInteractiveForm.get();
}

void CPDFSDK_FormFillEnvironment::UpdateAllViews(CPDFSDK_PageView* pSender,
                                                 CPDFSDK_Annot* pAnnot) {
  for (const auto& it : m_PageMap) {
    CPDFSDK_PageView* pPageView = it.second.get();
    if (pPageView != pSender)
      pPageView->UpdateView(pAnnot);
  }
}

bool CPDFSDK_FormFillEnvironment::SetFocusAnnot(
    CPDFSDK_Annot::ObservedPtr* pAnnot) {
//   if (m_bBeingDestroyed)
//     return false;
//   if (m_pFocusAnnot == *pAnnot)
//     return true;
//   if (m_pFocusAnnot && !KillFocusAnnot(0))
//     return false;
//   if (!pAnnot->HasObservable())
//     return false;
// 
//   CPDFSDK_PageView* pPageView = (*pAnnot)->GetPageView();
//   if (!pPageView || !pPageView->IsValid())
//     return false;
// 
//   CPDFSDK_AnnotHandlerMgr* pAnnotHandler = GetAnnotHandlerMgr();
//   if (m_pFocusAnnot)
//     return false;
// 
// #ifdef PDF_ENABLE_XFA
//   CPDFSDK_Annot::ObservedPtr pLastFocusAnnot(m_pFocusAnnot.Get());
//   if (!pAnnotHandler->Annot_OnChangeFocus(pAnnot, &pLastFocusAnnot))
//     return false;
// #endif  // PDF_ENABLE_XFA
//   if (!pAnnotHandler->Annot_OnSetFocus(pAnnot, 0))
//     return false;
//   if (m_pFocusAnnot)
//     return false;
// 
//   m_pFocusAnnot.Reset(pAnnot->Get());
//   return true;
  DebugBreak();
  return false;
}

bool CPDFSDK_FormFillEnvironment::KillFocusAnnot(uint32_t nFlag) {
//   if (!m_pFocusAnnot)
//     return false;
// 
//   CPDFSDK_AnnotHandlerMgr* pAnnotHandler = GetAnnotHandlerMgr();
//   CPDFSDK_Annot::ObservedPtr pFocusAnnot(m_pFocusAnnot.Get());
//   m_pFocusAnnot.Reset();
// 
// #ifdef PDF_ENABLE_XFA
//   CPDFSDK_Annot::ObservedPtr pNull;
//   if (!pAnnotHandler->Annot_OnChangeFocus(&pNull, &pFocusAnnot))
//     return false;
// #endif  // PDF_ENABLE_XFA
// 
//   if (!pAnnotHandler->Annot_OnKillFocus(&pFocusAnnot, nFlag)) {
//     m_pFocusAnnot.Reset(pFocusAnnot.Get());
//     return false;
//   }
// 
//   if (pFocusAnnot->GetAnnotSubtype() == CPDF_Annot::Subtype::WIDGET) {
//     CPDFSDK_Widget* pWidget = ToCPDFSDKWidget(pFocusAnnot.Get());
//     FormFieldType fieldType = pWidget->GetFieldType();
//     if (fieldType == FormFieldType::kTextField ||
//         fieldType == FormFieldType::kComboBox) {
//       OnSetFieldInputFocus(nullptr, 0, false);
//     }
//   }
//   return !m_pFocusAnnot;
  DebugBreak();
  return false;
}

#ifdef PDF_ENABLE_XFA
CPDFXFA_Context* CPDFSDK_FormFillEnvironment::GetXFAContext() const {
  if (!m_pCPDFDoc)
    return nullptr;
  return static_cast<CPDFXFA_Context*>(m_pCPDFDoc->GetExtension());
}

int CPDFSDK_FormFillEnvironment::GetPageViewCount() const {
  return pdfium::CollectionSize<int>(m_PageMap);
}
#endif  // PDF_ENABLE_XFA

int CPDFSDK_FormFillEnvironment::GetPageCount() const {
  CPDF_Document::Extension* pExtension = m_pCPDFDoc->GetExtension();
  return pExtension ? pExtension->GetPageCount() : m_pCPDFDoc->GetPageCount();
}

bool CPDFSDK_FormFillEnvironment::GetPermissions(int nFlag) const {
  return !!(m_pCPDFDoc->GetUserPermissions() & nFlag);
}

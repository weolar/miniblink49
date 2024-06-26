// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FPDFXFA_CPDFXFA_CONTEXT_H_
#define FPDFSDK_FPDFXFA_CPDFXFA_CONTEXT_H_

#include <memory>
#include <vector>

#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/observable.h"
#include "core/fxcrt/unowned_ptr.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "fpdfsdk/fpdfxfa/cpdfxfa_docenvironment.h"
#include "fpdfsdk/fpdfxfa/cpdfxfa_page.h"
#include "xfa/fxfa/cxfa_ffdoc.h"

class CJS_Runtime;
class CXFA_FFDocHandler;
class IJS_EventContext;
class IJS_Runtime;

enum LoadStatus {
  FXFA_LOADSTATUS_PRELOAD = 0,
  FXFA_LOADSTATUS_LOADING,
  FXFA_LOADSTATUS_LOADED,
  FXFA_LOADSTATUS_CLOSING,
  FXFA_LOADSTATUS_CLOSED
};

class CPDFXFA_Context final : public CPDF_Document::Extension,
                              public IXFA_AppProvider {
 public:
  explicit CPDFXFA_Context(CPDF_Document* pPDFDoc);
  ~CPDFXFA_Context() override;

  bool LoadXFADoc();
  CXFA_FFDoc* GetXFADoc() {
    DebugBreak();
    return nullptr;
    //return m_pXFADoc.get();
  }
  CXFA_FFDocView* GetXFADocView() const { return m_pXFADocView.Get(); }
  FormType GetFormType() const { return m_FormType; }
  bool ContainsXFAForm() const {
    return m_FormType == FormType::kXFAFull ||
           m_FormType == FormType::kXFAForeground;
  }
  CXFA_FFApp* GetXFAApp() { 
    //return m_pXFAApp.get();
    DebugBreak();
    return nullptr;
  }

  CPDFSDK_FormFillEnvironment* GetFormFillEnv() const {
    return m_pFormFillEnv.Get();
  }
  void SetFormFillEnv(CPDFSDK_FormFillEnvironment* pFormFillEnv);

  RetainPtr<CPDFXFA_Page> GetXFAPage(int page_index);
  RetainPtr<CPDFXFA_Page> GetXFAPage(CXFA_FFPageView* pPage) const;
  void ClearChangeMark();

  // CPDF_Document::Extension:
  CPDF_Document* GetPDFDoc() const override;
  int GetPageCount() const override;
  void DeletePage(int page_index) override;
  uint32_t GetUserPermissions() const override;

  // IFXA_AppProvider:
  WideString GetLanguage() override;
  WideString GetPlatform() override;
  WideString GetAppName() override;
  WideString GetAppTitle() const override;

  void Beep(uint32_t dwType) override;
  int32_t MsgBox(const WideString& wsMessage,
                 const WideString& wsTitle,
                 uint32_t dwIconType,
                 uint32_t dwButtonType) override;
  WideString Response(const WideString& wsQuestion,
                      const WideString& wsTitle,
                      const WideString& wsDefaultAnswer,
                      bool bMark) override;
  RetainPtr<IFX_SeekableReadStream> DownloadURL(
      const WideString& wsURL) override;
  bool PostRequestURL(const WideString& wsURL,
                      const WideString& wsData,
                      const WideString& wsContentType,
                      const WideString& wsEncode,
                      const WideString& wsHeader,
                      WideString& wsResponse) override;
  bool PutRequestURL(const WideString& wsURL,
                     const WideString& wsData,
                     const WideString& wsEncode) override;

  std::unique_ptr<IFWL_AdapterTimerMgr> NewTimerMgr() override;

 private:
  friend class CPDFXFA_DocEnvironment;

  int GetOriginalPageCount() const { return m_nPageCount; }
  void SetOriginalPageCount(int count) {
    m_nPageCount = count;
    m_XFAPageList.resize(count);
  }

  LoadStatus GetLoadStatus() const { return m_nLoadStatus; }
  std::vector<RetainPtr<CPDFXFA_Page>>* GetXFAPageList() {
    return &m_XFAPageList;
  }

  CJS_Runtime* GetCJSRuntime() const;
  void CloseXFADoc();

  FormType m_FormType = FormType::kNone;
  UnownedPtr<CPDF_Document> const m_pPDFDoc;
  //std::unique_ptr<CXFA_FFDoc> m_pXFADoc;
  Observable<CPDFSDK_FormFillEnvironment>::ObservedPtr m_pFormFillEnv;
  UnownedPtr<CXFA_FFDocView> m_pXFADocView;
  //std::unique_ptr<CXFA_FFApp> m_pXFAApp;
  //std::unique_ptr<CJS_Runtime> m_pRuntime;
  std::vector<RetainPtr<CPDFXFA_Page>> m_XFAPageList;
  LoadStatus m_nLoadStatus = FXFA_LOADSTATUS_PRELOAD;
  int m_nPageCount = 0;

  // Must be destroyed before |m_pFormFillEnv|.
  //CPDFXFA_DocEnvironment m_DocEnv;
};

#endif  // FPDFSDK_FPDFXFA_CPDFXFA_CONTEXT_H_

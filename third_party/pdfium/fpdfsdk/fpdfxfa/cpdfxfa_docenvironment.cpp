// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/fpdfxfa/cpdfxfa_docenvironment.h"

#include <memory>
#include <utility>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fxcrt/retain_ptr.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fpdfsdk/cpdfsdk_interactiveform.h"
#include "fpdfsdk/cpdfsdk_pageview.h"
#include "fpdfsdk/fpdfxfa/cpdfxfa_context.h"
#include "fpdfsdk/fpdfxfa/cpdfxfa_page.h"
//#include "fxjs/ijs_runtime.h"
#include "xfa/fxfa/cxfa_ffdocview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/cxfa_ffwidgethandler.h"
#include "xfa/fxfa/cxfa_readynodeiterator.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/cxfa_submit.h"

#define IDS_XFA_Validate_Input                                          \
  "At least one required field was empty. Please fill in the required " \
  "fields\r\n(highlighted) before continuing."

// submit
#define FXFA_CONFIG 0x00000001
#define FXFA_TEMPLATE 0x00000010
#define FXFA_LOCALESET 0x00000100
#define FXFA_DATASETS 0x00001000
#define FXFA_XMPMETA 0x00010000
#define FXFA_XFDF 0x00100000
#define FXFA_FORM 0x01000000
#define FXFA_PDF 0x10000000
#define FXFA_XFA_ALL 0x01111111

CPDFXFA_DocEnvironment::CPDFXFA_DocEnvironment(CPDFXFA_Context* pContext)
    : m_pContext(pContext) {
  ASSERT(m_pContext);
}

CPDFXFA_DocEnvironment::~CPDFXFA_DocEnvironment() {}

void CPDFXFA_DocEnvironment::SetChangeMark(CXFA_FFDoc* hDoc) {
  if (hDoc == m_pContext->GetXFADoc() && m_pContext->GetFormFillEnv())
    m_pContext->GetFormFillEnv()->SetChangeMark();
}

void CPDFXFA_DocEnvironment::InvalidateRect(CXFA_FFPageView* pPageView,
                                            const CFX_RectF& rt) {
  if (!m_pContext->GetXFADoc() || !m_pContext->GetFormFillEnv())
    return;

  if (m_pContext->GetFormType() != FormType::kXFAFull)
    return;

  RetainPtr<CPDFXFA_Page> pPage = m_pContext->GetXFAPage(pPageView);
  if (!pPage)
    return;

  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  if (!pFormFillEnv)
    return;

  pFormFillEnv->Invalidate(pPage.Get(), rt.ToFloatRect().ToFxRect());
}

void CPDFXFA_DocEnvironment::DisplayCaret(CXFA_FFWidget* hWidget,
                                          bool bVisible,
                                          const CFX_RectF* pRtAnchor) {
  if (!hWidget || !pRtAnchor || !m_pContext->GetXFADoc() ||
      !m_pContext->GetFormFillEnv() || !m_pContext->GetXFADocView())
    return;

  if (m_pContext->GetFormType() != FormType::kXFAFull)
    return;

  CXFA_FFWidgetHandler* pWidgetHandler =
      m_pContext->GetXFADocView()->GetWidgetHandler();
  if (!pWidgetHandler)
    return;

  CXFA_FFPageView* pPageView = hWidget->GetPageView();
  if (!pPageView)
    return;

  RetainPtr<CPDFXFA_Page> pPage = m_pContext->GetXFAPage(pPageView);
  if (!pPage)
    return;

  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  if (!pFormFillEnv)
    return;

  CFX_FloatRect rcCaret = pRtAnchor->ToFloatRect();
  pFormFillEnv->DisplayCaret(pPage.Get(), bVisible, rcCaret.left, rcCaret.top,
                             rcCaret.right, rcCaret.bottom);
}

bool CPDFXFA_DocEnvironment::GetPopupPos(CXFA_FFWidget* hWidget,
                                         float fMinPopup,
                                         float fMaxPopup,
                                         const CFX_RectF& rtAnchor,
                                         CFX_RectF* pPopupRect) {
  if (!hWidget)
    return false;

  CXFA_FFPageView* pXFAPageView = hWidget->GetPageView();
  if (!pXFAPageView)
    return false;

  RetainPtr<CPDFXFA_Page> pPage = m_pContext->GetXFAPage(pXFAPageView);
  if (!pPage)
    return false;

  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  if (!pFormFillEnv)
    return false;

  FS_RECTF page_view_rect = pFormFillEnv->GetPageViewRect(pPage.Get());
  int nRotate = hWidget->GetNode()->GetRotate();

  int space_available_below_anchor;
  int space_available_above_anchor;
  switch (nRotate) {
    case 0:
    default: {
      space_available_below_anchor =
          static_cast<int>(page_view_rect.bottom - rtAnchor.bottom());
      space_available_above_anchor =
          static_cast<int>(rtAnchor.top - page_view_rect.top);

      if (rtAnchor.left < page_view_rect.left)
        pPopupRect->left += page_view_rect.left - rtAnchor.left;
      if (rtAnchor.right() > page_view_rect.right)
        pPopupRect->left -= rtAnchor.right() - page_view_rect.right;
      break;
    }
    case 90: {
      space_available_below_anchor =
          static_cast<int>(page_view_rect.right - rtAnchor.right());
      space_available_above_anchor =
          static_cast<int>(rtAnchor.left - page_view_rect.left);

      if (rtAnchor.bottom() > page_view_rect.bottom)
        pPopupRect->left += rtAnchor.bottom() - page_view_rect.bottom;
      if (rtAnchor.top < page_view_rect.top)
        pPopupRect->left -= page_view_rect.top - rtAnchor.top;
      break;
    }
    case 180: {
      space_available_below_anchor =
          static_cast<int>(rtAnchor.top - page_view_rect.top);
      space_available_above_anchor =
          static_cast<int>(page_view_rect.bottom - rtAnchor.bottom());

      if (rtAnchor.right() > page_view_rect.right)
        pPopupRect->left += rtAnchor.right() - page_view_rect.right;
      if (rtAnchor.left < page_view_rect.left)
        pPopupRect->left -= page_view_rect.left - rtAnchor.left;
      break;
    }
    case 270: {
      space_available_below_anchor =
          static_cast<int>(rtAnchor.left - page_view_rect.left);
      space_available_above_anchor =
          static_cast<int>(page_view_rect.right - rtAnchor.right());

      if (rtAnchor.top < page_view_rect.top)
        pPopupRect->left += page_view_rect.top - rtAnchor.top;
      if (rtAnchor.bottom() > page_view_rect.bottom)
        pPopupRect->left -= rtAnchor.bottom() - page_view_rect.bottom;
      break;
    }
  }

  // If there is no space on either side, the popup can't be rendered.
  if (space_available_below_anchor <= 0 && space_available_above_anchor <= 0)
    return false;

  // Determine whether to draw above or below the anchor.
  bool draw_below_anchor;
  if (space_available_below_anchor <= 0)
    draw_below_anchor = false;
  else if (space_available_above_anchor <= 0)
    draw_below_anchor = true;
  else if (space_available_below_anchor > space_available_above_anchor)
    draw_below_anchor = true;
  else
    draw_below_anchor = false;

  int space_available = (draw_below_anchor ? space_available_below_anchor
                                           : space_available_above_anchor);

  // Set the popup height and y position according to what was decided above.
  float popup_height;
  if (space_available < fMinPopup)
    popup_height = fMinPopup;
  else if (space_available > fMaxPopup)
    popup_height = fMaxPopup;
  else
    popup_height = static_cast<float>(space_available);

  switch (nRotate) {
    case 0:
    case 180: {
      if (draw_below_anchor)
        pPopupRect->top = rtAnchor.height;
      else
        pPopupRect->top = -popup_height;
      break;
    }
    case 90:
    case 270: {
      if (draw_below_anchor)
        pPopupRect->top = rtAnchor.width;
      else
        pPopupRect->top = -popup_height;
      break;
    }
    default:
      break;
  }

  pPopupRect->height = popup_height;

  return true;
}

bool CPDFXFA_DocEnvironment::PopupMenu(CXFA_FFWidget* hWidget,
                                       CFX_PointF ptPopup) {
  if (!hWidget)
    return false;

  CXFA_FFPageView* pXFAPageView = hWidget->GetPageView();
  if (!pXFAPageView)
    return false;

  RetainPtr<CPDFXFA_Page> pPage = m_pContext->GetXFAPage(pXFAPageView);
  if (!pPage)
    return false;

  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  if (!pFormFillEnv)
    return false;

  int menuFlag = 0;
  if (hWidget->CanUndo())
    menuFlag |= FXFA_MENU_UNDO;
  if (hWidget->CanRedo())
    menuFlag |= FXFA_MENU_REDO;
  if (hWidget->CanPaste())
    menuFlag |= FXFA_MENU_PASTE;
  if (hWidget->CanCopy())
    menuFlag |= FXFA_MENU_COPY;
  if (hWidget->CanCut())
    menuFlag |= FXFA_MENU_CUT;
  if (hWidget->CanSelectAll())
    menuFlag |= FXFA_MENU_SELECTALL;

  return pFormFillEnv->PopupMenu(
      pPage.Get(), FPDFWidgetFromCXFAFFWidget(hWidget), menuFlag, ptPopup);
}

void CPDFXFA_DocEnvironment::PageViewEvent(CXFA_FFPageView* pPageView,
                                           uint32_t dwFlags) {
  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  if (!pFormFillEnv)
    return;

  if (m_pContext->GetLoadStatus() == FXFA_LOADSTATUS_LOADING ||
      m_pContext->GetLoadStatus() == FXFA_LOADSTATUS_CLOSING ||
      XFA_PAGEVIEWEVENT_StopLayout != dwFlags)
    return;

  int nNewCount = m_pContext->GetPageCount();
  if (nNewCount == m_pContext->GetOriginalPageCount())
    return;

  CXFA_FFDocView* pXFADocView = m_pContext->GetXFADocView();
  if (!pXFADocView)
    return;

  for (int iPageIter = 0; iPageIter < m_pContext->GetOriginalPageCount();
       iPageIter++) {
    RetainPtr<CPDFXFA_Page> pPage = (*m_pContext->GetXFAPageList())[iPageIter];
    if (!pPage)
      continue;

    m_pContext->GetFormFillEnv()->RemovePageView(pPage.Get());
    pPage->SetXFAPageViewIndex(iPageIter);
  }

  int flag = (nNewCount < m_pContext->GetOriginalPageCount())
                 ? FXFA_PAGEVIEWEVENT_POSTREMOVED
                 : FXFA_PAGEVIEWEVENT_POSTADDED;
  int count = abs(nNewCount - m_pContext->GetOriginalPageCount());
  m_pContext->SetOriginalPageCount(nNewCount);
  pFormFillEnv->PageEvent(count, flag);
}

void CPDFXFA_DocEnvironment::WidgetPostAdd(CXFA_FFWidget* hWidget) {
  if (m_pContext->GetFormType() != FormType::kXFAFull || !hWidget)
    return;

  CXFA_FFPageView* pPageView = hWidget->GetPageView();
  if (!pPageView)
    return;

  RetainPtr<CPDFXFA_Page> pXFAPage = m_pContext->GetXFAPage(pPageView);
  if (!pXFAPage)
    return;

  m_pContext->GetFormFillEnv()
      ->GetPageView(pXFAPage.Get(), true)
      ->AddAnnot(hWidget);
}

void CPDFXFA_DocEnvironment::WidgetPreRemove(CXFA_FFWidget* hWidget) {
  if (m_pContext->GetFormType() != FormType::kXFAFull || !hWidget)
    return;

  CXFA_FFPageView* pPageView = hWidget->GetPageView();
  if (!pPageView)
    return;

  RetainPtr<CPDFXFA_Page> pXFAPage = m_pContext->GetXFAPage(pPageView);
  if (!pXFAPage)
    return;

  CPDFSDK_PageView* pSdkPageView =
      m_pContext->GetFormFillEnv()->GetPageView(pXFAPage.Get(), true);
  CPDFSDK_Annot* pAnnot = pSdkPageView->GetAnnotByXFAWidget(hWidget);
  if (pAnnot)
    pSdkPageView->DeleteAnnot(pAnnot);
}

int32_t CPDFXFA_DocEnvironment::CountPages(CXFA_FFDoc* hDoc) {
  if (hDoc == m_pContext->GetXFADoc() && m_pContext->GetFormFillEnv())
    return m_pContext->GetPageCount();
  return 0;
}

int32_t CPDFXFA_DocEnvironment::GetCurrentPage(CXFA_FFDoc* hDoc) {
  if (hDoc != m_pContext->GetXFADoc() || !m_pContext->GetFormFillEnv())
    return -1;

  if (m_pContext->GetFormType() != FormType::kXFAFull)
    return -1;

  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  return pFormFillEnv ? pFormFillEnv->GetCurrentPageIndex() : -1;
}

void CPDFXFA_DocEnvironment::SetCurrentPage(CXFA_FFDoc* hDoc,
                                            int32_t iCurPage) {
  if (hDoc != m_pContext->GetXFADoc() || !m_pContext->GetFormFillEnv() ||
      !m_pContext->ContainsXFAForm() || iCurPage < 0 ||
      iCurPage >= m_pContext->GetFormFillEnv()->GetPageCount()) {
    return;
  }

  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  if (!pFormFillEnv)
    return;

  pFormFillEnv->SetCurrentPage(iCurPage);
}

bool CPDFXFA_DocEnvironment::IsCalculationsEnabled(CXFA_FFDoc* hDoc) {
  if (hDoc != m_pContext->GetXFADoc() || !m_pContext->GetFormFillEnv())
    return false;
  auto* pForm = m_pContext->GetFormFillEnv()->GetInteractiveForm();
  return pForm->IsXfaCalculateEnabled();
}

void CPDFXFA_DocEnvironment::SetCalculationsEnabled(CXFA_FFDoc* hDoc,
                                                    bool bEnabled) {
  if (hDoc != m_pContext->GetXFADoc() || !m_pContext->GetFormFillEnv())
    return;
  m_pContext->GetFormFillEnv()->GetInteractiveForm()->XfaEnableCalculate(
      bEnabled);
}

void CPDFXFA_DocEnvironment::GetTitle(CXFA_FFDoc* hDoc, WideString& wsTitle) {
  if (hDoc != m_pContext->GetXFADoc() || !m_pContext->GetPDFDoc())
    return;

  const CPDF_Dictionary* pInfoDict = m_pContext->GetPDFDoc()->GetInfo();
  if (!pInfoDict)
    return;

  ByteString csTitle = pInfoDict->GetStringFor("Title");
  wsTitle = WideString::FromDefANSI(csTitle.AsStringView());
}

void CPDFXFA_DocEnvironment::SetTitle(CXFA_FFDoc* hDoc,
                                      const WideString& wsTitle) {
  if (hDoc != m_pContext->GetXFADoc() || !m_pContext->GetPDFDoc())
    return;

  CPDF_Dictionary* pInfoDict = m_pContext->GetPDFDoc()->GetInfo();
  if (pInfoDict)
    pInfoDict->SetNewFor<CPDF_String>("Title", wsTitle);
}

void CPDFXFA_DocEnvironment::ExportData(CXFA_FFDoc* hDoc,
                                        const WideString& wsFilePath,
                                        bool bXDP) {
  if (hDoc != m_pContext->GetXFADoc())
    return;

  if (!m_pContext->ContainsXFAForm())
    return;

  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  if (!pFormFillEnv)
    return;

  int fileType = bXDP ? FXFA_SAVEAS_XDP : FXFA_SAVEAS_XML;
  ByteString bs = wsFilePath.ToUTF16LE();
  if (wsFilePath.IsEmpty()) {
    if (!pFormFillEnv->GetFormFillInfo() ||
        !pFormFillEnv->GetFormFillInfo()->m_pJsPlatform) {
      return;
    }

    WideString filepath = pFormFillEnv->JS_fieldBrowse();
    bs = filepath.ToUTF16LE();
  }
  FPDF_FILEHANDLER* pFileHandler = pFormFillEnv->OpenFile(
      bXDP ? FXFA_SAVEAS_XDP : FXFA_SAVEAS_XML, AsFPDFWideString(&bs), "wb");
  if (!pFileHandler)
    return;

  RetainPtr<IFX_SeekableStream> fileWrite = MakeSeekableStream(pFileHandler);
  if (fileType == FXFA_SAVEAS_XML) {
    ByteString content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";
    fileWrite->WriteBlockAtOffset(content.c_str(), fileWrite->GetSize(),
                                  content.GetLength());
    CXFA_FFDoc* ffdoc = m_pContext->GetXFADocView()->GetDoc();
    ffdoc->SavePackage(
        ToNode(ffdoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Data)), fileWrite);
  } else if (fileType == FXFA_SAVEAS_XDP) {
    if (!m_pContext->GetPDFDoc())
      return;

    const CPDF_Dictionary* pRoot = m_pContext->GetPDFDoc()->GetRoot();
    if (!pRoot)
      return;

    const CPDF_Dictionary* pAcroForm = pRoot->GetDictFor("AcroForm");
    if (!pAcroForm)
      return;

    const CPDF_Array* pArray = ToArray(pAcroForm->GetObjectFor("XFA"));
    if (!pArray)
      return;

    for (size_t i = 1; i < pArray->size(); i += 2) {
      const CPDF_Object* pPDFObj = pArray->GetObjectAt(i);
      const CPDF_Object* pPrePDFObj = pArray->GetObjectAt(i - 1);
      if (!pPrePDFObj->IsString())
        continue;
      if (!pPDFObj->IsReference())
        continue;

      const CPDF_Stream* pStream = ToStream(pPDFObj->GetDirect());
      if (!pStream)
        continue;
      if (pPrePDFObj->GetString() == "form") {
        CXFA_FFDoc* ffdoc = m_pContext->GetXFADocView()->GetDoc();
        ffdoc->SavePackage(
            ToNode(ffdoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Form)),
            fileWrite);
        continue;
      }
      if (pPrePDFObj->GetString() == "datasets") {
        CXFA_FFDoc* ffdoc = m_pContext->GetXFADocView()->GetDoc();
        ffdoc->SavePackage(
            ToNode(ffdoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Datasets)),
            fileWrite);
        continue;
      }
      if (i == pArray->size() - 1) {
        WideString wPath = WideString::FromUTF16LE(
            reinterpret_cast<const unsigned short*>(bs.c_str()),
            bs.GetLength() / sizeof(unsigned short));
        ByteString bPath = wPath.ToUTF8();
        static const char kFormat[] =
            "\n<pdf href=\"%s\" xmlns=\"http://ns.adobe.com/xdp/pdf/\"/>";
        ByteString content = ByteString::Format(kFormat, bPath.c_str());
        fileWrite->WriteBlockAtOffset(content.c_str(), fileWrite->GetSize(),
                                      content.GetLength());
      }
      auto pAcc = pdfium::MakeRetain<CPDF_StreamAcc>(pStream);
      pAcc->LoadAllDataFiltered();
      fileWrite->WriteBlockAtOffset(pAcc->GetData(), fileWrite->GetSize(),
                                    pAcc->GetSize());
    }
  }
  fileWrite->Flush();
}

void CPDFXFA_DocEnvironment::GotoURL(CXFA_FFDoc* hDoc,
                                     const WideString& wsURL) {
  if (hDoc != m_pContext->GetXFADoc())
    return;

  if (m_pContext->GetFormType() != FormType::kXFAFull)
    return;

  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  if (!pFormFillEnv)
    return;

  pFormFillEnv->GotoURL(wsURL);
}

bool CPDFXFA_DocEnvironment::IsValidationsEnabled(CXFA_FFDoc* hDoc) {
  if (hDoc != m_pContext->GetXFADoc() || !m_pContext->GetFormFillEnv())
    return false;

  auto* pForm = m_pContext->GetFormFillEnv()->GetInteractiveForm();
  return pForm->IsXfaValidationsEnabled();
}

void CPDFXFA_DocEnvironment::SetValidationsEnabled(CXFA_FFDoc* hDoc,
                                                   bool bEnabled) {
  if (hDoc != m_pContext->GetXFADoc() || !m_pContext->GetFormFillEnv())
    return;

  m_pContext->GetFormFillEnv()->GetInteractiveForm()->XfaSetValidationsEnabled(
      bEnabled);
}

void CPDFXFA_DocEnvironment::SetFocusWidget(CXFA_FFDoc* hDoc,
                                            CXFA_FFWidget* hWidget) {
  if (hDoc != m_pContext->GetXFADoc())
    return;

  if (!hWidget) {
    CPDFSDK_Annot::ObservedPtr pNull;
    m_pContext->GetFormFillEnv()->SetFocusAnnot(&pNull);
    return;
  }

  int pageViewCount = m_pContext->GetFormFillEnv()->GetPageViewCount();
  for (int i = 0; i < pageViewCount; i++) {
    CPDFSDK_PageView* pPageView = m_pContext->GetFormFillEnv()->GetPageView(i);
    if (!pPageView)
      continue;

    CPDFSDK_Annot::ObservedPtr pAnnot(pPageView->GetAnnotByXFAWidget(hWidget));
    if (pAnnot) {
      m_pContext->GetFormFillEnv()->SetFocusAnnot(&pAnnot);
      break;
    }
  }
}

void CPDFXFA_DocEnvironment::Print(CXFA_FFDoc* hDoc,
                                   int32_t nStartPage,
                                   int32_t nEndPage,
                                   uint32_t dwOptions) {
  if (hDoc != m_pContext->GetXFADoc())
    return;

  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  if (!pFormFillEnv || !pFormFillEnv->GetFormFillInfo() ||
      !pFormFillEnv->GetFormFillInfo()->m_pJsPlatform ||
      !pFormFillEnv->GetFormFillInfo()->m_pJsPlatform->Doc_print) {
    return;
  }

  pFormFillEnv->GetFormFillInfo()->m_pJsPlatform->Doc_print(
      pFormFillEnv->GetFormFillInfo()->m_pJsPlatform,
      dwOptions & XFA_PRINTOPT_ShowDialog, nStartPage, nEndPage,
      dwOptions & XFA_PRINTOPT_CanCancel, dwOptions & XFA_PRINTOPT_ShrinkPage,
      dwOptions & XFA_PRINTOPT_AsImage, dwOptions & XFA_PRINTOPT_ReverseOrder,
      dwOptions & XFA_PRINTOPT_PrintAnnot);
}

FX_ARGB CPDFXFA_DocEnvironment::GetHighlightColor(CXFA_FFDoc* hDoc) {
  if (hDoc != m_pContext->GetXFADoc() || !m_pContext->GetFormFillEnv())
    return 0;

  CPDFSDK_InteractiveForm* pForm =
      m_pContext->GetFormFillEnv()->GetInteractiveForm();
  return AlphaAndColorRefToArgb(pForm->GetHighlightAlpha(),
                                pForm->GetHighlightColor(FormFieldType::kXFA));
}

#ifdef PDF_XFA_ELEMENT_SUBMIT_ENABLED
bool CPDFXFA_DocEnvironment::NotifySubmit(bool bPrevOrPost) {
  if (bPrevOrPost)
    return OnBeforeNotifySubmit();

  OnAfterNotifySubmit();
  return true;
}

bool CPDFXFA_DocEnvironment::OnBeforeNotifySubmit() {
  if (!m_pContext->ContainsXFAForm())
    return true;

  CXFA_FFDocView* docView = m_pContext->GetXFADocView();
  if (!docView)
    return true;

  CXFA_FFWidgetHandler* pWidgetHandler = docView->GetWidgetHandler();
  if (!pWidgetHandler)
    return true;

  auto it = docView->CreateReadyNodeIterator();
  if (it) {
    CXFA_EventParam Param;
    Param.m_eType = XFA_EVENT_PreSubmit;
    while (CXFA_Node* pNode = it->MoveToNext())
      pWidgetHandler->ProcessEvent(pNode, &Param);
  }

  it = docView->CreateReadyNodeIterator();
  if (!it)
    return true;

  (void)it->MoveToNext();
  CXFA_Node* pNode = it->MoveToNext();

  while (pNode) {
    int fRet = pNode->ProcessValidate(docView, -1);
    if (fRet == XFA_EVENTERROR_Error) {
      CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
      if (!pFormFillEnv)
        return false;

      pFormFillEnv->JS_appAlert(WideString::FromDefANSI(IDS_XFA_Validate_Input),
                                WideString(), JSPLATFORM_ALERT_BUTTON_OK,
                                JSPLATFORM_ALERT_ICON_WARNING);
      return false;
    }
    pNode = it->MoveToNext();
  }

  docView->UpdateDocView();
  return true;
}

void CPDFXFA_DocEnvironment::OnAfterNotifySubmit() {
  if (!m_pContext->ContainsXFAForm())
    return;

  if (!m_pContext->GetXFADocView())
    return;

  CXFA_FFWidgetHandler* pWidgetHandler =
      m_pContext->GetXFADocView()->GetWidgetHandler();
  if (!pWidgetHandler)
    return;

  auto it = m_pContext->GetXFADocView()->CreateReadyNodeIterator();
  if (!it)
    return;

  CXFA_EventParam Param;
  Param.m_eType = XFA_EVENT_PostSubmit;
  CXFA_Node* pNode = it->MoveToNext();
  while (pNode) {
    pWidgetHandler->ProcessEvent(pNode, &Param);
    pNode = it->MoveToNext();
  }
  m_pContext->GetXFADocView()->UpdateDocView();
}

bool CPDFXFA_DocEnvironment::Submit(CXFA_FFDoc* hDoc, CXFA_Submit* submit) {
  if (!NotifySubmit(true) || !m_pContext->GetXFADocView())
    return false;

  m_pContext->GetXFADocView()->UpdateDocView();
  bool ret = SubmitInternal(hDoc, submit);
  NotifySubmit(false);
  return ret;
}
#endif  // PDF_XFA_ELEMENT_SUBMIT_ENABLED

RetainPtr<IFX_SeekableReadStream> CPDFXFA_DocEnvironment::OpenLinkedFile(
    CXFA_FFDoc* hDoc,
    const WideString& wsLink) {
  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  if (!pFormFillEnv)
    return nullptr;

  ByteString bs = wsLink.ToUTF16LE();
  FPDF_FILEHANDLER* pFileHandler =
      pFormFillEnv->OpenFile(0, AsFPDFWideString(&bs), "rb");
  if (!pFileHandler)
    return nullptr;

  return MakeSeekableStream(pFileHandler);
}

bool CPDFXFA_DocEnvironment::ExportSubmitFile(FPDF_FILEHANDLER* pFileHandler,
                                              int fileType,
                                              FPDF_DWORD encodeType,
                                              FPDF_DWORD flag) {
  if (!m_pContext->GetXFADocView())
    return false;

  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  if (!pFormFillEnv)
    return false;

  CXFA_FFDoc* ffdoc = m_pContext->GetXFADocView()->GetDoc();
  RetainPtr<IFX_SeekableStream> fileStream = MakeSeekableStream(pFileHandler);
  if (fileType == FXFA_SAVEAS_XML) {
    static constexpr char kContent[] =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";
    fileStream->WriteBlockAtOffset(kContent, 0, strlen(kContent));

    ffdoc->SavePackage(
        ToNode(ffdoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Data)),
        fileStream);
    return true;
  }

  if (fileType != FXFA_SAVEAS_XDP)
    return true;

  if (!flag) {
    flag = FXFA_CONFIG | FXFA_TEMPLATE | FXFA_LOCALESET | FXFA_DATASETS |
           FXFA_XMPMETA | FXFA_XFDF | FXFA_FORM;
  }
  if (!m_pContext->GetPDFDoc()) {
    fileStream->Flush();
    return false;
  }

  const CPDF_Dictionary* pRoot = m_pContext->GetPDFDoc()->GetRoot();
  if (!pRoot) {
    fileStream->Flush();
    return false;
  }

  const CPDF_Dictionary* pAcroForm = pRoot->GetDictFor("AcroForm");
  if (!pAcroForm) {
    fileStream->Flush();
    return false;
  }

  const CPDF_Array* pArray = ToArray(pAcroForm->GetObjectFor("XFA"));
  if (!pArray) {
    fileStream->Flush();
    return false;
  }

  for (size_t i = 1; i < pArray->size(); i += 2) {
    const CPDF_Object* pPDFObj = pArray->GetObjectAt(i);
    const CPDF_Object* pPrePDFObj = pArray->GetObjectAt(i - 1);
    if (!pPrePDFObj->IsString())
      continue;
    if (!pPDFObj->IsReference())
      continue;

    const CPDF_Object* pDirectObj = pPDFObj->GetDirect();
    if (!pDirectObj->IsStream())
      continue;
    ByteString bsType = pPrePDFObj->GetString();
    if (bsType == "config" && !(flag & FXFA_CONFIG))
      continue;
    if (bsType == "template" && !(flag & FXFA_TEMPLATE))
      continue;
    if (bsType == "localeSet" && !(flag & FXFA_LOCALESET))
      continue;
    if (bsType == "datasets" && !(flag & FXFA_DATASETS))
      continue;
    if (bsType == "xmpmeta" && !(flag & FXFA_XMPMETA))
      continue;
    if (bsType == "xfdf" && !(flag & FXFA_XFDF))
      continue;
    if (bsType == "form" && !(flag & FXFA_FORM))
      continue;

    if (bsType == "form") {
      ffdoc->SavePackage(
          ToNode(ffdoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Form)),
          fileStream);
    } else if (pPrePDFObj->GetString() == "datasets") {
      ffdoc->SavePackage(
          ToNode(ffdoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Datasets)),
          fileStream);
    }
  }
  return true;
}

void CPDFXFA_DocEnvironment::ToXFAContentFlags(WideString csSrcContent,
                                               FPDF_DWORD& flag) {
  if (csSrcContent.Contains(L" config "))
    flag |= FXFA_CONFIG;
  if (csSrcContent.Contains(L" template "))
    flag |= FXFA_TEMPLATE;
  if (csSrcContent.Contains(L" localeSet "))
    flag |= FXFA_LOCALESET;
  if (csSrcContent.Contains(L" datasets "))
    flag |= FXFA_DATASETS;
  if (csSrcContent.Contains(L" xmpmeta "))
    flag |= FXFA_XMPMETA;
  if (csSrcContent.Contains(L" xfdf "))
    flag |= FXFA_XFDF;
  if (csSrcContent.Contains(L" form "))
    flag |= FXFA_FORM;
  if (flag == 0) {
    flag = FXFA_CONFIG | FXFA_TEMPLATE | FXFA_LOCALESET | FXFA_DATASETS |
           FXFA_XMPMETA | FXFA_XFDF | FXFA_FORM;
  }
}

bool CPDFXFA_DocEnvironment::MailToInfo(WideString& csURL,
                                        WideString& csToAddress,
                                        WideString& csCCAddress,
                                        WideString& csBCCAddress,
                                        WideString& csSubject,
                                        WideString& csMsg) {
  WideString srcURL = csURL;
  srcURL.TrimLeft();
  if (srcURL.Left(7).CompareNoCase(L"mailto:") != 0)
    return false;

  auto pos = srcURL.Find(L'?');

  {
    WideString tmp;
    if (!pos.has_value()) {
      pos = srcURL.Find(L'@');
      if (!pos.has_value())
        return false;

      tmp = srcURL.Right(csURL.GetLength() - 7);
    } else {
      tmp = srcURL.Left(pos.value());
      tmp = tmp.Right(tmp.GetLength() - 7);
    }
    tmp.Trim();
    csToAddress = std::move(tmp);
  }

  srcURL = srcURL.Right(srcURL.GetLength() - (pos.value() + 1));
  while (!srcURL.IsEmpty()) {
    srcURL.Trim();
    pos = srcURL.Find(L'&');
    WideString tmp = (!pos.has_value()) ? srcURL : srcURL.Left(pos.value());
    tmp.Trim();
    if (tmp.GetLength() >= 3 && tmp.Left(3).CompareNoCase(L"cc=") == 0) {
      tmp = tmp.Right(tmp.GetLength() - 3);
      if (!csCCAddress.IsEmpty())
        csCCAddress += L';';
      csCCAddress += tmp;
    } else if (tmp.GetLength() >= 4 &&
               tmp.Left(4).CompareNoCase(L"bcc=") == 0) {
      tmp = tmp.Right(tmp.GetLength() - 4);
      if (!csBCCAddress.IsEmpty())
        csBCCAddress += L';';
      csBCCAddress += tmp;
    } else if (tmp.GetLength() >= 8 &&
               tmp.Left(8).CompareNoCase(L"subject=") == 0) {
      tmp = tmp.Right(tmp.GetLength() - 8);
      csSubject += tmp;
    } else if (tmp.GetLength() >= 5 &&
               tmp.Left(5).CompareNoCase(L"body=") == 0) {
      tmp = tmp.Right(tmp.GetLength() - 5);
      csMsg += tmp;
    }
    srcURL = pos.has_value()
                 ? srcURL.Right(csURL.GetLength() - (pos.value() + 1))
                 : WideString();
  }
  csToAddress.Replace(L",", L";");
  csCCAddress.Replace(L",", L";");
  csBCCAddress.Replace(L",", L";");
  return true;
}

#ifdef PDF_XFA_ELEMENT_SUBMIT_ENABLED
bool CPDFXFA_DocEnvironment::SubmitInternal(CXFA_FFDoc* hDoc,
                                            CXFA_Submit* submit) {
  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  if (!pFormFillEnv)
    return false;

  WideString csURL = submit->GetSubmitTarget();
  if (csURL.IsEmpty()) {
    pFormFillEnv->JS_appAlert(WideString::FromDefANSI("Submit cancelled."),
                              WideString(), JSPLATFORM_ALERT_BUTTON_OK,
                              JSPLATFORM_ALERT_ICON_ASTERISK);
    return false;
  }

  FPDF_FILEHANDLER* pFileHandler = nullptr;
  int fileFlag = -1;
  switch (submit->GetSubmitFormat()) {
    case XFA_AttributeValue::Xdp: {
      WideString csContent = submit->GetSubmitXDPContent();
      csContent.Trim();

      WideString space = WideString::FromDefANSI(" ");
      csContent = space + csContent + space;
      FPDF_DWORD flag = 0;
      if (submit->IsSubmitEmbedPDF())
        flag |= FXFA_PDF;

      ToXFAContentFlags(csContent, flag);
      pFileHandler = pFormFillEnv->OpenFile(FXFA_SAVEAS_XDP, nullptr, "wb");
      fileFlag = FXFA_SAVEAS_XDP;
      ExportSubmitFile(pFileHandler, FXFA_SAVEAS_XDP, 0, flag);
      break;
    }
    case XFA_AttributeValue::Xml:
      pFileHandler = pFormFillEnv->OpenFile(FXFA_SAVEAS_XML, nullptr, "wb");
      fileFlag = FXFA_SAVEAS_XML;
      ExportSubmitFile(pFileHandler, FXFA_SAVEAS_XML, 0, FXFA_XFA_ALL);
      break;
    case XFA_AttributeValue::Pdf:
      break;
    case XFA_AttributeValue::Urlencoded:
      pFileHandler = pFormFillEnv->OpenFile(FXFA_SAVEAS_XML, nullptr, "wb");
      fileFlag = FXFA_SAVEAS_XML;
      ExportSubmitFile(pFileHandler, FXFA_SAVEAS_XML, 0, FXFA_XFA_ALL);
      break;
    default:
      return false;
  }
  if (!pFileHandler)
    return false;

  if (csURL.Left(7).CompareNoCase(L"mailto:") == 0) {
    WideString csToAddress;
    WideString csCCAddress;
    WideString csBCCAddress;
    WideString csSubject;
    WideString csMsg;
    if (!MailToInfo(csURL, csToAddress, csCCAddress, csBCCAddress, csSubject,
                    csMsg)) {
      return false;
    }
    ByteString bsTo = WideString(csToAddress).ToUTF16LE();
    ByteString bsCC = WideString(csCCAddress).ToUTF16LE();
    ByteString bsBcc = WideString(csBCCAddress).ToUTF16LE();
    ByteString bsSubject = WideString(csSubject).ToUTF16LE();
    ByteString bsMsg = WideString(csMsg).ToUTF16LE();
    pFormFillEnv->EmailTo(pFileHandler, AsFPDFWideString(&bsTo),
                          AsFPDFWideString(&bsSubject), AsFPDFWideString(&bsCC),
                          AsFPDFWideString(&bsBcc), AsFPDFWideString(&bsMsg));
    return true;
  }

  // HTTP or FTP
  ByteString bs = csURL.ToUTF16LE();
  pFormFillEnv->UploadTo(pFileHandler, fileFlag, AsFPDFWideString(&bs));
  return true;
}
#endif  // PDF_XFA_ELEMENT_SUBMIT_ENABLED

bool CPDFXFA_DocEnvironment::SetPropertyInNonXFAGlobalObject(
    CXFA_FFDoc* hDoc,
    ByteStringView szPropName,
    CFXJSE_Value* pValue) {
  if (hDoc != m_pContext->GetXFADoc())
    return false;

  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  if (!pFormFillEnv)
    return false;

  IJS_Runtime* pIJSRuntime = pFormFillEnv->GetIJSRuntime();
  if (!pIJSRuntime)
    return false;

  IJS_Runtime::ScopedEventContext pContext(pIJSRuntime);
  return pIJSRuntime->SetValueByNameInGlobalObject(szPropName, pValue);
}

bool CPDFXFA_DocEnvironment::GetPropertyFromNonXFAGlobalObject(
    CXFA_FFDoc* hDoc,
    ByteStringView szPropName,
    CFXJSE_Value* pValue) {
  if (hDoc != m_pContext->GetXFADoc())
    return false;

  CPDFSDK_FormFillEnvironment* pFormFillEnv = m_pContext->GetFormFillEnv();
  if (!pFormFillEnv)
    return false;

  IJS_Runtime* pIJSRuntime = pFormFillEnv->GetIJSRuntime();
  if (!pIJSRuntime)
    return false;

  IJS_Runtime::ScopedEventContext pContext(pIJSRuntime);
  return pIJSRuntime->GetValueByNameFromGlobalObject(szPropName, pValue);
}

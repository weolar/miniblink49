// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_ffdoc.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfdoc/cpdf_nametree.h"
#include "core/fxcrt/cfx_readonlymemorystream.h"
#include "core/fxcrt/cfx_seekablemultistream.h"
#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/xml/cfx_xmldocument.h"
#include "core/fxcrt/xml/cfx_xmlelement.h"
#include "core/fxcrt/xml/cfx_xmlnode.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "fxjs/xfa/cjx_object.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fgas/font/cfgas_pdffontmgr.h"
#include "xfa/fwl/cfwl_notedriver.h"
#include "xfa/fxfa/cxfa_ffapp.h"
#include "xfa/fxfa/cxfa_ffdocview.h"
#include "xfa/fxfa/cxfa_ffnotify.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/cxfa_fontmgr.h"
#include "xfa/fxfa/parser/cxfa_acrobat.h"
#include "xfa/fxfa/parser/cxfa_acrobat7.h"
#include "xfa/fxfa/parser/cxfa_dataexporter.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_document_parser.h"
#include "xfa/fxfa/parser/cxfa_dynamicrender.h"
#include "xfa/fxfa/parser/cxfa_node.h"

FX_IMAGEDIB_AND_DPI::FX_IMAGEDIB_AND_DPI() = default;
FX_IMAGEDIB_AND_DPI::FX_IMAGEDIB_AND_DPI(const FX_IMAGEDIB_AND_DPI& that) =
    default;

FX_IMAGEDIB_AND_DPI::FX_IMAGEDIB_AND_DPI(const RetainPtr<CFX_DIBBase>& pDib,
                                         int32_t xDpi,
                                         int32_t yDpi)
    : pDibSource(pDib), iImageXDpi(xDpi), iImageYDpi(yDpi) {}

FX_IMAGEDIB_AND_DPI::~FX_IMAGEDIB_AND_DPI() = default;

CXFA_FFDoc::CXFA_FFDoc(CXFA_FFApp* pApp, IXFA_DocEnvironment* pDocEnvironment)
    : m_pDocEnvironment(pDocEnvironment), m_pApp(pApp) {}

CXFA_FFDoc::~CXFA_FFDoc() {
  CloseDoc();
}

bool CXFA_FFDoc::ParseDoc(const CPDF_Object* pElementXFA) {
  std::vector<const CPDF_Stream*> xfaStreams;
  if (pElementXFA->IsArray()) {
    const CPDF_Array* pXFAArray = pElementXFA->AsArray();
    for (size_t i = 0; i < pXFAArray->size() / 2; i++) {
      if (const CPDF_Stream* pStream = pXFAArray->GetStreamAt(i * 2 + 1))
        xfaStreams.push_back(pStream);
    }
  } else if (pElementXFA->IsStream()) {
    xfaStreams.push_back(pElementXFA->AsStream());
  }
  if (xfaStreams.empty())
    return false;

  auto stream = pdfium::MakeRetain<CFX_SeekableMultiStream>(xfaStreams);

  CXFA_DocumentParser parser(m_pDocument.get());
  bool parsed = parser.Parse(stream, XFA_PacketType::Xdp);

  // We have to set the XML document before we return so that we can clean
  // up in the OpenDoc method. If we don't, the XMLDocument will get free'd
  // when this method returns and UnownedPtrs get unhappy.
  m_pXMLDoc = parser.GetXMLDoc();

  if (!parsed)
    return false;

  m_pDocument->SetRoot(parser.GetRootNode());
  return true;
}

CXFA_FFDocView* CXFA_FFDoc::CreateDocView() {
  if (!m_DocView)
    m_DocView = pdfium::MakeUnique<CXFA_FFDocView>(this);

  return m_DocView.get();
}

CXFA_FFDocView* CXFA_FFDoc::GetDocView(CXFA_LayoutProcessor* pLayout) {
  return m_DocView && m_DocView->GetXFALayout() == pLayout ? m_DocView.get()
                                                           : nullptr;
}

CXFA_FFDocView* CXFA_FFDoc::GetDocView() {
  return m_DocView.get();
}

bool CXFA_FFDoc::OpenDoc(CPDF_Document* pPDFDoc) {
  if (!pPDFDoc)
    return false;

  const CPDF_Dictionary* pRoot = pPDFDoc->GetRoot();
  if (!pRoot)
    return false;

  const CPDF_Dictionary* pAcroForm = pRoot->GetDictFor("AcroForm");
  if (!pAcroForm)
    return false;

  const CPDF_Object* pElementXFA = pAcroForm->GetDirectObjectFor("XFA");
  if (!pElementXFA)
    return false;

  m_pPDFDoc = pPDFDoc;

  m_pNotify = pdfium::MakeUnique<CXFA_FFNotify>(this);
  m_pDocument = pdfium::MakeUnique<CXFA_Document>(m_pNotify.get());
  if (!ParseDoc(pElementXFA)) {
    CloseDoc();
    return false;
  }

  CFGAS_FontMgr* mgr = GetApp()->GetFDEFontMgr();
  if (!mgr)
    return false;

  // At this point we've got an XFA document and we want to always return
  // true to signify the load succeeded.
  m_pPDFFontMgr = pdfium::MakeUnique<CFGAS_PDFFontMgr>(GetPDFDoc(), mgr);

  m_FormType = FormType::kXFAForeground;
  CXFA_Node* pConfig = ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Config));
  if (!pConfig)
    return true;

  CXFA_Acrobat* pAcrobat =
      pConfig->GetFirstChildByClass<CXFA_Acrobat>(XFA_Element::Acrobat);
  if (!pAcrobat)
    return true;

  CXFA_Acrobat7* pAcrobat7 =
      pAcrobat->GetFirstChildByClass<CXFA_Acrobat7>(XFA_Element::Acrobat7);
  if (!pAcrobat7)
    return true;

  CXFA_DynamicRender* pDynamicRender =
      pAcrobat7->GetFirstChildByClass<CXFA_DynamicRender>(
          XFA_Element::DynamicRender);
  if (!pDynamicRender)
    return true;

  WideString wsType = pDynamicRender->JSObject()->GetContent(false);
  if (wsType.EqualsASCII("required"))
    m_FormType = FormType::kXFAFull;

  return true;
}

void CXFA_FFDoc::CloseDoc() {
  if (m_DocView) {
    m_DocView->RunDocClose();
    m_DocView.reset();
  }
  if (m_pDocument)
    m_pDocument->ClearLayoutData();

  m_pDocument.reset();
  m_pXMLDoc.reset();
  m_pNotify.reset();
  m_pPDFFontMgr.reset();
  m_HashToDibDpiMap.clear();
  m_pApp->ClearEventTargets();
}

RetainPtr<CFX_DIBitmap> CXFA_FFDoc::GetPDFNamedImage(WideStringView wsName,
                                                     int32_t& iImageXDpi,
                                                     int32_t& iImageYDpi) {
  if (!m_pPDFDoc)
    return nullptr;

  uint32_t dwHash = FX_HashCode_GetW(wsName, false);
  auto it = m_HashToDibDpiMap.find(dwHash);
  if (it != m_HashToDibDpiMap.end()) {
    iImageXDpi = it->second.iImageXDpi;
    iImageYDpi = it->second.iImageYDpi;
    return it->second.pDibSource.As<CFX_DIBitmap>();
  }

  CPDF_Dictionary* pRoot = m_pPDFDoc->GetRoot();
  if (!pRoot)
    return nullptr;

  CPDF_Dictionary* pNames = pRoot->GetDictFor("Names");
  if (!pNames)
    return nullptr;

  CPDF_Dictionary* pXFAImages = pNames->GetDictFor("XFAImages");
  if (!pXFAImages)
    return nullptr;

  CPDF_NameTree nametree(pXFAImages);
  CPDF_Object* pObject = nametree.LookupValue(WideString(wsName));
  if (!pObject) {
    for (size_t i = 0; i < nametree.GetCount(); i++) {
      WideString wsTemp;
      CPDF_Object* pTempObject = nametree.LookupValueAndName(i, &wsTemp);
      if (wsTemp == wsName) {
        pObject = pTempObject;
        break;
      }
    }
  }

  CPDF_Stream* pStream = ToStream(pObject);
  if (!pStream)
    return nullptr;

  auto pAcc = pdfium::MakeRetain<CPDF_StreamAcc>(pStream);
  pAcc->LoadAllDataFiltered();

  auto pImageFileRead =
      pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(pAcc->GetSpan());

  RetainPtr<CFX_DIBitmap> pDibSource = XFA_LoadImageFromBuffer(
      pImageFileRead, FXCODEC_IMAGE_UNKNOWN, iImageXDpi, iImageYDpi);
  m_HashToDibDpiMap[dwHash] = {pDibSource, iImageXDpi, iImageYDpi};
  return pDibSource;
}

bool CXFA_FFDoc::SavePackage(CXFA_Node* pNode,
                             const RetainPtr<IFX_SeekableStream>& pFile) {
  ASSERT(pNode || GetXFADoc()->GetRoot());

  CXFA_DataExporter exporter;
  return exporter.Export(pFile, pNode ? pNode : GetXFADoc()->GetRoot());
}

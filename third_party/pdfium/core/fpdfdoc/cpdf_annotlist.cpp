// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_annotlist.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfapi/render/cpdf_renderoptions.h"
#include "core/fpdfdoc/cpdf_annot.h"
#include "core/fpdfdoc/cpdf_formfield.h"
#include "core/fpdfdoc/cpdf_interactiveform.h"
#include "core/fpdfdoc/cpdf_occontext.h"
#include "core/fpdfdoc/cpvt_generateap.h"
#include "core/fxge/cfx_renderdevice.h"
#include "third_party/base/ptr_util.h"

namespace {

bool PopupAppearsForAnnotType(CPDF_Annot::Subtype subtype) {
  switch (subtype) {
    case CPDF_Annot::Subtype::TEXT:
    case CPDF_Annot::Subtype::LINE:
    case CPDF_Annot::Subtype::SQUARE:
    case CPDF_Annot::Subtype::CIRCLE:
    case CPDF_Annot::Subtype::POLYGON:
    case CPDF_Annot::Subtype::POLYLINE:
    case CPDF_Annot::Subtype::HIGHLIGHT:
    case CPDF_Annot::Subtype::UNDERLINE:
    case CPDF_Annot::Subtype::SQUIGGLY:
    case CPDF_Annot::Subtype::STRIKEOUT:
    case CPDF_Annot::Subtype::STAMP:
    case CPDF_Annot::Subtype::CARET:
    case CPDF_Annot::Subtype::INK:
    case CPDF_Annot::Subtype::FILEATTACHMENT:
      return true;
    case CPDF_Annot::Subtype::UNKNOWN:
    case CPDF_Annot::Subtype::LINK:
    case CPDF_Annot::Subtype::FREETEXT:
    case CPDF_Annot::Subtype::POPUP:
    case CPDF_Annot::Subtype::SOUND:
    case CPDF_Annot::Subtype::MOVIE:
    case CPDF_Annot::Subtype::WIDGET:
    case CPDF_Annot::Subtype::SCREEN:
    case CPDF_Annot::Subtype::PRINTERMARK:
    case CPDF_Annot::Subtype::TRAPNET:
    case CPDF_Annot::Subtype::WATERMARK:
    case CPDF_Annot::Subtype::THREED:
    case CPDF_Annot::Subtype::RICHMEDIA:
    case CPDF_Annot::Subtype::XFAWIDGET:
    default:
      return false;
  }
}

std::unique_ptr<CPDF_Annot> CreatePopupAnnot(CPDF_Annot* pAnnot,
                                             CPDF_Document* pDocument,
                                             CPDF_Page* pPage) {
  if (!PopupAppearsForAnnotType(pAnnot->GetSubtype()))
    return nullptr;

  CPDF_Dictionary* pParentDict = pAnnot->GetAnnotDict();
  if (!pParentDict)
    return nullptr;

  // TODO(jaepark): We shouldn't strip BOM for some strings and not for others.
  // See pdfium:593.
  WideString sContents = pParentDict->GetUnicodeTextFor("Contents");
  if (sContents.IsEmpty())
    return nullptr;

  auto pAnnotDict = pDocument->New<CPDF_Dictionary>();
  pAnnotDict->SetNewFor<CPDF_Name>("Type", "Annot");
  pAnnotDict->SetNewFor<CPDF_Name>("Subtype", "Popup");
  pAnnotDict->SetNewFor<CPDF_String>("T", pParentDict->GetStringFor("T"),
                                     false);
  pAnnotDict->SetNewFor<CPDF_String>("Contents", sContents.ToUTF8(), false);

  CFX_FloatRect rect = pParentDict->GetRectFor("Rect");
  rect.Normalize();
  CFX_FloatRect popupRect(0, 0, 200, 200);
  // Note that if the popup can set its own dimensions, then we will need to
  // make sure that it isn't larger than the page size.
  if (rect.left + popupRect.Width() > pPage->GetPageWidth() &&
      rect.bottom - popupRect.Height() < 0) {
    // If the annotation is on the bottom-right corner of the page, then place
    // the popup above and to the left of the annotation.
    popupRect.Translate(rect.right - popupRect.Width(), rect.top);
  } else {
    // Place the popup below and to the right of the annotation without getting
    // clipped by page edges.
    popupRect.Translate(
        std::min(rect.left, pPage->GetPageWidth() - popupRect.Width()),
        std::max(rect.bottom - popupRect.Height(), 0.f));
  }

  pAnnotDict->SetRectFor("Rect", popupRect);
  pAnnotDict->SetNewFor<CPDF_Number>("F", 0);

  auto pPopupAnnot =
      pdfium::MakeUnique<CPDF_Annot>(std::move(pAnnotDict), pDocument);
  pAnnot->SetPopupAnnot(pPopupAnnot.get());
  return pPopupAnnot;
}

void GenerateAP(CPDF_Document* pDoc, CPDF_Dictionary* pAnnotDict) {
  if (!pAnnotDict || pAnnotDict->GetStringFor("Subtype") != "Widget")
    return;

  CPDF_Object* pFieldTypeObj = FPDF_GetFieldAttr(pAnnotDict, "FT");
  if (!pFieldTypeObj)
    return;

  ByteString field_type = pFieldTypeObj->GetString();
  if (field_type == "Tx") {
    CPVT_GenerateAP::GenerateFormAP(CPVT_GenerateAP::kTextField, pDoc,
                                    pAnnotDict);
    return;
  }

  CPDF_Object* pFieldFlagsObj = FPDF_GetFieldAttr(pAnnotDict, "Ff");
  uint32_t flags = pFieldFlagsObj ? pFieldFlagsObj->GetInteger() : 0;
  if (field_type == "Ch") {
    CPVT_GenerateAP::GenerateFormAP((flags & (1 << 17))
                                        ? CPVT_GenerateAP::kComboBox
                                        : CPVT_GenerateAP::kListBox,
                                    pDoc, pAnnotDict);
    return;
  }

  if (field_type != "Btn")
    return;
  if (flags & (1 << 16))
    return;
  if (pAnnotDict->KeyExist("AS"))
    return;

  CPDF_Dictionary* pParentDict = pAnnotDict->GetDictFor("Parent");
  if (!pParentDict || !pParentDict->KeyExist("AS"))
    return;

  pAnnotDict->SetNewFor<CPDF_String>("AS", pParentDict->GetStringFor("AS"),
                                     false);
  return;
}

}  // namespace

CPDF_AnnotList::CPDF_AnnotList(CPDF_Page* pPage)
    : m_pDocument(pPage->GetDocument()) {
  if (!pPage->GetDict())
    return;

  CPDF_Array* pAnnots = pPage->GetDict()->GetArrayFor("Annots");
  if (!pAnnots)
    return;

  const CPDF_Dictionary* pRoot = m_pDocument->GetRoot();
  const CPDF_Dictionary* pAcroForm = pRoot->GetDictFor("AcroForm");
  bool bRegenerateAP =
      pAcroForm && pAcroForm->GetBooleanFor("NeedAppearances", false);
  for (size_t i = 0; i < pAnnots->size(); ++i) {
    CPDF_Dictionary* pDict = ToDictionary(pAnnots->GetDirectObjectAt(i));
    if (!pDict)
      continue;
    const ByteString subtype = pDict->GetStringFor("Subtype");
    if (subtype == "Popup") {
      // Skip creating Popup annotations in the PDF document since PDFium
      // provides its own Popup annotations.
      continue;
    }
    pAnnots->ConvertToIndirectObjectAt(i, m_pDocument);
    m_AnnotList.push_back(pdfium::MakeUnique<CPDF_Annot>(pDict, m_pDocument));
    if (bRegenerateAP && subtype == "Widget" &&
        CPDF_InteractiveForm::IsUpdateAPEnabled() && !pDict->GetDictFor("AP")) {
      GenerateAP(m_pDocument, pDict);
    }
  }

  size_t nAnnotListSize = m_AnnotList.size();
  for (size_t i = 0; i < nAnnotListSize; ++i) {
    std::unique_ptr<CPDF_Annot> pPopupAnnot(
        CreatePopupAnnot(m_AnnotList[i].get(), m_pDocument, pPage));
    if (pPopupAnnot)
      m_AnnotList.push_back(std::move(pPopupAnnot));
  }
}

CPDF_AnnotList::~CPDF_AnnotList() {}

void CPDF_AnnotList::DisplayPass(CPDF_Page* pPage,
                                 CFX_RenderDevice* pDevice,
                                 CPDF_RenderContext* pContext,
                                 bool bPrinting,
                                 const CFX_Matrix* pMatrix,
                                 bool bWidgetPass,
                                 CPDF_RenderOptions* pOptions,
                                 FX_RECT* clip_rect) {
  for (const auto& pAnnot : m_AnnotList) {
    bool bWidget = pAnnot->GetSubtype() == CPDF_Annot::Subtype::WIDGET;
    if ((bWidgetPass && !bWidget) || (!bWidgetPass && bWidget))
      continue;

    uint32_t annot_flags = pAnnot->GetFlags();
    if (annot_flags & ANNOTFLAG_HIDDEN)
      continue;

    if (bPrinting && (annot_flags & ANNOTFLAG_PRINT) == 0)
      continue;

    if (!bPrinting && (annot_flags & ANNOTFLAG_NOVIEW))
      continue;

    if (pOptions) {
      CPDF_Dictionary* pAnnotDict = pAnnot->GetAnnotDict();
      if (pOptions->GetOCContext() && pAnnotDict &&
          !pOptions->GetOCContext()->CheckOCGVisible(
              pAnnotDict->GetDictFor("OC"))) {
        continue;
      }
    }

    CFX_Matrix matrix = *pMatrix;
    if (clip_rect) {
      FX_RECT annot_rect =
          matrix.TransformRect(pAnnot->GetRect()).GetOuterRect();
      annot_rect.Intersect(*clip_rect);
      if (annot_rect.IsEmpty())
        continue;
    }
    if (pContext) {
      pAnnot->DrawInContext(pPage, pContext, &matrix, CPDF_Annot::Normal);
    } else if (!pAnnot->DrawAppearance(pPage, pDevice, matrix,
                                       CPDF_Annot::Normal, pOptions)) {
      pAnnot->DrawBorder(pDevice, &matrix, pOptions);
    }
  }
}

void CPDF_AnnotList::DisplayAnnots(CPDF_Page* pPage,
                                   CFX_RenderDevice* pDevice,
                                   CPDF_RenderContext* pContext,
                                   bool bPrinting,
                                   const CFX_Matrix* pUser2Device,
                                   uint32_t dwAnnotFlags,
                                   CPDF_RenderOptions* pOptions,
                                   FX_RECT* pClipRect) {
  if (dwAnnotFlags & ANNOTFLAG_INVISIBLE) {
    DisplayPass(pPage, pDevice, pContext, bPrinting, pUser2Device, false,
                pOptions, pClipRect);
  }
  if (dwAnnotFlags & ANNOTFLAG_HIDDEN) {
    DisplayPass(pPage, pDevice, pContext, bPrinting, pUser2Device, true,
                pOptions, pClipRect);
  }
}

void CPDF_AnnotList::DisplayAnnots(CPDF_Page* pPage,
                                   CPDF_RenderContext* pContext,
                                   bool bPrinting,
                                   const CFX_Matrix* pMatrix,
                                   bool bShowWidget,
                                   CPDF_RenderOptions* pOptions) {
  uint32_t dwAnnotFlags = bShowWidget ? ANNOTFLAG_INVISIBLE | ANNOTFLAG_HIDDEN
                                      : ANNOTFLAG_INVISIBLE;
  DisplayAnnots(pPage, nullptr, pContext, bPrinting, pMatrix, dwAnnotFlags,
                pOptions, nullptr);
}

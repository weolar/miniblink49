// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_annot.h"

#include <algorithm>
#include <utility>

#include "core/fpdfapi/page/cpdf_form.h"
#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_boolean.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/render/cpdf_rendercontext.h"
#include "core/fpdfapi/render/cpdf_renderoptions.h"
#include "core/fpdfdoc/cpvt_generateap.h"
#include "core/fxge/cfx_graphstatedata.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/cfx_renderdevice.h"
#include "third_party/base/ptr_util.h"

namespace {

const char kPDFiumKey_HasGeneratedAP[] = "PDFIUM_HasGeneratedAP";

bool IsTextMarkupAnnotation(CPDF_Annot::Subtype type) {
  return type == CPDF_Annot::Subtype::HIGHLIGHT ||
         type == CPDF_Annot::Subtype::SQUIGGLY ||
         type == CPDF_Annot::Subtype::STRIKEOUT ||
         type == CPDF_Annot::Subtype::UNDERLINE;
}

bool ShouldGenerateAPForAnnotation(CPDF_Dictionary* pAnnotDict) {
  // If AP dictionary exists and defines an appearance for normal mode, we use
  // the appearance defined in the existing AP dictionary.
  CPDF_Dictionary* pAP = pAnnotDict->GetDictFor("AP");
  if (pAP && pAP->GetDictFor("N"))
    return false;

  return !CPDF_Annot::IsAnnotationHidden(pAnnotDict);
}

CPDF_Form* AnnotGetMatrix(const CPDF_Page* pPage,
                          CPDF_Annot* pAnnot,
                          CPDF_Annot::AppearanceMode mode,
                          const CFX_Matrix* pUser2Device,
                          CFX_Matrix* matrix) {
  CPDF_Form* pForm = pAnnot->GetAPForm(pPage, mode);
  if (!pForm)
    return nullptr;

  CFX_Matrix form_matrix = pForm->GetDict()->GetMatrixFor("Matrix");
  CFX_FloatRect form_bbox =
      form_matrix.TransformRect(pForm->GetDict()->GetRectFor("BBox"));
  matrix->MatchRect(pAnnot->GetRect(), form_bbox);
  matrix->Concat(*pUser2Device);
  return pForm;
}

CPDF_Stream* GetAnnotAPInternal(CPDF_Dictionary* pAnnotDict,
                                CPDF_Annot::AppearanceMode eMode,
                                bool bFallbackToNormal) {
  CPDF_Dictionary* pAP = pAnnotDict->GetDictFor("AP");
  if (!pAP)
    return nullptr;

  const char* ap_entry = "N";
  if (eMode == CPDF_Annot::Down)
    ap_entry = "D";
  else if (eMode == CPDF_Annot::Rollover)
    ap_entry = "R";
  if (bFallbackToNormal && !pAP->KeyExist(ap_entry))
    ap_entry = "N";

  CPDF_Object* psub = pAP->GetDirectObjectFor(ap_entry);
  if (!psub)
    return nullptr;
  if (CPDF_Stream* pStream = psub->AsStream())
    return pStream;

  CPDF_Dictionary* pDict = psub->AsDictionary();
  if (!pDict)
    return nullptr;

  ByteString as = pAnnotDict->GetStringFor("AS");
  if (as.IsEmpty()) {
    ByteString value = pAnnotDict->GetStringFor("V");
    if (value.IsEmpty()) {
      const CPDF_Dictionary* pParentDict = pAnnotDict->GetDictFor("Parent");
      value = pParentDict ? pParentDict->GetStringFor("V") : ByteString();
    }
    as = (!value.IsEmpty() && pDict->KeyExist(value)) ? value : "Off";
  }
  return pDict->GetStreamFor(as);
}

}  // namespace

CPDF_Annot::CPDF_Annot(std::unique_ptr<CPDF_Dictionary> pDict,
                       CPDF_Document* pDocument)
    : m_pAnnotDict(std::move(pDict)), m_pDocument(pDocument) {
  Init();
}

CPDF_Annot::CPDF_Annot(CPDF_Dictionary* pDict, CPDF_Document* pDocument)
    : m_pAnnotDict(pDict), m_pDocument(pDocument) {
  Init();
}

CPDF_Annot::~CPDF_Annot() {
  ClearCachedAP();
}

void CPDF_Annot::Init() {
  m_nSubtype = StringToAnnotSubtype(m_pAnnotDict->GetStringFor("Subtype"));
  m_bIsTextMarkupAnnotation = IsTextMarkupAnnotation(m_nSubtype);
  m_bHasGeneratedAP =
      m_pAnnotDict->GetBooleanFor(kPDFiumKey_HasGeneratedAP, false);
  GenerateAPIfNeeded();
}

void CPDF_Annot::GenerateAPIfNeeded() {
  if (!ShouldGenerateAPForAnnotation(m_pAnnotDict.Get()))
    return;
  if (!CPVT_GenerateAP::GenerateAnnotAP(m_nSubtype, m_pDocument.Get(),
                                        m_pAnnotDict.Get())) {
    return;
  }

  m_pAnnotDict->SetNewFor<CPDF_Boolean>(kPDFiumKey_HasGeneratedAP, true);
  m_bHasGeneratedAP = true;
}

bool CPDF_Annot::ShouldDrawAnnotation() {
  if (IsAnnotationHidden(m_pAnnotDict.Get()))
    return false;

  if (m_nSubtype == CPDF_Annot::Subtype::POPUP && !m_bOpenState)
    return false;

  return true;
}

void CPDF_Annot::ClearCachedAP() {
  m_APMap.clear();
}

CPDF_Annot::Subtype CPDF_Annot::GetSubtype() const {
  return m_nSubtype;
}

CFX_FloatRect CPDF_Annot::RectForDrawing() const {
  if (!m_pAnnotDict)
    return CFX_FloatRect();

  bool bShouldUseQuadPointsCoords =
      m_bIsTextMarkupAnnotation && m_bHasGeneratedAP;
  if (bShouldUseQuadPointsCoords)
    return BoundingRectFromQuadPoints(m_pAnnotDict.Get());

  return m_pAnnotDict->GetRectFor("Rect");
}

CFX_FloatRect CPDF_Annot::GetRect() const {
  if (!m_pAnnotDict)
    return CFX_FloatRect();

  CFX_FloatRect rect = RectForDrawing();
  rect.Normalize();
  return rect;
}

uint32_t CPDF_Annot::GetFlags() const {
  return m_pAnnotDict->GetIntegerFor("F");
}

CPDF_Stream* GetAnnotAP(CPDF_Dictionary* pAnnotDict,
                        CPDF_Annot::AppearanceMode eMode) {
  return GetAnnotAPInternal(pAnnotDict, eMode, true);
}

CPDF_Stream* GetAnnotAPNoFallback(CPDF_Dictionary* pAnnotDict,
                                  CPDF_Annot::AppearanceMode eMode) {
  return GetAnnotAPInternal(pAnnotDict, eMode, false);
}

CPDF_Form* CPDF_Annot::GetAPForm(const CPDF_Page* pPage, AppearanceMode mode) {
  CPDF_Stream* pStream = GetAnnotAP(m_pAnnotDict.Get(), mode);
  if (!pStream)
    return nullptr;

  auto it = m_APMap.find(pStream);
  if (it != m_APMap.end())
    return it->second.get();

  auto pNewForm = pdfium::MakeUnique<CPDF_Form>(
      m_pDocument.Get(), pPage->m_pResources.Get(), pStream);
  pNewForm->ParseContent(nullptr, nullptr, nullptr, nullptr);

  CPDF_Form* pResult = pNewForm.get();
  m_APMap[pStream] = std::move(pNewForm);
  return pResult;
}

// static
CFX_FloatRect CPDF_Annot::RectFromQuadPointsArray(const CPDF_Array* pArray,
                                                  size_t nIndex) {
  ASSERT(pArray);
  ASSERT(nIndex < pArray->size() / 8);

  // QuadPoints are defined with 4 pairs of numbers
  // ([ pair0, pair1, pair2, pair3 ]), where
  // pair0 = top_left
  // pair1 = top_right
  // pair2 = bottom_left
  // pair3 = bottom_right
  //
  // On the other hand, /Rect is defined as 2 pairs [pair0, pair1] where:
  // pair0 = bottom_left
  // pair1 = top_right.

  return CFX_FloatRect(
      pArray->GetNumberAt(4 + nIndex * 8), pArray->GetNumberAt(5 + nIndex * 8),
      pArray->GetNumberAt(2 + nIndex * 8), pArray->GetNumberAt(3 + nIndex * 8));
}

// static
CFX_FloatRect CPDF_Annot::BoundingRectFromQuadPoints(
    const CPDF_Dictionary* pAnnotDict) {
  CFX_FloatRect ret;
  const CPDF_Array* pArray = pAnnotDict->GetArrayFor("QuadPoints");
  size_t nQuadPointCount = pArray ? QuadPointCount(pArray) : 0;
  if (nQuadPointCount == 0)
    return ret;

  ret = RectFromQuadPointsArray(pArray, 0);
  for (size_t i = 1; i < nQuadPointCount; ++i) {
    CFX_FloatRect rect = RectFromQuadPointsArray(pArray, i);
    ret.Union(rect);
  }
  return ret;
}

// static
CFX_FloatRect CPDF_Annot::RectFromQuadPoints(const CPDF_Dictionary* pAnnotDict,
                                             size_t nIndex) {
  const CPDF_Array* pArray = pAnnotDict->GetArrayFor("QuadPoints");
  size_t nQuadPointCount = pArray ? QuadPointCount(pArray) : 0;
  if (nIndex >= nQuadPointCount)
    return CFX_FloatRect();
  return RectFromQuadPointsArray(pArray, nIndex);
}

// static
bool CPDF_Annot::IsAnnotationHidden(CPDF_Dictionary* pAnnotDict) {
  return !!(pAnnotDict->GetIntegerFor("F") & ANNOTFLAG_HIDDEN);
}

// static
CPDF_Annot::Subtype CPDF_Annot::StringToAnnotSubtype(
    const ByteString& sSubtype) {
  if (sSubtype == "Text")
    return CPDF_Annot::Subtype::TEXT;
  if (sSubtype == "Link")
    return CPDF_Annot::Subtype::LINK;
  if (sSubtype == "FreeText")
    return CPDF_Annot::Subtype::FREETEXT;
  if (sSubtype == "Line")
    return CPDF_Annot::Subtype::LINE;
  if (sSubtype == "Square")
    return CPDF_Annot::Subtype::SQUARE;
  if (sSubtype == "Circle")
    return CPDF_Annot::Subtype::CIRCLE;
  if (sSubtype == "Polygon")
    return CPDF_Annot::Subtype::POLYGON;
  if (sSubtype == "PolyLine")
    return CPDF_Annot::Subtype::POLYLINE;
  if (sSubtype == "Highlight")
    return CPDF_Annot::Subtype::HIGHLIGHT;
  if (sSubtype == "Underline")
    return CPDF_Annot::Subtype::UNDERLINE;
  if (sSubtype == "Squiggly")
    return CPDF_Annot::Subtype::SQUIGGLY;
  if (sSubtype == "StrikeOut")
    return CPDF_Annot::Subtype::STRIKEOUT;
  if (sSubtype == "Stamp")
    return CPDF_Annot::Subtype::STAMP;
  if (sSubtype == "Caret")
    return CPDF_Annot::Subtype::CARET;
  if (sSubtype == "Ink")
    return CPDF_Annot::Subtype::INK;
  if (sSubtype == "Popup")
    return CPDF_Annot::Subtype::POPUP;
  if (sSubtype == "FileAttachment")
    return CPDF_Annot::Subtype::FILEATTACHMENT;
  if (sSubtype == "Sound")
    return CPDF_Annot::Subtype::SOUND;
  if (sSubtype == "Movie")
    return CPDF_Annot::Subtype::MOVIE;
  if (sSubtype == "Widget")
    return CPDF_Annot::Subtype::WIDGET;
  if (sSubtype == "Screen")
    return CPDF_Annot::Subtype::SCREEN;
  if (sSubtype == "PrinterMark")
    return CPDF_Annot::Subtype::PRINTERMARK;
  if (sSubtype == "TrapNet")
    return CPDF_Annot::Subtype::TRAPNET;
  if (sSubtype == "Watermark")
    return CPDF_Annot::Subtype::WATERMARK;
  if (sSubtype == "3D")
    return CPDF_Annot::Subtype::THREED;
  if (sSubtype == "RichMedia")
    return CPDF_Annot::Subtype::RICHMEDIA;
  if (sSubtype == "XFAWidget")
    return CPDF_Annot::Subtype::XFAWIDGET;
  return CPDF_Annot::Subtype::UNKNOWN;
}

// static
ByteString CPDF_Annot::AnnotSubtypeToString(CPDF_Annot::Subtype nSubtype) {
  if (nSubtype == CPDF_Annot::Subtype::TEXT)
    return "Text";
  if (nSubtype == CPDF_Annot::Subtype::LINK)
    return "Link";
  if (nSubtype == CPDF_Annot::Subtype::FREETEXT)
    return "FreeText";
  if (nSubtype == CPDF_Annot::Subtype::LINE)
    return "Line";
  if (nSubtype == CPDF_Annot::Subtype::SQUARE)
    return "Square";
  if (nSubtype == CPDF_Annot::Subtype::CIRCLE)
    return "Circle";
  if (nSubtype == CPDF_Annot::Subtype::POLYGON)
    return "Polygon";
  if (nSubtype == CPDF_Annot::Subtype::POLYLINE)
    return "PolyLine";
  if (nSubtype == CPDF_Annot::Subtype::HIGHLIGHT)
    return "Highlight";
  if (nSubtype == CPDF_Annot::Subtype::UNDERLINE)
    return "Underline";
  if (nSubtype == CPDF_Annot::Subtype::SQUIGGLY)
    return "Squiggly";
  if (nSubtype == CPDF_Annot::Subtype::STRIKEOUT)
    return "StrikeOut";
  if (nSubtype == CPDF_Annot::Subtype::STAMP)
    return "Stamp";
  if (nSubtype == CPDF_Annot::Subtype::CARET)
    return "Caret";
  if (nSubtype == CPDF_Annot::Subtype::INK)
    return "Ink";
  if (nSubtype == CPDF_Annot::Subtype::POPUP)
    return "Popup";
  if (nSubtype == CPDF_Annot::Subtype::FILEATTACHMENT)
    return "FileAttachment";
  if (nSubtype == CPDF_Annot::Subtype::SOUND)
    return "Sound";
  if (nSubtype == CPDF_Annot::Subtype::MOVIE)
    return "Movie";
  if (nSubtype == CPDF_Annot::Subtype::WIDGET)
    return "Widget";
  if (nSubtype == CPDF_Annot::Subtype::SCREEN)
    return "Screen";
  if (nSubtype == CPDF_Annot::Subtype::PRINTERMARK)
    return "PrinterMark";
  if (nSubtype == CPDF_Annot::Subtype::TRAPNET)
    return "TrapNet";
  if (nSubtype == CPDF_Annot::Subtype::WATERMARK)
    return "Watermark";
  if (nSubtype == CPDF_Annot::Subtype::THREED)
    return "3D";
  if (nSubtype == CPDF_Annot::Subtype::RICHMEDIA)
    return "RichMedia";
  if (nSubtype == CPDF_Annot::Subtype::XFAWIDGET)
    return "XFAWidget";
  return ByteString();
}

// static
size_t CPDF_Annot::QuadPointCount(const CPDF_Array* pArray) {
  return pArray->size() / 8;
}

bool CPDF_Annot::DrawAppearance(CPDF_Page* pPage,
                                CFX_RenderDevice* pDevice,
                                const CFX_Matrix& mtUser2Device,
                                AppearanceMode mode,
                                const CPDF_RenderOptions* pOptions) {
  if (!ShouldDrawAnnotation())
    return false;

  // It might happen that by the time this annotation instance was created,
  // it was flagged as "hidden" (e.g. /F 2), and hence CPVT_GenerateAP decided
  // to not "generate" its AP.
  // If for a reason the object is no longer hidden, but still does not have
  // its "AP" generated, generate it now.
  GenerateAPIfNeeded();

  CFX_Matrix matrix;
  CPDF_Form* pForm = AnnotGetMatrix(pPage, this, mode, &mtUser2Device, &matrix);
  if (!pForm)
    return false;

  CPDF_RenderContext context(pPage);
  context.AppendLayer(pForm, &matrix);
  context.Render(pDevice, pOptions, nullptr);
  return true;
}

bool CPDF_Annot::DrawInContext(const CPDF_Page* pPage,
                               CPDF_RenderContext* pContext,
                               const CFX_Matrix* pUser2Device,
                               AppearanceMode mode) {
  if (!ShouldDrawAnnotation())
    return false;

  // It might happen that by the time this annotation instance was created,
  // it was flagged as "hidden" (e.g. /F 2), and hence CPVT_GenerateAP decided
  // to not "generate" its AP.
  // If for a reason the object is no longer hidden, but still does not have
  // its "AP" generated, generate it now.
  GenerateAPIfNeeded();

  CFX_Matrix matrix;
  CPDF_Form* pForm = AnnotGetMatrix(pPage, this, mode, pUser2Device, &matrix);
  if (!pForm)
    return false;

  pContext->AppendLayer(pForm, &matrix);
  return true;
}

void CPDF_Annot::DrawBorder(CFX_RenderDevice* pDevice,
                            const CFX_Matrix* pUser2Device,
                            const CPDF_RenderOptions* pOptions) {
  if (GetSubtype() == CPDF_Annot::Subtype::POPUP)
    return;

  uint32_t annot_flags = GetFlags();
  if (annot_flags & ANNOTFLAG_HIDDEN) {
    return;
  }
  bool bPrinting = pDevice->GetDeviceClass() == FXDC_PRINTER ||
                   (pOptions && pOptions->GetOptions().bPrintPreview);
  if (bPrinting && (annot_flags & ANNOTFLAG_PRINT) == 0) {
    return;
  }
  if (!bPrinting && (annot_flags & ANNOTFLAG_NOVIEW)) {
    return;
  }
  CPDF_Dictionary* pBS = m_pAnnotDict->GetDictFor("BS");
  char style_char;
  float width;
  CPDF_Array* pDashArray = nullptr;
  if (!pBS) {
    CPDF_Array* pBorderArray = m_pAnnotDict->GetArrayFor("Border");
    style_char = 'S';
    if (pBorderArray) {
      width = pBorderArray->GetNumberAt(2);
      if (pBorderArray->size() == 4) {
        pDashArray = pBorderArray->GetArrayAt(3);
        if (!pDashArray) {
          return;
        }
        size_t nLen = pDashArray->size();
        size_t i = 0;
        for (; i < nLen; ++i) {
          CPDF_Object* pObj = pDashArray->GetDirectObjectAt(i);
          if (pObj && pObj->GetInteger()) {
            break;
          }
        }
        if (i == nLen) {
          return;
        }
        style_char = 'D';
      }
    } else {
      width = 1;
    }
  } else {
    ByteString style = pBS->GetStringFor("S");
    pDashArray = pBS->GetArrayFor("D");
    style_char = style[1];
    width = pBS->GetNumberFor("W");
  }
  if (width <= 0) {
    return;
  }
  CPDF_Array* pColor = m_pAnnotDict->GetArrayFor("C");
  uint32_t argb = 0xff000000;
  if (pColor) {
    int R = (int32_t)(pColor->GetNumberAt(0) * 255);
    int G = (int32_t)(pColor->GetNumberAt(1) * 255);
    int B = (int32_t)(pColor->GetNumberAt(2) * 255);
    argb = ArgbEncode(0xff, R, G, B);
  }
  CFX_GraphStateData graph_state;
  graph_state.m_LineWidth = width;
  if (style_char == 'D') {
    if (pDashArray) {
      size_t dash_count = pDashArray->size();
      if (dash_count % 2) {
        dash_count++;
      }
      graph_state.m_DashArray.resize(dash_count);
      size_t i;
      for (i = 0; i < pDashArray->size(); ++i) {
        graph_state.m_DashArray[i] = pDashArray->GetNumberAt(i);
      }
      if (i < dash_count) {
        graph_state.m_DashArray[i] = graph_state.m_DashArray[i - 1];
      }
    } else {
      graph_state.m_DashArray = {3.0f, 3.0f};
    }
  }

  CFX_FloatRect rect = GetRect();
  rect.Deflate(width / 2, width / 2);
  CFX_PathData path;
  path.AppendRect(rect);

  int fill_type = 0;
  if (pOptions && pOptions->GetOptions().bNoPathSmooth)
    fill_type |= FXFILL_NOPATHSMOOTH;

  pDevice->DrawPath(&path, pUser2Device, &graph_state, argb, argb, fill_type);
}

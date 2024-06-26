// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_page.h"

#include <set>
#include <utility>

#include "constants/page_object.h"
#include "core/fpdfapi/cpdf_pagerendercontext.h"
#include "core/fpdfapi/page/cpdf_contentparser.h"
#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fpdfapi/render/cpdf_pagerendercache.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

CPDF_Page::CPDF_Page(CPDF_Document* pDocument,
                     CPDF_Dictionary* pPageDict,
                     bool bPageCache)
    : CPDF_PageObjectHolder(pDocument, pPageDict, nullptr, nullptr),
      m_PageSize(100, 100),
      m_pPDFDocument(pDocument) {
  ASSERT(pPageDict);
  if (bPageCache)
    m_pPageRender = pdfium::MakeUnique<CPDF_PageRenderCache>(this);

  // Cannot initialize |m_pResources| and |m_pPageResources| via the
  // CPDF_PageObjectHolder ctor because GetPageAttr() requires
  // CPDF_PageObjectHolder to finish initializing first.
  CPDF_Object* pPageAttr = GetPageAttr(pdfium::page_object::kResources);
  m_pResources = pPageAttr ? pPageAttr->GetDict() : nullptr;
  m_pPageResources = m_pResources;

  UpdateDimensions();
  m_Transparency.SetIsolated();
  LoadTransInfo();
}

CPDF_Page::~CPDF_Page() {}

CPDF_Page* CPDF_Page::AsPDFPage() {
  return this;
}

CPDFXFA_Page* CPDF_Page::AsXFAPage() {
  return nullptr;
}

CPDF_Document* CPDF_Page::GetDocument() const {
  return GetPDFDocument();
}

float CPDF_Page::GetPageWidth() const {
  return m_PageSize.width;
}

float CPDF_Page::GetPageHeight() const {
  return m_PageSize.height;
}

bool CPDF_Page::IsPage() const {
  return true;
}

void CPDF_Page::ParseContent() {
  if (GetParseState() == ParseState::kParsed)
    return;

  if (GetParseState() == ParseState::kNotParsed)
    StartParse(pdfium::MakeUnique<CPDF_ContentParser>(this));

  ASSERT(GetParseState() == ParseState::kParsing);
  ContinueParse(nullptr);
}

void CPDF_Page::SetRenderContext(
    std::unique_ptr<CPDF_PageRenderContext> pContext) {
  m_pRenderContext = std::move(pContext);
}

CPDF_Object* CPDF_Page::GetPageAttr(const ByteString& name) const {
  CPDF_Dictionary* pPageDict = GetDict();
  std::set<CPDF_Dictionary*> visited;
  while (1) {
    visited.insert(pPageDict);
    if (CPDF_Object* pObj = pPageDict->GetDirectObjectFor(name))
      return pObj;

    pPageDict = pPageDict->GetDictFor(pdfium::page_object::kParent);
    if (!pPageDict || pdfium::ContainsKey(visited, pPageDict))
      break;
  }
  return nullptr;
}

CFX_FloatRect CPDF_Page::GetBox(const ByteString& name) const {
  CFX_FloatRect box;
  CPDF_Array* pBox = ToArray(GetPageAttr(name));
  if (pBox) {
    box = pBox->GetRect();
    box.Normalize();
  }
  return box;
}

Optional<CFX_PointF> CPDF_Page::DeviceToPage(
    const FX_RECT& rect,
    int rotate,
    const CFX_PointF& device_point) const {
  CFX_Matrix page2device = GetDisplayMatrix(rect, rotate);
  return page2device.GetInverse().Transform(device_point);
}

Optional<CFX_PointF> CPDF_Page::PageToDevice(
    const FX_RECT& rect,
    int rotate,
    const CFX_PointF& page_point) const {
  CFX_Matrix page2device = GetDisplayMatrix(rect, rotate);
  return page2device.Transform(page_point);
}

CFX_Matrix CPDF_Page::GetDisplayMatrix(const FX_RECT& rect, int iRotate) const {
  if (m_PageSize.width == 0 || m_PageSize.height == 0)
    return CFX_Matrix();

  float x0 = 0;
  float y0 = 0;
  float x1 = 0;
  float y1 = 0;
  float x2 = 0;
  float y2 = 0;
  iRotate %= 4;
  // This code implicitly inverts the y-axis to account for page coordinates
  // pointing up and bitmap coordinates pointing down. (x0, y0) is the base
  // point, (x1, y1) is that point translated on y and (x2, y2) is the point
  // translated on x. On iRotate = 0, y0 is rect.bottom and the translation
  // to get y1 is performed as negative. This results in the desired
  // transformation.
  switch (iRotate) {
    case 0:
      x0 = rect.left;
      y0 = rect.bottom;
      x1 = rect.left;
      y1 = rect.top;
      x2 = rect.right;
      y2 = rect.bottom;
      break;
    case 1:
      x0 = rect.left;
      y0 = rect.top;
      x1 = rect.right;
      y1 = rect.top;
      x2 = rect.left;
      y2 = rect.bottom;
      break;
    case 2:
      x0 = rect.right;
      y0 = rect.top;
      x1 = rect.right;
      y1 = rect.bottom;
      x2 = rect.left;
      y2 = rect.top;
      break;
    case 3:
      x0 = rect.right;
      y0 = rect.bottom;
      x1 = rect.left;
      y1 = rect.bottom;
      x2 = rect.right;
      y2 = rect.top;
      break;
  }
  CFX_Matrix matrix((x2 - x0) / m_PageSize.width, (y2 - y0) / m_PageSize.width,
                    (x1 - x0) / m_PageSize.height,
                    (y1 - y0) / m_PageSize.height, x0, y0);
  return m_PageMatrix * matrix;
}

int CPDF_Page::GetPageRotation() const {
  CPDF_Object* pRotate = GetPageAttr(pdfium::page_object::kRotate);
  int rotate = pRotate ? (pRotate->GetInteger() / 90) % 4 : 0;
  return (rotate < 0) ? (rotate + 4) : rotate;
}

void CPDF_Page::UpdateDimensions() {
  CFX_FloatRect mediabox = GetBox(pdfium::page_object::kMediaBox);
  if (mediabox.IsEmpty())
    mediabox = CFX_FloatRect(0, 0, 612, 792);

  m_BBox = GetBox(pdfium::page_object::kCropBox);
  if (m_BBox.IsEmpty())
    m_BBox = mediabox;
  else
    m_BBox.Intersect(mediabox);

  m_PageSize.width = m_BBox.Width();
  m_PageSize.height = m_BBox.Height();

  switch (GetPageRotation()) {
    case 0:
      m_PageMatrix = CFX_Matrix(1.0f, 0, 0, 1.0f, -m_BBox.left, -m_BBox.bottom);
      break;
    case 1:
      std::swap(m_PageSize.width, m_PageSize.height);
      m_PageMatrix =
          CFX_Matrix(0, -1.0f, 1.0f, 0, -m_BBox.bottom, m_BBox.right);
      break;
    case 2:
      m_PageMatrix = CFX_Matrix(-1.0f, 0, 0, -1.0f, m_BBox.right, m_BBox.top);
      break;
    case 3:
      std::swap(m_PageSize.width, m_PageSize.height);
      m_PageMatrix = CFX_Matrix(0, 1.0f, -1.0f, 0, m_BBox.top, -m_BBox.left);
      break;
  }
}

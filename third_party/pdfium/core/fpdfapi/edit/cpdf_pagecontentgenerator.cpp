// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/edit/cpdf_pagecontentgenerator.h"

#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <utility>

#include "core/fpdfapi/edit/cpdf_pagecontentmanager.h"
#include "core/fpdfapi/edit/cpdf_stringarchivestream.h"
#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fpdfapi/page/cpdf_contentmarks.h"
#include "core/fpdfapi/page/cpdf_docpagedata.h"
#include "core/fpdfapi/page/cpdf_image.h"
#include "core/fpdfapi/page/cpdf_imageobject.h"
#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/page/cpdf_path.h"
#include "core/fpdfapi/page/cpdf_pathobject.h"
#include "core/fpdfapi/page/cpdf_textobject.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/fpdf_parser_decode.h"
#include "core/fpdfapi/parser/fpdf_parser_utility.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "third_party/skia_shared/SkFloatToDecimal.h"

namespace {

std::ostream& operator<<(std::ostream& ar, const CFX_Matrix& matrix) {
  ar << matrix.a << " " << matrix.b << " " << matrix.c << " " << matrix.d << " "
     << matrix.e << " " << matrix.f;
  return ar;
}

bool GetColor(const CPDF_Color* pColor, float* rgb) {
  int intRGB[3];
  if (!pColor || !pColor->IsColorSpaceRGB() ||
      !pColor->GetRGB(&intRGB[0], &intRGB[1], &intRGB[2])) {
    return false;
  }
  rgb[0] = intRGB[0] / 255.0f;
  rgb[1] = intRGB[1] / 255.0f;
  rgb[2] = intRGB[2] / 255.0f;
  return true;
}

}  // namespace

CPDF_PageContentGenerator::CPDF_PageContentGenerator(
    CPDF_PageObjectHolder* pObjHolder)
    : m_pObjHolder(pObjHolder), m_pDocument(pObjHolder->GetDocument()) {
  for (const auto& pObj : *pObjHolder->GetPageObjectList()) {
    if (pObj)
      m_pageObjects.emplace_back(pObj.get());
  }
}

CPDF_PageContentGenerator::~CPDF_PageContentGenerator() {}

void CPDF_PageContentGenerator::GenerateContent() {
  ASSERT(m_pObjHolder->IsPage());

  std::map<int32_t, std::unique_ptr<std::ostringstream>> stream =
      GenerateModifiedStreams();

  UpdateContentStreams(&stream);
}

std::map<int32_t, std::unique_ptr<std::ostringstream>>
CPDF_PageContentGenerator::GenerateModifiedStreams() {
  // Make sure default graphics are created.
  GetOrCreateDefaultGraphics();

  // Figure out which streams are dirty.
  std::set<int32_t> all_dirty_streams;
  for (auto& pPageObj : m_pageObjects) {
    if (pPageObj->IsDirty())
      all_dirty_streams.insert(pPageObj->GetContentStream());
  }
  const std::set<int32_t>& marked_dirty_streams =
      m_pObjHolder->GetDirtyStreams();
  all_dirty_streams.insert(marked_dirty_streams.begin(),
                           marked_dirty_streams.end());

  // Start regenerating dirty streams.
  std::map<int32_t, std::unique_ptr<std::ostringstream>> streams;
  std::set<int32_t> empty_streams;
  std::unique_ptr<const CPDF_ContentMarks> empty_content_marks =
      pdfium::MakeUnique<CPDF_ContentMarks>();
  std::map<int32_t, const CPDF_ContentMarks*> current_content_marks;

  for (int32_t dirty_stream : all_dirty_streams) {
    std::unique_ptr<std::ostringstream> buf =
        pdfium::MakeUnique<std::ostringstream>();

    // Set the default graphic state values
    *buf << "q\n";
    if (!m_pObjHolder->GetLastCTM().IsIdentity())
      *buf << m_pObjHolder->GetLastCTM().GetInverse() << " cm\n";

    ProcessDefaultGraphics(buf.get());

    streams[dirty_stream] = std::move(buf);
    empty_streams.insert(dirty_stream);
    current_content_marks[dirty_stream] = empty_content_marks.get();
  }

  // Process the page objects, write into each dirty stream.
  for (auto& pPageObj : m_pageObjects) {
    int stream_index = pPageObj->GetContentStream();
    auto it = streams.find(stream_index);
    if (it == streams.end())
      continue;

    std::ostringstream* buf = it->second.get();
    empty_streams.erase(stream_index);
    current_content_marks[stream_index] = ProcessContentMarks(
        buf, pPageObj.Get(), current_content_marks[stream_index]);
    ProcessPageObject(buf, pPageObj.Get());
  }

  // Finish dirty streams.
  for (int32_t dirty_stream : all_dirty_streams) {
    std::ostringstream* buf = streams[dirty_stream].get();
    if (pdfium::ContainsKey(empty_streams, dirty_stream)) {
      // Clear to show that this stream needs to be deleted.
      buf->str("");
    } else {
      FinishMarks(buf, current_content_marks[dirty_stream]);

      // Return graphics to original state
      *buf << "Q\n";
    }
  }

  // Clear dirty streams in m_pObjHolder
  m_pObjHolder->ClearDirtyStreams();

  return streams;
}

void CPDF_PageContentGenerator::UpdateContentStreams(
    std::map<int32_t, std::unique_ptr<std::ostringstream>>* new_stream_data) {
  // If no streams were regenerated or removed, nothing to do here.
  if (new_stream_data->empty())
    return;

  CPDF_PageContentManager page_content_manager(m_pObjHolder.Get());

  for (auto& pair : *new_stream_data) {
    int32_t stream_index = pair.first;
    std::ostringstream* buf = pair.second.get();

    if (stream_index == CPDF_PageObject::kNoContentStream) {
      int new_stream_index = page_content_manager.AddStream(buf);
      UpdateStreamlessPageObjects(new_stream_index);
      continue;
    }

    CPDF_Stream* old_stream =
        page_content_manager.GetStreamByIndex(stream_index);
    ASSERT(old_stream);

    // If buf is now empty, remove the stream instead of setting the data.
    if (buf->tellp() <= 0)
      page_content_manager.ScheduleRemoveStreamByIndex(stream_index);
    else
      old_stream->SetDataFromStringstream(buf);
  }

  page_content_manager.ExecuteScheduledRemovals();
}

ByteString CPDF_PageContentGenerator::RealizeResource(
    const CPDF_Object* pResource,
    const ByteString& bsType) const {
  ASSERT(pResource);
  if (!m_pObjHolder->m_pResources) {
    m_pObjHolder->m_pResources = m_pDocument->NewIndirect<CPDF_Dictionary>();
    m_pObjHolder->GetDict()->SetFor(
        "Resources",
        m_pObjHolder->m_pResources->MakeReference(m_pDocument.Get()));
  }
  CPDF_Dictionary* pResList = m_pObjHolder->m_pResources->GetDictFor(bsType);
  if (!pResList)
    pResList = m_pObjHolder->m_pResources->SetNewFor<CPDF_Dictionary>(bsType);

  ByteString name;
  int idnum = 1;
  while (1) {
    name = ByteString::Format("FX%c%d", bsType[0], idnum);
    if (!pResList->KeyExist(name))
      break;

    idnum++;
  }
  pResList->SetFor(name, pResource->MakeReference(m_pDocument.Get()));
  return name;
}

bool CPDF_PageContentGenerator::ProcessPageObjects(std::ostringstream* buf) {
  bool bDirty = false;
  std::unique_ptr<const CPDF_ContentMarks> empty_content_marks =
      pdfium::MakeUnique<CPDF_ContentMarks>();
  const CPDF_ContentMarks* content_marks = empty_content_marks.get();

  for (auto& pPageObj : m_pageObjects) {
    if (m_pObjHolder->IsPage() && !pPageObj->IsDirty())
      continue;

    bDirty = true;
    content_marks = ProcessContentMarks(buf, pPageObj.Get(), content_marks);
    ProcessPageObject(buf, pPageObj.Get());
  }
  FinishMarks(buf, content_marks);
  return bDirty;
}

void CPDF_PageContentGenerator::UpdateStreamlessPageObjects(
    int new_content_stream_index) {
  for (auto& pPageObj : m_pageObjects) {
    if (pPageObj->GetContentStream() == CPDF_PageObject::kNoContentStream)
      pPageObj->SetContentStream(new_content_stream_index);
  }
}

const CPDF_ContentMarks* CPDF_PageContentGenerator::ProcessContentMarks(
    std::ostringstream* buf,
    const CPDF_PageObject* pPageObj,
    const CPDF_ContentMarks* pPrev) {
  const CPDF_ContentMarks* pNext = &pPageObj->m_ContentMarks;

  size_t first_different = pPrev->FindFirstDifference(pNext);

  // Close all marks that are in prev but not in next.
  // Technically we should iterate backwards to close from the top to the
  // bottom, but since the EMC operators do not identify which mark they are
  // closing, it does not matter.
  for (size_t i = first_different; i < pPrev->CountItems(); ++i)
    *buf << "EMC\n";

  // Open all marks that are in next but not in prev.
  for (size_t i = first_different; i < pNext->CountItems(); ++i) {
    const CPDF_ContentMarkItem* item = pNext->GetItem(i);

    // Write mark tag.
    *buf << "/" << PDF_NameEncode(item->GetName()) << " ";

    // If there are no parameters, write a BMC (begin marked content) operator.
    if (item->GetParamType() == CPDF_ContentMarkItem::kNone) {
      *buf << "BMC\n";
      continue;
    }

    // If there are parameters, write properties, direct or indirect.
    switch (item->GetParamType()) {
      case CPDF_ContentMarkItem::kDirectDict: {
        CPDF_StringArchiveStream archive_stream(buf);
        item->GetParam()->WriteTo(&archive_stream, nullptr);
        *buf << " ";
        break;
      }
      case CPDF_ContentMarkItem::kPropertiesDict: {
        *buf << "/" << item->GetPropertyName() << " ";
        break;
      }
      default:
        NOTREACHED();
        break;
    }

    // Write BDC (begin dictionary content) operator.
    *buf << "BDC\n";
  }

  return pNext;
}

void CPDF_PageContentGenerator::FinishMarks(
    std::ostringstream* buf,
    const CPDF_ContentMarks* pContentMarks) {
  // Technically we should iterate backwards to close from the top to the
  // bottom, but since the EMC operators do not identify which mark they are
  // closing, it does not matter.
  for (size_t i = 0; i < pContentMarks->CountItems(); ++i)
    *buf << "EMC\n";
}

void CPDF_PageContentGenerator::ProcessPageObject(std::ostringstream* buf,
                                                  CPDF_PageObject* pPageObj) {
  if (CPDF_ImageObject* pImageObject = pPageObj->AsImage())
    ProcessImage(buf, pImageObject);
  else if (CPDF_PathObject* pPathObj = pPageObj->AsPath())
    ProcessPath(buf, pPathObj);
  else if (CPDF_TextObject* pTextObj = pPageObj->AsText())
    ProcessText(buf, pTextObj);
  pPageObj->SetDirty(false);
}

void CPDF_PageContentGenerator::ProcessImage(std::ostringstream* buf,
                                             CPDF_ImageObject* pImageObj) {
  if ((pImageObj->matrix().a == 0 && pImageObj->matrix().b == 0) ||
      (pImageObj->matrix().c == 0 && pImageObj->matrix().d == 0)) {
    return;
  }
  *buf << "q " << pImageObj->matrix() << " cm ";

  RetainPtr<CPDF_Image> pImage = pImageObj->GetImage();
  if (pImage->IsInline())
    return;

  CPDF_Stream* pStream = pImage->GetStream();
  if (!pStream)
    return;

  bool bWasInline = pStream->IsInline();
  if (bWasInline)
    pImage->ConvertStreamToIndirectObject();

  ByteString name = RealizeResource(pStream, "XObject");
  if (bWasInline)
    pImageObj->SetImage(
        m_pDocument->GetPageData()->GetImage(pStream->GetObjNum()));

  *buf << "/" << PDF_NameEncode(name) << " Do Q\n";
}

// Processing path with operators from Tables 4.9 and 4.10 of PDF spec 1.7:
// "re" appends a rectangle (here, used only if the whole path is a rectangle)
// "m" moves current point to the given coordinates
// "l" creates a line from current point to the new point
// "c" adds a Bezier curve from current to last point, using the two other
// points as the Bezier control points
// Note: "l", "c" change the current point
// "h" closes the subpath (appends a line from current to starting point)
// Path painting operators: "S", "n", "B", "f", "B*", "f*", depending on
// the filling mode and whether we want stroking the path or not.
// "Q" restores the graphics state imposed by the ProcessGraphics method.
void CPDF_PageContentGenerator::ProcessPath(std::ostringstream* buf,
                                            CPDF_PathObject* pPathObj) {
  ProcessGraphics(buf, pPathObj);

  *buf << pPathObj->matrix() << " cm ";

  const auto& pPoints = pPathObj->path().GetPoints();
  if (pPathObj->path().IsRect()) {
    CFX_PointF diff = pPoints[2].m_Point - pPoints[0].m_Point;
    *buf << pPoints[0].m_Point.x << " " << pPoints[0].m_Point.y << " " << diff.x
         << " " << diff.y << " re";
  } else {
    for (size_t i = 0; i < pPoints.size(); i++) {
      if (i > 0)
        *buf << " ";

      char buffer[pdfium::skia::kMaximumSkFloatToDecimalLength];
      unsigned size =
          pdfium::skia::SkFloatToDecimal(pPoints[i].m_Point.x, buffer);
      buf->write(buffer, size) << " ";
      size = pdfium::skia::SkFloatToDecimal(pPoints[i].m_Point.y, buffer);
      buf->write(buffer, size);

      FXPT_TYPE pointType = pPoints[i].m_Type;
      if (pointType == FXPT_TYPE::MoveTo) {
        *buf << " m";
      } else if (pointType == FXPT_TYPE::LineTo) {
        *buf << " l";
      } else if (pointType == FXPT_TYPE::BezierTo) {
        if (i + 2 >= pPoints.size() ||
            !pPoints[i].IsTypeAndOpen(FXPT_TYPE::BezierTo) ||
            !pPoints[i + 1].IsTypeAndOpen(FXPT_TYPE::BezierTo) ||
            pPoints[i + 2].m_Type != FXPT_TYPE::BezierTo) {
          // If format is not supported, close the path and paint
          *buf << " h";
          break;
        }
        *buf << " " << pPoints[i + 1].m_Point.x << " "
             << pPoints[i + 1].m_Point.y << " " << pPoints[i + 2].m_Point.x
             << " " << pPoints[i + 2].m_Point.y << " c";
        i += 2;
      }
      if (pPoints[i].m_CloseFigure)
        *buf << " h";
    }
  }
  if (pPathObj->filltype() == 0)
    *buf << (pPathObj->stroke() ? " S" : " n");
  else if (pPathObj->filltype() == FXFILL_WINDING)
    *buf << (pPathObj->stroke() ? " B" : " f");
  else if (pPathObj->filltype() == FXFILL_ALTERNATE)
    *buf << (pPathObj->stroke() ? " B*" : " f*");
  *buf << " Q\n";
}

// This method supports color operators rg and RGB from Table 4.24 of PDF spec
// 1.7. A color will not be set if the colorspace is not DefaultRGB or the RGB
// values cannot be obtained. The method also adds an external graphics
// dictionary, as described in Section 4.3.4.
// "rg" sets the fill color, "RG" sets the stroke color (using DefaultRGB)
// "w" sets the stroke line width.
// "ca" sets the fill alpha, "CA" sets the stroke alpha.
// "q" saves the graphics state, so that the settings can later be reversed
void CPDF_PageContentGenerator::ProcessGraphics(std::ostringstream* buf,
                                                CPDF_PageObject* pPageObj) {
  *buf << "q ";
  float fillColor[3];
  if (GetColor(pPageObj->m_ColorState.GetFillColor(), fillColor)) {
    *buf << fillColor[0] << " " << fillColor[1] << " " << fillColor[2]
         << " rg ";
  }
  float strokeColor[3];
  if (GetColor(pPageObj->m_ColorState.GetStrokeColor(), strokeColor)) {
    *buf << strokeColor[0] << " " << strokeColor[1] << " " << strokeColor[2]
         << " RG ";
  }
  float lineWidth = pPageObj->m_GraphState.GetLineWidth();
  if (lineWidth != 1.0f)
    *buf << lineWidth << " w ";
  CFX_GraphStateData::LineCap lineCap = pPageObj->m_GraphState.GetLineCap();
  if (lineCap != CFX_GraphStateData::LineCapButt)
    *buf << static_cast<int>(lineCap) << " J ";
  CFX_GraphStateData::LineJoin lineJoin = pPageObj->m_GraphState.GetLineJoin();
  if (lineJoin != CFX_GraphStateData::LineJoinMiter)
    *buf << static_cast<int>(lineJoin) << " j ";

  GraphicsData graphD;
  graphD.fillAlpha = pPageObj->m_GeneralState.GetFillAlpha();
  graphD.strokeAlpha = pPageObj->m_GeneralState.GetStrokeAlpha();
  graphD.blendType = pPageObj->m_GeneralState.GetBlendType();
  if (graphD.fillAlpha == 1.0f && graphD.strokeAlpha == 1.0f &&
      graphD.blendType == BlendMode::kNormal) {
    return;
  }

  ByteString name;
  auto it = m_pObjHolder->m_GraphicsMap.find(graphD);
  if (it != m_pObjHolder->m_GraphicsMap.end()) {
    name = it->second;
  } else {
    auto gsDict = pdfium::MakeUnique<CPDF_Dictionary>();
    if (graphD.fillAlpha != 1.0f)
      gsDict->SetNewFor<CPDF_Number>("ca", graphD.fillAlpha);

    if (graphD.strokeAlpha != 1.0f)
      gsDict->SetNewFor<CPDF_Number>("CA", graphD.strokeAlpha);

    if (graphD.blendType != BlendMode::kNormal) {
      gsDict->SetNewFor<CPDF_Name>("BM",
                                   pPageObj->m_GeneralState.GetBlendMode());
    }
    CPDF_Object* pDict = m_pDocument->AddIndirectObject(std::move(gsDict));
    name = RealizeResource(pDict, "ExtGState");
    m_pObjHolder->m_GraphicsMap[graphD] = name;
  }
  *buf << "/" << PDF_NameEncode(name) << " gs ";
}

void CPDF_PageContentGenerator::ProcessDefaultGraphics(
    std::ostringstream* buf) {
  *buf << "0 0 0 RG 0 0 0 rg 1 w "
       << static_cast<int>(CFX_GraphStateData::LineCapButt) << " J "
       << static_cast<int>(CFX_GraphStateData::LineJoinMiter) << " j\n";
  ByteString name = GetOrCreateDefaultGraphics();
  *buf << "/" << PDF_NameEncode(name) << " gs ";
}

ByteString CPDF_PageContentGenerator::GetOrCreateDefaultGraphics() const {
  GraphicsData defaultGraphics;
  defaultGraphics.fillAlpha = 1.0f;
  defaultGraphics.strokeAlpha = 1.0f;
  defaultGraphics.blendType = BlendMode::kNormal;
  auto it = m_pObjHolder->m_GraphicsMap.find(defaultGraphics);

  // If default graphics already exists, return it.
  if (it != m_pObjHolder->m_GraphicsMap.end())
    return it->second;

  // Otherwise, create them.
  auto gsDict = pdfium::MakeUnique<CPDF_Dictionary>();
  gsDict->SetNewFor<CPDF_Number>("ca", defaultGraphics.fillAlpha);
  gsDict->SetNewFor<CPDF_Number>("CA", defaultGraphics.strokeAlpha);
  gsDict->SetNewFor<CPDF_Name>("BM", "Normal");
  CPDF_Object* pDict = m_pDocument->AddIndirectObject(std::move(gsDict));
  ByteString name = RealizeResource(pDict, "ExtGState");
  m_pObjHolder->m_GraphicsMap[defaultGraphics] = name;
  return name;
}

// This method adds text to the buffer, BT begins the text object, ET ends it.
// Tm sets the text matrix (allows positioning and transforming text).
// Tf sets the font name (from Font in Resources) and font size.
// Tj sets the actual text, <####...> is used when specifying charcodes.
void CPDF_PageContentGenerator::ProcessText(std::ostringstream* buf,
                                            CPDF_TextObject* pTextObj) {
  ProcessGraphics(buf, pTextObj);
  *buf << "BT " << pTextObj->GetTextMatrix() << " Tm ";
  CPDF_Font* pFont = pTextObj->GetFont();
  if (!pFont)
    pFont = CPDF_Font::GetStockFont(m_pDocument.Get(), "Helvetica");
  FontData fontD;
  if (pFont->IsType1Font())
    fontD.type = "Type1";
  else if (pFont->IsTrueTypeFont())
    fontD.type = "TrueType";
  else if (pFont->IsCIDFont())
    fontD.type = "Type0";
  else
    return;
  fontD.baseFont = pFont->GetBaseFont();
  auto it = m_pObjHolder->m_FontsMap.find(fontD);
  ByteString dictName;
  if (it != m_pObjHolder->m_FontsMap.end()) {
    dictName = it->second;
  } else {
    CPDF_Object* pIndirectFont = pFont->GetFontDict();
    if (pIndirectFont->IsInline()) {
      // In this case we assume it must be a standard font
      auto fontDict = pdfium::MakeUnique<CPDF_Dictionary>();
      fontDict->SetNewFor<CPDF_Name>("Type", "Font");
      fontDict->SetNewFor<CPDF_Name>("Subtype", fontD.type);
      fontDict->SetNewFor<CPDF_Name>("BaseFont", fontD.baseFont);
      pIndirectFont = m_pDocument->AddIndirectObject(std::move(fontDict));
    }
    dictName = RealizeResource(pIndirectFont, "Font");
    m_pObjHolder->m_FontsMap[fontD] = dictName;
  }
  *buf << "/" << PDF_NameEncode(dictName) << " " << pTextObj->GetFontSize()
       << " Tf ";
  ByteString text;
  for (uint32_t charcode : pTextObj->GetCharCodes()) {
    if (charcode != CPDF_Font::kInvalidCharCode)
      pFont->AppendChar(&text, charcode);
  }
  *buf << PDF_EncodeString(text, true) << " Tj ET";
  *buf << " Q\n";
}

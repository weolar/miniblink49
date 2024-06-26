// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpvt_generateap.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <utility>

#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_boolean.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfapi/parser/fpdf_parser_decode.h"
#include "core/fpdfapi/parser/fpdf_parser_utility.h"
#include "core/fpdfdoc/cpdf_annot.h"
#include "core/fpdfdoc/cpdf_defaultappearance.h"
#include "core/fpdfdoc/cpdf_formfield.h"
#include "core/fpdfdoc/cpvt_fontmap.h"
#include "core/fpdfdoc/cpvt_word.h"
#include "third_party/base/ptr_util.h"

struct CPVT_Dash {
  CPVT_Dash(int32_t dash, int32_t gap, int32_t phase)
      : nDash(dash), nGap(gap), nPhase(phase) {}

  int32_t nDash;
  int32_t nGap;
  int32_t nPhase;
};

namespace {

enum class PaintOperation { STROKE, FILL };

ByteString GetPDFWordString(IPVT_FontMap* pFontMap,
                            int32_t nFontIndex,
                            uint16_t Word,
                            uint16_t SubWord) {
  if (SubWord > 0)
    return ByteString::Format("%c", SubWord);

  if (!pFontMap)
    return ByteString();

  CPDF_Font* pPDFFont = pFontMap->GetPDFFont(nFontIndex);
  if (!pPDFFont)
    return ByteString();

  if (pPDFFont->GetBaseFont().Compare("Symbol") == 0 ||
      pPDFFont->GetBaseFont().Compare("ZapfDingbats") == 0) {
    return ByteString::Format("%c", Word);
  }

  ByteString sWord;
  uint32_t dwCharCode = pPDFFont->CharCodeFromUnicode(Word);
  if (dwCharCode != CPDF_Font::kInvalidCharCode)
    pPDFFont->AppendChar(&sWord, dwCharCode);
  return sWord;
}

ByteString GetWordRenderString(const ByteString& strWords) {
  if (strWords.GetLength() > 0)
    return PDF_EncodeString(strWords, false) + " Tj\n";
  return ByteString();
}

ByteString GetFontSetString(IPVT_FontMap* pFontMap,
                            int32_t nFontIndex,
                            float fFontSize) {
  std::ostringstream sRet;
  if (pFontMap) {
    ByteString sFontAlias = pFontMap->GetPDFFontAlias(nFontIndex);
    if (sFontAlias.GetLength() > 0 && fFontSize > 0)
      sRet << "/" << sFontAlias << " " << fFontSize << " Tf\n";
  }
  return ByteString(sRet);
}

ByteString GenerateEditAP(IPVT_FontMap* pFontMap,
                          CPDF_VariableText::Iterator* pIterator,
                          const CFX_PointF& ptOffset,
                          bool bContinuous,
                          uint16_t SubWord) {
  std::ostringstream sEditStream;
  std::ostringstream sLineStream;
  std::ostringstream sWords;
  CFX_PointF ptOld;
  CFX_PointF ptNew;
  int32_t nCurFontIndex = -1;
  CPVT_WordPlace oldplace;

  pIterator->SetAt(0);
  while (pIterator->NextWord()) {
    CPVT_WordPlace place = pIterator->GetWordPlace();
    if (bContinuous) {
      if (place.LineCmp(oldplace) != 0) {
        if (sWords.tellp() > 0) {
          sLineStream << GetWordRenderString(ByteString(sWords));
          sEditStream << sLineStream.str();
          sLineStream.str("");
          sWords.str("");
        }
        CPVT_Word word;
        if (pIterator->GetWord(word)) {
          ptNew = CFX_PointF(word.ptWord.x + ptOffset.x,
                             word.ptWord.y + ptOffset.y);
        } else {
          CPVT_Line line;
          pIterator->GetLine(line);
          ptNew = CFX_PointF(line.ptLine.x + ptOffset.x,
                             line.ptLine.y + ptOffset.y);
        }
        if (ptNew != ptOld) {
          sLineStream << ptNew.x - ptOld.x << " " << ptNew.y - ptOld.y
                      << " Td\n";
          ptOld = ptNew;
        }
      }
      CPVT_Word word;
      if (pIterator->GetWord(word)) {
        if (word.nFontIndex != nCurFontIndex) {
          if (sWords.tellp() > 0) {
            sLineStream << GetWordRenderString(ByteString(sWords));
            sWords.str("");
          }
          sLineStream << GetFontSetString(pFontMap, word.nFontIndex,
                                          word.fFontSize);
          nCurFontIndex = word.nFontIndex;
        }
        sWords << GetPDFWordString(pFontMap, nCurFontIndex, word.Word, SubWord);
      }
      oldplace = place;
    } else {
      CPVT_Word word;
      if (pIterator->GetWord(word)) {
        ptNew =
            CFX_PointF(word.ptWord.x + ptOffset.x, word.ptWord.y + ptOffset.y);
        if (ptNew != ptOld) {
          sEditStream << ptNew.x - ptOld.x << " " << ptNew.y - ptOld.y
                      << " Td\n";
          ptOld = ptNew;
        }
        if (word.nFontIndex != nCurFontIndex) {
          sEditStream << GetFontSetString(pFontMap, word.nFontIndex,
                                          word.fFontSize);
          nCurFontIndex = word.nFontIndex;
        }
        sEditStream << GetWordRenderString(
            GetPDFWordString(pFontMap, nCurFontIndex, word.Word, SubWord));
      }
    }
  }
  if (sWords.tellp() > 0) {
    sLineStream << GetWordRenderString(ByteString(sWords));
    sEditStream << sLineStream.str();
    sWords.str("");
  }
  return ByteString(sEditStream);
}

ByteString GenerateColorAP(const CFX_Color& color, PaintOperation nOperation) {
  std::ostringstream sColorStream;
  switch (color.nColorType) {
    case CFX_Color::kRGB:
      sColorStream << color.fColor1 << " " << color.fColor2 << " "
                   << color.fColor3 << " "
                   << (nOperation == PaintOperation::STROKE ? "RG" : "rg")
                   << "\n";
      break;
    case CFX_Color::kGray:
      sColorStream << color.fColor1 << " "
                   << (nOperation == PaintOperation::STROKE ? "G" : "g")
                   << "\n";
      break;
    case CFX_Color::kCMYK:
      sColorStream << color.fColor1 << " " << color.fColor2 << " "
                   << color.fColor3 << " " << color.fColor4 << " "
                   << (nOperation == PaintOperation::STROKE ? "K" : "k")
                   << "\n";
      break;
    case CFX_Color::kTransparent:
      break;
  }
  return ByteString(sColorStream);
}

ByteString GenerateBorderAP(const CFX_FloatRect& rect,
                            float fWidth,
                            const CFX_Color& color,
                            const CFX_Color& crLeftTop,
                            const CFX_Color& crRightBottom,
                            BorderStyle nStyle,
                            const CPVT_Dash& dash) {
  std::ostringstream sAppStream;
  ByteString sColor;
  float fLeft = rect.left;
  float fRight = rect.right;
  float fTop = rect.top;
  float fBottom = rect.bottom;
  if (fWidth > 0.0f) {
    float fHalfWidth = fWidth / 2.0f;
    switch (nStyle) {
      default:
      case BorderStyle::SOLID:
        sColor = GenerateColorAP(color, PaintOperation::FILL);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fLeft << " " << fBottom << " " << fRight - fLeft << " "
                     << fTop - fBottom << " re\n";
          sAppStream << fLeft + fWidth << " " << fBottom + fWidth << " "
                     << fRight - fLeft - fWidth * 2 << " "
                     << fTop - fBottom - fWidth * 2 << " re\n";
          sAppStream << "f*\n";
        }
        break;
      case BorderStyle::DASH:
        sColor = GenerateColorAP(color, PaintOperation::STROKE);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fWidth << " w"
                     << " [" << dash.nDash << " " << dash.nGap << "] "
                     << dash.nPhase << " d\n";
          sAppStream << fLeft + fWidth / 2 << " " << fBottom + fWidth / 2
                     << " m\n";
          sAppStream << fLeft + fWidth / 2 << " " << fTop - fWidth / 2
                     << " l\n";
          sAppStream << fRight - fWidth / 2 << " " << fTop - fWidth / 2
                     << " l\n";
          sAppStream << fRight - fWidth / 2 << " " << fBottom + fWidth / 2
                     << " l\n";
          sAppStream << fLeft + fWidth / 2 << " " << fBottom + fWidth / 2
                     << " l S\n";
        }
        break;
      case BorderStyle::BEVELED:
      case BorderStyle::INSET:
        sColor = GenerateColorAP(crLeftTop, PaintOperation::FILL);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fLeft + fHalfWidth << " " << fBottom + fHalfWidth
                     << " m\n";
          sAppStream << fLeft + fHalfWidth << " " << fTop - fHalfWidth
                     << " l\n";
          sAppStream << fRight - fHalfWidth << " " << fTop - fHalfWidth
                     << " l\n";
          sAppStream << fRight - fHalfWidth * 2 << " " << fTop - fHalfWidth * 2
                     << " l\n";
          sAppStream << fLeft + fHalfWidth * 2 << " " << fTop - fHalfWidth * 2
                     << " l\n";
          sAppStream << fLeft + fHalfWidth * 2 << " "
                     << fBottom + fHalfWidth * 2 << " l f\n";
        }
        sColor = GenerateColorAP(crRightBottom, PaintOperation::FILL);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fRight - fHalfWidth << " " << fTop - fHalfWidth
                     << " m\n";
          sAppStream << fRight - fHalfWidth << " " << fBottom + fHalfWidth
                     << " l\n";
          sAppStream << fLeft + fHalfWidth << " " << fBottom + fHalfWidth
                     << " l\n";
          sAppStream << fLeft + fHalfWidth * 2 << " "
                     << fBottom + fHalfWidth * 2 << " l\n";
          sAppStream << fRight - fHalfWidth * 2 << " "
                     << fBottom + fHalfWidth * 2 << " l\n";
          sAppStream << fRight - fHalfWidth * 2 << " " << fTop - fHalfWidth * 2
                     << " l f\n";
        }
        sColor = GenerateColorAP(color, PaintOperation::FILL);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fLeft << " " << fBottom << " " << fRight - fLeft << " "
                     << fTop - fBottom << " re\n";
          sAppStream << fLeft + fHalfWidth << " " << fBottom + fHalfWidth << " "
                     << fRight - fLeft - fHalfWidth * 2 << " "
                     << fTop - fBottom - fHalfWidth * 2 << " re f*\n";
        }
        break;
      case BorderStyle::UNDERLINE:
        sColor = GenerateColorAP(color, PaintOperation::STROKE);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fWidth << " w\n";
          sAppStream << fLeft << " " << fBottom + fWidth / 2 << " m\n";
          sAppStream << fRight << " " << fBottom + fWidth / 2 << " l S\n";
        }
        break;
    }
  }
  return ByteString(sAppStream);
}

ByteString GetColorStringWithDefault(CPDF_Array* pColor,
                                     const CFX_Color& crDefaultColor,
                                     PaintOperation nOperation) {
  if (pColor) {
    CFX_Color color = CFX_Color::ParseColor(*pColor);
    return GenerateColorAP(color, nOperation);
  }

  return GenerateColorAP(crDefaultColor, nOperation);
}

float GetBorderWidth(const CPDF_Dictionary& pAnnotDict) {
  if (const CPDF_Dictionary* pBorderStyleDict = pAnnotDict.GetDictFor("BS")) {
    if (pBorderStyleDict->KeyExist("W"))
      return pBorderStyleDict->GetNumberFor("W");
  }

  if (const CPDF_Array* pBorderArray = pAnnotDict.GetArrayFor("Border")) {
    if (pBorderArray->size() > 2)
      return pBorderArray->GetNumberAt(2);
  }

  return 1;
}

const CPDF_Array* GetDashArray(const CPDF_Dictionary& pAnnotDict) {
  if (const CPDF_Dictionary* pBorderStyleDict = pAnnotDict.GetDictFor("BS")) {
    if (pBorderStyleDict->GetStringFor("S") == "D")
      return pBorderStyleDict->GetArrayFor("D");
  }

  if (const CPDF_Array* pBorderArray = pAnnotDict.GetArrayFor("Border")) {
    if (pBorderArray->size() == 4)
      return pBorderArray->GetArrayAt(3);
  }

  return nullptr;
}

ByteString GetDashPatternString(const CPDF_Dictionary& pAnnotDict) {
  const CPDF_Array* pDashArray = GetDashArray(pAnnotDict);
  if (!pDashArray || pDashArray->IsEmpty())
    return ByteString();

  // Support maximum of ten elements in the dash array.
  size_t pDashArrayCount = std::min<size_t>(pDashArray->size(), 10);
  std::ostringstream sDashStream;

  sDashStream << "[";
  for (size_t i = 0; i < pDashArrayCount; ++i)
    sDashStream << pDashArray->GetNumberAt(i) << " ";
  sDashStream << "] 0 d\n";

  return ByteString(sDashStream);
}

ByteString GetPopupContentsString(CPDF_Document* pDoc,
                                  const CPDF_Dictionary& pAnnotDict,
                                  CPDF_Font* pDefFont,
                                  const ByteString& sFontName) {
  WideString swValue(pAnnotDict.GetUnicodeTextFor("T"));
  swValue += L'\n';
  swValue += pAnnotDict.GetUnicodeTextFor("Contents");
  CPVT_FontMap map(pDoc, nullptr, pDefFont, sFontName);

  CPDF_VariableText::Provider prd(&map);
  CPDF_VariableText vt;
  vt.SetProvider(&prd);
  vt.SetPlateRect(pAnnotDict.GetRectFor("Rect"));
  vt.SetFontSize(12);
  vt.SetAutoReturn(true);
  vt.SetMultiLine(true);

  vt.Initialize();
  vt.SetText(swValue);
  vt.RearrangeAll();
  CFX_PointF ptOffset(3.0f, -3.0f);
  ByteString sContent =
      GenerateEditAP(&map, vt.GetIterator(), ptOffset, false, 0);

  if (sContent.IsEmpty())
    return ByteString();

  std::ostringstream sAppStream;
  sAppStream << "BT\n"
             << GenerateColorAP(CFX_Color(CFX_Color::kRGB, 0, 0, 0),
                                PaintOperation::FILL)
             << sContent << "ET\n"
             << "Q\n";
  return ByteString(sAppStream);
}

std::unique_ptr<CPDF_Dictionary> GenerateResourceFontDict(
    CPDF_Document* pDoc,
    const ByteString& sFontDictName) {
  CPDF_Dictionary* pFontDict = pDoc->NewIndirect<CPDF_Dictionary>();
  pFontDict->SetNewFor<CPDF_Name>("Type", "Font");
  pFontDict->SetNewFor<CPDF_Name>("Subtype", "Type1");
  pFontDict->SetNewFor<CPDF_Name>("BaseFont", CFX_Font::kDefaultAnsiFontName);
  pFontDict->SetNewFor<CPDF_Name>("Encoding", "WinAnsiEncoding");

  auto pResourceFontDict = pDoc->New<CPDF_Dictionary>();
  pResourceFontDict->SetFor(sFontDictName, pFontDict->MakeReference(pDoc));
  return pResourceFontDict;
}

ByteString GetPaintOperatorString(bool bIsStrokeRect, bool bIsFillRect) {
  if (bIsStrokeRect)
    return bIsFillRect ? "b" : "s";
  return bIsFillRect ? "f" : "n";
}

ByteString GenerateTextSymbolAP(const CFX_FloatRect& rect) {
  std::ostringstream sAppStream;
  sAppStream << GenerateColorAP(CFX_Color(CFX_Color::kRGB, 1, 1, 0),
                                PaintOperation::FILL);
  sAppStream << GenerateColorAP(CFX_Color(CFX_Color::kRGB, 0, 0, 0),
                                PaintOperation::STROKE);

  const float fBorderWidth = 1;
  sAppStream << fBorderWidth << " w\n";

  const float fHalfWidth = fBorderWidth / 2;
  const float fTipDelta = 4;

  CFX_FloatRect outerRect1 = rect;
  outerRect1.Deflate(fHalfWidth, fHalfWidth);
  outerRect1.bottom += fTipDelta;

  CFX_FloatRect outerRect2 = outerRect1;
  outerRect2.left += fTipDelta;
  outerRect2.right = outerRect2.left + fTipDelta;
  outerRect2.top = outerRect2.bottom - fTipDelta;
  float outerRect2Middle = (outerRect2.left + outerRect2.right) / 2;

  // Draw outer boxes.
  sAppStream << outerRect1.left << " " << outerRect1.bottom << " m\n"
             << outerRect1.left << " " << outerRect1.top << " l\n"
             << outerRect1.right << " " << outerRect1.top << " l\n"
             << outerRect1.right << " " << outerRect1.bottom << " l\n"
             << outerRect2.right << " " << outerRect2.bottom << " l\n"
             << outerRect2Middle << " " << outerRect2.top << " l\n"
             << outerRect2.left << " " << outerRect2.bottom << " l\n"
             << outerRect1.left << " " << outerRect1.bottom << " l\n";

  // Draw inner lines.
  CFX_FloatRect lineRect = outerRect1;
  const float fXDelta = 2;
  const float fYDelta = (lineRect.top - lineRect.bottom) / 4;

  lineRect.left += fXDelta;
  lineRect.right -= fXDelta;
  for (int i = 0; i < 3; ++i) {
    lineRect.top -= fYDelta;
    sAppStream << lineRect.left << " " << lineRect.top << " m\n"
               << lineRect.right << " " << lineRect.top << " l\n";
  }
  sAppStream << "B*\n";

  return ByteString(sAppStream);
}

std::unique_ptr<CPDF_Dictionary> GenerateExtGStateDict(
    const CPDF_Dictionary& pAnnotDict,
    const ByteString& sExtGSDictName,
    const ByteString& sBlendMode) {
  auto pGSDict =
      pdfium::MakeUnique<CPDF_Dictionary>(pAnnotDict.GetByteStringPool());
  pGSDict->SetNewFor<CPDF_Name>("Type", "ExtGState");

  float fOpacity =
      pAnnotDict.KeyExist("CA") ? pAnnotDict.GetNumberFor("CA") : 1;
  pGSDict->SetNewFor<CPDF_Number>("CA", fOpacity);
  pGSDict->SetNewFor<CPDF_Number>("ca", fOpacity);
  pGSDict->SetNewFor<CPDF_Boolean>("AIS", false);
  pGSDict->SetNewFor<CPDF_Name>("BM", sBlendMode);

  auto pExtGStateDict =
      pdfium::MakeUnique<CPDF_Dictionary>(pAnnotDict.GetByteStringPool());
  pExtGStateDict->SetFor(sExtGSDictName, std::move(pGSDict));
  return pExtGStateDict;
}

std::unique_ptr<CPDF_Dictionary> GenerateResourceDict(
    CPDF_Document* pDoc,
    std::unique_ptr<CPDF_Dictionary> pExtGStateDict,
    std::unique_ptr<CPDF_Dictionary> pResourceFontDict) {
  auto pResourceDict = pDoc->New<CPDF_Dictionary>();
  if (pExtGStateDict)
    pResourceDict->SetFor("ExtGState", std::move(pExtGStateDict));
  if (pResourceFontDict)
    pResourceDict->SetFor("Font", std::move(pResourceFontDict));
  return pResourceDict;
}

void GenerateAndSetAPDict(CPDF_Document* pDoc,
                          CPDF_Dictionary* pAnnotDict,
                          std::ostringstream* psAppStream,
                          std::unique_ptr<CPDF_Dictionary> pResourceDict,
                          bool bIsTextMarkupAnnotation) {
  CPDF_Stream* pNormalStream = pDoc->NewIndirect<CPDF_Stream>();
  pNormalStream->SetDataFromStringstream(psAppStream);

  CPDF_Dictionary* pAPDict = pAnnotDict->GetDictFor("AP");
  if (!pAPDict)
    pAPDict = pAnnotDict->SetNewFor<CPDF_Dictionary>("AP");

  pAPDict->SetFor("N", pNormalStream->MakeReference(pDoc));

  CPDF_Dictionary* pStreamDict = pNormalStream->GetDict();
  pStreamDict->SetNewFor<CPDF_Number>("FormType", 1);
  pStreamDict->SetNewFor<CPDF_Name>("Type", "XObject");
  pStreamDict->SetNewFor<CPDF_Name>("Subtype", "Form");
  pStreamDict->SetMatrixFor("Matrix", CFX_Matrix());

  CFX_FloatRect rect = bIsTextMarkupAnnotation
                           ? CPDF_Annot::BoundingRectFromQuadPoints(pAnnotDict)
                           : pAnnotDict->GetRectFor("Rect");
  pStreamDict->SetRectFor("BBox", rect);
  pStreamDict->SetFor("Resources", std::move(pResourceDict));
}

bool GenerateCircleAP(CPDF_Document* pDoc, CPDF_Dictionary* pAnnotDict) {
  std::ostringstream sAppStream;
  ByteString sExtGSDictName = "GS";
  sAppStream << "/" << sExtGSDictName << " gs ";

  CPDF_Array* pInteriorColor = pAnnotDict->GetArrayFor("IC");
  sAppStream << GetColorStringWithDefault(
      pInteriorColor, CFX_Color(CFX_Color::kTransparent), PaintOperation::FILL);

  sAppStream << GetColorStringWithDefault(pAnnotDict->GetArrayFor("C"),
                                          CFX_Color(CFX_Color::kRGB, 0, 0, 0),
                                          PaintOperation::STROKE);

  float fBorderWidth = GetBorderWidth(*pAnnotDict);
  bool bIsStrokeRect = fBorderWidth > 0;

  if (bIsStrokeRect) {
    sAppStream << fBorderWidth << " w ";
    sAppStream << GetDashPatternString(*pAnnotDict);
  }

  CFX_FloatRect rect = pAnnotDict->GetRectFor("Rect");
  rect.Normalize();

  if (bIsStrokeRect) {
    // Deflating rect because stroking a path entails painting all points whose
    // perpendicular distance from the path in user space is less than or equal
    // to half the line width.
    rect.Deflate(fBorderWidth / 2, fBorderWidth / 2);
  }

  const float fMiddleX = (rect.left + rect.right) / 2;
  const float fMiddleY = (rect.top + rect.bottom) / 2;

  // |fL| is precalculated approximate value of 4 * tan((3.14 / 2) / 4) / 3,
  // where |fL| * radius is a good approximation of control points for
  // arc with 90 degrees.
  const float fL = 0.5523f;
  const float fDeltaX = fL * rect.Width() / 2.0;
  const float fDeltaY = fL * rect.Height() / 2.0;

  // Starting point
  sAppStream << fMiddleX << " " << rect.top << " m\n";
  // First Bezier Curve
  sAppStream << fMiddleX + fDeltaX << " " << rect.top << " " << rect.right
             << " " << fMiddleY + fDeltaY << " " << rect.right << " "
             << fMiddleY << " c\n";
  // Second Bezier Curve
  sAppStream << rect.right << " " << fMiddleY - fDeltaY << " "
             << fMiddleX + fDeltaX << " " << rect.bottom << " " << fMiddleX
             << " " << rect.bottom << " c\n";
  // Third Bezier Curve
  sAppStream << fMiddleX - fDeltaX << " " << rect.bottom << " " << rect.left
             << " " << fMiddleY - fDeltaY << " " << rect.left << " " << fMiddleY
             << " c\n";
  // Fourth Bezier Curve
  sAppStream << rect.left << " " << fMiddleY + fDeltaY << " "
             << fMiddleX - fDeltaX << " " << rect.top << " " << fMiddleX << " "
             << rect.top << " c\n";

  bool bIsFillRect = pInteriorColor && !pInteriorColor->IsEmpty();
  sAppStream << GetPaintOperatorString(bIsStrokeRect, bIsFillRect) << "\n";

  auto pExtGStateDict =
      GenerateExtGStateDict(*pAnnotDict, sExtGSDictName, "Normal");
  auto pResourceDict =
      GenerateResourceDict(pDoc, std::move(pExtGStateDict), nullptr);
  GenerateAndSetAPDict(pDoc, pAnnotDict, &sAppStream, std::move(pResourceDict),
                       false /*IsTextMarkupAnnotation*/);
  return true;
}

bool GenerateHighlightAP(CPDF_Document* pDoc, CPDF_Dictionary* pAnnotDict) {
  std::ostringstream sAppStream;
  ByteString sExtGSDictName = "GS";
  sAppStream << "/" << sExtGSDictName << " gs ";

  sAppStream << GetColorStringWithDefault(pAnnotDict->GetArrayFor("C"),
                                          CFX_Color(CFX_Color::kRGB, 1, 1, 0),
                                          PaintOperation::FILL);

  CPDF_Array* pArray = pAnnotDict->GetArrayFor("QuadPoints");
  if (pArray) {
    size_t nQuadPointCount = CPDF_Annot::QuadPointCount(pArray);
    for (size_t i = 0; i < nQuadPointCount; ++i) {
      CFX_FloatRect rect = CPDF_Annot::RectFromQuadPoints(pAnnotDict, i);
      rect.Normalize();

      sAppStream << rect.left << " " << rect.top << " m " << rect.right << " "
                 << rect.top << " l " << rect.right << " " << rect.bottom
                 << " l " << rect.left << " " << rect.bottom << " l h f\n";
    }
  }

  auto pExtGStateDict =
      GenerateExtGStateDict(*pAnnotDict, sExtGSDictName, "Multiply");
  auto pResourceDict =
      GenerateResourceDict(pDoc, std::move(pExtGStateDict), nullptr);
  GenerateAndSetAPDict(pDoc, pAnnotDict, &sAppStream, std::move(pResourceDict),
                       true /*IsTextMarkupAnnotation*/);

  return true;
}

bool GenerateInkAP(CPDF_Document* pDoc, CPDF_Dictionary* pAnnotDict) {
  float fBorderWidth = GetBorderWidth(*pAnnotDict);
  bool bIsStroke = fBorderWidth > 0;

  if (!bIsStroke)
    return false;

  CPDF_Array* pInkList = pAnnotDict->GetArrayFor("InkList");
  if (!pInkList || pInkList->IsEmpty())
    return false;

  std::ostringstream sAppStream;
  ByteString sExtGSDictName = "GS";
  sAppStream << "/" << sExtGSDictName << " gs ";

  sAppStream << GetColorStringWithDefault(pAnnotDict->GetArrayFor("C"),
                                          CFX_Color(CFX_Color::kRGB, 0, 0, 0),
                                          PaintOperation::STROKE);

  sAppStream << fBorderWidth << " w ";
  sAppStream << GetDashPatternString(*pAnnotDict);

  // Set inflated rect as a new rect because paths near the border with large
  // width should not be clipped to the original rect.
  CFX_FloatRect rect = pAnnotDict->GetRectFor("Rect");
  rect.Inflate(fBorderWidth / 2, fBorderWidth / 2);
  pAnnotDict->SetRectFor("Rect", rect);

  for (size_t i = 0; i < pInkList->size(); i++) {
    CPDF_Array* pInkCoordList = pInkList->GetArrayAt(i);
    if (!pInkCoordList || pInkCoordList->size() < 2)
      continue;

    sAppStream << pInkCoordList->GetNumberAt(0) << " "
               << pInkCoordList->GetNumberAt(1) << " m ";

    for (size_t j = 0; j < pInkCoordList->size() - 1; j += 2) {
      sAppStream << pInkCoordList->GetNumberAt(j) << " "
                 << pInkCoordList->GetNumberAt(j + 1) << " l ";
    }

    sAppStream << "S\n";
  }

  auto pExtGStateDict =
      GenerateExtGStateDict(*pAnnotDict, sExtGSDictName, "Normal");
  auto pResourceDict =
      GenerateResourceDict(pDoc, std::move(pExtGStateDict), nullptr);
  GenerateAndSetAPDict(pDoc, pAnnotDict, &sAppStream, std::move(pResourceDict),
                       false /*IsTextMarkupAnnotation*/);
  return true;
}

bool GenerateTextAP(CPDF_Document* pDoc, CPDF_Dictionary* pAnnotDict) {
  std::ostringstream sAppStream;
  ByteString sExtGSDictName = "GS";
  sAppStream << "/" << sExtGSDictName << " gs ";

  CFX_FloatRect rect = pAnnotDict->GetRectFor("Rect");
  const float fNoteLength = 20;
  CFX_FloatRect noteRect(rect.left, rect.bottom, rect.left + fNoteLength,
                         rect.bottom + fNoteLength);
  pAnnotDict->SetRectFor("Rect", noteRect);

  sAppStream << GenerateTextSymbolAP(noteRect);

  auto pExtGStateDict =
      GenerateExtGStateDict(*pAnnotDict, sExtGSDictName, "Normal");
  auto pResourceDict =
      GenerateResourceDict(pDoc, std::move(pExtGStateDict), nullptr);
  GenerateAndSetAPDict(pDoc, pAnnotDict, &sAppStream, std::move(pResourceDict),
                       false /*IsTextMarkupAnnotation*/);
  return true;
}

bool GenerateUnderlineAP(CPDF_Document* pDoc, CPDF_Dictionary* pAnnotDict) {
  std::ostringstream sAppStream;
  ByteString sExtGSDictName = "GS";
  sAppStream << "/" << sExtGSDictName << " gs ";

  sAppStream << GetColorStringWithDefault(pAnnotDict->GetArrayFor("C"),
                                          CFX_Color(CFX_Color::kRGB, 0, 0, 0),
                                          PaintOperation::STROKE);

  CPDF_Array* pArray = pAnnotDict->GetArrayFor("QuadPoints");
  if (pArray) {
    static constexpr float kLineWidth = 1.0f;
    sAppStream << kLineWidth << " w ";
    size_t nQuadPointCount = CPDF_Annot::QuadPointCount(pArray);
    for (size_t i = 0; i < nQuadPointCount; ++i) {
      CFX_FloatRect rect = CPDF_Annot::RectFromQuadPoints(pAnnotDict, i);
      rect.Normalize();
      sAppStream << rect.left << " " << rect.bottom + kLineWidth << " m "
                 << rect.right << " " << rect.bottom + kLineWidth << " l S\n";
    }
  }

  auto pExtGStateDict =
      GenerateExtGStateDict(*pAnnotDict, sExtGSDictName, "Normal");
  auto pResourceDict =
      GenerateResourceDict(pDoc, std::move(pExtGStateDict), nullptr);
  GenerateAndSetAPDict(pDoc, pAnnotDict, &sAppStream, std::move(pResourceDict),
                       true /*IsTextMarkupAnnotation*/);
  return true;
}

bool GeneratePopupAP(CPDF_Document* pDoc, CPDF_Dictionary* pAnnotDict) {
  std::ostringstream sAppStream;
  ByteString sExtGSDictName = "GS";
  sAppStream << "/" << sExtGSDictName << " gs\n";

  sAppStream << GenerateColorAP(CFX_Color(CFX_Color::kRGB, 1, 1, 0),
                                PaintOperation::FILL);
  sAppStream << GenerateColorAP(CFX_Color(CFX_Color::kRGB, 0, 0, 0),
                                PaintOperation::STROKE);

  const float fBorderWidth = 1;
  sAppStream << fBorderWidth << " w\n";

  CFX_FloatRect rect = pAnnotDict->GetRectFor("Rect");
  rect.Normalize();
  rect.Deflate(fBorderWidth / 2, fBorderWidth / 2);

  sAppStream << rect.left << " " << rect.bottom << " " << rect.Width() << " "
             << rect.Height() << " re b\n";

  ByteString sFontName = "FONT";
  auto pResourceFontDict = GenerateResourceFontDict(pDoc, sFontName);
  CPDF_Font* pDefFont = pDoc->LoadFont(pResourceFontDict.get());
  if (!pDefFont)
    return false;

  auto pExtGStateDict =
      GenerateExtGStateDict(*pAnnotDict, sExtGSDictName, "Normal");
  auto pResourceDict = GenerateResourceDict(pDoc, std::move(pExtGStateDict),
                                            std::move(pResourceFontDict));

  sAppStream << GetPopupContentsString(pDoc, *pAnnotDict, pDefFont, sFontName);
  GenerateAndSetAPDict(pDoc, pAnnotDict, &sAppStream, std::move(pResourceDict),
                       false /*IsTextMarkupAnnotation*/);
  return true;
}

bool GenerateSquareAP(CPDF_Document* pDoc, CPDF_Dictionary* pAnnotDict) {
  std::ostringstream sAppStream;
  ByteString sExtGSDictName = "GS";
  sAppStream << "/" << sExtGSDictName << " gs ";

  CPDF_Array* pInteriorColor = pAnnotDict->GetArrayFor("IC");
  sAppStream << GetColorStringWithDefault(
      pInteriorColor, CFX_Color(CFX_Color::kTransparent), PaintOperation::FILL);

  sAppStream << GetColorStringWithDefault(pAnnotDict->GetArrayFor("C"),
                                          CFX_Color(CFX_Color::kRGB, 0, 0, 0),
                                          PaintOperation::STROKE);

  float fBorderWidth = GetBorderWidth(*pAnnotDict);
  bool bIsStrokeRect = fBorderWidth > 0;

  if (bIsStrokeRect) {
    sAppStream << fBorderWidth << " w ";
    sAppStream << GetDashPatternString(*pAnnotDict);
  }

  CFX_FloatRect rect = pAnnotDict->GetRectFor("Rect");
  rect.Normalize();

  if (bIsStrokeRect) {
    // Deflating rect because stroking a path entails painting all points whose
    // perpendicular distance from the path in user space is less than or equal
    // to half the line width.
    rect.Deflate(fBorderWidth / 2, fBorderWidth / 2);
  }

  bool bIsFillRect = pInteriorColor && (pInteriorColor->size() > 0);

  sAppStream << rect.left << " " << rect.bottom << " " << rect.Width() << " "
             << rect.Height() << " re "
             << GetPaintOperatorString(bIsStrokeRect, bIsFillRect) << "\n";

  auto pExtGStateDict =
      GenerateExtGStateDict(*pAnnotDict, sExtGSDictName, "Normal");
  auto pResourceDict =
      GenerateResourceDict(pDoc, std::move(pExtGStateDict), nullptr);
  GenerateAndSetAPDict(pDoc, pAnnotDict, &sAppStream, std::move(pResourceDict),
                       false /*IsTextMarkupAnnotation*/);
  return true;
}

bool GenerateSquigglyAP(CPDF_Document* pDoc, CPDF_Dictionary* pAnnotDict) {
  std::ostringstream sAppStream;
  ByteString sExtGSDictName = "GS";
  sAppStream << "/" << sExtGSDictName << " gs ";

  sAppStream << GetColorStringWithDefault(pAnnotDict->GetArrayFor("C"),
                                          CFX_Color(CFX_Color::kRGB, 0, 0, 0),
                                          PaintOperation::STROKE);

  CPDF_Array* pArray = pAnnotDict->GetArrayFor("QuadPoints");
  if (pArray) {
    static constexpr float kLineWidth = 1.0f;
    static constexpr float kDelta = 2.0f;
    sAppStream << kLineWidth << " w ";
    size_t nQuadPointCount = CPDF_Annot::QuadPointCount(pArray);
    for (size_t i = 0; i < nQuadPointCount; ++i) {
      CFX_FloatRect rect = CPDF_Annot::RectFromQuadPoints(pAnnotDict, i);
      rect.Normalize();

      const float fTop = rect.bottom + kDelta;
      const float fBottom = rect.bottom;
      sAppStream << rect.left << " " << fTop << " m ";

      float fX = rect.left + kDelta;
      bool isUpwards = false;
      while (fX < rect.right) {
        sAppStream << fX << " " << (isUpwards ? fTop : fBottom) << " l ";
        fX += kDelta;
        isUpwards = !isUpwards;
      }

      float fRemainder = rect.right - (fX - kDelta);
      if (isUpwards)
        sAppStream << rect.right << " " << fBottom + fRemainder << " l ";
      else
        sAppStream << rect.right << " " << fTop - fRemainder << " l ";

      sAppStream << "S\n";
    }
  }

  auto pExtGStateDict =
      GenerateExtGStateDict(*pAnnotDict, sExtGSDictName, "Normal");
  auto pResourceDict =
      GenerateResourceDict(pDoc, std::move(pExtGStateDict), nullptr);
  GenerateAndSetAPDict(pDoc, pAnnotDict, &sAppStream, std::move(pResourceDict),
                       true /*IsTextMarkupAnnotation*/);
  return true;
}

bool GenerateStrikeOutAP(CPDF_Document* pDoc, CPDF_Dictionary* pAnnotDict) {
  std::ostringstream sAppStream;
  ByteString sExtGSDictName = "GS";
  sAppStream << "/" << sExtGSDictName << " gs ";

  sAppStream << GetColorStringWithDefault(pAnnotDict->GetArrayFor("C"),
                                          CFX_Color(CFX_Color::kRGB, 0, 0, 0),
                                          PaintOperation::STROKE);

  CPDF_Array* pArray = pAnnotDict->GetArrayFor("QuadPoints");
  if (pArray) {
    static constexpr float kLineWidth = 1.0f;
    size_t nQuadPointCount = CPDF_Annot::QuadPointCount(pArray);
    for (size_t i = 0; i < nQuadPointCount; ++i) {
      CFX_FloatRect rect = CPDF_Annot::RectFromQuadPoints(pAnnotDict, i);
      rect.Normalize();

      float fY = (rect.top + rect.bottom) / 2;
      sAppStream << kLineWidth << " w " << rect.left << " " << fY << " m "
                 << rect.right << " " << fY << " l S\n";
    }
  }

  auto pExtGStateDict =
      GenerateExtGStateDict(*pAnnotDict, sExtGSDictName, "Normal");
  auto pResourceDict =
      GenerateResourceDict(pDoc, std::move(pExtGStateDict), nullptr);
  GenerateAndSetAPDict(pDoc, pAnnotDict, &sAppStream, std::move(pResourceDict),
                       true /*IsTextMarkupAnnotation*/);
  return true;
}

}  // namespace

// static
void CPVT_GenerateAP::GenerateFormAP(Type type,
                                     CPDF_Document* pDoc,
                                     CPDF_Dictionary* pAnnotDict) {
  CPDF_Dictionary* pRootDict = pDoc->GetRoot();
  if (!pRootDict)
    return;

  CPDF_Dictionary* pFormDict = pRootDict->GetDictFor("AcroForm");
  if (!pFormDict)
    return;

  ByteString DA;
  if (CPDF_Object* pDAObj = FPDF_GetFieldAttr(pAnnotDict, "DA"))
    DA = pDAObj->GetString();
  if (DA.IsEmpty())
    DA = pFormDict->GetStringFor("DA");
  if (DA.IsEmpty())
    return;

  CPDF_DefaultAppearance appearance(DA);

  float fFontSize = 0;
  Optional<ByteString> font = appearance.GetFont(&fFontSize);
  if (!font)
    return;

  ByteString font_name = *font;
  CFX_Color crText = CFX_Color::ParseColor(DA);
  CPDF_Dictionary* pDRDict = pFormDict->GetDictFor("DR");
  if (!pDRDict)
    return;

  CPDF_Dictionary* pDRFontDict = pDRDict->GetDictFor("Font");
  if (!pDRFontDict)
    return;

  CPDF_Dictionary* pFontDict = pDRFontDict->GetDictFor(font_name);
  if (!pFontDict) {
    pFontDict = pDoc->NewIndirect<CPDF_Dictionary>();
    pFontDict->SetNewFor<CPDF_Name>("Type", "Font");
    pFontDict->SetNewFor<CPDF_Name>("Subtype", "Type1");
    pFontDict->SetNewFor<CPDF_Name>("BaseFont", CFX_Font::kDefaultAnsiFontName);
    pFontDict->SetNewFor<CPDF_Name>("Encoding", "WinAnsiEncoding");
    pDRFontDict->SetFor(font_name, pFontDict->MakeReference(pDoc));
  }
  CPDF_Font* pDefFont = pDoc->LoadFont(pFontDict);
  if (!pDefFont)
    return;

  CFX_FloatRect rcAnnot = pAnnotDict->GetRectFor("Rect");
  int32_t nRotate = 0;
  if (CPDF_Dictionary* pMKDict = pAnnotDict->GetDictFor("MK"))
    nRotate = pMKDict->GetIntegerFor("R");

  CFX_FloatRect rcBBox;
  CFX_Matrix matrix;
  switch (nRotate % 360) {
    case 0:
      rcBBox = CFX_FloatRect(0, 0, rcAnnot.right - rcAnnot.left,
                             rcAnnot.top - rcAnnot.bottom);
      break;
    case 90:
      matrix = CFX_Matrix(0, 1, -1, 0, rcAnnot.right - rcAnnot.left, 0);
      rcBBox = CFX_FloatRect(0, 0, rcAnnot.top - rcAnnot.bottom,
                             rcAnnot.right - rcAnnot.left);
      break;
    case 180:
      matrix = CFX_Matrix(-1, 0, 0, -1, rcAnnot.right - rcAnnot.left,
                          rcAnnot.top - rcAnnot.bottom);
      rcBBox = CFX_FloatRect(0, 0, rcAnnot.right - rcAnnot.left,
                             rcAnnot.top - rcAnnot.bottom);
      break;
    case 270:
      matrix = CFX_Matrix(0, -1, 1, 0, 0, rcAnnot.top - rcAnnot.bottom);
      rcBBox = CFX_FloatRect(0, 0, rcAnnot.top - rcAnnot.bottom,
                             rcAnnot.right - rcAnnot.left);
      break;
  }

  BorderStyle nBorderStyle = BorderStyle::SOLID;
  float fBorderWidth = 1;
  CPVT_Dash dsBorder(3, 0, 0);
  CFX_Color crLeftTop;
  CFX_Color crRightBottom;
  if (CPDF_Dictionary* pBSDict = pAnnotDict->GetDictFor("BS")) {
    if (pBSDict->KeyExist("W"))
      fBorderWidth = pBSDict->GetNumberFor("W");

    if (CPDF_Array* pArray = pBSDict->GetArrayFor("D")) {
      dsBorder = CPVT_Dash(pArray->GetIntegerAt(0), pArray->GetIntegerAt(1),
                           pArray->GetIntegerAt(2));
    }
    if (pBSDict->GetStringFor("S").GetLength()) {
      switch (pBSDict->GetStringFor("S")[0]) {
        case 'S':
          nBorderStyle = BorderStyle::SOLID;
          break;
        case 'D':
          nBorderStyle = BorderStyle::DASH;
          break;
        case 'B':
          nBorderStyle = BorderStyle::BEVELED;
          fBorderWidth *= 2;
          crLeftTop = CFX_Color(CFX_Color::kGray, 1);
          crRightBottom = CFX_Color(CFX_Color::kGray, 0.5);
          break;
        case 'I':
          nBorderStyle = BorderStyle::INSET;
          fBorderWidth *= 2;
          crLeftTop = CFX_Color(CFX_Color::kGray, 0.5);
          crRightBottom = CFX_Color(CFX_Color::kGray, 0.75);
          break;
        case 'U':
          nBorderStyle = BorderStyle::UNDERLINE;
          break;
      }
    }
  }
  CFX_Color crBorder;
  CFX_Color crBG;
  if (CPDF_Dictionary* pMKDict = pAnnotDict->GetDictFor("MK")) {
    if (CPDF_Array* pArray = pMKDict->GetArrayFor("BC"))
      crBorder = CFX_Color::ParseColor(*pArray);
    if (CPDF_Array* pArray = pMKDict->GetArrayFor("BG"))
      crBG = CFX_Color::ParseColor(*pArray);
  }
  std::ostringstream sAppStream;
  ByteString sBG = GenerateColorAP(crBG, PaintOperation::FILL);
  if (sBG.GetLength() > 0) {
    sAppStream << "q\n"
               << sBG << rcBBox.left << " " << rcBBox.bottom << " "
               << rcBBox.Width() << " " << rcBBox.Height() << " re f\n"
               << "Q\n";
  }
  ByteString sBorderStream =
      GenerateBorderAP(rcBBox, fBorderWidth, crBorder, crLeftTop, crRightBottom,
                       nBorderStyle, dsBorder);
  if (sBorderStream.GetLength() > 0)
    sAppStream << "q\n" << sBorderStream << "Q\n";

  CFX_FloatRect rcBody =
      CFX_FloatRect(rcBBox.left + fBorderWidth, rcBBox.bottom + fBorderWidth,
                    rcBBox.right - fBorderWidth, rcBBox.top - fBorderWidth);
  rcBody.Normalize();

  CPDF_Dictionary* pAPDict = pAnnotDict->GetDictFor("AP");
  if (!pAPDict)
    pAPDict = pAnnotDict->SetNewFor<CPDF_Dictionary>("AP");

  CPDF_Stream* pNormalStream = pAPDict->GetStreamFor("N");
  if (!pNormalStream) {
    pNormalStream = pDoc->NewIndirect<CPDF_Stream>();
    pAPDict->SetFor("N", pNormalStream->MakeReference(pDoc));
  }
  CPDF_Dictionary* pStreamDict = pNormalStream->GetDict();
  if (pStreamDict) {
    pStreamDict->SetMatrixFor("Matrix", matrix);
    pStreamDict->SetRectFor("BBox", rcBBox);
    CPDF_Dictionary* pStreamResList = pStreamDict->GetDictFor("Resources");
    if (pStreamResList) {
      CPDF_Dictionary* pStreamResFontList = pStreamResList->GetDictFor("Font");
      if (!pStreamResFontList)
        pStreamResFontList = pStreamResList->SetNewFor<CPDF_Dictionary>("Font");
      if (!pStreamResFontList->KeyExist(font_name)) {
        pStreamResFontList->SetFor(font_name, pFontDict->MakeReference(pDoc));
      }
    } else {
      pStreamDict->SetFor("Resources", pFormDict->GetDictFor("DR")->Clone());
      pStreamResList = pStreamDict->GetDictFor("Resources");
    }
  }
  switch (type) {
    case CPVT_GenerateAP::kTextField: {
      WideString swValue =
          FPDF_GetFieldAttr(pAnnotDict, "V")
              ? FPDF_GetFieldAttr(pAnnotDict, "V")->GetUnicodeText()
              : WideString();
      int32_t nAlign = FPDF_GetFieldAttr(pAnnotDict, "Q")
                           ? FPDF_GetFieldAttr(pAnnotDict, "Q")->GetInteger()
                           : 0;
      uint32_t dwFlags = FPDF_GetFieldAttr(pAnnotDict, "Ff")
                             ? FPDF_GetFieldAttr(pAnnotDict, "Ff")->GetInteger()
                             : 0;
      uint32_t dwMaxLen =
          FPDF_GetFieldAttr(pAnnotDict, "MaxLen")
              ? FPDF_GetFieldAttr(pAnnotDict, "MaxLen")->GetInteger()
              : 0;
      CPVT_FontMap map(
          pDoc, pStreamDict ? pStreamDict->GetDictFor("Resources") : nullptr,
          pDefFont, font_name);
      CPDF_VariableText::Provider prd(&map);
      CPDF_VariableText vt;
      vt.SetProvider(&prd);
      vt.SetPlateRect(rcBody);
      vt.SetAlignment(nAlign);
      if (IsFloatZero(fFontSize))
        vt.SetAutoFontSize(true);
      else
        vt.SetFontSize(fFontSize);

      bool bMultiLine = (dwFlags >> 12) & 1;
      if (bMultiLine) {
        vt.SetMultiLine(true);
        vt.SetAutoReturn(true);
      }
      uint16_t subWord = 0;
      if ((dwFlags >> 13) & 1) {
        subWord = '*';
        vt.SetPasswordChar(subWord);
      }
      bool bCharArray = (dwFlags >> 24) & 1;
      if (bCharArray)
        vt.SetCharArray(dwMaxLen);
      else
        vt.SetLimitChar(dwMaxLen);

      vt.Initialize();
      vt.SetText(swValue);
      vt.RearrangeAll();
      CFX_FloatRect rcContent = vt.GetContentRect();
      CFX_PointF ptOffset;
      if (!bMultiLine) {
        ptOffset =
            CFX_PointF(0.0f, (rcContent.Height() - rcBody.Height()) / 2.0f);
      }
      ByteString sBody = GenerateEditAP(&map, vt.GetIterator(), ptOffset,
                                        !bCharArray, subWord);
      if (sBody.GetLength() > 0) {
        sAppStream << "/Tx BMC\n"
                   << "q\n";
        if (rcContent.Width() > rcBody.Width() ||
            rcContent.Height() > rcBody.Height()) {
          sAppStream << rcBody.left << " " << rcBody.bottom << " "
                     << rcBody.Width() << " " << rcBody.Height()
                     << " re\nW\nn\n";
        }
        sAppStream << "BT\n"
                   << GenerateColorAP(crText, PaintOperation::FILL) << sBody
                   << "ET\n"
                   << "Q\nEMC\n";
      }
      break;
    }
    case CPVT_GenerateAP::kComboBox: {
      WideString swValue =
          FPDF_GetFieldAttr(pAnnotDict, "V")
              ? FPDF_GetFieldAttr(pAnnotDict, "V")->GetUnicodeText()
              : WideString();
      CPVT_FontMap map(
          pDoc, pStreamDict ? pStreamDict->GetDictFor("Resources") : nullptr,
          pDefFont, font_name);
      CPDF_VariableText::Provider prd(&map);
      CPDF_VariableText vt;
      vt.SetProvider(&prd);
      CFX_FloatRect rcButton = rcBody;
      rcButton.left = rcButton.right - 13;
      rcButton.Normalize();
      CFX_FloatRect rcEdit = rcBody;
      rcEdit.right = rcButton.left;
      rcEdit.Normalize();
      vt.SetPlateRect(rcEdit);
      if (IsFloatZero(fFontSize))
        vt.SetAutoFontSize(true);
      else
        vt.SetFontSize(fFontSize);

      vt.Initialize();
      vt.SetText(swValue);
      vt.RearrangeAll();
      CFX_FloatRect rcContent = vt.GetContentRect();
      CFX_PointF ptOffset =
          CFX_PointF(0.0f, (rcContent.Height() - rcEdit.Height()) / 2.0f);
      ByteString sEdit =
          GenerateEditAP(&map, vt.GetIterator(), ptOffset, true, 0);
      if (sEdit.GetLength() > 0) {
        sAppStream << "/Tx BMC\n"
                   << "q\n";
        sAppStream << rcEdit.left << " " << rcEdit.bottom << " "
                   << rcEdit.Width() << " " << rcEdit.Height() << " re\nW\nn\n";
        sAppStream << "BT\n"
                   << GenerateColorAP(crText, PaintOperation::FILL) << sEdit
                   << "ET\n"
                   << "Q\nEMC\n";
      }
      ByteString sButton =
          GenerateColorAP(CFX_Color(CFX_Color::kRGB, 220.0f / 255.0f,
                                    220.0f / 255.0f, 220.0f / 255.0f),
                          PaintOperation::FILL);
      if (sButton.GetLength() > 0 && !rcButton.IsEmpty()) {
        sAppStream << "q\n" << sButton;
        sAppStream << rcButton.left << " " << rcButton.bottom << " "
                   << rcButton.Width() << " " << rcButton.Height() << " re f\n";
        sAppStream << "Q\n";
        ByteString sButtonBorder = GenerateBorderAP(
            rcButton, 2, CFX_Color(CFX_Color::kGray, 0),
            CFX_Color(CFX_Color::kGray, 1), CFX_Color(CFX_Color::kGray, 0.5),
            BorderStyle::BEVELED, CPVT_Dash(3, 0, 0));
        if (sButtonBorder.GetLength() > 0)
          sAppStream << "q\n" << sButtonBorder << "Q\n";

        CFX_PointF ptCenter = CFX_PointF((rcButton.left + rcButton.right) / 2,
                                         (rcButton.top + rcButton.bottom) / 2);
        if (IsFloatBigger(rcButton.Width(), 6) &&
            IsFloatBigger(rcButton.Height(), 6)) {
          sAppStream << "q\n"
                     << " 0 g\n";
          sAppStream << ptCenter.x - 3 << " " << ptCenter.y + 1.5f << " m\n";
          sAppStream << ptCenter.x + 3 << " " << ptCenter.y + 1.5f << " l\n";
          sAppStream << ptCenter.x << " " << ptCenter.y - 1.5f << " l\n";
          sAppStream << ptCenter.x - 3 << " " << ptCenter.y + 1.5f << " l f\n";
          sAppStream << sButton << "Q\n";
        }
      }
      break;
    }
    case CPVT_GenerateAP::kListBox: {
      CPVT_FontMap map(
          pDoc, pStreamDict ? pStreamDict->GetDictFor("Resources") : nullptr,
          pDefFont, font_name);
      CPDF_VariableText::Provider prd(&map);
      CPDF_Array* pOpts = ToArray(FPDF_GetFieldAttr(pAnnotDict, "Opt"));
      CPDF_Array* pSels = ToArray(FPDF_GetFieldAttr(pAnnotDict, "I"));
      CPDF_Object* pTi = FPDF_GetFieldAttr(pAnnotDict, "TI");
      int32_t nTop = pTi ? pTi->GetInteger() : 0;
      std::ostringstream sBody;
      if (pOpts) {
        float fy = rcBody.top;
        for (size_t i = nTop, sz = pOpts->size(); i < sz; i++) {
          if (IsFloatSmaller(fy, rcBody.bottom))
            break;

          if (CPDF_Object* pOpt = pOpts->GetDirectObjectAt(i)) {
            WideString swItem;
            if (pOpt->IsString())
              swItem = pOpt->GetUnicodeText();
            else if (CPDF_Array* pArray = pOpt->AsArray())
              swItem = pArray->GetDirectObjectAt(1)->GetUnicodeText();

            bool bSelected = false;
            if (pSels) {
              for (size_t s = 0, ssz = pSels->size(); s < ssz; s++) {
                int value = pSels->GetIntegerAt(s);
                if (value >= 0 && i == static_cast<size_t>(value)) {
                  bSelected = true;
                  break;
                }
              }
            }
            CPDF_VariableText vt;
            vt.SetProvider(&prd);
            vt.SetPlateRect(
                CFX_FloatRect(rcBody.left, 0.0f, rcBody.right, 0.0f));
            vt.SetFontSize(IsFloatZero(fFontSize) ? 12.0f : fFontSize);

            vt.Initialize();
            vt.SetText(swItem);
            vt.RearrangeAll();
            float fItemHeight = vt.GetContentRect().Height();
            if (bSelected) {
              CFX_FloatRect rcItem = CFX_FloatRect(
                  rcBody.left, fy - fItemHeight, rcBody.right, fy);
              sBody << "q\n"
                    << GenerateColorAP(
                           CFX_Color(CFX_Color::kRGB, 0, 51.0f / 255.0f,
                                     113.0f / 255.0f),
                           PaintOperation::FILL)
                    << rcItem.left << " " << rcItem.bottom << " "
                    << rcItem.Width() << " " << rcItem.Height() << " re f\n"
                    << "Q\n";
              sBody << "BT\n"
                    << GenerateColorAP(CFX_Color(CFX_Color::kGray, 1),
                                       PaintOperation::FILL)
                    << GenerateEditAP(&map, vt.GetIterator(),
                                      CFX_PointF(0.0f, fy), true, 0)
                    << "ET\n";
            } else {
              sBody << "BT\n"
                    << GenerateColorAP(crText, PaintOperation::FILL)
                    << GenerateEditAP(&map, vt.GetIterator(),
                                      CFX_PointF(0.0f, fy), true, 0)
                    << "ET\n";
            }
            fy -= fItemHeight;
          }
        }
      }
      if (sBody.tellp() > 0) {
        sAppStream << "/Tx BMC\nq\n"
                   << rcBody.left << " " << rcBody.bottom << " "
                   << rcBody.Width() << " " << rcBody.Height() << " re\nW\nn\n"
                   << sBody.str() << "Q\nEMC\n";
      }
      break;
    }
  }

  if (pNormalStream) {
    pNormalStream->SetDataFromStringstreamAndRemoveFilter(&sAppStream);
    pStreamDict = pNormalStream->GetDict();
    if (pStreamDict) {
      pStreamDict->SetMatrixFor("Matrix", matrix);
      pStreamDict->SetRectFor("BBox", rcBBox);
      CPDF_Dictionary* pStreamResList = pStreamDict->GetDictFor("Resources");
      if (pStreamResList) {
        CPDF_Dictionary* pStreamResFontList =
            pStreamResList->GetDictFor("Font");
        if (!pStreamResFontList) {
          pStreamResFontList =
              pStreamResList->SetNewFor<CPDF_Dictionary>("Font");
        }
        if (!pStreamResFontList->KeyExist(font_name)) {
          pStreamResFontList->SetFor(font_name, pFontDict->MakeReference(pDoc));
        }
      } else {
        pStreamDict->SetFor("Resources", pFormDict->GetDictFor("DR")->Clone());
        pStreamResList = pStreamDict->GetDictFor("Resources");
      }
    }
  }
  return;
}

// static
void CPVT_GenerateAP::GenerateEmptyAP(CPDF_Document* pDoc,
                                      CPDF_Dictionary* pAnnotDict) {
  auto pExtGStateDict = GenerateExtGStateDict(*pAnnotDict, "GS", "Normal");
  auto pResourceDict =
      GenerateResourceDict(pDoc, std::move(pExtGStateDict), nullptr);

  std::ostringstream sStream;
  GenerateAndSetAPDict(pDoc, pAnnotDict, &sStream, std::move(pResourceDict),
                       false);
}

// static
bool CPVT_GenerateAP::GenerateAnnotAP(CPDF_Annot::Subtype subtype,
                                      CPDF_Document* pDoc,
                                      CPDF_Dictionary* pAnnotDict) {
  switch (subtype) {
    case CPDF_Annot::Subtype::CIRCLE:
      return GenerateCircleAP(pDoc, pAnnotDict);
    case CPDF_Annot::Subtype::HIGHLIGHT:
      return GenerateHighlightAP(pDoc, pAnnotDict);
    case CPDF_Annot::Subtype::INK:
      return GenerateInkAP(pDoc, pAnnotDict);
    case CPDF_Annot::Subtype::POPUP:
      return GeneratePopupAP(pDoc, pAnnotDict);
    case CPDF_Annot::Subtype::SQUARE:
      return GenerateSquareAP(pDoc, pAnnotDict);
    case CPDF_Annot::Subtype::SQUIGGLY:
      return GenerateSquigglyAP(pDoc, pAnnotDict);
    case CPDF_Annot::Subtype::STRIKEOUT:
      return GenerateStrikeOutAP(pDoc, pAnnotDict);
    case CPDF_Annot::Subtype::TEXT:
      return GenerateTextAP(pDoc, pAnnotDict);
    case CPDF_Annot::Subtype::UNDERLINE:
      return GenerateUnderlineAP(pDoc, pAnnotDict);
    default:
      return false;
  }
}

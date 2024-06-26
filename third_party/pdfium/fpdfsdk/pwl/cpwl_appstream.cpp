// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/pwl/cpwl_appstream.h"

#include <utility>

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfapi/parser/fpdf_parser_decode.h"
#include "core/fpdfdoc/cpvt_word.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fpdfsdk/cpdfsdk_interactiveform.h"
#include "fpdfsdk/cpdfsdk_pageview.h"
#include "fpdfsdk/cpdfsdk_widget.h"
#include "fpdfsdk/formfiller/cba_fontmap.h"
#include "fpdfsdk/pwl/cpwl_edit.h"
#include "fpdfsdk/pwl/cpwl_edit_impl.h"
#include "fpdfsdk/pwl/cpwl_icon.h"
#include "fpdfsdk/pwl/cpwl_wnd.h"
#include "third_party/base/ptr_util.h"

namespace {

// Checkbox & radiobutton styles.
enum class CheckStyle { kCheck = 0, kCircle, kCross, kDiamond, kSquare, kStar };

// Pushbutton layout styles.
enum class ButtonStyle {
  kLabel = 0,
  kIcon,
  kIconTopLabelBottom,
  kIconBottomLabelTop,
  kIconLeftLabelRight,
  kIconRightLabelLeft,
  kLabelOverIcon
};

const char kAppendRectOperator[] = "re";
const char kConcatMatrixOperator[] = "cm";
const char kCurveToOperator[] = "c";
const char kEndPathNoFillOrStrokeOperator[] = "n";
const char kFillOperator[] = "f";
const char kFillEvenOddOperator[] = "f*";
const char kInvokeNamedXObjectOperator[] = "Do";
const char kLineToOperator[] = "l";
const char kMarkedSequenceBeginOperator[] = "BMC";
const char kMarkedSequenceEndOperator[] = "EMC";
const char kMoveTextPositionOperator[] = "Td";
const char kMoveToOperator[] = "m";
const char kSetCharacterSpacingOperator[] = "Tc";
const char kSetCMYKOperator[] = "k";
const char kSetCMKYStrokedOperator[] = "K";
const char kSetDashOperator[] = "d";
const char kSetGrayOperator[] = "g";
const char kSetGrayStrokedOperator[] = "G";
const char kSetLineCapStyleOperator[] = "J";
const char kSetLineJoinStyleOperator[] = "j";
const char kSetLineWidthOperator[] = "w";
const char kSetNonZeroWindingClipOperator[] = "W";
const char kSetRGBOperator[] = "rg";
const char kSetRGBStrokedOperator[] = "RG";
const char kSetTextFontAndSizeOperator[] = "Tf";
const char kShowTextOperator[] = "Tj";
const char kStateRestoreOperator[] = "Q";
const char kStateSaveOperator[] = "q";
const char kStrokeOperator[] = "S";
const char kTextBeginOperator[] = "BT";
const char kTextEndOperator[] = "ET";

class AutoClosedCommand {
 public:
  AutoClosedCommand(std::ostringstream* stream,
                    ByteString open,
                    ByteString close)
      : stream_(stream), close_(close) {
    *stream_ << open << "\n";
  }

  virtual ~AutoClosedCommand() { *stream_ << close_ << "\n"; }

 private:
  std::ostringstream* stream_;
  ByteString close_;
};

class AutoClosedQCommand final : public AutoClosedCommand {
 public:
  explicit AutoClosedQCommand(std::ostringstream* stream)
      : AutoClosedCommand(stream, kStateSaveOperator, kStateRestoreOperator) {}
  ~AutoClosedQCommand() override {}
};

ByteString GetColorAppStream(const CFX_Color& color,
                             const bool& bFillOrStroke) {
  std::ostringstream sColorStream;

  switch (color.nColorType) {
    case CFX_Color::kRGB:
      sColorStream << color.fColor1 << " " << color.fColor2 << " "
                   << color.fColor3 << " "
                   << (bFillOrStroke ? kSetRGBOperator : kSetRGBStrokedOperator)
                   << "\n";
      break;
    case CFX_Color::kGray:
      sColorStream << color.fColor1 << " "
                   << (bFillOrStroke ? kSetGrayOperator
                                     : kSetGrayStrokedOperator)
                   << "\n";
      break;
    case CFX_Color::kCMYK:
      sColorStream << color.fColor1 << " " << color.fColor2 << " "
                   << color.fColor3 << " " << color.fColor4 << " "
                   << (bFillOrStroke ? kSetCMYKOperator
                                     : kSetCMKYStrokedOperator)
                   << "\n";
      break;
  }

  return ByteString(sColorStream);
}

ByteString GetAP_Check(const CFX_FloatRect& crBBox) {
  const float fWidth = crBBox.Width();
  const float fHeight = crBBox.Height();

  CFX_PointF pts[8][3] = {{CFX_PointF(0.28f, 0.52f), CFX_PointF(0.27f, 0.48f),
                           CFX_PointF(0.29f, 0.40f)},
                          {CFX_PointF(0.30f, 0.33f), CFX_PointF(0.31f, 0.29f),
                           CFX_PointF(0.31f, 0.28f)},
                          {CFX_PointF(0.39f, 0.28f), CFX_PointF(0.49f, 0.29f),
                           CFX_PointF(0.77f, 0.67f)},
                          {CFX_PointF(0.76f, 0.68f), CFX_PointF(0.78f, 0.69f),
                           CFX_PointF(0.76f, 0.75f)},
                          {CFX_PointF(0.76f, 0.75f), CFX_PointF(0.73f, 0.80f),
                           CFX_PointF(0.68f, 0.75f)},
                          {CFX_PointF(0.68f, 0.74f), CFX_PointF(0.68f, 0.74f),
                           CFX_PointF(0.44f, 0.47f)},
                          {CFX_PointF(0.43f, 0.47f), CFX_PointF(0.40f, 0.47f),
                           CFX_PointF(0.41f, 0.58f)},
                          {CFX_PointF(0.40f, 0.60f), CFX_PointF(0.28f, 0.66f),
                           CFX_PointF(0.30f, 0.56f)}};

  for (size_t i = 0; i < FX_ArraySize(pts); ++i) {
    for (size_t j = 0; j < FX_ArraySize(pts[0]); ++j) {
      pts[i][j].x = pts[i][j].x * fWidth + crBBox.left;
      pts[i][j].y *= pts[i][j].y * fHeight + crBBox.bottom;
    }
  }

  std::ostringstream csAP;
  csAP << pts[0][0].x << " " << pts[0][0].y << " " << kMoveToOperator << "\n";

  for (size_t i = 0; i < FX_ArraySize(pts); ++i) {
    size_t nNext = i < FX_ArraySize(pts) - 1 ? i + 1 : 0;

    float px1 = pts[i][1].x - pts[i][0].x;
    float py1 = pts[i][1].y - pts[i][0].y;
    float px2 = pts[i][2].x - pts[nNext][0].x;
    float py2 = pts[i][2].y - pts[nNext][0].y;

    csAP << pts[i][0].x + px1 * FX_BEZIER << " "
         << pts[i][0].y + py1 * FX_BEZIER << " "
         << pts[nNext][0].x + px2 * FX_BEZIER << " "
         << pts[nNext][0].y + py2 * FX_BEZIER << " " << pts[nNext][0].x << " "
         << pts[nNext][0].y << " " << kCurveToOperator << "\n";
  }

  return ByteString(csAP);
}

ByteString GetAP_Circle(const CFX_FloatRect& crBBox) {
  std::ostringstream csAP;

  float fWidth = crBBox.Width();
  float fHeight = crBBox.Height();

  CFX_PointF pt1(crBBox.left, crBBox.bottom + fHeight / 2);
  CFX_PointF pt2(crBBox.left + fWidth / 2, crBBox.top);
  CFX_PointF pt3(crBBox.right, crBBox.bottom + fHeight / 2);
  CFX_PointF pt4(crBBox.left + fWidth / 2, crBBox.bottom);

  csAP << pt1.x << " " << pt1.y << " " << kMoveToOperator << "\n";

  float px = pt2.x - pt1.x;
  float py = pt2.y - pt1.y;

  csAP << pt1.x << " " << pt1.y + py * FX_BEZIER << " "
       << pt2.x - px * FX_BEZIER << " " << pt2.y << " " << pt2.x << " " << pt2.y
       << " " << kCurveToOperator << "\n";

  px = pt3.x - pt2.x;
  py = pt2.y - pt3.y;

  csAP << pt2.x + px * FX_BEZIER << " " << pt2.y << " " << pt3.x << " "
       << pt3.y + py * FX_BEZIER << " " << pt3.x << " " << pt3.y << " "
       << kCurveToOperator << "\n";

  px = pt3.x - pt4.x;
  py = pt3.y - pt4.y;

  csAP << pt3.x << " " << pt3.y - py * FX_BEZIER << " "
       << pt4.x + px * FX_BEZIER << " " << pt4.y << " " << pt4.x << " " << pt4.y
       << " " << kCurveToOperator << "\n";

  px = pt4.x - pt1.x;
  py = pt1.y - pt4.y;

  csAP << pt4.x - px * FX_BEZIER << " " << pt4.y << " " << pt1.x << " "
       << pt1.y - py * FX_BEZIER << " " << pt1.x << " " << pt1.y << " "
       << kCurveToOperator << "\n";

  return ByteString(csAP);
}

ByteString GetAP_Cross(const CFX_FloatRect& crBBox) {
  std::ostringstream csAP;

  csAP << crBBox.left << " " << crBBox.top << " " << kMoveToOperator << "\n";
  csAP << crBBox.right << " " << crBBox.bottom << " " << kLineToOperator
       << "\n";
  csAP << crBBox.left << " " << crBBox.bottom << " " << kMoveToOperator << "\n";
  csAP << crBBox.right << " " << crBBox.top << " " << kLineToOperator << "\n";

  return ByteString(csAP);
}

ByteString GetAP_Diamond(const CFX_FloatRect& crBBox) {
  std::ostringstream csAP;

  float fWidth = crBBox.Width();
  float fHeight = crBBox.Height();

  CFX_PointF pt1(crBBox.left, crBBox.bottom + fHeight / 2);
  CFX_PointF pt2(crBBox.left + fWidth / 2, crBBox.top);
  CFX_PointF pt3(crBBox.right, crBBox.bottom + fHeight / 2);
  CFX_PointF pt4(crBBox.left + fWidth / 2, crBBox.bottom);

  csAP << pt1.x << " " << pt1.y << " " << kMoveToOperator << "\n";
  csAP << pt2.x << " " << pt2.y << " " << kLineToOperator << "\n";
  csAP << pt3.x << " " << pt3.y << " " << kLineToOperator << "\n";
  csAP << pt4.x << " " << pt4.y << " " << kLineToOperator << "\n";
  csAP << pt1.x << " " << pt1.y << " " << kLineToOperator << "\n";

  return ByteString(csAP);
}

ByteString GetAP_Square(const CFX_FloatRect& crBBox) {
  std::ostringstream csAP;

  csAP << crBBox.left << " " << crBBox.top << " " << kMoveToOperator << "\n";
  csAP << crBBox.right << " " << crBBox.top << " " << kLineToOperator << "\n";
  csAP << crBBox.right << " " << crBBox.bottom << " " << kLineToOperator
       << "\n";
  csAP << crBBox.left << " " << crBBox.bottom << " " << kLineToOperator << "\n";
  csAP << crBBox.left << " " << crBBox.top << " " << kLineToOperator << "\n";

  return ByteString(csAP);
}

ByteString GetAP_Star(const CFX_FloatRect& crBBox) {
  std::ostringstream csAP;

  float fRadius = (crBBox.top - crBBox.bottom) / (1 + (float)cos(FX_PI / 5.0f));
  CFX_PointF ptCenter = CFX_PointF((crBBox.left + crBBox.right) / 2.0f,
                                   (crBBox.top + crBBox.bottom) / 2.0f);

  float px[5];
  float py[5];
  float fAngel = FX_PI / 10.0f;
  for (int32_t i = 0; i < 5; i++) {
    px[i] = ptCenter.x + fRadius * (float)cos(fAngel);
    py[i] = ptCenter.y + fRadius * (float)sin(fAngel);
    fAngel += FX_PI * 2 / 5.0f;
  }

  csAP << px[0] << " " << py[0] << " " << kMoveToOperator << "\n";

  int32_t nNext = 0;
  for (int32_t j = 0; j < 5; j++) {
    nNext += 2;
    if (nNext >= 5)
      nNext -= 5;
    csAP << px[nNext] << " " << py[nNext] << " " << kLineToOperator << "\n";
  }

  return ByteString(csAP);
}

ByteString GetAP_HalfCircle(const CFX_FloatRect& crBBox, float fRotate) {
  std::ostringstream csAP;

  float fWidth = crBBox.Width();
  float fHeight = crBBox.Height();

  CFX_PointF pt1(-fWidth / 2, 0);
  CFX_PointF pt2(0, fHeight / 2);
  CFX_PointF pt3(fWidth / 2, 0);

  float px;
  float py;

  csAP << cos(fRotate) << " " << sin(fRotate) << " " << -sin(fRotate) << " "
       << cos(fRotate) << " " << crBBox.left + fWidth / 2 << " "
       << crBBox.bottom + fHeight / 2 << " " << kConcatMatrixOperator << "\n";

  csAP << pt1.x << " " << pt1.y << " " << kMoveToOperator << "\n";

  px = pt2.x - pt1.x;
  py = pt2.y - pt1.y;

  csAP << pt1.x << " " << pt1.y + py * FX_BEZIER << " "
       << pt2.x - px * FX_BEZIER << " " << pt2.y << " " << pt2.x << " " << pt2.y
       << " " << kCurveToOperator << "\n";

  px = pt3.x - pt2.x;
  py = pt2.y - pt3.y;

  csAP << pt2.x + px * FX_BEZIER << " " << pt2.y << " " << pt3.x << " "
       << pt3.y + py * FX_BEZIER << " " << pt3.x << " " << pt3.y << " "
       << kCurveToOperator << "\n";

  return ByteString(csAP);
}

ByteString GetAppStream_Check(const CFX_FloatRect& rcBBox,
                              const CFX_Color& crText) {
  std::ostringstream sAP;
  {
    AutoClosedQCommand q(&sAP);
    sAP << GetColorAppStream(crText, true) << GetAP_Check(rcBBox)
        << kFillOperator << "\n";
  }
  return ByteString(sAP);
}

ByteString GetAppStream_Circle(const CFX_FloatRect& rcBBox,
                               const CFX_Color& crText) {
  std::ostringstream sAP;
  {
    AutoClosedQCommand q(&sAP);
    sAP << GetColorAppStream(crText, true) << GetAP_Circle(rcBBox)
        << kFillOperator << "\n";
  }
  return ByteString(sAP);
}

ByteString GetAppStream_Cross(const CFX_FloatRect& rcBBox,
                              const CFX_Color& crText) {
  std::ostringstream sAP;
  {
    AutoClosedQCommand q(&sAP);
    sAP << GetColorAppStream(crText, false) << GetAP_Cross(rcBBox)
        << kStrokeOperator << "\n";
  }
  return ByteString(sAP);
}

ByteString GetAppStream_Diamond(const CFX_FloatRect& rcBBox,
                                const CFX_Color& crText) {
  std::ostringstream sAP;
  {
    AutoClosedQCommand q(&sAP);
    sAP << "1 " << kSetLineWidthOperator << "\n"
        << GetColorAppStream(crText, true) << GetAP_Diamond(rcBBox)
        << kFillOperator << "\n";
  }
  return ByteString(sAP);
}

ByteString GetAppStream_Square(const CFX_FloatRect& rcBBox,
                               const CFX_Color& crText) {
  std::ostringstream sAP;
  {
    AutoClosedQCommand q(&sAP);
    sAP << GetColorAppStream(crText, true) << GetAP_Square(rcBBox)
        << kFillOperator << "\n";
  }
  return ByteString(sAP);
}

ByteString GetAppStream_Star(const CFX_FloatRect& rcBBox,
                             const CFX_Color& crText) {
  std::ostringstream sAP;
  {
    AutoClosedQCommand q(&sAP);
    sAP << GetColorAppStream(crText, true) << GetAP_Star(rcBBox)
        << kFillOperator << "\n";
  }
  return ByteString(sAP);
}

ByteString GetCircleFillAppStream(const CFX_FloatRect& rect,
                                  const CFX_Color& color) {
  std::ostringstream sAppStream;
  ByteString sColor = GetColorAppStream(color, true);
  if (sColor.GetLength() > 0) {
    AutoClosedQCommand q(&sAppStream);
    sAppStream << sColor << GetAP_Circle(rect) << kFillOperator << "\n";
  }
  return ByteString(sAppStream);
}

ByteString GetCircleBorderAppStream(const CFX_FloatRect& rect,
                                    float fWidth,
                                    const CFX_Color& color,
                                    const CFX_Color& crLeftTop,
                                    const CFX_Color& crRightBottom,
                                    BorderStyle nStyle,
                                    const CPWL_Dash& dash) {
  std::ostringstream sAppStream;
  ByteString sColor;

  if (fWidth > 0.0f) {
    AutoClosedQCommand q(&sAppStream);

    float fHalfWidth = fWidth / 2.0f;
    CFX_FloatRect rect_by_2 = rect.GetDeflated(fHalfWidth, fHalfWidth);

    float div = fHalfWidth * 0.75f;
    CFX_FloatRect rect_by_75 = rect.GetDeflated(div, div);
    switch (nStyle) {
      default:
      case BorderStyle::SOLID:
      case BorderStyle::UNDERLINE: {
        sColor = GetColorAppStream(color, false);
        if (sColor.GetLength() > 0) {
          AutoClosedQCommand q2(&sAppStream);
          sAppStream << fWidth << " " << kSetLineWidthOperator << "\n"
                     << sColor << GetAP_Circle(rect_by_2) << " "
                     << kStrokeOperator << "\n";
        }
      } break;
      case BorderStyle::DASH: {
        sColor = GetColorAppStream(color, false);
        if (sColor.GetLength() > 0) {
          AutoClosedQCommand q2(&sAppStream);
          sAppStream << fWidth << " " << kSetLineWidthOperator << "\n"
                     << "[" << dash.nDash << " " << dash.nGap << "] "
                     << dash.nPhase << " " << kSetDashOperator << "\n"
                     << sColor << GetAP_Circle(rect_by_2) << " "
                     << kStrokeOperator << "\n";
        }
      } break;
      case BorderStyle::BEVELED: {
        sColor = GetColorAppStream(color, false);
        if (sColor.GetLength() > 0) {
          AutoClosedQCommand q2(&sAppStream);
          sAppStream << fHalfWidth << " " << kSetLineWidthOperator << "\n"
                     << sColor << GetAP_Circle(rect) << " " << kStrokeOperator
                     << "\n";
        }

        sColor = GetColorAppStream(crLeftTop, false);
        if (sColor.GetLength() > 0) {
          AutoClosedQCommand q2(&sAppStream);
          sAppStream << fHalfWidth << " " << kSetLineWidthOperator << "\n"
                     << sColor << GetAP_HalfCircle(rect_by_75, FX_PI / 4.0f)
                     << " " << kStrokeOperator << "\n";
        }

        sColor = GetColorAppStream(crRightBottom, false);
        if (sColor.GetLength() > 0) {
          AutoClosedQCommand q2(&sAppStream);
          sAppStream << fHalfWidth << " " << kSetLineWidthOperator << "\n"
                     << sColor << GetAP_HalfCircle(rect_by_75, FX_PI * 5 / 4.0f)
                     << " " << kStrokeOperator << "\n";
        }
      } break;
      case BorderStyle::INSET: {
        sColor = GetColorAppStream(color, false);
        if (sColor.GetLength() > 0) {
          AutoClosedQCommand q2(&sAppStream);
          sAppStream << fHalfWidth << " " << kSetLineWidthOperator << "\n"
                     << sColor << GetAP_Circle(rect) << " " << kStrokeOperator
                     << "\n";
        }

        sColor = GetColorAppStream(crLeftTop, false);
        if (sColor.GetLength() > 0) {
          AutoClosedQCommand q2(&sAppStream);
          sAppStream << fHalfWidth << " " << kSetLineWidthOperator << "\n"
                     << sColor << GetAP_HalfCircle(rect_by_75, FX_PI / 4.0f)
                     << " " << kStrokeOperator << "\n";
        }

        sColor = GetColorAppStream(crRightBottom, false);
        if (sColor.GetLength() > 0) {
          AutoClosedQCommand q2(&sAppStream);
          sAppStream << fHalfWidth << " " << kSetLineWidthOperator << "\n"
                     << sColor << GetAP_HalfCircle(rect_by_75, FX_PI * 5 / 4.0f)
                     << " " << kStrokeOperator << "\n";
        }
      } break;
    }
  }
  return ByteString(sAppStream);
}

ByteString GetCheckBoxAppStream(const CFX_FloatRect& rcBBox,
                                CheckStyle nStyle,
                                const CFX_Color& crText) {
  CFX_FloatRect rcCenter = rcBBox.GetCenterSquare();
  switch (nStyle) {
    default:
    case CheckStyle::kCheck:
      return GetAppStream_Check(rcCenter, crText);
    case CheckStyle::kCircle:
      rcCenter.ScaleFromCenterPoint(2.0f / 3.0f);
      return GetAppStream_Circle(rcCenter, crText);
    case CheckStyle::kCross:
      return GetAppStream_Cross(rcCenter, crText);
    case CheckStyle::kDiamond:
      rcCenter.ScaleFromCenterPoint(2.0f / 3.0f);
      return GetAppStream_Diamond(rcCenter, crText);
    case CheckStyle::kSquare:
      rcCenter.ScaleFromCenterPoint(2.0f / 3.0f);
      return GetAppStream_Square(rcCenter, crText);
    case CheckStyle::kStar:
      rcCenter.ScaleFromCenterPoint(2.0f / 3.0f);
      return GetAppStream_Star(rcCenter, crText);
  }
}

ByteString GetRadioButtonAppStream(const CFX_FloatRect& rcBBox,
                                   CheckStyle nStyle,
                                   const CFX_Color& crText) {
  CFX_FloatRect rcCenter = rcBBox.GetCenterSquare();
  switch (nStyle) {
    default:
    case CheckStyle::kCheck:
      return GetAppStream_Check(rcCenter, crText);
    case CheckStyle::kCircle:
      rcCenter.ScaleFromCenterPoint(1.0f / 2.0f);
      return GetAppStream_Circle(rcCenter, crText);
    case CheckStyle::kCross:
      return GetAppStream_Cross(rcCenter, crText);
    case CheckStyle::kDiamond:
      rcCenter.ScaleFromCenterPoint(2.0f / 3.0f);
      return GetAppStream_Diamond(rcCenter, crText);
    case CheckStyle::kSquare:
      rcCenter.ScaleFromCenterPoint(2.0f / 3.0f);
      return GetAppStream_Square(rcCenter, crText);
    case CheckStyle::kStar:
      rcCenter.ScaleFromCenterPoint(2.0f / 3.0f);
      return GetAppStream_Star(rcCenter, crText);
  }
}

ByteString GetFontSetString(IPVT_FontMap* pFontMap,
                            int32_t nFontIndex,
                            float fFontSize) {
  if (!pFontMap)
    return ByteString();

  ByteString sFontAlias = pFontMap->GetPDFFontAlias(nFontIndex);
  if (sFontAlias.GetLength() <= 0 || fFontSize <= 0)
    return ByteString();

  std::ostringstream sRet;
  sRet << "/" << sFontAlias << " " << fFontSize << " "
       << kSetTextFontAndSizeOperator << "\n";
  return ByteString(sRet);
}

ByteString GetWordRenderString(const ByteString& strWords) {
  if (strWords.GetLength() > 0) {
    return PDF_EncodeString(strWords, false) + " " + kShowTextOperator + "\n";
  }
  return ByteString();
}

ByteString GetEditAppStream(CPWL_EditImpl* pEdit,
                            const CFX_PointF& ptOffset,
                            bool bContinuous,
                            uint16_t SubWord) {
  CPWL_EditImpl_Iterator* pIterator = pEdit->GetIterator();
  pIterator->SetAt(0);

  std::ostringstream sEditStream;
  std::ostringstream sWords;
  int32_t nCurFontIndex = -1;
  CFX_PointF ptOld;
  CFX_PointF ptNew;
  CPVT_WordPlace oldplace;

  while (pIterator->NextWord()) {
    CPVT_WordPlace place = pIterator->GetAt();
    if (bContinuous) {
      if (place.LineCmp(oldplace) != 0) {
        if (sWords.tellp() > 0) {
          sEditStream << GetWordRenderString(ByteString(sWords));
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

        if (ptNew.x != ptOld.x || ptNew.y != ptOld.y) {
          sEditStream << ptNew.x - ptOld.x << " " << ptNew.y - ptOld.y << " "
                      << kMoveTextPositionOperator << "\n";

          ptOld = ptNew;
        }
      }

      CPVT_Word word;
      if (pIterator->GetWord(word)) {
        if (word.nFontIndex != nCurFontIndex) {
          if (sWords.tellp() > 0) {
            sEditStream << GetWordRenderString(ByteString(sWords));
            sWords.str("");
          }
          sEditStream << GetFontSetString(pEdit->GetFontMap(), word.nFontIndex,
                                          word.fFontSize);
          nCurFontIndex = word.nFontIndex;
        }

        sWords << pEdit->GetPDFWordString(nCurFontIndex, word.Word, SubWord);
      }

      oldplace = place;
    } else {
      CPVT_Word word;
      if (pIterator->GetWord(word)) {
        ptNew =
            CFX_PointF(word.ptWord.x + ptOffset.x, word.ptWord.y + ptOffset.y);

        if (ptNew.x != ptOld.x || ptNew.y != ptOld.y) {
          sEditStream << ptNew.x - ptOld.x << " " << ptNew.y - ptOld.y << " "
                      << kMoveTextPositionOperator << "\n";
          ptOld = ptNew;
        }

        if (word.nFontIndex != nCurFontIndex) {
          sEditStream << GetFontSetString(pEdit->GetFontMap(), word.nFontIndex,
                                          word.fFontSize);
          nCurFontIndex = word.nFontIndex;
        }

        sEditStream << GetWordRenderString(
            pEdit->GetPDFWordString(nCurFontIndex, word.Word, SubWord));
      }
    }
  }

  if (sWords.tellp() > 0) {
    sEditStream << GetWordRenderString(ByteString(sWords));
    sWords.str("");
  }

  std::ostringstream sAppStream;
  if (sEditStream.tellp() > 0) {
    float fCharSpace = pEdit->GetCharSpace();
    if (!IsFloatZero(fCharSpace))
      sAppStream << fCharSpace << " " << kSetCharacterSpacingOperator << "\n";

    sAppStream << sEditStream.str();
  }

  return ByteString(sAppStream);
}

ByteString GenerateIconAppStream(CPDF_IconFit& fit,
                                 CPDF_Stream* pIconStream,
                                 const CFX_FloatRect& rcIcon) {
  if (rcIcon.IsEmpty() || !pIconStream)
    return ByteString();

  CPWL_Wnd::CreateParams cp;
  cp.dwFlags = PWS_VISIBLE;

  CPWL_Icon icon(cp, nullptr);
  icon.Realize();
  icon.SetIconFit(&fit);
  icon.SetPDFStream(pIconStream);
  if (!icon.Move(rcIcon, false, false))
    return ByteString();

  ByteString sAlias = icon.GetImageAlias();
  if (sAlias.GetLength() <= 0)
    return ByteString();

  CFX_FloatRect rcPlate = icon.GetClientRect();
  CFX_Matrix mt = icon.GetImageMatrix().GetInverse();

  float fHScale;
  float fVScale;
  std::tie(fHScale, fVScale) = icon.GetScale();

  float fx;
  float fy;
  std::tie(fx, fy) = icon.GetImageOffset();

  std::ostringstream str;
  {
    AutoClosedQCommand q(&str);
    str << rcPlate.left << " " << rcPlate.bottom << " "
        << rcPlate.right - rcPlate.left << " " << rcPlate.top - rcPlate.bottom
        << " " << kAppendRectOperator << " " << kSetNonZeroWindingClipOperator
        << " " << kEndPathNoFillOrStrokeOperator << "\n";

    str << fHScale << " 0 0 " << fVScale << " " << rcPlate.left + fx << " "
        << rcPlate.bottom + fy << " " << kConcatMatrixOperator << "\n";
    str << mt.a << " " << mt.b << " " << mt.c << " " << mt.d << " " << mt.e
        << " " << mt.f << " " << kConcatMatrixOperator << "\n";

    str << "0 " << kSetGrayOperator << " 0 " << kSetGrayStrokedOperator << " 1 "
        << kSetLineWidthOperator << " /" << sAlias << " "
        << kInvokeNamedXObjectOperator << "\n";
  }
  icon.Destroy();

  return ByteString(str);
}

ByteString GetPushButtonAppStream(const CFX_FloatRect& rcBBox,
                                  IPVT_FontMap* pFontMap,
                                  CPDF_Stream* pIconStream,
                                  CPDF_IconFit& IconFit,
                                  const WideString& sLabel,
                                  const CFX_Color& crText,
                                  float fFontSize,
                                  ButtonStyle nLayOut) {
  const float fAutoFontScale = 1.0f / 3.0f;

  auto pEdit = pdfium::MakeUnique<CPWL_EditImpl>();
  pEdit->SetFontMap(pFontMap);
  pEdit->SetAlignmentH(1, true);
  pEdit->SetAlignmentV(1, true);
  pEdit->SetMultiLine(false, true);
  pEdit->SetAutoReturn(false, true);
  if (IsFloatZero(fFontSize))
    pEdit->SetAutoFontSize(true, true);
  else
    pEdit->SetFontSize(fFontSize);

  pEdit->Initialize();
  pEdit->SetText(sLabel);

  CFX_FloatRect rcLabelContent = pEdit->GetContentRect();
  CFX_FloatRect rcLabel;
  CFX_FloatRect rcIcon;
  float fWidth = 0.0f;
  float fHeight = 0.0f;

  switch (nLayOut) {
    case ButtonStyle::kLabel:
      rcLabel = rcBBox;
      break;
    case ButtonStyle::kIcon:
      rcIcon = rcBBox;
      break;
    case ButtonStyle::kIconTopLabelBottom:
      if (pIconStream) {
        if (IsFloatZero(fFontSize)) {
          fHeight = rcBBox.Height();
          rcLabel = CFX_FloatRect(rcBBox.left, rcBBox.bottom, rcBBox.right,
                                  rcBBox.bottom + fHeight * fAutoFontScale);
          rcIcon =
              CFX_FloatRect(rcBBox.left, rcLabel.top, rcBBox.right, rcBBox.top);
        } else {
          fHeight = rcLabelContent.Height();

          if (rcBBox.bottom + fHeight > rcBBox.top) {
            rcLabel = rcBBox;
          } else {
            rcLabel = CFX_FloatRect(rcBBox.left, rcBBox.bottom, rcBBox.right,
                                    rcBBox.bottom + fHeight);
            rcIcon = CFX_FloatRect(rcBBox.left, rcLabel.top, rcBBox.right,
                                   rcBBox.top);
          }
        }
      } else {
        rcLabel = rcBBox;
      }
      break;
    case ButtonStyle::kIconBottomLabelTop:
      if (pIconStream) {
        if (IsFloatZero(fFontSize)) {
          fHeight = rcBBox.Height();
          rcLabel =
              CFX_FloatRect(rcBBox.left, rcBBox.top - fHeight * fAutoFontScale,
                            rcBBox.right, rcBBox.top);
          rcIcon = CFX_FloatRect(rcBBox.left, rcBBox.bottom, rcBBox.right,
                                 rcLabel.bottom);
        } else {
          fHeight = rcLabelContent.Height();

          if (rcBBox.bottom + fHeight > rcBBox.top) {
            rcLabel = rcBBox;
          } else {
            rcLabel = CFX_FloatRect(rcBBox.left, rcBBox.top - fHeight,
                                    rcBBox.right, rcBBox.top);
            rcIcon = CFX_FloatRect(rcBBox.left, rcBBox.bottom, rcBBox.right,
                                   rcLabel.bottom);
          }
        }
      } else {
        rcLabel = rcBBox;
      }
      break;
    case ButtonStyle::kIconLeftLabelRight:
      if (pIconStream) {
        if (IsFloatZero(fFontSize)) {
          fWidth = rcBBox.right - rcBBox.left;
          if (rcLabelContent.Width() < fWidth * fAutoFontScale) {
            rcLabel = CFX_FloatRect(rcBBox.right - fWidth * fAutoFontScale,
                                    rcBBox.bottom, rcBBox.right, rcBBox.top);
            rcIcon = CFX_FloatRect(rcBBox.left, rcBBox.bottom, rcLabel.left,
                                   rcBBox.top);
          } else {
            if (rcLabelContent.Width() < fWidth) {
              rcLabel = CFX_FloatRect(rcBBox.right - rcLabelContent.Width(),
                                      rcBBox.bottom, rcBBox.right, rcBBox.top);
              rcIcon = CFX_FloatRect(rcBBox.left, rcBBox.bottom, rcLabel.left,
                                     rcBBox.top);
            } else {
              rcLabel = rcBBox;
            }
          }
        } else {
          fWidth = rcLabelContent.Width();
          if (rcBBox.left + fWidth > rcBBox.right) {
            rcLabel = rcBBox;
          } else {
            rcLabel = CFX_FloatRect(rcBBox.right - fWidth, rcBBox.bottom,
                                    rcBBox.right, rcBBox.top);
            rcIcon = CFX_FloatRect(rcBBox.left, rcBBox.bottom, rcLabel.left,
                                   rcBBox.top);
          }
        }
      } else {
        rcLabel = rcBBox;
      }
      break;
    case ButtonStyle::kIconRightLabelLeft:
      if (pIconStream) {
        if (IsFloatZero(fFontSize)) {
          fWidth = rcBBox.right - rcBBox.left;
          if (rcLabelContent.Width() < fWidth * fAutoFontScale) {
            rcLabel = CFX_FloatRect(rcBBox.left, rcBBox.bottom,
                                    rcBBox.left + fWidth * fAutoFontScale,
                                    rcBBox.top);
            rcIcon = CFX_FloatRect(rcLabel.right, rcBBox.bottom, rcBBox.right,
                                   rcBBox.top);
          } else {
            if (rcLabelContent.Width() < fWidth) {
              rcLabel = CFX_FloatRect(rcBBox.left, rcBBox.bottom,
                                      rcBBox.left + rcLabelContent.Width(),
                                      rcBBox.top);
              rcIcon = CFX_FloatRect(rcLabel.right, rcBBox.bottom, rcBBox.right,
                                     rcBBox.top);
            } else {
              rcLabel = rcBBox;
            }
          }
        } else {
          fWidth = rcLabelContent.Width();
          if (rcBBox.left + fWidth > rcBBox.right) {
            rcLabel = rcBBox;
          } else {
            rcLabel = CFX_FloatRect(rcBBox.left, rcBBox.bottom,
                                    rcBBox.left + fWidth, rcBBox.top);
            rcIcon = CFX_FloatRect(rcLabel.right, rcBBox.bottom, rcBBox.right,
                                   rcBBox.top);
          }
        }
      } else {
        rcLabel = rcBBox;
      }
      break;
    case ButtonStyle::kLabelOverIcon:
      rcLabel = rcBBox;
      rcIcon = rcBBox;
      break;
  }

  std::ostringstream sTemp;
  sTemp << GenerateIconAppStream(IconFit, pIconStream, rcIcon);

  if (!rcLabel.IsEmpty()) {
    pEdit->SetPlateRect(rcLabel);
    ByteString sEdit =
        GetEditAppStream(pEdit.get(), CFX_PointF(0.0f, 0.0f), true, 0);
    if (sEdit.GetLength() > 0) {
      AutoClosedCommand bt(&sTemp, kTextBeginOperator, kTextEndOperator);
      sTemp << GetColorAppStream(crText, true) << sEdit;
    }
  }

  if (sTemp.tellp() <= 0)
    return ByteString();

  std::ostringstream sAppStream;
  {
    AutoClosedQCommand q(&sAppStream);
    sAppStream << rcBBox.left << " " << rcBBox.bottom << " "
               << rcBBox.right - rcBBox.left << " "
               << rcBBox.top - rcBBox.bottom << " " << kAppendRectOperator
               << " " << kSetNonZeroWindingClipOperator << " "
               << kEndPathNoFillOrStrokeOperator << "\n";
    sAppStream << sTemp.str().c_str();
  }
  return ByteString(sAppStream);
}

ByteString GetBorderAppStreamInternal(const CFX_FloatRect& rect,
                                      float fWidth,
                                      const CFX_Color& color,
                                      const CFX_Color& crLeftTop,
                                      const CFX_Color& crRightBottom,
                                      BorderStyle nStyle,
                                      const CPWL_Dash& dash) {
  std::ostringstream sAppStream;
  ByteString sColor;

  float fLeft = rect.left;
  float fRight = rect.right;
  float fTop = rect.top;
  float fBottom = rect.bottom;

  if (fWidth > 0.0f) {
    float fHalfWidth = fWidth / 2.0f;
    AutoClosedQCommand q(&sAppStream);

    switch (nStyle) {
      default:
      case BorderStyle::SOLID:
        sColor = GetColorAppStream(color, true);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fLeft << " " << fBottom << " " << fRight - fLeft << " "
                     << fTop - fBottom << " " << kAppendRectOperator << "\n";
          sAppStream << fLeft + fWidth << " " << fBottom + fWidth << " "
                     << fRight - fLeft - fWidth * 2 << " "
                     << fTop - fBottom - fWidth * 2 << " "
                     << kAppendRectOperator << "\n";
          sAppStream << kFillEvenOddOperator << "\n";
        }
        break;
      case BorderStyle::DASH:
        sColor = GetColorAppStream(color, false);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fWidth << " " << kSetLineWidthOperator << " ["
                     << dash.nDash << " " << dash.nGap << "] " << dash.nPhase
                     << " " << kSetDashOperator << "\n";
          sAppStream << fLeft + fWidth / 2 << " " << fBottom + fWidth / 2 << " "
                     << kMoveToOperator << "\n";
          sAppStream << fLeft + fWidth / 2 << " " << fTop - fWidth / 2 << " "
                     << kLineToOperator << "\n";
          sAppStream << fRight - fWidth / 2 << " " << fTop - fWidth / 2 << " "
                     << kLineToOperator << "\n";
          sAppStream << fRight - fWidth / 2 << " " << fBottom + fWidth / 2
                     << " " << kLineToOperator << "\n";
          sAppStream << fLeft + fWidth / 2 << " " << fBottom + fWidth / 2 << " "
                     << kLineToOperator << " " << kStrokeOperator << "\n";
        }
        break;
      case BorderStyle::BEVELED:
      case BorderStyle::INSET:
        sColor = GetColorAppStream(crLeftTop, true);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fLeft + fHalfWidth << " " << fBottom + fHalfWidth << " "
                     << kMoveToOperator << "\n";
          sAppStream << fLeft + fHalfWidth << " " << fTop - fHalfWidth << " "
                     << kLineToOperator << "\n";
          sAppStream << fRight - fHalfWidth << " " << fTop - fHalfWidth << " "
                     << kLineToOperator << "\n";
          sAppStream << fRight - fHalfWidth * 2 << " " << fTop - fHalfWidth * 2
                     << " " << kLineToOperator << "\n";
          sAppStream << fLeft + fHalfWidth * 2 << " " << fTop - fHalfWidth * 2
                     << " " << kLineToOperator << "\n";
          sAppStream << fLeft + fHalfWidth * 2 << " "
                     << fBottom + fHalfWidth * 2 << " " << kLineToOperator
                     << " " << kFillOperator << "\n";
        }

        sColor = GetColorAppStream(crRightBottom, true);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fRight - fHalfWidth << " " << fTop - fHalfWidth << " "
                     << kMoveToOperator << "\n";
          sAppStream << fRight - fHalfWidth << " " << fBottom + fHalfWidth
                     << " " << kLineToOperator << "\n";
          sAppStream << fLeft + fHalfWidth << " " << fBottom + fHalfWidth << " "
                     << kLineToOperator << "\n";
          sAppStream << fLeft + fHalfWidth * 2 << " "
                     << fBottom + fHalfWidth * 2 << " " << kLineToOperator
                     << "\n";
          sAppStream << fRight - fHalfWidth * 2 << " "
                     << fBottom + fHalfWidth * 2 << " " << kLineToOperator
                     << "\n";
          sAppStream << fRight - fHalfWidth * 2 << " " << fTop - fHalfWidth * 2
                     << " " << kLineToOperator << " " << kFillOperator << "\n";
        }

        sColor = GetColorAppStream(color, true);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fLeft << " " << fBottom << " " << fRight - fLeft << " "
                     << fTop - fBottom << " " << kAppendRectOperator << "\n";
          sAppStream << fLeft + fHalfWidth << " " << fBottom + fHalfWidth << " "
                     << fRight - fLeft - fHalfWidth * 2 << " "
                     << fTop - fBottom - fHalfWidth * 2 << " "
                     << kAppendRectOperator << " " << kFillEvenOddOperator
                     << "\n";
        }
        break;
      case BorderStyle::UNDERLINE:
        sColor = GetColorAppStream(color, false);
        if (sColor.GetLength() > 0) {
          sAppStream << sColor;
          sAppStream << fWidth << " " << kSetLineWidthOperator << "\n";
          sAppStream << fLeft << " " << fBottom + fWidth / 2 << " "
                     << kMoveToOperator << "\n";
          sAppStream << fRight << " " << fBottom + fWidth / 2 << " "
                     << kLineToOperator << " " << kStrokeOperator << "\n";
        }
        break;
    }
  }

  return ByteString(sAppStream);
}

ByteString GetDropButtonAppStream(const CFX_FloatRect& rcBBox) {
  if (rcBBox.IsEmpty())
    return ByteString();

  std::ostringstream sAppStream;
  {
    AutoClosedQCommand q(&sAppStream);
    sAppStream << GetColorAppStream(CFX_Color(CFX_Color::kRGB, 220.0f / 255.0f,
                                              220.0f / 255.0f, 220.0f / 255.0f),
                                    true)
               << rcBBox.left << " " << rcBBox.bottom << " "
               << rcBBox.right - rcBBox.left << " "
               << rcBBox.top - rcBBox.bottom << " " << kAppendRectOperator
               << " " << kFillOperator << "\n";
  }

  {
    AutoClosedQCommand q(&sAppStream);
    sAppStream << GetBorderAppStreamInternal(
        rcBBox, 2, CFX_Color(CFX_Color::kGray, 0),
        CFX_Color(CFX_Color::kGray, 1), CFX_Color(CFX_Color::kGray, 0.5),
        BorderStyle::BEVELED, CPWL_Dash(3, 0, 0));
  }

  CFX_PointF ptCenter = CFX_PointF((rcBBox.left + rcBBox.right) / 2,
                                   (rcBBox.top + rcBBox.bottom) / 2);
  if (IsFloatBigger(rcBBox.right - rcBBox.left, 6) &&
      IsFloatBigger(rcBBox.top - rcBBox.bottom, 6)) {
    AutoClosedQCommand q(&sAppStream);
    sAppStream << " 0 " << kSetGrayOperator << "\n"
               << ptCenter.x - 3 << " " << ptCenter.y + 1.5f << " "
               << kMoveToOperator << "\n"
               << ptCenter.x + 3 << " " << ptCenter.y + 1.5f << " "
               << kLineToOperator << "\n"
               << ptCenter.x << " " << ptCenter.y - 1.5f << " "
               << kLineToOperator << "\n"
               << ptCenter.x - 3 << " " << ptCenter.y + 1.5f << " "
               << kLineToOperator << " " << kFillOperator << "\n";
  }

  return ByteString(sAppStream);
}

ByteString GetRectFillAppStream(const CFX_FloatRect& rect,
                                const CFX_Color& color) {
  std::ostringstream sAppStream;
  ByteString sColor = GetColorAppStream(color, true);
  if (sColor.GetLength() > 0) {
    AutoClosedQCommand q(&sAppStream);
    sAppStream << sColor << rect.left << " " << rect.bottom << " "
               << rect.right - rect.left << " " << rect.top - rect.bottom << " "
               << kAppendRectOperator << " " << kFillOperator << "\n";
  }

  return ByteString(sAppStream);
}

}  // namespace

CPWL_AppStream::CPWL_AppStream(CPDFSDK_Widget* widget, CPDF_Dictionary* dict)
    : widget_(widget), dict_(dict) {}

CPWL_AppStream::~CPWL_AppStream() {}

void CPWL_AppStream::SetAsPushButton() {
  CPDF_FormControl* pControl = widget_->GetFormControl();
  CFX_FloatRect rcWindow = widget_->GetRotatedRect();
  ButtonStyle nLayout = ButtonStyle::kLabel;
  switch (pControl->GetTextPosition()) {
    case TEXTPOS_ICON:
      nLayout = ButtonStyle::kIcon;
      break;
    case TEXTPOS_BELOW:
      nLayout = ButtonStyle::kIconTopLabelBottom;
      break;
    case TEXTPOS_ABOVE:
      nLayout = ButtonStyle::kIconBottomLabelTop;
      break;
    case TEXTPOS_RIGHT:
      nLayout = ButtonStyle::kIconLeftLabelRight;
      break;
    case TEXTPOS_LEFT:
      nLayout = ButtonStyle::kIconRightLabelLeft;
      break;
    case TEXTPOS_OVERLAID:
      nLayout = ButtonStyle::kLabelOverIcon;
      break;
    default:
      nLayout = ButtonStyle::kLabel;
      break;
  }

  CFX_Color crBackground;
  CFX_Color crBorder;
  int iColorType;
  float fc[4];
  pControl->GetOriginalBackgroundColor(iColorType, fc);
  if (iColorType > 0)
    crBackground = CFX_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  pControl->GetOriginalBorderColor(iColorType, fc);
  if (iColorType > 0)
    crBorder = CFX_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  float fBorderWidth = static_cast<float>(widget_->GetBorderWidth());
  CPWL_Dash dsBorder(3, 0, 0);
  CFX_Color crLeftTop;
  CFX_Color crRightBottom;

  BorderStyle nBorderStyle = widget_->GetBorderStyle();
  switch (nBorderStyle) {
    case BorderStyle::DASH:
      dsBorder = CPWL_Dash(3, 3, 0);
      break;
    case BorderStyle::BEVELED:
      fBorderWidth *= 2;
      crLeftTop = CFX_Color(CFX_Color::kGray, 1);
      crRightBottom = crBackground / 2.0f;
      break;
    case BorderStyle::INSET:
      fBorderWidth *= 2;
      crLeftTop = CFX_Color(CFX_Color::kGray, 0.5);
      crRightBottom = CFX_Color(CFX_Color::kGray, 0.75);
      break;
    default:
      break;
  }

  CFX_FloatRect rcClient = rcWindow.GetDeflated(fBorderWidth, fBorderWidth);
  CFX_Color crText(CFX_Color::kGray, 0);
  ByteString csNameTag;
  CPDF_DefaultAppearance da = pControl->GetDefaultAppearance();
  Optional<CFX_Color::Type> color = da.GetColor(fc);
  if (color) {
    iColorType = *color;
    crText = CFX_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);
  }

  float fFontSize;
  Optional<ByteString> font = da.GetFont(&fFontSize);
  if (font)
    csNameTag = *font;
  else
    fFontSize = 12.0f;

  WideString csWCaption;
  WideString csNormalCaption;
  WideString csRolloverCaption;
  WideString csDownCaption;
  if (pControl->HasMKEntry("CA"))
    csNormalCaption = pControl->GetNormalCaption();

  if (pControl->HasMKEntry("RC"))
    csRolloverCaption = pControl->GetRolloverCaption();

  if (pControl->HasMKEntry("AC"))
    csDownCaption = pControl->GetDownCaption();

  CPDF_Stream* pNormalIcon = nullptr;
  CPDF_Stream* pRolloverIcon = nullptr;
  CPDF_Stream* pDownIcon = nullptr;
  if (pControl->HasMKEntry("I"))
    pNormalIcon = pControl->GetNormalIcon();

  if (pControl->HasMKEntry("RI"))
    pRolloverIcon = pControl->GetRolloverIcon();

  if (pControl->HasMKEntry("IX"))
    pDownIcon = pControl->GetDownIcon();

  if (pNormalIcon) {
    if (CPDF_Dictionary* pImageDict = pNormalIcon->GetDict()) {
      if (pImageDict->GetStringFor("Name").IsEmpty())
        pImageDict->SetNewFor<CPDF_String>("Name", "ImgA", false);
    }
  }

  if (pRolloverIcon) {
    if (CPDF_Dictionary* pImageDict = pRolloverIcon->GetDict()) {
      if (pImageDict->GetStringFor("Name").IsEmpty())
        pImageDict->SetNewFor<CPDF_String>("Name", "ImgB", false);
    }
  }

  if (pDownIcon) {
    if (CPDF_Dictionary* pImageDict = pDownIcon->GetDict()) {
      if (pImageDict->GetStringFor("Name").IsEmpty())
        pImageDict->SetNewFor<CPDF_String>("Name", "ImgC", false);
    }
  }

  CPDF_IconFit iconFit = pControl->GetIconFit();

  CBA_FontMap font_map(
      widget_.Get(),
      widget_->GetInteractiveForm()->GetFormFillEnv()->GetSysHandler());
  font_map.SetAPType("N");

  ByteString csAP =
      GetRectFillAppStream(rcWindow, crBackground) +
      GetBorderAppStreamInternal(rcWindow, fBorderWidth, crBorder, crLeftTop,
                                 crRightBottom, nBorderStyle, dsBorder) +
      GetPushButtonAppStream(iconFit.GetFittingBounds() ? rcWindow : rcClient,
                             &font_map, pNormalIcon, iconFit, csNormalCaption,
                             crText, fFontSize, nLayout);

  Write("N", csAP, ByteString());
  if (pNormalIcon)
    AddImage("N", pNormalIcon);

  CPDF_FormControl::HighlightingMode eHLM = pControl->GetHighlightingMode();
  if (eHLM == CPDF_FormControl::Push || eHLM == CPDF_FormControl::Toggle) {
    if (csRolloverCaption.IsEmpty() && !pRolloverIcon) {
      csRolloverCaption = csNormalCaption;
      pRolloverIcon = pNormalIcon;
    }

    font_map.SetAPType("R");

    csAP =
        GetRectFillAppStream(rcWindow, crBackground) +
        GetBorderAppStreamInternal(rcWindow, fBorderWidth, crBorder, crLeftTop,
                                   crRightBottom, nBorderStyle, dsBorder) +
        GetPushButtonAppStream(iconFit.GetFittingBounds() ? rcWindow : rcClient,
                               &font_map, pRolloverIcon, iconFit,
                               csRolloverCaption, crText, fFontSize, nLayout);

    Write("R", csAP, ByteString());
    if (pRolloverIcon)
      AddImage("R", pRolloverIcon);

    if (csDownCaption.IsEmpty() && !pDownIcon) {
      csDownCaption = csNormalCaption;
      pDownIcon = pNormalIcon;
    }

    switch (nBorderStyle) {
      case BorderStyle::BEVELED: {
        CFX_Color crTemp = crLeftTop;
        crLeftTop = crRightBottom;
        crRightBottom = crTemp;
        break;
      }
      case BorderStyle::INSET: {
        crLeftTop = CFX_Color(CFX_Color::kGray, 0);
        crRightBottom = CFX_Color(CFX_Color::kGray, 1);
        break;
      }
      default:
        break;
    }

    font_map.SetAPType("D");

    csAP =
        GetRectFillAppStream(rcWindow, crBackground - 0.25f) +
        GetBorderAppStreamInternal(rcWindow, fBorderWidth, crBorder, crLeftTop,
                                   crRightBottom, nBorderStyle, dsBorder) +
        GetPushButtonAppStream(iconFit.GetFittingBounds() ? rcWindow : rcClient,
                               &font_map, pDownIcon, iconFit, csDownCaption,
                               crText, fFontSize, nLayout);

    Write("D", csAP, ByteString());
    if (pDownIcon)
      AddImage("D", pDownIcon);
  } else {
    Remove("D");
    Remove("R");
  }
}

void CPWL_AppStream::SetAsCheckBox() {
  CPDF_FormControl* pControl = widget_->GetFormControl();
  CFX_Color crBackground, crBorder, crText;
  int iColorType;
  float fc[4];

  pControl->GetOriginalBackgroundColor(iColorType, fc);
  if (iColorType > 0)
    crBackground = CFX_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  pControl->GetOriginalBorderColor(iColorType, fc);
  if (iColorType > 0)
    crBorder = CFX_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  float fBorderWidth = static_cast<float>(widget_->GetBorderWidth());
  CPWL_Dash dsBorder(3, 0, 0);
  CFX_Color crLeftTop, crRightBottom;

  BorderStyle nBorderStyle = widget_->GetBorderStyle();
  switch (nBorderStyle) {
    case BorderStyle::DASH:
      dsBorder = CPWL_Dash(3, 3, 0);
      break;
    case BorderStyle::BEVELED:
      fBorderWidth *= 2;
      crLeftTop = CFX_Color(CFX_Color::kGray, 1);
      crRightBottom = crBackground / 2.0f;
      break;
    case BorderStyle::INSET:
      fBorderWidth *= 2;
      crLeftTop = CFX_Color(CFX_Color::kGray, 0.5);
      crRightBottom = CFX_Color(CFX_Color::kGray, 0.75);
      break;
    default:
      break;
  }

  CFX_FloatRect rcWindow = widget_->GetRotatedRect();
  CFX_FloatRect rcClient = rcWindow.GetDeflated(fBorderWidth, fBorderWidth);
  CPDF_DefaultAppearance da = pControl->GetDefaultAppearance();
  Optional<CFX_Color::Type> color = da.GetColor(fc);
  if (color) {
    iColorType = *color;
    crText = CFX_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);
  }

  CheckStyle nStyle = CheckStyle::kCheck;
  WideString csWCaption = pControl->GetNormalCaption();
  if (csWCaption.GetLength() > 0) {
    switch (csWCaption[0]) {
      case L'l':
        nStyle = CheckStyle::kCircle;
        break;
      case L'8':
        nStyle = CheckStyle::kCross;
        break;
      case L'u':
        nStyle = CheckStyle::kDiamond;
        break;
      case L'n':
        nStyle = CheckStyle::kSquare;
        break;
      case L'H':
        nStyle = CheckStyle::kStar;
        break;
      case L'4':
      default:
        nStyle = CheckStyle::kCheck;
    }
  }

  ByteString csAP_N_ON =
      GetRectFillAppStream(rcWindow, crBackground) +
      GetBorderAppStreamInternal(rcWindow, fBorderWidth, crBorder, crLeftTop,
                                 crRightBottom, nBorderStyle, dsBorder);

  ByteString csAP_N_OFF = csAP_N_ON;

  switch (nBorderStyle) {
    case BorderStyle::BEVELED: {
      CFX_Color crTemp = crLeftTop;
      crLeftTop = crRightBottom;
      crRightBottom = crTemp;
      break;
    }
    case BorderStyle::INSET: {
      crLeftTop = CFX_Color(CFX_Color::kGray, 0);
      crRightBottom = CFX_Color(CFX_Color::kGray, 1);
      break;
    }
    default:
      break;
  }

  ByteString csAP_D_ON =
      GetRectFillAppStream(rcWindow, crBackground - 0.25f) +
      GetBorderAppStreamInternal(rcWindow, fBorderWidth, crBorder, crLeftTop,
                                 crRightBottom, nBorderStyle, dsBorder);

  ByteString csAP_D_OFF = csAP_D_ON;

  csAP_N_ON += GetCheckBoxAppStream(rcClient, nStyle, crText);
  csAP_D_ON += GetCheckBoxAppStream(rcClient, nStyle, crText);

  Write("N", csAP_N_ON, pControl->GetCheckedAPState());
  Write("N", csAP_N_OFF, "Off");

  Write("D", csAP_D_ON, pControl->GetCheckedAPState());
  Write("D", csAP_D_OFF, "Off");

  ByteString csAS = widget_->GetAppState();
  if (csAS.IsEmpty())
    widget_->SetAppState("Off");
}

void CPWL_AppStream::SetAsRadioButton() {
  CPDF_FormControl* pControl = widget_->GetFormControl();
  CFX_Color crBackground;
  CFX_Color crBorder;
  CFX_Color crText;
  int iColorType;
  float fc[4];

  pControl->GetOriginalBackgroundColor(iColorType, fc);
  if (iColorType > 0)
    crBackground = CFX_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  pControl->GetOriginalBorderColor(iColorType, fc);
  if (iColorType > 0)
    crBorder = CFX_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  float fBorderWidth = static_cast<float>(widget_->GetBorderWidth());
  CPWL_Dash dsBorder(3, 0, 0);
  CFX_Color crLeftTop;
  CFX_Color crRightBottom;
  BorderStyle nBorderStyle = widget_->GetBorderStyle();
  switch (nBorderStyle) {
    case BorderStyle::DASH:
      dsBorder = CPWL_Dash(3, 3, 0);
      break;
    case BorderStyle::BEVELED:
      fBorderWidth *= 2;
      crLeftTop = CFX_Color(CFX_Color::kGray, 1);
      crRightBottom = crBackground / 2.0f;
      break;
    case BorderStyle::INSET:
      fBorderWidth *= 2;
      crLeftTop = CFX_Color(CFX_Color::kGray, 0.5);
      crRightBottom = CFX_Color(CFX_Color::kGray, 0.75);
      break;
    default:
      break;
  }

  CFX_FloatRect rcWindow = widget_->GetRotatedRect();
  CFX_FloatRect rcClient = rcWindow.GetDeflated(fBorderWidth, fBorderWidth);
  CPDF_DefaultAppearance da = pControl->GetDefaultAppearance();
  Optional<CFX_Color::Type> color = da.GetColor(fc);
  if (color) {
    iColorType = *color;
    crText = CFX_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);
  }

  CheckStyle nStyle = CheckStyle::kCircle;
  WideString csWCaption = pControl->GetNormalCaption();
  if (csWCaption.GetLength() > 0) {
    switch (csWCaption[0]) {
      case L'8':
        nStyle = CheckStyle::kCross;
        break;
      case L'u':
        nStyle = CheckStyle::kDiamond;
        break;
      case L'n':
        nStyle = CheckStyle::kSquare;
        break;
      case L'H':
        nStyle = CheckStyle::kStar;
        break;
      case L'4':
        nStyle = CheckStyle::kCheck;
        break;
      case L'l':
      default:
        nStyle = CheckStyle::kCircle;
    }
  }

  ByteString csAP_N_ON;
  CFX_FloatRect rcCenter = rcWindow.GetCenterSquare().GetDeflated(1.0f, 1.0f);
  if (nStyle == CheckStyle::kCircle) {
    if (nBorderStyle == BorderStyle::BEVELED) {
      crLeftTop = CFX_Color(CFX_Color::kGray, 1);
      crRightBottom = crBackground - 0.25f;
    } else if (nBorderStyle == BorderStyle::INSET) {
      crLeftTop = CFX_Color(CFX_Color::kGray, 0.5f);
      crRightBottom = CFX_Color(CFX_Color::kGray, 0.75f);
    }

    csAP_N_ON =
        GetCircleFillAppStream(rcCenter, crBackground) +
        GetCircleBorderAppStream(rcCenter, fBorderWidth, crBorder, crLeftTop,
                                 crRightBottom, nBorderStyle, dsBorder);
  } else {
    csAP_N_ON =
        GetRectFillAppStream(rcWindow, crBackground) +
        GetBorderAppStreamInternal(rcWindow, fBorderWidth, crBorder, crLeftTop,
                                   crRightBottom, nBorderStyle, dsBorder);
  }

  ByteString csAP_N_OFF = csAP_N_ON;

  switch (nBorderStyle) {
    case BorderStyle::BEVELED: {
      CFX_Color crTemp = crLeftTop;
      crLeftTop = crRightBottom;
      crRightBottom = crTemp;
      break;
    }
    case BorderStyle::INSET: {
      crLeftTop = CFX_Color(CFX_Color::kGray, 0);
      crRightBottom = CFX_Color(CFX_Color::kGray, 1);
      break;
    }
    default:
      break;
  }

  ByteString csAP_D_ON;

  if (nStyle == CheckStyle::kCircle) {
    CFX_Color crBK = crBackground - 0.25f;
    if (nBorderStyle == BorderStyle::BEVELED) {
      crLeftTop = crBackground - 0.25f;
      crRightBottom = CFX_Color(CFX_Color::kGray, 1);
      crBK = crBackground;
    } else if (nBorderStyle == BorderStyle::INSET) {
      crLeftTop = CFX_Color(CFX_Color::kGray, 0);
      crRightBottom = CFX_Color(CFX_Color::kGray, 1);
    }

    csAP_D_ON =
        GetCircleFillAppStream(rcCenter, crBK) +
        GetCircleBorderAppStream(rcCenter, fBorderWidth, crBorder, crLeftTop,
                                 crRightBottom, nBorderStyle, dsBorder);
  } else {
    csAP_D_ON =
        GetRectFillAppStream(rcWindow, crBackground - 0.25f) +
        GetBorderAppStreamInternal(rcWindow, fBorderWidth, crBorder, crLeftTop,
                                   crRightBottom, nBorderStyle, dsBorder);
  }

  ByteString csAP_D_OFF = csAP_D_ON;

  csAP_N_ON += GetRadioButtonAppStream(rcClient, nStyle, crText);
  csAP_D_ON += GetRadioButtonAppStream(rcClient, nStyle, crText);

  Write("N", csAP_N_ON, pControl->GetCheckedAPState());
  Write("N", csAP_N_OFF, "Off");

  Write("D", csAP_D_ON, pControl->GetCheckedAPState());
  Write("D", csAP_D_OFF, "Off");

  ByteString csAS = widget_->GetAppState();
  if (csAS.IsEmpty())
    widget_->SetAppState("Off");
}

void CPWL_AppStream::SetAsComboBox(Optional<WideString> sValue) {
  CPDF_FormControl* pControl = widget_->GetFormControl();
  CPDF_FormField* pField = pControl->GetField();
  std::ostringstream sBody;

  CFX_FloatRect rcClient = widget_->GetClientRect();
  CFX_FloatRect rcButton = rcClient;
  rcButton.left = rcButton.right - 13;
  rcButton.Normalize();

  // Font map must outlive |pEdit|.
  CBA_FontMap font_map(
      widget_.Get(),
      widget_->GetInteractiveForm()->GetFormFillEnv()->GetSysHandler());

  auto pEdit = pdfium::MakeUnique<CPWL_EditImpl>();
  pEdit->EnableRefresh(false);
  pEdit->SetFontMap(&font_map);

  CFX_FloatRect rcEdit = rcClient;
  rcEdit.right = rcButton.left;
  rcEdit.Normalize();

  pEdit->SetPlateRect(rcEdit);
  pEdit->SetAlignmentV(1, true);

  float fFontSize = widget_->GetFontSize();
  if (IsFloatZero(fFontSize))
    pEdit->SetAutoFontSize(true, true);
  else
    pEdit->SetFontSize(fFontSize);

  pEdit->Initialize();

  if (sValue.has_value()) {
    pEdit->SetText(sValue.value());
  } else {
    int32_t nCurSel = pField->GetSelectedIndex(0);
    if (nCurSel < 0)
      pEdit->SetText(pField->GetValue());
    else
      pEdit->SetText(pField->GetOptionLabel(nCurSel));
  }

  CFX_FloatRect rcContent = pEdit->GetContentRect();
  ByteString sEdit = GetEditAppStream(pEdit.get(), CFX_PointF(), true, 0);
  if (sEdit.GetLength() > 0) {
    sBody << "/Tx ";
    AutoClosedCommand bmc(&sBody, kMarkedSequenceBeginOperator,
                          kMarkedSequenceEndOperator);
    AutoClosedQCommand q(&sBody);

    if (rcContent.Width() > rcEdit.Width() ||
        rcContent.Height() > rcEdit.Height()) {
      sBody << rcEdit.left << " " << rcEdit.bottom << " " << rcEdit.Width()
            << " " << rcEdit.Height() << " " << kAppendRectOperator << "\n"
            << kSetNonZeroWindingClipOperator << "\n"
            << kEndPathNoFillOrStrokeOperator << "\n";
    }

    CFX_Color crText = widget_->GetTextPWLColor();
    AutoClosedCommand bt(&sBody, kTextBeginOperator, kTextEndOperator);
    sBody << GetColorAppStream(crText, true) << sEdit;
  }

  sBody << GetDropButtonAppStream(rcButton);
  Write("N",
        GetBackgroundAppStream() + GetBorderAppStream() + ByteString(sBody),
        ByteString());
}

void CPWL_AppStream::SetAsListBox() {
  CPDF_FormControl* pControl = widget_->GetFormControl();
  CPDF_FormField* pField = pControl->GetField();
  CFX_FloatRect rcClient = widget_->GetClientRect();
  std::ostringstream sBody;

  // Font map must outlive |pEdit|.
  CBA_FontMap font_map(
      widget_.Get(),
      widget_->GetInteractiveForm()->GetFormFillEnv()->GetSysHandler());

  auto pEdit = pdfium::MakeUnique<CPWL_EditImpl>();
  pEdit->EnableRefresh(false);
  pEdit->SetFontMap(&font_map);
  pEdit->SetPlateRect(CFX_FloatRect(rcClient.left, 0.0f, rcClient.right, 0.0f));

  float fFontSize = widget_->GetFontSize();
  pEdit->SetFontSize(IsFloatZero(fFontSize) ? 12.0f : fFontSize);
  pEdit->Initialize();

  std::ostringstream sList;
  float fy = rcClient.top;

  int32_t nTop = pField->GetTopVisibleIndex();
  int32_t nCount = pField->CountOptions();
  int32_t nSelCount = pField->CountSelectedItems();

  for (int32_t i = nTop; i < nCount; ++i) {
    bool bSelected = false;
    for (int32_t j = 0; j < nSelCount; ++j) {
      if (pField->GetSelectedIndex(j) == i) {
        bSelected = true;
        break;
      }
    }

    pEdit->SetText(pField->GetOptionLabel(i));

    CFX_FloatRect rcContent = pEdit->GetContentRect();
    float fItemHeight = rcContent.Height();

    if (bSelected) {
      CFX_FloatRect rcItem =
          CFX_FloatRect(rcClient.left, fy - fItemHeight, rcClient.right, fy);
      {
        AutoClosedQCommand q(&sList);
        sList << GetColorAppStream(CFX_Color(CFX_Color::kRGB, 0, 51.0f / 255.0f,
                                             113.0f / 255.0f),
                                   true)
              << rcItem.left << " " << rcItem.bottom << " " << rcItem.Width()
              << " " << rcItem.Height() << " " << kAppendRectOperator << " "
              << kFillOperator << "\n";
      }

      AutoClosedCommand bt(&sList, kTextBeginOperator, kTextEndOperator);
      sList << GetColorAppStream(CFX_Color(CFX_Color::kGray, 1), true)
            << GetEditAppStream(pEdit.get(), CFX_PointF(0.0f, fy), true, 0);
    } else {
      CFX_Color crText = widget_->GetTextPWLColor();

      AutoClosedCommand bt(&sList, kTextBeginOperator, kTextEndOperator);
      sList << GetColorAppStream(crText, true)
            << GetEditAppStream(pEdit.get(), CFX_PointF(0.0f, fy), true, 0);
    }

    fy -= fItemHeight;
  }

  if (sList.tellp() > 0) {
    sBody << "/Tx ";
    AutoClosedCommand bmc(&sBody, kMarkedSequenceBeginOperator,
                          kMarkedSequenceEndOperator);
    AutoClosedQCommand q(&sBody);

    sBody << rcClient.left << " " << rcClient.bottom << " " << rcClient.Width()
          << " " << rcClient.Height() << " " << kAppendRectOperator << "\n"
          << kSetNonZeroWindingClipOperator << "\n"
          << kEndPathNoFillOrStrokeOperator << "\n"
          << sList.str();
  }
  Write("N",
        GetBackgroundAppStream() + GetBorderAppStream() + ByteString(sBody),
        ByteString());
}

void CPWL_AppStream::SetAsTextField(Optional<WideString> sValue) {
  CPDF_FormControl* pControl = widget_->GetFormControl();
  CPDF_FormField* pField = pControl->GetField();
  std::ostringstream sBody;
  std::ostringstream sLines;

  // Font map must outlive |pEdit|.
  CBA_FontMap font_map(
      widget_.Get(),
      widget_->GetInteractiveForm()->GetFormFillEnv()->GetSysHandler());

  auto pEdit = pdfium::MakeUnique<CPWL_EditImpl>();
  pEdit->EnableRefresh(false);
  pEdit->SetFontMap(&font_map);

  CFX_FloatRect rcClient = widget_->GetClientRect();
  pEdit->SetPlateRect(rcClient);
  pEdit->SetAlignmentH(pControl->GetControlAlignment(), true);

  uint32_t dwFieldFlags = pField->GetFieldFlags();
  bool bMultiLine = (dwFieldFlags >> 12) & 1;
  if (bMultiLine) {
    pEdit->SetMultiLine(true, true);
    pEdit->SetAutoReturn(true, true);
  } else {
    pEdit->SetAlignmentV(1, true);
  }

  uint16_t subWord = 0;
  if ((dwFieldFlags >> 13) & 1) {
    subWord = '*';
    pEdit->SetPasswordChar(subWord, true);
  }

  int nMaxLen = pField->GetMaxLen();
  bool bCharArray = (dwFieldFlags >> 24) & 1;
  float fFontSize = widget_->GetFontSize();

#ifdef PDF_ENABLE_XFA
  if (!sValue.has_value() && widget_->GetMixXFAWidget())
    sValue = widget_->GetValue();
#endif  // PDF_ENABLE_XFA

  if (nMaxLen > 0) {
    if (bCharArray) {
      pEdit->SetCharArray(nMaxLen);

      if (IsFloatZero(fFontSize)) {
        fFontSize = CPWL_Edit::GetCharArrayAutoFontSize(font_map.GetPDFFont(0),
                                                        rcClient, nMaxLen);
      }
    } else {
      if (sValue.has_value())
        nMaxLen = sValue.value().GetLength();
      pEdit->SetLimitChar(nMaxLen);
    }
  }

  if (IsFloatZero(fFontSize))
    pEdit->SetAutoFontSize(true, true);
  else
    pEdit->SetFontSize(fFontSize);

  pEdit->Initialize();
  pEdit->SetText(sValue.value_or(pField->GetValue()));

  CFX_FloatRect rcContent = pEdit->GetContentRect();
  ByteString sEdit =
      GetEditAppStream(pEdit.get(), CFX_PointF(), !bCharArray, subWord);

  if (sEdit.GetLength() > 0) {
    sBody << "/Tx ";
    AutoClosedCommand bmc(&sBody, kMarkedSequenceBeginOperator,
                          kMarkedSequenceEndOperator);
    AutoClosedQCommand q(&sBody);

    if (rcContent.Width() > rcClient.Width() ||
        rcContent.Height() > rcClient.Height()) {
      sBody << rcClient.left << " " << rcClient.bottom << " "
            << rcClient.Width() << " " << rcClient.Height() << " "
            << kAppendRectOperator << "\n"
            << kSetNonZeroWindingClipOperator << "\n"
            << kEndPathNoFillOrStrokeOperator << "\n";
    }
    CFX_Color crText = widget_->GetTextPWLColor();

    AutoClosedCommand bt(&sBody, kTextBeginOperator, kTextEndOperator);
    sBody << GetColorAppStream(crText, true) << sEdit;
  }

  if (bCharArray) {
    switch (widget_->GetBorderStyle()) {
      case BorderStyle::SOLID: {
        ByteString sColor =
            GetColorAppStream(widget_->GetBorderPWLColor(), false);
        if (sColor.GetLength() > 0) {
          AutoClosedQCommand q(&sLines);
          sLines << widget_->GetBorderWidth() << " " << kSetLineWidthOperator
                 << "\n"
                 << GetColorAppStream(widget_->GetBorderPWLColor(), false)
                 << " 2 " << kSetLineCapStyleOperator << " 0 "
                 << kSetLineJoinStyleOperator << "\n";

          for (int32_t i = 1; i < nMaxLen; ++i) {
            sLines << rcClient.left +
                          ((rcClient.right - rcClient.left) / nMaxLen) * i
                   << " " << rcClient.bottom << " " << kMoveToOperator << "\n"
                   << rcClient.left +
                          ((rcClient.right - rcClient.left) / nMaxLen) * i
                   << " " << rcClient.top << " " << kLineToOperator << " "
                   << kStrokeOperator << "\n";
          }
        }
        break;
      }
      case BorderStyle::DASH: {
        ByteString sColor =
            GetColorAppStream(widget_->GetBorderPWLColor(), false);
        if (sColor.GetLength() > 0) {
          CPWL_Dash dsBorder = CPWL_Dash(3, 3, 0);
          AutoClosedQCommand q(&sLines);
          sLines << widget_->GetBorderWidth() << " " << kSetLineWidthOperator
                 << "\n"
                 << GetColorAppStream(widget_->GetBorderPWLColor(), false)
                 << "[" << dsBorder.nDash << " " << dsBorder.nGap << "] "
                 << dsBorder.nPhase << " " << kSetDashOperator << "\n";

          for (int32_t i = 1; i < nMaxLen; ++i) {
            sLines << rcClient.left +
                          ((rcClient.right - rcClient.left) / nMaxLen) * i
                   << " " << rcClient.bottom << " " << kMoveToOperator << "\n"
                   << rcClient.left +
                          ((rcClient.right - rcClient.left) / nMaxLen) * i
                   << " " << rcClient.top << " " << kLineToOperator << " "
                   << kStrokeOperator << "\n";
          }
        }
        break;
      }
      default:
        break;
    }
  }

  Write("N",
        GetBackgroundAppStream() + GetBorderAppStream() + ByteString(sLines) +
            ByteString(sBody),
        ByteString());
}

void CPWL_AppStream::AddImage(const ByteString& sAPType, CPDF_Stream* pImage) {
  CPDF_Stream* pStream = dict_->GetStreamFor(sAPType);
  CPDF_Dictionary* pStreamDict = pStream->GetDict();
  ByteString sImageAlias = "IMG";

  if (CPDF_Dictionary* pImageDict = pImage->GetDict()) {
    sImageAlias = pImageDict->GetStringFor("Name");
    if (sImageAlias.IsEmpty())
      sImageAlias = "IMG";
  }

  CPDF_Dictionary* pStreamResList = pStreamDict->GetDictFor("Resources");
  if (!pStreamResList)
    pStreamResList = pStreamDict->SetNewFor<CPDF_Dictionary>("Resources");

  CPDF_Dictionary* pXObject =
      pStreamResList->SetNewFor<CPDF_Dictionary>("XObject");
  pXObject->SetFor(sImageAlias, pImage->MakeReference(
                                    widget_->GetPageView()->GetPDFDocument()));
}

void CPWL_AppStream::Write(const ByteString& sAPType,
                           const ByteString& sContents,
                           const ByteString& sAPState) {
  CPDF_Stream* pStream = nullptr;
  CPDF_Dictionary* pParentDict = nullptr;
  if (sAPState.IsEmpty()) {
    pParentDict = dict_.Get();
    pStream = dict_->GetStreamFor(sAPType);
  } else {
    CPDF_Dictionary* pAPTypeDict = dict_->GetDictFor(sAPType);
    if (!pAPTypeDict)
      pAPTypeDict = dict_->SetNewFor<CPDF_Dictionary>(sAPType);

    pParentDict = pAPTypeDict;
    pStream = pAPTypeDict->GetStreamFor(sAPState);
  }

  if (!pStream) {
    CPDF_Document* doc = widget_->GetPageView()->GetPDFDocument();
    pStream = doc->NewIndirect<CPDF_Stream>();
    pParentDict->SetFor(sAPType, pStream->MakeReference(doc));
  }

  CPDF_Dictionary* pStreamDict = pStream->GetDict();
  if (!pStreamDict) {
    auto pNewDict =
        widget_->GetPDFAnnot()->GetDocument()->New<CPDF_Dictionary>();
    pStreamDict = pNewDict.get();
    pStreamDict->SetNewFor<CPDF_Name>("Type", "XObject");
    pStreamDict->SetNewFor<CPDF_Name>("Subtype", "Form");
    pStreamDict->SetNewFor<CPDF_Number>("FormType", 1);
    pStream->InitStream({}, std::move(pNewDict));
  }
  pStreamDict->SetMatrixFor("Matrix", widget_->GetMatrix());
  pStreamDict->SetRectFor("BBox", widget_->GetRotatedRect());
  pStream->SetDataAndRemoveFilter(sContents.AsRawSpan());
}

void CPWL_AppStream::Remove(const ByteString& sAPType) {
  dict_->RemoveFor(sAPType);
}

ByteString CPWL_AppStream::GetBackgroundAppStream() const {
  CFX_Color crBackground = widget_->GetFillPWLColor();
  if (crBackground.nColorType != CFX_Color::kTransparent)
    return GetRectFillAppStream(widget_->GetRotatedRect(), crBackground);

  return ByteString();
}

ByteString CPWL_AppStream::GetBorderAppStream() const {
  CFX_FloatRect rcWindow = widget_->GetRotatedRect();
  CFX_Color crBorder = widget_->GetBorderPWLColor();
  CFX_Color crBackground = widget_->GetFillPWLColor();
  CFX_Color crLeftTop;
  CFX_Color crRightBottom;

  float fBorderWidth = static_cast<float>(widget_->GetBorderWidth());
  CPWL_Dash dsBorder(3, 0, 0);

  BorderStyle nBorderStyle = widget_->GetBorderStyle();
  switch (nBorderStyle) {
    case BorderStyle::DASH:
      dsBorder = CPWL_Dash(3, 3, 0);
      break;
    case BorderStyle::BEVELED:
      fBorderWidth *= 2;
      crLeftTop = CFX_Color(CFX_Color::kGray, 1);
      crRightBottom = crBackground / 2.0f;
      break;
    case BorderStyle::INSET:
      fBorderWidth *= 2;
      crLeftTop = CFX_Color(CFX_Color::kGray, 0.5);
      crRightBottom = CFX_Color(CFX_Color::kGray, 0.75);
      break;
    default:
      break;
  }

  return GetBorderAppStreamInternal(rcWindow, fBorderWidth, crBorder, crLeftTop,
                                    crRightBottom, nBorderStyle, dsBorder);
}

// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2009 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fxbarcode/oned/BC_OnedEAN13Writer.h"

#include <algorithm>
#include <cwctype>
#include <memory>
#include <vector>

#include "core/fxcrt/fx_extension.h"
#include "core/fxge/cfx_defaultrenderdevice.h"
#include "fxbarcode/BC_Writer.h"
#include "fxbarcode/oned/BC_OneDimWriter.h"
#include "fxbarcode/oned/BC_OnedEANChecksum.h"

namespace {

const int8_t kFirstDigitEncodings[10] = {0x00, 0x0B, 0x0D, 0xE,  0x13,
                                         0x19, 0x1C, 0x15, 0x16, 0x1A};
const int8_t kOnedEAN13StartPattern[3] = {1, 1, 1};
const int8_t kOnedEAN13MiddlePattern[5] = {1, 1, 1, 1, 1};
const int8_t kOnedEAN13LPattern[10][4] = {
    {3, 2, 1, 1}, {2, 2, 2, 1}, {2, 1, 2, 2}, {1, 4, 1, 1}, {1, 1, 3, 2},
    {1, 2, 3, 1}, {1, 1, 1, 4}, {1, 3, 1, 2}, {1, 2, 1, 3}, {3, 1, 1, 2}};
const int8_t L_AND_G_PATTERNS[20][4] = {
    {3, 2, 1, 1}, {2, 2, 2, 1}, {2, 1, 2, 2}, {1, 4, 1, 1}, {1, 1, 3, 2},
    {1, 2, 3, 1}, {1, 1, 1, 4}, {1, 3, 1, 2}, {1, 2, 1, 3}, {3, 1, 1, 2},
    {1, 1, 2, 3}, {1, 2, 2, 2}, {2, 2, 1, 2}, {1, 1, 4, 1}, {2, 3, 1, 1},
    {1, 3, 2, 1}, {4, 1, 1, 1}, {2, 1, 3, 1}, {3, 1, 2, 1}, {2, 1, 1, 3}};

}  // namespace

CBC_OnedEAN13Writer::CBC_OnedEAN13Writer() {
  m_bLeftPadding = true;
  m_codeWidth = 3 + (7 * 6) + 5 + (7 * 6) + 3;
}
CBC_OnedEAN13Writer::~CBC_OnedEAN13Writer() {}

bool CBC_OnedEAN13Writer::CheckContentValidity(WideStringView contents) {
  return std::all_of(contents.begin(), contents.end(),
                     [](wchar_t c) { return FXSYS_IsDecimalDigit(c); });
}

WideString CBC_OnedEAN13Writer::FilterContents(WideStringView contents) {
  WideString filtercontents;
  filtercontents.Reserve(contents.GetLength());
  wchar_t ch;
  for (size_t i = 0; i < contents.GetLength(); i++) {
    ch = contents[i];
    if (ch > 175) {
      i++;
      continue;
    }
    if (FXSYS_IsDecimalDigit(ch))
      filtercontents += ch;
  }
  return filtercontents;
}

int32_t CBC_OnedEAN13Writer::CalcChecksum(const ByteString& contents) {
  return EANCalcChecksum(contents);
}

uint8_t* CBC_OnedEAN13Writer::EncodeWithHint(const ByteString& contents,
                                             BCFORMAT format,
                                             int32_t& outWidth,
                                             int32_t& outHeight,
                                             int32_t hints) {
  if (format != BCFORMAT_EAN_13)
    return nullptr;
  return CBC_OneDimWriter::EncodeWithHint(contents, format, outWidth, outHeight,
                                          hints);
}

uint8_t* CBC_OnedEAN13Writer::EncodeImpl(const ByteString& contents,
                                         int32_t& outLength) {
  if (contents.GetLength() != 13)
    return nullptr;

  m_iDataLenth = 13;
  int32_t firstDigit = FXSYS_DecimalCharToInt(contents.First());
  int32_t parities = kFirstDigitEncodings[firstDigit];
  outLength = m_codeWidth;
  std::unique_ptr<uint8_t, FxFreeDeleter> result(
      FX_Alloc(uint8_t, m_codeWidth));
  int32_t pos = 0;
  pos += AppendPattern(result.get(), pos, kOnedEAN13StartPattern, 3, true);

  int32_t i = 0;
  for (i = 1; i <= 6; i++) {
    int32_t digit = FXSYS_DecimalCharToInt(contents[i]);
    if ((parities >> (6 - i) & 1) == 1) {
      digit += 10;
    }
    pos += AppendPattern(result.get(), pos, L_AND_G_PATTERNS[digit], 4, false);
  }
  pos += AppendPattern(result.get(), pos, kOnedEAN13MiddlePattern, 5, false);

  for (i = 7; i <= 12; i++) {
    int32_t digit = FXSYS_DecimalCharToInt(contents[i]);
    pos += AppendPattern(result.get(), pos, kOnedEAN13LPattern[digit], 4, true);
  }
  pos += AppendPattern(result.get(), pos, kOnedEAN13StartPattern, 3, true);
  return result.release();
}

bool CBC_OnedEAN13Writer::ShowChars(WideStringView contents,
                                    CFX_RenderDevice* device,
                                    const CFX_Matrix* matrix,
                                    int32_t barWidth,
                                    int32_t multiple) {
  if (!device)
    return false;

  int32_t leftPadding = 7 * multiple;
  int32_t leftPosition = 3 * multiple + leftPadding;
  ByteString str = FX_UTF8Encode(contents);
  size_t length = str.GetLength();
  std::vector<FXTEXT_CHARPOS> charpos(length);
  int32_t iFontSize = (int32_t)fabs(m_fFontSize);
  int32_t iTextHeight = iFontSize + 1;
  ByteString tempStr = str.Mid(1, 6);
  int32_t strWidth = multiple * 42;

  CFX_Matrix matr(m_outputHScale, 0.0, 0.0, 1.0, 0.0, 0.0);
  CFX_FloatRect rect((float)leftPosition, (float)(m_Height - iTextHeight),
                     (float)(leftPosition + strWidth - 0.5), (float)m_Height);
  matr.Concat(*matrix);
  FX_RECT re = matr.TransformRect(rect).GetOuterRect();
  device->FillRect(re, m_backgroundColor);
  CFX_FloatRect rect1(
      (float)(leftPosition + 47 * multiple), (float)(m_Height - iTextHeight),
      (float)(leftPosition + 47 * multiple + strWidth - 0.5), (float)m_Height);
  CFX_Matrix matr1(m_outputHScale, 0.0, 0.0, 1.0, 0.0, 0.0);
  matr1.Concat(*matrix);
  re = matr1.TransformRect(rect1).GetOuterRect();
  device->FillRect(re, m_backgroundColor);
  int32_t strWidth1 = multiple * 7;
  CFX_Matrix matr2(m_outputHScale, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
  CFX_FloatRect rect2(0.0f, (float)(m_Height - iTextHeight),
                      (float)strWidth1 - 0.5f, (float)m_Height);
  matr2.Concat(*matrix);
  re = matr2.TransformRect(rect2).GetOuterRect();
  device->FillRect(re, m_backgroundColor);

  float blank = 0.0;
  length = tempStr.GetLength();
  strWidth = (int32_t)(strWidth * m_outputHScale);

  CalcTextInfo(tempStr, &charpos[1], m_pFont.Get(), (float)strWidth, iFontSize,
               blank);
  {
    CFX_Matrix affine_matrix1(1.0, 0.0, 0.0, -1.0,
                              (float)leftPosition * m_outputHScale,
                              (float)(m_Height - iTextHeight) + iFontSize);
    if (matrix)
      affine_matrix1.Concat(*matrix);
    device->DrawNormalText(length, &charpos[1], m_pFont.Get(),
                           static_cast<float>(iFontSize), &affine_matrix1,
                           m_fontColor, FXTEXT_CLEARTYPE);
  }
  tempStr = str.Mid(7, 6);
  length = tempStr.GetLength();
  CalcTextInfo(tempStr, &charpos[7], m_pFont.Get(), (float)strWidth, iFontSize,
               blank);
  {
    CFX_Matrix affine_matrix1(
        1.0, 0.0, 0.0, -1.0,
        (float)(leftPosition + 47 * multiple) * m_outputHScale,
        (float)(m_Height - iTextHeight + iFontSize));
    if (matrix)
      affine_matrix1.Concat(*matrix);
    device->DrawNormalText(length, &charpos[7], m_pFont.Get(),
                           static_cast<float>(iFontSize), &affine_matrix1,
                           m_fontColor, FXTEXT_CLEARTYPE);
  }
  tempStr = str.Left(1);
  length = tempStr.GetLength();
  strWidth = multiple * 7;
  strWidth = (int32_t)(strWidth * m_outputHScale);

  CalcTextInfo(tempStr, charpos.data(), m_pFont.Get(), (float)strWidth,
               iFontSize, blank);
  {
    CFX_Matrix affine_matrix1(1.0, 0.0, 0.0, -1.0, 0.0,
                              (float)(m_Height - iTextHeight + iFontSize));
    if (matrix)
      affine_matrix1.Concat(*matrix);
    device->DrawNormalText(length, charpos.data(), m_pFont.Get(),
                           static_cast<float>(iFontSize), &affine_matrix1,
                           m_fontColor, FXTEXT_CLEARTYPE);
  }
  return true;
}

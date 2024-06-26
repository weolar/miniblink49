// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2008 ZXing authors
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

#include "fxbarcode/qrcode/BC_QRCoderMatrixUtil.h"

#include "core/fxcrt/fx_memory.h"
#include "fxbarcode/common/BC_CommonByteMatrix.h"
#include "fxbarcode/qrcode/BC_QRCoder.h"
#include "fxbarcode/qrcode/BC_QRCoderBitVector.h"
#include "fxbarcode/qrcode/BC_QRCoderErrorCorrectionLevel.h"
#include "fxbarcode/qrcode/BC_QRCoderMaskUtil.h"

namespace {

constexpr uint8_t POSITION_DETECTION_PATTERN[7][7] = {
    {1, 1, 1, 1, 1, 1, 1}, {1, 0, 0, 0, 0, 0, 1}, {1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1}, {1, 0, 1, 1, 1, 0, 1}, {1, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1}};

constexpr uint8_t POSITION_ADJUSTMENT_PATTERN[5][5] = {{1, 1, 1, 1, 1},
                                                       {1, 0, 0, 0, 1},
                                                       {1, 0, 1, 0, 1},
                                                       {1, 0, 0, 0, 1},
                                                       {1, 1, 1, 1, 1}};

constexpr size_t kNumCoordinate = 7;
constexpr uint8_t kPositionAdjustmentPatternCoordinates[39][kNumCoordinate] = {
    {6, 18, 0, 0, 0, 0, 0},         {6, 22, 0, 0, 0, 0, 0},
    {6, 26, 0, 0, 0, 0, 0},         {6, 30, 0, 0, 0, 0, 0},
    {6, 34, 0, 0, 0, 0, 0},         {6, 22, 38, 0, 0, 0, 0},
    {6, 24, 42, 0, 0, 0, 0},        {6, 26, 46, 0, 0, 0, 0},
    {6, 28, 50, 0, 0, 0, 0},        {6, 30, 54, 0, 0, 0, 0},
    {6, 32, 58, 0, 0, 0, 0},        {6, 34, 62, 0, 0, 0, 0},
    {6, 26, 46, 66, 0, 0, 0},       {6, 26, 48, 70, 0, 0, 0},
    {6, 26, 50, 74, 0, 0, 0},       {6, 30, 54, 78, 0, 0, 0},
    {6, 30, 56, 82, 0, 0, 0},       {6, 30, 58, 86, 0, 0, 0},
    {6, 34, 62, 90, 0, 0, 0},       {6, 28, 50, 72, 94, 0, 0},
    {6, 26, 50, 74, 98, 0, 0},      {6, 30, 54, 78, 102, 0, 0},
    {6, 28, 54, 80, 106, 0, 0},     {6, 32, 58, 84, 110, 0, 0},
    {6, 30, 58, 86, 114, 0, 0},     {6, 34, 62, 90, 118, 0, 0},
    {6, 26, 50, 74, 98, 122, 0},    {6, 30, 54, 78, 102, 126, 0},
    {6, 26, 52, 78, 104, 130, 0},   {6, 30, 56, 82, 108, 134, 0},
    {6, 34, 60, 86, 112, 138, 0},   {6, 30, 58, 86, 114, 142, 0},
    {6, 34, 62, 90, 118, 146, 0},   {6, 30, 54, 78, 102, 126, 150},
    {6, 24, 50, 76, 102, 128, 154}, {6, 28, 54, 80, 106, 132, 158},
    {6, 32, 58, 84, 110, 136, 162}, {6, 26, 54, 82, 110, 138, 166},
    {6, 30, 58, 86, 114, 142, 170},
};

const uint8_t TYPE_INFO_COORDINATES[15][2] = {
    {8, 0}, {8, 1}, {8, 2}, {8, 3}, {8, 4}, {8, 5}, {8, 7}, {8, 8},
    {7, 8}, {5, 8}, {4, 8}, {3, 8}, {2, 8}, {1, 8}, {0, 8},
};

const int32_t VERSION_INFO_POLY = 0x1f25;
const int32_t TYPE_INFO_POLY = 0x0537;
const int32_t TYPE_INFO_MASK_PATTERN = 0x5412;

bool IsEmpty(int32_t value) {
  return (uint8_t)value == 0xff;
}

bool IsValidValue(int32_t value) {
  return ((uint8_t)value == 0xff || (uint8_t)value == 0x00 ||
          (uint8_t)value == 0x01);
}

int32_t FindMSBSet(int32_t value) {
  int32_t numDigits = 0;
  while (value != 0) {
    value >>= 1;
    ++numDigits;
  }
  return numDigits;
}

bool EmbedDataBits(CBC_QRCoderBitVector* dataBits,
                   int32_t maskPattern,
                   CBC_CommonByteMatrix* matrix) {
  size_t szBitIndex = 0;
  int32_t direction = -1;
  int32_t x = matrix->GetWidth() - 1;
  int32_t y = matrix->GetHeight() - 1;
  while (x > 0) {
    if (x == 6)
      x -= 1;

    while (y >= 0 && y < matrix->GetHeight()) {
      if (y == 6) {
        y += direction;
        continue;
      }
      for (int32_t i = 0; i < 2; i++) {
        int32_t xx = x - i;
        if (!IsEmpty(matrix->Get(xx, y))) {
          continue;
        }
        int32_t bit;
        if (szBitIndex < dataBits->Size()) {
          bit = dataBits->At(szBitIndex);
          szBitIndex++;
        } else {
          bit = 0;
        }
        ASSERT(CBC_QRCoder::IsValidMaskPattern(maskPattern));
        if (CBC_QRCoderMaskUtil::GetDataMaskBit(maskPattern, xx, y))
          bit ^= 0x01;
        matrix->Set(xx, y, bit);
      }
      y += direction;
    }
    direction = -direction;
    y += direction;
    x -= 2;
  }
  return szBitIndex == dataBits->Size();
}

int32_t CalculateBCHCode(int32_t value, int32_t poly) {
  int32_t msbSetInPoly = FindMSBSet(poly);
  value <<= msbSetInPoly - 1;
  while (FindMSBSet(value) >= msbSetInPoly) {
    value ^= poly << (FindMSBSet(value) - msbSetInPoly);
  }
  return value;
}

bool MakeTypeInfoBits(const CBC_QRCoderErrorCorrectionLevel* ecLevel,
                      int32_t maskPattern,
                      CBC_QRCoderBitVector* bits) {
  if (!CBC_QRCoder::IsValidMaskPattern(maskPattern))
    return false;

  int32_t typeInfo = (ecLevel->GetBits() << 3) | maskPattern;
  bits->AppendBits(typeInfo, 5);
  int32_t bchCode = CalculateBCHCode(typeInfo, TYPE_INFO_POLY);
  bits->AppendBits(bchCode, 10);
  CBC_QRCoderBitVector maskBits;
  maskBits.AppendBits(TYPE_INFO_MASK_PATTERN, 15);
  if (!bits->XOR(&maskBits))
    return false;

  ASSERT(bits->Size() == 15);
  return true;
}

void MakeVersionInfoBits(int32_t version, CBC_QRCoderBitVector* bits) {
  bits->AppendBits(version, 6);
  int32_t bchCode = CalculateBCHCode(version, VERSION_INFO_POLY);
  bits->AppendBits(bchCode, 12);
  ASSERT(bits->Size() == 18);
}

bool EmbedTypeInfo(const CBC_QRCoderErrorCorrectionLevel* ecLevel,
                   int32_t maskPattern,
                   CBC_CommonByteMatrix* matrix) {
  CBC_QRCoderBitVector typeInfoBits;
  if (!MakeTypeInfoBits(ecLevel, maskPattern, &typeInfoBits))
    return false;

  for (size_t i = 0; i < typeInfoBits.Size(); i++) {
    int32_t bit = typeInfoBits.At(typeInfoBits.Size() - 1 - i);
    int32_t x1 = TYPE_INFO_COORDINATES[i][0];
    int32_t y1 = TYPE_INFO_COORDINATES[i][1];
    matrix->Set(x1, y1, bit);
    if (i < 8) {
      int32_t x2 = matrix->GetWidth() - i - 1;
      int32_t y2 = 8;
      matrix->Set(x2, y2, bit);
    } else {
      int32_t x2 = 8;
      int32_t y2 = matrix->GetHeight() - 7 + (i - 8);
      matrix->Set(x2, y2, bit);
    }
  }
  return true;
}

void MaybeEmbedVersionInfo(int32_t version, CBC_CommonByteMatrix* matrix) {
  if (version < 7)
    return;

  CBC_QRCoderBitVector versionInfoBits;
  MakeVersionInfoBits(version, &versionInfoBits);
  int32_t bitIndex = 6 * 3 - 1;
  for (int32_t i = 0; i < 6; i++) {
    for (int32_t j = 0; j < 3; j++) {
      int32_t bit = versionInfoBits.At(bitIndex);
      bitIndex--;
      matrix->Set(i, matrix->GetHeight() - 11 + j, bit);
      matrix->Set(matrix->GetHeight() - 11 + j, i, bit);
    }
  }
}

bool EmbedTimingPatterns(CBC_CommonByteMatrix* matrix) {
  for (int32_t i = 8; i < matrix->GetWidth() - 8; i++) {
    int32_t bit = (i + 1) % 2;
    if (!IsValidValue(matrix->Get(i, 6)))
      return false;

    if (IsEmpty(matrix->Get(i, 6)))
      matrix->Set(i, 6, bit);

    if (!IsValidValue(matrix->Get(6, i)))
      return false;

    if (IsEmpty(matrix->Get(6, i)))
      matrix->Set(6, i, bit);
  }
  return true;
}

bool EmbedDarkDotAtLeftBottomCorner(CBC_CommonByteMatrix* matrix) {
  if (matrix->Get(8, matrix->GetHeight() - 8) == 0)
    return false;

  matrix->Set(8, matrix->GetHeight() - 8, 1);
  return true;
}

bool EmbedHorizontalSeparationPattern(int32_t xStart,
                                      int32_t yStart,
                                      CBC_CommonByteMatrix* matrix) {
  for (int32_t x = 0; x < 8; x++) {
    if (!IsEmpty(matrix->Get(xStart + x, yStart)))
      return false;

    matrix->Set(xStart + x, yStart, 0);
  }
  return true;
}

bool EmbedVerticalSeparationPattern(int32_t xStart,
                                    int32_t yStart,
                                    CBC_CommonByteMatrix* matrix) {
  for (int32_t y = 0; y < 7; y++) {
    if (!IsEmpty(matrix->Get(xStart, yStart + y)))
      return false;

    matrix->Set(xStart, yStart + y, 0);
  }
  return true;
}

bool EmbedPositionAdjustmentPattern(int32_t xStart,
                                    int32_t yStart,
                                    CBC_CommonByteMatrix* matrix) {
  for (int32_t y = 0; y < 5; y++) {
    for (int32_t x = 0; x < 5; x++) {
      if (!IsEmpty(matrix->Get(xStart + x, y + yStart)))
        return false;

      matrix->Set(xStart + x, yStart + y, POSITION_ADJUSTMENT_PATTERN[y][x]);
    }
  }
  return true;
}

bool EmbedPositionDetectionPattern(int32_t xStart,
                                   int32_t yStart,
                                   CBC_CommonByteMatrix* matrix) {
  for (int32_t y = 0; y < 7; y++) {
    for (int32_t x = 0; x < 7; x++) {
      if (!IsEmpty(matrix->Get(xStart + x, yStart + y)))
        return false;

      matrix->Set(xStart + x, yStart + y, POSITION_DETECTION_PATTERN[y][x]);
    }
  }
  return true;
}

bool EmbedPositionDetectionPatternsAndSeparators(CBC_CommonByteMatrix* matrix) {
  constexpr int32_t pdpWidth = 7;
  if (!EmbedPositionDetectionPattern(0, 0, matrix))
    return false;
  if (!EmbedPositionDetectionPattern(matrix->GetWidth() - pdpWidth, 0, matrix))
    return false;
  if (!EmbedPositionDetectionPattern(0, matrix->GetWidth() - pdpWidth, matrix))
    return false;

  constexpr int32_t hspWidth = 8;
  if (!EmbedHorizontalSeparationPattern(0, hspWidth - 1, matrix))
    return false;
  if (!EmbedHorizontalSeparationPattern(matrix->GetWidth() - hspWidth,
                                        hspWidth - 1, matrix)) {
    return false;
  }
  if (!EmbedHorizontalSeparationPattern(0, matrix->GetWidth() - hspWidth,
                                        matrix)) {
    return false;
  }

  constexpr int32_t vspSize = 7;
  if (!EmbedVerticalSeparationPattern(vspSize, 0, matrix))
    return false;
  if (!EmbedVerticalSeparationPattern(matrix->GetHeight() - vspSize - 1, 0,
                                      matrix)) {
    return false;
  }
  if (!EmbedVerticalSeparationPattern(vspSize, matrix->GetHeight() - vspSize,
                                      matrix)) {
    return false;
  }
  return true;
}

bool MaybeEmbedPositionAdjustmentPatterns(int32_t version,
                                          CBC_CommonByteMatrix* matrix) {
  if (version < 2)
    return true;

  const size_t index = version - 2;
  if (index >= FX_ArraySize(kPositionAdjustmentPatternCoordinates))
    return false;

  const auto* coordinates = &kPositionAdjustmentPatternCoordinates[index][0];
  for (size_t i = 0; i < kNumCoordinate; i++) {
    const int32_t y = coordinates[i];
    if (y == 0)
      break;
    for (size_t j = 0; j < kNumCoordinate; j++) {
      const int32_t x = coordinates[j];
      if (x == 0)
        break;

      if (IsEmpty(matrix->Get(x, y))) {
        if (!EmbedPositionAdjustmentPattern(x - 2, y - 2, matrix))
          return false;
      }
    }
  }
  return true;
}

bool EmbedBasicPatterns(int32_t version, CBC_CommonByteMatrix* matrix) {
  if (!EmbedPositionDetectionPatternsAndSeparators(matrix))
    return false;
  if (!EmbedDarkDotAtLeftBottomCorner(matrix))
    return false;
  if (!MaybeEmbedPositionAdjustmentPatterns(version, matrix))
    return false;
  if (!EmbedTimingPatterns(matrix))
    return false;
  return true;
}

}  // namespace

bool CBC_QRCoderMatrixUtil::BuildMatrix(
    CBC_QRCoderBitVector* dataBits,
    const CBC_QRCoderErrorCorrectionLevel* ecLevel,
    int32_t version,
    int32_t maskPattern,
    CBC_CommonByteMatrix* matrix) {
  if (!dataBits || !matrix)
    return false;

  matrix->clear(0xff);

  if (!EmbedBasicPatterns(version, matrix))
    return false;
  if (!EmbedTypeInfo(ecLevel, maskPattern, matrix))
    return false;

  MaybeEmbedVersionInfo(version, matrix);
  return EmbedDataBits(dataBits, maskPattern, matrix);
}

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

#include "fxbarcode/common/BC_CommonByteMatrix.h"
#include "fxbarcode/qrcode/BC_QRCoder.h"
#include "fxbarcode/qrcode/BC_QRCoderErrorCorrectionLevel.h"
#include "fxbarcode/qrcode/BC_QRCoderMaskUtil.h"

namespace {

int32_t ApplyMaskPenaltyRule1Internal(CBC_CommonByteMatrix* matrix,
                                      bool isHorizontal) {
  int32_t penalty = 0;
  int32_t numSameBitCells = 0;
  int32_t prevBit = -1;
  int32_t width = matrix->GetWidth();
  int32_t height = matrix->GetHeight();
  int32_t iLimit = isHorizontal ? height : width;
  int32_t jLimit = isHorizontal ? width : height;
  pdfium::span<const uint8_t> array = matrix->GetArray();
  for (int32_t i = 0; i < iLimit; ++i) {
    for (int32_t j = 0; j < jLimit; ++j) {
      int32_t bit = isHorizontal ? array[i * width + j] : array[j * width + i];
      if (bit == prevBit) {
        numSameBitCells += 1;
        if (numSameBitCells == 5) {
          penalty += 3;
        } else if (numSameBitCells > 5) {
          penalty += 1;
        }
      } else {
        numSameBitCells = 1;
        prevBit = bit;
      }
    }
    numSameBitCells = 0;
  }
  return penalty;
}

}  // namespace

// static
int32_t CBC_QRCoderMaskUtil::ApplyMaskPenaltyRule1(
    CBC_CommonByteMatrix* matrix) {
  return ApplyMaskPenaltyRule1Internal(matrix, true) +
         ApplyMaskPenaltyRule1Internal(matrix, false);
}

// static
int32_t CBC_QRCoderMaskUtil::ApplyMaskPenaltyRule2(
    CBC_CommonByteMatrix* matrix) {
  int32_t penalty = 0;
  pdfium::span<const uint8_t> array = matrix->GetArray();
  int32_t width = matrix->GetWidth();
  int32_t height = matrix->GetHeight();
  for (int32_t y = 0; y < height - 1; y++) {
    for (int32_t x = 0; x < width - 1; x++) {
      int32_t value = array[y * width + x];
      if (value == array[y * width + x + 1] &&
          value == array[(y + 1) * width + x] &&
          value == array[(y + 1) * width + x + 1]) {
        penalty++;
      }
    }
  }
  return 3 * penalty;
}

// static
int32_t CBC_QRCoderMaskUtil::ApplyMaskPenaltyRule3(
    CBC_CommonByteMatrix* matrix) {
  int32_t penalty = 0;
  pdfium::span<const uint8_t> array = matrix->GetArray();
  int32_t width = matrix->GetWidth();
  int32_t height = matrix->GetHeight();
  for (int32_t y = 0; y < height; ++y) {
    for (int32_t x = 0; x < width; ++x) {
      if (x == 0 &&
          ((y >= 0 && y <= 6) || (y >= height - 7 && y <= height - 1))) {
        continue;
      }
      if (x == width - 7 && (y >= 0 && y <= 6)) {
        continue;
      }
      if (y == 0 &&
          ((x >= 0 && x <= 6) || (x >= width - 7 && x <= width - 1))) {
        continue;
      }
      if (y == height - 7 && (x >= 0 && x <= 6)) {
        continue;
      }
      if (x + 6 < width && array[y * width + x] == 1 &&
          array[y * width + x + 1] == 0 && array[y * width + x + 2] == 1 &&
          array[y * width + x + 3] == 1 && array[y * width + x + 4] == 1 &&
          array[y * width + x + 5] == 0 && array[y * width + x + 6] == 1 &&
          ((x + 10 < width && array[y * width + x + 7] == 0 &&
            array[y * width + x + 8] == 0 && array[y * width + x + 9] == 0 &&
            array[y * width + x + 10] == 0) ||
           (x - 4 >= 0 && array[y * width + x - 1] == 0 &&
            array[y * width + x - 2] == 0 && array[y * width + x - 3] == 0 &&
            array[y * width + x - 4] == 0))) {
        penalty += 40;
      }
      if (y + 6 < height && array[y * width + x] == 1 &&
          array[(y + 1) * width + x] == 0 && array[(y + 2) * width + x] == 1 &&
          array[(y + 3) * width + x] == 1 && array[(y + 4) * width + x] == 1 &&
          array[(y + 5) * width + x] == 0 && array[(y + 6) * width + x] == 1 &&
          ((y + 10 < height && array[(y + 7) * width + x] == 0 &&
            array[(y + 8) * width + x] == 0 &&
            array[(y + 9) * width + x] == 0 &&
            array[(y + 10) * width + x] == 0) ||
           (y - 4 >= 0 && array[(y - 1) * width + x] == 0 &&
            array[(y - 2) * width + x] == 0 &&
            array[(y - 3) * width + x] == 0 &&
            array[(y - 4) * width + x] == 0))) {
        penalty += 40;
      }
    }
  }
  return penalty;
}

// static
int32_t CBC_QRCoderMaskUtil::ApplyMaskPenaltyRule4(
    CBC_CommonByteMatrix* matrix) {
  int32_t numDarkCells = 0;
  pdfium::span<const uint8_t> array = matrix->GetArray();
  int32_t width = matrix->GetWidth();
  int32_t height = matrix->GetHeight();
  for (int32_t y = 0; y < height; ++y) {
    for (int32_t x = 0; x < width; ++x) {
      if (array[y * width + x] == 1)
        numDarkCells += 1;
    }
  }
  int32_t numTotalCells = matrix->GetHeight() * matrix->GetWidth();
  double darkRatio = static_cast<double>(numDarkCells) / numTotalCells;
  return abs(static_cast<int32_t>(darkRatio * 100 - 50) / 5) * 5 * 10;
}

// static
bool CBC_QRCoderMaskUtil::GetDataMaskBit(int32_t maskPattern,
                                         int32_t x,
                                         int32_t y) {
  ASSERT(CBC_QRCoder::IsValidMaskPattern(maskPattern));

  int32_t intermediate = 0, temp = 0;
  switch (maskPattern) {
    case 0:
      intermediate = (y + x) & 0x1;
      break;
    case 1:
      intermediate = y & 0x1;
      break;
    case 2:
      intermediate = x % 3;
      break;
    case 3:
      intermediate = (y + x) % 3;
      break;
    case 4:
      intermediate = ((y >> 1) + (x / 3)) & 0x1;
      break;
    case 5:
      temp = y * x;
      intermediate = (temp & 0x1) + (temp % 3);
      break;
    case 6:
      temp = y * x;
      intermediate = (((temp & 0x1) + (temp % 3)) & 0x1);
      break;
    case 7:
      temp = y * x;
      intermediate = (((temp % 3) + ((y + x) & 0x1)) & 0x1);
      break;
    default:
      NOTREACHED();
      return false;
  }
  return intermediate == 0;
}

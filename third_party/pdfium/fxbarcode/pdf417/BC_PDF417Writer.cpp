// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2012 ZXing authors
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

#include "fxbarcode/pdf417/BC_PDF417Writer.h"

#include <algorithm>
#include <utility>

#include "fxbarcode/BC_TwoDimWriter.h"
#include "fxbarcode/common/BC_CommonBitMatrix.h"
#include "fxbarcode/pdf417/BC_PDF417.h"
#include "fxbarcode/pdf417/BC_PDF417BarcodeMatrix.h"
#include "third_party/base/stl_util.h"

CBC_PDF417Writer::CBC_PDF417Writer() : CBC_TwoDimWriter(false) {}

CBC_PDF417Writer::~CBC_PDF417Writer() = default;

bool CBC_PDF417Writer::SetErrorCorrectionLevel(int32_t level) {
  if (level < 0 || level > 8) {
    return false;
  }
  set_error_correction_level(level);
  return true;
}

std::vector<uint8_t> CBC_PDF417Writer::Encode(WideStringView contents,
                                              int32_t* pOutWidth,
                                              int32_t* pOutHeight) {
  std::vector<uint8_t> results;
  CBC_PDF417 encoder;
  int32_t col = (m_Width / m_ModuleWidth - 69) / 17;
  int32_t row = m_Height / (m_ModuleWidth * 20);
  if (row >= 3 && row <= 90 && col >= 1 && col <= 30)
    encoder.setDimensions(col, 1, row, 3);
  else if (col >= 1 && col <= 30)
    encoder.setDimensions(col, col, 90, 3);
  else if (row >= 3 && row <= 90)
    encoder.setDimensions(30, 1, row, row);
  if (!encoder.GenerateBarcodeLogic(contents, error_correction_level()))
    return results;

  CBC_BarcodeMatrix* barcodeMatrix = encoder.getBarcodeMatrix();
  std::vector<uint8_t> matrixData = barcodeMatrix->toBitArray();
  int32_t matrixWidth = barcodeMatrix->getWidth();
  int32_t matrixHeight = barcodeMatrix->getHeight();

  if (matrixWidth < matrixHeight) {
    RotateArray(&matrixData, matrixHeight, matrixWidth);
    std::swap(matrixWidth, matrixHeight);
  }
  *pOutWidth = matrixWidth;
  *pOutHeight = matrixHeight;
  results = pdfium::Vector2D<uint8_t>(*pOutWidth, *pOutHeight);
  memcpy(results.data(), matrixData.data(), *pOutWidth * *pOutHeight);
  return results;
}

void CBC_PDF417Writer::RotateArray(std::vector<uint8_t>* bitarray,
                                   int32_t height,
                                   int32_t width) {
  std::vector<uint8_t> temp = *bitarray;
  for (int32_t ii = 0; ii < height; ii++) {
    int32_t inverseii = height - ii - 1;
    for (int32_t jj = 0; jj < width; jj++) {
      (*bitarray)[jj * height + inverseii] = temp[ii * width + jj];
    }
  }
}

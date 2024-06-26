// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2011 ZXing authors
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

#include "fxbarcode/pdf417/BC_PDF417BarcodeMatrix.h"
#include "fxbarcode/pdf417/BC_PDF417BarcodeRow.h"
#include "third_party/base/ptr_util.h"

CBC_BarcodeMatrix::CBC_BarcodeMatrix(size_t width, size_t height)
    : m_width((width + 4) * 17 + 1), m_height(height) {
  m_matrix.resize(m_height);
  for (size_t i = 0; i < m_height; ++i)
    m_matrix[i] = pdfium::MakeUnique<CBC_BarcodeRow>(m_width);
}

CBC_BarcodeMatrix::~CBC_BarcodeMatrix() {}

std::vector<uint8_t> CBC_BarcodeMatrix::toBitArray() {
  std::vector<uint8_t> bitArray(m_width * m_height);
  for (size_t i = 0; i < m_height; ++i) {
    std::vector<uint8_t>& bytearray = m_matrix[i]->getRow();
    for (size_t j = 0; j < m_width; ++j)
      bitArray[i * m_width + j] = bytearray[j];
  }
  return bitArray;
}

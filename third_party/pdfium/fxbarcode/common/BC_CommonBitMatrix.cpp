// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2007 ZXing authors
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

#include "fxbarcode/common/BC_CommonBitMatrix.h"

#include <algorithm>
#include <iterator>

#include "third_party/base/stl_util.h"

CBC_CommonBitMatrix::CBC_CommonBitMatrix() {}

void CBC_CommonBitMatrix::Init(int32_t width, int32_t height) {
  m_width = width;
  m_height = height;
  m_rowSize = (width + 31) >> 5;
  m_bits = pdfium::Vector2D<int32_t>(m_rowSize, m_height);
}

CBC_CommonBitMatrix::~CBC_CommonBitMatrix() = default;

bool CBC_CommonBitMatrix::Get(int32_t x, int32_t y) const {
  int32_t offset = y * m_rowSize + (x >> 5);
  if (offset >= m_rowSize * m_height || offset < 0)
    return false;
  return ((((uint32_t)m_bits[offset]) >> (x & 0x1f)) & 1) != 0;
}

void CBC_CommonBitMatrix::Set(int32_t x, int32_t y) {
  int32_t offset = y * m_rowSize + (x >> 5);
  ASSERT(offset >= 0);
  ASSERT(offset < m_rowSize * m_height);
  m_bits[offset] |= 1 << (x & 0x1f);
}

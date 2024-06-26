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

#include <algorithm>
#include <iterator>
#include "third_party/base/stl_util.h"

CBC_CommonByteMatrix::CBC_CommonByteMatrix(int32_t width, int32_t height)
    : m_width(width), m_height(height) {
  m_bytes = pdfium::Vector2D<uint8_t>(m_height, m_width);
  clear(0xff);
}

CBC_CommonByteMatrix::~CBC_CommonByteMatrix() = default;

uint8_t CBC_CommonByteMatrix::Get(int32_t x, int32_t y) const {
  return m_bytes[y * m_width + x];
}

void CBC_CommonByteMatrix::Set(int32_t x, int32_t y, int32_t value) {
  m_bytes[y * m_width + x] = (uint8_t)value;
}

void CBC_CommonByteMatrix::Set(int32_t x, int32_t y, uint8_t value) {
  m_bytes[y * m_width + x] = value;
}

void CBC_CommonByteMatrix::clear(uint8_t value) {
  std::fill(std::begin(m_bytes), std::end(m_bytes), value);
}

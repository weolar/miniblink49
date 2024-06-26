// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2006 Jeremias Maerki
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

#include "fxbarcode/datamatrix/BC_SymbolInfo.h"

#include "fxbarcode/common/BC_CommonBitMatrix.h"
#include "fxbarcode/datamatrix/BC_DataMatrixSymbolInfo144.h"
#include "fxbarcode/datamatrix/BC_Encoder.h"

namespace {

constexpr size_t kSymbolsCount = 30;

CBC_SymbolInfo* g_symbols[kSymbolsCount] = {
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

}  // namespace

void CBC_SymbolInfo::Initialize() {
  g_symbols[0] = new CBC_SymbolInfo(3, 5, 8, 8, 1);
  g_symbols[1] = new CBC_SymbolInfo(5, 7, 10, 10, 1);
  g_symbols[2] = new CBC_SymbolInfo(5, 7, 16, 6, 1);
  g_symbols[3] = new CBC_SymbolInfo(8, 10, 12, 12, 1);
  g_symbols[4] = new CBC_SymbolInfo(10, 11, 14, 6, 2);
  g_symbols[5] = new CBC_SymbolInfo(12, 12, 14, 14, 1);
  g_symbols[6] = new CBC_SymbolInfo(16, 14, 24, 10, 1);
  g_symbols[7] = new CBC_SymbolInfo(18, 14, 16, 16, 1);
  g_symbols[8] = new CBC_SymbolInfo(22, 18, 18, 18, 1);
  g_symbols[9] = new CBC_SymbolInfo(22, 18, 16, 10, 2);
  g_symbols[10] = new CBC_SymbolInfo(30, 20, 20, 20, 1);
  g_symbols[11] = new CBC_SymbolInfo(32, 24, 16, 14, 2);
  g_symbols[12] = new CBC_SymbolInfo(36, 24, 22, 22, 1);
  g_symbols[13] = new CBC_SymbolInfo(44, 28, 24, 24, 1);
  g_symbols[14] = new CBC_SymbolInfo(49, 28, 22, 14, 2);
  g_symbols[15] = new CBC_SymbolInfo(62, 36, 14, 14, 4);
  g_symbols[16] = new CBC_SymbolInfo(86, 42, 16, 16, 4);
  g_symbols[17] = new CBC_SymbolInfo(114, 48, 18, 18, 4);
  g_symbols[18] = new CBC_SymbolInfo(144, 56, 20, 20, 4);
  g_symbols[19] = new CBC_SymbolInfo(174, 68, 22, 22, 4);
  g_symbols[20] = new CBC_SymbolInfo(204, 84, 24, 24, 4, 102, 42);
  g_symbols[21] = new CBC_SymbolInfo(280, 112, 14, 14, 16, 140, 56);
  g_symbols[22] = new CBC_SymbolInfo(368, 144, 16, 16, 16, 92, 36);
  g_symbols[23] = new CBC_SymbolInfo(456, 192, 18, 18, 16, 114, 48);
  g_symbols[24] = new CBC_SymbolInfo(576, 224, 20, 20, 16, 144, 56);
  g_symbols[25] = new CBC_SymbolInfo(696, 272, 22, 22, 16, 174, 68);
  g_symbols[26] = new CBC_SymbolInfo(816, 336, 24, 24, 16, 136, 56);
  g_symbols[27] = new CBC_SymbolInfo(1050, 408, 18, 18, 36, 175, 68);
  g_symbols[28] = new CBC_SymbolInfo(1304, 496, 20, 20, 36, 163, 62);
  g_symbols[29] = new CBC_DataMatrixSymbolInfo144();
}

void CBC_SymbolInfo::Finalize() {
  for (size_t i = 0; i < kSymbolsCount; i++) {
    delete g_symbols[i];
    g_symbols[i] = nullptr;
  }
}

CBC_SymbolInfo::CBC_SymbolInfo(size_t dataCapacity,
                               size_t errorCodewords,
                               int32_t matrixWidth,
                               int32_t matrixHeight,
                               int32_t dataRegions)
    : CBC_SymbolInfo(dataCapacity,
                     errorCodewords,
                     matrixWidth,
                     matrixHeight,
                     dataRegions,
                     dataCapacity,
                     errorCodewords) {}

CBC_SymbolInfo::CBC_SymbolInfo(size_t dataCapacity,
                               size_t errorCodewords,
                               int32_t matrixWidth,
                               int32_t matrixHeight,
                               int32_t dataRegions,
                               size_t rsBlockData,
                               size_t rsBlockError)
    : m_rectangular(matrixWidth != matrixHeight),
      m_dataCapacity(dataCapacity),
      m_errorCodewords(errorCodewords),
      m_matrixWidth(matrixWidth),
      m_matrixHeight(matrixHeight),
      m_dataRegions(dataRegions),
      m_rsBlockData(rsBlockData),
      m_rsBlockError(rsBlockError) {}

CBC_SymbolInfo::~CBC_SymbolInfo() = default;

const CBC_SymbolInfo* CBC_SymbolInfo::Lookup(size_t iDataCodewords,
                                             bool bAllowRectangular) {
  for (size_t i = 0; i < kSymbolsCount; i++) {
    CBC_SymbolInfo* symbol = g_symbols[i];
    if (symbol->m_rectangular && !bAllowRectangular)
      continue;

    if (iDataCodewords <= symbol->dataCapacity())
      return symbol;
  }
  return nullptr;
}

int32_t CBC_SymbolInfo::getHorizontalDataRegions() const {
  switch (m_dataRegions) {
    case 1:
      return 1;
    case 2:
      return 2;
    case 4:
      return 2;
    case 16:
      return 4;
    case 36:
      return 6;
    default:
      NOTREACHED();
      return 0;
  }
}

int32_t CBC_SymbolInfo::getVerticalDataRegions() const {
  switch (m_dataRegions) {
    case 1:
      return 1;
    case 2:
      return 1;
    case 4:
      return 2;
    case 16:
      return 4;
    case 36:
      return 6;
    default:
      NOTREACHED();
      return 0;
  }
}

int32_t CBC_SymbolInfo::getSymbolDataWidth() const {
  return getHorizontalDataRegions() * m_matrixWidth;
}

int32_t CBC_SymbolInfo::getSymbolDataHeight() const {
  return getVerticalDataRegions() * m_matrixHeight;
}

int32_t CBC_SymbolInfo::getSymbolWidth() const {
  return getSymbolDataWidth() + (getHorizontalDataRegions() * 2);
}

int32_t CBC_SymbolInfo::getSymbolHeight() const {
  return getSymbolDataHeight() + (getVerticalDataRegions() * 2);
}

size_t CBC_SymbolInfo::getCodewordCount() const {
  return m_dataCapacity + m_errorCodewords;
}

size_t CBC_SymbolInfo::getInterleavedBlockCount() const {
  return m_dataCapacity / m_rsBlockData;
}

size_t CBC_SymbolInfo::getDataLengthForInterleavedBlock() const {
  return m_rsBlockData;
}

size_t CBC_SymbolInfo::getErrorLengthForInterleavedBlock() const {
  return m_rsBlockError;
}

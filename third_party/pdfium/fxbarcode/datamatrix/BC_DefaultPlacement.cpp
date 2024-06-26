// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2006 Jeremias Maerki.
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

#include "fxbarcode/datamatrix/BC_DefaultPlacement.h"

#include <utility>

#include "fxbarcode/datamatrix/BC_Encoder.h"

CBC_DefaultPlacement::CBC_DefaultPlacement(WideString codewords,
                                           int32_t numcols,
                                           int32_t numrows)
    : m_codewords(std::move(codewords)),
      m_numrows(numrows),
      m_numcols(numcols) {
  m_bits.resize(numcols * numrows);
  for (int32_t i = 0; i < numcols * numrows; i++) {
    m_bits[i] = (uint8_t)2;
  }
}

CBC_DefaultPlacement::~CBC_DefaultPlacement() {}

int32_t CBC_DefaultPlacement::getNumrows() {
  return m_numrows;
}
int32_t CBC_DefaultPlacement::getNumcols() {
  return m_numcols;
}
std::vector<uint8_t>& CBC_DefaultPlacement::getBits() {
  return m_bits;
}
bool CBC_DefaultPlacement::getBit(int32_t col, int32_t row) {
  return m_bits[row * m_numcols + col] == 1;
}
void CBC_DefaultPlacement::setBit(int32_t col, int32_t row, bool bit) {
  m_bits[row * m_numcols + col] = bit ? (uint8_t)1 : (uint8_t)0;
}
bool CBC_DefaultPlacement::hasBit(int32_t col, int32_t row) {
  return m_bits[row * m_numcols + col] != 2;
}
void CBC_DefaultPlacement::place() {
  int32_t pos = 0;
  int32_t row = 4;
  int32_t col = 0;
  do {
    if ((row == m_numrows) && (col == 0)) {
      corner1(pos++);
    }
    if ((row == m_numrows - 2) && (col == 0) && ((m_numcols % 4) != 0)) {
      corner2(pos++);
    }
    if ((row == m_numrows - 2) && (col == 0) && (m_numcols % 8 == 4)) {
      corner3(pos++);
    }
    if ((row == m_numrows + 4) && (col == 2) && ((m_numcols % 8) == 0)) {
      corner4(pos++);
    }
    do {
      if ((row < m_numrows) && (col >= 0) && !hasBit(col, row)) {
        utah(row, col, pos++);
      }
      row -= 2;
      col += 2;
    } while (row >= 0 && (col < m_numcols));
    row++;
    col += 3;
    do {
      if ((row >= 0) && (col < m_numcols) && !hasBit(col, row)) {
        utah(row, col, pos++);
      }
      row += 2;
      col -= 2;
    } while ((row < m_numrows) && (col >= 0));
    row += 3;
    col++;
  } while ((row < m_numrows) || (col < m_numcols));
  if (!hasBit(m_numcols - 1, m_numrows - 1)) {
    setBit(m_numcols - 1, m_numrows - 1, true);
    setBit(m_numcols - 2, m_numrows - 2, true);
  }
}
void CBC_DefaultPlacement::module(int32_t row,
                                  int32_t col,
                                  int32_t pos,
                                  int32_t bit) {
  if (row < 0) {
    row += m_numrows;
    col += 4 - ((m_numrows + 4) % 8);
  }
  if (col < 0) {
    col += m_numcols;
    row += 4 - ((m_numcols + 4) % 8);
  }
  int32_t v = m_codewords[pos];
  v &= 1 << (8 - bit);
  setBit(col, row, v != 0);
}
void CBC_DefaultPlacement::utah(int32_t row, int32_t col, int32_t pos) {
  module(row - 2, col - 2, pos, 1);
  module(row - 2, col - 1, pos, 2);
  module(row - 1, col - 2, pos, 3);
  module(row - 1, col - 1, pos, 4);
  module(row - 1, col, pos, 5);
  module(row, col - 2, pos, 6);
  module(row, col - 1, pos, 7);
  module(row, col, pos, 8);
}
void CBC_DefaultPlacement::corner1(int32_t pos) {
  module(m_numrows - 1, 0, pos, 1);
  module(m_numrows - 1, 1, pos, 2);
  module(m_numrows - 1, 2, pos, 3);
  module(0, m_numcols - 2, pos, 4);
  module(0, m_numcols - 1, pos, 5);
  module(1, m_numcols - 1, pos, 6);
  module(2, m_numcols - 1, pos, 7);
  module(3, m_numcols - 1, pos, 8);
}
void CBC_DefaultPlacement::corner2(int32_t pos) {
  module(m_numrows - 3, 0, pos, 1);
  module(m_numrows - 2, 0, pos, 2);
  module(m_numrows - 1, 0, pos, 3);
  module(0, m_numcols - 4, pos, 4);
  module(0, m_numcols - 3, pos, 5);
  module(0, m_numcols - 2, pos, 6);
  module(0, m_numcols - 1, pos, 7);
  module(1, m_numcols - 1, pos, 8);
}
void CBC_DefaultPlacement::corner3(int32_t pos) {
  module(m_numrows - 3, 0, pos, 1);
  module(m_numrows - 2, 0, pos, 2);
  module(m_numrows - 1, 0, pos, 3);
  module(0, m_numcols - 2, pos, 4);
  module(0, m_numcols - 1, pos, 5);
  module(1, m_numcols - 1, pos, 6);
  module(2, m_numcols - 1, pos, 7);
  module(3, m_numcols - 1, pos, 8);
}
void CBC_DefaultPlacement::corner4(int32_t pos) {
  module(m_numrows - 1, 0, pos, 1);
  module(m_numrows - 1, m_numcols - 1, pos, 2);
  module(0, m_numcols - 3, pos, 3);
  module(0, m_numcols - 2, pos, 4);
  module(0, m_numcols - 1, pos, 5);
  module(1, m_numcols - 3, pos, 6);
  module(1, m_numcols - 2, pos, 7);
  module(1, m_numcols - 1, pos, 8);
}

// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_DATAMATRIX_BC_DEFAULTPLACEMENT_H_
#define FXBARCODE_DATAMATRIX_BC_DEFAULTPLACEMENT_H_

#include <vector>

#include "core/fxcrt/fx_string.h"

class CBC_DefaultPlacement {
 public:
  CBC_DefaultPlacement(WideString codewords, int32_t numcols, int32_t numrows);
  virtual ~CBC_DefaultPlacement();

  int32_t getNumrows();
  int32_t getNumcols();
  std::vector<uint8_t>& getBits();
  bool getBit(int32_t col, int32_t row);
  void setBit(int32_t col, int32_t row, bool bit);
  bool hasBit(int32_t col, int32_t row);
  void place();

 private:
  WideString m_codewords;
  int32_t m_numrows;
  int32_t m_numcols;
  std::vector<uint8_t> m_bits;
  void module(int32_t row, int32_t col, int32_t pos, int32_t bit);
  void utah(int32_t row, int32_t col, int32_t pos);
  void corner1(int32_t pos);
  void corner2(int32_t pos);
  void corner3(int32_t pos);
  void corner4(int32_t pos);
};

#endif  // FXBARCODE_DATAMATRIX_BC_DEFAULTPLACEMENT_H_

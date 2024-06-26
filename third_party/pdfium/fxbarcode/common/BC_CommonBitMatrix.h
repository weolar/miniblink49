// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_COMMON_BC_COMMONBITMATRIX_H_
#define FXBARCODE_COMMON_BC_COMMONBITMATRIX_H_

#include <vector>

#include "core/fxcrt/fx_system.h"

class CBC_CommonBitMatrix {
 public:
  CBC_CommonBitMatrix();
  ~CBC_CommonBitMatrix();

  void Init(int32_t width, int32_t height);

  bool Get(int32_t x, int32_t y) const;
  void Set(int32_t x, int32_t y);
  int32_t GetWidth() const { return m_width; }
  int32_t GetHeight() const { return m_height; }

 private:
  int32_t m_width = 0;
  int32_t m_height = 0;
  int32_t m_rowSize = 0;
  std::vector<int32_t> m_bits;
};

#endif  // FXBARCODE_COMMON_BC_COMMONBITMATRIX_H_

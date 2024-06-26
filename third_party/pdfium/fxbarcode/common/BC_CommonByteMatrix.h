// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_COMMON_BC_COMMONBYTEMATRIX_H_
#define FXBARCODE_COMMON_BC_COMMONBYTEMATRIX_H_

#include <stdint.h>

#include <vector>

#include "core/fxcrt/fx_system.h"
#include "third_party/base/span.h"

class CBC_CommonByteMatrix final {
 public:
  CBC_CommonByteMatrix(int32_t width, int32_t height);
  ~CBC_CommonByteMatrix();

  int32_t GetWidth() const { return m_width; }
  int32_t GetHeight() const { return m_height; }
  pdfium::span<const uint8_t> GetArray() const { return m_bytes; }
  uint8_t Get(int32_t x, int32_t y) const;

  void Set(int32_t x, int32_t y, int32_t value);
  void Set(int32_t x, int32_t y, uint8_t value);
  void clear(uint8_t value);

 private:
  int32_t m_width;
  int32_t m_height;
  std::vector<uint8_t> m_bytes;
};

#endif  // FXBARCODE_COMMON_BC_COMMONBYTEMATRIX_H_

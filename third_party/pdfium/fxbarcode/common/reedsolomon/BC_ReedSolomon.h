// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMON_H_
#define FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMON_H_

#include <memory>
#include <vector>

#include "core/fxcrt/unowned_ptr.h"

class CBC_ReedSolomonGF256;
class CBC_ReedSolomonGF256Poly;

class CBC_ReedSolomonEncoder {
 public:
  explicit CBC_ReedSolomonEncoder(CBC_ReedSolomonGF256* field);
  ~CBC_ReedSolomonEncoder();

  bool Encode(std::vector<int32_t>* toEncode, size_t ecBytes);

 private:
  CBC_ReedSolomonGF256Poly* BuildGenerator(size_t degree);

  UnownedPtr<CBC_ReedSolomonGF256> const m_field;
  std::vector<std::unique_ptr<CBC_ReedSolomonGF256Poly>> m_cachedGenerators;
};

#endif  // FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMON_H_

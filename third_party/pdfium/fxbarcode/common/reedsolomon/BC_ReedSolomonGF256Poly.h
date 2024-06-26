// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMONGF256POLY_H_
#define FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMONGF256POLY_H_

#include <memory>
#include <vector>

#include "core/fxcrt/unowned_ptr.h"

class CBC_ReedSolomonGF256;

class CBC_ReedSolomonGF256Poly final {
 public:
  CBC_ReedSolomonGF256Poly(CBC_ReedSolomonGF256* field,
                           const std::vector<int32_t>& coefficients);
  ~CBC_ReedSolomonGF256Poly();

  int32_t GetCoefficients(int32_t degree) const;
  const std::vector<int32_t>& GetCoefficients() const;
  int32_t GetDegree() const;
  bool IsZero() const;
  std::unique_ptr<CBC_ReedSolomonGF256Poly> AddOrSubtract(
      const CBC_ReedSolomonGF256Poly* other);
  std::unique_ptr<CBC_ReedSolomonGF256Poly> Multiply(
      const CBC_ReedSolomonGF256Poly* other);
  std::unique_ptr<CBC_ReedSolomonGF256Poly> Multiply(int32_t scalar);
  std::unique_ptr<CBC_ReedSolomonGF256Poly> MultiplyByMonomial(
      int32_t degree,
      int32_t coefficient) const;
  std::unique_ptr<CBC_ReedSolomonGF256Poly> Divide(
      const CBC_ReedSolomonGF256Poly* other);
  std::unique_ptr<CBC_ReedSolomonGF256Poly> Clone() const;

 private:
  UnownedPtr<CBC_ReedSolomonGF256> const m_field;
  std::vector<int32_t> m_coefficients;
};

#endif  // FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMONGF256POLY_H_

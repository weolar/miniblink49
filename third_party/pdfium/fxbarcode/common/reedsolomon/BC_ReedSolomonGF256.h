// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMONGF256_H_
#define FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMONGF256_H_

#include <memory>

#include "third_party/base/optional.h"

class CBC_ReedSolomonGF256Poly;

class CBC_ReedSolomonGF256 {
 public:
  explicit CBC_ReedSolomonGF256(int32_t primitive);
  ~CBC_ReedSolomonGF256();

  static void Initialize();
  static void Finalize();

  CBC_ReedSolomonGF256Poly* GetZero() const;
  CBC_ReedSolomonGF256Poly* GetOne() const;
  std::unique_ptr<CBC_ReedSolomonGF256Poly> BuildMonomial(int32_t degree,
                                                          int32_t coefficient);
  static int32_t AddOrSubtract(int32_t a, int32_t b);
  int32_t Exp(int32_t a);
  Optional<int32_t> Inverse(int32_t a);
  int32_t Multiply(int32_t a, int32_t b);
  void Init();

  static CBC_ReedSolomonGF256* QRCodeField;
  static CBC_ReedSolomonGF256* DataMatrixField;

 private:
  std::unique_ptr<CBC_ReedSolomonGF256Poly> m_zero;
  std::unique_ptr<CBC_ReedSolomonGF256Poly> m_one;
  int32_t m_expTable[256];
  int32_t m_logTable[256];
};

#endif  // FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMONGF256_H_

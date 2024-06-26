// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xfa/fxfa/cxfa_ffbarcode.h"

#include "testing/gtest/include/gtest/gtest.h"

TEST(XFA_FFBarcode, GetBarcodeTypeByName) {
  EXPECT_EQ(nullptr, CXFA_FFBarcode::GetBarcodeTypeByName(L""));
  EXPECT_EQ(nullptr, CXFA_FFBarcode::GetBarcodeTypeByName(L"not_found"));

  auto* data = CXFA_FFBarcode::GetBarcodeTypeByName(L"ean13");
  ASSERT_NE(nullptr, data);
  EXPECT_EQ(BarcodeType::ean13, data->eName);

  data = CXFA_FFBarcode::GetBarcodeTypeByName(L"pdf417");
  ASSERT_NE(nullptr, data);
  EXPECT_EQ(BarcodeType::pdf417, data->eName);

  data = CXFA_FFBarcode::GetBarcodeTypeByName(L"code3Of9");
  ASSERT_NE(nullptr, data);
  EXPECT_EQ(BarcodeType::code3Of9, data->eName);
}

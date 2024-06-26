// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcodec/jbig2/JBig2_BitStream.h"

#include <memory>
#include <utility>

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/base/ptr_util.h"

TEST(JBig2_BitStream, ReadNBits) {
  std::unique_ptr<uint8_t, FxFreeDeleter> data(FX_Alloc(uint8_t, 1));
  data.get()[0] = 0xb1;  // 10110001

  auto in_stream = pdfium::MakeUnique<CPDF_Stream>(std::move(data), 1, nullptr);
  auto acc = pdfium::MakeRetain<CPDF_StreamAcc>(in_stream.get());
  acc->LoadAllDataFiltered();

  CJBig2_BitStream stream(acc);

  uint32_t val1;
  EXPECT_EQ(0, stream.readNBits(1, &val1));
  EXPECT_EQ(1U, val1);

  int32_t val2;
  EXPECT_EQ(0, stream.readNBits(1, &val2));
  EXPECT_EQ(0, val2);

  EXPECT_EQ(0, stream.readNBits(2, &val2));
  EXPECT_EQ(3, val2);

  EXPECT_EQ(0, stream.readNBits(4, &val2));
  EXPECT_EQ(1, val2);
}

TEST(JBig2_BitStream, ReadNBitsLargerThenData) {
  std::unique_ptr<uint8_t, FxFreeDeleter> data(FX_Alloc(uint8_t, 1));
  data.get()[0] = 0xb1;

  auto in_stream = pdfium::MakeUnique<CPDF_Stream>(std::move(data), 1, nullptr);
  auto acc = pdfium::MakeRetain<CPDF_StreamAcc>(in_stream.get());
  acc->LoadAllDataFiltered();

  CJBig2_BitStream stream(acc);

  uint32_t val1;
  EXPECT_EQ(0, stream.readNBits(10, &val1));
  EXPECT_EQ(0xb1U, val1);
}

TEST(JBig2_BitStream, ReadNBitsNullStream) {
  auto in_stream = pdfium::MakeUnique<CPDF_Stream>(nullptr, 0, nullptr);
  auto acc = pdfium::MakeRetain<CPDF_StreamAcc>(in_stream.get());
  acc->LoadAllDataFiltered();

  CJBig2_BitStream stream(acc);

  uint32_t val1;
  EXPECT_EQ(-1, stream.readNBits(1, &val1));

  int32_t val2;
  EXPECT_EQ(-1, stream.readNBits(2, &val2));
}

TEST(JBig2_BitStream, ReadNBitsEmptyStream) {
  std::unique_ptr<uint8_t, FxFreeDeleter> data(FX_Alloc(uint8_t, 1));

  auto in_stream = pdfium::MakeUnique<CPDF_Stream>(std::move(data), 0, nullptr);
  auto acc = pdfium::MakeRetain<CPDF_StreamAcc>(in_stream.get());
  acc->LoadAllDataFiltered();

  CJBig2_BitStream stream(acc);

  uint32_t val1;
  EXPECT_EQ(-1, stream.readNBits(1, &val1));

  int32_t val2;
  EXPECT_EQ(-1, stream.readNBits(2, &val2));
}

TEST(JBig2_BitStream, ReadNBitsOutOfBounds) {
  std::unique_ptr<uint8_t, FxFreeDeleter> data(FX_Alloc(uint8_t, 1));
  data.get()[0] = 0xb1;  // 10110001

  auto in_stream = pdfium::MakeUnique<CPDF_Stream>(std::move(data), 1, nullptr);
  auto acc = pdfium::MakeRetain<CPDF_StreamAcc>(in_stream.get());
  acc->LoadAllDataFiltered();

  CJBig2_BitStream stream(acc);

  uint32_t val1;
  EXPECT_EQ(0, stream.readNBits(8, &val1));

  int32_t val2;
  EXPECT_EQ(-1, stream.readNBits(2, &val2));
}

TEST(JBig2_BitStream, ReadNBitsWhereNIs36) {
  std::unique_ptr<uint8_t, FxFreeDeleter> data(FX_Alloc(uint8_t, 5));
  data.get()[0] = 0xb0;
  data.get()[1] = 0x01;
  data.get()[2] = 0x00;
  data.get()[3] = 0x00;
  data.get()[4] = 0x40;

  auto in_stream = pdfium::MakeUnique<CPDF_Stream>(std::move(data), 5, nullptr);
  auto acc = pdfium::MakeRetain<CPDF_StreamAcc>(in_stream.get());
  acc->LoadAllDataFiltered();

  CJBig2_BitStream stream(acc);

  // This will shift off the top two bits and they end up lost.
  uint32_t val1;
  EXPECT_EQ(0, stream.readNBits(34, &val1));
  EXPECT_EQ(0xc0040001U, val1);
}

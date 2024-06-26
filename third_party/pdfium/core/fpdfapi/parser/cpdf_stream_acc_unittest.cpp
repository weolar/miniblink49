// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/parser/cpdf_stream_acc.h"

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fxcrt/fx_stream.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

class InvalidStream final : public IFX_SeekableReadStream {
 public:
  InvalidStream() = default;
  ~InvalidStream() override = default;

  // IFX_SeekableReadStream overrides:
  bool ReadBlockAtOffset(void* buffer,
                         FX_FILESIZE offset,
                         size_t size) override {
    // Read failure.
    return false;
  }

  FX_FILESIZE GetSize() override { return 1024; }
};

}  // namespace

TEST(CPDF_StreamAccTest, ReadRawDataFailed) {
  CPDF_Stream stream;
  stream.InitStreamFromFile(pdfium::MakeRetain<InvalidStream>(), nullptr);
  auto stream_acc = pdfium::MakeRetain<CPDF_StreamAcc>(&stream);
  stream_acc->LoadAllDataRaw();
  EXPECT_EQ(0u, stream_acc->GetSize());
  EXPECT_FALSE(stream_acc->GetData());
}

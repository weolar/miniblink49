// Copyright 2019 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/cfx_memorystream.h"

#include "core/fxcrt/retain_ptr.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

namespace {

const char kSomeText[] = "Lets make holes in streams";
const size_t kSomeTextLen = sizeof(kSomeText) - 1;

}  // namespace

TEST(CFX_MemoryStreamTest, SparseBlockWrites) {
  auto stream = pdfium::MakeRetain<CFX_MemoryStream>();
  for (FX_FILESIZE offset = 0; offset <= 200000; offset += 100000)
    stream->WriteBlockAtOffset(kSomeText, offset, kSomeTextLen);

  EXPECT_EQ(200000 + kSomeTextLen, static_cast<size_t>(stream->GetSize()));
}

TEST(CFX_MemoryStreamTest, OverlappingBlockWrites) {
  auto stream = pdfium::MakeRetain<CFX_MemoryStream>();
  for (FX_FILESIZE offset = 0; offset <= 100; ++offset)
    stream->WriteBlockAtOffset(kSomeText, offset, kSomeTextLen);

  EXPECT_EQ(100 + kSomeTextLen, static_cast<size_t>(stream->GetSize()));
}

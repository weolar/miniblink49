// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "public/cpp/fpdf_scopers.h"
#include "testing/embedder_test.h"
#include "testing/gtest/include/gtest/gtest.h"

class FPDFPageFuncEmbedderTest : public EmbedderTest {};

TEST_F(FPDFPageFuncEmbedderTest, Bug_551460) {
  // Should not crash under ASan.
  // Tests that the number of inputs is not simply calculated from the domain
  // and trusted. The number of inputs has to be 1.
  EXPECT_TRUE(OpenDocument("bug_551460.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  ScopedFPDFBitmap bitmap = RenderLoadedPage(page);
  CompareBitmap(bitmap.get(), 612, 792, "1940568c9ba33bac5d0b1ee9558c76b3");
  UnloadPage(page);
}

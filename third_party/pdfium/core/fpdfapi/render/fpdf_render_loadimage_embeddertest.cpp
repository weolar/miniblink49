// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "public/cpp/fpdf_scopers.h"
#include "testing/embedder_test.h"
#include "testing/gtest/include/gtest/gtest.h"

class FPDFRenderLoadImageEmbedderTest : public EmbedderTest {};

TEST_F(FPDFRenderLoadImageEmbedderTest, Bug_554151) {
  // Test scanline downsampling with a BitsPerComponent of 4.
  // Should not crash.
  EXPECT_TRUE(OpenDocument("bug_554151.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  ScopedFPDFBitmap bitmap = RenderLoadedPage(page);
  CompareBitmap(bitmap.get(), 612, 792, "1940568c9ba33bac5d0b1ee9558c76b3");
  UnloadPage(page);
}

TEST_F(FPDFRenderLoadImageEmbedderTest, Bug_557223) {
  // Should not crash
  EXPECT_TRUE(OpenDocument("bug_557223.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  ScopedFPDFBitmap bitmap = RenderLoadedPage(page);
  CompareBitmap(bitmap.get(), 24, 24, "dc0ea1b743c2edb22c597cadc8537f7b");
  UnloadPage(page);
}

TEST_F(FPDFRenderLoadImageEmbedderTest, Bug_603518) {
  // Should not crash
  EXPECT_TRUE(OpenDocument("bug_603518.pdf"));
  FPDF_PAGE page = LoadPage(0);
  EXPECT_NE(nullptr, page);
  ScopedFPDFBitmap bitmap = RenderLoadedPage(page);
  CompareBitmap(bitmap.get(), 749, 749, "b9e75190cdc5edf0069a408744ca07dc");
  UnloadPage(page);
}

TEST_F(FPDFRenderLoadImageEmbedderTest, Bug_1087) {
  EXPECT_TRUE(OpenDocument("bug_1087.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  ScopedFPDFBitmap bitmap = RenderLoadedPage(page);
  CompareBitmap(bitmap.get(), 548, 238, "2aac40afa121feb0f38883cb04a55001");
  UnloadPage(page);
}

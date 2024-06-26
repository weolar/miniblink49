// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "testing/embedder_test.h"
#include "testing/gtest/include/gtest/gtest.h"

class CPDF_DocPageDataEmbedderTest : public EmbedderTest {};

TEST_F(CPDF_DocPageDataEmbedderTest, Crasher_828206) {
  EXPECT_TRUE(OpenDocument("bug_828206.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  RenderLoadedPage(page);
  UnloadPage(page);
}

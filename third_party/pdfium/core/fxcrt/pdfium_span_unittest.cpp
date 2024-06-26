// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/base/span.h"

// Tests PDFium-modifications to base::span. The name of this file is
// chosen to avoid collisions with base's span_unittest.cc

TEST(PdfiumSpan, EmptySpan) {
  int stuff[] = {1, 2, 3};
  pdfium::span<int> stuff_span(stuff);
  pdfium::span<int> empty_first_span = stuff_span.first(0);
  pdfium::span<int> empty_last_span = stuff_span.last(0);
  pdfium::span<int> empty_sub_span1 = stuff_span.subspan(0, 0);
  pdfium::span<int> empty_sub_span2 = stuff_span.subspan(3, 0);
  EXPECT_TRUE(empty_first_span.empty());
  EXPECT_TRUE(empty_last_span.empty());
  EXPECT_TRUE(empty_sub_span1.empty());
  EXPECT_TRUE(empty_sub_span2.empty());
}

TEST(PdfiumSpan, EmptySpanDeath) {
  int stuff[] = {1, 2, 3};
  pdfium::span<int> stuff_span(stuff);
  pdfium::span<int> empty_span = stuff_span.last(0);
  EXPECT_DEATH(empty_span[0] += 1, ".*");
}

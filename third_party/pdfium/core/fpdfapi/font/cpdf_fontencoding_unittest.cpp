// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/font/cpdf_fontencoding.h"

#include "testing/gtest/include/gtest/gtest.h"

TEST(CPDFFontEncodingTest, PDF_AdobeNameFromUnicode) {
  EXPECT_STREQ("", PDF_AdobeNameFromUnicode(0x0000).c_str());
  EXPECT_STREQ("divide", PDF_AdobeNameFromUnicode(0x00f7).c_str());
  EXPECT_STREQ("Lslash", PDF_AdobeNameFromUnicode(0x0141).c_str());
  EXPECT_STREQ("tonos", PDF_AdobeNameFromUnicode(0x0384).c_str());
  EXPECT_STREQ("afii57513", PDF_AdobeNameFromUnicode(0x0691).c_str());
  EXPECT_STREQ("angkhankhuthai", PDF_AdobeNameFromUnicode(0x0e5a).c_str());
  EXPECT_STREQ("Euro", PDF_AdobeNameFromUnicode(0x20ac).c_str());
}

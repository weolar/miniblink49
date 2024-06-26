// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfdoc/cpdf_defaultappearance.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"
#include "third_party/base/span.h"

TEST(CPDFDefaultAppearanceTest, FindTagParamFromStart) {
  static const struct FindTagTestStruct {
    const unsigned char* input;
    unsigned int input_size;
    const char* token;
    int num_params;
    bool result;
    unsigned int result_pos;
  } test_data[] = {
      // Empty strings.
      STR_IN_TEST_CASE("", "Tj", 1, false, 0),
      STR_IN_TEST_CASE("", "", 1, false, 0),
      // Empty token.
      STR_IN_TEST_CASE("  T j", "", 1, false, 5),
      // No parameter.
      STR_IN_TEST_CASE("Tj", "Tj", 1, false, 2),
      STR_IN_TEST_CASE("(Tj", "Tj", 1, false, 3),
      // Partial token match.
      STR_IN_TEST_CASE("\r12\t34  56 78Tj", "Tj", 1, false, 15),
      // Regular cases with various parameters.
      STR_IN_TEST_CASE("\r\0abd Tj", "Tj", 1, true, 0),
      STR_IN_TEST_CASE("12 4 Tj 3 46 Tj", "Tj", 1, true, 2),
      STR_IN_TEST_CASE("er^ 2 (34) (5667) Tj", "Tj", 2, true, 5),
      STR_IN_TEST_CASE("<344> (232)\t343.4\n12 45 Tj", "Tj", 3, true, 11),
      STR_IN_TEST_CASE("1 2 3 4 5 6 7 8 cm", "cm", 6, true, 3),
  };

  CPDF_DefaultAppearance da;
  for (size_t i = 0; i < FX_ArraySize(test_data); ++i) {
    CPDF_SimpleParser parser(
        pdfium::make_span(test_data[i].input, test_data[i].input_size));
    EXPECT_EQ(test_data[i].result,
              da.FindTagParamFromStartForTesting(&parser, test_data[i].token,
                                                 test_data[i].num_params))
        << " for case " << i;
    EXPECT_EQ(test_data[i].result_pos, parser.GetCurPos()) << " for case " << i;
  }
}

// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxbarcode/pdf417/BC_PDF417HighLevelEncoder.h"

#include "testing/gtest/include/gtest/gtest.h"

TEST(PDF417HighLevelEncoderTest, EncodeHighLevel) {
  // TODO(tsepez): implement test cases.
}

TEST(PDF417HighLevelEncoderTest, EncodeText) {
  // TODO(tsepez): implement test cases.
}

TEST(PDF417HighLevelEncoderTest, EncodeBinary) {
  struct EncodeBinaryCase {
    const char* input;
    int offset;
    int count;
    CBC_PDF417HighLevelEncoder::EncodingMode startmode;
    const wchar_t* expected;
    int expected_length;
  } encode_binary_cases[] = {
      // Empty string encodes as empty string.
      {"", 0, 0, CBC_PDF417HighLevelEncoder::EncodingMode::kText, L"", 0},

      // Fewer than 6 characters encodes as prefix without compaction.
      {"xxxxx", 0, 5, CBC_PDF417HighLevelEncoder::EncodingMode::kText,
       L"\x0385xxxxx", 6},

      // 6 charcters triggerst text encoding compaction.
      {"xxxxxx", 0, 6, CBC_PDF417HighLevelEncoder::EncodingMode::kText,
       L"\u039c\u00c9\u031f\u012a\u00d2\u02d0", 6},

      // Same result if initially in numeric compaction mode.
      {"xxxxxx", 0, 6, CBC_PDF417HighLevelEncoder::EncodingMode::kNumeric,
       L"\u039c\u00c9\u031f\u012a\u00d2\u02d0", 6},
  };

  for (size_t i = 0; i < FX_ArraySize(encode_binary_cases); ++i) {
    EncodeBinaryCase* ptr = &encode_binary_cases[i];
    std::vector<uint8_t> input_array;
    size_t input_length = strlen(ptr->input);
    input_array.resize(input_length);
    for (size_t j = 0; j < input_length; ++j) {
      input_array[j] = ptr->input[j];
    }
    WideString expected(ptr->expected, ptr->expected_length);
    WideString result;
    CBC_PDF417HighLevelEncoder::EncodeBinary(
        input_array, ptr->offset, ptr->count, ptr->startmode, &result);
    EXPECT_EQ(expected, result) << " for case number " << i;
  }
}

TEST(PDF417HighLevelEncoderTest, EncodeNumeric) {
  struct EncodeNumericCase {
    const wchar_t* input;
    int offset;
    int count;
    const wchar_t* expected;
    int expected_length;
  } encode_numeric_cases[] = {
      // Empty string encodes as empty string.
      {L"", 0, 0, L"", 0},

      // Single 0 should encode as 10 base-900 == a.
      {L"0", 0, 1, L"\x000a", 1},

      // 800 should encode as 1800 base-900 == 2,0.
      {L"800", 0, 3, L"\x0002\x0000", 2},

      // Test longer strings and sub-strings.
      {L"123456", 0, 6, L"\x0001\x015c\x0100", 3},
      {L"123456", 0, 5, L"\x007c\x02e9", 2},
      {L"123456", 1, 5, L"\x0089\x009c", 2},
      {L"123456", 2, 2, L"\x0086", 1},

      // Up to 44 characters encodes as 15 base-900 words.
      {L"00000000000000000000000000000000000000000000", 0, 44,
       L"\x01b5\x006f\x02cc\x0084\x01bc\x0076\x00b3\x005c\x01f0\x034f\x01e6"
       L"\x0090\x020b\x019b\x0064",
       15},

      // 45 characters should encode as same 15 words followed by one additional
      // word.
      {L"000000000000000000000000000000000000000000000", 0, 45,
       L"\x01b5\x006f\x02cc\x0084\x01bc\x0076\x00b3\x005c\x01f0\x034f\x01e6"
       L"\x0090\x020b\x019b\x0064\x000a",
       16},

      // 44 characters followed by 800 should encode as 15 words followed by
      // 1800 base-900 == 2,0.
      {L"00000000000000000000000000000000000000000000800", 0, 47,
       L"\x01b5\x006f\x02cc\x0084\x01bc\x0076\x00b3\x005c\x01f0\x034f\x01e6"
       L"\x0090\x020b\x019b\x0064\x0002\x0000",
       17},

      // Even longer input.
      {L"10000000000000000000000000000000000000000000000000", 0, 50,
       L"\x01e0\x02f0\x036d\x02ad\x029c\x01ea\x0011\x000b\x02d6\x023c\x0108"
       L"\x02bb\x0023\x02d2\x00c8\x0001\x00d3\x0064",
       18},
  };

  for (size_t i = 0; i < FX_ArraySize(encode_numeric_cases); ++i) {
    EncodeNumericCase* ptr = &encode_numeric_cases[i];
    WideString input(ptr->input);
    WideString expected(ptr->expected, ptr->expected_length);
    WideString result;
    CBC_PDF417HighLevelEncoder::EncodeNumeric(input, ptr->offset, ptr->count,
                                              &result);
    EXPECT_EQ(expected, result) << " for case number " << i;
  }
}

TEST(PDF417HighLevelEncoderTest, ConsecutiveDigitCount) {
  struct ConsecutiveDigitCase {
    const wchar_t* input;
    int offset;
    int expected_count;
  } consecutive_digit_cases[] = {
      // Empty string contains 0 consecuitve digits.
      {L"", 0, 0},

      // Single non-digit character contains 0 consecutive digits.
      {L"X", 0, 0},

      // Leading non-digit followed by digits contains 0 consecutive.
      {L"X123", 0, 0},

      // Single digit contains 1 consecutive digit.
      {L"1", 0, 1},

      // Single digit followe by non-digit contains 1 consecutive digit.
      {L"1Z", 0, 1},

      // Test longer strings.
      {L"123FOO45678", 0, 3},

      // Test subtring starting in digits field.
      {L"123FOO45678", 3, 0},

      // Test subtring starting in non-digits field.
      {L"123FOO45678", 3, 0},

      // Test substring starting in digits field following non-digit field.
      {L"123FOO45678", 6, 5},
  };

  for (size_t i = 0; i < FX_ArraySize(consecutive_digit_cases); ++i) {
    ConsecutiveDigitCase* ptr = &consecutive_digit_cases[i];
    WideString input(ptr->input);
    int actual_count =
        CBC_PDF417HighLevelEncoder::DetermineConsecutiveDigitCount(input,
                                                                   ptr->offset);
    EXPECT_EQ(ptr->expected_count, actual_count) << " for case number " << i;
  }
}

TEST(PDF417HighLevelEncoderTest, ConsecutiveTextCount) {
  struct ConsecutiveTextCase {
    const wchar_t* input;
    int offset;
    int expected_count;
  } consecutive_text_cases[] = {
      // Empty string contains 0 consecutive text characters.
      {L"", 0, 0},

      // Single text character is 1 consecutive text characters.
      {L"X", 0, 1},

      // Trailing numbers count as text characters.
      {L"X123", 0, 4},

      // Leading numbers count as text characters.
      {L"123X", 0, 4},

      // Embedded lo-value binary characters terminate text runs.
      {L"ABC\x0001XXXX", 0, 3},

      // Embedded hi-value binary characters terminate text runs.
      {L"ABC\x0100XXXX", 0, 3},

      // Text run still found after indexing past lo-value character.
      {L"ABC\x0001XXXX", 4, 4},

      // Text run still found after indexing past hi-value character.
      {L"ABC\x0100XXXX", 4, 4},

      // Leading hi-value character results in 0 consecutive characters.
      {L"\x0100XXX", 0, 0},

      // Up to 12 numbers count as text.
      {L"123456789012", 0, 12},

      // 13 or more numbers are compresssed using numeric compression, not text.
      {L"1234567890123", 0, 0},

      // Leading Text character doesn't affect the 12 character case.
      {L"X123456789012", 0, 13},

      // Leading Text character doesn't affect the 13 character case.
      {L"X1234567890123", 0, 1},

      // Jumping between numbers and letters works properly.
      {L"XXX121XXX12345678901234", 0, 9},
  };

  for (size_t i = 0; i < FX_ArraySize(consecutive_text_cases); ++i) {
    ConsecutiveTextCase* ptr = &consecutive_text_cases[i];
    WideString input(ptr->input);
    int actual_count =
        CBC_PDF417HighLevelEncoder::DetermineConsecutiveTextCount(input,
                                                                  ptr->offset);
    EXPECT_EQ(ptr->expected_count, actual_count) << " for case number " << i;
  }
}

TEST(PDF417HighLevelEncoderTest, ConsecutiveBinaryCount) {
  // TODO(tsepez): implement test cases.
}

// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/bytestring.h"

#include <algorithm>
#include <iterator>
#include <vector>

#include "core/fxcrt/fx_string.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/base/span.h"
#include "third_party/base/stl_util.h"

namespace fxcrt {

TEST(ByteString, ElementAccess) {
  const ByteString abc("abc");
  EXPECT_EQ('a', abc[0]);
  EXPECT_EQ('b', abc[1]);
  EXPECT_EQ('c', abc[2]);
#ifndef NDEBUG
  EXPECT_DEATH({ abc[3]; }, ".*");
#endif

  pdfium::span<const char> abc_span = abc.AsSpan();
  EXPECT_EQ(3u, abc_span.size());
  EXPECT_EQ(0, memcmp(abc_span.data(), "abc", 3));

  pdfium::span<const uint8_t> abc_raw_span = abc.AsRawSpan();
  EXPECT_EQ(3u, abc_raw_span.size());
  EXPECT_EQ(0, memcmp(abc_raw_span.data(), "abc", 3));

  ByteString mutable_abc = abc;
  EXPECT_EQ(abc.c_str(), mutable_abc.c_str());
  EXPECT_EQ('a', mutable_abc[0]);
  EXPECT_EQ('b', mutable_abc[1]);
  EXPECT_EQ('c', mutable_abc[2]);
  EXPECT_EQ(abc.c_str(), mutable_abc.c_str());
  EXPECT_EQ("abc", abc);

  const char* c_str = abc.c_str();
  mutable_abc.SetAt(0, 'd');
  EXPECT_EQ(c_str, abc.c_str());
  EXPECT_NE(c_str, mutable_abc.c_str());
  EXPECT_EQ("abc", abc);
  EXPECT_EQ("dbc", mutable_abc);

  mutable_abc.SetAt(1, 'e');
  EXPECT_EQ("abc", abc);
  EXPECT_EQ("dec", mutable_abc);

  mutable_abc.SetAt(2, 'f');
  EXPECT_EQ("abc", abc);
  EXPECT_EQ("def", mutable_abc);
#ifndef NDEBUG
  EXPECT_DEATH({ mutable_abc.SetAt(3, 'g'); }, ".*");
  EXPECT_EQ("abc", abc);
#endif
}

TEST(ByteString, Assign) {
  {
    // Copy-assign.
    ByteString string1;
    EXPECT_EQ(0, string1.ReferenceCountForTesting());
    {
      ByteString string2("abc");
      EXPECT_EQ(1, string2.ReferenceCountForTesting());

      string1 = string2;
      EXPECT_EQ(2, string1.ReferenceCountForTesting());
      EXPECT_EQ(2, string2.ReferenceCountForTesting());
    }
    EXPECT_EQ(1, string1.ReferenceCountForTesting());
  }
  {
    // Move-assign.
    ByteString string1;
    EXPECT_EQ(0, string1.ReferenceCountForTesting());
    {
      ByteString string2("abc");
      EXPECT_EQ(1, string2.ReferenceCountForTesting());

      string1 = std::move(string2);
      EXPECT_EQ(1, string1.ReferenceCountForTesting());
      EXPECT_EQ(0, string2.ReferenceCountForTesting());
    }
    EXPECT_EQ(1, string1.ReferenceCountForTesting());
  }
}

TEST(ByteString, OperatorLT) {
  ByteString empty;
  ByteString a("a");
  ByteString abc("abc");
  ByteString def("def");
  ByteStringView v_empty;
  ByteStringView v_a("a");
  ByteStringView v_abc("abc");
  ByteStringView v_def("def");
  const char* const c_null = nullptr;
  const char* const c_empty = "";
  const char* const c_a = "a";
  const char* const c_abc = "abc";
  const char* const c_def = "def";

  EXPECT_FALSE(empty < empty);
  EXPECT_FALSE(a < a);
  EXPECT_FALSE(abc < abc);
  EXPECT_FALSE(def < def);
  EXPECT_FALSE(c_null < empty);
  EXPECT_FALSE(c_empty < empty);
  EXPECT_FALSE(c_a < a);
  EXPECT_FALSE(c_abc < abc);
  EXPECT_FALSE(c_def < def);
  EXPECT_FALSE(empty < c_null);
  EXPECT_FALSE(empty < c_empty);
  EXPECT_FALSE(a < c_a);
  EXPECT_FALSE(abc < c_abc);
  EXPECT_FALSE(def < c_def);
  EXPECT_FALSE(empty < v_empty);
  EXPECT_FALSE(a < v_a);
  EXPECT_FALSE(abc < v_abc);
  EXPECT_FALSE(def < v_def);

  EXPECT_TRUE(empty < a);
  EXPECT_FALSE(a < empty);
  EXPECT_TRUE(c_null < a);
  EXPECT_TRUE(c_empty < a);
  EXPECT_FALSE(c_a < empty);
  EXPECT_TRUE(empty < c_a);
  EXPECT_FALSE(a < c_null);
  EXPECT_FALSE(a < c_empty);
  EXPECT_TRUE(empty < v_a);
  EXPECT_FALSE(a < v_empty);

  EXPECT_TRUE(empty < abc);
  EXPECT_FALSE(abc < empty);
  EXPECT_TRUE(c_null < abc);
  EXPECT_TRUE(c_empty < abc);
  EXPECT_FALSE(c_abc < empty);
  EXPECT_TRUE(empty < c_abc);
  EXPECT_FALSE(abc < c_null);
  EXPECT_FALSE(abc < c_empty);
  EXPECT_TRUE(empty < v_abc);
  EXPECT_FALSE(abc < v_empty);

  EXPECT_TRUE(empty < def);
  EXPECT_FALSE(def < empty);
  EXPECT_TRUE(c_null < def);
  EXPECT_TRUE(c_empty < def);
  EXPECT_FALSE(c_def < empty);
  EXPECT_TRUE(empty < c_def);
  EXPECT_FALSE(def < c_null);
  EXPECT_FALSE(def < c_empty);
  EXPECT_TRUE(empty < v_def);
  EXPECT_FALSE(def < v_empty);

  EXPECT_TRUE(a < abc);
  EXPECT_FALSE(abc < a);
  EXPECT_TRUE(c_a < abc);
  EXPECT_FALSE(c_abc < a);
  EXPECT_TRUE(a < c_abc);
  EXPECT_FALSE(abc < c_a);
  EXPECT_TRUE(a < v_abc);
  EXPECT_FALSE(abc < v_a);

  EXPECT_TRUE(a < def);
  EXPECT_FALSE(def < a);
  EXPECT_TRUE(c_a < def);
  EXPECT_FALSE(c_def < a);
  EXPECT_TRUE(a < c_def);
  EXPECT_FALSE(def < c_a);
  EXPECT_TRUE(a < v_def);
  EXPECT_FALSE(def < v_a);

  EXPECT_TRUE(abc < def);
  EXPECT_FALSE(def < abc);
  EXPECT_TRUE(c_abc < def);
  EXPECT_FALSE(c_def < abc);
  EXPECT_TRUE(abc < c_def);
  EXPECT_FALSE(def < c_abc);
  EXPECT_TRUE(abc < v_def);
  EXPECT_FALSE(def < v_abc);
}

TEST(ByteString, OperatorEQ) {
  ByteString null_string;
  EXPECT_TRUE(null_string == null_string);

  ByteString empty_string("");
  EXPECT_TRUE(empty_string == empty_string);
  EXPECT_TRUE(empty_string == null_string);
  EXPECT_TRUE(null_string == empty_string);

  ByteString deleted_string("hello");
  deleted_string.Delete(0, 5);
  EXPECT_TRUE(deleted_string == deleted_string);
  EXPECT_TRUE(deleted_string == null_string);
  EXPECT_TRUE(deleted_string == empty_string);
  EXPECT_TRUE(null_string == deleted_string);
  EXPECT_TRUE(empty_string == deleted_string);

  ByteString byte_string("hello");
  EXPECT_TRUE(byte_string == byte_string);
  EXPECT_FALSE(byte_string == null_string);
  EXPECT_FALSE(byte_string == empty_string);
  EXPECT_FALSE(byte_string == deleted_string);
  EXPECT_FALSE(null_string == byte_string);
  EXPECT_FALSE(empty_string == byte_string);
  EXPECT_FALSE(deleted_string == byte_string);

  ByteString byte_string_same1("hello");
  EXPECT_TRUE(byte_string == byte_string_same1);
  EXPECT_TRUE(byte_string_same1 == byte_string);

  ByteString byte_string_same2(byte_string);
  EXPECT_TRUE(byte_string == byte_string_same2);
  EXPECT_TRUE(byte_string_same2 == byte_string);

  ByteString byte_string1("he");
  ByteString byte_string2("hellp");
  ByteString byte_string3("hellod");
  EXPECT_FALSE(byte_string == byte_string1);
  EXPECT_FALSE(byte_string == byte_string2);
  EXPECT_FALSE(byte_string == byte_string3);
  EXPECT_FALSE(byte_string1 == byte_string);
  EXPECT_FALSE(byte_string2 == byte_string);
  EXPECT_FALSE(byte_string3 == byte_string);

  ByteStringView null_string_c;
  ByteStringView empty_string_c("");
  EXPECT_TRUE(null_string == null_string_c);
  EXPECT_TRUE(null_string == empty_string_c);
  EXPECT_TRUE(empty_string == null_string_c);
  EXPECT_TRUE(empty_string == empty_string_c);
  EXPECT_TRUE(deleted_string == null_string_c);
  EXPECT_TRUE(deleted_string == empty_string_c);
  EXPECT_TRUE(null_string_c == null_string);
  EXPECT_TRUE(empty_string_c == null_string);
  EXPECT_TRUE(null_string_c == empty_string);
  EXPECT_TRUE(empty_string_c == empty_string);
  EXPECT_TRUE(null_string_c == deleted_string);
  EXPECT_TRUE(empty_string_c == deleted_string);

  ByteStringView byte_string_c_same1("hello");
  EXPECT_TRUE(byte_string == byte_string_c_same1);
  EXPECT_TRUE(byte_string_c_same1 == byte_string);

  ByteStringView byte_string_c1("he");
  ByteStringView byte_string_c2("hellp");
  ByteStringView byte_string_c3("hellod");
  EXPECT_FALSE(byte_string == byte_string_c1);
  EXPECT_FALSE(byte_string == byte_string_c2);
  EXPECT_FALSE(byte_string == byte_string_c3);
  EXPECT_FALSE(byte_string_c1 == byte_string);
  EXPECT_FALSE(byte_string_c2 == byte_string);
  EXPECT_FALSE(byte_string_c3 == byte_string);

  const char* const c_null_string = nullptr;
  const char* const c_empty_string = "";
  EXPECT_TRUE(null_string == c_null_string);
  EXPECT_TRUE(null_string == c_empty_string);
  EXPECT_TRUE(empty_string == c_null_string);
  EXPECT_TRUE(empty_string == c_empty_string);
  EXPECT_TRUE(deleted_string == c_null_string);
  EXPECT_TRUE(deleted_string == c_empty_string);
  EXPECT_TRUE(c_null_string == null_string);
  EXPECT_TRUE(c_empty_string == null_string);
  EXPECT_TRUE(c_null_string == empty_string);
  EXPECT_TRUE(c_empty_string == empty_string);
  EXPECT_TRUE(c_null_string == deleted_string);
  EXPECT_TRUE(c_empty_string == deleted_string);

  const char* const c_string_same1 = "hello";
  EXPECT_TRUE(byte_string == c_string_same1);
  EXPECT_TRUE(c_string_same1 == byte_string);

  const char* const c_string1 = "he";
  const char* const c_string2 = "hellp";
  const char* const c_string3 = "hellod";
  EXPECT_FALSE(byte_string == c_string1);
  EXPECT_FALSE(byte_string == c_string2);
  EXPECT_FALSE(byte_string == c_string3);
  EXPECT_FALSE(c_string1 == byte_string);
  EXPECT_FALSE(c_string2 == byte_string);
  EXPECT_FALSE(c_string3 == byte_string);
}

TEST(ByteString, OperatorNE) {
  ByteString null_string;
  EXPECT_FALSE(null_string != null_string);

  ByteString empty_string("");
  EXPECT_FALSE(empty_string != empty_string);
  EXPECT_FALSE(empty_string != null_string);
  EXPECT_FALSE(null_string != empty_string);

  ByteString deleted_string("hello");
  deleted_string.Delete(0, 5);
  EXPECT_FALSE(deleted_string != deleted_string);
  EXPECT_FALSE(deleted_string != null_string);
  EXPECT_FALSE(deleted_string != empty_string);
  EXPECT_FALSE(deleted_string != deleted_string);
  EXPECT_FALSE(null_string != deleted_string);
  EXPECT_FALSE(empty_string != deleted_string);
  EXPECT_FALSE(deleted_string != deleted_string);

  ByteString byte_string("hello");
  EXPECT_FALSE(byte_string != byte_string);
  EXPECT_TRUE(byte_string != null_string);
  EXPECT_TRUE(byte_string != empty_string);
  EXPECT_TRUE(byte_string != deleted_string);
  EXPECT_TRUE(null_string != byte_string);
  EXPECT_TRUE(empty_string != byte_string);
  EXPECT_TRUE(deleted_string != byte_string);

  ByteString byte_string_same1("hello");
  EXPECT_FALSE(byte_string != byte_string_same1);
  EXPECT_FALSE(byte_string_same1 != byte_string);

  ByteString byte_string_same2(byte_string);
  EXPECT_FALSE(byte_string != byte_string_same2);
  EXPECT_FALSE(byte_string_same2 != byte_string);

  ByteString byte_string1("he");
  ByteString byte_string2("hellp");
  ByteString byte_string3("hellod");
  EXPECT_TRUE(byte_string != byte_string1);
  EXPECT_TRUE(byte_string != byte_string2);
  EXPECT_TRUE(byte_string != byte_string3);
  EXPECT_TRUE(byte_string1 != byte_string);
  EXPECT_TRUE(byte_string2 != byte_string);
  EXPECT_TRUE(byte_string3 != byte_string);

  ByteStringView null_string_c;
  ByteStringView empty_string_c("");
  EXPECT_FALSE(null_string != null_string_c);
  EXPECT_FALSE(null_string != empty_string_c);
  EXPECT_FALSE(empty_string != null_string_c);
  EXPECT_FALSE(empty_string != empty_string_c);
  EXPECT_FALSE(null_string_c != null_string);
  EXPECT_FALSE(empty_string_c != null_string);
  EXPECT_FALSE(null_string_c != empty_string);
  EXPECT_FALSE(empty_string_c != empty_string);

  ByteStringView byte_string_c_same1("hello");
  EXPECT_FALSE(byte_string != byte_string_c_same1);
  EXPECT_FALSE(byte_string_c_same1 != byte_string);

  ByteStringView byte_string_c1("he");
  ByteStringView byte_string_c2("hellp");
  ByteStringView byte_string_c3("hellod");
  EXPECT_TRUE(byte_string != byte_string_c1);
  EXPECT_TRUE(byte_string != byte_string_c2);
  EXPECT_TRUE(byte_string != byte_string_c3);
  EXPECT_TRUE(byte_string_c1 != byte_string);
  EXPECT_TRUE(byte_string_c2 != byte_string);
  EXPECT_TRUE(byte_string_c3 != byte_string);

  const char* const c_null_string = nullptr;
  const char* const c_empty_string = "";
  EXPECT_FALSE(null_string != c_null_string);
  EXPECT_FALSE(null_string != c_empty_string);
  EXPECT_FALSE(empty_string != c_null_string);
  EXPECT_FALSE(empty_string != c_empty_string);
  EXPECT_FALSE(deleted_string != c_null_string);
  EXPECT_FALSE(deleted_string != c_empty_string);
  EXPECT_FALSE(c_null_string != null_string);
  EXPECT_FALSE(c_empty_string != null_string);
  EXPECT_FALSE(c_null_string != empty_string);
  EXPECT_FALSE(c_empty_string != empty_string);
  EXPECT_FALSE(c_null_string != deleted_string);
  EXPECT_FALSE(c_empty_string != deleted_string);

  const char* const c_string_same1 = "hello";
  EXPECT_FALSE(byte_string != c_string_same1);
  EXPECT_FALSE(c_string_same1 != byte_string);

  const char* const c_string1 = "he";
  const char* const c_string2 = "hellp";
  const char* const c_string3 = "hellod";
  EXPECT_TRUE(byte_string != c_string1);
  EXPECT_TRUE(byte_string != c_string2);
  EXPECT_TRUE(byte_string != c_string3);
  EXPECT_TRUE(c_string1 != byte_string);
  EXPECT_TRUE(c_string2 != byte_string);
  EXPECT_TRUE(c_string3 != byte_string);
}

TEST(ByteString, OperatorPlus) {
  EXPECT_EQ("I like dogs", "I like " + ByteString("dogs"));
  EXPECT_EQ("Dogs like me", ByteString("Dogs") + " like me");
  EXPECT_EQ("Oh no, error number 42",
            "Oh no, error number " + ByteString::Format("%d", 42));

  {
    // Make sure operator+= and Concat() increases string memory allocation
    // geometrically.
    int allocations = 0;
    ByteString str("ABCDEFGHIJKLMN");
    const char* buffer = str.c_str();
    for (size_t i = 0; i < 10000; ++i) {
      str += "!";
      const char* new_buffer = str.c_str();
      if (new_buffer != buffer) {
        buffer = new_buffer;
        ++allocations;
      }
    }
    EXPECT_LT(allocations, 25);
    EXPECT_GT(allocations, 10);
  }
}

TEST(ByteString, Concat) {
  ByteString fred;
  fred.Concat("FRED", 4);
  EXPECT_EQ("FRED", fred);

  fred.Concat("DY", 2);
  EXPECT_EQ("FREDDY", fred);

  fred.Delete(3, 3);
  EXPECT_EQ("FRE", fred);

  fred.Concat("D", 1);
  EXPECT_EQ("FRED", fred);

  ByteString copy = fred;
  fred.Concat("DY", 2);
  EXPECT_EQ("FREDDY", fred);
  EXPECT_EQ("FRED", copy);
}

TEST(ByteString, Remove) {
  ByteString freed("FREED");
  freed.Remove('E');
  EXPECT_EQ("FRD", freed);
  freed.Remove('F');
  EXPECT_EQ("RD", freed);
  freed.Remove('D');
  EXPECT_EQ("R", freed);
  freed.Remove('X');
  EXPECT_EQ("R", freed);
  freed.Remove('R');
  EXPECT_EQ("", freed);

  ByteString empty;
  empty.Remove('X');
  EXPECT_EQ("", empty);
}

TEST(ByteString, RemoveCopies) {
  ByteString freed("FREED");
  const char* old_buffer = freed.c_str();

  // No change with single reference - no copy.
  freed.Remove('Q');
  EXPECT_EQ("FREED", freed);
  EXPECT_EQ(old_buffer, freed.c_str());

  // Change with single reference - no copy.
  freed.Remove('E');
  EXPECT_EQ("FRD", freed);
  EXPECT_EQ(old_buffer, freed.c_str());

  // No change with multiple references - no copy.
  ByteString shared(freed);
  freed.Remove('Q');
  EXPECT_EQ("FRD", freed);
  EXPECT_EQ(old_buffer, freed.c_str());
  EXPECT_EQ(old_buffer, shared.c_str());

  // Change with multiple references -- must copy.
  freed.Remove('D');
  EXPECT_EQ("FR", freed);
  EXPECT_NE(old_buffer, freed.c_str());
  EXPECT_EQ("FRD", shared);
  EXPECT_EQ(old_buffer, shared.c_str());
}

TEST(ByteString, Replace) {
  ByteString fred("FRED");
  fred.Replace("FR", "BL");
  EXPECT_EQ("BLED", fred);
  fred.Replace("D", "DDY");
  EXPECT_EQ("BLEDDY", fred);
  fred.Replace("LEDD", "");
  EXPECT_EQ("BY", fred);
  fred.Replace("X", "CLAMS");
  EXPECT_EQ("BY", fred);
  fred.Replace("BY", "HI");
  EXPECT_EQ("HI", fred);
  fred.Replace("", "CLAMS");
  EXPECT_EQ("HI", fred);
  fred.Replace("HI", "");
  EXPECT_EQ("", fred);
}

TEST(ByteString, Insert) {
  ByteString fred("FRED");
  EXPECT_EQ(5u, fred.Insert(0, 'S'));
  EXPECT_EQ("SFRED", fred);
  EXPECT_EQ(6u, fred.Insert(1, 'T'));
  EXPECT_EQ("STFRED", fred);
  EXPECT_EQ(7u, fred.Insert(4, 'U'));
  EXPECT_EQ("STFRUED", fred);
  EXPECT_EQ(8u, fred.Insert(7, 'V'));
  EXPECT_EQ("STFRUEDV", fred);
  EXPECT_EQ(8u, fred.Insert(12, 'P'));
  EXPECT_EQ("STFRUEDV", fred);
  {
    ByteString empty;
    EXPECT_EQ(1u, empty.Insert(0, 'X'));
    EXPECT_EQ("X", empty);
  }
  {
    ByteString empty;
    EXPECT_EQ(0u, empty.Insert(5, 'X'));
    EXPECT_NE("X", empty);
  }
}

TEST(ByteString, InsertAtFrontAndInsertAtBack) {
  {
    ByteString empty;
    EXPECT_EQ(1u, empty.InsertAtFront('D'));
    EXPECT_EQ("D", empty);
    EXPECT_EQ(2u, empty.InsertAtFront('E'));
    EXPECT_EQ("ED", empty);
    EXPECT_EQ(3u, empty.InsertAtFront('R'));
    EXPECT_EQ("RED", empty);
    EXPECT_EQ(4u, empty.InsertAtFront('F'));
    EXPECT_EQ("FRED", empty);
  }
  {
    ByteString empty;
    EXPECT_EQ(1u, empty.InsertAtBack('F'));
    EXPECT_EQ("F", empty);
    EXPECT_EQ(2u, empty.InsertAtBack('R'));
    EXPECT_EQ("FR", empty);
    EXPECT_EQ(3u, empty.InsertAtBack('E'));
    EXPECT_EQ("FRE", empty);
    EXPECT_EQ(4u, empty.InsertAtBack('D'));
    EXPECT_EQ("FRED", empty);
  }
  {
    ByteString empty;
    EXPECT_EQ(1u, empty.InsertAtBack('E'));
    EXPECT_EQ("E", empty);
    EXPECT_EQ(2u, empty.InsertAtFront('R'));
    EXPECT_EQ("RE", empty);
    EXPECT_EQ(3u, empty.InsertAtBack('D'));
    EXPECT_EQ("RED", empty);
    EXPECT_EQ(4u, empty.InsertAtFront('F'));
    EXPECT_EQ("FRED", empty);
  }
}

TEST(ByteString, Delete) {
  ByteString fred("FRED");
  EXPECT_EQ(4u, fred.Delete(0, 0));
  EXPECT_EQ("FRED", fred);
  EXPECT_EQ(2u, fred.Delete(0, 2));
  EXPECT_EQ("ED", fred);
  EXPECT_EQ(1u, fred.Delete(1));
  EXPECT_EQ("E", fred);
  EXPECT_EQ(0u, fred.Delete(0));
  EXPECT_EQ("", fred);
  EXPECT_EQ(0u, fred.Delete(0));
  EXPECT_EQ("", fred);

  ByteString empty;
  EXPECT_EQ(0u, empty.Delete(0));
  EXPECT_EQ("", empty);
  EXPECT_EQ(0u, empty.Delete(1));
  EXPECT_EQ("", empty);
}

TEST(ByteString, Mid) {
  ByteString fred("FRED");
  EXPECT_EQ("", fred.Mid(0, 0));
  EXPECT_EQ("", fred.Mid(3, 0));
  EXPECT_EQ("FRED", fred.Mid(0, 4));
  EXPECT_EQ("RED", fred.Mid(1, 3));
  EXPECT_EQ("ED", fred.Mid(2, 2));
  EXPECT_EQ("D", fred.Mid(3, 1));
  EXPECT_EQ("F", fred.Mid(0, 1));
  EXPECT_EQ("R", fred.Mid(1, 1));
  EXPECT_EQ("E", fred.Mid(2, 1));
  EXPECT_EQ("D", fred.Mid(3, 1));
  EXPECT_EQ("FR", fred.Mid(0, 2));
  EXPECT_EQ("FRED", fred.Mid(0, 4));
  EXPECT_EQ("", fred.Mid(0, 10));

  EXPECT_EQ("RED", fred.Mid(1, 3));
  EXPECT_EQ("", fred.Mid(4, 1));

  ByteString empty;
  EXPECT_EQ("", empty.Mid(0, 0));
}

TEST(ByteString, Left) {
  ByteString fred("FRED");
  EXPECT_EQ("", fred.Left(0));
  EXPECT_EQ("F", fred.Left(1));
  EXPECT_EQ("FR", fred.Left(2));
  EXPECT_EQ("FRE", fred.Left(3));
  EXPECT_EQ("FRED", fred.Left(4));

  EXPECT_EQ("", fred.Left(5));

  ByteString empty;
  EXPECT_EQ("", empty.Left(0));
  EXPECT_EQ("", empty.Left(1));
}

TEST(ByteString, Right) {
  ByteString fred("FRED");
  EXPECT_EQ("", fred.Right(0));
  EXPECT_EQ("D", fred.Right(1));
  EXPECT_EQ("ED", fred.Right(2));
  EXPECT_EQ("RED", fred.Right(3));
  EXPECT_EQ("FRED", fred.Right(4));

  EXPECT_EQ("", fred.Right(5));

  ByteString empty;
  EXPECT_EQ("", empty.Right(0));
  EXPECT_EQ("", empty.Right(1));
}

TEST(ByteString, Find) {
  ByteString null_string;
  EXPECT_FALSE(null_string.Find('a').has_value());
  EXPECT_FALSE(null_string.Find('\0').has_value());

  ByteString empty_string("");
  EXPECT_FALSE(empty_string.Find('a').has_value());
  EXPECT_FALSE(empty_string.Find('\0').has_value());

  Optional<size_t> result;
  ByteString single_string("a");
  result = single_string.Find('a');
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(0u, result.value());
  EXPECT_FALSE(single_string.Find('b').has_value());
  EXPECT_FALSE(single_string.Find('\0').has_value());

  ByteString longer_string("abccc");
  result = longer_string.Find('a');
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(0u, result.value());
  result = longer_string.Find('c');
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(2u, result.value());
  result = longer_string.Find('c', 3);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(3u, result.value());
  EXPECT_FALSE(longer_string.Find('d').has_value());
  EXPECT_FALSE(longer_string.Find('\0').has_value());

  result = longer_string.Find("ab");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(0u, result.value());
  result = longer_string.Find("ccc");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(2u, result.value());
  result = longer_string.Find("cc", 3);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(3u, result.value());
  EXPECT_FALSE(longer_string.Find("d").has_value());

  ByteString hibyte_string(
      "ab\x8c"
      "def");
  result = hibyte_string.Find('\x8c');
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(2u, result.value());
}

TEST(ByteString, ReverseFind) {
  ByteString null_string;
  EXPECT_FALSE(null_string.ReverseFind('a').has_value());
  EXPECT_FALSE(null_string.ReverseFind('\0').has_value());

  ByteString empty_string("");
  EXPECT_FALSE(empty_string.ReverseFind('a').has_value());
  EXPECT_FALSE(empty_string.ReverseFind('\0').has_value());

  Optional<size_t> result;
  ByteString single_string("a");
  result = single_string.ReverseFind('a');
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(0u, result.value());
  EXPECT_FALSE(single_string.ReverseFind('b').has_value());
  EXPECT_FALSE(single_string.ReverseFind('\0').has_value());

  ByteString longer_string("abccc");
  result = longer_string.ReverseFind('a');
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(0u, result.value());
  result = longer_string.ReverseFind('c');
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(4u, result.value());
  EXPECT_FALSE(longer_string.ReverseFind('\0').has_value());

  ByteString hibyte_string(
      "ab\x8c"
      "def");
  result = hibyte_string.ReverseFind('\x8c');
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(2u, result.value());
}

TEST(ByteString, UpperLower) {
  ByteString fred("F-Re.42D");
  fred.MakeLower();
  EXPECT_EQ("f-re.42d", fred);
  fred.MakeUpper();
  EXPECT_EQ("F-RE.42D", fred);

  ByteString empty;
  empty.MakeLower();
  EXPECT_EQ("", empty);
  empty.MakeUpper();
  EXPECT_EQ("", empty);
}

TEST(ByteString, Trim) {
  ByteString fred("  FRED  ");
  fred.Trim();
  EXPECT_EQ("FRED", fred);
  fred.Trim('E');
  EXPECT_EQ("FRED", fred);
  fred.Trim('F');
  EXPECT_EQ("RED", fred);
  fred.Trim("ERP");
  EXPECT_EQ("D", fred);

  ByteString blank("   ");
  blank.Trim("ERP");
  EXPECT_EQ("   ", blank);
  blank.Trim('E');
  EXPECT_EQ("   ", blank);
  blank.Trim();
  EXPECT_EQ("", blank);

  ByteString empty;
  empty.Trim("ERP");
  EXPECT_EQ("", empty);
  empty.Trim('E');
  EXPECT_EQ("", empty);
  empty.Trim();
  EXPECT_EQ("", empty);

  ByteString abc("  ABCCBA  ");
  abc.Trim("A");
  EXPECT_EQ("  ABCCBA  ", abc);
  abc.Trim(" A");
  EXPECT_EQ("BCCB", abc);
}

TEST(ByteString, TrimLeft) {
  ByteString fred("  FRED  ");
  fred.TrimLeft();
  EXPECT_EQ("FRED  ", fred);
  fred.TrimLeft('E');
  EXPECT_EQ("FRED  ", fred);
  fred.TrimLeft('F');
  EXPECT_EQ("RED  ", fred);
  fred.TrimLeft("ERP");
  EXPECT_EQ("D  ", fred);

  ByteString blank("   ");
  blank.TrimLeft("ERP");
  EXPECT_EQ("   ", blank);
  blank.TrimLeft('E');
  EXPECT_EQ("   ", blank);
  blank.TrimLeft();
  EXPECT_EQ("", blank);

  ByteString empty;
  empty.TrimLeft("ERP");
  EXPECT_EQ("", empty);
  empty.TrimLeft('E');
  EXPECT_EQ("", empty);
  empty.TrimLeft();
  EXPECT_EQ("", empty);
}

TEST(ByteString, TrimLeftCopies) {
  {
    // With a single reference, no copy takes place.
    ByteString fred("  FRED  ");
    const char* old_buffer = fred.c_str();
    fred.TrimLeft();
    EXPECT_EQ("FRED  ", fred);
    EXPECT_EQ(old_buffer, fred.c_str());
  }
  {
    // With multiple references, we must copy.
    ByteString fred("  FRED  ");
    ByteString other_fred = fred;
    const char* old_buffer = fred.c_str();
    fred.TrimLeft();
    EXPECT_EQ("FRED  ", fred);
    EXPECT_EQ("  FRED  ", other_fred);
    EXPECT_NE(old_buffer, fred.c_str());
  }
  {
    // With multiple references, but no modifications, no copy.
    ByteString fred("FRED");
    ByteString other_fred = fred;
    const char* old_buffer = fred.c_str();
    fred.TrimLeft();
    EXPECT_EQ("FRED", fred);
    EXPECT_EQ("FRED", other_fred);
    EXPECT_EQ(old_buffer, fred.c_str());
  }
}

TEST(ByteString, TrimRight) {
  ByteString fred("  FRED  ");
  fred.TrimRight();
  EXPECT_EQ("  FRED", fred);
  fred.TrimRight('E');
  EXPECT_EQ("  FRED", fred);
  fred.TrimRight('D');
  EXPECT_EQ("  FRE", fred);
  fred.TrimRight("ERP");
  EXPECT_EQ("  F", fred);

  ByteString blank("   ");
  blank.TrimRight("ERP");
  EXPECT_EQ("   ", blank);
  blank.TrimRight('E');
  EXPECT_EQ("   ", blank);
  blank.TrimRight();
  EXPECT_EQ("", blank);

  ByteString empty;
  empty.TrimRight("ERP");
  EXPECT_EQ("", empty);
  empty.TrimRight('E');
  EXPECT_EQ("", empty);
  empty.TrimRight();
  EXPECT_EQ("", empty);
}

TEST(ByteString, TrimRightCopies) {
  {
    // With a single reference, no copy takes place.
    ByteString fred("  FRED  ");
    const char* old_buffer = fred.c_str();
    fred.TrimRight();
    EXPECT_EQ("  FRED", fred);
    EXPECT_EQ(old_buffer, fred.c_str());
  }
  {
    // With multiple references, we must copy.
    ByteString fred("  FRED  ");
    ByteString other_fred = fred;
    const char* old_buffer = fred.c_str();
    fred.TrimRight();
    EXPECT_EQ("  FRED", fred);
    EXPECT_EQ("  FRED  ", other_fred);
    EXPECT_NE(old_buffer, fred.c_str());
  }
  {
    // With multiple references, but no modifications, no copy.
    ByteString fred("FRED");
    ByteString other_fred = fred;
    const char* old_buffer = fred.c_str();
    fred.TrimRight();
    EXPECT_EQ("FRED", fred);
    EXPECT_EQ("FRED", other_fred);
    EXPECT_EQ(old_buffer, fred.c_str());
  }
}

TEST(ByteString, Reserve) {
  {
    ByteString str;
    str.Reserve(6);
    const char* old_buffer = str.c_str();
    str += "ABCDEF";
    EXPECT_EQ(old_buffer, str.c_str());
    str += "Blah Blah Blah Blah Blah Blah";
    EXPECT_NE(old_buffer, str.c_str());
  }
  {
    ByteString str("A");
    str.Reserve(6);
    const char* old_buffer = str.c_str();
    str += "BCDEF";
    EXPECT_EQ(old_buffer, str.c_str());
    str += "Blah Blah Blah Blah Blah Blah";
    EXPECT_NE(old_buffer, str.c_str());
  }
}

TEST(ByteString, GetBuffer) {
  ByteString str1;
  {
    pdfium::span<char> buffer = str1.GetBuffer(12);
    // NOLINTNEXTLINE(runtime/printf)
    strcpy(buffer.data(), "clams");
  }
  str1.ReleaseBuffer(str1.GetStringLength());
  EXPECT_EQ("clams", str1);

  ByteString str2("cl");
  {
    pdfium::span<char> buffer = str2.GetBuffer(12);
    // NOLINTNEXTLINE(runtime/printf)
    strcpy(&buffer[2], "ams");
  }
  str2.ReleaseBuffer(str2.GetStringLength());
  EXPECT_EQ("clams", str2);
}

TEST(ByteString, ReleaseBuffer) {
  {
    ByteString str;
    str.Reserve(12);
    str += "clams";
    const char* old_buffer = str.c_str();
    str.ReleaseBuffer(4);
    EXPECT_EQ(old_buffer, str.c_str());
    EXPECT_EQ("clam", str);
  }
  {
    ByteString str("c");
    str.Reserve(12);
    str += "lams";
    const char* old_buffer = str.c_str();
    str.ReleaseBuffer(4);
    EXPECT_EQ(old_buffer, str.c_str());
    EXPECT_EQ("clam", str);
  }
  {
    ByteString str;
    str.Reserve(200);
    str += "clams";
    const char* old_buffer = str.c_str();
    str.ReleaseBuffer(4);
    EXPECT_NE(old_buffer, str.c_str());
    EXPECT_EQ("clam", str);
  }
  {
    ByteString str("c");
    str.Reserve(200);
    str += "lams";
    const char* old_buffer = str.c_str();
    str.ReleaseBuffer(4);
    EXPECT_NE(old_buffer, str.c_str());
    EXPECT_EQ("clam", str);
  }
}

TEST(ByteString, EmptyReverseIterator) {
  ByteString empty;
  auto iter = empty.rbegin();
  EXPECT_TRUE(iter == empty.rend());
  EXPECT_FALSE(iter != empty.rend());
  EXPECT_FALSE(iter < empty.rend());
}

TEST(ByteString, OneCharReverseIterator) {
  ByteString one_str("a");
  auto iter = one_str.rbegin();
  EXPECT_FALSE(iter == one_str.rend());
  EXPECT_TRUE(iter != one_str.rend());
  EXPECT_TRUE(iter < one_str.rend());

  char ch = *iter++;
  EXPECT_EQ('a', ch);
  EXPECT_TRUE(iter == one_str.rend());
  EXPECT_FALSE(iter != one_str.rend());
  EXPECT_FALSE(iter < one_str.rend());
}

TEST(ByteString, MultiCharReverseIterator) {
  ByteString multi_str("abcd");
  auto iter = multi_str.rbegin();
  EXPECT_FALSE(iter == multi_str.rend());

  char ch = *iter++;
  EXPECT_EQ('d', ch);
  EXPECT_EQ('c', *iter);
  EXPECT_FALSE(iter == multi_str.rend());

  ch = *(++iter);
  EXPECT_EQ('b', ch);
  EXPECT_EQ('b', *iter);
  EXPECT_FALSE(iter == multi_str.rend());

  ch = *iter++;
  EXPECT_EQ('b', ch);
  EXPECT_EQ('a', *iter);
  EXPECT_FALSE(iter == multi_str.rend());

  ch = *iter++;
  EXPECT_EQ('a', ch);
  EXPECT_TRUE(iter == multi_str.rend());

  ch = *(--iter);
  EXPECT_EQ('a', ch);
  EXPECT_EQ('a', *iter);
  EXPECT_FALSE(iter == multi_str.rend());

  ch = *iter--;
  EXPECT_EQ('a', ch);
  EXPECT_EQ('b', *iter);
  EXPECT_FALSE(iter == multi_str.rend());

  ch = *iter--;
  EXPECT_EQ('b', ch);
  EXPECT_EQ('c', *iter);
  EXPECT_FALSE(iter == multi_str.rend());

  ch = *(--iter);
  EXPECT_EQ('d', ch);
  EXPECT_EQ('d', *iter);
  EXPECT_TRUE(iter == multi_str.rbegin());
}

TEST(ByteStringView, Null) {
  ByteStringView null_string;
  EXPECT_FALSE(null_string.raw_str());
  EXPECT_EQ(0u, null_string.GetLength());
  EXPECT_TRUE(null_string.IsEmpty());

  ByteStringView another_null_string;
  EXPECT_EQ(null_string, another_null_string);

  ByteStringView copied_null_string(null_string);
  EXPECT_FALSE(copied_null_string.raw_str());
  EXPECT_EQ(0u, copied_null_string.GetLength());
  EXPECT_TRUE(copied_null_string.IsEmpty());
  EXPECT_EQ(null_string, copied_null_string);

  ByteStringView span_null_string = pdfium::span<const uint8_t>();
  EXPECT_FALSE(span_null_string.raw_str());
  EXPECT_EQ(0u, span_null_string.GetLength());
  EXPECT_TRUE(span_null_string.IsEmpty());
  EXPECT_EQ(null_string, span_null_string);

  ByteStringView empty_string("");  // Pointer to NUL, not NULL pointer.
  EXPECT_TRUE(empty_string.raw_str());
  EXPECT_EQ(0u, empty_string.GetLength());
  EXPECT_TRUE(empty_string.IsEmpty());
  EXPECT_EQ(null_string, empty_string);

  ByteStringView assigned_null_string("initially not nullptr");
  assigned_null_string = null_string;
  EXPECT_FALSE(assigned_null_string.raw_str());
  EXPECT_EQ(0u, assigned_null_string.GetLength());
  EXPECT_TRUE(assigned_null_string.IsEmpty());
  EXPECT_EQ(null_string, assigned_null_string);

  ByteStringView assigned_nullptr_string("initially not nullptr");
  assigned_nullptr_string = nullptr;
  EXPECT_FALSE(assigned_nullptr_string.raw_str());
  EXPECT_EQ(0u, assigned_nullptr_string.GetLength());
  EXPECT_TRUE(assigned_nullptr_string.IsEmpty());
  EXPECT_EQ(null_string, assigned_nullptr_string);

  ByteStringView assigned_span_null_string("initially not null span");
  assigned_span_null_string = pdfium::span<const uint8_t>();
  EXPECT_FALSE(assigned_span_null_string.raw_str());
  EXPECT_EQ(0u, assigned_span_null_string.GetLength());
  EXPECT_TRUE(assigned_span_null_string.IsEmpty());
  EXPECT_EQ(null_string, assigned_span_null_string);

  ByteStringView non_null_string("a");
  EXPECT_NE(null_string, non_null_string);
}

TEST(ByteStringView, NotNull) {
  ByteStringView string3("abc");
  ByteStringView string6("abcdef");
  ByteStringView alternate_string3("abcdef", 3);
  ByteStringView span_string4(pdfium::as_bytes(pdfium::make_span("abcd", 4)));
  ByteStringView embedded_nul_string7("abc\0def", 7);
  ByteStringView illegal_string7("abcdef", 7);

  EXPECT_EQ(3u, string3.GetLength());
  EXPECT_EQ(6u, string6.GetLength());
  EXPECT_EQ(3u, alternate_string3.GetLength());
  EXPECT_EQ(4u, span_string4.GetLength());
  EXPECT_EQ(7u, embedded_nul_string7.GetLength());
  EXPECT_EQ(7u, illegal_string7.GetLength());

  EXPECT_NE(string3, string6);
  EXPECT_EQ(string3, alternate_string3);
  EXPECT_NE(string3, embedded_nul_string7);
  EXPECT_NE(string3, illegal_string7);
  EXPECT_NE(string6, alternate_string3);
  EXPECT_NE(string6, embedded_nul_string7);
  EXPECT_NE(string6, illegal_string7);
  EXPECT_NE(alternate_string3, embedded_nul_string7);
  EXPECT_NE(alternate_string3, illegal_string7);
  EXPECT_NE(embedded_nul_string7, illegal_string7);

  ByteStringView copied_string3(string3);
  ByteStringView copied_alternate_string3(alternate_string3);
  ByteStringView copied_embedded_nul_string7(embedded_nul_string7);

  EXPECT_EQ(string3, copied_string3);
  EXPECT_EQ(alternate_string3, copied_alternate_string3);
  EXPECT_EQ(embedded_nul_string7, copied_embedded_nul_string7);

  ByteStringView assigned_string3("intially something else");
  ByteStringView assigned_alternate_string3("initally something else");
  ByteStringView assigned_ptr_string3("initially something else");
  ByteStringView assigned_embedded_nul_string7("initially something else");

  assigned_string3 = string3;
  assigned_alternate_string3 = alternate_string3;
  assigned_ptr_string3 = "abc";
  assigned_embedded_nul_string7 = embedded_nul_string7;
  EXPECT_EQ(string3, assigned_string3);
  EXPECT_EQ(alternate_string3, assigned_alternate_string3);
  EXPECT_EQ(alternate_string3, assigned_ptr_string3);
  EXPECT_EQ(embedded_nul_string7, assigned_embedded_nul_string7);
}

TEST(ByteStringView, FromChar) {
  ByteStringView null_string;
  ByteStringView lower_a_string("a");

  // Must have lvalues that outlive the corresponding ByteStringC.
  char nul = '\0';
  char lower_a = 'a';
  ByteStringView nul_string_from_char(nul);
  ByteStringView lower_a_string_from_char(lower_a);

  // Pointer to nul, not nullptr ptr, hence length 1 ...
  EXPECT_EQ(1u, nul_string_from_char.GetLength());
  EXPECT_NE(null_string, nul_string_from_char);

  EXPECT_EQ(1u, lower_a_string_from_char.GetLength());
  EXPECT_EQ(lower_a_string, lower_a_string_from_char);
  EXPECT_NE(nul_string_from_char, lower_a_string_from_char);

  ByteStringView longer_string("ab");
  EXPECT_NE(longer_string, lower_a_string_from_char);
}

TEST(ByteStringView, FromVector) {
  std::vector<uint8_t> null_vec;
  ByteStringView null_string(null_vec);
  EXPECT_EQ(0u, null_string.GetLength());

  std::vector<uint8_t> lower_a_vec(10, static_cast<uint8_t>('a'));
  ByteStringView lower_a_string(lower_a_vec);
  EXPECT_EQ(static_cast<size_t>(10), lower_a_string.GetLength());
  EXPECT_EQ("aaaaaaaaaa", lower_a_string);

  std::vector<uint8_t> cleared_vec;
  cleared_vec.push_back(42);
  cleared_vec.pop_back();
  ByteStringView cleared_string(cleared_vec);
  EXPECT_EQ(0u, cleared_string.GetLength());
  EXPECT_EQ(nullptr, cleared_string.raw_str());
}

TEST(ByteStringView, GetID) {
  ByteStringView null_string;
  EXPECT_EQ(0u, null_string.GetID());

  ByteStringView empty_string("");
  EXPECT_EQ(0u, empty_string.GetID());

  ByteStringView short_string("ab");
  EXPECT_EQ(FXBSTR_ID('a', 'b', 0, 0), short_string.GetID());

  ByteStringView longer_string("abcdef");
  EXPECT_EQ(FXBSTR_ID('a', 'b', 'c', 'd'), longer_string.GetID());
}

TEST(ByteStringView, Find) {
  ByteStringView null_string;
  EXPECT_FALSE(null_string.Find('a').has_value());
  EXPECT_FALSE(null_string.Find('\0').has_value());

  ByteStringView empty_string("");
  EXPECT_FALSE(empty_string.Find('a').has_value());
  EXPECT_FALSE(empty_string.Find('\0').has_value());

  Optional<size_t> result;
  ByteStringView single_string("a");
  result = single_string.Find('a');
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(0u, result.value());
  EXPECT_FALSE(single_string.Find('b').has_value());
  EXPECT_FALSE(single_string.Find('\0').has_value());

  ByteStringView longer_string("abccc");
  result = longer_string.Find('a');
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(0u, result.value());
  result = longer_string.Find('c');
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(2u, result.value());
  EXPECT_FALSE(longer_string.Find('d').has_value());
  EXPECT_FALSE(longer_string.Find('\0').has_value());

  ByteStringView hibyte_string(
      "ab\x8c"
      "def");
  result = hibyte_string.Find('\x8c');
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(2u, result.value());
}

TEST(ByteStringView, Mid) {
  ByteStringView null_string;
  EXPECT_EQ(null_string, null_string.Mid(0, 1));
  EXPECT_EQ(null_string, null_string.Mid(1, 1));

  ByteStringView empty_string("");
  EXPECT_EQ("", empty_string.Mid(0, 1));
  EXPECT_EQ("", empty_string.Mid(1, 1));

  ByteStringView single_character("a");
  EXPECT_EQ("", single_character.Mid(0, 0));
  EXPECT_EQ(single_character, single_character.Mid(0, 1));
  EXPECT_EQ("", single_character.Mid(1, 0));
  EXPECT_EQ("", single_character.Mid(1, 1));

  ByteStringView longer_string("abcdef");
  EXPECT_EQ(longer_string, longer_string.Mid(0, 6));
  EXPECT_EQ("", longer_string.Mid(0, 187));

  ByteStringView leading_substring("ab");
  EXPECT_EQ(leading_substring, longer_string.Mid(0, 2));

  ByteStringView middle_substring("bcde");
  EXPECT_EQ(middle_substring, longer_string.Mid(1, 4));

  ByteStringView trailing_substring("ef");
  EXPECT_EQ(trailing_substring, longer_string.Mid(4, 2));
  EXPECT_EQ("", longer_string.Mid(4, 3));
}

TEST(ByteStringView, TrimmedRight) {
  ByteStringView fred("FRED");
  EXPECT_EQ("FRED", fred.TrimmedRight('E'));
  EXPECT_EQ("FRE", fred.TrimmedRight('D'));
  ByteStringView fredd("FREDD");
  EXPECT_EQ("FRE", fred.TrimmedRight('D'));
}

TEST(ByteStringView, ElementAccess) {
  // ByteStringView includes the NUL terminator for non-empty strings.
  ByteStringView abc("abc");
  EXPECT_EQ('a', static_cast<char>(abc[0]));
  EXPECT_EQ('b', static_cast<char>(abc[1]));
  EXPECT_EQ('c', static_cast<char>(abc[2]));
#ifndef NDEBUG
  EXPECT_DEATH({ abc[3]; }, ".*");
#endif
}

TEST(ByteStringView, OperatorLT) {
  ByteStringView empty;
  ByteStringView a("a");
  ByteStringView abc("abc");
  ByteStringView def("def");
  const char* const c_null = nullptr;
  const char* const c_empty = "";
  const char* const c_a = "a";
  const char* const c_abc = "abc";
  const char* const c_def = "def";

  EXPECT_FALSE(empty < empty);
  EXPECT_FALSE(a < a);
  EXPECT_FALSE(abc < abc);
  EXPECT_FALSE(def < def);
  EXPECT_FALSE(c_null < empty);
  EXPECT_FALSE(c_empty < empty);
  EXPECT_FALSE(c_a < a);
  EXPECT_FALSE(c_abc < abc);
  EXPECT_FALSE(c_def < def);
  EXPECT_FALSE(empty < c_null);
  EXPECT_FALSE(empty < c_empty);
  EXPECT_FALSE(a < c_a);
  EXPECT_FALSE(abc < c_abc);
  EXPECT_FALSE(def < c_def);

  EXPECT_TRUE(empty < a);
  EXPECT_FALSE(a < empty);
  EXPECT_TRUE(empty < c_a);
  EXPECT_FALSE(a < c_null);
  EXPECT_FALSE(a < c_empty);

  EXPECT_TRUE(empty < abc);
  EXPECT_FALSE(abc < empty);
  EXPECT_TRUE(empty < c_abc);
  EXPECT_FALSE(abc < c_null);
  EXPECT_FALSE(abc < c_empty);

  EXPECT_TRUE(empty < def);
  EXPECT_FALSE(def < empty);
  EXPECT_TRUE(empty < c_def);
  EXPECT_FALSE(def < c_null);
  EXPECT_FALSE(def < c_empty);

  EXPECT_TRUE(a < abc);
  EXPECT_FALSE(abc < a);
  EXPECT_TRUE(a < c_abc);
  EXPECT_FALSE(abc < c_a);

  EXPECT_TRUE(a < def);
  EXPECT_FALSE(def < a);
  EXPECT_TRUE(a < c_def);
  EXPECT_FALSE(def < c_a);

  EXPECT_TRUE(abc < def);
  EXPECT_FALSE(def < abc);
  EXPECT_TRUE(abc < c_def);
  EXPECT_FALSE(def < c_abc);
}

TEST(ByteStringView, OperatorEQ) {
  ByteStringView byte_string_c("hello");
  EXPECT_TRUE(byte_string_c == byte_string_c);

  ByteStringView byte_string_c_same1("hello");
  EXPECT_TRUE(byte_string_c == byte_string_c_same1);
  EXPECT_TRUE(byte_string_c_same1 == byte_string_c);

  ByteStringView byte_string_c_same2(byte_string_c);
  EXPECT_TRUE(byte_string_c == byte_string_c_same2);
  EXPECT_TRUE(byte_string_c_same2 == byte_string_c);

  ByteStringView byte_string_c1("he");
  ByteStringView byte_string_c2("hellp");
  ByteStringView byte_string_c3("hellod");
  EXPECT_FALSE(byte_string_c == byte_string_c1);
  EXPECT_FALSE(byte_string_c == byte_string_c2);
  EXPECT_FALSE(byte_string_c == byte_string_c3);
  EXPECT_FALSE(byte_string_c1 == byte_string_c);
  EXPECT_FALSE(byte_string_c2 == byte_string_c);
  EXPECT_FALSE(byte_string_c3 == byte_string_c);

  ByteString byte_string_same1("hello");
  EXPECT_TRUE(byte_string_c == byte_string_same1);
  EXPECT_TRUE(byte_string_same1 == byte_string_c);

  ByteString byte_string1("he");
  ByteString byte_string2("hellp");
  ByteString byte_string3("hellod");
  EXPECT_FALSE(byte_string_c == byte_string1);
  EXPECT_FALSE(byte_string_c == byte_string2);
  EXPECT_FALSE(byte_string_c == byte_string3);
  EXPECT_FALSE(byte_string1 == byte_string_c);
  EXPECT_FALSE(byte_string2 == byte_string_c);
  EXPECT_FALSE(byte_string3 == byte_string_c);

  const char* const c_string_same1 = "hello";
  EXPECT_TRUE(byte_string_c == c_string_same1);
  EXPECT_TRUE(c_string_same1 == byte_string_c);

  const char* const c_string1 = "he";
  const char* const c_string2 = "hellp";
  const char* const c_string3 = "hellod";
  EXPECT_FALSE(byte_string_c == c_string1);
  EXPECT_FALSE(byte_string_c == c_string2);
  EXPECT_FALSE(byte_string_c == c_string3);

  EXPECT_FALSE(c_string1 == byte_string_c);
  EXPECT_FALSE(c_string2 == byte_string_c);
  EXPECT_FALSE(c_string3 == byte_string_c);

  pdfium::span<const uint8_t> span5(
      pdfium::as_bytes(pdfium::make_span("hello", 5)));
  EXPECT_EQ(byte_string_c.span(), span5);
}

TEST(ByteStringView, OperatorNE) {
  ByteStringView byte_string_c("hello");
  EXPECT_FALSE(byte_string_c != byte_string_c);

  ByteStringView byte_string_c_same1("hello");
  EXPECT_FALSE(byte_string_c != byte_string_c_same1);
  EXPECT_FALSE(byte_string_c_same1 != byte_string_c);

  ByteStringView byte_string_c_same2(byte_string_c);
  EXPECT_FALSE(byte_string_c != byte_string_c_same2);
  EXPECT_FALSE(byte_string_c_same2 != byte_string_c);

  ByteStringView byte_string_c1("he");
  ByteStringView byte_string_c2("hellp");
  ByteStringView byte_string_c3("hellod");
  EXPECT_TRUE(byte_string_c != byte_string_c1);
  EXPECT_TRUE(byte_string_c != byte_string_c2);
  EXPECT_TRUE(byte_string_c != byte_string_c3);
  EXPECT_TRUE(byte_string_c1 != byte_string_c);
  EXPECT_TRUE(byte_string_c2 != byte_string_c);
  EXPECT_TRUE(byte_string_c3 != byte_string_c);

  ByteString byte_string_same1("hello");
  EXPECT_FALSE(byte_string_c != byte_string_same1);
  EXPECT_FALSE(byte_string_same1 != byte_string_c);

  ByteString byte_string1("he");
  ByteString byte_string2("hellp");
  ByteString byte_string3("hellod");
  EXPECT_TRUE(byte_string_c != byte_string1);
  EXPECT_TRUE(byte_string_c != byte_string2);
  EXPECT_TRUE(byte_string_c != byte_string3);
  EXPECT_TRUE(byte_string1 != byte_string_c);
  EXPECT_TRUE(byte_string2 != byte_string_c);
  EXPECT_TRUE(byte_string3 != byte_string_c);

  const char* const c_string_same1 = "hello";
  EXPECT_FALSE(byte_string_c != c_string_same1);
  EXPECT_FALSE(c_string_same1 != byte_string_c);

  const char* const c_string1 = "he";
  const char* const c_string2 = "hellp";
  const char* const c_string3 = "hellod";
  EXPECT_TRUE(byte_string_c != c_string1);
  EXPECT_TRUE(byte_string_c != c_string2);
  EXPECT_TRUE(byte_string_c != c_string3);

  EXPECT_TRUE(c_string1 != byte_string_c);
  EXPECT_TRUE(c_string2 != byte_string_c);
  EXPECT_TRUE(c_string3 != byte_string_c);
}

TEST(ByteStringView, NullIterator) {
  ByteStringView null_str;
  int32_t sum = 0;
  bool any_present = false;
  for (const auto& c : null_str) {
    sum += c;  // Avoid unused arg warnings.
    any_present = true;
  }
  EXPECT_FALSE(any_present);
  EXPECT_EQ(0, sum);
}

TEST(ByteStringView, EmptyIterator) {
  ByteStringView empty_str("");
  int32_t sum = 0;
  bool any_present = false;
  for (const auto& c : empty_str) {
    any_present = true;
    sum += c;  // Avoid unused arg warnings.
  }
  EXPECT_FALSE(any_present);
  EXPECT_EQ(0, sum);
}

TEST(ByteStringView, OneCharIterator) {
  ByteStringView one_str("a");
  int32_t sum = 0;
  bool any_present = false;
  for (const auto& c : one_str) {
    any_present = true;
    sum += c;  // Avoid unused arg warnings.
  }
  EXPECT_TRUE(any_present);
  EXPECT_EQ('a', sum);
}

TEST(ByteStringView, MultiCharIterator) {
  ByteStringView one_str("abc");
  int32_t sum = 0;
  bool any_present = false;
  for (const auto& c : one_str) {
    any_present = true;
    sum += c;  // Avoid unused arg warnings.
  }
  EXPECT_TRUE(any_present);
  EXPECT_EQ('a' + 'b' + 'c', sum);
}

TEST(ByteStringView, EmptyReverseIterator) {
  ByteStringView empty;
  auto iter = empty.rbegin();
  EXPECT_TRUE(iter == empty.rend());
  EXPECT_FALSE(iter != empty.rend());
  EXPECT_FALSE(iter < empty.rend());
}

TEST(ByteStringView, OneCharReverseIterator) {
  ByteStringView one_str("a");
  auto iter = one_str.rbegin();
  EXPECT_FALSE(iter == one_str.rend());
  EXPECT_TRUE(iter != one_str.rend());
  EXPECT_TRUE(iter < one_str.rend());

  char ch = *iter++;
  EXPECT_EQ('a', ch);
  EXPECT_TRUE(iter == one_str.rend());
  EXPECT_FALSE(iter != one_str.rend());
  EXPECT_FALSE(iter < one_str.rend());
}

TEST(ByteStringView, MultiCharReverseIterator) {
  ByteStringView multi_str("abcd");
  auto iter = multi_str.rbegin();
  EXPECT_FALSE(iter == multi_str.rend());

  char ch = *iter++;
  EXPECT_EQ('d', ch);
  EXPECT_EQ('c', *iter);
  EXPECT_FALSE(iter == multi_str.rend());

  ch = *(++iter);
  EXPECT_EQ('b', ch);
  EXPECT_EQ('b', *iter);
  EXPECT_FALSE(iter == multi_str.rend());

  ch = *iter++;
  EXPECT_EQ('b', ch);
  EXPECT_EQ('a', *iter);
  EXPECT_FALSE(iter == multi_str.rend());

  ch = *iter++;
  EXPECT_EQ('a', ch);
  EXPECT_TRUE(iter == multi_str.rend());

  ch = *(--iter);
  EXPECT_EQ('a', ch);
  EXPECT_EQ('a', *iter);
  EXPECT_FALSE(iter == multi_str.rend());

  ch = *iter--;
  EXPECT_EQ('a', ch);
  EXPECT_EQ('b', *iter);
  EXPECT_FALSE(iter == multi_str.rend());

  ch = *iter--;
  EXPECT_EQ('b', ch);
  EXPECT_EQ('c', *iter);
  EXPECT_FALSE(iter == multi_str.rend());

  ch = *(--iter);
  EXPECT_EQ('d', ch);
  EXPECT_EQ('d', *iter);
  EXPECT_TRUE(iter == multi_str.rbegin());
}

TEST(ByteStringView, AnyAllNoneOf) {
  ByteStringView str("aaaaaaaaaaaaaaaaab");
  EXPECT_FALSE(std::all_of(str.begin(), str.end(),
                           [](const char& c) { return c == 'a'; }));

  EXPECT_FALSE(std::none_of(str.begin(), str.end(),
                            [](const char& c) { return c == 'a'; }));

  EXPECT_TRUE(std::any_of(str.begin(), str.end(),
                          [](const char& c) { return c == 'a'; }));

  EXPECT_TRUE(pdfium::ContainsValue(str, 'a'));
  EXPECT_TRUE(pdfium::ContainsValue(str, 'b'));
  EXPECT_FALSE(pdfium::ContainsValue(str, 'z'));
}

TEST(ByteString, FormatWidth) {
  EXPECT_EQ("    1", ByteString::Format("%5d", 1));
  EXPECT_EQ("1", ByteString::Format("%d", 1));
  EXPECT_EQ("    1", ByteString::Format("%*d", 5, 1));
  EXPECT_EQ("1", ByteString::Format("%-1d", 1));
  EXPECT_EQ("1", ByteString::Format("%0d", 1));
}

TEST(ByteString, FormatPrecision) {
  EXPECT_EQ("1.12", ByteString::Format("%.2f", 1.12345));
  EXPECT_EQ("1.123", ByteString::Format("%.*f", 3, 1.12345));
  EXPECT_EQ("1.123450", ByteString::Format("%f", 1.12345));
  EXPECT_EQ("1.123450", ByteString::Format("%-1f", 1.12345));
  EXPECT_EQ("1.123450", ByteString::Format("%0f", 1.12345));
}

TEST(ByteString, Empty) {
  ByteString empty_str;
  EXPECT_TRUE(empty_str.IsEmpty());
  EXPECT_EQ(0u, empty_str.GetLength());

  const char* cstr = empty_str.c_str();
  EXPECT_NE(nullptr, cstr);
  EXPECT_EQ(0u, strlen(cstr));

  const uint8_t* rstr = empty_str.raw_str();
  EXPECT_EQ(nullptr, rstr);

  pdfium::span<const char> cspan = empty_str.AsSpan();
  EXPECT_TRUE(cspan.empty());
  EXPECT_EQ(nullptr, cspan.data());

  pdfium::span<const uint8_t> rspan = empty_str.AsRawSpan();
  EXPECT_TRUE(rspan.empty());
  EXPECT_EQ(nullptr, rspan.data());
}

TEST(ByteString, InitializerList) {
  ByteString many_str({"clams", " and ", "oysters"});
  EXPECT_EQ("clams and oysters", many_str);
  many_str = {"fish", " and ", "chips", " and ", "soda"};
  EXPECT_EQ("fish and chips and soda", many_str);
}

TEST(ByteString, NullIterator) {
  ByteString null_str;
  int32_t sum = 0;
  bool any_present = false;
  for (const auto& c : null_str) {
    sum += c;  // Avoid unused arg warnings.
    any_present = true;
  }
  EXPECT_FALSE(any_present);
  EXPECT_EQ(0, sum);
}

TEST(ByteString, EmptyIterator) {
  ByteString empty_str("");
  int32_t sum = 0;
  bool any_present = false;
  for (const auto& c : empty_str) {
    any_present = true;
    sum += c;  // Avoid unused arg warnings.
  }
  EXPECT_FALSE(any_present);
  EXPECT_EQ(0, sum);
}

TEST(ByteString, OneCharIterator) {
  ByteString one_str("a");
  int32_t sum = 0;
  bool any_present = false;
  for (const auto& c : one_str) {
    any_present = true;
    sum += c;  // Avoid unused arg warnings.
  }
  EXPECT_TRUE(any_present);
  EXPECT_EQ('a', sum);
}

TEST(ByteString, MultiCharIterator) {
  ByteString one_str("abc");
  int32_t sum = 0;
  bool any_present = false;
  for (const auto& c : one_str) {
    any_present = true;
    sum += c;  // Avoid unused arg warnings.
  }
  EXPECT_TRUE(any_present);
  EXPECT_EQ('a' + 'b' + 'c', sum);
}

TEST(ByteString, StdBegin) {
  ByteString one_str("abc");
  std::vector<uint8_t> vec(std::begin(one_str), std::end(one_str));
  ASSERT_EQ(3u, vec.size());
  EXPECT_EQ('a', vec[0]);
  EXPECT_EQ('b', vec[1]);
  EXPECT_EQ('c', vec[2]);
}

TEST(ByteString, AnyAllNoneOf) {
  ByteString str("aaaaaaaaaaaaaaaaab");
  EXPECT_FALSE(std::all_of(str.begin(), str.end(),
                           [](const char& c) { return c == 'a'; }));

  EXPECT_FALSE(std::none_of(str.begin(), str.end(),
                            [](const char& c) { return c == 'a'; }));

  EXPECT_TRUE(std::any_of(str.begin(), str.end(),
                          [](const char& c) { return c == 'a'; }));

  EXPECT_TRUE(pdfium::ContainsValue(str, 'a'));
  EXPECT_TRUE(pdfium::ContainsValue(str, 'b'));
  EXPECT_FALSE(pdfium::ContainsValue(str, 'z'));
}

TEST(CFX_BytrString, EqualNoCase) {
  ByteString str("aaa");
  EXPECT_TRUE(str.EqualNoCase("aaa"));
  EXPECT_TRUE(str.EqualNoCase("AAA"));
  EXPECT_TRUE(str.EqualNoCase("aaA"));
  EXPECT_TRUE(str.EqualNoCase("Aaa"));
  EXPECT_FALSE(str.EqualNoCase("aab"));
  EXPECT_FALSE(str.EqualNoCase("aaaa"));
  EXPECT_FALSE(str.EqualNoCase("BBBB"));
  EXPECT_FALSE(str.EqualNoCase("a"));
  EXPECT_FALSE(str.EqualNoCase(""));
}

TEST(ByteString, OStreamOverload) {
  std::ostringstream stream;

  // Basic case, empty string
  ByteString str;
  stream << str;
  EXPECT_EQ("", stream.str());

  // Basic case, non-empty string
  str = "def";
  stream.str("");
  stream << "abc" << str << "ghi";
  EXPECT_EQ("abcdefghi", stream.str());

  // Changing the ByteString does not change the stream it was written to.
  str = "123";
  EXPECT_EQ("abcdefghi", stream.str());

  // Writing it again to the stream will use the latest value.
  stream.str("");
  stream << "abc" << str << "ghi";
  EXPECT_EQ("abc123ghi", stream.str());

  char stringWithNulls[]{'x', 'y', '\0', 'z'};

  // Writing a ByteString with nulls and no specified length treats it as
  // a C-style null-terminated string.
  str = ByteString(stringWithNulls);
  EXPECT_EQ(2u, str.GetLength());
  stream.str("");
  stream << str;
  EXPECT_EQ(2u, stream.tellp());

  // Writing a ByteString with nulls but specifying its length treats it as
  // a C++-style string.
  str = ByteString(stringWithNulls, 4);
  EXPECT_EQ(4u, str.GetLength());
  stream.str("");
  stream << str;
  EXPECT_EQ(4u, stream.tellp());

  // << operators can be chained.
  ByteString str1("abc");
  ByteString str2("def");
  stream.str("");
  stream << str1 << str2;
  EXPECT_EQ("abcdef", stream.str());
}

TEST(ByteStringView, OStreamOverload) {
  // Basic case, empty string
  {
    std::ostringstream stream;
    ByteStringView str;
    stream << str;
    EXPECT_EQ("", stream.str());
  }

  // Basic case, non-empty string
  {
    std::ostringstream stream;
    ByteStringView str("def");
    stream << "abc" << str << "ghi";
    EXPECT_EQ("abcdefghi", stream.str());
  }

  // Changing the ByteStringView does not change the stream it was written to.
  {
    std::ostringstream stream;
    ByteStringView str("abc");
    stream << str;
    str = "123";
    EXPECT_EQ("abc", stream.str());
  }

  // Writing it again to the stream will use the latest value.
  {
    std::ostringstream stream;
    ByteStringView str("abc");
    stream << str;
    stream.str("");
    str = "123";
    stream << str;
    EXPECT_EQ("123", stream.str());
  }

  // Writing a ByteStringView with nulls and no specified length treats it as
  // a C-style null-terminated string.
  {
    std::ostringstream stream;
    char stringWithNulls[]{'x', 'y', '\0', 'z'};
    ByteStringView str(stringWithNulls);
    EXPECT_EQ(2u, str.GetLength());
    stream << str;
    EXPECT_EQ(2u, stream.tellp());
    str = "";
  }

  // Writing a ByteStringView with nulls but specifying its length treats it as
  // a C++-style string.
  {
    std::ostringstream stream;
    char stringWithNulls[]{'x', 'y', '\0', 'z'};
    ByteStringView str(stringWithNulls, 4);
    EXPECT_EQ(4u, str.GetLength());
    stream << str;
    EXPECT_EQ(4u, stream.tellp());
    str = "";
  }

  // << operators can be chained.
  {
    std::ostringstream stream;
    ByteStringView str1("abc");
    ByteStringView str2("def");
    stream << str1 << str2;
    EXPECT_EQ("abcdef", stream.str());
  }
}

TEST(ByteString, FormatInteger) {
  // Base case of 0.
  EXPECT_EQ("0", ByteString::FormatInteger(0));

  // Positive ordinary number.
  EXPECT_EQ("123456", ByteString::FormatInteger(123456));

  // Negative ordinary number.
  EXPECT_EQ("-123456", ByteString::FormatInteger(-123456));

  // int limits.
  EXPECT_EQ("2147483647", ByteString::FormatInteger(INT_MAX));
  EXPECT_EQ("-2147483648", ByteString::FormatInteger(INT_MIN));
}

TEST(ByteString, FX_HashCode_Ascii) {
  EXPECT_EQ(0u, FX_HashCode_GetA("", false));
  EXPECT_EQ(65u, FX_HashCode_GetA("A", false));
  EXPECT_EQ(97u, FX_HashCode_GetA("A", true));
  EXPECT_EQ(31 * 65u + 66u, FX_HashCode_GetA("AB", false));
  EXPECT_EQ(31u * 65u + 255u, FX_HashCode_GetA("A\xff", false));
  EXPECT_EQ(31u * 97u + 255u, FX_HashCode_GetA("A\xff", true));
}

TEST(ByteString, FX_HashCode_Wide) {
  EXPECT_EQ(0u, FX_HashCode_GetAsIfW("", false));
  EXPECT_EQ(65u, FX_HashCode_GetAsIfW("A", false));
  EXPECT_EQ(97u, FX_HashCode_GetAsIfW("A", true));
  EXPECT_EQ(1313u * 65u + 66u, FX_HashCode_GetAsIfW("AB", false));
  EXPECT_EQ(1313u * 65u + 255u, FX_HashCode_GetAsIfW("A\xff", false));
  EXPECT_EQ(1313u * 97u + 255u, FX_HashCode_GetAsIfW("A\xff", true));
}

}  // namespace fxcrt

// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxjs/xfa/cfxjse_engine.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/xfa_js_embedder_test.h"
#include "xfa/fxfa/cxfa_eventparam.h"

class CFXJSE_FormCalcContextEmbedderTest : public XFAJSEmbedderTest {};

// TODO(dsinclair): Comment out tests are broken and need to be fixed.

TEST_F(CFXJSE_FormCalcContextEmbedderTest, TranslateEmpty) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  const char input[] = "";
  EXPECT_TRUE(Execute(input));
  // TODO(dsinclair): This should probably throw as a blank formcalc script
  // is invalid.
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, TranslateNumber) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  const char input[] = "123";
  EXPECT_TRUE(Execute(input));

  CFXJSE_Value* value = GetValue();
  EXPECT_TRUE(value->IsInteger());
  EXPECT_EQ(123, value->ToInteger()) << "Program: " << input;
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Numeric) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {{"123 + 456", 579},
               {"2 - 3 * 10 / 2 + 7", -6},
               {"10 * 3 + 5 * 4", 50},
               {"(5 - \"abc\") * 3", 15},
               {"\"100\" / 10e1", 1},
               {"5 + null + 3", 8},
               // {"if (\"abc\") then\n"
               //  "  10\n"
               //  "else\n"
               //  "  20\n"
               //  "endif",
               //  20},
               // {"3 / 0 + 1", 0},
               {"-(17)", -17},
               {"-(-17)", 17},
               {"+(17)", 17},
               {"+(-17)", -17},
               {"if (1 < 2) then\n1\nendif", 1},
               {"if (\"abc\" > \"def\") then\n"
                "  1 and 0\n"
                "else\n"
                "  0\n"
                "endif",
                0}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Strings) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {
      {"\"abc\"", "abc"},
      {"concat(\"The total is \", 2, \" dollars and \", 57, \" cents.\")",
       "The total is 2 dollars and 57 cents."}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Booleans) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    bool result;
  } tests[] = {{"0 and 1 or 2 > 1", true},
               {"2 < 3 not 1 == 1", false},
               {"\"abc\" | 2", true},
               {"1 or 0", true},
               {"0 | 0", false},
               {"0 or 1 | 0 or 0", true},
               {"1 and 0", false},
               // {"0 & 0", true},  // TODO(dsinclair) Confirm with Reader.
               {"0 and 1 & 0 and 0", false},
               {"not(\"true\")", true},
               {"not(1)", false},
               {"3 == 3", true},
               {"3 <> 4", true},
               {"\"abc\" eq \"def\"", false},
               {"\"def\" ne \"abc\"", true},
               {"5 + 5 == 10", true},
               {"5 + 5 <> \"10\"", false},
               {"3 < 3", false},
               {"3 > 4", false},
               {"\"abc\" <= \"def\"", true},
               {"\"def\" > \"abc\"", true},
               {"12 >= 12", true},
               {"\"true\" < \"false\"", false}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger()) << "Program: " << tests[i].program;
    EXPECT_EQ(tests[i].result, value->ToBoolean())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Abs) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    float result;
  } tests[] = {{"Abs(1.03)", 1.03f}, {"Abs(-1.03)", 1.03f}, {"Abs(0)", 0.0f}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsNumber());
    EXPECT_FLOAT_EQ(tests[i].result, value->ToFloat())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Avg) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    float result;
  } tests[] = {{"Avg(0, 32, 16)", 16.0f}, {"Avg(2.5, 17, null)", 9.75f}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsNumber());
    EXPECT_FLOAT_EQ(tests[i].result, value->ToFloat())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Ceil) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {{"Ceil(2.5875)", 3}, {"Ceil(-5.9)", -5}, {"Ceil(\"abc\")", 0}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Count) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {{"Count(\"Tony\", \"Blue\", 41)", 3}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Floor) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {{"Floor(21.3409873)", 21},
               {"Floor(5.999965342)", 5},
               {"Floor(3.2 * 15)", 48}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Max) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {{"Max(234, 15, 107)", 234},
               {"Max(\"abc\", 15, \"Tony Blue\")", 15},
               {"Max(\"abc\")", 0}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Min) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {{"Min(234, 15, 107)", 15},
               // TODO(dsinclair): Verify with Reader; I believe this should
               // have a return of 0.
               // {"Min(\"abc\", 15, \"Tony Blue\")", 15},
               {"Min(\"abc\")", 0}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Mod) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {{"Mod(64, -3)", 1}, {"Mod(-13, 3)", -1}, {"Mod(\"abc\", 2)", 0}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Round) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    float result;
  } tests[] = {{"Round(12.389764537, 4)", 12.3898f},
               {"Round(20/3, 2)", 6.67f},
               {"Round(8.9897, \"abc\")", 9.0f},
               {"Round(FV(400, 0.10/12, 30*12), 2)", 904195.17f}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsNumber()) << "Program: " << tests[i].program;
    EXPECT_FLOAT_EQ(tests[i].result, value->ToFloat())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Sum) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {{"Sum(2, 4, 6, 8)", 20},
               {"Sum(-2, 4, -6, 8)", 4},
               {"Sum(4, 16, \"abc\", 19)", 39}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

// TEST_F(CFXJSE_FormCalcContextEmbedderTest, DISABLED_Date) {
//   ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));
//
//   TODO(dsinclair): Make compatible with windows.
//   time_t seconds = time(nullptr);
//   int days = seconds / (60 * 60 * 24);

//   EXPECT_TRUE(Execute("Date()"));

//   CFXJSE_Value* value = GetValue();
//   EXPECT_TRUE(value->IsNumber());
//   EXPECT_EQ(days, value->ToInteger());
// }

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Date2Num) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {
      // {"Date2Num(\"Mar 15, 1996\")", 35138},
      {"Date2Num(\"1/1/1900\", \"D/M/YYYY\")", 1},
      {"Date2Num(\"03/15/96\", \"MM/DD/YY\")", 35138},
      // {"Date2Num(\"Aug 1, 1996\", \"MMM D, YYYY\")", 35277},
      {"Date2Num(\"96-08-20\", \"YY-MM-DD\", \"fr_FR\")", 35296},
      {"Date2Num(\"1/3/00\", \"D/M/YY\") - Date2Num(\"1/2/00\", \"D/M/YY\")",
       29}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, DateFmt) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {
      // {"DateFmt(1)", "M/D/YY"},
      // {"DateFmt(2, \"fr_CA\")", "YY-MM-DD"},
      {"DateFmt(3, \"de_DE\")", "D. MMMM YYYY"},
      // {"DateFmt(4, \"fr_FR\")", "EEE D' MMMM YYYY"}
  };

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, IsoDate2Num) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {{"IsoDate2Num(\"1900\")", 1},
               {"IsoDate2Num(\"1900-01\")", 1},
               {"IsoDate2Num(\"1900-01-01\")", 1},
               {"IsoDate2Num(\"19960315T20:20:20\")", 35138},
               {"IsoDate2Num(\"2000-03-01\") - IsoDate2Num(\"20000201\")", 29}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, DISABLED_IsoTime2Num) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {{"IsoTime2Num(\"00:00:00Z\")", 1}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, LocalDateFmt) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {// {"LocalDateFmt(1, \"de_DE\")", "tt.MM.uu"},
               // {"LocalDateFmt(2, \"fr_CA\")", "aa-MM-jj"},
               {"LocalDateFmt(3, \"de_CH\")", "t. MMMM jjjj"},
               {"LocalDateFmt(4, \"fr_FR\")", "EEEE j MMMM aaaa"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, DISABLED_LocalTimeFmt) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"LocalTimeFmt(1, \"de_DE\")", "HH:mm"},
               {"LocalTimeFmt(2, \"fr_CA\")", "HH:mm::ss"},
               {"LocalTimeFmt(3, \"de_CH\")", "HH:mm:ss z"},
               {"LocalTimeFmt(4, \"fr_FR\")", "HH' h 'mm z"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Num2Date) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {
      {"Num2Date(1, \"DD/MM/YYYY\")", "01/01/1900"},
      {"Num2Date(35139, \"DD-MMM-YYYY\", \"de_DE\")", "16-Mrz-1996"},
      // {"Num2Date(Date2Num(\"Mar 15, 2000\") - Date2Num(\"98-03-15\", "
      //  "\"YY-MM-DD\", \"fr_CA\"))",
      //  "Jan 1, 1902"}
  };

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString()) << "Program: " << tests[i].program;
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, DISABLED_Num2GMTime) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {// Broken on Windows only.
               {"Num2GMTime(1, \"HH:MM:SS\")", "00:00:00"},
               // Below broken on other platforms.
               {"Num2GMTime(65593001, \"HH:MM:SS Z\")", "18:13:13 GMT"},
               {"Num2GMTime(43993001, TimeFmt(4, \"de_DE\"), \"de_DE\")",
                "12.13 Uhr GMT"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

// TODO(dsinclair): Broken on Mac ...
TEST_F(CFXJSE_FormCalcContextEmbedderTest, DISABLED_Num2Time) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Num2Time(1, \"HH:MM:SS\")", "00:00:00"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

// TEST_F(CFXJSE_FormCalcContextEmbedderTest, DISABLED_Time) {
//   ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));
//   TODO(dsinclair): Make compatible with windows.
//   struct timeval tp;
//   gettimeofday(&tp, nullptr);

//   EXPECT_TRUE(Execute("Time()"));

//   CFXJSE_Value* value = GetValue();
//   EXPECT_TRUE(value->IsInteger());
//   EXPECT_EQ(tp.tv_sec * 1000L + tp.tv_usec / 1000, value->ToInteger())
//       << "Program: Time()";
// }

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Time2Num) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {
      // {"Time2Num(\"00:00:00 GMT\", \"HH:MM:SS Z\")", 1},
      {"Time2Num(\"13:13:13 GMT\", \"HH:MM:SS Z\", \"fr_FR\")", 47593001}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, TimeFmt) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {
      // {"TimeFmt(1)", "h::MM A"},
      {"TimeFmt(2, \"fr_CA\")", "HH:MM:SS"},
      {"TimeFmt(3, \"fr_FR\")", "HH:MM:SS Z"},
      // {"TimeFmt(4, \"de_DE\")", "H.MM' Uhr 'Z"}
  };

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, DISABLED_Apr) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    float result;
  } tests[] = {{"Apr(35000, 269.50, 360)", 0.08515404566f},
               {"Apr(210000 * 0.75, 850 + 110, 25 * 26)", 0.07161332404f}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsNumber());
    EXPECT_FLOAT_EQ(tests[i].result, value->ToFloat())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, CTerm) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    float result;
  } tests[] = {
      // {"CTerm(0.02, 1000, 100)", 116.2767474515f},
      {"CTerm(0.10, 500000, 12000)", 39.13224648502f},
      // {"CTerm(0.0275 + 0.0025, 1000000, 55000 * 0.10)", 176.02226044975f}
  };

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsNumber());
    EXPECT_FLOAT_EQ(tests[i].result, value->ToFloat())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, FV) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    float result;
  } tests[] = {{"FV(400, 0.10 / 12, 30 * 12)", 904195.16991842445f},
               {"FV(1000, 0.075 / 4, 10 * 4)", 58791.96145535981f}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsNumber());
    EXPECT_FLOAT_EQ(tests[i].result, value->ToFloat())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, IPmt) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    float result;
  } tests[] = {{"IPmt(30000, 0.085, 295.50, 7, 3)", 624.8839283142f},
               {"IPmt(160000, 0.0475, 980, 24, 12)", 7103.80833569485f},
               {"IPmt(15000, 0.065, 65.50, 15, 1)", 0.0f}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsNumber());
    EXPECT_FLOAT_EQ(tests[i].result, value->ToFloat())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, DISABLED_NPV) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    float result;
  } tests[] = {{"NPV(0.065, 5000)", 4694.83568075117f},
               {"NPV(0.10, 500, 1500, 4000, 10000)", 11529.60863329007f},
               {"NPV(0.0275 / 12, 50, 60, 40, 100, 25)", 273.14193838457f}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsNumber()) << "Program: " << tests[i].program;
    EXPECT_FLOAT_EQ(tests[i].result, value->ToFloat())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Pmt) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    float result;
  } tests[] = {// {"Pmt(150000, 0.0475 / 12, 25 * 12)", 855.17604207164f},
               {"Pmt(25000, 0.085, 12)", 3403.82145169876f}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsNumber());
    EXPECT_FLOAT_EQ(tests[i].result, value->ToFloat())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, PPmt) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    float result;
  } tests[] = {
      {"PPmt(30000, 0.085, 295.50, 7, 3)", 261.6160716858f},
      {"PPmt(160000, 0.0475, 980, 24, 12)", 4656.19166430515f},
      // {"PPmt(15000, 0.065, 65.50, 15, 1)", 0.0f}
  };

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsNumber());
    EXPECT_FLOAT_EQ(tests[i].result, value->ToFloat())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, PV) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    float result;
  } tests[] = {
      {"PV(400, 0.10 / 12, 30 * 12)", 45580.32799074439f},
      // {"PV(1000, 0.075 / 4, 10 * 4)", 58791.96145535981f}
  };

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsNumber());
    EXPECT_FLOAT_EQ(tests[i].result, value->ToFloat())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, DISABLED_Rate) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    float result;
  } tests[] = {{"Rate(12000, 8000, 5)", 0.0844717712f},
               {"Rate(10000, 0.25 * 5000, 4 * 12)", 0.04427378243f}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsNumber());
    EXPECT_FLOAT_EQ(tests[i].result, value->ToFloat())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Term) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    float result;
  } tests[] = {// {"Term(475, .05, 1500)", 3.00477517728f},
               {"Term(2500, 0.0275 + 0.0025, 5000)", 1.97128786369f}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsNumber());
    EXPECT_FLOAT_EQ(tests[i].result, value->ToFloat())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Choose) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {
      {"Choose(3, \"Taxes\", \"Price\", \"Person\", \"Teller\")", "Person"},
      {"Choose(2, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)", "9"},
      {"Choose(20/3, \"A\", \"B\", \"C\", \"D\", \"E\", \"F\", \"G\", \"H\")",
       "F"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Exists) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  EXPECT_TRUE(Execute("Exists(\"hello world\")"));
  CFXJSE_Value* value = GetValue();
  EXPECT_TRUE(value->IsInteger());
  EXPECT_FALSE(value->ToBoolean());
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, HasValue) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    bool result;
  } tests[] = {{"HasValue(2)", true}, {"HasValue(\" \")", false}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger()) << "Program: " << tests[i].program;
    EXPECT_EQ(tests[i].result, value->ToBoolean())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Oneof) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    bool result;
  } tests[] = {
      {"Oneof(3, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)", true},
      {"Oneof(\"John\", \"Bill\", \"Gary\", \"Joan\", \"John\", \"Lisa\")",
       true},
      {"Oneof(3, 1, 25)", false},
      {"Oneof(3, 3, null)", true},
      {"Oneof(3, null, null)", false},
  };

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger()) << "Program: " << tests[i].program;
    EXPECT_EQ(tests[i].result, value->ToBoolean())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Within) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    bool result;
  } tests[] = {{"Within(\"C\", \"A\", \"D\")", true},
               {"Within(1.5, 0, 2)", true},
               {"Within(-1, 0, 2)", false}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger()) << "Program: " << tests[i].program;
    EXPECT_EQ(tests[i].result, value->ToBoolean())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, DISABLED_Eval) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {{"eval(\"10*3+5*4\")", 50}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, DISABLED_Null) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Null()", "null"},
               {"Concat(\"ABC\", Null(), \"DEF\")", "ABCDEF"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }

  EXPECT_TRUE(Execute("Null() + 5"));

  CFXJSE_Value* value = GetValue();
  EXPECT_TRUE(value->IsInteger());
  EXPECT_EQ(5, value->ToInteger());
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Ref) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Ref(\"10*3+5*4\")", "10*3+5*4"}, {"Ref(\"hello\")", "hello"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, UnitType) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"UnitType(\"36 in\")", "in"},
               {"UnitType(\"2.54centimeters\")", "cm"},
               {"UnitType(\"picas\")", "pt"},
               {"UnitType(\"2.cm\")", "cm"},
               {"UnitType(\"2.zero cm\")", "in"},
               {"UnitType(\"kilometers\")", "in"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, UnitValue) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    float result;
  } tests[] = {
      {"UnitValue(\"2in\")", 2.0f}, {"UnitValue(\"2in\", \"cm\")", 5.08f},
      // {"UnitValue(\"6\", \"pt\")", 432f},
      // {"UnitType(\"A\", \"cm\")", 0.0f},
      // {"UnitType(\"5.08cm\", \"kilograms\")", 2.0f}
  };

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsNumber());
    EXPECT_FLOAT_EQ(tests[i].result, value->ToFloat())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, At) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {{"At(\"ABCDEFGH\", \"AB\")", 1},
               {"At(\"ABCDEFGH\", \"F\")", 6},
               {"At(23412931298471, 29)", 5}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Concat) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Concat(\"ABC\", \"DEF\")", "ABCDEF"},
               {"Concat(\"Tony\", Space(1), \"Blue\")", "Tony Blue"},
               {"Concat(\"You owe \", WordNum(1154.67, 2), \".\")",
                "You owe One Thousand One Hundred Fifty-four Dollars And "
                "Sixty-seven Cents."}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Decode) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {
      // HTML
      {R"(Decode("", "html"))", ""},
      {R"(Decode("abc&Acirc;xyz", "html"))", "abc\xC3\x82xyz"},
      {R"(Decode("abc&NoneSuchButVeryLongIndeed;", "html"))", "abc"},
      {R"(Decode("&#x0041;&AElig;&Aacute;", "html"))", "A\xC3\x86\xC3\x81"},
      {R"(Decode("xyz&#", "html"))", "xyz"},

      // XML
      {R"(Decode("", "xml"))", ""},
      {R"(Decode("~!@#$%%^&amp;*()_+|`", "xml"))", "~!@#$%%^&*()_+|`"},
      {R"(Decode("abc&nonesuchbutverylongindeed;", "xml"))", "abc"},
      {R"(Decode("&quot;&#x45;&lt;&gt;[].&apos;", "xml"))", "\"E<>[].'"},
      {R"(Decode("xyz&#", "xml"))", "xyz"},

      // URL
      {R"(Decode("", "url"))", ""},
      {R"(Decode("~%26^&*()_+|`{", "url"))", "~&^&*()_+|`{"},
      {R"(Decode("~%26^&*()_+|`{", "mbogo"))", "~&^&*()_+|`{"},
      {R"(Decode("~%26^&*()_+|`{"))", "~&^&*()_+|`{"},
      {R"(Decode("~%~~"))", ""},
      {R"(Decode("?%~"))", ""},
      {R"(Decode("?%"))", "?"},
  };

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));
    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, DISABLED_Encode) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {
      {"Encode(\"\"\"hello, world!\"\"\", \"url\")",
       "%%22hello,%%20world!%%22"},
      {"Encode(\"ÁÂÃÄÅÆ\", \"html\")", "&#xc1;&#Xc2;&#Xc3;&#xc4;&#xc5;&#xc6;"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, DISABLED_Format) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Format(\"MMM D, YYYY\", \"20020901\")", "Sep 1, 2002"},
               {"Format(\"$9,999,999.99\", 1234567.89)", "$1,234,567.89"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Left) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Left(\"ABCDEFGH\", 3)", "ABC"},
               {"Left(\"Tony Blue\", 5)", "Tony "}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Len) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    int result;
  } tests[] = {
      {"Len(\"ABCDEFGH\")", 8}, {"Len(4)", 1}, {"Len(Str(4.532, 6, 4))", 6}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsInteger());
    EXPECT_EQ(tests[i].result, value->ToInteger())
        << "Program: " << tests[i].program;
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Lower) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Lower(\"ABC\")", "abc"},
               {"Lower(\"21 Main St.\")", "21 main st."},
               {"Lower(15)", "15"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

// This is testing for an OOB read, so will likely only fail under ASAN.
TEST_F(CFXJSE_FormCalcContextEmbedderTest, bug_854623) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  const uint8_t test_string[] = {
      0x4c, 0x6f, 0x77, 0x65, 0x72, 0x28, 0x22, 0xc3,
      0x85, 0xc3, 0x85, 0xc3, 0x85, 0x22, 0x29};  // Lower("ÅÅÅ")
  Execute(ByteString(test_string, sizeof(test_string)).AsStringView());
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Ltrim) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Ltrim(\"   ABCD\")", "ABCD"},
               {"Ltrim(Rtrim(\"    Tony Blue    \"))", "Tony Blue"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, DISABLED_Parse) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Parse(\"MMM D, YYYY\", \"Sep 1, 2002\")", "2002-09-01"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }

  EXPECT_TRUE(Execute("Parse(\"$9,999,999.99\", \"$1,234,567.89\")"));
  CFXJSE_Value* value = GetValue();
  EXPECT_TRUE(value->IsNumber());
  EXPECT_FLOAT_EQ(1234567.89f, value->ToFloat());
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Replace) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Replace(\"Tony Blue\", \"Tony\", \"Chris\")", "Chris Blue"},
               {"Replace(\"ABCDEFGH\", \"D\")", "ABCEFGH"},
               {"Replace(\"ABCDEFGH\", \"d\")", "ABCDEFGH"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Right) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Right(\"ABCDEFGH\", 3)", "FGH"},
               {"Right(\"Tony Blue\", 5)", " Blue"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Rtrim) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Rtrim(\"ABCD   \")", "ABCD"},
               {"Rtrim(\"Tony Blue      \t\")", "Tony Blue"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Space) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Space(5)", "     "},
               {"Concat(\"Tony\", Space(1), \"Blue\")", "Tony Blue"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Str) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Str(2.456)", "         2"},
               {"Str(4.532, 6, 4)", "4.5320"},
               {"Str(234.458, 4)", " 234"},
               {"Str(31.2345, 4, 2)", "****"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Stuff) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Stuff(\"TonyBlue\", 5, 0, \" \")", "Tony Blue"},
               {"Stuff(\"ABCDEFGH\", 4, 2)", "ABCFGH"},
               {"Stuff(\"members-list@myweb.com\", 0, 0, \"cc:\")",
                "cc:members-list@myweb.com"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Substr) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Substr(\"ABCDEFG\", 3, 4)", "CDEF"},
               {"Substr(3214, 2, 1)", "2"},
               {"Substr(\"ABCDEFG\", 5, 0)", ""},
               {"Substr(\"21 Waterloo St.\", 4, 5)", "Water"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Uuid) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  EXPECT_TRUE(Execute("Uuid()"));

  CFXJSE_Value* value = GetValue();
  EXPECT_TRUE(value->IsString());
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Upper) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {{"Upper(\"abc\")", "ABC"},
               {"Upper(\"21 Main St.\")", "21 MAIN ST."},
               {"Upper(15)", "15"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, WordNum) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  struct {
    const char* program;
    const char* result;
  } tests[] = {
      // {"WordNum(123.45)",
      //  "One Hundred and Twenty-three"},  // This looks like it's wrong in the
      //                                    // Formcalc document.
      // {"WordNum(123.45, 1)", "One Hundred and Twenty-three Dollars"},
      {"WordNum(1154.67, 2)",
       "One Thousand One Hundred Fifty-four Dollars And Sixty-seven Cents"},
      {"WordNum(43, 2)", "Forty-three Dollars And Zero Cents"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(Execute(tests[i].program));

    CFXJSE_Value* value = GetValue();
    EXPECT_TRUE(value->IsString());
    EXPECT_STREQ(tests[i].result, value->ToString().c_str())
        << "Program: " << tests[i].program << " Result: '" << value->ToString()
        << "'";
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Get) {
  // TODO(dsinclair): Is this supported?
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Post) {
  // TODO(dsinclair): Is this supported?
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, Put) {
  // TODO(dsinclair): Is this supported?
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, InvalidFunctions) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  const char* const tests[] = {
      "F()", "()", "()()()", "Round(2.0)()",
  };

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_FALSE(ExecuteSilenceFailure(tests[i]));
  }
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, MethodCall) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  const char test[] = {"$form.form1.TextField11.getAttribute(\"h\")"};
  EXPECT_TRUE(Execute(test));

  CFXJSE_Value* value = GetValue();
  EXPECT_TRUE(value->IsString());
  EXPECT_STREQ("12.7mm", value->ToString().c_str());
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, GetXFAEventChange) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  CXFA_EventParam params;
  params.m_wsChange = L"changed";

  CFXJSE_Engine* context = GetScriptContext();
  context->SetEventParam(&params);

  const char test[] = {"xfa.event.change"};
  EXPECT_TRUE(Execute(test));

  CFXJSE_Value* value = GetValue();
  EXPECT_TRUE(value->IsString());
  EXPECT_STREQ("changed", value->ToString().c_str());
  context->SetEventParam(nullptr);
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, SetXFAEventChange) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  CXFA_EventParam params;
  CFXJSE_Engine* context = GetScriptContext();
  context->SetEventParam(&params);

  const char test[] = {"xfa.event.change = \"changed\""};
  EXPECT_TRUE(Execute(test));
  EXPECT_EQ(L"changed", params.m_wsChange);
  context->SetEventParam(nullptr);
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, SetXFAEventFullTextFails) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  CXFA_EventParam params;
  params.m_wsFullText = L"Original Full Text";

  CFXJSE_Engine* context = GetScriptContext();
  context->SetEventParam(&params);

  const char test[] = {"xfa.event.fullText = \"Changed Full Text\""};
  EXPECT_TRUE(Execute(test));
  EXPECT_EQ(L"Original Full Text", params.m_wsFullText);
  context->SetEventParam(nullptr);
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, EventChangeSelection) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  CXFA_EventParam params;
  params.m_wsPrevText = L"1234";
  params.m_iSelStart = 1;
  params.m_iSelEnd = 3;

  CFXJSE_Engine* context = GetScriptContext();
  context->SetEventParam(&params);

  // Moving end to start works fine.
  EXPECT_TRUE(Execute("xfa.event.selEnd = \"1\""));
  EXPECT_EQ(1, params.m_iSelStart);
  EXPECT_EQ(1, params.m_iSelEnd);

  // Moving end before end, forces start to move in response.
  EXPECT_TRUE(Execute("xfa.event.selEnd = \"0\""));
  EXPECT_EQ(0, params.m_iSelStart);
  EXPECT_EQ(0, params.m_iSelEnd);

  // Negatives not allowed
  EXPECT_TRUE(Execute("xfa.event.selEnd = \"-1\""));
  EXPECT_EQ(0, params.m_iSelStart);
  EXPECT_EQ(0, params.m_iSelEnd);

  // Negatives not allowed
  EXPECT_TRUE(Execute("xfa.event.selStart = \"-1\""));
  EXPECT_EQ(0, params.m_iSelStart);
  EXPECT_EQ(0, params.m_iSelEnd);

  params.m_iSelEnd = 1;

  // Moving start to end works fine.
  EXPECT_TRUE(Execute("xfa.event.selStart = \"1\""));
  EXPECT_EQ(1, params.m_iSelStart);
  EXPECT_EQ(1, params.m_iSelEnd);

  // Moving start after end moves end.
  EXPECT_TRUE(Execute("xfa.event.selStart = \"2\""));
  EXPECT_EQ(2, params.m_iSelStart);
  EXPECT_EQ(2, params.m_iSelEnd);

  // Setting End past end of string clamps to string length;
  EXPECT_TRUE(Execute("xfa.event.selEnd = \"20\""));
  EXPECT_EQ(2, params.m_iSelStart);
  EXPECT_EQ(4, params.m_iSelEnd);

  // Setting Start past end of string clamps to string length;
  EXPECT_TRUE(Execute("xfa.event.selStart = \"20\""));
  EXPECT_EQ(4, params.m_iSelStart);
  EXPECT_EQ(4, params.m_iSelEnd);

  context->SetEventParam(nullptr);
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, XFAEventCancelAction) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  CXFA_EventParam params;
  params.m_bCancelAction = false;

  CFXJSE_Engine* context = GetScriptContext();
  context->SetEventParam(&params);

  EXPECT_TRUE(Execute("xfa.event.cancelAction"));

  CFXJSE_Value* value = GetValue();
  EXPECT_TRUE(value->IsBoolean());
  EXPECT_FALSE(value->ToBoolean());

  EXPECT_TRUE(Execute("xfa.event.cancelAction = \"true\""));
  EXPECT_TRUE(params.m_bCancelAction);

  context->SetEventParam(nullptr);
}

TEST_F(CFXJSE_FormCalcContextEmbedderTest, ComplexTextChangeEvent) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  CXFA_EventParam params;
  params.m_wsChange = L"g";
  params.m_wsPrevText = L"abcd";
  params.m_iSelStart = 1;
  params.m_iSelEnd = 3;

  CFXJSE_Engine* context = GetScriptContext();
  context->SetEventParam(&params);

  EXPECT_EQ(L"abcd", params.m_wsPrevText);
  EXPECT_EQ(L"agd", params.GetNewText());
  EXPECT_EQ(L"g", params.m_wsChange);
  EXPECT_EQ(1, params.m_iSelStart);
  EXPECT_EQ(3, params.m_iSelEnd);

  const char change_event[] = {"xfa.event.change = \"xyz\""};
  EXPECT_TRUE(Execute(change_event));

  EXPECT_EQ(L"abcd", params.m_wsPrevText);
  EXPECT_EQ(L"xyz", params.m_wsChange);
  EXPECT_EQ(L"axyzd", params.GetNewText());
  EXPECT_EQ(1, params.m_iSelStart);
  EXPECT_EQ(3, params.m_iSelEnd);

  const char sel_event[] = {"xfa.event.selEnd = \"1\""};
  EXPECT_TRUE(Execute(sel_event));

  EXPECT_EQ(L"abcd", params.m_wsPrevText);
  EXPECT_EQ(L"xyz", params.m_wsChange);
  EXPECT_EQ(L"axyzbcd", params.GetNewText());
  EXPECT_EQ(1, params.m_iSelStart);
  EXPECT_EQ(1, params.m_iSelEnd);

  context->SetEventParam(nullptr);
}

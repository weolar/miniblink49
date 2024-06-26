// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/crt/cfgas_formatstring.h"

#include <time.h>

#include <memory>

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "testing/fx_string_testhelpers.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_localemgr.h"

class CFGAS_FormatStringTest : public testing::Test {
 public:
  CFGAS_FormatStringTest() {
    SetTZ("UTC");
    CPDF_ModuleMgr::Get()->Init();
  }

  ~CFGAS_FormatStringTest() override { CPDF_ModuleMgr::Get()->Destroy(); }

  void TearDown() override {
    fmt_.reset();
    mgr_.reset();
  }

  void SetTZ(const char* tz) {
#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
    _putenv_s("TZ", tz);
    _tzset();
#else
    setenv("TZ", tz, 1);
    tzset();
#endif
  }

  // Note, this re-creates the fmt on each call. If you need to multiple
  // times store it locally.
  CFGAS_FormatString* fmt(const WideString& locale) {
    fmt_.reset();  // Can't outlive |mgr_|.
    mgr_ = pdfium::MakeUnique<CXFA_LocaleMgr>(nullptr, locale);
    fmt_ = pdfium::MakeUnique<CFGAS_FormatString>(mgr_.get());
    return fmt_.get();
  }

 protected:
  std::unique_ptr<CXFA_LocaleMgr> mgr_;
  std::unique_ptr<CFGAS_FormatString> fmt_;
};

// TODO(dsinclair): Looks like the formatter/parser does not handle the various
// 'g' flags.
TEST_F(CFGAS_FormatStringTest, DateFormat) {
  struct {
    const wchar_t* locale;
    const wchar_t* input;
    const wchar_t* pattern;
    const wchar_t* output;
  } tests[] = {
      {L"en", L"2002-10-25", L"MMMM DD, YYYY", L"October 25, 2002"},
      // Note, this is in the doc as 5 but it's wrong and should be 3 by the
      // example in the Picture Clause Reference section.
      {L"en", L"20040722", L"'Week of the month is' w",
       L"Week of the month is 3"},
      {L"en", L"20040722", L"e 'days after Sunday'", L"4 days after Sunday"},
      {L"en", L"20040722", L"YYYY-'W'WW-e", L"2004-W30-4"},
      {L"en", L"20040722", L"E 'days after Saturday'",
       L"5 days after Saturday"},
      {L"en", L"2000-01-01", L"EEE, 'the' D 'of' MMMM, YYYY",
       L"Sat, the 1 of January, 2000"},
      {L"en", L"2000-01-01", L"EEEE, 'the' D 'of' MMMM, YYYY",
       L"Saturday, the 1 of January, 2000"},
      {L"en", L"19991202", L"MM/D/YY", L"12/2/99"},
      {L"en", L"19990110", L"MMM D, YYYY", L"Jan 10, 1999"},
      {L"en", L"19990202", L"J", L"33"},
      {L"en", L"19990202", L"JJJ", L"033"},
      {L"en", L"19991231", L"J", L"365"},
      {L"en", L"20001231", L"J", L"366"},
      {L"en", L"19990501", L"J", L"121"},
      {L"en", L"19990901", L"J", L"244"},
      {L"en", L"19990228", L"J", L"59"},
      {L"en", L"20000229", L"J", L"60"},
      {L"en", L"21000501", L"J", L"121"},
      {L"en", L"19990102", L"M", L"1"},
      {L"en", L"19990102", L"MMM", L"Jan"},
      {L"en", L"19990102", L"YYYY G", L"1999 AD"},
      // Week 01 of the year is the week containing Jan 04.
      // {L"en", L"19990102", L"WW", L"00"},  -- Returns 01 incorrectly
      // {L"en", L"19990104", L"WW", L"01"},  -- Returns 02 incorrectly
      // The ?*+ should format as whitespace.
      // {L"en", L"19990104", L"YYYY?*+MM", L"1999   01"},
      // {L"en", L"1999-07-16", L"date{DD/MM/YY} '('date{MMM DD, YYYY}')'",
      //  L"16/07/99 (Jul 16, 1999)"},
      {L"de_CH", L"20041030", L"D. MMMM YYYY", L"30. Oktober 2004"},
      {L"fr_CA", L"20041030", L"D MMMM YYYY", L"30 octobre 2004"},
      {L"en", L"2002-10-25", L"date(fr){DD MMMM, YYYY}", L"25 octobre, 2002"},
      {L"en", L"2002-10-25", L"date(es){EEEE, D 'de' MMMM 'de' YYYY}",
       L"viernes, 25 de octubre de 2002"},
      // {L"en", L"2002-20-25", L"date.long(fr)()", L"25 octobre, 2002"},
      // {L"ja", L"2003-11-03", L"gY/M/D", L"H15/11/3"},
      // {L"ja", L"1989-01-08", L"ggY-M-D", L"\u5e731-1-8"},
      // {L"ja", L"1989-11-03", L"gggYY/MM/DD", L"\u5e73\u621089/11/03"},
  };
  // Note, none of the full width date symbols are listed here
  // as they are not supported. In theory there are the full width versions
  // of DDD, DDDD, MMM, MMMM, E, e, gg, YYY, YYYYY.

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    WideString result;
    EXPECT_TRUE(fmt(tests[i].locale)
                    ->FormatDateTime(tests[i].input, tests[i].pattern,
                                     FX_DATETIMETYPE_Date, &result));
    EXPECT_STREQ(tests[i].output, result.c_str()) << " TEST: " << i;
  }
}

TEST_F(CFGAS_FormatStringTest, TimeFormat) {
  struct {
    const wchar_t* locale;
    const wchar_t* input;
    const wchar_t* pattern;
    const wchar_t* output;
  } tests[] = {{L"en", L"01:01:11", L"h:M A", L"1:1 AM"},
               {L"en", L"13:01:11", L"h:M A", L"1:1 PM"},
               {L"en", L"01:01:11", L"hh:MM:SS A", L"01:01:11 AM"},
               {L"en", L"13:01:11", L"hh:MM:SS A", L"01:01:11 PM"},
               {L"en", L"01:01:11", L"hh:MM:SS A Z", L"01:01:11 AM GMT-02:00"},
               {L"en", L"01:01:11", L"hh:MM:SS A z", L"01:01:11 AM -02:00"},
               // {L"en", L"01:01:11", L"hh:MM:SS A zz", L"01:01:11 AM GMT"},
               // Should change ?*+ into ' ' when formatting.
               // {L"en", L"01:01:11", L"hh:MM:SS?*+A", L"01:01:11   AM"},
               {L"en", L"12:01:01", L"k:MM:SS", L"12:01:01"},
               {L"en", L"14:01:01", L"k:MM:SS", L"2:01:01"},
               {L"en", L"12:01:11", L"kk:MM", L"12:01"},
               {L"en", L"14:01:11", L"kk:MM", L"02:01"},
               {L"en", L"12:01:11 +04:30", L"kk:MM", L"05:31"},
               {L"en", L"12:01:11", L"kk:MM A", L"12:01 PM"},
               {L"en", L"00:01:01", L"H:M:S", L"0:1:1"},
               {L"en", L"13:02:11", L"H:M:S", L"13:2:11"},
               {L"en", L"00:01:11.001", L"HH:M:S.FFF", L"00:1:11.001"},
               {L"en", L"13:02:11", L"HH:M", L"13:2"},
               {L"en", L"00:01:11", L"K:M", L"24:1"},
               {L"en", L"00:02:11", L"KK:M", L"24:2"},
               {L"en", L"11:11:11", L"HH:MM:SS 'o''clock' A Z",
                L"11:11:11 o'clock AM GMT-02:00"},
               {L"en", L"14:30:59", L"h:MM A", L"2:30 PM"},
               {L"en", L"14:30:59", L"HH:MM:SS A Z", L"14:30:59 PM GMT-02:00"}};
  // Note, none of the full width time symbols are listed here
  // as they are not supported. In theory there are the full
  // width versions of kkk, kkkk, HHH, HHHH, KKK, KKKK, MMM, MMMM,
  // SSS, SSSS plus 2 more that the spec apparently forgot to
  // list the symbol.

  // The z modifier only appends if the TZ is outside of +0
  SetTZ("UTC+2");

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    WideString result;
    EXPECT_TRUE(fmt(tests[i].locale)
                    ->FormatDateTime(tests[i].input, tests[i].pattern,
                                     FX_DATETIMETYPE_Time, &result));
    EXPECT_STREQ(tests[i].output, result.c_str()) << " TEST: " << i;
  }

  SetTZ("UTC");
}

TEST_F(CFGAS_FormatStringTest, DateTimeFormat) {
  struct {
    const wchar_t* locale;
    const wchar_t* input;
    const wchar_t* pattern;
    const wchar_t* output;
  } tests[] = {
      {L"en", L"1999-07-16T10:30Z",
       L"'At' time{HH:MM Z} 'on' date{MMM DD, YYYY}",
       L"At 10:30 GMT on Jul 16, 1999"},
      {L"en", L"1999-07-16T10:30", L"'At' time{HH:MM} 'on' date{MMM DD, YYYY}",
       L"At 10:30 on Jul 16, 1999"},
      {L"en", L"1999-07-16T10:30Z",
       L"time{'At' HH:MM Z} date{'on' MMM DD, YYYY}",
       L"At 10:30 GMT on Jul 16, 1999"},
      {L"en", L"1999-07-16T10:30Z",
       L"time{'At 'HH:MM Z}date{' on 'MMM DD, YYYY}",
       L"At 10:30 GMT on Jul 16, 1999"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    WideString result;
    EXPECT_TRUE(fmt(tests[i].locale)
                    ->FormatDateTime(tests[i].input, tests[i].pattern,
                                     FX_DATETIMETYPE_TimeDate, &result));
    EXPECT_STREQ(tests[i].output, result.c_str()) << " TEST: " << i;
  }
}

TEST_F(CFGAS_FormatStringTest, DateParse) {
  struct {
    const wchar_t* locale;
    const wchar_t* input;
    const wchar_t* pattern;
    CFX_DateTime output;
  } tests[] = {
      {L"en", L"12/2/99", L"MM/D/YY", CFX_DateTime(1999, 12, 2, 0, 0, 0, 0)},
      {L"en", L"2/2/99", L"M/D/YY", CFX_DateTime(1999, 2, 2, 0, 0, 0, 0)},
      {L"en", L"2/2/10", L"M/D/YY", CFX_DateTime(2010, 2, 2, 0, 0, 0, 0)},
      {L"en", L"Jan 10, 1999", L"MMM D, YYYY",
       CFX_DateTime(1999, 1, 10, 0, 0, 0, 0)},
      {L"en", L"Jan 10, 1999 AD", L"MMM D, YYYY G",
       CFX_DateTime(1999, 1, 10, 0, 0, 0, 0)},
      // TODO(dsinclair): Should this be -2 instead of 2?
      {L"en", L"Jan 10, 0002 BC", L"MMM D, YYYY G",
       CFX_DateTime(2, 1, 10, 0, 0, 0, 0)},
      {L"en", L"October 25, 2002", L"MMMM DD, YYYY",
       CFX_DateTime(2002, 10, 25, 0, 0, 0, 0)},
      // TODO(dsinclair): The J and JJJ are ignored during parsing when they
      // could be turned back into a date.
      {L"en", L"1999-33", L"YYYY-J", CFX_DateTime(1999, 1, 1, 0, 0, 0, 0)},
      {L"en", L"1999-033", L"YYYY-JJJ", CFX_DateTime(1999, 1, 1, 0, 0, 0, 0)},
      {L"de_CH", L"30. Oktober 2004", L"D. MMMM YYYY",
       CFX_DateTime(2004, 10, 30, 0, 0, 0, 0)},
      {L"fr_CA", L"30 octobre 2004", L"D MMMM YYYY",
       CFX_DateTime(2004, 10, 30, 0, 0, 0, 0)},
      {L"en", L"Saturday, the 1 of January, 2000",
       L"EEEE, 'the' D 'of' MMMM, YYYY", CFX_DateTime(2000, 1, 1, 0, 0, 0, 0)},
      {L"en", L"Sat, the 1 of January, 2000", L"EEE, 'the' D 'of' MMMM, YYYY",
       CFX_DateTime(2000, 1, 1, 0, 0, 0, 0)},
      {L"en", L"7, the 1 of January, 2000",  // 7 == Saturday as 1 == Sunday
       L"E, 'the' D 'of' MMMM, YYYY", CFX_DateTime(2000, 1, 1, 0, 0, 0, 0)},
      {L"en", L"6, the 1 of January, 2000",  // 6 == Saturday as 1 == Monday
       L"e, 'the' D 'of' MMMM, YYYY", CFX_DateTime(2000, 1, 1, 0, 0, 0, 0)},
      {L"en", L"2004-07-22 Week of the month is 3",
       L"YYYY-MM-DD 'Week of the month is' w",
       CFX_DateTime(2004, 7, 22, 0, 0, 0, 0)},
      {L"en", L"2004-07-22 Week of the year is 03",
       L"YYYY-MM-DD 'Week of the year is' WW",
       CFX_DateTime(2004, 7, 22, 0, 0, 0, 0)}
      // {L"ja", L"H15/11/3", L"gY/M/D", CFX_DateTime(2003, 11, 3, 0, 0, 0, 0)},
      // {L"ja", L"\u5e731-1-8", L"ggY-M-D", CFX_DateTime(1989, 1, 8, 0, 0, 0,
      // 0)}, {L"ja", L"\u5e73\u621089/11/03", L"gggYY/MM/DD",
      //  CFX_DateTime(1989, 11, 3, 0, 0, 0, 0)},
      // {L"ja", L"u337b99/01/08", L"\u0067\u0067YY/MM/DD",
      //  CFX_DateTime(1999, 1, 8, 0, 0, 0, 0)}
  };
  // Note, none of the full width date symbols are listed here as they are
  // not supported. In theory there are the full width versions of DDD,
  // DDDD, MMM, MMMM, E, e, gg, YYY, YYYYY.

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    CFX_DateTime result;
    EXPECT_TRUE(fmt(tests[i].locale)
                    ->ParseDateTime(tests[i].input, tests[i].pattern,
                                    FX_DATETIMETYPE_Date, &result));
    EXPECT_EQ(tests[i].output, result) << " TEST: " << i;
  }
}

// TODO(dsinclair): GetDateTimeFormat is broken and doesn't allow just returning
// a parsed Time. It will assume it's a Date. The method needs to be re-written.
// TEST_F(CFGAS_FormatStringTest, TimeParse) {
//   struct {
//     const wchar_t* locale;
//     const wchar_t* input;
//     const wchar_t* pattern;
//     CFX_DateTime output;
//   } tests[] = {
//       {L"en", L"18:00", L"HH:MM", CFX_DateTime(0, 0, 0, 18, 0, 0, 0)},
//       {L"en", L"12.59 Uhr", L"H.MM 'Uhr'", CFX_DateTime(0, 0, 0, 12, 59, 0,
//       0)}, {L"en", L"1:05:10 PM PST", L"h:MM:SS A Z",
//        CFX_DateTime(0, 0, 0, 17, 05, 10, 0)}};
//   // Note, none of the full width date symbols are listed here as they are
//   // not supported. In theory there are the full width versions of kkk,
//   // kkkk, HHH, HHHH, KKK, KKKK, MMM, MMMM, SSS, SSSS plus 2 more that the
//   // spec apparently forgot to list the symbol.

//   for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
//     CFX_DateTime result;
//     EXPECT_TRUE(fmt(tests[i].locale)
//                     ->ParseDateTime(tests[i].input, tests[i].pattern,
//                                     FX_DATETIMETYPE_Time, &result));
//     EXPECT_EQ(tests[i].output, result) << " TEST: " << i;
//   }
// }

TEST_F(CFGAS_FormatStringTest, SplitFormatString) {
  std::vector<WideString> results;
  fmt(L"en")->SplitFormatString(
      L"null{'No data'} | null{} | text{999*9999} | text{999*999*9999}",
      &results);
  EXPECT_EQ(4UL, results.size());

  const wchar_t* patterns[] = {L"null{'No data'} ", L" null{} ",
                               L" text{999*9999} ", L" text{999*999*9999}"};

  for (size_t i = 0; i < results.size(); ++i) {
    EXPECT_STREQ(patterns[i], results[i].c_str());
  }
}

TEST_F(CFGAS_FormatStringTest, NumParse) {
  struct {
    const wchar_t* locale;
    const wchar_t* input;
    const wchar_t* pattern;
    const wchar_t* output;
  } tests[] = {
      // {L"en", L"€100.00", L"num(en_GB){$z,zz9.99}", L"100"},
      // {L"en", L"1050", L"99V99", L"10.50"},
      // {L"en", L"3125", L"99V99", L"31.25"},
      {L"en", L"12.345e3", L"99.999E", L"12345.000000"},
      {L"en", L"12.345e+3", L"99.999E", L"12345.000000"},
      {L"en", L"12.345E-2", L"99.999E", L"0.123450"},
      // TODO(dsinclair): Returns 0.000?
      // {L"en", L"12e-2", L"99E", L"0.12"},
      {L"en", L"150", L"z999", L"150"},
      {L"en", L"150.50$", L"zzz.zz$", L"150.50"},
      {L"en", L"0150", L"z999", L"0150"},
      {L"en", L"123CR", L"999cr", L"-123"},
      {L"en", L"123", L"999cr", L"123"},
      {L"en", L"123CR", L"999CR", L"-123"},
      {L"en", L"123  ", L"999CR", L"123"},
      {L"en", L"123DB", L"999db", L"-123"},
      {L"en", L"123", L"999db", L"123"},
      {L"en", L"123DB", L"999DB", L"-123"},
      {L"en", L"123  ", L"999DB", L"123"},
      {L"en", L"123.5CR", L"999.9cr", L"-123.5"},
      {L"en", L"123.5", L"999.9cr", L"123.5"},
      {L"en", L"123.5CR", L"999.9CR", L"-123.5"},
      // {L"en", L"123.5  ", L"999.9CR", L"123.5"},
      {L"en", L"123.5DB", L"999.9db", L"-123.5"},
      {L"en", L"123.5", L"999.9db", L"123.5"},
      {L"en", L"123.5DB", L"999.9DB", L"-123.5"},
      // {L"en", L"123.5  ", L"999.9DB", L"123.5"},
      {L"en", L"10.50", L"z,zz9.99", L"10.50"},
      {L"en", L"3,125.00", L"z,zz9.99", L"3125.00"},
      {L"en", L"$1,234.00", L"$z,zz9.99DB", L"1234.00"},
      // TODO(dsinclair): Comes out as 1234 instead of -1234.
      // {L"en", L"$,1234.00DB", L"$z,zz9.99DB", L"-1234.00"},
      {L"en", L"1.234", L"zz9.zzz", L"1.234"},
      {L"en", L"1 text", L"num{z 'text'}", L"1"},
      {L"en", L"1.234 text", L"z.zzz 'text'", L"1.234"},
      {L"en", L"  1.234", L"ZZ9.ZZZ", L"1.234"},
      {L"en", L"12.345", L"zz9.zzz", L"12.345"},
      {L"en", L" 12.345", L"ZZ9.ZZZ", L"12.345"},
      {L"en", L"123.456", L"zz9.zzz", L"123.456"},
      {L"en", L"123.456", L"ZZ9.ZZZ", L"123.456"},
      {L"en", L"123.456-", L"ZZ9.ZZZS", L"-123.456"},
      {L"en", L"123.456+", L"ZZ9.ZZZS", L"123.456"},
      {L"en", L"123.456 ", L"ZZ9.ZZZS", L"123.456"},
      {L"en", L"123.456-", L"ZZ9.ZZZS", L"-123.456"},
      {L"en", L"123.456+", L"ZZ9.ZZZS", L"123.456"},
      {L"en", L"123", L"zz9.zzz", L"123"},
      {L"en", L"123.", L"ZZ9.ZZZ", L"123."},
      {L"en", L"123.", L"zz9.zzz", L"123."},
      {L"en", L"123.", L"ZZ9.ZZZ", L"123."},
      {L"en", L"123.0", L"zz9.zzz", L"123.0"},
      {L"en", L"123.0", L"ZZ9.ZZZ", L"123.0"},
      {L"en", L"123.000", L"zz9.zzz", L"123.000"},
      {L"en", L"123.000", L"ZZ9.ZZZ", L"123.000"},
      {L"en", L"12,345.67", L"zzz,zz9.88888888", L"12345.67"},
      {L"en", L"12,345.0000", L"zzz,zz9.88888888", L"12345.0000"},
      {L"en", L"12,345.6789", L"zzz,zz9.8", L"12345.6789"},
      {L"en", L"12,345.", L"zzz,zz9.8", L"12345."},
      {L"en", L"123,456.000", L"zzz,zz9.8888", L"123456.000"},
      {L"en", L"123,456.0", L"zzz,zz9.8888", L"123456.0"},
      {L"en", L"123,456", L"zzz,zz9.8888", L"123456"},
      {L"en", L"123,456", L"ZZZ,ZZ9.88", L"123456"},
      {L"en", L"12,345.67", L"zzz,zz9.88888888", L"12345.67"},
      {L"en", L"12,345.0000", L"zzz,zz9.88888888", L"12345.0000"},
      {L"en", L"12,345.6789", L"zzz,zz9.8", L"12345.6789"},
      {L"en", L"12,345.", L"zzz,zz9.8", L"12345."},
      // TODO(dsinclair): Parses to 0
      // {L"en", L"12%", L"zz9.%%", L".12"},
      {L"en", L"1,234.50%", L"zzz,zz9.99%%", L"12.345"},
      // {L"en", L"-00123", L"S999v99", L"-1.23"},
      {L"en", L" 001.23", L"S999V99", L"001.23"},
      // {L"en", L" 123.00", L"S999V99", L"123"},
      {L"en", L"  12.30", L"SZZ9.99", L"12.30"},
      {L"en", L"- 12.30", L"SZ99.99", L"-12.30"},
      {L"en", L"123.00", L"szz9.99", L"123.00"},
      {L"en", L"-123.00", L"szz9.99", L"-123.00"},
      // {L"en", L"$  1,234.00  ", L"$ZZ,ZZ9.99CR", L"1234"},
      // {L"en", L"$  1,234.00CR", L"$ZZ,ZZ9.99CR", L"-1234"},
      // {L"en", L"$1,23400", L"$z,zz9.99DB", L"1234"},
      {L"en", L"$1,234.00DB", L"$z,zz9.99DB", L"-1234.00"},
      {L"en",
       L"1\xA0"
       L"234",
       L"num(fr){z,zzz}", L"1234"},
      // TODO(dsinclair): Parses to blank
      // {L"en", L"1,234%", L"num.percent{}", L"12.34"},
      // {L"en", L"1\xA0" L"234%%", L"num(fr).percent{}", L"12.34"},
      // TODO(dsinclair): Parses to blank
      // {L"en", L"1,234%", L"num{9,999%%}", L"12.34"},
      {L"fr",
       L"123\xA0"
       L"456",
       L"zzz,zzz", L"123456"},
      {L"en", L"12%", L"zz%", L"0.12"},
      {L"en", L"(123", L"(zzz", L"-123"},
      {L"en", L"123)", L"zzz)", L"-123"},
      {L"en", L"(123)", L"(zzz)", L"-123"},
      {L"en", L"123 ", L"zzz)", L"123"},
      {L"en", L" 123", L"(zzz", L"123"},
      {L"en", L" 123 ", L"(zzz)", L"123"},
      {L"en", L"123.5(", L"zzz.z(", L"-123.5"},
      {L"en", L"123.5)", L"zzz.z)", L"-123.5"},
      {L"en", L"123.5 ", L"zzz.z)", L"123.5"},
      {L"en", L"123.5 ", L"zzz.z(", L"123.5"},
      {L"en", L"123.545,4", L"zzz.zzz,z", L"123.5454"},
  };

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    WideString result;
    EXPECT_TRUE(fmt(tests[i].locale)
                    ->ParseNum(tests[i].input, tests[i].pattern, &result))
        << " TEST: " << i;
    EXPECT_STREQ(tests[i].output, result.c_str()) << " TEST: " << i;
  }
}

TEST_F(CFGAS_FormatStringTest, NumFormat) {
  struct {
    const wchar_t* locale;
    const wchar_t* input;
    const wchar_t* pattern;
    const wchar_t* output;
  } tests[] = {
      {L"en", L"1.234", L"zz9.zzz", L"1.234"},
      {L"en", L"1", L"num{z 'text'}", L"1 text"},
      {L"en", L"1", L"num{'text' z}", L"text 1"},
      {L"en", L"1.234", L"ZZ9.ZZZ", L"  1.234"},
      {L"en", L"12.345", L"zz9.zzz", L"12.345"},
      {L"en", L"12.345", L"ZZ9.ZZZ", L" 12.345"},
      {L"en", L"123.456", L"zz9.zzz", L"123.456"},
      {L"en", L"123.456", L"ZZ9.ZZZ", L"123.456"},
      {L"en", L"123", L"zz9.zzz", L"123"},
      {L"en", L"123", L"ZZ9.ZZZ", L"123.000"},
      {L"en", L"123.", L"zz9.zzz", L"123."},
      {L"en", L"123.", L"ZZ9.ZZZ", L"123.000"},
      {L"en", L"123.0", L"zz9.zzz", L"123"},
      {L"en", L"123.0", L"ZZ9.ZZZ", L"123.000"},
      {L"en", L"123.000", L"zz9.zzz", L"123"},
      {L"en", L"123.000", L"ZZ9.ZZZ", L"123.000"},
      // {L"en", L"12345.67", L"zzz,zz9.88888888", L"12,345.67"},
      // {L"en", L"12345.0000", L"zzz,zz9.88888888", L"12,345.0000"},
      // {L"en", L"12345.6789", L"zzz,zz9.8", L"12,345.6789"},
      // {L"en", L"12345.", L"zzz,zz9.8", L"12,345"},
      // {L"en", L"123456.000", L"zzz,zz9.8888", L"123,456.000"},
      // {L"en", L"123456.0", L"zzz,zz9.8888", L"123,456.0"},
      {L"en", L"123456", L"zzz,zz9.8888", L"123,456"},
      {L"en", L"123456", L"ZZZ,ZZ9.88", L"123,456"},
      // {L"en", L"12345.67", L"zzz,zz9.88888888", L"12,345.67"},
      // {L"en", L"12345.0000", L"zzz,zz9.88888888", L"12,345.0000"},
      // {L"en", L"12345.6789", L"zzz,zz9.8", L"12,345.6789"},
      // {L"en", L"12345.", L"zzz,zz9.8", L"12,345"},
      // {L"en", L"12%%", L"zz9.%%", L"12%%"},
      // {L"en", L"1,234.5%%", L"zzz,zz9.99%%", L"1,234.50%%"},
      {L"en", L"-1.23", L"S999v99", L"-00123"},
      {L"en", L"1.23", L"S999V99", L" 001.23"},
      {L"en", L"123", L"S999V99", L" 123.00"},
      {L"en", L"12.3", L"SZZ9.99", L"  12.30"},
      {L"en", L"-12.3", L"SZ99.99", L"- 12.30"},
      {L"en", L"123", L"szz9.99", L"123.00"},
      {L"en", L"-123", L"szz9.99", L"-123.00"},
      // {L"en", L"1234", L"$ZZ,ZZ9.99CR", L"$  1,234.00  "},
      // {L"en", L"-1234", L"$ZZ,ZZ9.99CR", L"$  1,234.00CR"},
      // {L"en", L"1234", L"$z,zz9.99DB", L"$1,234.00"},
      {L"en", L"-1234", L"$z,zz9.99DB", L"$1,234.00DB"},
      {L"en", L"12345", L"99.999E", L"12.345E+3"},
      {L"en", L"12345", L"99999E", L"12345E+0"},
      {L"en", L".12345", L"99.999E", L"12.345E-2"},
      {L"en", L"12345", L"99,999", L"12,345"},
      {L"en", L"1234", L"num(fr){z,zzz}",
       L"1\xA0"
       L"234"},
      {L"en", L"12.34", L"num.percent{}", L"1,234%"},
      {L"en", L"12.34", L"num(fr).percent{}",
       L"1\xA0"
       L"234%"},
      // {L"en", L"12.34", L"num{9,999%%}", L"1,234%"},
      {L"en", L"-123", L"zzzCR", L"123CR"},
      {L"en", L"123", L"zzzCR", L"123  "},
      {L"en", L"-123", L"zzzcr", L"123CR"},
      {L"en", L"123", L"zzzcr", L"123"},
      {L"en", L"123", L"zzz$", L"123$"},
      {L"en", L"-123.5", L"zzz.zCR", L"123.5CR"},
      {L"en", L"123.5", L"zzz.zCR", L"123.5  "},
      {L"en", L"-123.5", L"zzz.zcr", L"123.5CR"},
      {L"en", L"123.5", L"zzz.zcr", L"123.5"},

      {L"en", L"-123.5", L"999.9db", L"123.5db"},
      {L"en", L"123.5", L"999.9db", L"123.5"},
      {L"en", L"-123.5", L"999.9DB", L"123.5DB"},
      {L"en", L"123.5", L"999.9DB", L"123.5  "},

      {L"en", L"-123", L"(zzz", L"(123"},
      // {L"en", L"-123", L"zzz)", L"123)"},
      {L"en", L"-123", L"(zzz)", L"(123)"},
      {L"en", L"123", L"zzz)", L"123 "},
      {L"en", L"123", L"(zzz", L" 123"},
      {L"en", L"123", L"(zzz)", L" 123 "},
      {L"en", L"-123.5", L"zzz.z(", L"123.5("},
      // {L"en", L"-123.5", L"zzz.z)", L"123.5)"},
      {L"en", L"123.5", L"zzz.z)", L"123.5 "},
      {L"en", L"123.5", L"zzz.z(", L"123.5 "},
  };

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    WideString result;
    EXPECT_TRUE(fmt(tests[i].locale)
                    ->FormatNum(tests[i].input, tests[i].pattern, &result))
        << " TEST: " << i;
    EXPECT_STREQ(tests[i].output, result.c_str()) << " TEST: " << i;
  }
}

TEST_F(CFGAS_FormatStringTest, TextParse) {
  struct {
    const wchar_t* locale;
    const wchar_t* input;
    const wchar_t* pattern;
    const wchar_t* output;
  } tests[] = {// TODO(dsinclair) Missing support for the global modifiers:
               //  ? - wildcard
               //  * - zero or more whitespace
               //  + - one or more whitespace
               // {L"en", L"555-1212", L"text(th_TH){999*9999}", L"5551212"},
               {L"en", L"ABC-1234-5", L"AAA-9999-X", L"ABC12345"},
               {L"en", L"ABC-1234-D", L"AAA-9999-X", L"ABC1234D"},
               {L"en", L"A1C-1234-D", L"OOO-9999-X", L"A1C1234D"},
               {L"en", L"A1C-1234-D", L"000-9999-X", L"A1C1234D"},
               {L"en", L"A1C-1234-D text", L"000-9999-X 'text'", L"A1C1234D"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    WideString result;
    EXPECT_TRUE(fmt(tests[i].locale)
                    ->ParseText(tests[i].input, tests[i].pattern, &result));
    EXPECT_STREQ(tests[i].output, result.c_str()) << " TEST: " << i;
  }
}

TEST_F(CFGAS_FormatStringTest, InvalidTextParse) {
  // Input does not match mask.
  WideString result;
  EXPECT_FALSE(fmt(L"en")->ParseText(L"123-4567-8", L"AAA-9999-X", &result));
}

TEST_F(CFGAS_FormatStringTest, TextFormat) {
  struct {
    const wchar_t* locale;
    const wchar_t* input;
    const wchar_t* pattern;
    const wchar_t* output;
  } tests[] = {
      {L"en", L"K1S5K2", L"A9A 9A9", L"K1S 5K2"},
      {L"en", L"K1S5K2", L"text(fr){A9A 9A9}", L"K1S 5K2"},
      {L"en", L"6135551212", L"'+1 ('9\u002399') '999-9999",
       L"+1 (6#13) 555-1212"},
      {L"en", L"6135551212", L"999.999.9999", L"613.555.1212"},
      {L"en", L"6135551212", L"999\u0023999\u002A9999", L"613#555*1212"},
      {L"en", L"K1#5K2", L"00X OO9", L"K1# 5K2"},
  };

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    WideString result;
    EXPECT_TRUE(fmt(tests[i].locale)
                    ->FormatText(tests[i].input, tests[i].pattern, &result));
    EXPECT_STREQ(tests[i].output, result.c_str()) << " TEST: " << i;
  }
}

TEST_F(CFGAS_FormatStringTest, NullParse) {
  struct {
    const wchar_t* locale;
    const wchar_t* input;
    const wchar_t* pattern;
  } tests[] = {
      {L"en", L"", L"null{}"}, {L"en", L"No data", L"null{'No data'}"},
  };

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(
        fmt(tests[i].locale)->ParseNull(tests[i].input, tests[i].pattern))
        << " TEST: " << i;
  }
}

TEST_F(CFGAS_FormatStringTest, NullFormat) {
  struct {
    const wchar_t* locale;
    const wchar_t* pattern;
    const wchar_t* output;
  } tests[] = {{L"en", L"null{'n/a'}", L"n/a"}, {L"en", L"null{}", L""}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    WideString result;
    EXPECT_TRUE(fmt(tests[i].locale)->FormatNull(tests[i].pattern, &result));
    EXPECT_STREQ(tests[i].output, result.c_str()) << " TEST: " << i;
  }
}

TEST_F(CFGAS_FormatStringTest, ZeroParse) {
  struct {
    const wchar_t* locale;
    const wchar_t* input;
    const wchar_t* pattern;
  } tests[] = {{L"en", L"", L"zero{}"},
               {L"en", L"9", L"zero{9}"},
               {L"en", L"a", L"zero{'a'}"}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    EXPECT_TRUE(
        fmt(tests[i].locale)->ParseZero(tests[i].input, tests[i].pattern))
        << " TEST: " << i;
  }
}

TEST_F(CFGAS_FormatStringTest, ZeroFormat) {
  struct {
    const wchar_t* locale;
    const wchar_t* input;
    const wchar_t* pattern;
    const wchar_t* output;
  } tests[] = {// TODO(dsinclair): The zero format can take a number specifier
               // which we don't take into account.
               // {L"en", L"", L"zero {9}", L""},
               // {L"en", L"0", L"zero {9}", L"0"},
               // {L"en", L"0.0", L"zero{9}", L"0"},
               {L"en", L"0", L"zero{}", L""}};

  for (size_t i = 0; i < FX_ArraySize(tests); ++i) {
    WideString result;
    EXPECT_TRUE(
        fmt(tests[i].locale)
            ->FormatZero(/* tests[i].input,*/ tests[i].pattern, &result));
    EXPECT_STREQ(tests[i].output, result.c_str()) << " TEST: " << i;
  }
}

TEST_F(CFGAS_FormatStringTest, GetCategory) {
  CFGAS_FormatString* f = fmt(L"en");

  EXPECT_EQ(FX_LOCALECATEGORY_Unknown, f->GetCategory(L"'just text'"));
  EXPECT_EQ(FX_LOCALECATEGORY_Null, f->GetCategory(L"null{}"));
  EXPECT_EQ(FX_LOCALECATEGORY_Zero, f->GetCategory(L"zero{}"));
  EXPECT_EQ(FX_LOCALECATEGORY_Num, f->GetCategory(L"num{}"));
  EXPECT_EQ(FX_LOCALECATEGORY_Text, f->GetCategory(L"text{}"));
  EXPECT_EQ(FX_LOCALECATEGORY_DateTime, f->GetCategory(L"datetime{}"));
  EXPECT_EQ(FX_LOCALECATEGORY_Time, f->GetCategory(L"time{}"));
  EXPECT_EQ(FX_LOCALECATEGORY_Date, f->GetCategory(L"date{}"));
  EXPECT_EQ(FX_LOCALECATEGORY_DateTime, f->GetCategory(L"time{} date{}"));
  EXPECT_EQ(FX_LOCALECATEGORY_DateTime, f->GetCategory(L"date{} time{}"));
  EXPECT_EQ(FX_LOCALECATEGORY_Num, f->GetCategory(L"num(en_GB){}"));
  EXPECT_EQ(FX_LOCALECATEGORY_Date, f->GetCategory(L"date.long{}"));
}

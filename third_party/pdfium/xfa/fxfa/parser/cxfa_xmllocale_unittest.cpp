// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xfa/fxfa/parser/cxfa_xmllocale.h"

#include <memory>

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

namespace {

const char kXMLData[] =
    "<locale name=\"en_US\" desc=\"English(America)\">"
    "<calendarSymbols name=\"gregorian\"><monthNames><month>January</month>"
    "<month>February</month>"
    "<month>March</month>"
    "<month>April</month>"
    "<month>May</month>"
    "<month>June</month>"
    "<month>July</month>"
    "<month>August</month>"
    "<month>September</month>"
    "<month>October</month>"
    "<month>November</month>"
    "<month>December</month>"
    "</monthNames>"
    "<monthNames abbr=\"1\"><month>Jan</month>"
    "<month>Feb</month>"
    "<month>Mar</month>"
    "<month>Apr</month>"
    "<month>May</month>"
    "<month>Jun</month>"
    "<month>Jul</month>"
    "<month>Aug</month>"
    "<month>Sep</month>"
    "<month>Oct</month>"
    "<month>Nov</month>"
    "<month>Dec</month>"
    "</monthNames>"
    "<dayNames><day>Sunday</day>"
    "<day>Monday</day>"
    "<day>Tuesday</day>"
    "<day>Wednesday</day>"
    "<day>Thursday</day>"
    "<day>Friday</day>"
    "<day>Saturday</day>"
    "</dayNames>"
    "<dayNames abbr=\"1\"><day>Sun</day>"
    "<day>Mon</day>"
    "<day>Tue</day>"
    "<day>Wed</day>"
    "<day>Thu</day>"
    "<day>Fri</day>"
    "<day>Sat</day>"
    "</dayNames>"
    "<meridiemNames><meridiem>AM</meridiem>"
    "<meridiem>PM</meridiem>"
    "</meridiemNames>"
    "<eraNames><era>BC</era>"
    "<era>AD</era>"
    "</eraNames>"
    "</calendarSymbols>"
    "<datePatterns><datePattern name=\"full\">EEEE, MMMM D, YYYY</datePattern>"
    "<datePattern name=\"long\">MMMM D, YYYY</datePattern>"
    "<datePattern name=\"med\">MMM D, YYYY</datePattern>"
    "<datePattern name=\"short\">M/D/YY</datePattern>"
    "</datePatterns>"
    "<timePatterns><timePattern name=\"full\">h:MM:SS A Z</timePattern>"
    "<timePattern name=\"long\">h:MM:SS A Z</timePattern>"
    "<timePattern name=\"med\">h:MM:SS A</timePattern>"
    "<timePattern name=\"short\">h:MM A</timePattern>"
    "</timePatterns>"
    "<dateTimeSymbols>GyMdkHmsSEDFwWahKzZ</dateTimeSymbols>"
    "<numberPatterns><numberPattern name=\"numeric\">z,zz9.zzz</numberPattern>"
    "<numberPattern name=\"currency\">$z,zz9.99|($z,zz9.99)</numberPattern>"
    "<numberPattern name=\"percent\">z,zz9%</numberPattern>"
    "</numberPatterns>"
    "<numberSymbols><numberSymbol name=\"decimal\">.</numberSymbol>"
    "<numberSymbol name=\"grouping\">,</numberSymbol>"
    "<numberSymbol name=\"percent\">%</numberSymbol>"
    "<numberSymbol name=\"minus\">-</numberSymbol>"
    "<numberSymbol name=\"zero\">0</numberSymbol>"
    "</numberSymbols>"
    "<currencySymbols><currencySymbol name=\"symbol\">$</currencySymbol>"
    "<currencySymbol name=\"isoname\">USD</currencySymbol>"
    "<currencySymbol name=\"decimal\">.</currencySymbol>"
    "</currencySymbols>"
    "</locale>";

std::unique_ptr<CXFA_XMLLocale> CreateLocaleHelper() {
  return CXFA_XMLLocale::Create(pdfium::as_writable_bytes(
      pdfium::make_span(const_cast<char*>(kXMLData), strlen(kXMLData))));
}

}  // namespace

TEST(CXFA_XMLLocaleTest, Create) {
  auto locale = CreateLocaleHelper();
  EXPECT_TRUE(locale != nullptr);
}

TEST(CXFA_XMLLocaleTest, CreateBadXML) {
  auto locale = CXFA_XMLLocale::Create(pdfium::span<uint8_t>());
  EXPECT_TRUE(locale == nullptr);
}

TEST(CXFA_XMLLocaleTest, GetName) {
  auto locale = CreateLocaleHelper();
  ASSERT_TRUE(locale != nullptr);

  EXPECT_EQ(L"en_US", locale->GetName());
}

TEST(CXFA_XMLLocaleTest, GetNumericSymbols) {
  auto locale = CreateLocaleHelper();
  ASSERT_TRUE(locale != nullptr);

  EXPECT_EQ(L".", locale->GetDecimalSymbol());
  EXPECT_EQ(L",", locale->GetGroupingSymbol());
  EXPECT_EQ(L"%", locale->GetPercentSymbol());
  EXPECT_EQ(L"-", locale->GetMinusSymbol());
  EXPECT_EQ(L"$", locale->GetCurrencySymbol());
}

TEST(CXFA_XMLLocaleTest, GetDateTimeSymbols) {
  auto locale = CreateLocaleHelper();
  ASSERT_TRUE(locale != nullptr);

  EXPECT_EQ(L"GyMdkHmsSEDFwWahKzZ", locale->GetDateTimeSymbols());
}

TEST(CXFA_XMLLocaleTest, GetMonthName) {
  auto locale = CreateLocaleHelper();
  ASSERT_TRUE(locale != nullptr);

  EXPECT_EQ(L"", locale->GetMonthName(24, false));
  EXPECT_EQ(L"", locale->GetMonthName(-5, false));
  EXPECT_EQ(L"Feb", locale->GetMonthName(1, true));
  EXPECT_EQ(L"February", locale->GetMonthName(1, false));
}

TEST(CXFA_XMLLocaleTest, GetDayName) {
  auto locale = CreateLocaleHelper();
  ASSERT_TRUE(locale != nullptr);

  EXPECT_EQ(L"", locale->GetDayName(24, false));
  EXPECT_EQ(L"", locale->GetDayName(-5, false));
  EXPECT_EQ(L"Mon", locale->GetDayName(1, true));
  EXPECT_EQ(L"Monday", locale->GetDayName(1, false));
}

TEST(CXFA_XMLLocaleTest, GetMeridiemName) {
  auto locale = CreateLocaleHelper();
  ASSERT_TRUE(locale != nullptr);

  EXPECT_EQ(L"AM", locale->GetMeridiemName(true));
  EXPECT_EQ(L"PM", locale->GetMeridiemName(false));
}

TEST(CXFA_XMLLocaleTest, GetEraName) {
  auto locale = CreateLocaleHelper();
  ASSERT_TRUE(locale != nullptr);

  EXPECT_EQ(L"AD", locale->GetEraName(true));
  EXPECT_EQ(L"BC", locale->GetEraName(false));
}

TEST(CXFA_XMLLocaleTest, GetDatePattern) {
  auto locale = CreateLocaleHelper();
  ASSERT_TRUE(locale != nullptr);

  EXPECT_EQ(L"M/D/YY",
            locale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Short));
  EXPECT_EQ(L"MMM D, YYYY",
            locale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Default));
  EXPECT_EQ(L"MMM D, YYYY",
            locale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Medium));
  EXPECT_EQ(L"EEEE, MMMM D, YYYY",
            locale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Full));
  EXPECT_EQ(L"MMMM D, YYYY",
            locale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Long));
}

TEST(CXFA_XMLLocaleTest, GetTimePattern) {
  auto locale = CreateLocaleHelper();
  ASSERT_TRUE(locale != nullptr);

  EXPECT_EQ(L"h:MM A",
            locale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Short));
  EXPECT_EQ(L"h:MM:SS A",
            locale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Default));
  EXPECT_EQ(L"h:MM:SS A",
            locale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Medium));
  EXPECT_EQ(L"h:MM:SS A Z",
            locale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Full));
  EXPECT_EQ(L"h:MM:SS A Z",
            locale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Long));
}

TEST(CXFA_XMLLocaleTest, GetNumPattern) {
  auto locale = CreateLocaleHelper();
  ASSERT_TRUE(locale != nullptr);

  EXPECT_EQ(L"z,zzz,zzz,zzz,zzz,zzz%",
            locale->GetNumPattern(FX_LOCALENUMPATTERN_Percent));
  EXPECT_EQ(L"$z,zzz,zzz,zzz,zzz,zz9.99",
            locale->GetNumPattern(FX_LOCALENUMPATTERN_Currency));
  EXPECT_EQ(L"z,zzz,zzz,zzz,zzz,zz9.zzz",
            locale->GetNumPattern(FX_LOCALENUMPATTERN_Decimal));
  EXPECT_EQ(L"z,zzz,zzz,zzz,zzz,zzz",
            locale->GetNumPattern(FX_LOCALENUMPATTERN_Integer));
}

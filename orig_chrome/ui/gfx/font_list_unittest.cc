// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/font_list.h"

#include <algorithm>
#include <string>
#include <vector>

#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "build/build_config.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

// Helper function for comparing fonts for equality.
std::string FontToString(const gfx::Font& font)
{
    std::string font_string = font.GetFontName();
    font_string += "|";
    font_string += base::IntToString(font.GetFontSize());
    int style = font.GetStyle();
    if (style & gfx::Font::BOLD)
        font_string += "|bold";
    if (style & gfx::Font::ITALIC)
        font_string += "|italic";
    if (style & gfx::Font::UNDERLINE)
        font_string += "|underline";
    return font_string;
}

} // namespace

namespace gfx {

TEST(FontListTest, ParseDescription)
{
    std::vector<std::string> families;
    int style = gfx::Font::NORMAL;
    int size_pixels = 0;

    // Parse a well-formed description containing styles and a size.
    EXPECT_TRUE(FontList::ParseDescription("Arial,Helvetica,Bold Italic 12px",
        &families, &style, &size_pixels));
    ASSERT_EQ(2U, families.size());
    EXPECT_EQ("Arial", families[0]);
    EXPECT_EQ("Helvetica", families[1]);
    EXPECT_EQ(gfx::Font::BOLD | gfx::Font::ITALIC, style);
    EXPECT_EQ(12, size_pixels);

    // Whitespace should be removed.
    EXPECT_TRUE(FontList::ParseDescription("  Verdana , Italic  Bold   10px ",
        &families, &style, &size_pixels));
    ASSERT_EQ(1U, families.size());
    EXPECT_EQ("Verdana", families[0]);
    EXPECT_EQ(gfx::Font::BOLD | gfx::Font::ITALIC, style);
    EXPECT_EQ(10, size_pixels);

    // Invalid descriptions should be rejected.
    EXPECT_FALSE(FontList::ParseDescription("", &families, &style, &size_pixels));
    EXPECT_FALSE(FontList::ParseDescription("Arial", &families, &style,
        &size_pixels));
    EXPECT_FALSE(FontList::ParseDescription("Arial,12", &families, &style,
        &size_pixels));
    EXPECT_FALSE(FontList::ParseDescription("Arial 12px", &families, &style,
        &size_pixels));
    EXPECT_FALSE(FontList::ParseDescription("Arial,12px,", &families, &style,
        &size_pixels));
    EXPECT_FALSE(FontList::ParseDescription("Arial,0px", &families, &style,
        &size_pixels));
    EXPECT_FALSE(FontList::ParseDescription("Arial,-1px", &families, &style,
        &size_pixels));
    EXPECT_FALSE(FontList::ParseDescription("Arial,foo 12px", &families, &style,
        &size_pixels));
}

// TODO(489354): Enable this on android.
#if defined(OS_ANDROID)
#define MAYBE_Fonts_FromDescString DISABLED_Fonts_FromDescString
#else
#define MAYBE_Fonts_FromDescString Fonts_FromDescString
#endif
TEST(FontListTest, MAYBE_Fonts_FromDescString)
{
    // Test init from font name size string.
    FontList font_list = FontList("arial, Courier New, 13px");
    const std::vector<Font>& fonts = font_list.GetFonts();
    ASSERT_EQ(2U, fonts.size());
    EXPECT_EQ("arial|13", FontToString(fonts[0]));
    EXPECT_EQ("Courier New|13", FontToString(fonts[1]));
}

// TODO(489354): Enable this on android.
#if defined(OS_ANDROID)
#define MAYBE_Fonts_FromDescStringInFlexibleFormat \
    DISABLED_Fonts_FromDescStringInFlexibleFormat
#else
#define MAYBE_Fonts_FromDescStringInFlexibleFormat \
    Fonts_FromDescStringInFlexibleFormat
#endif
TEST(FontListTest, MAYBE_Fonts_FromDescStringInFlexibleFormat)
{
    // Test init from font name size string with flexible format.
    FontList font_list = FontList("  arial   ,   Courier New ,   13px");
    const std::vector<Font>& fonts = font_list.GetFonts();
    ASSERT_EQ(2U, fonts.size());
    EXPECT_EQ("arial|13", FontToString(fonts[0]));
    EXPECT_EQ("Courier New|13", FontToString(fonts[1]));
}

// TODO(489354): Enable this on android.
#if defined(OS_ANDROID)
#define MAYBE_Fonts_FromDescStringWithStyleInFlexibleFormat \
    DISABLED_Fonts_FromDescStringWithStyleInFlexibleFormat
#else
#define MAYBE_Fonts_FromDescStringWithStyleInFlexibleFormat \
    Fonts_FromDescStringWithStyleInFlexibleFormat
#endif
TEST(FontListTest, MAYBE_Fonts_FromDescStringWithStyleInFlexibleFormat)
{
    // Test init from font name style size string with flexible format.
    FontList font_list = FontList("  arial  ,  Courier New ,  Bold   "
                                  "  Italic   13px");
    const std::vector<Font>& fonts = font_list.GetFonts();
    ASSERT_EQ(2U, fonts.size());
    EXPECT_EQ("arial|13|bold|italic", FontToString(fonts[0]));
    EXPECT_EQ("Courier New|13|bold|italic", FontToString(fonts[1]));
}

// TODO(489354): Enable this on android.
#if defined(OS_ANDROID)
#define MAYBE_Fonts_FromFont DISABLED_Fonts_FromFont
#else
#define MAYBE_Fonts_FromFont Fonts_FromFont
#endif
TEST(FontListTest, MAYBE_Fonts_FromFont)
{
    // Test init from Font.
    Font font("Arial", 8);
    FontList font_list = FontList(font);
    const std::vector<Font>& fonts = font_list.GetFonts();
    ASSERT_EQ(1U, fonts.size());
    EXPECT_EQ("Arial|8", FontToString(fonts[0]));
}

// TODO(489354): Enable this on android.
#if defined(OS_ANDROID)
#define MAYBE_Fonts_FromFontWithNonNormalStyle \
    DISABLED_Fonts_FromFontWithNonNormalStyle
#else
#define MAYBE_Fonts_FromFontWithNonNormalStyle Fonts_FromFontWithNonNormalStyle
#endif
TEST(FontListTest, MAYBE_Fonts_FromFontWithNonNormalStyle)
{
    // Test init from Font with non-normal style.
    Font font("Arial", 8);
    FontList font_list = FontList(font.Derive(2, Font::BOLD));
    std::vector<Font> fonts = font_list.GetFonts();
    ASSERT_EQ(1U, fonts.size());
    EXPECT_EQ("Arial|10|bold", FontToString(fonts[0]));

    font_list = FontList(font.Derive(-2, Font::ITALIC));
    fonts = font_list.GetFonts();
    ASSERT_EQ(1U, fonts.size());
    EXPECT_EQ("Arial|6|italic", FontToString(fonts[0]));
}

// TODO(489354): Enable this on android.
#if defined(OS_ANDROID)
#define MAYBE_Fonts_FromFontVector DISABLED_Fonts_FromFontVector
#else
#define MAYBE_Fonts_FromFontVector Fonts_FromFontVector
#endif
TEST(FontListTest, MAYBE_Fonts_FromFontVector)
{
    // Test init from Font vector.
    Font font("Arial", 8);
    Font font_1("Courier New", 10);
    std::vector<Font> input_fonts;
    input_fonts.push_back(font.Derive(0, Font::BOLD));
    input_fonts.push_back(font_1.Derive(-2, Font::BOLD));
    FontList font_list = FontList(input_fonts);
    const std::vector<Font>& fonts = font_list.GetFonts();
    ASSERT_EQ(2U, fonts.size());
    EXPECT_EQ("Arial|8|bold", FontToString(fonts[0]));
    EXPECT_EQ("Courier New|8|bold", FontToString(fonts[1]));
}

TEST(FontListTest, FontDescString_GetStyle)
{
    FontList font_list = FontList("Arial,Sans serif, 8px");
    EXPECT_EQ(Font::NORMAL, font_list.GetFontStyle());

    font_list = FontList("Arial,Sans serif,Bold 8px");
    EXPECT_EQ(Font::BOLD, font_list.GetFontStyle());

    font_list = FontList("Arial,Sans serif,Italic 8px");
    EXPECT_EQ(Font::ITALIC, font_list.GetFontStyle());

    font_list = FontList("Arial,Italic Bold 8px");
    EXPECT_EQ(Font::BOLD | Font::ITALIC, font_list.GetFontStyle());
}

// TODO(489354): Enable this on android.
#if defined(OS_ANDROID)
#define MAYBE_Fonts_GetStyle DISABLED_Fonts_GetStyle
#else
#define MAYBE_Fonts_GetStyle Fonts_GetStyle
#endif
TEST(FontListTest, MAYBE_Fonts_GetStyle)
{
    std::vector<Font> fonts;
    fonts.push_back(gfx::Font("Arial", 8));
    fonts.push_back(gfx::Font("Sans serif", 8));
    FontList font_list = FontList(fonts);
    EXPECT_EQ(Font::NORMAL, font_list.GetFontStyle());
    fonts[0] = fonts[0].Derive(0, Font::ITALIC | Font::BOLD);
    fonts[1] = fonts[1].Derive(0, Font::ITALIC | Font::BOLD);
    font_list = FontList(fonts);
    EXPECT_EQ(Font::ITALIC | Font::BOLD, font_list.GetFontStyle());
}

// TODO(489354): Enable this on android.
#if defined(OS_ANDROID)
#define MAYBE_Fonts_Derive DISABLED_Fonts_Derive
#else
#define MAYBE_Fonts_Derive Fonts_Derive
#endif
TEST(FontListTest, MAYBE_Fonts_Derive)
{
    std::vector<Font> fonts;
    fonts.push_back(gfx::Font("Arial", 8));
    fonts.push_back(gfx::Font("Courier New", 8));
    FontList font_list = FontList(fonts);

    FontList derived = font_list.Derive(5, Font::BOLD | Font::UNDERLINE);
    const std::vector<Font>& derived_fonts = derived.GetFonts();

    EXPECT_EQ(2U, derived_fonts.size());
    EXPECT_EQ("Arial|13|bold|underline", FontToString(derived_fonts[0]));
    EXPECT_EQ("Courier New|13|bold|underline", FontToString(derived_fonts[1]));
}

// TODO(489354): Enable this on android.
#if defined(OS_ANDROID)
#define MAYBE_Fonts_DeriveWithSizeDelta DISABLED_Fonts_DeriveWithSizeDelta
#else
#define MAYBE_Fonts_DeriveWithSizeDelta Fonts_DeriveWithSizeDelta
#endif
TEST(FontListTest, MAYBE_Fonts_DeriveWithSizeDelta)
{
    std::vector<Font> fonts;
    fonts.push_back(gfx::Font("Arial", 18).Derive(0, Font::ITALIC));
    fonts.push_back(gfx::Font("Courier New", 18).Derive(0, Font::ITALIC));
    FontList font_list = FontList(fonts);

    FontList derived = font_list.DeriveWithSizeDelta(-5);
    const std::vector<Font>& derived_fonts = derived.GetFonts();

    EXPECT_EQ(2U, derived_fonts.size());
    EXPECT_EQ("Arial|13|italic", FontToString(derived_fonts[0]));
    EXPECT_EQ("Courier New|13|italic", FontToString(derived_fonts[1]));
}

// TODO(489354): Enable this on android.
#if defined(OS_ANDROID)
#define MAYBE_Fonts_GetHeight_GetBaseline DISABLED_Fonts_GetHeight_GetBaseline
#else
#define MAYBE_Fonts_GetHeight_GetBaseline Fonts_GetHeight_GetBaseline
#endif
TEST(FontListTest, MAYBE_Fonts_GetHeight_GetBaseline)
{
    // If a font list has only one font, the height and baseline must be the same.
    Font font1("Arial", 16);
    ASSERT_EQ("arial", base::ToLowerASCII(font1.GetActualFontNameForTesting()));
    FontList font_list1("Arial, 16px");
    EXPECT_EQ(font1.GetHeight(), font_list1.GetHeight());
    EXPECT_EQ(font1.GetBaseline(), font_list1.GetBaseline());

    // If there are two different fonts, the font list returns the max value
    // for the baseline (ascent) and height.
    Font font2("Symbol", 16);
    ASSERT_EQ("symbol", base::ToLowerASCII(font2.GetActualFontNameForTesting()));
    EXPECT_NE(font1.GetBaseline(), font2.GetBaseline());
    // TODO(ananta): Find a size and font pair with reliably distinct descents.
    EXPECT_NE(font1.GetHeight(), font2.GetHeight());
    std::vector<Font> fonts;
    fonts.push_back(font1);
    fonts.push_back(font2);
    FontList font_list_mix(fonts);
    // ascent of FontList == max(ascent of Fonts)
    EXPECT_EQ(std::max(font1.GetBaseline(), font2.GetBaseline()),
        font_list_mix.GetBaseline());
    // descent of FontList == max(descent of Fonts)
    EXPECT_EQ(std::max(font1.GetHeight() - font1.GetBaseline(),
                  font2.GetHeight() - font2.GetBaseline()),
        font_list_mix.GetHeight() - font_list_mix.GetBaseline());
}

// TODO(489354): Enable this on android.
#if defined(OS_ANDROID)
#define MAYBE_Fonts_DeriveWithHeightUpperBound \
    DISABLED_Fonts_DeriveWithHeightUpperBound
#else
#define MAYBE_Fonts_DeriveWithHeightUpperBound Fonts_DeriveWithHeightUpperBound
#endif
TEST(FontListTest, MAYBE_Fonts_DeriveWithHeightUpperBound)
{
    std::vector<Font> fonts;

    fonts.push_back(gfx::Font("Arial", 18));
    fonts.push_back(gfx::Font("Sans serif", 18));
    fonts.push_back(gfx::Font("Symbol", 18));
    FontList font_list = FontList(fonts);

    // A smaller upper bound should derive a font list with a smaller height.
    const int height_1 = font_list.GetHeight() - 5;
    FontList derived_1 = font_list.DeriveWithHeightUpperBound(height_1);
    EXPECT_LE(derived_1.GetHeight(), height_1);
    EXPECT_LT(derived_1.GetHeight(), font_list.GetHeight());
    EXPECT_LT(derived_1.GetFontSize(), font_list.GetFontSize());

    // A larger upper bound should not change the height of the font list.
    const int height_2 = font_list.GetHeight() + 5;
    FontList derived_2 = font_list.DeriveWithHeightUpperBound(height_2);
    EXPECT_LE(derived_2.GetHeight(), height_2);
    EXPECT_EQ(font_list.GetHeight(), derived_2.GetHeight());
    EXPECT_EQ(font_list.GetFontSize(), derived_2.GetFontSize());
}

} // namespace gfx

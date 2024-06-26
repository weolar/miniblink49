// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/platform_font_win.h"

#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "base/strings/string16.h"
#include "base/strings/utf_string_conversions.h"
#include "base/win/scoped_hdc.h"
#include "base/win/scoped_select_object.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/font.h"
#include "ui/gfx/win/direct_write.h"
#include "ui/gfx/win/scoped_set_map_mode.h"

namespace gfx {

TEST(PlatformFontWinTest, DeriveFontWithHeight)
{
    const Font base_font;
    PlatformFontWin* platform_font = static_cast<PlatformFontWin*>(base_font.platform_font());

    for (int i = -10; i < 10; i++) {
        const int target_height = base_font.GetHeight() + i;

        Font derived_font = platform_font->DeriveFontWithHeight(target_height, 0);
        EXPECT_LE(derived_font.GetHeight(), target_height);
        EXPECT_GT(derived_font.Derive(1, 0).GetHeight(), target_height);
        EXPECT_EQ(platform_font->GetActualFontNameForTesting(),
            derived_font.GetActualFontNameForTesting());
        EXPECT_EQ(0, derived_font.GetStyle());

        derived_font = platform_font->DeriveFontWithHeight(target_height,
            Font::BOLD);
        EXPECT_LE(derived_font.GetHeight(), target_height);
        EXPECT_GT(derived_font.Derive(1, 0).GetHeight(), target_height);
        EXPECT_EQ(platform_font->GetActualFontNameForTesting(),
            derived_font.GetActualFontNameForTesting());
        EXPECT_EQ(Font::BOLD, derived_font.GetStyle());
    }
}

TEST(PlatformFontWinTest, DeriveFontWithHeight_Consistency)
{
    gfx::Font arial_12("Arial", 12);
    ASSERT_GT(16, arial_12.GetHeight());
    gfx::Font derived_1 = static_cast<PlatformFontWin*>(
        arial_12.platform_font())
                              ->DeriveFontWithHeight(16, 0);

    gfx::Font arial_15("Arial", 15);
    ASSERT_LT(16, arial_15.GetHeight());
    gfx::Font derived_2 = static_cast<PlatformFontWin*>(
        arial_15.platform_font())
                              ->DeriveFontWithHeight(16, 0);

    EXPECT_EQ(derived_1.GetFontSize(), derived_2.GetFontSize());
    EXPECT_EQ(16, derived_1.GetHeight());
    EXPECT_EQ(16, derived_2.GetHeight());
}

// Callback function used by DeriveFontWithHeight_MinSize() below.
static int GetMinFontSize()
{
    return 10;
}

TEST(PlatformFontWinTest, DeriveFontWithHeight_MinSize)
{
    PlatformFontWin::GetMinimumFontSizeCallback old_callback = PlatformFontWin::get_minimum_font_size_callback;
    PlatformFontWin::get_minimum_font_size_callback = &GetMinFontSize;

    const Font base_font;
    const Font min_font(base_font.GetFontName(), GetMinFontSize());
    PlatformFontWin* platform_font = static_cast<PlatformFontWin*>(base_font.platform_font());

    const Font derived_font = platform_font->DeriveFontWithHeight(min_font.GetHeight() - 1, 0);
    EXPECT_EQ(min_font.GetFontSize(), derived_font.GetFontSize());
    EXPECT_EQ(min_font.GetHeight(), derived_font.GetHeight());

    PlatformFontWin::get_minimum_font_size_callback = old_callback;
}

TEST(PlatformFontWinTest, DeriveFontWithHeight_TooSmall)
{
    const Font base_font;
    PlatformFontWin* platform_font = static_cast<PlatformFontWin*>(base_font.platform_font());

    const Font derived_font = platform_font->DeriveFontWithHeight(1, 0);
    EXPECT_GT(derived_font.GetHeight(), 1);
}

// Test whether font metrics retrieved by DirectWrite (skia) and GDI match as
// per assumptions mentioned below:-
// 1. Font size is the same
// 2. The difference between GDI and DirectWrite for font height, baseline,
//    and cap height is at most 1. For smaller font sizes under 12, GDI
//    font heights/baselines/cap height are equal/larger by 1 point. For larger
//    font sizes DirectWrite font heights/baselines/cap height are equal/larger
//    by 1 point.
TEST(PlatformFontWinTest, Metrics_SkiaVersusGDI)
{
    if (!gfx::win::IsDirectWriteEnabled())
        return;

    // Describes the font being tested.
    struct FontInfo {
        base::string16 font_name;
        int font_size;
    };

    FontInfo fonts[] = {
        { base::ASCIIToUTF16("Arial"), 6 },
        { base::ASCIIToUTF16("Arial"), 8 },
        { base::ASCIIToUTF16("Arial"), 10 },
        { base::ASCIIToUTF16("Arial"), 12 },
        { base::ASCIIToUTF16("Arial"), 16 },
        { base::ASCIIToUTF16("Symbol"), 6 },
        { base::ASCIIToUTF16("Symbol"), 10 },
        { base::ASCIIToUTF16("Symbol"), 12 },
        { base::ASCIIToUTF16("Tahoma"), 10 },
        { base::ASCIIToUTF16("Tahoma"), 16 },
        { base::ASCIIToUTF16("Segoe UI"), 6 },
        { base::ASCIIToUTF16("Segoe UI"), 8 },
        { base::ASCIIToUTF16("Segoe UI"), 20 },
    };

    base::win::ScopedGetDC screen_dc(NULL);
    gfx::ScopedSetMapMode mode(screen_dc, MM_TEXT);

    for (const FontInfo& font : fonts) {
        LOGFONT font_info = { 0 };

        font_info.lfHeight = -font.font_size;
        font_info.lfWeight = FW_NORMAL;
        wcscpy_s(font_info.lfFaceName, font.font_name.length() + 1,
            font.font_name.c_str());

        HFONT hFont = CreateFontIndirect(&font_info);

        TEXTMETRIC font_metrics;
        PlatformFontWin::GetTextMetricsForFont(screen_dc, hFont, &font_metrics);

        scoped_refptr<PlatformFontWin::HFontRef> h_font_gdi(
            PlatformFontWin::CreateHFontRefFromGDI(hFont, font_metrics));

        scoped_refptr<PlatformFontWin::HFontRef> h_font_skia(
            PlatformFontWin::CreateHFontRefFromSkia(hFont, font_metrics));

        EXPECT_EQ(h_font_gdi->font_size(), h_font_skia->font_size());
        EXPECT_EQ(h_font_gdi->style(), h_font_skia->style());
        EXPECT_EQ(h_font_gdi->font_name(), h_font_skia->font_name());
        EXPECT_EQ(h_font_gdi->ave_char_width(), h_font_skia->ave_char_width());

        EXPECT_LE(abs(h_font_gdi->cap_height() - h_font_skia->cap_height()), 1);
        EXPECT_LE(abs(h_font_gdi->baseline() - h_font_skia->baseline()), 1);
        EXPECT_LE(abs(h_font_gdi->height() - h_font_skia->height()), 1);
    }
}

// Test if DirectWrite font fallback works correctly, i.e. whether DirectWrite
// fonts handle the font names in the
// HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\FontSubstitutes key
// correctly. The expectation is that the actual font created should be the
// one specified by the value for the substituted font. For e.g. MS Shell Dlg
// should create Microsoft Sans Serif, etc.
// For random fonts which are not substitutes, DirectWrite should fallback
// to Arial on a properly configured machine.
TEST(PlatformFontWinTest, DirectWriteFontSubstitution)
{
    if (!gfx::win::IsDirectWriteEnabled())
        return;

    // Describes the font being tested.
    struct FontInfo {
        base::string16 font_name;
        std::string expected_font_name;
    };

    FontInfo fonts[] = {
        { base::ASCIIToUTF16("MS Shell Dlg"), "Microsoft Sans Serif" },
        { base::ASCIIToUTF16("MS Shell Dlg 2"), "Tahoma" },
        { base::ASCIIToUTF16("FooBar"), "Arial" },
    };

    base::win::ScopedGetDC screen_dc(NULL);
    gfx::ScopedSetMapMode mode(screen_dc, MM_TEXT);

    for (const FontInfo& font : fonts) {
        LOGFONT font_info = { 0 };

        font_info.lfHeight = -10;
        font_info.lfWeight = FW_NORMAL;
        wcscpy_s(font_info.lfFaceName, font.font_name.length() + 1,
            font.font_name.c_str());

        HFONT hFont = CreateFontIndirect(&font_info);

        TEXTMETRIC font_metrics;
        PlatformFontWin::GetTextMetricsForFont(screen_dc, hFont, &font_metrics);

        scoped_refptr<PlatformFontWin::HFontRef> h_font_skia(
            PlatformFontWin::CreateHFontRefFromSkia(hFont, font_metrics));

        EXPECT_EQ(font.expected_font_name, h_font_skia->font_name());
    }
}

} // namespace gfx

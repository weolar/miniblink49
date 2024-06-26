// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/macros.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/font_fallback_win.h"

namespace gfx {

namespace {

    // Subclass of LinkedFontsIterator for testing that allows mocking the linked
    // fonts vector.
    class TestLinkedFontsIterator : public internal::LinkedFontsIterator {
    public:
        explicit TestLinkedFontsIterator(Font font)
            : LinkedFontsIterator(font)
        {
        }

        ~TestLinkedFontsIterator() override { }

        // Add a linked font to the mocked vector of linked fonts.
        void AddLinkedFontForTesting(Font font)
        {
            test_linked_fonts.push_back(font);
        }

        const std::vector<Font>* GetLinkedFonts() const override
        {
            return &test_linked_fonts;
        }

    private:
        std::vector<Font> test_linked_fonts;

        DISALLOW_COPY_AND_ASSIGN(TestLinkedFontsIterator);
    };

} // namespace

TEST(FontFallbackWinTest, ParseFontLinkEntry)
{
    std::string file;
    std::string font;

    internal::ParseFontLinkEntry("TAHOMA.TTF", &file, &font);
    EXPECT_EQ("TAHOMA.TTF", file);
    EXPECT_EQ("", font);

    internal::ParseFontLinkEntry("MSGOTHIC.TTC,MS UI Gothic", &file, &font);
    EXPECT_EQ("MSGOTHIC.TTC", file);
    EXPECT_EQ("MS UI Gothic", font);

    internal::ParseFontLinkEntry("MALGUN.TTF,128,96", &file, &font);
    EXPECT_EQ("MALGUN.TTF", file);
    EXPECT_EQ("", font);

    internal::ParseFontLinkEntry("MEIRYO.TTC,Meiryo,128,85", &file, &font);
    EXPECT_EQ("MEIRYO.TTC", file);
    EXPECT_EQ("Meiryo", font);
}

TEST(FontFallbackWinTest, ParseFontFamilyString)
{
    std::vector<std::string> font_names;

    internal::ParseFontFamilyString("Times New Roman (TrueType)", &font_names);
    ASSERT_EQ(1U, font_names.size());
    EXPECT_EQ("Times New Roman", font_names[0]);
    font_names.clear();

    internal::ParseFontFamilyString("Cambria & Cambria Math (TrueType)",
        &font_names);
    ASSERT_EQ(2U, font_names.size());
    EXPECT_EQ("Cambria", font_names[0]);
    EXPECT_EQ("Cambria Math", font_names[1]);
    font_names.clear();

    internal::ParseFontFamilyString(
        "Meiryo & Meiryo Italic & Meiryo UI & Meiryo UI Italic (TrueType)",
        &font_names);
    ASSERT_EQ(4U, font_names.size());
    EXPECT_EQ("Meiryo", font_names[0]);
    EXPECT_EQ("Meiryo Italic", font_names[1]);
    EXPECT_EQ("Meiryo UI", font_names[2]);
    EXPECT_EQ("Meiryo UI Italic", font_names[3]);
}

TEST(FontFallbackWinTest, LinkedFontsIterator)
{
    TestLinkedFontsIterator iterator(Font("Arial", 16));
    iterator.AddLinkedFontForTesting(Font("Times New Roman", 16));

    Font font;
    EXPECT_TRUE(iterator.NextFont(&font));
    ASSERT_EQ("Arial", font.GetFontName());

    EXPECT_TRUE(iterator.NextFont(&font));
    ASSERT_EQ("Times New Roman", font.GetFontName());

    EXPECT_FALSE(iterator.NextFont(&font));
}

TEST(FontFallbackWinTest, LinkedFontsIteratorSetNextFont)
{
    TestLinkedFontsIterator iterator(Font("Arial", 16));
    iterator.AddLinkedFontForTesting(Font("Times New Roman", 16));

    Font font;
    EXPECT_TRUE(iterator.NextFont(&font));
    ASSERT_EQ("Arial", font.GetFontName());

    iterator.SetNextFont(Font("Tahoma", 16));
    EXPECT_TRUE(iterator.NextFont(&font));
    ASSERT_EQ("Tahoma", font.GetFontName());

    EXPECT_TRUE(iterator.NextFont(&font));
    ASSERT_EQ("Times New Roman", font.GetFontName());

    EXPECT_FALSE(iterator.NextFont(&font));
}

} // namespace gfx

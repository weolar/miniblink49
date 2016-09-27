// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "platform/fonts/FontCache.h"
#include "platform/fonts/GlyphBuffer.h"
#include "platform/fonts/shaping/CachingWordShapeIterator.h"
#include <gtest/gtest.h>

namespace blink {

class CachingWordShaperTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        fontDescription.setComputedSize(12.0);
        fontDescription.setScript(USCRIPT_LATIN);
        fontDescription.setGenericFamily(FontDescription::StandardFamily);

        font = new Font(fontDescription);
        font->update(nullptr);
        ASSERT_TRUE(font->canShapeWordByWord());
        fallbackFonts = nullptr;
        cache = new ShapeCache();
    }

    void TearDown() override
    {
        delete cache;
        delete font;
    }

    FontCachePurgePreventer fontCachePurgePreventer;
    FontDescription fontDescription;
    Font* font;
    ShapeCache* cache;
    HashSet<const SimpleFontData*>* fallbackFonts;
    unsigned startIndex = 0;
    unsigned numGlyphs = 0;
    hb_script_t script = HB_SCRIPT_INVALID;
};

TEST_F(CachingWordShaperTest, LatinLeftToRightByWord)
{
    TextRun textRun(reinterpret_cast<const LChar*>("ABC DEF."), 8);

    RefPtr<ShapeResult> result;
    CachingWordShapeIterator iterator(cache, textRun, font, fallbackFonts);
    ASSERT_TRUE(iterator.next(&result));
    ASSERT_TRUE(result->runInfoForTesting(0, startIndex, numGlyphs, script));
    EXPECT_EQ(0u, startIndex);
    EXPECT_EQ(3u, numGlyphs);
    EXPECT_EQ(HB_SCRIPT_LATIN, script);

    ASSERT_TRUE(iterator.next(&result));
    ASSERT_TRUE(result->runInfoForTesting(0, startIndex, numGlyphs, script));
    EXPECT_EQ(0u, startIndex);
    EXPECT_EQ(1u, numGlyphs);
    EXPECT_EQ(HB_SCRIPT_COMMON, script);

    ASSERT_TRUE(iterator.next(&result));
    ASSERT_TRUE(result->runInfoForTesting(0, startIndex, numGlyphs, script));
    EXPECT_EQ(0u, startIndex);
    EXPECT_EQ(4u, numGlyphs);
    EXPECT_EQ(HB_SCRIPT_LATIN, script);

    ASSERT_FALSE(iterator.next(&result));
}

TEST_F(CachingWordShaperTest, CommonAccentLeftToRightByWord)
{
    const UChar str[] = { 0x2F, 0x301, 0x2E, 0x20, 0x2E, 0x0 };
    TextRun textRun(str, 5);

    unsigned offset = 0;
    RefPtr<ShapeResult> result;
    CachingWordShapeIterator iterator(cache, textRun, font, fallbackFonts);
    ASSERT_TRUE(iterator.next(&result));
    ASSERT_TRUE(result->runInfoForTesting(0, startIndex, numGlyphs, script));
    EXPECT_EQ(0u, offset + startIndex);
    EXPECT_EQ(3u, numGlyphs);
    EXPECT_EQ(HB_SCRIPT_COMMON, script);
    offset += result->numCharacters();

    ASSERT_TRUE(iterator.next(&result));
    ASSERT_TRUE(result->runInfoForTesting(0, startIndex, numGlyphs, script));
    EXPECT_EQ(3u, offset + startIndex);
    EXPECT_EQ(1u, numGlyphs);
    EXPECT_EQ(HB_SCRIPT_COMMON, script);
    offset += result->numCharacters();

    ASSERT_TRUE(iterator.next(&result));
    ASSERT_TRUE(result->runInfoForTesting(0, startIndex, numGlyphs, script));
    EXPECT_EQ(4u, offset + startIndex);
    EXPECT_EQ(1u, numGlyphs);
    EXPECT_EQ(HB_SCRIPT_COMMON, script);
    offset += result->numCharacters();

    ASSERT_EQ(5u, offset);
    ASSERT_FALSE(iterator.next(&result));
}

// Tests that filling a glyph buffer for a specific range returns the same
// results when shaping word by word as when shaping the full run in one go.
TEST_F(CachingWordShaperTest, CommonAccentLeftToRightFillGlyphBuffer)
{
    // "/. ." with an accent mark over the first dot.
    const UChar str[] = { 0x2F, 0x301, 0x2E, 0x20, 0x2E, 0x0 };
    TextRun textRun(str, 5);

    CachingWordShaper shaper;
    GlyphBuffer glyphBuffer;
    shaper.fillGlyphBuffer(font, textRun, fallbackFonts, &glyphBuffer, 0, 3);

    CachingWordShaper referenceShaper;
    GlyphBuffer referenceGlyphBuffer;
    font->setCanShapeWordByWordForTesting(false);
    referenceShaper.fillGlyphBuffer(font, textRun, fallbackFonts,
        &referenceGlyphBuffer, 0, 3);

    ASSERT_EQ(referenceGlyphBuffer.glyphAt(0), glyphBuffer.glyphAt(0));
    ASSERT_EQ(referenceGlyphBuffer.glyphAt(1), glyphBuffer.glyphAt(1));
    ASSERT_EQ(referenceGlyphBuffer.glyphAt(2), glyphBuffer.glyphAt(2));
}

// Tests that filling a glyph buffer for a specific range returns the same
// results when shaping word by word as when shaping the full run in one go.
TEST_F(CachingWordShaperTest, CommonAccentRightToLeftFillGlyphBuffer)
{
    // "[] []" with an accent mark over the last square bracket.
    const UChar str[] = { 0x5B, 0x5D, 0x20, 0x5B, 0x301, 0x5D, 0x0 };
    TextRun textRun(str, 6);
    textRun.setDirection(RTL);

    CachingWordShaper shaper;
    GlyphBuffer glyphBuffer;
    shaper.fillGlyphBuffer(font, textRun, fallbackFonts, &glyphBuffer, 1, 6);

    CachingWordShaper referenceShaper;
    GlyphBuffer referenceGlyphBuffer;
    font->setCanShapeWordByWordForTesting(false);
    referenceShaper.fillGlyphBuffer(font, textRun, fallbackFonts,
        &referenceGlyphBuffer, 1, 6);

    ASSERT_EQ(5u, referenceGlyphBuffer.size());
    ASSERT_EQ(referenceGlyphBuffer.size(), glyphBuffer.size());

    ASSERT_EQ(referenceGlyphBuffer.glyphAt(0), glyphBuffer.glyphAt(0));
    ASSERT_EQ(referenceGlyphBuffer.glyphAt(1), glyphBuffer.glyphAt(1));
    ASSERT_EQ(referenceGlyphBuffer.glyphAt(2), glyphBuffer.glyphAt(2));
    ASSERT_EQ(referenceGlyphBuffer.glyphAt(3), glyphBuffer.glyphAt(3));
    ASSERT_EQ(referenceGlyphBuffer.glyphAt(4), glyphBuffer.glyphAt(4));
}

// Tests that runs with zero glyphs (the ZWJ non-printable character in this
// case) are handled correctly. This test passes if it does not cause a crash.
TEST_F(CachingWordShaperTest, SubRunWithZeroGlyphs)
{
    // "Foo &zwnj; bar"
    const UChar str[] = {
        0x46, 0x6F, 0x6F, 0x20, 0x200C, 0x20, 0x62, 0x61, 0x71, 0x0
    };
    TextRun textRun(str, 9);

    CachingWordShaper shaper;
    FloatRect glyphBounds;
    ASSERT_GT(shaper.width(font, textRun, nullptr, &glyphBounds), 0);

    GlyphBuffer glyphBuffer;
    shaper.fillGlyphBuffer(font, textRun, fallbackFonts, &glyphBuffer, 0, 8);

    FloatPoint point;
    int height = 16;
    shaper.selectionRect(font, textRun, point, height, 0, 8);
}


} // namespace blink

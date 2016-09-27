// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/fonts/GlyphBuffer.h"

#include "platform/fonts/SimpleFontData.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include <gtest/gtest.h>

namespace blink {

namespace {

// Minimal TestSimpleFontData implementation.
// Font has no glyphs, but that's okay.
class TestSimpleFontData : public SimpleFontData {
public:
    static PassRefPtr<TestSimpleFontData> create()
    {
        return adoptRef(new TestSimpleFontData);
    }

private:
    TestSimpleFontData() : SimpleFontData(nullptr, 10, false, false) { }

    bool fillGlyphPage(GlyphPage* pageToFill, unsigned offset, unsigned length, UChar* buffer, unsigned bufferLength) const override
    {
        return false;
    }
};

} // anonymous namespace

TEST(GlyphBufferTest, StartsEmpty)
{
    GlyphBuffer glyphBuffer;
    EXPECT_TRUE(glyphBuffer.isEmpty());
    EXPECT_EQ(0u, glyphBuffer.size());
}

TEST(GlyphBufferTest, StoresGlyphs)
{
    RefPtr<SimpleFontData> font1 = TestSimpleFontData::create();
    RefPtr<SimpleFontData> font2 = TestSimpleFontData::create();

    GlyphBuffer glyphBuffer;
    glyphBuffer.add(42, font1.get(), 10);
    glyphBuffer.add(43, font1.get(), 15);
    glyphBuffer.add(44, font2.get(), 22);

    EXPECT_FALSE(glyphBuffer.isEmpty());
    EXPECT_FALSE(glyphBuffer.hasVerticalOffsets());
    EXPECT_EQ(3u, glyphBuffer.size());

    EXPECT_EQ(42, glyphBuffer.glyphAt(0));
    EXPECT_EQ(43, glyphBuffer.glyphAt(1));
    EXPECT_EQ(44, glyphBuffer.glyphAt(2));

    const Glyph* glyphs = glyphBuffer.glyphs(0);
    EXPECT_EQ(42, glyphs[0]);
    EXPECT_EQ(43, glyphs[1]);
    EXPECT_EQ(44, glyphs[2]);
}

TEST(GlyphBufferTest, StoresVerticalOffsets)
{
    RefPtr<SimpleFontData> font1 = TestSimpleFontData::create();
    RefPtr<SimpleFontData> font2 = TestSimpleFontData::create();

    GlyphBuffer glyphBuffer;
    EXPECT_FALSE(glyphBuffer.hasVerticalOffsets());

    glyphBuffer.add(42, font1.get(), FloatPoint(10, 0));
    glyphBuffer.add(43, font1.get(), FloatPoint(15, 0));
    glyphBuffer.add(44, font2.get(), FloatPoint(12, 2));

    EXPECT_FALSE(glyphBuffer.isEmpty());
    EXPECT_TRUE(glyphBuffer.hasVerticalOffsets());
    EXPECT_EQ(3u, glyphBuffer.size());

    const float* offsets = glyphBuffer.offsets(0);
    EXPECT_EQ(10, glyphBuffer.xOffsetAt(0));
    EXPECT_EQ(0, glyphBuffer.yOffsetAt(0));
    EXPECT_EQ(15, glyphBuffer.xOffsetAt(1));
    EXPECT_EQ(0, glyphBuffer.yOffsetAt(1));
    EXPECT_EQ(12, glyphBuffer.xOffsetAt(2));
    EXPECT_EQ(2, glyphBuffer.yOffsetAt(2));

    EXPECT_EQ(10, offsets[0]);
    EXPECT_EQ(0, offsets[1]);
    EXPECT_EQ(15, offsets[2]);
    EXPECT_EQ(0, offsets[3]);
    EXPECT_EQ(12, offsets[4]);
    EXPECT_EQ(2, offsets[5]);
}

TEST(GlyphBufferTest, StoresOffsets)
{
    RefPtr<SimpleFontData> font1 = TestSimpleFontData::create();
    RefPtr<SimpleFontData> font2 = TestSimpleFontData::create();

    GlyphBuffer glyphBuffer;
    glyphBuffer.add(42, font1.get(), 10);
    glyphBuffer.add(43, font1.get(), 15);
    glyphBuffer.add(44, font2.get(), 20);

    EXPECT_FALSE(glyphBuffer.isEmpty());
    EXPECT_FALSE(glyphBuffer.hasVerticalOffsets());
    EXPECT_EQ(3u, glyphBuffer.size());

    EXPECT_EQ(10, glyphBuffer.xOffsetAt(0));
    EXPECT_EQ(15, glyphBuffer.xOffsetAt(1));
    EXPECT_EQ(20, glyphBuffer.xOffsetAt(2));

    const float* offsets = glyphBuffer.offsets(0);
    EXPECT_EQ(10, offsets[0]);
    EXPECT_EQ(15, offsets[1]);
    EXPECT_EQ(20, offsets[2]);
}

TEST(GlyphBufferTest, StoresSimpleFontData)
{
    RefPtr<SimpleFontData> font1 = TestSimpleFontData::create();
    RefPtr<SimpleFontData> font2 = TestSimpleFontData::create();

    GlyphBuffer glyphBuffer;
    glyphBuffer.add(42, font1.get(), 10);
    glyphBuffer.add(43, font1.get(), 15);
    glyphBuffer.add(44, font2.get(), 12);

    EXPECT_FALSE(glyphBuffer.isEmpty());
    EXPECT_EQ(3u, glyphBuffer.size());

    EXPECT_EQ(font1.get(), glyphBuffer.fontDataAt(0));
    EXPECT_EQ(font1.get(), glyphBuffer.fontDataAt(1));
    EXPECT_EQ(font2.get(), glyphBuffer.fontDataAt(2));
}

TEST(GlyphBufferTest, GlyphArrayWithOffset)
{
    RefPtr<SimpleFontData> font1 = TestSimpleFontData::create();
    RefPtr<SimpleFontData> font2 = TestSimpleFontData::create();

    GlyphBuffer glyphBuffer;
    glyphBuffer.add(42, font1.get(), 10);
    glyphBuffer.add(43, font1.get(), 15);
    glyphBuffer.add(44, font2.get(), 12);

    EXPECT_FALSE(glyphBuffer.isEmpty());
    EXPECT_EQ(3u, glyphBuffer.size());

    const Glyph* glyphs = glyphBuffer.glyphs(1);
    EXPECT_EQ(43, glyphs[0]);
    EXPECT_EQ(44, glyphs[1]);
}

TEST(GlyphBufferTest, OffsetArrayWithNonZeroIndex)
{
    RefPtr<SimpleFontData> font1 = TestSimpleFontData::create();
    RefPtr<SimpleFontData> font2 = TestSimpleFontData::create();

    {
        GlyphBuffer glyphBuffer;
        glyphBuffer.add(42, font1.get(), 10);
        glyphBuffer.add(43, font1.get(), 15);
        glyphBuffer.add(43, font2.get(), 12);

        EXPECT_FALSE(glyphBuffer.isEmpty());
        EXPECT_FALSE(glyphBuffer.hasVerticalOffsets());
        EXPECT_EQ(3u, glyphBuffer.size());

        const float* offsets = glyphBuffer.offsets(1);
        EXPECT_EQ(15, offsets[0]);
        EXPECT_EQ(12, offsets[1]);
    }

    {
        GlyphBuffer glyphBuffer;
        glyphBuffer.add(42, font1.get(), FloatPoint(10, 0));
        glyphBuffer.add(43, font1.get(), FloatPoint(15, 0));
        glyphBuffer.add(43, font2.get(), FloatPoint(12, 2));

        EXPECT_FALSE(glyphBuffer.isEmpty());
        EXPECT_TRUE(glyphBuffer.hasVerticalOffsets());
        EXPECT_EQ(3u, glyphBuffer.size());

        const float* offsets = glyphBuffer.offsets(1);
        EXPECT_EQ(15, offsets[0]);
        EXPECT_EQ(0, offsets[1]);
        EXPECT_EQ(12, offsets[2]);
        EXPECT_EQ(2, offsets[3]);
    }
}

TEST(GlyphBufferTest, ReverseForSimpleRTL)
{
    RefPtr<SimpleFontData> font1 = TestSimpleFontData::create();
    RefPtr<SimpleFontData> font2 = TestSimpleFontData::create();

    GlyphBuffer glyphBuffer;
    glyphBuffer.add(42, font1.get(), 10);
    glyphBuffer.add(43, font1.get(), 15);
    glyphBuffer.add(44, font2.get(), 25);

    EXPECT_FALSE(glyphBuffer.isEmpty());
    EXPECT_EQ(3u, glyphBuffer.size());

    glyphBuffer.reverseForSimpleRTL(30, 100);

    EXPECT_FALSE(glyphBuffer.isEmpty());
    EXPECT_EQ(3u, glyphBuffer.size());
    EXPECT_EQ(44, glyphBuffer.glyphAt(0));
    EXPECT_EQ(43, glyphBuffer.glyphAt(1));
    EXPECT_EQ(42, glyphBuffer.glyphAt(2));
    EXPECT_EQ(font2.get(), glyphBuffer.fontDataAt(0));
    EXPECT_EQ(font1.get(), glyphBuffer.fontDataAt(1));
    EXPECT_EQ(font1.get(), glyphBuffer.fontDataAt(2));
    EXPECT_EQ(70, glyphBuffer.xOffsetAt(0));
    EXPECT_EQ(75, glyphBuffer.xOffsetAt(1));
    EXPECT_EQ(85, glyphBuffer.xOffsetAt(2));
}

} // namespace blink

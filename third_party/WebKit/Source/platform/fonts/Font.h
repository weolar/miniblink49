/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2006, 2007, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Holger Hans Peter Freyther
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef Font_h
#define Font_h

#include "platform/LayoutUnit.h"
#include "platform/PlatformExport.h"
#include "platform/fonts/FontDescription.h"
#include "platform/fonts/FontFallbackList.h"
#include "platform/fonts/SimpleFontData.h"
#include "platform/fonts/TextBlob.h"
#include "platform/text/TabSize.h"
#include "platform/text/TextDirection.h"
#include "platform/text/TextPath.h"
#include "wtf/HashMap.h"
#include "wtf/HashSet.h"
#include "wtf/MathExtras.h"
#include "wtf/text/CharacterNames.h"

// "X11/X.h" defines Complex to 0 and conflicts
// with Complex value in CodePath enum.
#ifdef Complex
#undef Complex
#endif

class SkCanvas;
class SkPaint;
class SkTextBlob;
struct SkPoint;

namespace blink {

class FloatPoint;
class FloatRect;
class FontData;
class FontMetrics;
class FontSelector;
class GlyphBuffer;
class TextRun;
struct TextRunPaintInfo;

struct GlyphData;

class PLATFORM_EXPORT Font {
public:
    Font();
    Font(const FontDescription&);
    ~Font();

    Font(const Font&);
    Font& operator=(const Font&);

    bool operator==(const Font& other) const;
    bool operator!=(const Font& other) const { return !(*this == other); }

    const FontDescription& fontDescription() const { return m_fontDescription; }

    void update(PassRefPtrWillBeRawPtr<FontSelector>) const;

    enum CustomFontNotReadyAction { DoNotPaintIfFontNotReady, UseFallbackIfFontNotReady };
    void drawText(SkCanvas*, const TextRunPaintInfo&, const FloatPoint&, float deviceScaleFactor, const SkPaint&) const;
    void drawBidiText(SkCanvas*, const TextRunPaintInfo&, const FloatPoint&, CustomFontNotReadyAction, float deviceScaleFactor, const SkPaint&) const;
    void drawEmphasisMarks(SkCanvas*, const TextRunPaintInfo&, const AtomicString& mark, const FloatPoint&, float deviceScaleFactor, const SkPaint&) const;

    // Glyph bounds will be the minimum rect containing all glyph strokes, in coordinates using
    // (<text run x position>, <baseline position>) as the origin.
    float width(const TextRun&, HashSet<const SimpleFontData*>* fallbackFonts = nullptr, FloatRect* glyphBounds = nullptr) const;

    int offsetForPosition(const TextRun&, float position, bool includePartialGlyphs) const;
    FloatRect selectionRectForText(const TextRun&, const FloatPoint&, int h, int from = 0, int to = -1, bool accountForGlyphBounds = false) const;

    // Metrics that we query the FontFallbackList for.
    const FontMetrics& fontMetrics() const { return primaryFont()->fontMetrics(); }
    float spaceWidth() const { return primaryFont()->spaceWidth() + fontDescription().letterSpacing(); }
    float tabWidth(const SimpleFontData&, const TabSize&, float position) const;
    float tabWidth(const TabSize& tabSize, float position) const { return tabWidth(*primaryFont(), tabSize, position); }

    int emphasisMarkAscent(const AtomicString&) const;
    int emphasisMarkDescent(const AtomicString&) const;
    int emphasisMarkHeight(const AtomicString&) const;

    const SimpleFontData* primaryFont() const;
    const FontData* fontDataAt(unsigned) const;

    GlyphData glyphDataForCharacter(UChar32&, bool mirror, bool normalizeSpace = false, FontDataVariant = AutoVariant) const;
    CodePath codePath(const TextRunPaintInfo&) const;

    // Whether the font supports shaping word by word instead of shaping the
    // full run in one go. Allows better caching for fonts where space cannot
    // participate in kerning and/or ligatures.
    bool canShapeWordByWord() const;

    void setCanShapeWordByWordForTesting(bool b)
    {
        m_canShapeWordByWord = b;
        m_shapeWordByWordComputed = true;
    }

private:
    enum ForTextEmphasisOrNot { NotForTextEmphasis, ForTextEmphasis };

    // Returns the total advance.
    float buildGlyphBuffer(const TextRunPaintInfo&, GlyphBuffer&, const GlyphData* emphasisData = nullptr) const;
    PassTextBlobPtr buildTextBlob(const GlyphBuffer&) const;
    void paintGlyphs(SkCanvas*, const SkPaint&, const SimpleFontData*, const Glyph glyphs[], unsigned numGlyphs,
        const SkPoint pos[], const FloatRect& textRect, float deviceScaleFactor) const;
    void paintGlyphsHorizontal(SkCanvas*, const SkPaint&, const SimpleFontData*, const Glyph glyphs[], unsigned numGlyphs,
        const SkScalar xpos[], SkScalar constY, const FloatRect& textRect, float deviceScaleFactor) const;
    void drawGlyphs(SkCanvas*, const SkPaint&, const SimpleFontData*, const GlyphBuffer&, unsigned from, unsigned numGlyphs,
        const FloatPoint&, const FloatRect& textRect, float deviceScaleFactor) const;
    void drawTextBlob(SkCanvas*, const SkPaint&, const SkTextBlob*, const SkPoint& origin) const;
    void drawGlyphBuffer(SkCanvas*, const SkPaint&, const TextRunPaintInfo&, const GlyphBuffer&, const FloatPoint&, float deviceScaleFactor) const;
    float floatWidthForSimpleText(const TextRun&, HashSet<const SimpleFontData*>* fallbackFonts = 0, FloatRect* glyphBounds = 0) const;
    int offsetForPositionForSimpleText(const TextRun&, float position, bool includePartialGlyphs) const;
    FloatRect selectionRectForSimpleText(const TextRun&, const FloatPoint&, int h, int from, int to, bool accountForGlyphBounds) const;

    bool getEmphasisMarkGlyphData(const AtomicString&, GlyphData&) const;

    float floatWidthForComplexText(const TextRun&, HashSet<const SimpleFontData*>* fallbackFonts, FloatRect* glyphBounds) const;
    int offsetForPositionForComplexText(const TextRun&, float position, bool includePartialGlyphs) const;
    FloatRect selectionRectForComplexText(const TextRun&, const FloatPoint&, int h, int from, int to) const;

    bool computeCanShapeWordByWord() const;

    friend struct SimpleShaper;

public:
    FontSelector* fontSelector() const;

    void willUseFontData(UChar32) const;

    bool loadingCustomFonts() const;
    bool isFallbackValid() const;

private:
    bool shouldSkipDrawing() const
    {
        return m_fontFallbackList && m_fontFallbackList->shouldSkipDrawing();
    }

    FontDescription m_fontDescription;
    mutable RefPtr<FontFallbackList> m_fontFallbackList;
    mutable unsigned m_canShapeWordByWord : 1;
    mutable unsigned m_shapeWordByWordComputed : 1;
};

inline const SimpleFontData* Font::primaryFont() const
{
    ASSERT(m_fontFallbackList);
    return m_fontFallbackList->primarySimpleFontData(m_fontDescription);
}

inline const FontData* Font::fontDataAt(unsigned index) const
{
    ASSERT(m_fontFallbackList);
    return m_fontFallbackList->fontDataAt(m_fontDescription, index);
}

inline FontSelector* Font::fontSelector() const
{
    return m_fontFallbackList ? m_fontFallbackList->fontSelector() : 0;
}

inline float Font::tabWidth(const SimpleFontData& fontData, const TabSize& tabSize, float position) const
{
    float baseTabWidth = tabSize.getPixelSize(fontData.spaceWidth());
    if (!baseTabWidth)
        return fontDescription().letterSpacing();
    float distanceToTabStop = baseTabWidth - fmodf(position, baseTabWidth);

    // The smallest allowable tab space is letterSpacing() (but must be at least one layout unit).
    // if the distance to the next tab stop is less than that, advance an additional tab stop.
    if (distanceToTabStop < std::max(fontDescription().letterSpacing(), LayoutUnit::epsilon()))
        distanceToTabStop += baseTabWidth;

    return distanceToTabStop;
}

} // namespace blink

#endif

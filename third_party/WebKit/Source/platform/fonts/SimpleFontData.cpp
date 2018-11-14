/*
 * Copyright (C) 2005, 2008, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/fonts/SimpleFontData.h"

#include "SkPaint.h"
#include "SkPath.h"
#include "SkTypeface.h"
#include "SkTypes.h"
#include "SkUtils.h"
#include "platform/fonts/FontDescription.h"
#include "platform/fonts/GlyphPage.h"
#include "platform/fonts/VDMXParser.h"
#include "platform/geometry/FloatRect.h"
#include "wtf/MathExtras.h"
#include "wtf/text/CharacterNames.h"
#include "wtf/text/Unicode.h"
//#include <unicode/normlzr.h>

namespace blink {

const float smallCapsFontSizeMultiplier = 0.7f;
const float emphasisMarkFontSizeMultiplier = 0.5f;

#if OS(LINUX) || OS(ANDROID)
// This is the largest VDMX table which we'll try to load and parse.
static const size_t maxVDMXTableSize = 1024 * 1024; // 1 MB
#endif

SimpleFontData::SimpleFontData(const FontPlatformData& platformData, PassRefPtr<CustomFontData> customData, bool isTextOrientationFallback)
    : m_maxCharWidth(-1)
    , m_avgCharWidth(-1)
    , m_platformData(platformData)
    , m_isTextOrientationFallback(isTextOrientationFallback)
    , m_verticalData(nullptr)
    , m_hasVerticalGlyphs(false)
    , m_customFontData(customData)
{
    platformInit();
    platformGlyphInit();
#if 1 // def MINIBLINK_NOT_IMPLEMENTED
    if (platformData.isVerticalAnyUpright() && !isTextOrientationFallback) {
        m_verticalData = platformData.verticalData();
        m_hasVerticalGlyphs = m_verticalData.get() && m_verticalData->hasVerticalMetrics();
    }
#endif
}

SimpleFontData::SimpleFontData(PassRefPtr<CustomFontData> customData, float fontSize, bool syntheticBold, bool syntheticItalic)
    : m_platformData(FontPlatformData(fontSize, syntheticBold, syntheticItalic))
    , m_isTextOrientationFallback(false)
    , m_verticalData(nullptr)
    , m_hasVerticalGlyphs(false)
    , m_customFontData(customData)
{
    if (m_customFontData)
        m_customFontData->initializeFontData(this, fontSize);
}

void SimpleFontData::platformInit()
{
    if (!m_platformData.size()) {
        m_fontMetrics.reset();
        m_avgCharWidth = 0;
        m_maxCharWidth = 0;
        return;
    }

    SkPaint paint;
    SkPaint::FontMetrics metrics;

    m_platformData.setupPaint(&paint);
    paint.getFontMetrics(&metrics);
    SkTypeface* face = paint.getTypeface();
    ASSERT(face);

    int vdmxAscent = 0, vdmxDescent = 0;
    bool isVDMXValid = false;

#if OS(LINUX) || OS(ANDROID)
    // Manually digging up VDMX metrics is only applicable when bytecode hinting using FreeType.
    // With GDI, the metrics will already have taken this into account (as needed).
    // With DirectWrite or CoreText, no bytecode hinting is ever done.
    // This code should be pushed into FreeType (hinted font metrics).
    static const uint32_t vdmxTag = SkSetFourByteTag('V', 'D', 'M', 'X');
    int pixelSize = m_platformData.size() + 0.5;
    if (!paint.isAutohinted()
        &&    (paint.getHinting() == SkPaint::kFull_Hinting
            || paint.getHinting() == SkPaint::kNormal_Hinting))
    {
        size_t vdmxSize = face->getTableSize(vdmxTag);
        if (vdmxSize && vdmxSize < maxVDMXTableSize) {
            uint8_t* vdmxTable = (uint8_t*) fastMalloc(vdmxSize);
            if (vdmxTable
                && face->getTableData(vdmxTag, 0, vdmxSize, vdmxTable) == vdmxSize
                && parseVDMX(&vdmxAscent, &vdmxDescent, vdmxTable, vdmxSize, pixelSize))
                isVDMXValid = true;
            fastFree(vdmxTable);
        }
    }
#endif

    float ascent;
    float descent;

    // Beware those who step here: This code is designed to match Win32 font
    // metrics *exactly* (except the adjustment of ascent/descent on Linux/Android).
    if (isVDMXValid) {
        ascent = vdmxAscent;
        descent = -vdmxDescent;
    } else {
        ascent = SkScalarRoundToInt(-metrics.fAscent);
        descent = SkScalarRoundToInt(metrics.fDescent);
#if OS(LINUX) || OS(ANDROID)
        // When subpixel positioning is enabled, if the descent is rounded down, the descent part
        // of the glyph may be truncated when displayed in a 'overflow: hidden' container.
        // To avoid that, borrow 1 unit from the ascent when possible.
        // FIXME: This can be removed if sub-pixel ascent/descent is supported.
        if (platformData().fontRenderStyle().useSubpixelPositioning && descent < SkScalarToFloat(metrics.fDescent) && ascent >= 1) {
            ++descent;
            --ascent;
        }
#endif
    }

#if OS(MACOSX)
    // We are preserving this ascent hack to match Safari's ascent adjustment
    // in their SimpleFontDataMac.mm, for details see crbug.com/445830.
    // We need to adjust Times, Helvetica, and Courier to closely match the
    // vertical metrics of their Microsoft counterparts that are the de facto
    // web standard. The AppKit adjustment of 20% is too big and is
    // incorrectly added to line spacing, so we use a 15% adjustment instead
    // and add it to the ascent.
    DEFINE_STATIC_LOCAL(AtomicString, timesName, ("Times", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(AtomicString, helveticaName, ("Helvetica", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(AtomicString, courierName, ("Courier", AtomicString::ConstructFromLiteral));
    String familyName = m_platformData.fontFamilyName();
    if (familyName == timesName || familyName == helveticaName || familyName == courierName)
        ascent += floorf(((ascent + descent) * 0.15f) + 0.5f);
#endif

    m_fontMetrics.setAscent(ascent);
    m_fontMetrics.setDescent(descent);

    float xHeight;
    if (metrics.fXHeight) {
        xHeight = metrics.fXHeight;
#if OS(MACOSX)
        // Mac OS CTFontGetXHeight reports the bounding box height of x,
        // including parts extending below the baseline and apparently no x-height
        // value from the OS/2 table. However, the CSS ex unit
        // expects only parts above the baseline, hence measuring the glyph:
        // http://www.w3.org/TR/css3-values/#ex-unit
        GlyphPage* glyphPageZero = GlyphPageTreeNode::getRootChild(this, 0)->page();
        if (glyphPageZero) {
            static const UChar32 xChar = 'x';
            const Glyph xGlyph = glyphPageZero->glyphForCharacter(xChar);
            if (xGlyph) {
                FloatRect glyphBounds(boundsForGlyph(xGlyph));
                // SkGlyph bounds, y down, based on rendering at (0,0).
                xHeight = - glyphBounds.y();
            }
        }
#endif
        m_fontMetrics.setXHeight(xHeight);
    } else {
        xHeight = ascent * 0.56; // Best guess from Windows font metrics.
        m_fontMetrics.setXHeight(xHeight);
        m_fontMetrics.setHasXHeight(false);
    }

    float lineGap = SkScalarToFloat(metrics.fLeading);
    m_fontMetrics.setLineGap(lineGap);
    m_fontMetrics.setLineSpacing(lroundf(ascent) + lroundf(descent) + lroundf(lineGap));

    if (platformData().isVerticalAnyUpright() && !isTextOrientationFallback()) {
        static const uint32_t vheaTag = SkSetFourByteTag('v', 'h', 'e', 'a');
        static const uint32_t vorgTag = SkSetFourByteTag('V', 'O', 'R', 'G');
        size_t vheaSize = face->getTableSize(vheaTag);
        size_t vorgSize = face->getTableSize(vorgTag);
        if ((vheaSize > 0) || (vorgSize > 0))
            m_hasVerticalGlyphs = true;
    }

    // In WebKit/WebCore/platform/graphics/SimpleFontData.cpp, m_spaceWidth is
    // calculated for us, but we need to calculate m_maxCharWidth and
    // m_avgCharWidth in order for text entry widgets to be sized correctly.
#if OS(WIN)
    m_maxCharWidth = SkScalarRoundToInt(metrics.fMaxCharWidth);

    // Older version of the DirectWrite API doesn't implement support for max
    // char width. Fall back on a multiple of the ascent. This is entirely
    // arbitrary but comes pretty close to the expected value in most cases.
    if (m_maxCharWidth < 1)
        m_maxCharWidth = ascent * 2;
#elif OS(MACOSX)
    // FIXME: The current avg/max character width calculation is not ideal,
    // it should check either the OS2 table or, better yet, query FontMetrics.
    // Sadly FontMetrics provides incorrect data on Mac at the moment.
    // https://crbug.com/420901
    m_maxCharWidth = std::max(m_avgCharWidth, m_fontMetrics.floatAscent());
#else
    // Better would be to rely on either fMaxCharWidth or fAveCharWidth.
    // skbug.com/3087
    m_maxCharWidth = SkScalarRoundToInt(metrics.fXMax - metrics.fXMin);

#endif

#if !OS(MACOSX)
    if (metrics.fAvgCharWidth) {
        m_avgCharWidth = SkScalarRoundToInt(metrics.fAvgCharWidth);
    } else {
#endif
        m_avgCharWidth = xHeight;

        GlyphPage* glyphPageZero = GlyphPageTreeNode::getRootChild(this, 0)->page();

        if (glyphPageZero) {
            static const UChar32 xChar = 'x';
            const Glyph xGlyph = glyphPageZero->glyphForCharacter(xChar);

            if (xGlyph) {
                // In widthForGlyph(), xGlyph will be compared with
                // m_zeroWidthSpaceGlyph, which isn't initialized yet here.
                // Initialize it with zero to make sure widthForGlyph() returns
                // the right width.
                m_zeroWidthSpaceGlyph = 0;
                m_avgCharWidth = widthForGlyph(xGlyph);
            }
        }
#if !OS(MACOSX)
    }
#endif

    if (int unitsPerEm = face->getUnitsPerEm())
        m_fontMetrics.setUnitsPerEm(unitsPerEm);
}

void SimpleFontData::platformGlyphInit()
{
    GlyphPage* glyphPageZero = GlyphPageTreeNode::getRootChild(this, 0)->page();
    if (!glyphPageZero) {
        m_spaceGlyph = 0;
        m_spaceWidth = 0;
        m_zeroGlyph = 0;
        m_zeroWidthSpaceGlyph = 0;
        m_missingGlyphData.fontData = this;
        m_missingGlyphData.glyph = 0;
        return;
    }

    // Ask for the glyph for 0 to avoid paging in ZERO WIDTH SPACE. Control characters, including 0,
    // are mapped to the ZERO WIDTH SPACE glyph.
    m_zeroWidthSpaceGlyph = glyphPageZero->glyphForCharacter(0);

    // Nasty hack to determine if we should round or ceil space widths.
    // If the font is monospace or fake monospace we ceil to ensure that
    // every character and the space are the same width.  Otherwise we round.
    m_spaceGlyph = glyphPageZero->glyphForCharacter(spaceCharacter);
    float width = widthForGlyph(m_spaceGlyph);
    m_spaceWidth = width;
    m_zeroGlyph = glyphPageZero->glyphForCharacter('0');
    m_fontMetrics.setZeroWidth(widthForGlyph(m_zeroGlyph));

    // Force the glyph for ZERO WIDTH SPACE to have zero width, unless it is shared with SPACE.
    // Helvetica is an example of a non-zero width ZERO WIDTH SPACE glyph.
    // See <http://bugs.webkit.org/show_bug.cgi?id=13178>
    if (m_zeroWidthSpaceGlyph == m_spaceGlyph) {
        m_zeroWidthSpaceGlyph = 0;
        //WTF_LOG_ERROR("Font maps SPACE and ZERO WIDTH SPACE to the same glyph. Glyph width will not be overridden.");
    }

    m_missingGlyphData.fontData = this;
    m_missingGlyphData.glyph = 0;
}

SimpleFontData::~SimpleFontData()
{
    if (isCustomFont())
        GlyphPageTreeNode::pruneTreeCustomFontData(this);
    else
        GlyphPageTreeNode::pruneTreeFontData(this);
}

const SimpleFontData* SimpleFontData::fontDataForCharacter(UChar32) const
{
    return this;
}

Glyph SimpleFontData::glyphForCharacter(UChar32 character) const
{
    // As GlyphPage::size is power of 2 so shifting is valid
    GlyphPageTreeNode* node = GlyphPageTreeNode::getNormalRootChild(this, character >> GlyphPage::sizeBits);
    return node->page() ? node->page()->glyphAt(character & 0xFF) : 0;
}

bool SimpleFontData::isSegmented() const
{
    return false;
}

PassRefPtr<SimpleFontData> SimpleFontData::verticalRightOrientationFontData() const
{
    if (!m_derivedFontData)
        m_derivedFontData = DerivedFontData::create(isCustomFont());
    if (!m_derivedFontData->verticalRightOrientation) {
        FontPlatformData verticalRightPlatformData(m_platformData);
        verticalRightPlatformData.setOrientation(FontOrientation::Horizontal);
        m_derivedFontData->verticalRightOrientation = create(verticalRightPlatformData, isCustomFont() ? CustomFontData::create(): nullptr, true);
    }
    return m_derivedFontData->verticalRightOrientation;
}

PassRefPtr<SimpleFontData> SimpleFontData::uprightOrientationFontData() const
{
    if (!m_derivedFontData)
        m_derivedFontData = DerivedFontData::create(isCustomFont());
    if (!m_derivedFontData->uprightOrientation)
        m_derivedFontData->uprightOrientation = create(m_platformData, isCustomFont() ? CustomFontData::create(): nullptr, true);
    return m_derivedFontData->uprightOrientation;
}

PassRefPtr<SimpleFontData> SimpleFontData::smallCapsFontData(const FontDescription& fontDescription) const
{
    if (!m_derivedFontData)
        m_derivedFontData = DerivedFontData::create(isCustomFont());
    if (!m_derivedFontData->smallCaps)
        m_derivedFontData->smallCaps = createScaledFontData(fontDescription, smallCapsFontSizeMultiplier);

    return m_derivedFontData->smallCaps;
}

PassRefPtr<SimpleFontData> SimpleFontData::emphasisMarkFontData(const FontDescription& fontDescription) const
{
    if (!m_derivedFontData)
        m_derivedFontData = DerivedFontData::create(isCustomFont());
    if (!m_derivedFontData->emphasisMark)
        m_derivedFontData->emphasisMark = createScaledFontData(fontDescription, emphasisMarkFontSizeMultiplier);

    return m_derivedFontData->emphasisMark;
}

PassOwnPtr<SimpleFontData::DerivedFontData> SimpleFontData::DerivedFontData::create(bool forCustomFont)
{
    return adoptPtr(new DerivedFontData(forCustomFont));
}

SimpleFontData::DerivedFontData::~DerivedFontData()
{
    if (!forCustomFont)
        return;

    if (smallCaps)
        GlyphPageTreeNode::pruneTreeCustomFontData(smallCaps.get());
    if (emphasisMark)
        GlyphPageTreeNode::pruneTreeCustomFontData(emphasisMark.get());
    if (verticalRightOrientation)
        GlyphPageTreeNode::pruneTreeCustomFontData(verticalRightOrientation.get());
    if (uprightOrientation)
        GlyphPageTreeNode::pruneTreeCustomFontData(uprightOrientation.get());
}

PassRefPtr<SimpleFontData> SimpleFontData::createScaledFontData(const FontDescription& fontDescription, float scaleFactor) const
{
    return platformCreateScaledFontData(fontDescription, scaleFactor);
}

PassRefPtr<SimpleFontData> SimpleFontData::platformCreateScaledFontData(const FontDescription& fontDescription, float scaleFactor) const
{
    const float scaledSize = lroundf(fontDescription.computedSize() * scaleFactor);
    return SimpleFontData::create(FontPlatformData(m_platformData, scaledSize), isCustomFont() ? CustomFontData::create() : nullptr);
}

static inline void getSkiaBoundsForGlyph(SkPaint& paint, Glyph glyph, SkRect& bounds)
{
    paint.setTextEncoding(SkPaint::kGlyphID_TextEncoding);

    SkPath path;
    paint.getTextPath(&glyph, sizeof(glyph), 0, 0, &path);
    bounds = path.getBounds();

    if (!paint.isSubpixelText()) {
        SkIRect ir;
        bounds.round(&ir);
        bounds.set(ir);
    }
}

FloatRect SimpleFontData::platformBoundsForGlyph(Glyph glyph) const
{
    if (!m_platformData.size())
        return FloatRect();

    SkASSERT(sizeof(glyph) == 2); // compile-time assert

    SkPaint paint;
    m_platformData.setupPaint(&paint);

    SkRect bounds;
    getSkiaBoundsForGlyph(paint, glyph, bounds);
    return FloatRect(bounds);
}

float SimpleFontData::platformWidthForGlyph(Glyph glyph) const
{
    if (!m_platformData.size())
        return 0;

    SkASSERT(sizeof(glyph) == 2); // compile-time assert

    SkPaint paint;

    m_platformData.setupPaint(&paint);

    paint.setTextEncoding(SkPaint::kGlyphID_TextEncoding);
    SkScalar width = paint.measureText(&glyph, 2);
    if (!paint.isSubpixelText())
        width = SkScalarRoundToInt(width);
    return SkScalarToFloat(width);
}

bool SimpleFontData::canRenderCombiningCharacterSequence(const UChar* characters, size_t length) const
{
    if (!m_combiningCharacterSequenceSupport)
        m_combiningCharacterSequenceSupport = adoptPtr(new HashMap<String, bool>);

    WTF::HashMap<String, bool>::AddResult addResult = m_combiningCharacterSequenceSupport->add(String(characters, length), false);
    if (!addResult.isNewEntry)
        return addResult.storedValue->value;

#ifdef MINIBLINK_NOT_IMPLEMENTED
    UErrorCode error = U_ZERO_ERROR;
    Vector<UChar, 4> normalizedCharacters(length);
    size_t normalizedLength = unorm_normalize(characters, length, UNORM_NFC, UNORM_UNICODE_3_2, &normalizedCharacters[0], length, &error);
    // Can't render if we have an error or no composition occurred.
    if (U_FAILURE(error) || normalizedLength == length)
        return false;

    for (size_t offset = 0; offset < normalizedLength;) {
        UChar32 character;
        U16_NEXT(normalizedCharacters, offset, normalizedLength, character);
        if (!glyphForCharacter(character))
            return false;
    }
    addResult.storedValue->value = true;
    return true;
#else
    return false;
#endif // MINIBLINK_NOT_IMPLEMENTED
}

bool SimpleFontData::fillGlyphPage(GlyphPage* pageToFill, unsigned offset, unsigned length, UChar* buffer, unsigned bufferLength) const
{
    if (SkUTF16_IsHighSurrogate(buffer[bufferLength-1])) {
        SkDebugf("%s last char is high-surrogate", __FUNCTION__);
        return false;
    }

    SkTypeface* typeface = platformData().typeface();
    if (!typeface) {
        WTF_LOG_ERROR("fillGlyphPage called on an empty Skia typeface.");
        return false;
    }

    SkAutoSTMalloc<GlyphPage::size, uint16_t> glyphStorage(length);
    uint16_t* glyphs = glyphStorage.get();
    typeface->charsToGlyphs(buffer, SkTypeface::kUTF16_Encoding, glyphs, length);

    bool haveGlyphs = false;
    for (unsigned i = 0; i < length; i++) {
        if (glyphs[i]) {
            pageToFill->setGlyphDataForIndex(offset + i, glyphs[i], this);
            haveGlyphs = true;
        }
    }

    return haveGlyphs;
}

} // namespace blink

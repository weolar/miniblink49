/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2006, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (c) 2007, 2008, 2010 Google Inc. All rights reserved.
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

#include "config.h"
#include "platform/fonts/Font.h"

#include "SkPaint.h"
#include "SkTemplates.h"
#include "platform/LayoutUnit.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/fonts/Character.h"
#include "platform/fonts/FontCache.h"
#include "platform/fonts/FontFallbackList.h"
#include "platform/fonts/GlyphBuffer.h"
#include "platform/fonts/GlyphPageTreeNode.h"
#include "platform/fonts/SimpleFontData.h"
#ifndef MINIBLINK_NO_HARFBUZZ
#include "platform/fonts/shaping/HarfBuzzFace.h"
#include "platform/fonts/shaping/HarfBuzzShaper.h"
#endif
#include "platform/fonts/shaping/SimpleShaper.h"
#include "platform/geometry/FloatRect.h"
#include "platform/graphics/skia/SkiaUtils.h"
#include "platform/text/BidiResolver.h"
#include "platform/text/TextRun.h"
#include "platform/text/TextRunIterator.h"
#include "platform/transforms/AffineTransform.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "wtf/MainThread.h"
#include "wtf/StdLibExtras.h"
#include "wtf/text/CharacterNames.h"
#include "wtf/text/Unicode.h"
#include "wtf/RefCountedLeakCounter.h"

using namespace WTF;
using namespace Unicode;

namespace blink {

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, fontCounter, ("FontCounter"));
Font* gFontDebug = nullptr;
#endif

Font::Font()
{
#ifndef NDEBUG
    fontCounter.increment();
//     WTF::String outstr = String::format("Font::Font: %p\n", this);
//     OutputDebugStringW(outstr.charactersWithNullTermination().data());
//     if (!gFontDebug)
//         gFontDebug = this;
#endif
}

Font::Font(const FontDescription& fd)
    : m_fontDescription(fd)
    , m_canShapeWordByWord(0)
    , m_shapeWordByWordComputed(0)
{
#ifndef NDEBUG
    fontCounter.increment();
//     WTF::String outstr = String::format("Font::Font 2: %p\n", this);
//     OutputDebugStringW(outstr.charactersWithNullTermination().data());
#endif
}

Font::Font(const Font& other)
    : m_fontDescription(other.m_fontDescription)
    , m_fontFallbackList(other.m_fontFallbackList)
    , m_canShapeWordByWord(0)
    , m_shapeWordByWordComputed(0)
{
#ifndef NDEBUG
    fontCounter.increment();
//     WTF::String outstr = String::format("Font::Font 3: %p\n", this);
//     OutputDebugStringW(outstr.charactersWithNullTermination().data());
#endif
}

Font::~Font()
{
#ifndef NDEBUG
//     WTF::String outstr;
//     if (m_fontFallbackList.get()) {
//         outstr = String::format("Font::~Font: %p %p %d\n", this, m_fontFallbackList.get(), m_fontFallbackList->refCount());
//         OutputDebugStringW(outstr.charactersWithNullTermination().data());
//     } else {
//         outstr = String::format("Font::~Font: %p\n", this);
//         OutputDebugStringW(outstr.charactersWithNullTermination().data());
//     }
    fontCounter.decrement();
#endif
}

Font& Font::operator=(const Font& other)
{
    m_fontDescription = other.m_fontDescription;
    m_fontFallbackList = other.m_fontFallbackList;
    m_canShapeWordByWord = other.m_canShapeWordByWord;
    m_shapeWordByWordComputed = other.m_shapeWordByWordComputed;
    return *this;
}

bool Font::operator==(const Font& other) const
{
    FontSelector* first = m_fontFallbackList ? m_fontFallbackList->fontSelector() : 0;
    FontSelector* second = other.m_fontFallbackList ? other.m_fontFallbackList->fontSelector() : 0;

    return first == second
        && m_fontDescription == other.m_fontDescription
        && (m_fontFallbackList ? m_fontFallbackList->fontSelectorVersion() : 0) == (other.m_fontFallbackList ? other.m_fontFallbackList->fontSelectorVersion() : 0)
        && (m_fontFallbackList ? m_fontFallbackList->generation() : 0) == (other.m_fontFallbackList ? other.m_fontFallbackList->generation() : 0);
}

void Font::update(PassRefPtrWillBeRawPtr<FontSelector> fontSelector) const
{
    // FIXME: It is pretty crazy that we are willing to just poke into a RefPtr, but it ends up
    // being reasonably safe (because inherited fonts in the render tree pick up the new
    // style anyway. Other copies are transient, e.g., the state in the GraphicsContext, and
    // won't stick around long enough to get you in trouble). Still, this is pretty disgusting,
    // and could eventually be rectified by using RefPtrs for Fonts themselves.
    if (!m_fontFallbackList) {
        m_fontFallbackList = FontFallbackList::create();
//         WTF::String outstr = String::format("Font::update: %p %p\n", this, m_fontFallbackList.get());
//         OutputDebugStringW(outstr.charactersWithNullTermination().data());
//         if (gFontDebug == this)
//             OutputDebugStringA("");
    }
    m_fontFallbackList->invalidate(fontSelector);
}

float Font::buildGlyphBuffer(const TextRunPaintInfo& runInfo, GlyphBuffer& glyphBuffer,
    const GlyphData* emphasisData) const
{
    if (codePath(runInfo) == ComplexPath) {
        float width;
#ifndef MINIBLINK_NO_HARFBUZZ
        CachingWordShaper& shaper = m_fontFallbackList->cachingWordShaper();
        if (emphasisData) {
            width = shaper.fillGlyphBufferForTextEmphasis(this, runInfo.run,
                emphasisData, &glyphBuffer, runInfo.from, runInfo.to);
        } else {
            width = shaper.fillGlyphBuffer(this, runInfo.run, nullptr,
                &glyphBuffer, runInfo.from, runInfo.to);
        }
#else
        if (runInfo.run.is8Bit())
            return 0;

        int dummyLength = std::min(runInfo.to - runInfo.from, runInfo.run.length());
        Vector<UChar> dummyString;
        dummyString.resize(dummyLength);
        const UChar* runCharacters16 = runInfo.run.characters16();
        int endPos = std::min(runInfo.run.length(), runInfo.to);
        for (int i = runInfo.from; i < endPos; ++i) {
            UChar temp[3] = { runCharacters16[i], 0, 0 };
            if (i < endPos - 1) 
                temp[1] = runCharacters16[i + 1];
            
            if (ComplexPath == Character::characterRangeCodePath(temp, 2))
                dummyString[i] = L'?';
            else
                dummyString[i] = runCharacters16[i];
        }

        TextRun dummyTextRun(dummyString.data(), dummyString.size());
        TextRunPaintInfo dummyRunInfo(dummyTextRun);
        width = buildGlyphBuffer(dummyRunInfo, glyphBuffer, emphasisData);
#endif
        return width;
    }

    SimpleShaper shaper(this, runInfo.run, emphasisData, nullptr /* fallbackFonts */, nullptr);
    shaper.advance(runInfo.from);
    shaper.advance(runInfo.to, &glyphBuffer);
    float width = shaper.runWidthSoFar();

    if (runInfo.run.rtl()) {
        // Glyphs are shaped & stored in RTL advance order - reverse them for LTR drawing.
        shaper.advance(runInfo.run.length());
        glyphBuffer.reverseForSimpleRTL(width, shaper.runWidthSoFar());
    }

    return width;
}

void Font::drawText(SkCanvas* canvas, const TextRunPaintInfo& runInfo,
    const FloatPoint& point, float deviceScaleFactor, const SkPaint& paint) const
{
    // Don't draw anything while we are using custom fonts that are in the process of loading.
    if (shouldSkipDrawing())
        return;

    if (runInfo.cachedTextBlob && runInfo.cachedTextBlob->get()) {
        ASSERT(RuntimeEnabledFeatures::textBlobEnabled());
        // we have a pre-cached blob -- happy joy!
        drawTextBlob(canvas, paint, runInfo.cachedTextBlob->get(), point.data());
        return;
    }

    GlyphBuffer glyphBuffer;
    buildGlyphBuffer(runInfo, glyphBuffer);

    drawGlyphBuffer(canvas, paint, runInfo, glyphBuffer, point, deviceScaleFactor);
}

void Font::drawBidiText(SkCanvas* canvas, const TextRunPaintInfo& runInfo, const FloatPoint& point, CustomFontNotReadyAction customFontNotReadyAction, float deviceScaleFactor, const SkPaint& paint) const
{
    // Don't draw anything while we are using custom fonts that are in the process of loading,
    // except if the 'force' argument is set to true (in which case it will use a fallback
    // font).
    if (shouldSkipDrawing() && customFontNotReadyAction == DoNotPaintIfFontNotReady)
        return;

    // sub-run painting is not supported for Bidi text.
    const TextRun& run = runInfo.run;
    ASSERT((runInfo.from == 0) && (runInfo.to == run.length()));
    BidiResolver<TextRunIterator, BidiCharacterRun> bidiResolver;
    bidiResolver.setStatus(BidiStatus(run.direction(), run.directionalOverride()));
    bidiResolver.setPositionIgnoringNestedIsolates(TextRunIterator(&run, 0));

    // FIXME: This ownership should be reversed. We should pass BidiRunList
    // to BidiResolver in createBidiRunsForLine.
    BidiRunList<BidiCharacterRun>& bidiRuns = bidiResolver.runs();
    bidiResolver.createBidiRunsForLine(TextRunIterator(&run, run.length()));
    if (!bidiRuns.runCount())
        return;

    FloatPoint currPoint = point;
    BidiCharacterRun* bidiRun = bidiRuns.firstRun();
    while (bidiRun) {
        TextRun subrun = run.subRun(bidiRun->start(), bidiRun->stop() - bidiRun->start());
        bool isRTL = bidiRun->level() % 2;
        subrun.setDirection(isRTL ? RTL : LTR);
        subrun.setDirectionalOverride(bidiRun->dirOverride(false));

        TextRunPaintInfo subrunInfo(subrun);
        subrunInfo.bounds = runInfo.bounds;

        // TODO: investigate blob consolidation/caching (technically,
        //       all subruns could be part of the same blob).
        GlyphBuffer glyphBuffer;
        float runWidth = buildGlyphBuffer(subrunInfo, glyphBuffer);
        drawGlyphBuffer(canvas, paint, subrunInfo, glyphBuffer, currPoint, deviceScaleFactor);

        bidiRun = bidiRun->next();
        currPoint.move(runWidth, 0);
    }

    bidiRuns.deleteRuns();
}

void Font::drawEmphasisMarks(SkCanvas* canvas, const TextRunPaintInfo& runInfo, const AtomicString& mark, const FloatPoint& point, float deviceScaleFactor, const SkPaint& paint) const
{
    if (shouldSkipDrawing())
        return;

    FontCachePurgePreventer purgePreventer;

    GlyphData emphasisGlyphData;
    if (!getEmphasisMarkGlyphData(mark, emphasisGlyphData))
        return;

    ASSERT(emphasisGlyphData.fontData);
    if (!emphasisGlyphData.fontData)
        return;

    GlyphBuffer glyphBuffer;
    buildGlyphBuffer(runInfo, glyphBuffer, &emphasisGlyphData);

    if (glyphBuffer.isEmpty())
        return;

    drawGlyphBuffer(canvas, paint, runInfo, glyphBuffer, point, deviceScaleFactor);
}

float Font::width(const TextRun& run, HashSet<const SimpleFontData*>* fallbackFonts, FloatRect* glyphBounds) const
{
    FontCachePurgePreventer purgePreventer;

    if (codePath(TextRunPaintInfo(run)) == ComplexPath) {
#ifndef MINIBLINK_NO_HARFBUZZ
        return floatWidthForComplexText(run, fallbackFonts, glyphBounds);
#else
        Vector<UChar> dummy;
        dummy.fill(L'?', run.length());
        TextRun runDummy(dummy.data(), run.length(), run.xPos(), run.expansion(), TextRun::AllowTrailingExpansion | TextRun::ForbidLeadingExpansion, run.direction(), run.directionalOverride());
        floatWidthForSimpleText(runDummy, fallbackFonts, glyphBounds);
#endif // MINIBLINK_NO_HARFBUZZ
    }
    return floatWidthForSimpleText(run, fallbackFonts, glyphBounds);
}

PassTextBlobPtr Font::buildTextBlob(const GlyphBuffer& glyphBuffer) const
{
    ASSERT(RuntimeEnabledFeatures::textBlobEnabled());

    SkTextBlobBuilder builder;
    bool hasVerticalOffsets = glyphBuffer.hasVerticalOffsets();

    unsigned i = 0;
    while (i < glyphBuffer.size()) {
        const SimpleFontData* fontData = glyphBuffer.fontDataAt(i);

        // FIXME: Handle vertical text.
        if (fontData->platformData().isVerticalAnyUpright())
            return nullptr;

        SkPaint paint;
        // FIXME: FontPlatformData makes some decisions on the device scale
        // factor, which is found via the GraphicsContext. This should be fixed
        // to avoid correctness problems here.
        float deviceScaleFactor = 1.0f;
        fontData->platformData().setupPaint(&paint, deviceScaleFactor, this);
        paint.setTextEncoding(SkPaint::kGlyphID_TextEncoding);

        unsigned start = i++;
        while (i < glyphBuffer.size() && glyphBuffer.fontDataAt(i) == fontData)
            i++;
        unsigned count = i - start;

        const SkTextBlobBuilder::RunBuffer& buffer = hasVerticalOffsets
            ? builder.allocRunPos(paint, count)
            : builder.allocRunPosH(paint, count, 0);

        const uint16_t* glyphs = glyphBuffer.glyphs(start);
        const float* offsets = glyphBuffer.offsets(start);
        std::copy(glyphs, glyphs + count, buffer.glyphs);
        std::copy(offsets, offsets + (hasVerticalOffsets ? 2 * count : count), buffer.pos);
    }

    return adoptRef(builder.build());
}

static inline FloatRect pixelSnappedSelectionRect(FloatRect rect)
{
    // Using roundf() rather than ceilf() for the right edge as a compromise to
    // ensure correct caret positioning.
    float roundedX = roundf(rect.x());
    return FloatRect(roundedX, rect.y(), roundf(rect.maxX() - roundedX), rect.height());
}

FloatRect Font::selectionRectForText(const TextRun& run, const FloatPoint& point, int h, int from, int to, bool accountForGlyphBounds) const
{
    to = (to == -1 ? run.length() : to);

    TextRunPaintInfo runInfo(run);
    runInfo.from = from;
    runInfo.to = to;

    FontCachePurgePreventer purgePreventer;

    if (codePath(runInfo) != ComplexPath)
        return pixelSnappedSelectionRect(selectionRectForSimpleText(run, point, h, from, to, accountForGlyphBounds));
    return pixelSnappedSelectionRect(selectionRectForComplexText(run, point, h, from, to));
}

int Font::offsetForPosition(const TextRun& run, float x, bool includePartialGlyphs) const
{
    FontCachePurgePreventer purgePreventer;

    if (codePath(TextRunPaintInfo(run)) != ComplexPath
#ifndef MINIBLINK_NO_HARFBUZZ
		&& !fontDescription().typesettingFeatures()
#endif
		)
        return offsetForPositionForSimpleText(run, x, includePartialGlyphs);

    return offsetForPositionForComplexText(run, x, includePartialGlyphs);
}

CodePath Font::codePath(const TextRunPaintInfo& runInfo) const
{
    if (RuntimeEnabledFeatures::alwaysUseComplexTextEnabled())
        return ComplexPath;

    const TextRun& run = runInfo.run;
#ifndef MINIBLINK_NO_HARFBUZZ
    if (fontDescription().typesettingFeatures() && (runInfo.from || runInfo.to != run.length()))
        return ComplexPath;

    if (m_fontDescription.featureSettings() && m_fontDescription.featureSettings()->size() > 0 && m_fontDescription.letterSpacing() == 0)
        return ComplexPath;

    if (m_fontDescription.isVerticalBaseline())
        return ComplexPath;

    if (m_fontDescription.widthVariant() != RegularWidth)
        return ComplexPath;

    if (run.length() > 1 && fontDescription().typesettingFeatures())
        return ComplexPath;

    // FIXME: This really shouldn't be needed but for some reason the
    // TextRendering setting doesn't propagate to typesettingFeatures in time
    // for the prefs width calculation.

    if (fontDescription().textRendering() == OptimizeLegibility || fontDescription().textRendering() == GeometricPrecision)
        return ComplexPath;
#endif

    if (run.codePath() == TextRun::ForceComplex)
        return ComplexPath;

    if (run.codePath() == TextRun::ForceSimple)
        return SimplePath;

    if (run.is8Bit())
        return SimplePath;

    // Start from 0 since drawing and highlighting also measure the characters before run->from.
    return Character::characterRangeCodePath(run.characters16(), run.length());
}

bool Font::canShapeWordByWord() const
{
    if (!m_shapeWordByWordComputed) {
        m_canShapeWordByWord = computeCanShapeWordByWord();
        m_shapeWordByWordComputed = true;
    }
    return m_canShapeWordByWord;
};

bool Font::computeCanShapeWordByWord() const
{
    if (!fontDescription().typesettingFeatures())
        return true;

    const FontPlatformData& platformData = primaryFont()->platformData();
    TypesettingFeatures features = fontDescription().typesettingFeatures();
    return !platformData.hasSpaceInLigaturesOrKerning(features);
};

void Font::willUseFontData(UChar32 character) const
{
    const FontFamily& family = fontDescription().family();
    if (m_fontFallbackList && m_fontFallbackList->fontSelector() && !family.familyIsEmpty())
        m_fontFallbackList->fontSelector()->willUseFontData(fontDescription(), family.family(), character);
}

static inline GlyphData glyphDataForNonCJKCharacterWithGlyphOrientation(UChar32 character, bool isUpright, GlyphData& data, unsigned pageNumber)
{
    if (isUpright) {
        RefPtr<SimpleFontData> uprightFontData = data.fontData->uprightOrientationFontData();
        GlyphPageTreeNode* uprightNode = GlyphPageTreeNode::getNormalRootChild(uprightFontData.get(), pageNumber);
        GlyphPage* uprightPage = uprightNode->page();
        if (uprightPage) {
            GlyphData uprightData = uprightPage->glyphDataForCharacter(character);
            // If the glyphs are the same, then we know we can just use the horizontal glyph rotated vertically to be upright.
            if (data.glyph == uprightData.glyph)
                return data;
            // The glyphs are distinct, meaning that the font has a vertical-right glyph baked into it. We can't use that
            // glyph, so we fall back to the upright data and use the horizontal glyph.
            if (uprightData.fontData)
                return uprightData;
        }
    } else {
        RefPtr<SimpleFontData> verticalRightFontData = data.fontData->verticalRightOrientationFontData();
        GlyphPageTreeNode* verticalRightNode = GlyphPageTreeNode::getNormalRootChild(verticalRightFontData.get(), pageNumber);
        GlyphPage* verticalRightPage = verticalRightNode->page();
        if (verticalRightPage) {
            GlyphData verticalRightData = verticalRightPage->glyphDataForCharacter(character);
            // If the glyphs are distinct, we will make the assumption that the font has a vertical-right glyph baked
            // into it.
            if (data.glyph != verticalRightData.glyph)
                return data;
            // The glyphs are identical, meaning that we should just use the horizontal glyph.
            if (verticalRightData.fontData)
                return verticalRightData;
        }
    }
    return data;
}

GlyphData Font::glyphDataForCharacter(UChar32& c, bool mirror, bool normalizeSpace, FontDataVariant variant) const
{
    ASSERT(isMainThread());

    if (variant == AutoVariant) {
        if (m_fontDescription.variant() == FontVariantSmallCaps) {
            bool includeDefault = false;
            UChar32 upperC = toUpper(c, m_fontDescription.locale(includeDefault));
            if (upperC != c) {
                c = upperC;
                variant = SmallCapsVariant;
            } else {
                variant = NormalVariant;
            }
        } else {
            variant = NormalVariant;
        }
    }

    if (normalizeSpace && Character::isNormalizedCanvasSpaceCharacter(c))
        c = spaceCharacter;

    if (mirror)
        c = mirroredChar(c);

    unsigned pageNumber = (c / GlyphPage::size);

    GlyphPageTreeNodeBase* node = m_fontFallbackList->getPageNode(pageNumber);
    if (!node) {
        node = GlyphPageTreeNode::getRootChild(fontDataAt(0), pageNumber);
        m_fontFallbackList->setPageNode(pageNumber, node);
    }

    GlyphPage* page = 0;
    if (variant == NormalVariant) {
        // Fastest loop, for the common case (normal variant).
        while (true) {
            page = node->page(m_fontDescription.script());
            if (page) {
                GlyphData data = page->glyphDataForCharacter(c);
                if (data.fontData) {
                    if (!data.fontData->platformData().isVerticalAnyUpright() || data.fontData->isTextOrientationFallback())
                        return data;

                    bool isUpright = m_fontDescription.isVerticalUpright(c);
                    if (!isUpright || !Character::isCJKIdeographOrSymbol(c))
                        return glyphDataForNonCJKCharacterWithGlyphOrientation(c, isUpright, data, pageNumber);

                    return data;
                }

                if (node->isSystemFallback())
                    break;
            }

            // Proceed with the fallback list.
            node = toGlyphPageTreeNode(node)->getChild(fontDataAt(node->level()), pageNumber);
            m_fontFallbackList->setPageNode(pageNumber, node);
        }
    }
    if (variant != NormalVariant) {
        while (true) {
            page = node->page(m_fontDescription.script());
            if (page) {
                GlyphData data = page->glyphDataForCharacter(c);
                if (data.fontData) {
                    // The variantFontData function should not normally return 0.
                    // But if it does, we will just render the capital letter big.
                    RefPtr<SimpleFontData> variantFontData = data.fontData->variantFontData(m_fontDescription, variant);
                    if (!variantFontData)
                        return data;

                    GlyphPageTreeNode* variantNode = GlyphPageTreeNode::getNormalRootChild(variantFontData.get(), pageNumber);
                    GlyphPage* variantPage = variantNode->page();
                    if (variantPage) {
                        GlyphData data = variantPage->glyphDataForCharacter(c);
                        if (data.fontData)
                            return data;
                    }

                    // Do not attempt system fallback off the variantFontData. This is the very unlikely case that
                    // a font has the lowercase character but the small caps font does not have its uppercase version.
                    return variantFontData->missingGlyphData();
                }

                if (node->isSystemFallback())
                    break;
            }

            // Proceed with the fallback list.
            node = toGlyphPageTreeNode(node)->getChild(fontDataAt(node->level()), pageNumber);
            m_fontFallbackList->setPageNode(pageNumber, node);
        }
    }

    ASSERT(page);
    ASSERT(node->isSystemFallback());

    // System fallback is character-dependent. When we get here, we
    // know that the character in question isn't in the system fallback
    // font's glyph page. Try to lazily create it here.

    // FIXME: Unclear if this should normalizeSpaces above 0xFFFF.
    // Doing so changes fast/text/international/plane2-diffs.html
    UChar32 characterToRender = c;
    if (characterToRender <=  0xFFFF)
        characterToRender = Character::normalizeSpaces(characterToRender);

    const FontData* fontData = fontDataAt(0);
    if (fontData) {
        const SimpleFontData* fontDataToSubstitute = fontData->fontDataForCharacter(characterToRender);
        RefPtr<SimpleFontData> characterFontData = FontCache::fontCache()->fallbackFontForCharacter(m_fontDescription, characterToRender, fontDataToSubstitute);
        if (characterFontData && variant != NormalVariant) {
            characterFontData = characterFontData->variantFontData(m_fontDescription, variant);
        }
        if (characterFontData) {
            // Got the fallback glyph and font.
            GlyphPage* fallbackPage = GlyphPageTreeNode::getRootChild(characterFontData.get(), pageNumber)->page();
            GlyphData data = fallbackPage && fallbackPage->glyphForCharacter(c) ? fallbackPage->glyphDataForCharacter(c) : characterFontData->missingGlyphData();
            // Cache it so we don't have to do system fallback again next time.
            if (variant == NormalVariant) {
                page->setGlyphDataForCharacter(c, data.glyph, data.fontData);
                data.fontData->setMaxGlyphPageTreeLevel(std::max(data.fontData->maxGlyphPageTreeLevel(), node->level()));
                if (data.fontData->platformData().isVerticalAnyUpright() && !data.fontData->isTextOrientationFallback() && !Character::isCJKIdeographOrSymbol(c))
                    return glyphDataForNonCJKCharacterWithGlyphOrientation(c, m_fontDescription.isVerticalUpright(c), data, pageNumber);
            }
            return data;
        }
    }

    // Even system fallback can fail; use the missing glyph in that case.
    // FIXME: It would be nicer to use the missing glyph from the last resort font instead.
    ASSERT(primaryFont());
    GlyphData data = primaryFont()->missingGlyphData();
    if (variant == NormalVariant) {
        page->setGlyphDataForCharacter(c, data.glyph, data.fontData);
        data.fontData->setMaxGlyphPageTreeLevel(std::max(data.fontData->maxGlyphPageTreeLevel(), node->level()));
    }
    return data;
}

// FIXME: This function may not work if the emphasis mark uses a complex script, but none of the
// standard emphasis marks do so.
bool Font::getEmphasisMarkGlyphData(const AtomicString& mark, GlyphData& glyphData) const
{
    if (mark.isEmpty())
        return false;

    UChar32 character = mark[0];

    if (U16_IS_SURROGATE(character)) {
        if (!U16_IS_SURROGATE_LEAD(character))
            return false;

        if (mark.length() < 2)
            return false;

        UChar low = mark[1];
        if (!U16_IS_TRAIL(low))
            return false;

        character = U16_GET_SUPPLEMENTARY(character, low);
    }

    bool normalizeSpace = false;
    glyphData = glyphDataForCharacter(character, false, normalizeSpace, EmphasisMarkVariant);
    return true;
}

int Font::emphasisMarkAscent(const AtomicString& mark) const
{
    FontCachePurgePreventer purgePreventer;

    GlyphData markGlyphData;
    if (!getEmphasisMarkGlyphData(mark, markGlyphData))
        return 0;

    const SimpleFontData* markFontData = markGlyphData.fontData;
    ASSERT(markFontData);
    if (!markFontData)
        return 0;

    return markFontData->fontMetrics().ascent();
}

int Font::emphasisMarkDescent(const AtomicString& mark) const
{
    FontCachePurgePreventer purgePreventer;

    GlyphData markGlyphData;
    if (!getEmphasisMarkGlyphData(mark, markGlyphData))
        return 0;

    const SimpleFontData* markFontData = markGlyphData.fontData;
    ASSERT(markFontData);
    if (!markFontData)
        return 0;

    return markFontData->fontMetrics().descent();
}

int Font::emphasisMarkHeight(const AtomicString& mark) const
{
    FontCachePurgePreventer purgePreventer;

    GlyphData markGlyphData;
    if (!getEmphasisMarkGlyphData(mark, markGlyphData))
        return 0;

    const SimpleFontData* markFontData = markGlyphData.fontData;
    ASSERT(markFontData);
    if (!markFontData)
        return 0;

    return markFontData->fontMetrics().height();
}

void Font::paintGlyphs(SkCanvas* canvas, const SkPaint& paint, const SimpleFontData* font,
    const Glyph glyphs[], unsigned numGlyphs,
    const SkPoint pos[], const FloatRect& textRect, float deviceScaleFactor) const
{
    SkPaint fontPaint(paint);
    font->platformData().setupPaint(&fontPaint, deviceScaleFactor, this);
    fontPaint.setTextEncoding(SkPaint::kGlyphID_TextEncoding);
    canvas->drawPosText(glyphs, numGlyphs * sizeof(Glyph), pos, fontPaint);
}

void Font::paintGlyphsHorizontal(SkCanvas* canvas, const SkPaint& paint, const SimpleFontData* font,
    const Glyph glyphs[], unsigned numGlyphs,
    const SkScalar xpos[], SkScalar constY, const FloatRect& textRect, float deviceScaleFactor) const
{
    SkPaint fontPaint(paint);
    font->platformData().setupPaint(&fontPaint, deviceScaleFactor, this);
    fontPaint.setTextEncoding(SkPaint::kGlyphID_TextEncoding);
    canvas->drawPosTextH(glyphs, numGlyphs * sizeof(Glyph), xpos, constY, fontPaint);
}

void Font::drawGlyphs(SkCanvas* canvas, const SkPaint& paint, const SimpleFontData* font,
    const GlyphBuffer& glyphBuffer, unsigned from, unsigned numGlyphs,
    const FloatPoint& point, const FloatRect& textRect, float deviceScaleFactor) const
{
    ASSERT(glyphBuffer.size() >= from + numGlyphs);

    if (!glyphBuffer.hasVerticalOffsets()) {
        SkAutoSTMalloc<64, SkScalar> storage(numGlyphs);
        SkScalar* xpos = storage.get();
        for (unsigned i = 0; i < numGlyphs; i++)
            xpos[i] = SkFloatToScalar(point.x() + glyphBuffer.xOffsetAt(from + i));

        paintGlyphsHorizontal(canvas, paint, font, glyphBuffer.glyphs(from), numGlyphs, xpos,
            SkFloatToScalar(point.y()), textRect, deviceScaleFactor);
        return;
    }

    bool drawVertically = font->platformData().isVerticalAnyUpright() && font->verticalData();

    int canvasStackLevel = canvas->getSaveCount();
    if (drawVertically) {
        canvas->save();
        canvas->concat(affineTransformToSkMatrix(AffineTransform(0, -1, 1, 0, point.x(), point.y())));
        canvas->concat(affineTransformToSkMatrix(AffineTransform(1, 0, 0, 1, -point.x(), -point.y())));
    }

    const float verticalBaselineXOffset = drawVertically ? SkFloatToScalar(font->fontMetrics().floatAscent() - font->fontMetrics().floatAscent(IdeographicBaseline)) : 0;

    ASSERT(glyphBuffer.hasVerticalOffsets());
    SkAutoSTMalloc<32, SkPoint> storage(numGlyphs);
    SkPoint* pos = storage.get();
    for (unsigned i = 0; i < numGlyphs; i++) {
        pos[i].set(
            SkFloatToScalar(point.x() + verticalBaselineXOffset + glyphBuffer.xOffsetAt(from + i)),
            SkFloatToScalar(point.y() + glyphBuffer.yOffsetAt(from + i)));
    }

    paintGlyphs(canvas, paint, font, glyphBuffer.glyphs(from), numGlyphs, pos, textRect, deviceScaleFactor);
    canvas->restoreToCount(canvasStackLevel);
}

void Font::drawTextBlob(SkCanvas* canvas, const SkPaint& paint, const SkTextBlob* blob, const SkPoint& origin) const
{
    ASSERT(RuntimeEnabledFeatures::textBlobEnabled());

    canvas->drawTextBlob(blob, origin.x(), origin.y(), paint);
}

float Font::floatWidthForComplexText(const TextRun& run, HashSet<const SimpleFontData*>* fallbackFonts, FloatRect* glyphBounds) const
{
    CachingWordShaper& shaper = m_fontFallbackList->cachingWordShaper();
    float width = shaper.width(this, run, fallbackFonts, glyphBounds);
    return width;
}

// Return the code point index for the given |x| offset into the text run.
int Font::offsetForPositionForComplexText(const TextRun& run, float xFloat,
    bool includePartialGlyphs) const
{
#ifndef MINIBLINK_NO_HARFBUZZ
    HarfBuzzShaper shaper(this, run);
    RefPtr<ShapeResult> shapeResult = shaper.shapeResult();
    if (!shapeResult)
        return 0;
    return shapeResult->offsetForPosition(xFloat);
#else
	  notImplemented();
	  return 0;
#endif // MINIBLINK_NOT_IMPLEMENTED
}

// Return the rectangle for selecting the given range of code-points in the TextRun.
FloatRect Font::selectionRectForComplexText(const TextRun& run,
    const FloatPoint& point, int height, int from, int to) const
{
    CachingWordShaper& shaper = m_fontFallbackList->cachingWordShaper();
    return shaper.selectionRect(this, run, point, height, from, to);
}

void Font::drawGlyphBuffer(SkCanvas* canvas, const SkPaint& paint, const TextRunPaintInfo& runInfo, const GlyphBuffer& glyphBuffer, const FloatPoint& point, float deviceScaleFactor) const
{
    if (glyphBuffer.isEmpty())
        return;

    if (RuntimeEnabledFeatures::textBlobEnabled()) {
        // Enabling text-blobs forces the blob rendering path even for uncacheable blobs.
        TextBlobPtr uncacheableTextBlob;
        TextBlobPtr& textBlob = runInfo.cachedTextBlob ? *runInfo.cachedTextBlob : uncacheableTextBlob;

        textBlob = buildTextBlob(glyphBuffer);
        if (textBlob) {
            drawTextBlob(canvas, paint, textBlob.get(), point.data());
            return;
        }
    }

    // Draw each contiguous run of glyphs that use the same font data.
    const SimpleFontData* fontData = glyphBuffer.fontDataAt(0);
    unsigned lastFrom = 0;
    unsigned nextGlyph;

    for (nextGlyph = 0; nextGlyph < glyphBuffer.size(); ++nextGlyph) {
        const SimpleFontData* nextFontData = glyphBuffer.fontDataAt(nextGlyph);

        if (nextFontData != fontData) {
            drawGlyphs(canvas, paint, fontData, glyphBuffer, lastFrom, nextGlyph - lastFrom, point, runInfo.bounds, deviceScaleFactor);
            lastFrom = nextGlyph;
            fontData = nextFontData;
        }
    }

    drawGlyphs(canvas, paint, fontData, glyphBuffer, lastFrom, nextGlyph - lastFrom, point, runInfo.bounds, deviceScaleFactor);
}

float Font::floatWidthForSimpleText(const TextRun& run, HashSet<const SimpleFontData*>* fallbackFonts, FloatRect* glyphBounds) const
{
    SimpleShaper shaper(this, run, nullptr, fallbackFonts, glyphBounds);
    shaper.advance(run.length());
    return shaper.runWidthSoFar();
}

FloatRect Font::selectionRectForSimpleText(const TextRun& run, const FloatPoint& point, int h, int from, int to, bool accountForGlyphBounds) const
{
    FloatRect bounds;
    SimpleShaper shaper(this, run, nullptr, nullptr, accountForGlyphBounds ? &bounds : nullptr);
    shaper.advance(from);
    float fromX = shaper.runWidthSoFar();
    shaper.advance(to);
    float toX = shaper.runWidthSoFar();

    if (run.rtl()) {
        shaper.advance(run.length());
        float totalWidth = shaper.runWidthSoFar();
        float beforeWidth = fromX;
        float afterWidth = toX;
        fromX = totalWidth - afterWidth;
        toX = totalWidth - beforeWidth;
    }

    return FloatRect(point.x() + fromX,
        accountForGlyphBounds ? bounds.y(): point.y(),
        toX - fromX,
        accountForGlyphBounds ? bounds.maxY()- bounds.y(): h);
}

int Font::offsetForPositionForSimpleText(const TextRun& run, float x, bool includePartialGlyphs) const
{
    float delta = x;

    SimpleShaper shaper(this, run);
    unsigned offset;
    if (run.rtl()) {
        delta -= floatWidthForSimpleText(run);
        while (1) {
            offset = shaper.currentOffset();
            float w;
            if (!shaper.advanceOneCharacter(w))
                break;
            delta += w;
            if (includePartialGlyphs) {
                if (delta - w / 2 >= 0)
                    break;
            } else {
                if (delta >= 0)
                    break;
            }
        }
    } else {
        while (1) {
            offset = shaper.currentOffset();
            float w;
            if (!shaper.advanceOneCharacter(w))
                break;
            delta -= w;
            if (includePartialGlyphs) {
                if (delta + w / 2 <= 0)
                    break;
            } else {
                if (delta <= 0)
                    break;
            }
        }
    }

    return offset;
}

bool Font::loadingCustomFonts() const
{
    return m_fontFallbackList && m_fontFallbackList->loadingCustomFonts();
}

bool Font::isFallbackValid() const
{
    return !m_fontFallbackList || m_fontFallbackList->isValid();
}

} // namespace blink

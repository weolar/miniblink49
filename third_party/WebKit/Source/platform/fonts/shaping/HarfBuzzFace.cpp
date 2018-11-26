/*
 * Copyright (c) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MINIBLINK_NO_HARFBUZZ
#include "config.h"
#include "platform/fonts/shaping/HarfBuzzFace.h"

#include "third_party/harfbuzz-ng/src/hb-ot.h"
#include "third_party/harfbuzz-ng/src/hb.h"
#if OS(MACOSX)
#include "hb-coretext.h"
#endif
#include "SkPaint.h"
#include "SkPath.h"
#include "SkPoint.h"
#include "SkRect.h"
#include "SkTypeface.h"
#include "SkUtils.h"
#include "platform/fonts/FontCache.h"
#include "platform/fonts/FontPlatformData.h"
#include "platform/fonts/SimpleFontData.h"
#include "platform/fonts/shaping/HarfBuzzShaper.h"
#include "wtf/HashMap.h"

namespace blink {

const hb_tag_t HarfBuzzFace::vertTag = HB_TAG('v', 'e', 'r', 't');

// Though we have FontCache class, which provides the cache mechanism for
// WebKit's font objects, we also need additional caching layer for HarfBuzz
// to reduce the memory consumption because hb_face_t should be associated with
// underling font data (e.g. CTFontRef, FTFace).

class FaceCacheEntry : public RefCounted<FaceCacheEntry> {
public:
    static PassRefPtr<FaceCacheEntry> create(hb_face_t* face)
    {
        ASSERT(face);
        return adoptRef(new FaceCacheEntry(face));
    }
    ~FaceCacheEntry()
    {
        hb_face_destroy(m_face);
    }

    hb_face_t* face() { return m_face; }
    HashMap<uint32_t, uint16_t>* glyphCache() { return &m_glyphCache; }

private:
    explicit FaceCacheEntry(hb_face_t* face)
        : m_face(face)
    { }

    hb_face_t* m_face;
    HashMap<uint32_t, uint16_t> m_glyphCache;
};

typedef HashMap<uint64_t, RefPtr<FaceCacheEntry>, WTF::IntHash<uint64_t>, WTF::UnsignedWithZeroKeyHashTraits<uint64_t>> HarfBuzzFaceCache;

static HarfBuzzFaceCache* harfBuzzFaceCache()
{
    DEFINE_STATIC_LOCAL(HarfBuzzFaceCache, s_harfBuzzFaceCache, ());
    return &s_harfBuzzFaceCache;
}

HarfBuzzFace::HarfBuzzFace(FontPlatformData* platformData, uint64_t uniqueID)
    : m_platformData(platformData)
    , m_uniqueID(uniqueID)
    , m_scriptForVerticalText(HB_SCRIPT_INVALID)
{
    HarfBuzzFaceCache::AddResult result = harfBuzzFaceCache()->add(m_uniqueID, nullptr);
    if (result.isNewEntry)
        result.storedValue->value = FaceCacheEntry::create(createFace());
    result.storedValue->value->ref();
    m_face = result.storedValue->value->face();
    m_glyphCacheForFaceCacheEntry = result.storedValue->value->glyphCache();
}

HarfBuzzFace::~HarfBuzzFace()
{
    HarfBuzzFaceCache::iterator result = harfBuzzFaceCache()->find(m_uniqueID);
    ASSERT_WITH_SECURITY_IMPLICATION(result != harfBuzzFaceCache()->end());
    ASSERT(result.get()->value->refCount() > 1);
    result.get()->value->deref();
    if (result.get()->value->refCount() == 1)
        harfBuzzFaceCache()->remove(m_uniqueID);
}

static hb_script_t findScriptForVerticalGlyphSubstitution(hb_face_t* face)
{
    static const unsigned maxCount = 32;

    unsigned scriptCount = maxCount;
    hb_tag_t scriptTags[maxCount];
    hb_ot_layout_table_get_script_tags(face, HB_OT_TAG_GSUB, 0, &scriptCount, scriptTags);
    for (unsigned scriptIndex = 0; scriptIndex < scriptCount; ++scriptIndex) {
        unsigned languageCount = maxCount;
        hb_tag_t languageTags[maxCount];
        hb_ot_layout_script_get_language_tags(face, HB_OT_TAG_GSUB, scriptIndex, 0, &languageCount, languageTags);
        unsigned featureIndex;
        for (unsigned languageIndex = 0; languageIndex < languageCount; ++languageIndex) {
            if (hb_ot_layout_language_find_feature(face, HB_OT_TAG_GSUB, scriptIndex, languageIndex, HarfBuzzFace::vertTag, &featureIndex))
                return hb_ot_tag_to_script(scriptTags[scriptIndex]);
        }
        // Try DefaultLangSys if all LangSys failed.
        if (hb_ot_layout_language_find_feature(face, HB_OT_TAG_GSUB, scriptIndex, HB_OT_LAYOUT_DEFAULT_LANGUAGE_INDEX, HarfBuzzFace::vertTag, &featureIndex))
            return hb_ot_tag_to_script(scriptTags[scriptIndex]);
    }
    return HB_SCRIPT_INVALID;
}

void HarfBuzzFace::setScriptForVerticalGlyphSubstitution(hb_buffer_t* buffer)
{
    if (m_scriptForVerticalText == HB_SCRIPT_INVALID)
        m_scriptForVerticalText = findScriptForVerticalGlyphSubstitution(m_face);
    hb_buffer_set_script(buffer, m_scriptForVerticalText);
}

struct HarfBuzzFontData {
    HarfBuzzFontData(WTF::HashMap<uint32_t, uint16_t>* glyphCacheForFaceCacheEntry, hb_face_t* face)
        : m_glyphCacheForFaceCacheEntry(glyphCacheForFaceCacheEntry)
        , m_face(face)
        , m_hbOpenTypeFont(nullptr)
    { }

    ~HarfBuzzFontData()
    {
        if (m_hbOpenTypeFont)
            hb_font_destroy(m_hbOpenTypeFont);
    }

    SkPaint m_paint;
    RefPtr<SimpleFontData> m_simpleFontData;
    WTF::HashMap<uint32_t, uint16_t>* m_glyphCacheForFaceCacheEntry;
    hb_face_t* m_face;
    hb_font_t* m_hbOpenTypeFont;
};

static hb_position_t SkiaScalarToHarfBuzzPosition(SkScalar value)
{
    return SkScalarToFixed(value);
}

static void SkiaGetGlyphWidthAndExtents(SkPaint* paint, hb_codepoint_t codepoint, hb_position_t* width, hb_glyph_extents_t* extents)
{
    ASSERT(codepoint <= 0xFFFF);
    paint->setTextEncoding(SkPaint::kGlyphID_TextEncoding);

    SkScalar skWidth;
    SkRect skBounds;
    uint16_t glyph = codepoint;

    paint->getTextWidths(&glyph, sizeof(glyph), &skWidth, &skBounds);
    if (width)
        *width = SkiaScalarToHarfBuzzPosition(skWidth);
    if (extents) {
        // Invert y-axis because Skia is y-grows-down but we set up HarfBuzz to be y-grows-up.
        extents->x_bearing = SkiaScalarToHarfBuzzPosition(skBounds.fLeft);
        extents->y_bearing = SkiaScalarToHarfBuzzPosition(-skBounds.fTop);
        extents->width = SkiaScalarToHarfBuzzPosition(skBounds.width());
        extents->height = SkiaScalarToHarfBuzzPosition(-skBounds.height());
    }
}

static hb_bool_t harfBuzzGetGlyph(hb_font_t* hbFont, void* fontData, hb_codepoint_t unicode, hb_codepoint_t variationSelector, hb_codepoint_t* glyph, void* userData)
{
    HarfBuzzFontData* hbFontData = reinterpret_cast<HarfBuzzFontData*>(fontData);

    if (variationSelector) {
#if OS(LINUX)
        // TODO(kojii): Linux non-official builds cannot use new HB APIs
        // until crbug.com/462689 resolved or pangoft2 updates its HB.
        return false;
#else
        // Skia does not support variation selectors, but hb does.
        // We're not fully ready to switch to hb-ot-font yet,
        // but are good enough to get glyph IDs for OpenType fonts.
        if (!hbFontData->m_hbOpenTypeFont) {
            hbFontData->m_hbOpenTypeFont = hb_font_create(hbFontData->m_face);
            hb_ot_font_set_funcs(hbFontData->m_hbOpenTypeFont);
        }
        return hb_font_get_glyph(hbFontData->m_hbOpenTypeFont, unicode, variationSelector, glyph);
        // When not found, glyph_func should return false rather than fallback to the base.
        // http://lists.freedesktop.org/archives/harfbuzz/2015-May/004888.html
#endif
    }

    WTF::HashMap<uint32_t, uint16_t>::AddResult result = hbFontData->m_glyphCacheForFaceCacheEntry->add(unicode, 0);
    if (result.isNewEntry) {
        SkPaint* paint = &hbFontData->m_paint;
        paint->setTextEncoding(SkPaint::kUTF32_TextEncoding);
        uint16_t glyph16;
        paint->textToGlyphs(&unicode, sizeof(hb_codepoint_t), &glyph16);
        result.storedValue->value = glyph16;
        *glyph = glyph16;
    }
    *glyph = result.storedValue->value;
    return !!*glyph;
}

static hb_position_t harfBuzzGetGlyphHorizontalAdvance(hb_font_t* hbFont, void* fontData, hb_codepoint_t glyph, void* userData)
{
    HarfBuzzFontData* hbFontData = reinterpret_cast<HarfBuzzFontData*>(fontData);
    hb_position_t advance = 0;

    SkiaGetGlyphWidthAndExtents(&hbFontData->m_paint, glyph, &advance, 0);
    return advance;
}

static hb_bool_t harfBuzzGetGlyphHorizontalOrigin(hb_font_t* hbFont, void* fontData, hb_codepoint_t glyph, hb_position_t* x, hb_position_t* y, void* userData)
{
    // Just return true, following the way that HarfBuzz-FreeType
    // implementation does.
    return true;
}

static hb_bool_t harfBuzzGetGlyphVerticalOrigin(hb_font_t* hbFont, void* fontData, hb_codepoint_t glyph, hb_position_t* x, hb_position_t* y, void* userData)
{
    HarfBuzzFontData* hbFontData = reinterpret_cast<HarfBuzzFontData*>(fontData);
    const OpenTypeVerticalData* verticalData = hbFontData->m_simpleFontData->verticalData();
    if (!verticalData)
        return false;

    float result[] = { 0, 0 };
    Glyph theGlyph = glyph;
    verticalData->getVerticalTranslationsForGlyphs(hbFontData->m_simpleFontData.get(), &theGlyph, 1, result);
    *x = SkiaScalarToHarfBuzzPosition(-result[0]);
    *y = SkiaScalarToHarfBuzzPosition(-result[1]);
    return true;
}

static hb_position_t harfBuzzGetGlyphVerticalAdvance(hb_font_t* hbFont, void* fontData, hb_codepoint_t glyph, void* userData)
{
    HarfBuzzFontData* hbFontData = reinterpret_cast<HarfBuzzFontData*>(fontData);
    const OpenTypeVerticalData* verticalData = hbFontData->m_simpleFontData->verticalData();
    if (!verticalData)
        return SkiaScalarToHarfBuzzPosition(hbFontData->m_simpleFontData->fontMetrics().height());

    Glyph theGlyph = glyph;
    float advanceHeight = -verticalData->advanceHeight(hbFontData->m_simpleFontData.get(), theGlyph);
    return SkiaScalarToHarfBuzzPosition(SkFloatToScalar(advanceHeight));
}

static hb_position_t harfBuzzGetGlyphHorizontalKerning(hb_font_t*, void* fontData, hb_codepoint_t leftGlyph, hb_codepoint_t rightGlyph, void*)
{
    HarfBuzzFontData* hbFontData = reinterpret_cast<HarfBuzzFontData*>(fontData);
    if (hbFontData->m_paint.isVerticalText()) {
        // We don't support cross-stream kerning
        return 0;
    }

    SkTypeface* typeface = hbFontData->m_paint.getTypeface();

    const uint16_t glyphs[2] = { static_cast<uint16_t>(leftGlyph), static_cast<uint16_t>(rightGlyph) };
    int32_t kerningAdjustments[1] = { 0 };

    if (typeface->getKerningPairAdjustments(glyphs, 2, kerningAdjustments)) {
        SkScalar upm = SkIntToScalar(typeface->getUnitsPerEm());
        SkScalar size = hbFontData->m_paint.getTextSize();
        return SkiaScalarToHarfBuzzPosition(SkScalarMulDiv(SkIntToScalar(kerningAdjustments[0]), size, upm));
    }

    return 0;
}

static hb_bool_t harfBuzzGetGlyphExtents(hb_font_t* hbFont, void* fontData, hb_codepoint_t glyph, hb_glyph_extents_t* extents, void* userData)
{
    HarfBuzzFontData* hbFontData = reinterpret_cast<HarfBuzzFontData*>(fontData);

    SkiaGetGlyphWidthAndExtents(&hbFontData->m_paint, glyph, 0, extents);
    return true;
}

static hb_font_funcs_t* harfBuzzSkiaGetFontFuncs()
{
    static hb_font_funcs_t* harfBuzzSkiaFontFuncs = 0;

    // We don't set callback functions which we can't support.
    // HarfBuzz will use the fallback implementation if they aren't set.
    if (!harfBuzzSkiaFontFuncs) {
        harfBuzzSkiaFontFuncs = hb_font_funcs_create();
        hb_font_funcs_set_glyph_func(harfBuzzSkiaFontFuncs, harfBuzzGetGlyph, 0, 0);
        hb_font_funcs_set_glyph_h_advance_func(harfBuzzSkiaFontFuncs, harfBuzzGetGlyphHorizontalAdvance, 0, 0);
        hb_font_funcs_set_glyph_h_kerning_func(harfBuzzSkiaFontFuncs, harfBuzzGetGlyphHorizontalKerning, 0, 0);
        hb_font_funcs_set_glyph_h_origin_func(harfBuzzSkiaFontFuncs, harfBuzzGetGlyphHorizontalOrigin, 0, 0);
        hb_font_funcs_set_glyph_v_advance_func(harfBuzzSkiaFontFuncs, harfBuzzGetGlyphVerticalAdvance, 0, 0);
        hb_font_funcs_set_glyph_v_origin_func(harfBuzzSkiaFontFuncs, harfBuzzGetGlyphVerticalOrigin, 0, 0);
        hb_font_funcs_set_glyph_extents_func(harfBuzzSkiaFontFuncs, harfBuzzGetGlyphExtents, 0, 0);
        hb_font_funcs_make_immutable(harfBuzzSkiaFontFuncs);
    }
    return harfBuzzSkiaFontFuncs;
}

#if !OS(MACOSX)
static hb_blob_t* harfBuzzSkiaGetTable(hb_face_t* face, hb_tag_t tag, void* userData)
{
    SkTypeface* typeface = reinterpret_cast<SkTypeface*>(userData);

    const size_t tableSize = typeface->getTableSize(tag);
    if (!tableSize) {
        return 0;
    }

    char* buffer = reinterpret_cast<char*>(fastMalloc(tableSize));
    if (!buffer)
        return 0;
    size_t actualSize = typeface->getTableData(tag, 0, tableSize, buffer);
    if (tableSize != actualSize) {
        fastFree(buffer);
        return 0;
    }

    return hb_blob_create(const_cast<char*>(buffer), tableSize, HB_MEMORY_MODE_WRITABLE, buffer, fastFree);
}
#endif

static void destroyHarfBuzzFontData(void* userData)
{
    HarfBuzzFontData* hbFontData = reinterpret_cast<HarfBuzzFontData*>(userData);
    delete hbFontData;
}

hb_face_t* HarfBuzzFace::createFace()
{
#if OS(MACOSX)
    hb_face_t* face = hb_coretext_face_create(m_platformData->cgFont());
#else
    hb_face_t* face = hb_face_create_for_tables(harfBuzzSkiaGetTable, m_platformData->typeface(), 0);
#endif
    ASSERT(face);
    return face;
}

hb_font_t* HarfBuzzFace::createFont() const
{
    HarfBuzzFontData* hbFontData = new HarfBuzzFontData(m_glyphCacheForFaceCacheEntry, m_face);
    m_platformData->setupPaint(&hbFontData->m_paint);
    hbFontData->m_simpleFontData = FontCache::fontCache()->fontDataFromFontPlatformData(m_platformData);
    ASSERT(hbFontData->m_simpleFontData);
    hb_font_t* font = hb_font_create(m_face);
    hb_font_set_funcs(font, harfBuzzSkiaGetFontFuncs(), hbFontData, destroyHarfBuzzFontData);
    float size = m_platformData->size();
    int scale = SkiaScalarToHarfBuzzPosition(size);
    hb_font_set_scale(font, scale, scale);
    hb_font_make_immutable(font);
    return font;
}

} // namespace blink

#endif
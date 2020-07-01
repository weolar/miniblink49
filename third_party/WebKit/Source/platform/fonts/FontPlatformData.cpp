/*
 * Copyright (C) 2011 Brent Fulgham
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
#include "platform/fonts/FontPlatformData.h"

#include "SkEndian.h"
#include "SkTypeface.h"
#ifndef MINIBLINK_NO_HARFBUZZ
#include "third_party/harfbuzz-ng/src/hb-ot.h"
#include "third_party/harfbuzz-ng/src/hb.h"
#endif
#include "platform/fonts/Character.h"
#include "platform/fonts/FontCache.h"
#ifndef MINIBLINK_NO_HARFBUZZ
#include "platform/fonts/shaping/HarfBuzzFace.h"
#endif
#include "wtf/HashMap.h"
#include "wtf/text/StringHash.h"
#include "wtf/text/WTFString.h"

#if OS(MACOSX)
#include "third_party/skia/include/ports/SkTypeface_mac.h"
#endif

using namespace std;

namespace blink {

FontPlatformData::FontPlatformData(WTF::HashTableDeletedValueType)
    : m_typeface(nullptr)
#if !OS(WIN)
    , m_family(CString())
#endif
    , m_textSize(0)
    , m_syntheticBold(false)
    , m_syntheticItalic(false)
    , m_orientation(FontOrientation::Horizontal)
#if !OS(MACOSX)
    , m_style(FontRenderStyle())
#endif
    , m_isHashTableDeletedValue(true)
#if OS(WIN)
    , m_paintTextFlags(0)
    , m_useSubpixelPositioning(false)
    , m_minSizeForAntiAlias(0)
    , m_minSizeForSubpixel(0)
#endif
{
}

FontPlatformData::FontPlatformData()
    : m_typeface(nullptr)
#if !OS(WIN)
    , m_family(CString())
#endif
    , m_textSize(0)
    , m_syntheticBold(false)
    , m_syntheticItalic(false)
    , m_orientation(FontOrientation::Horizontal)
#if !OS(MACOSX)
    , m_style(FontRenderStyle())
#endif
    , m_isHashTableDeletedValue(false)
#if OS(WIN)
    , m_paintTextFlags(0)
    , m_useSubpixelPositioning(false)
    , m_minSizeForAntiAlias(0)
    , m_minSizeForSubpixel(0)
#endif
{
}

FontPlatformData::FontPlatformData(float size, bool syntheticBold, bool syntheticItalic, FontOrientation orientation)
    : m_typeface(nullptr)
#if !OS(WIN)
    , m_family(CString())
#endif
    , m_textSize(size)
    , m_syntheticBold(syntheticBold)
    , m_syntheticItalic(syntheticItalic)
    , m_orientation(orientation)
#if !OS(MACOSX)
    , m_style(FontRenderStyle())
#endif
    , m_isHashTableDeletedValue(false)
#if OS(WIN)
    , m_paintTextFlags(0)
    , m_useSubpixelPositioning(false)
    , m_minSizeForAntiAlias(0)
    , m_minSizeForSubpixel(0)
#endif
{
}

FontPlatformData::FontPlatformData(const FontPlatformData& source)
    : m_typeface(source.m_typeface)
#if !OS(WIN)
    , m_family(source.m_family)
#endif
    , m_textSize(source.m_textSize)
    , m_syntheticBold(source.m_syntheticBold)
    , m_syntheticItalic(source.m_syntheticItalic)
    , m_orientation(source.m_orientation)
#if !OS(MACOSX)
    , m_style(source.m_style)
#endif
    //, m_harfBuzzFace(nullptr)
    , m_isHashTableDeletedValue(false)
#if OS(WIN)
    , m_paintTextFlags(source.m_paintTextFlags)
    , m_useSubpixelPositioning(source.m_useSubpixelPositioning)
    , m_minSizeForAntiAlias(source.m_minSizeForAntiAlias)
    , m_minSizeForSubpixel(source.m_minSizeForSubpixel)
#endif
{
}

FontPlatformData::FontPlatformData(const FontPlatformData& src, float textSize)
    : m_typeface(src.m_typeface)
#if !OS(WIN)
    , m_family(src.m_family)
#endif
    , m_textSize(textSize)
    , m_syntheticBold(src.m_syntheticBold)
    , m_syntheticItalic(src.m_syntheticItalic)
    , m_orientation(src.m_orientation)
#if !OS(MACOSX)
    , m_style(src.m_style)
#endif
    //, m_harfBuzzFace(nullptr)
    , m_isHashTableDeletedValue(false)
#if OS(WIN)
    , m_paintTextFlags(src.m_paintTextFlags)
    , m_useSubpixelPositioning(src.m_useSubpixelPositioning)
    , m_minSizeForAntiAlias(src.m_minSizeForAntiAlias)
    , m_minSizeForSubpixel(src.m_minSizeForSubpixel)
#endif
{
#if !OS(MACOSX)
    querySystemForRenderStyle(FontDescription::subpixelPositioning());
#endif
}

FontPlatformData::FontPlatformData(PassRefPtr<SkTypeface> tf, const char* family, float textSize, bool syntheticBold, bool syntheticItalic, FontOrientation orientation, bool subpixelTextPosition)
    : m_typeface(tf)
#if !OS(WIN)
    , m_family(family)
#endif
    , m_textSize(textSize)
    , m_syntheticBold(syntheticBold)
    , m_syntheticItalic(syntheticItalic)
    , m_orientation(orientation)
    , m_isHashTableDeletedValue(false)
#if OS(WIN)
    , m_paintTextFlags(0)
    , m_useSubpixelPositioning(subpixelTextPosition)
    , m_minSizeForAntiAlias(0)
    , m_minSizeForSubpixel(0)
#endif
{
#if !OS(MACOSX)
    querySystemForRenderStyle(subpixelTextPosition);
#endif
}

FontPlatformData::~FontPlatformData()
{
}

#if OS(MACOSX)
CTFontRef FontPlatformData::ctFont() const
{
    return SkTypeface_GetCTFontRef(m_typeface.get());
};

CGFontRef FontPlatformData::cgFont() const
{
    return CTFontCopyGraphicsFont(ctFont(), 0);
}
#endif

const FontPlatformData& FontPlatformData::operator=(const FontPlatformData& other)
{
    // Check for self-assignment.
    if (this == &other)
        return *this;

    m_typeface = other.m_typeface;
#if !OS(WIN)
    m_family = other.m_family;
#endif
    m_textSize = other.m_textSize;
    m_syntheticBold = other.m_syntheticBold;
    m_syntheticItalic = other.m_syntheticItalic;
    //m_harfBuzzFace = nullptr;
    m_orientation = other.m_orientation;
#if !OS(MACOSX)
    m_style = other.m_style;
#endif

#if OS(WIN)
    m_paintTextFlags = 0;
    m_minSizeForAntiAlias = other.m_minSizeForAntiAlias;
    m_minSizeForSubpixel = other.m_minSizeForSubpixel;
    m_useSubpixelPositioning = other.m_useSubpixelPositioning;
#endif

    return *this;
}

bool FontPlatformData::operator==(const FontPlatformData& a) const
{
    // If either of the typeface pointers are null then we test for pointer
    // equality. Otherwise, we call SkTypeface::Equal on the valid pointers.
    bool typefacesEqual = false;
    if (!typeface() || !a.typeface())
        typefacesEqual = typeface() == a.typeface();
    else
        typefacesEqual = SkTypeface::Equal(typeface(), a.typeface());

    return typefacesEqual
        && m_textSize == a.m_textSize
        && m_isHashTableDeletedValue == a.m_isHashTableDeletedValue
        && m_syntheticBold == a.m_syntheticBold
        && m_syntheticItalic == a.m_syntheticItalic
#if !OS(MACOSX)
        && m_style == a.m_style
#endif
        && m_orientation == a.m_orientation;
}

SkFontID FontPlatformData::uniqueID() const
{
    return typeface()->uniqueID();
}

String FontPlatformData::fontFamilyName() const
{
    ASSERT(this->typeface());
    SkTypeface::LocalizedStrings* fontFamilyIterator = this->typeface()->createFamilyNameIterator();
    SkTypeface::LocalizedString localizedString;
    while (fontFamilyIterator->next(&localizedString) && !localizedString.fString.size()) { }
    fontFamilyIterator->unref();
    return String(localizedString.fString.c_str());
}

SkTypeface* FontPlatformData::typeface() const
{
    return m_typeface.get();
}

HarfBuzzFace* FontPlatformData::harfBuzzFace() const
{
#ifndef MINIBLINK_NO_HARFBUZZ
    if (!m_harfBuzzFace)
        m_harfBuzzFace = HarfBuzzFace::create(const_cast<FontPlatformData*>(this), uniqueID());

    return m_harfBuzzFace.get();
#else
	notImplemented();
	return nullptr;
#endif // MINIBLINK_NO_HARFBUZZ
}

#ifndef MINIBLINK_NO_HARFBUZZ
static inline bool tableHasSpace(hb_face_t* face, hb_set_t* glyphs,
    hb_tag_t tag, hb_codepoint_t space)
{
    unsigned count = hb_ot_layout_table_get_lookup_count(face, tag);
    for (unsigned i = 0; i < count; i++) {
        hb_ot_layout_lookup_collect_glyphs(face, tag, i, glyphs, glyphs, glyphs,
            0);
        if (hb_set_has(glyphs, space))
            return true;
    }
    return false;
}
#endif // MINIBLINK_NO_HARFBUZZ

bool FontPlatformData::hasSpaceInLigaturesOrKerning(
    TypesettingFeatures features) const
{
#ifndef MINIBLINK_NO_HARFBUZZ
    const HarfBuzzFace* hbFace = harfBuzzFace();
    if (!hbFace)
        return true;

    hb_face_t* face = hbFace->face();
    ASSERT(face);
    hb_font_t* font = hbFace->createFont();
    ASSERT(font);

    hb_codepoint_t space;
    // If the space glyph isn't present in the font then each space character
    // will be rendering using a fallback font, which grantees that it cannot
    // affect the shape of the preceding word.
    if (!hb_font_get_glyph(font, spaceCharacter, 0, &space))
        return true;

    if (!hb_ot_layout_has_substitution(face)
        && !hb_ot_layout_has_positioning(face)) {
        return true;
    }

    bool foundSpaceInTable = false;
    hb_set_t* glyphs = hb_set_create();
    if (features & Kerning)
        foundSpaceInTable = tableHasSpace(face, glyphs, HB_OT_TAG_GPOS, space);
    if (!foundSpaceInTable && (features & Ligatures))
        foundSpaceInTable = tableHasSpace(face, glyphs, HB_OT_TAG_GSUB, space);

    hb_set_destroy(glyphs);
    hb_font_destroy(font);

    return foundSpaceInTable;
#else
	notImplemented();
	return false;
#endif // MINIBLINK_NOT_IMPLEMENTED
}

unsigned FontPlatformData::hash() const
{
    unsigned h = SkTypeface::UniqueID(typeface());
    h ^= 0x01010101 * ((static_cast<int>(m_isHashTableDeletedValue) << 3) | (static_cast<int>(m_orientation) << 2) | (static_cast<int>(m_syntheticBold) << 1) | static_cast<int>(m_syntheticItalic));

    // This memcpy is to avoid a reinterpret_cast that breaks strict-aliasing
    // rules. Memcpy is generally optimized enough so that performance doesn't
    // matter here.
    uint32_t textSizeBytes;
    memcpy(&textSizeBytes, &m_textSize, sizeof(uint32_t));
    h ^= textSizeBytes;

    return h;
}

#if !OS(MACOSX)
bool FontPlatformData::fontContainsCharacter(UChar32 character)
{
    SkPaint paint;
    setupPaint(&paint);
    paint.setTextEncoding(SkPaint::kUTF32_TextEncoding);

    uint16_t glyph;
    paint.textToGlyphs(&character, sizeof(character), &glyph);
    return glyph;
}

#endif

PassRefPtr<OpenTypeVerticalData> FontPlatformData::verticalData() const
{
#ifndef MINIBLINK_NO_HARFBUZZ
    return FontCache::fontCache()->getVerticalData(typeface()->uniqueID(), *this);
#else
    return nullptr;
#endif // MINIBLINK_NO_HARFBUZZ
}

PassRefPtr<SharedBuffer> FontPlatformData::openTypeTable(uint32_t table) const
{
    RefPtr<SharedBuffer> buffer;

    SkFontTableTag tag = SkEndianSwap32(table);
    const size_t tableSize = m_typeface->getTableSize(tag);
    if (tableSize) {
        Vector<char> tableBuffer(tableSize);
        m_typeface->getTableData(tag, 0, tableSize, &tableBuffer[0]);
        buffer = SharedBuffer::adoptVector(tableBuffer);
    }
    return buffer.release();
}

} // namespace blink

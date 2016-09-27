/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef FontBuilder_h
#define FontBuilder_h

#include "core/CSSValueKeywords.h"
#include "core/CoreExport.h"
#include "core/css/FontSize.h"
#include "platform/fonts/FontDescription.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class CSSValue;
class FontSelector;
class ComputedStyle;

class CORE_EXPORT FontBuilder {
    STACK_ALLOCATED();
    WTF_MAKE_NONCOPYABLE(FontBuilder);
public:
    FontBuilder(const Document&);

    void setInitial(float effectiveZoom);

    void didChangeEffectiveZoom();
    void didChangeTextOrientation();
    void didChangeWritingMode();

    FontFamily standardFontFamily() const;
    AtomicString standardFontFamilyName() const;
    AtomicString genericFontFamilyName(FontDescription::GenericFamilyType) const;

    void setWeight(FontWeight);
    void setSize(const FontDescription::Size&);
    void setSizeAdjust(const float aspectValue);
    void setStretch(FontStretch);
    void setFamilyDescription(const FontDescription::FamilyDescription&);
    void setFeatureSettings(PassRefPtr<FontFeatureSettings>);
    void setScript(const AtomicString& locale);
    void setStyle(FontStyle);
    void setVariant(FontVariant);
    void setVariantLigatures(const FontDescription::VariantLigatures&);
    void setTextRendering(TextRenderingMode);
    void setKerning(FontDescription::Kerning);
    void setFontSmoothing(FontSmoothingMode);

    // FIXME: These need to just vend a Font object eventually.
    void createFont(PassRefPtrWillBeRawPtr<FontSelector>, ComputedStyle&);

    void createFontForDocument(PassRefPtrWillBeRawPtr<FontSelector>, ComputedStyle&);

    bool fontDirty() const { return m_flags; }

    static FontDescription::FamilyDescription initialFamilyDescription() { return FontDescription::FamilyDescription(initialGenericFamily()); }
    static FontFeatureSettings* initialFeatureSettings() { return nullptr; }
    static FontDescription::GenericFamilyType initialGenericFamily() { return FontDescription::StandardFamily; }
    static FontDescription::Size initialSize() { return FontDescription::Size(FontSize::initialKeywordSize(), 0.0f, false); }
    static float initialSizeAdjust() { return FontSizeAdjustNone; }
    static TextRenderingMode initialTextRendering() { return AutoTextRendering; }
    static FontVariant initialVariant() { return FontVariantNormal; }
    static FontDescription::VariantLigatures initialVariantLigatures() { return FontDescription::VariantLigatures(); }
    static FontStyle initialStyle() { return FontStyleNormal; }
    static FontDescription::Kerning initialKerning() { return FontDescription::AutoKerning; }
    static FontSmoothingMode initialFontSmoothing() { return AutoSmoothing; }
    static FontStretch initialStretch() { return FontStretchNormal; }
    static FontWeight initialWeight() { return FontWeightNormal; }

private:

    void setFamilyDescription(FontDescription&, const FontDescription::FamilyDescription&);
    void setSize(FontDescription&, const FontDescription::Size&);
    void updateOrientation(FontDescription&, const ComputedStyle&);
    // This function fixes up the default font size if it detects that the current generic font family has changed. -dwh
    void checkForGenericFamilyChange(const FontDescription&, FontDescription&);
    void updateSpecifiedSize(FontDescription&, const ComputedStyle&);
    void updateComputedSize(FontDescription&, const ComputedStyle&);
    void updateAdjustedSize(FontDescription&, const ComputedStyle&, FontSelector*);

    float getComputedSizeFromSpecifiedSize(FontDescription&, float effectiveZoom, float specifiedSize);

    const Document& m_document;
    FontDescription m_fontDescription;

    enum class PropertySetFlag {
        Weight,
        Size,
        Stretch,
        Family,
        FeatureSettings,
        Script,
        Style,
        SizeAdjust,
        Variant,
        VariantLigatures,
        TextRendering,
        Kerning,
        FontSmoothing,

        EffectiveZoom,
        TextOrientation,
        WritingMode
    };

    void set(PropertySetFlag flag) { m_flags |= (1 << unsigned(flag)); }
    bool isSet(PropertySetFlag flag) const { return m_flags & (1 << unsigned(flag)); }

    unsigned m_flags;
};

}

#endif

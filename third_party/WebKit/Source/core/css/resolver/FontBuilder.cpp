/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
 * Copyright (C) 2015 Collabora Ltd. All rights reserved.
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
#include "core/css/resolver/FontBuilder.h"

#include "core/CSSValueKeywords.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/layout/LayoutTheme.h"
#include "core/layout/LayoutView.h"
#include "core/layout/TextAutosizer.h"
#include "platform/FontFamilyNames.h"
#include "platform/fonts/FontDescription.h"
#include "platform/text/LocaleToScriptMapping.h"

namespace blink {

FontBuilder::FontBuilder(const Document& document)
    : m_document(document)
    , m_flags(0)
{
    ASSERT(document.frame());
}

void FontBuilder::setInitial(float effectiveZoom)
{
    ASSERT(m_document.settings());
    if (!m_document.settings())
        return;

    setFamilyDescription(m_fontDescription, FontBuilder::initialFamilyDescription());
    setSize(m_fontDescription, FontBuilder::initialSize());
}

void FontBuilder::didChangeEffectiveZoom()
{
    set(PropertySetFlag::EffectiveZoom);
}

void FontBuilder::didChangeTextOrientation()
{
    set(PropertySetFlag::TextOrientation);
}

void FontBuilder::didChangeWritingMode()
{
    set(PropertySetFlag::WritingMode);
}

FontFamily FontBuilder::standardFontFamily() const
{
    FontFamily family;
    family.setFamily(standardFontFamilyName());
    return family;
}

AtomicString FontBuilder::standardFontFamilyName() const
{
    Settings* settings = m_document.settings();
    if (settings)
        return settings->genericFontFamilySettings().standard();
    return AtomicString();
}

AtomicString FontBuilder::genericFontFamilyName(FontDescription::GenericFamilyType genericFamily) const
{
    switch (genericFamily) {
    default:
        ASSERT_NOT_REACHED();
    case FontDescription::NoFamily:
        return AtomicString();
    case FontDescription::StandardFamily:
        return standardFontFamilyName();
    case FontDescription::SerifFamily:
        return FontFamilyNames::webkit_serif;
    case FontDescription::SansSerifFamily:
        return FontFamilyNames::webkit_sans_serif;
    case FontDescription::MonospaceFamily:
        return FontFamilyNames::webkit_monospace;
    case FontDescription::CursiveFamily:
        return FontFamilyNames::webkit_cursive;
    case FontDescription::FantasyFamily:
        return FontFamilyNames::webkit_fantasy;
    case FontDescription::PictographFamily:
        return FontFamilyNames::webkit_pictograph;
    }
}

void FontBuilder::setFamilyDescription(const FontDescription::FamilyDescription& familyDescription)
{
    setFamilyDescription(m_fontDescription, familyDescription);
}

void FontBuilder::setWeight(FontWeight fontWeight)
{
    set(PropertySetFlag::Weight);

    m_fontDescription.setWeight(fontWeight);
}

void FontBuilder::setSize(const FontDescription::Size& size)
{
    setSize(m_fontDescription, size);
}

void FontBuilder::setSizeAdjust(float aspectValue)
{
    set(PropertySetFlag::SizeAdjust);

    m_fontDescription.setSizeAdjust(aspectValue);
}

void FontBuilder::setStretch(FontStretch fontStretch)
{
    set(PropertySetFlag::Stretch);

    m_fontDescription.setStretch(fontStretch);
}

void FontBuilder::setScript(const AtomicString& locale)
{
    set(PropertySetFlag::Script);

    m_fontDescription.setLocale(locale);
    m_fontDescription.setScript(localeToScriptCodeForFontSelection(locale));
}

void FontBuilder::setStyle(FontStyle italic)
{
    set(PropertySetFlag::Style);

    m_fontDescription.setStyle(italic);
}

void FontBuilder::setVariant(FontVariant smallCaps)
{
    set(PropertySetFlag::Variant);

    m_fontDescription.setVariant(smallCaps);
}

void FontBuilder::setVariantLigatures(const FontDescription::VariantLigatures& ligatures)
{
    set(PropertySetFlag::VariantLigatures);

    m_fontDescription.setVariantLigatures(ligatures);
}

void FontBuilder::setTextRendering(TextRenderingMode textRenderingMode)
{
    set(PropertySetFlag::TextRendering);

    m_fontDescription.setTextRendering(textRenderingMode);
}

void FontBuilder::setKerning(FontDescription::Kerning kerning)
{
    set(PropertySetFlag::Kerning);

    m_fontDescription.setKerning(kerning);
}

void FontBuilder::setFontSmoothing(FontSmoothingMode foontSmoothingMode)
{
    set(PropertySetFlag::FontSmoothing);

    m_fontDescription.setFontSmoothing(foontSmoothingMode);
}

void FontBuilder::setFeatureSettings(PassRefPtr<FontFeatureSettings> settings)
{
    set(PropertySetFlag::FeatureSettings);

    m_fontDescription.setFeatureSettings(settings);
}

void FontBuilder::setFamilyDescription(FontDescription& fontDescription, const FontDescription::FamilyDescription& familyDescription)
{
    set(PropertySetFlag::Family);

    bool isInitial = familyDescription.genericFamily == FontDescription::StandardFamily && familyDescription.family.familyIsEmpty();

    fontDescription.setGenericFamily(familyDescription.genericFamily);
    fontDescription.setFamily(isInitial ? standardFontFamily() : familyDescription.family);
}

void FontBuilder::setSize(FontDescription& fontDescription, const FontDescription::Size& size)
{
    float specifiedSize = size.value;

    if (specifiedSize < 0)
        return;

    set(PropertySetFlag::Size);

    // Overly large font sizes will cause crashes on some platforms (such as Windows).
    // Cap font size here to make sure that doesn't happen.
    specifiedSize = std::min(maximumAllowedFontSize, specifiedSize);

    fontDescription.setKeywordSize(size.keyword);
    fontDescription.setSpecifiedSize(specifiedSize);
    fontDescription.setIsAbsoluteSize(size.isAbsolute);
}

float FontBuilder::getComputedSizeFromSpecifiedSize(FontDescription& fontDescription, float effectiveZoom, float specifiedSize)
{
    float zoomFactor = effectiveZoom;
    // FIXME: Why is this here!!!!?!
    if (LocalFrame* frame = m_document.frame())
        zoomFactor *= frame->textZoomFactor();

    return FontSize::getComputedSizeFromSpecifiedSize(&m_document, zoomFactor, fontDescription.isAbsoluteSize(), specifiedSize);
}

static FontOrientation fontOrientation(const ComputedStyle& style)
{
    if (style.isHorizontalWritingMode())
        return FontOrientation::Horizontal;

    switch (style.textOrientation()) {
    case TextOrientationVerticalRight:
        return FontOrientation::VerticalMixed;
    case TextOrientationUpright:
        return FontOrientation::VerticalUpright;
    case TextOrientationSideways:
        if (style.writingMode() == LeftToRightWritingMode) {
            // FIXME: This should map to sideways-left, which is not supported yet.
            return FontOrientation::VerticalRotated;
        }
        return FontOrientation::VerticalRotated;
    case TextOrientationSidewaysRight:
        return FontOrientation::VerticalRotated;
    default:
        ASSERT_NOT_REACHED();
        return FontOrientation::VerticalMixed;
    }
}

void FontBuilder::updateOrientation(FontDescription& description, const ComputedStyle& style)
{
    description.setOrientation(fontOrientation(style));
}

void FontBuilder::checkForGenericFamilyChange(const FontDescription& oldDescription, FontDescription& newDescription)
{
    if (newDescription.isAbsoluteSize())
        return;

    if (newDescription.isMonospace() == oldDescription.isMonospace())
        return;

    // For now, lump all families but monospace together.
    if (newDescription.genericFamily() != FontDescription::MonospaceFamily
        && oldDescription.genericFamily() != FontDescription::MonospaceFamily)
        return;

    // We know the parent is monospace or the child is monospace, and that font
    // size was unspecified. We want to scale our font size as appropriate.
    // If the font uses a keyword size, then we refetch from the table rather than
    // multiplying by our scale factor.
    float size;
    if (newDescription.keywordSize()) {
        size = FontSize::fontSizeForKeyword(&m_document, newDescription.keywordSize(), newDescription.isMonospace());
    } else {
        Settings* settings = m_document.settings();
        float fixedScaleFactor = (settings && settings->defaultFixedFontSize() && settings->defaultFontSize())
            ? static_cast<float>(settings->defaultFixedFontSize()) / settings->defaultFontSize()
            : 1;
        size = oldDescription.isMonospace() ?
            newDescription.specifiedSize() / fixedScaleFactor :
            newDescription.specifiedSize() * fixedScaleFactor;
    }

    newDescription.setSpecifiedSize(size);
}

void FontBuilder::updateSpecifiedSize(FontDescription& fontDescription, const ComputedStyle& style)
{
    float specifiedSize = fontDescription.specifiedSize();

    if (!specifiedSize && fontDescription.keywordSize())
        specifiedSize = FontSize::fontSizeForKeyword(&m_document, fontDescription.keywordSize(), fontDescription.isMonospace());

    fontDescription.setSpecifiedSize(specifiedSize);

    checkForGenericFamilyChange(style.fontDescription(), fontDescription);
}

void FontBuilder::updateAdjustedSize(FontDescription& fontDescription, const ComputedStyle& style, FontSelector* fontSelector)
{
    const float specifiedSize = fontDescription.specifiedSize();
    if (!fontDescription.hasSizeAdjust() || !specifiedSize)
        return;

    // We need to create a temporal Font to get xHeight of a primary font.
    // The aspect value is based on the xHeight of the font for the computed font size,
    // so we need to reset the adjustedSize to computedSize. See FontDescription::effectiveFontSize.
    fontDescription.setAdjustedSize(fontDescription.computedSize());

    Font font(fontDescription);
    font.update(fontSelector);
    if (!font.fontMetrics().hasXHeight())
        return;

    const float sizeAdjust = fontDescription.sizeAdjust();
    float aspectValue = font.fontMetrics().xHeight() / specifiedSize;
    float adjustedSize = (sizeAdjust / aspectValue) * specifiedSize;
    adjustedSize = getComputedSizeFromSpecifiedSize(fontDescription, style.effectiveZoom(), adjustedSize);

    float multiplier = style.textAutosizingMultiplier();
    if (multiplier > 1)
        adjustedSize = TextAutosizer::computeAutosizedFontSize(adjustedSize, multiplier);
    fontDescription.setAdjustedSize(adjustedSize);
}

void FontBuilder::updateComputedSize(FontDescription& fontDescription, const ComputedStyle& style)
{
    float computedSize = getComputedSizeFromSpecifiedSize(fontDescription, style.effectiveZoom(), fontDescription.specifiedSize());
    float multiplier = style.textAutosizingMultiplier();
    if (multiplier > 1)
        computedSize = TextAutosizer::computeAutosizedFontSize(computedSize, multiplier);
    fontDescription.setComputedSize(computedSize);
}

void FontBuilder::createFont(PassRefPtrWillBeRawPtr<FontSelector> fontSelector, ComputedStyle& style)
{
    if (!m_flags)
        return;

    FontDescription description = style.fontDescription();

    if (isSet(PropertySetFlag::Family)) {
        description.setGenericFamily(m_fontDescription.genericFamily());
        description.setFamily(m_fontDescription.family());
    }
    if (isSet(PropertySetFlag::Size)) {
        description.setKeywordSize(m_fontDescription.keywordSize());
        description.setSpecifiedSize(m_fontDescription.specifiedSize());
        description.setIsAbsoluteSize(m_fontDescription.isAbsoluteSize());
    }
    if (isSet(PropertySetFlag::SizeAdjust))
        description.setSizeAdjust(m_fontDescription.sizeAdjust());
    if (isSet(PropertySetFlag::Weight))
        description.setWeight(m_fontDescription.weight());
    if (isSet(PropertySetFlag::Stretch))
        description.setStretch(m_fontDescription.stretch());
    if (isSet(PropertySetFlag::FeatureSettings))
        description.setFeatureSettings(m_fontDescription.featureSettings());
    if (isSet(PropertySetFlag::Script)) {
        description.setLocale(m_fontDescription.locale());
        description.setScript(m_fontDescription.script());
    }
    if (isSet(PropertySetFlag::Style))
        description.setStyle(m_fontDescription.style());
    if (isSet(PropertySetFlag::Variant))
        description.setVariant(m_fontDescription.variant());
    if (isSet(PropertySetFlag::VariantLigatures))
        description.setVariantLigatures(m_fontDescription.variantLigatures());
    if (isSet(PropertySetFlag::TextRendering))
        description.setTextRendering(m_fontDescription.textRendering());
    if (isSet(PropertySetFlag::Kerning))
        description.setKerning(m_fontDescription.kerning());
    if (isSet(PropertySetFlag::FontSmoothing))
        description.setFontSmoothing(m_fontDescription.fontSmoothing());
    if (isSet(PropertySetFlag::TextOrientation) || isSet(PropertySetFlag::WritingMode))
        updateOrientation(description, style);

    updateSpecifiedSize(description, style);
    updateComputedSize(description, style);
    updateAdjustedSize(description, style, fontSelector.get());

    style.setFontDescription(description);
    style.font().update(fontSelector);
    m_flags = 0;
}

void FontBuilder::createFontForDocument(PassRefPtrWillBeRawPtr<FontSelector> fontSelector, ComputedStyle& documentStyle)
{
    FontDescription fontDescription = FontDescription();
    fontDescription.setLocale(documentStyle.locale());
    fontDescription.setScript(localeToScriptCodeForFontSelection(documentStyle.locale()));

    setFamilyDescription(fontDescription, FontBuilder::initialFamilyDescription());
    setSize(fontDescription, FontDescription::Size(FontSize::initialKeywordSize(), 0.0f, false));
    updateSpecifiedSize(fontDescription, documentStyle);
    updateComputedSize(fontDescription, documentStyle);

    updateOrientation(fontDescription, documentStyle);
    documentStyle.setFontDescription(fontDescription);
    documentStyle.font().update(fontSelector);
}

}

// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/resolver/FontBuilder.h"

#include "core/css/CSSFontSelector.h"
#include "core/dom/Document.h"
#include "core/dom/StyleEngine.h"
#include "core/frame/Settings.h"
#include "core/style/ComputedStyle.h"
#include "core/testing/DummyPageHolder.h"
#include "platform/text/LocaleToScriptMapping.h"

#include <gtest/gtest.h>

namespace blink {

class FontBuilderTest {
public:
    FontBuilderTest()
        : m_dummy(DummyPageHolder::create(IntSize(800, 600)))
    {
        settings().setDefaultFontSize(16.0f);
    }

    Document& document() { return m_dummy->document(); }
    Settings& settings() { return *document().settings(); }

private:
    OwnPtr<DummyPageHolder> m_dummy;
};

typedef void (*BuilderFunc)(FontBuilder&);
typedef void (*DescriptionFunc)(FontDescription&);

struct FunctionPair {
    FunctionPair(DescriptionFunc base, BuilderFunc value)
        : setBaseValue(base)
        , setValue(value)
    {
    }

    DescriptionFunc setBaseValue;
    BuilderFunc setValue;
};

class FontBuilderInitTest : public FontBuilderTest, public ::testing::Test { };
class FontBuilderAdditiveTest : public FontBuilderTest, public ::testing::TestWithParam<FunctionPair> { };

TEST_F(FontBuilderInitTest, InitialFontSizeNotScaled)
{
    RefPtr<ComputedStyle> initial = ComputedStyle::create();

    FontBuilder builder(document());
    builder.setInitial(1.0f); // FIXME: Remove unused param.
    builder.createFont(document().styleEngine().fontSelector(), *initial);

    EXPECT_EQ(16.0f, initial->fontDescription().computedSize());
}

TEST_F(FontBuilderInitTest, NotDirty)
{
    FontBuilder builder(document());
    ASSERT_FALSE(builder.fontDirty());
}

// This test verifies that when you are setting some field F via FontBuilder,
// only F is actually modified on the incoming ComputedStyle::fontDescription.
TEST_P(FontBuilderAdditiveTest, OnlySetValueIsModified)
{
    FunctionPair funcs = GetParam();

    FontDescription parentDescription;
    funcs.setBaseValue(parentDescription);

    RefPtr<ComputedStyle> style = ComputedStyle::create();
    style->setFontDescription(parentDescription);

    FontBuilder fontBuilder(document());
    funcs.setValue(fontBuilder);
    fontBuilder.createFont(document().styleEngine().fontSelector(), *style);

    FontDescription outputDescription = style->fontDescription();

    // FontBuilder should have overwritten our base value set in the parent,
    // hence the descriptions should not be equal.
    ASSERT_NE(parentDescription, outputDescription);

    // Overwrite the value set by FontBuilder with the base value, directly
    // on outputDescription.
    funcs.setBaseValue(outputDescription);

    // Now the descriptions should be equal again. If they are, we know that
    // FontBuilder did not change something it wasn't supposed to.
    ASSERT_EQ(parentDescription, outputDescription);
}

static void fontWeightBase(FontDescription& d) { d.setWeight(FontWeight900); }
static void fontWeightValue(FontBuilder& b) { b.setWeight(FontWeightNormal); }

static void fontStretchBase(FontDescription& d) { d.setStretch(FontStretchUltraExpanded); }
static void fontStretchValue(FontBuilder& b) { b.setStretch(FontStretchExtraCondensed); }

static void fontFamilyBase(FontDescription& d) { d.setGenericFamily(FontDescription::FantasyFamily); }
static void fontFamilyValue(FontBuilder& b) { b.setFamilyDescription(FontDescription::FamilyDescription(FontDescription::CursiveFamily)); }

static void fontFeatureSettingsBase(FontDescription& d) { d.setFeatureSettings(nullptr); }
static void fontFeatureSettingsValue(FontBuilder& b) { b.setFeatureSettings(FontFeatureSettings::create()); }

static void fontStyleBase(FontDescription& d) { d.setStyle(FontStyleItalic); }
static void fontStyleValue(FontBuilder& b) { b.setStyle(FontStyleNormal); }

static void fontVariantBase(FontDescription& d) { d.setVariant(FontVariantSmallCaps); }
static void fontVariantValue(FontBuilder& b) { b.setVariant(FontVariantNormal); }

static void fontVariantLigaturesBase(FontDescription& d) { d.setVariantLigatures(FontDescription::VariantLigatures(FontDescription::EnabledLigaturesState)); }
static void fontVariantLigaturesValue(FontBuilder& b) { b.setVariantLigatures(FontDescription::VariantLigatures(FontDescription::DisabledLigaturesState)); }

static void fontTextRenderingBase(FontDescription& d) { d.setTextRendering(GeometricPrecision); }
static void fontTextRenderingValue(FontBuilder& b) { b.setTextRendering(OptimizeLegibility); }

static void fontKerningBase(FontDescription& d) { d.setKerning(FontDescription::NormalKerning); }
static void fontKerningValue(FontBuilder& b) { b.setKerning(FontDescription::NoneKerning); }

static void fontFontSmoothingBase(FontDescription& d) { d.setFontSmoothing(Antialiased); }
static void fontFontSmoothingValue(FontBuilder& b) { b.setFontSmoothing(SubpixelAntialiased); }

static void fontSizeBase(FontDescription& d)
{
    d.setSpecifiedSize(37.0f);
    d.setComputedSize(37.0f);
    d.setIsAbsoluteSize(true);
    d.setKeywordSize(7);
}
static void fontSizeValue(FontBuilder& b)
{
    b.setSize(FontDescription::Size(20.0f, 0, false));
}

static void fontScriptBase(FontDescription& d)
{
    d.setLocale("no");
    d.setScript(localeToScriptCodeForFontSelection("no"));
}
static void fontScriptValue(FontBuilder& b)
{
    b.setScript("se");
}

INSTANTIATE_TEST_CASE_P(AllFields, FontBuilderAdditiveTest,
    ::testing::Values(FunctionPair(fontWeightBase, fontWeightValue),
    FunctionPair(fontStretchBase, fontStretchValue),
    FunctionPair(fontFamilyBase, fontFamilyValue),
    FunctionPair(fontFeatureSettingsBase, fontFeatureSettingsValue),
    FunctionPair(fontStyleBase, fontStyleValue),
    FunctionPair(fontVariantBase, fontVariantValue),
    FunctionPair(fontVariantLigaturesBase, fontVariantLigaturesValue),
    FunctionPair(fontTextRenderingBase, fontTextRenderingValue),
    FunctionPair(fontKerningBase, fontKerningValue),
    FunctionPair(fontFontSmoothingBase, fontFontSmoothingValue),
    FunctionPair(fontSizeBase, fontSizeValue),
    FunctionPair(fontScriptBase, fontScriptValue)));

} // namespace blink

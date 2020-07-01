/*
 * Copyright (C) 2007 Apple Inc.
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 * Copyright (C) 2008 Collabora Ltd.
 * Copyright (C) 2008, 2009 Google Inc.
 * Copyright (C) 2009 Kenneth Rohde Christiansen
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
#include "core/layout/LayoutThemeDefault.h"

#include "core/CSSValueKeywords.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutProgress.h"
#include "core/layout/LayoutThemeFontProvider.h"
#include "core/paint/MediaControlsPainter.h"
#include "platform/LayoutTestSupport.h"
#include "platform/PlatformResourceLoader.h"
#include "platform/graphics/Color.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "public/platform/Platform.h"
#include "public/platform/WebThemeEngine.h"

namespace blink {

enum PaddingType {
    TopPadding,
    RightPadding,
    BottomPadding,
    LeftPadding
};

static const int styledMenuListInternalPadding[4] = { 1, 4, 1, 4 };

// These values all match Safari/Win.
static const float defaultControlFontPixelSize = 13;
static const float defaultCancelButtonSize = 9;
static const float minCancelButtonSize = 5;
static const float maxCancelButtonSize = 21;
static const float defaultSearchFieldResultsDecorationSize = 13;
static const float minSearchFieldResultsDecorationSize = 9;
static const float maxSearchFieldResultsDecorationSize = 30;

static bool useMockTheme()
{
    return LayoutTestSupport::isRunningLayoutTest();
}

unsigned LayoutThemeDefault::m_activeSelectionBackgroundColor = 0xff1e90ff;
unsigned LayoutThemeDefault::m_activeSelectionForegroundColor = Color::white; // Color::black;
unsigned LayoutThemeDefault::m_inactiveSelectionBackgroundColor = 0xffc8c8c8;
unsigned LayoutThemeDefault::m_inactiveSelectionForegroundColor = 0xff323232;

double LayoutThemeDefault::m_caretBlinkInterval;

LayoutThemeDefault::LayoutThemeDefault()
{
    m_caretBlinkInterval = LayoutTheme::caretBlinkInterval();
}

LayoutThemeDefault::~LayoutThemeDefault()
{
}

bool LayoutThemeDefault::supportsFocusRing(const ComputedStyle& style) const
{
    if (useMockTheme()) {
        // Don't use focus rings for buttons when mocking controls.
        return style.appearance() == ButtonPart
            || style.appearance() == PushButtonPart
            || style.appearance() == SquareButtonPart;
    }

    // This causes Blink to draw the focus rings for us.
    return false;
}

Color LayoutThemeDefault::systemColor(CSSValueID cssValueId) const
{
    static const Color defaultButtonGrayColor(0xffdddddd);
    static const Color defaultMenuColor(0xfff7f7f7);

    if (cssValueId == CSSValueButtonface) {
        if (useMockTheme())
            return Color(0xc0, 0xc0, 0xc0);
        return defaultButtonGrayColor;
    }
    if (cssValueId == CSSValueMenu)
        return defaultMenuColor;
    return LayoutTheme::systemColor(cssValueId);
}

// Use the Windows style sheets to match their metrics.
String LayoutThemeDefault::extraDefaultStyleSheet()
{
    return LayoutTheme::extraDefaultStyleSheet()
        + loadResourceAsASCIIString("themeWin.css")
        + loadResourceAsASCIIString("themeChromiumSkia.css")
#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
        + loadResourceAsASCIIString("themeInputMultipleFields.css")
#endif
        + loadResourceAsASCIIString("themeChromium.css");
}

String LayoutThemeDefault::extraQuirksStyleSheet()
{
    return loadResourceAsASCIIString("themeWinQuirks.css");
}

Color LayoutThemeDefault::activeListBoxSelectionBackgroundColor() const
{
    return Color(0x28, 0x28, 0x28);
}

Color LayoutThemeDefault::activeListBoxSelectionForegroundColor() const
{
    return Color::black;
}

Color LayoutThemeDefault::inactiveListBoxSelectionBackgroundColor() const
{
    return Color(0xc8, 0xc8, 0xc8);
}

Color LayoutThemeDefault::inactiveListBoxSelectionForegroundColor() const
{
    return Color(0x32, 0x32, 0x32);
}

Color LayoutThemeDefault::platformActiveSelectionBackgroundColor() const
{
    if (useMockTheme())
        return Color(0x00, 0x00, 0xff); // Royal blue.
    return m_activeSelectionBackgroundColor;
}

Color LayoutThemeDefault::platformInactiveSelectionBackgroundColor() const
{
    if (useMockTheme())
        return Color(0x99, 0x99, 0x99); // Medium gray.
    return m_inactiveSelectionBackgroundColor;
}

Color LayoutThemeDefault::platformActiveSelectionForegroundColor() const
{
    if (useMockTheme())
        return Color(0xff, 0xff, 0xcc); // Pale yellow.
    return m_activeSelectionForegroundColor;
}

Color LayoutThemeDefault::platformInactiveSelectionForegroundColor() const
{
    if (useMockTheme())
        return Color::white;
    return m_inactiveSelectionForegroundColor;
}

IntSize LayoutThemeDefault::sliderTickSize() const
{
    if (useMockTheme())
        return IntSize(1, 3);
    return IntSize(1, 6);
}

int LayoutThemeDefault::sliderTickOffsetFromTrackCenter() const
{
    if (useMockTheme())
        return 11;
    return -16;
}

void LayoutThemeDefault::adjustSliderThumbSize(ComputedStyle& style, Element* element) const
{
    IntSize size = Platform::current()->themeEngine()->getSize(WebThemeEngine::PartSliderThumb);

    // FIXME: Mock theme doesn't handle zoomed sliders.
    float zoomLevel = useMockTheme() ? 1 : style.effectiveZoom();
    if (style.appearance() == SliderThumbHorizontalPart) {
        style.setWidth(Length(size.width() * zoomLevel, Fixed));
        style.setHeight(Length(size.height() * zoomLevel, Fixed));
    } else if (style.appearance() == SliderThumbVerticalPart) {
        style.setWidth(Length(size.height() * zoomLevel, Fixed));
        style.setHeight(Length(size.width() * zoomLevel, Fixed));
    } else {
        MediaControlsPainter::adjustMediaSliderThumbSize(style);
    }
}

void LayoutThemeDefault::setCaretBlinkInterval(double interval)
{
    m_caretBlinkInterval = interval;
}

void LayoutThemeDefault::setSelectionColors(
    unsigned activeBackgroundColor,
    unsigned activeForegroundColor,
    unsigned inactiveBackgroundColor,
    unsigned inactiveForegroundColor)
{
    m_activeSelectionBackgroundColor = activeBackgroundColor;
    m_activeSelectionForegroundColor = activeForegroundColor;
    m_inactiveSelectionBackgroundColor = inactiveBackgroundColor;
    m_inactiveSelectionForegroundColor = inactiveForegroundColor;
}

void LayoutThemeDefault::setCheckboxSize(ComputedStyle& style) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style.width().isIntrinsicOrAuto() && !style.height().isAuto())
        return;

    IntSize size = Platform::current()->themeEngine()->getSize(WebThemeEngine::PartCheckbox);
    float zoomLevel = style.effectiveZoom();
    size.setWidth(size.width() * zoomLevel);
    size.setHeight(size.height() * zoomLevel);
    setSizeIfAuto(style, size);
}

void LayoutThemeDefault::setRadioSize(ComputedStyle& style) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style.width().isIntrinsicOrAuto() && !style.height().isAuto())
        return;

    IntSize size = Platform::current()->themeEngine()->getSize(WebThemeEngine::PartRadio);
    float zoomLevel = style.effectiveZoom();
    size.setWidth(size.width() * zoomLevel);
    size.setHeight(size.height() * zoomLevel);
    setSizeIfAuto(style, size);
}

void LayoutThemeDefault::adjustInnerSpinButtonStyle(ComputedStyle& style, Element*) const
{
    IntSize size = Platform::current()->themeEngine()->getSize(WebThemeEngine::PartInnerSpinButton);

    style.setWidth(Length(size.width(), Fixed));
    style.setMinWidth(Length(size.width(), Fixed));
}

bool LayoutThemeDefault::shouldOpenPickerWithF4Key() const
{
    return true;
}

bool LayoutThemeDefault::shouldUseFallbackTheme(const ComputedStyle& style) const
{
    if (useMockTheme()) {
        // The mock theme can't handle zoomed controls, so we fall back to the "fallback" theme.
        ControlPart part = style.appearance();
        if (part == CheckboxPart || part == RadioPart)
            return style.effectiveZoom() != 1;
    }
    return LayoutTheme::shouldUseFallbackTheme(style);
}

bool LayoutThemeDefault::supportsHover(const ComputedStyle& style) const
{
    return true;
}

Color LayoutThemeDefault::platformFocusRingColor() const
{
    static Color focusRingColor(229, 151, 0, 255);
    return focusRingColor;
}

double LayoutThemeDefault::caretBlinkInterval() const
{
    // Disable the blinking caret in layout test mode, as it introduces
    // a race condition for the pixel tests. http://b/1198440
    if (LayoutTestSupport::isRunningLayoutTest())
        return 0;

    return m_caretBlinkInterval;
}

void LayoutThemeDefault::systemFont(CSSValueID systemFontID, FontStyle& fontStyle, FontWeight& fontWeight, float& fontSize, AtomicString& fontFamily) const
{
    LayoutThemeFontProvider::systemFont(systemFontID, fontStyle, fontWeight, fontSize, fontFamily);
}

int LayoutThemeDefault::minimumMenuListSize(const ComputedStyle& style) const
{
    return 0;
}

// Return a rectangle that has the same center point as |original|, but with a
// size capped at |width| by |height|.
IntRect center(const IntRect& original, int width, int height)
{
    width = std::min(original.width(), width);
    height = std::min(original.height(), height);
    int x = original.x() + (original.width() - width) / 2;
    int y = original.y() + (original.height() - height) / 2;

    return IntRect(x, y, width, height);
}

void LayoutThemeDefault::adjustButtonStyle(ComputedStyle& style, Element*) const
{
    if (style.appearance() == PushButtonPart) {
        // Ignore line-height.
        style.setLineHeight(ComputedStyle::initialLineHeight());
    }
}

void LayoutThemeDefault::adjustSearchFieldStyle(ComputedStyle& style, Element*) const
{
    // Ignore line-height.
    style.setLineHeight(ComputedStyle::initialLineHeight());
}

void LayoutThemeDefault::adjustSearchFieldCancelButtonStyle(ComputedStyle& style, Element*) const
{
    // Scale the button size based on the font size
    float fontScale = style.fontSize() / defaultControlFontPixelSize;
    int cancelButtonSize = lroundf(std::min(std::max(minCancelButtonSize, defaultCancelButtonSize * fontScale), maxCancelButtonSize));
    style.setWidth(Length(cancelButtonSize, Fixed));
    style.setHeight(Length(cancelButtonSize, Fixed));
}

void LayoutThemeDefault::adjustSearchFieldDecorationStyle(ComputedStyle& style, Element*) const
{
    IntSize emptySize(1, 11);
    style.setWidth(Length(emptySize.width(), Fixed));
    style.setHeight(Length(emptySize.height(), Fixed));
}

void LayoutThemeDefault::adjustSearchFieldResultsDecorationStyle(ComputedStyle& style, Element*) const
{
    // Scale the decoration size based on the font size
    float fontScale = style.fontSize() / defaultControlFontPixelSize;
    int magnifierSize = lroundf(std::min(std::max(minSearchFieldResultsDecorationSize, defaultSearchFieldResultsDecorationSize * fontScale),
        maxSearchFieldResultsDecorationSize));
    style.setWidth(Length(magnifierSize, Fixed));
    style.setHeight(Length(magnifierSize, Fixed));
}

void LayoutThemeDefault::adjustMenuListStyle(ComputedStyle& style, Element*) const
{
    // Height is locked to auto on all browsers.
    style.setLineHeight(ComputedStyle::initialLineHeight());
}

void LayoutThemeDefault::adjustMenuListButtonStyle(ComputedStyle& style, Element* e) const
{
    adjustMenuListStyle(style, e);
}

int LayoutThemeDefault::popupInternalPaddingLeft(const ComputedStyle& style) const
{
    return menuListInternalPadding(style, LeftPadding);
}

int LayoutThemeDefault::popupInternalPaddingRight(const ComputedStyle& style) const
{
    return menuListInternalPadding(style, RightPadding);
}

int LayoutThemeDefault::popupInternalPaddingTop(const ComputedStyle& style) const
{
    return menuListInternalPadding(style, TopPadding);
}

int LayoutThemeDefault::popupInternalPaddingBottom(const ComputedStyle& style) const
{
    return menuListInternalPadding(style, BottomPadding);
}

// static
void LayoutThemeDefault::setDefaultFontSize(int fontSize)
{
    LayoutThemeFontProvider::setDefaultFontSize(fontSize);
}

int LayoutThemeDefault::menuListArrowPadding() const
{
    return ScrollbarTheme::theme()->scrollbarThickness();
}

int LayoutThemeDefault::menuListInternalPadding(const ComputedStyle& style, int paddingType) const
{
    // This internal padding is in addition to the user-supplied padding.
    // Matches the FF behavior.
    int padding = styledMenuListInternalPadding[paddingType];

    // Reserve the space for right arrow here. The rest of the padding is
    // set by adjustMenuListStyle, since PopMenuWin.cpp uses the padding from
    // LayoutMenuList to lay out the individual items in the popup.
    // If the MenuList actually has appearance "NoAppearance", then that means
    // we don't draw a button, so don't reserve space for it.
    const int barType = style.direction() == LTR ? RightPadding : LeftPadding;
    if (paddingType == barType && style.appearance() != NoControlPart)
        padding += menuListArrowPadding();

    return padding;
}

bool LayoutThemeDefault::shouldShowPlaceholderWhenFocused() const
{
    return true;
}

//
// Following values are come from default of GTK+
//
static const int progressAnimationFrames = 10;
static const double progressAnimationInterval = 0.125;

double LayoutThemeDefault::animationRepeatIntervalForProgressBar() const
{
    return progressAnimationInterval;
}

double LayoutThemeDefault::animationDurationForProgressBar() const
{
    return progressAnimationInterval * progressAnimationFrames * 2; // "2" for back and forth
}

} // namespace blink

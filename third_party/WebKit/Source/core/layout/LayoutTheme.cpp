/**
 * This file is part of the theme implementation for form controls in WebCore.
 *
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Computer, Inc.
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
 */

#include "config.h"
#include "core/layout/LayoutTheme.h"

#include "core/CSSValueKeywords.h"
#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/dom/Document.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/editing/FrameSelection.h"
#include "core/fileapi/FileList.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLCollection.h"
#include "core/html/HTMLDataListElement.h"
#include "core/html/HTMLDataListOptionsCollection.h"
#include "core/html/HTMLFormControlElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLMeterElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/html/shadow/MediaControlElements.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/html/shadow/SpinButtonElement.h"
#include "core/html/shadow/TextControlInnerElements.h"
#include "core/page/FocusController.h"
#include "core/page/Page.h"
#include "core/style/AuthorStyleInfo.h"
#include "core/style/ComputedStyle.h"
#include "platform/FileMetadata.h"
#include "platform/FloatConversion.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/fonts/FontSelector.h"
#include "platform/text/PlatformLocale.h"
#include "platform/text/StringTruncator.h"
#include "public/platform/Platform.h"
#include "public/platform/WebFallbackThemeEngine.h"
#include "public/platform/WebRect.h"
#include "wtf/text/StringBuilder.h"

#if USE(NEW_THEME)
#include "platform/Theme.h"
#endif

// The methods in this file are shared by all themes on every platform.

namespace blink {

using namespace HTMLNames;

LayoutTheme::LayoutTheme()
    : m_hasCustomFocusRingColor(false)
#if USE(NEW_THEME)
    , m_platformTheme(platformTheme())
#endif
{
}

void LayoutTheme::adjustStyle(ComputedStyle& style, Element* e, const AuthorStyleInfo& authorStyle)
{
    ASSERT(style.hasAppearance());

    // Force inline and table display styles to be inline-block (except for table- which is block)
    ControlPart part = style.appearance();
    if (style.display() == INLINE || style.display() == INLINE_TABLE || style.display() == TABLE_ROW_GROUP
        || style.display() == TABLE_HEADER_GROUP || style.display() == TABLE_FOOTER_GROUP
        || style.display() == TABLE_ROW || style.display() == TABLE_COLUMN_GROUP || style.display() == TABLE_COLUMN
        || style.display() == TABLE_CELL || style.display() == TABLE_CAPTION)
        style.setDisplay(INLINE_BLOCK);
    else if (style.display() == LIST_ITEM || style.display() == TABLE)
        style.setDisplay(BLOCK);

    if (isControlStyled(style, authorStyle)) {
        if (part == MenulistPart) {
            style.setAppearance(MenulistButtonPart);
            part = MenulistButtonPart;
        } else {
            style.setAppearance(NoControlPart);
        }
    }

    if (!style.hasAppearance())
        return;

    if (shouldUseFallbackTheme(style)) {
        adjustStyleUsingFallbackTheme(style, e);
        return;
    }

#if USE(NEW_THEME)
    switch (part) {
    case CheckboxPart:
    case InnerSpinButtonPart:
    case RadioPart:
    case PushButtonPart:
    case SquareButtonPart:
    case ButtonPart: {
        // Border
        LengthBox borderBox(style.borderTopWidth(), style.borderRightWidth(), style.borderBottomWidth(), style.borderLeftWidth());
        borderBox = m_platformTheme->controlBorder(part, style.font().fontDescription(), borderBox, style.effectiveZoom());
        if (borderBox.top().value() != static_cast<int>(style.borderTopWidth())) {
            if (borderBox.top().value())
                style.setBorderTopWidth(borderBox.top().value());
            else
                style.resetBorderTop();
        }
        if (borderBox.right().value() != static_cast<int>(style.borderRightWidth())) {
            if (borderBox.right().value())
                style.setBorderRightWidth(borderBox.right().value());
            else
                style.resetBorderRight();
        }
        if (borderBox.bottom().value() != static_cast<int>(style.borderBottomWidth())) {
            style.setBorderBottomWidth(borderBox.bottom().value());
            if (borderBox.bottom().value())
                style.setBorderBottomWidth(borderBox.bottom().value());
            else
                style.resetBorderBottom();
        }
        if (borderBox.left().value() != static_cast<int>(style.borderLeftWidth())) {
            style.setBorderLeftWidth(borderBox.left().value());
            if (borderBox.left().value())
                style.setBorderLeftWidth(borderBox.left().value());
            else
                style.resetBorderLeft();
        }

        // Padding
        LengthBox paddingBox = m_platformTheme->controlPadding(part, style.font().fontDescription(), style.paddingBox(), style.effectiveZoom());
        if (paddingBox != style.paddingBox())
            style.setPaddingBox(paddingBox);

        // Whitespace
        if (m_platformTheme->controlRequiresPreWhiteSpace(part))
            style.setWhiteSpace(PRE);

        // Width / Height
        // The width and height here are affected by the zoom.
        // FIXME: Check is flawed, since it doesn't take min-width/max-width into account.
        LengthSize controlSize = m_platformTheme->controlSize(part, style.font().fontDescription(), LengthSize(style.width(), style.height()), style.effectiveZoom());
        if (controlSize.width() != style.width())
            style.setWidth(controlSize.width());
        if (controlSize.height() != style.height())
            style.setHeight(controlSize.height());

        // Min-Width / Min-Height
        LengthSize minControlSize = m_platformTheme->minimumControlSize(part, style.font().fontDescription(), style.effectiveZoom());
        if (minControlSize.width() != style.minWidth())
            style.setMinWidth(minControlSize.width());
        if (minControlSize.height() != style.minHeight())
            style.setMinHeight(minControlSize.height());

        // Font
        FontDescription controlFont = m_platformTheme->controlFont(part, style.font().fontDescription(), style.effectiveZoom());
        if (controlFont != style.font().fontDescription()) {
            // Reset our line-height
            style.setLineHeight(ComputedStyle::initialLineHeight());

            // Now update our font.
            if (style.setFontDescription(controlFont))
                style.font().update(nullptr);
        }
    }
    default:
        break;
    }
#endif

    // Call the appropriate style adjustment method based off the appearance value.
    switch (style.appearance()) {
#if !USE(NEW_THEME)
    case CheckboxPart:
        return adjustCheckboxStyle(style, e);
    case RadioPart:
        return adjustRadioStyle(style, e);
    case PushButtonPart:
    case SquareButtonPart:
    case ButtonPart:
        return adjustButtonStyle(style, e);
    case InnerSpinButtonPart:
        return adjustInnerSpinButtonStyle(style, e);
#endif
    case MenulistPart:
        return adjustMenuListStyle(style, e);
    case MenulistButtonPart:
        return adjustMenuListButtonStyle(style, e);
    case SliderThumbHorizontalPart:
    case SliderThumbVerticalPart:
        return adjustSliderThumbStyle(style, e);
    case SearchFieldPart:
        return adjustSearchFieldStyle(style, e);
    case SearchFieldCancelButtonPart:
        return adjustSearchFieldCancelButtonStyle(style, e);
    case SearchFieldDecorationPart:
        return adjustSearchFieldDecorationStyle(style, e);
    case SearchFieldResultsDecorationPart:
        return adjustSearchFieldResultsDecorationStyle(style, e);
    default:
        break;
    }
}

String LayoutTheme::extraDefaultStyleSheet()
{
    StringBuilder runtimeCSS;
    if (RuntimeEnabledFeatures::contextMenuEnabled())
        runtimeCSS.appendLiteral("menu[type=\"popup\" i] { display: none; }");
    return runtimeCSS.toString();
}

static String formatChromiumMediaControlsTime(float time, float duration)
{
    if (!std::isfinite(time))
        time = 0;
    if (!std::isfinite(duration))
        duration = 0;
    int seconds = static_cast<int>(fabsf(time));
    int hours = seconds / (60 * 60);
    int minutes = (seconds / 60) % 60;
    seconds %= 60;

    // duration defines the format of how the time is rendered
    int durationSecs = static_cast<int>(fabsf(duration));
    int durationHours = durationSecs / (60 * 60);
    int durationMins = (durationSecs / 60) % 60;

    if (durationHours || hours)
        return String::format("%s%01d:%02d:%02d", (time < 0 ? "-" : ""), hours, minutes, seconds);
    if (durationMins > 9)
        return String::format("%s%02d:%02d", (time < 0 ? "-" : ""), minutes, seconds);

    return String::format("%s%01d:%02d", (time < 0 ? "-" : ""), minutes, seconds);
}

String LayoutTheme::formatMediaControlsTime(float time) const
{
    return formatChromiumMediaControlsTime(time, time);
}

String LayoutTheme::formatMediaControlsCurrentTime(float currentTime, float duration) const
{
    return formatChromiumMediaControlsTime(currentTime, duration);
}

Color LayoutTheme::activeSelectionBackgroundColor() const
{
    return platformActiveSelectionBackgroundColor().blendWithWhite();
}

Color LayoutTheme::inactiveSelectionBackgroundColor() const
{
    return platformInactiveSelectionBackgroundColor().blendWithWhite();
}

Color LayoutTheme::activeSelectionForegroundColor() const
{
    return platformActiveSelectionForegroundColor();
}

Color LayoutTheme::inactiveSelectionForegroundColor() const
{
    return platformInactiveSelectionForegroundColor();
}

Color LayoutTheme::activeListBoxSelectionBackgroundColor() const
{
    return platformActiveListBoxSelectionBackgroundColor();
}

Color LayoutTheme::inactiveListBoxSelectionBackgroundColor() const
{
    return platformInactiveListBoxSelectionBackgroundColor();
}

Color LayoutTheme::activeListBoxSelectionForegroundColor() const
{
    return platformActiveListBoxSelectionForegroundColor();
}

Color LayoutTheme::inactiveListBoxSelectionForegroundColor() const
{
    return platformInactiveListBoxSelectionForegroundColor();
}

Color LayoutTheme::platformActiveSelectionBackgroundColor() const
{
    // Use a blue color by default if the platform theme doesn't define anything.
    return Color(0, 0, 255);
}

Color LayoutTheme::platformActiveSelectionForegroundColor() const
{
    // Use a white color by default if the platform theme doesn't define anything.
    return Color::white;
}

Color LayoutTheme::platformInactiveSelectionBackgroundColor() const
{
    // Use a grey color by default if the platform theme doesn't define anything.
    // This color matches Firefox's inactive color.
    return Color(176, 176, 176);
}

Color LayoutTheme::platformInactiveSelectionForegroundColor() const
{
    // Use a black color by default.
    return Color::black;
}

Color LayoutTheme::platformActiveListBoxSelectionBackgroundColor() const
{
    return platformActiveSelectionBackgroundColor();
}

Color LayoutTheme::platformActiveListBoxSelectionForegroundColor() const
{
    return platformActiveSelectionForegroundColor();
}

Color LayoutTheme::platformInactiveListBoxSelectionBackgroundColor() const
{
    return platformInactiveSelectionBackgroundColor();
}

Color LayoutTheme::platformInactiveListBoxSelectionForegroundColor() const
{
    return platformInactiveSelectionForegroundColor();
}

int LayoutTheme::baselinePosition(const LayoutObject* o) const
{
    if (!o->isBox())
        return 0;

    const LayoutBox* box = toLayoutBox(o);

#if USE(NEW_THEME)
    return box->size().height() + box->marginTop() + m_platformTheme->baselinePositionAdjustment(o->style()->appearance()) * o->style()->effectiveZoom();
#else
    return box->size().height() + box->marginTop();
#endif
}

bool LayoutTheme::isControlContainer(ControlPart appearance) const
{
    // There are more leaves than this, but we'll patch this function as we add support for
    // more controls.
    return appearance != CheckboxPart && appearance != RadioPart;
}

bool LayoutTheme::isControlStyled(const ComputedStyle& style, const AuthorStyleInfo& authorStyle) const
{
    switch (style.appearance()) {
    case PushButtonPart:
    case SquareButtonPart:
    case ButtonPart:
    case ProgressBarPart:
    case MeterPart:
    case RelevancyLevelIndicatorPart:
    case ContinuousCapacityLevelIndicatorPart:
    case DiscreteCapacityLevelIndicatorPart:
    case RatingLevelIndicatorPart:
        return authorStyle.specifiesBackground() || authorStyle.specifiesBorder();

    case MenulistPart:
    case SearchFieldPart:
    case TextAreaPart:
    case TextFieldPart:
        return authorStyle.specifiesBackground() || authorStyle.specifiesBorder() || style.boxShadow();

    case SliderHorizontalPart:
    case SliderVerticalPart:
        return style.boxShadow();

    default:
        return false;
    }
}

void LayoutTheme::addVisualOverflow(const LayoutObject& object, IntRect& borderBox)
{
#if USE(NEW_THEME)
    m_platformTheme->addVisualOverflow(object.style()->appearance(), controlStatesForLayoutObject(&object), object.style()->effectiveZoom(), borderBox);
#endif
}

static bool isInputNode(LayoutObject* layoutObject)
{
    Node* node = layoutObject->node();
    if (!node)
        return false;
    if (!node->hasTagName(HTMLNames::inputTag))
        return false;
    
    return true;
}

bool LayoutTheme::shouldDrawDefaultFocusRing(LayoutObject* layoutObject) const
{
#if 1 // def MINIBLINK_NOT_IMPLEMENTED // weolar
    if (supportsFocusRing(layoutObject->styleRef()) || !isInputNode(layoutObject))
        return false;
    Node* node = layoutObject->node();
    if (!node)
        return true;
    if (!layoutObject->styleRef().hasAppearance() && !node->isLink())
        return true;
    // We can't use LayoutTheme::isFocused because outline:auto might be
    // specified to non-:focus rulesets.
    if (node->focused() && !node->shouldHaveFocusAppearance())
        return false;
    return true;
#endif
}

bool LayoutTheme::supportsFocusRing(const ComputedStyle& style) const
{
    return (style.hasAppearance() && style.appearance() != TextFieldPart && style.appearance() != TextAreaPart && style.appearance() != MenulistButtonPart && style.appearance() != ListboxPart);
}

bool LayoutTheme::controlStateChanged(LayoutObject& o, ControlState state) const
{
    if (!o.styleRef().hasAppearance())
        return false;

    // Default implementation assumes the controls don't respond to changes in :hover state
    if (state == HoverControlState && !supportsHover(o.styleRef()))
        return false;

    // Assume pressed state is only responded to if the control is enabled.
    if (state == PressedControlState && !isEnabled(&o))
        return false;

    o.setShouldDoFullPaintInvalidation();
    if (RuntimeEnabledFeatures::slimmingPaintEnabled())
        o.invalidateDisplayItemClientForNonCompositingDescendants();
    return true;
}

ControlStates LayoutTheme::controlStatesForLayoutObject(const LayoutObject* o)
{
    ControlStates result = 0;
    if (isHovered(o)) {
        result |= HoverControlState;
        if (isSpinUpButtonPartHovered(o))
            result |= SpinUpControlState;
    }
    if (isPressed(o)) {
        result |= PressedControlState;
        if (isSpinUpButtonPartPressed(o))
            result |= SpinUpControlState;
    }
    if (isFocused(o) && o->style()->outlineStyleIsAuto())
        result |= FocusControlState;
    if (isEnabled(o))
        result |= EnabledControlState;
    if (isChecked(o))
        result |= CheckedControlState;
    if (isReadOnlyControl(o))
        result |= ReadOnlyControlState;
    if (!isActive(o))
        result |= WindowInactiveControlState;
    if (isIndeterminate(o))
        result |= IndeterminateControlState;
    return result;
}

bool LayoutTheme::isActive(const LayoutObject* o)
{
    Node* node = o->node();
    if (!node)
        return false;

    Page* page = node->document().page();
    if (!page)
        return false;

    return page->focusController().isActive();
}

bool LayoutTheme::isChecked(const LayoutObject* o)
{
    if (!isHTMLInputElement(o->node()))
        return false;
    return toHTMLInputElement(o->node())->shouldAppearChecked();
}

bool LayoutTheme::isIndeterminate(const LayoutObject* o)
{
    if (!isHTMLInputElement(o->node()))
        return false;
    return toHTMLInputElement(o->node())->shouldAppearIndeterminate();
}

bool LayoutTheme::isEnabled(const LayoutObject* o)
{
    Node* node = o->node();
    if (!node || !node->isElementNode())
        return true;
    return !toElement(node)->isDisabledFormControl();
}

bool LayoutTheme::isFocused(const LayoutObject* o)
{
    Node* node = o->node();
    if (!node)
        return false;

    node = node->focusDelegate();
    Document& document = node->document();
    LocalFrame* frame = document.frame();
    return node == document.focusedElement() && node->focused() && node->shouldHaveFocusAppearance() && frame && frame->selection().isFocusedAndActive();
}

bool LayoutTheme::isPressed(const LayoutObject* o)
{
    if (!o->node())
        return false;
    return o->node()->active();
}

bool LayoutTheme::isSpinUpButtonPartPressed(const LayoutObject* o)
{
    Node* node = o->node();
    if (!node || !node->active() || !node->isElementNode()
        || !toElement(node)->isSpinButtonElement())
        return false;
    SpinButtonElement* element = toSpinButtonElement(node);
    return element->upDownState() == SpinButtonElement::Up;
}

bool LayoutTheme::isReadOnlyControl(const LayoutObject* o)
{
    Node* node = o->node();
    if (!node || !node->isElementNode() || !toElement(node)->isFormControlElement())
        return false;
    HTMLFormControlElement* element = toHTMLFormControlElement(node);
    return element->isReadOnly();
}

bool LayoutTheme::isHovered(const LayoutObject* o)
{
    Node* node = o->node();
    if (!node)
        return false;
    if (!node->isElementNode() || !toElement(node)->isSpinButtonElement())
        return node->hovered();
    SpinButtonElement* element = toSpinButtonElement(node);
    return element->hovered() && element->upDownState() != SpinButtonElement::Indeterminate;
}

bool LayoutTheme::isSpinUpButtonPartHovered(const LayoutObject* o)
{
    Node* node = o->node();
    if (!node || !node->isElementNode() || !toElement(node)->isSpinButtonElement())
        return false;
    SpinButtonElement* element = toSpinButtonElement(node);
    return element->upDownState() == SpinButtonElement::Up;
}

#if !USE(NEW_THEME)

void LayoutTheme::adjustCheckboxStyle(ComputedStyle& style, Element*) const
{
    // A summary of the rules for checkbox designed to match WinIE:
    // width/height - honored (WinIE actually scales its control for small widths, but lets it overflow for small heights.)
    // font-size - not honored (control has no text), but we use it to decide which control size to use.
    setCheckboxSize(style);

    // padding - not honored by WinIE, needs to be removed.
    style.resetPadding();

    // border - honored by WinIE, but looks terrible (just paints in the control box and turns off the Windows XP theme)
    // for now, we will not honor it.
    style.resetBorder();
}

void LayoutTheme::adjustRadioStyle(ComputedStyle& style, Element*) const
{
    // A summary of the rules for checkbox designed to match WinIE:
    // width/height - honored (WinIE actually scales its control for small widths, but lets it overflow for small heights.)
    // font-size - not honored (control has no text), but we use it to decide which control size to use.
    setRadioSize(style);

    // padding - not honored by WinIE, needs to be removed.
    style.resetPadding();

    // border - honored by WinIE, but looks terrible (just paints in the control box and turns off the Windows XP theme)
    // for now, we will not honor it.
    style.resetBorder();
}

void LayoutTheme::adjustButtonStyle(ComputedStyle& style, Element*) const
{
}

void LayoutTheme::adjustInnerSpinButtonStyle(ComputedStyle&, Element*) const
{
}
#endif

void LayoutTheme::adjustMenuListStyle(ComputedStyle&, Element*) const
{
}

IntSize LayoutTheme::meterSizeForBounds(const LayoutMeter*, const IntRect& bounds) const
{
    return bounds.size();
}

bool LayoutTheme::supportsMeter(ControlPart) const
{
    return false;
}

double LayoutTheme::animationRepeatIntervalForProgressBar() const
{
    return 0;
}

double LayoutTheme::animationDurationForProgressBar() const
{
    return 0;
}

bool LayoutTheme::shouldHaveSpinButton(HTMLInputElement* inputElement) const
{
    return inputElement->isSteppable() && inputElement->type() != InputTypeNames::range;
}

void LayoutTheme::adjustMenuListButtonStyle(ComputedStyle&, Element*) const
{
}

void LayoutTheme::adjustSliderThumbStyle(ComputedStyle& style, Element* element) const
{
    adjustSliderThumbSize(style, element);
}

void LayoutTheme::adjustSliderThumbSize(ComputedStyle&, Element*) const
{
}

void LayoutTheme::adjustSearchFieldStyle(ComputedStyle&, Element*) const
{
}

void LayoutTheme::adjustSearchFieldCancelButtonStyle(ComputedStyle&, Element*) const
{
}

void LayoutTheme::adjustSearchFieldDecorationStyle(ComputedStyle&, Element*) const
{
}

void LayoutTheme::adjustSearchFieldResultsDecorationStyle(ComputedStyle&, Element*) const
{
}

void LayoutTheme::platformColorsDidChange()
{
    Page::platformColorsChanged();
}

static FontDescription& getCachedFontDescription(CSSValueID systemFontID)
{
    DEFINE_STATIC_LOCAL(FontDescription, caption, ());
    DEFINE_STATIC_LOCAL(FontDescription, icon, ());
    DEFINE_STATIC_LOCAL(FontDescription, menu, ());
    DEFINE_STATIC_LOCAL(FontDescription, messageBox, ());
    DEFINE_STATIC_LOCAL(FontDescription, smallCaption, ());
    DEFINE_STATIC_LOCAL(FontDescription, statusBar, ());
    DEFINE_STATIC_LOCAL(FontDescription, webkitMiniControl, ());
    DEFINE_STATIC_LOCAL(FontDescription, webkitSmallControl, ());
    DEFINE_STATIC_LOCAL(FontDescription, webkitControl, ());
    DEFINE_STATIC_LOCAL(FontDescription, defaultDescription, ());
    switch (systemFontID) {
    case CSSValueCaption:
        return caption;
    case CSSValueIcon:
        return icon;
    case CSSValueMenu:
        return menu;
    case CSSValueMessageBox:
        return messageBox;
    case CSSValueSmallCaption:
        return smallCaption;
    case CSSValueStatusBar:
        return statusBar;
    case CSSValueWebkitMiniControl:
        return webkitMiniControl;
    case CSSValueWebkitSmallControl:
        return webkitSmallControl;
    case CSSValueWebkitControl:
        return webkitControl;
    case CSSValueNone:
        return defaultDescription;
    default:
        ASSERT_NOT_REACHED();
        return defaultDescription;
    }
}

void LayoutTheme::systemFont(CSSValueID systemFontID, FontDescription& fontDescription)
{
    fontDescription = getCachedFontDescription(systemFontID);
    if (fontDescription.isAbsoluteSize())
        return;

    FontStyle fontStyle = FontStyleNormal;
    FontWeight fontWeight = FontWeightNormal;
    float fontSize = 0;
    AtomicString fontFamily;
    systemFont(systemFontID, fontStyle, fontWeight, fontSize, fontFamily);
    fontDescription.setStyle(fontStyle);
    fontDescription.setWeight(fontWeight);
    fontDescription.setSpecifiedSize(fontSize);
    fontDescription.setIsAbsoluteSize(true);
    fontDescription.firstFamily().setFamily(fontFamily);
    fontDescription.setGenericFamily(FontDescription::NoFamily);
}

Color LayoutTheme::systemColor(CSSValueID cssValueId) const
{
    switch (cssValueId) {
    case CSSValueActiveborder:
        return 0xFFFFFFFF;
    case CSSValueActivecaption:
        return 0xFFCCCCCC;
    case CSSValueAppworkspace:
        return 0xFFFFFFFF;
    case CSSValueBackground:
        return 0xFF6363CE;
    case CSSValueButtonface:
        return 0xFFC0C0C0;
    case CSSValueButtonhighlight:
        return 0xFFDDDDDD;
    case CSSValueButtonshadow:
        return 0xFF888888;
    case CSSValueButtontext:
        return 0xFF000000;
    case CSSValueCaptiontext:
        return 0xFF000000;
    case CSSValueGraytext:
        return 0xFF808080;
    case CSSValueHighlight:
        return 0xFFB5D5FF;
    case CSSValueHighlighttext:
        return 0xFF000000;
    case CSSValueInactiveborder:
        return 0xFFFFFFFF;
    case CSSValueInactivecaption:
        return 0xFFFFFFFF;
    case CSSValueInactivecaptiontext:
        return 0xFF7F7F7F;
    case CSSValueInfobackground:
        return 0xFFFBFCC5;
    case CSSValueInfotext:
        return 0xFF000000;
    case CSSValueMenu:
        return 0xFFC0C0C0;
    case CSSValueMenutext:
        return 0xFF000000;
    case CSSValueScrollbar:
        return 0xFFFFFFFF;
    case CSSValueText:
        return 0xFF000000;
    case CSSValueThreeddarkshadow:
        return 0xFF666666;
    case CSSValueThreedface:
        return 0xFFC0C0C0;
    case CSSValueThreedhighlight:
        return 0xFFDDDDDD;
    case CSSValueThreedlightshadow:
        return 0xFFC0C0C0;
    case CSSValueThreedshadow:
        return 0xFF888888;
    case CSSValueWindow:
        return 0xFFFFFFFF;
    case CSSValueWindowframe:
        return 0xFFCCCCCC;
    case CSSValueWindowtext:
        return 0xFF000000;
    case CSSValueInternalActiveListBoxSelection:
        return activeListBoxSelectionBackgroundColor();
        break;
    case CSSValueInternalActiveListBoxSelectionText:
        return activeListBoxSelectionForegroundColor();
        break;
    case CSSValueInternalInactiveListBoxSelection:
        return inactiveListBoxSelectionBackgroundColor();
        break;
    case CSSValueInternalInactiveListBoxSelectionText:
        return inactiveListBoxSelectionForegroundColor();
        break;
    default:
        break;
    }
    ASSERT_NOT_REACHED();
    return Color();
}

Color LayoutTheme::platformActiveTextSearchHighlightColor() const
{
    return Color(255, 150, 50); // Orange.
}

Color LayoutTheme::platformInactiveTextSearchHighlightColor() const
{
    return Color(255, 255, 0); // Yellow.
}

Color LayoutTheme::tapHighlightColor()
{
    return theme().platformTapHighlightColor();
}

void LayoutTheme::setCustomFocusRingColor(const Color& c)
{
    m_customFocusRingColor = c;
    m_hasCustomFocusRingColor = true;
}

Color LayoutTheme::focusRingColor() const
{
    return m_hasCustomFocusRingColor ? m_customFocusRingColor : theme().platformFocusRingColor();
}

String LayoutTheme::fileListNameForWidth(Locale& locale, const FileList* fileList, const Font& font, int width) const
{
    if (width <= 0)
        return String();

    String string;
    if (fileList->isEmpty()) {
        string = locale.queryString(WebLocalizedString::FileButtonNoFileSelectedLabel);
    } else if (fileList->length() == 1) {
        string = fileList->item(0)->name();
    } else {
        // FIXME: Localization of fileList->length().
        return StringTruncator::rightTruncate(locale.queryString(WebLocalizedString::MultipleFileUploadText, String::number(fileList->length())), width, font);
    }

    return StringTruncator::centerTruncate(string, width, font);
}

bool LayoutTheme::shouldOpenPickerWithF4Key() const
{
    return false;
}

#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
bool LayoutTheme::supportsCalendarPicker(const AtomicString& type) const
{
    return type == InputTypeNames::date
        || type == InputTypeNames::datetime
        || type == InputTypeNames::datetime_local
        || type == InputTypeNames::month
        || type == InputTypeNames::week;
}
#endif

bool LayoutTheme::shouldUseFallbackTheme(const ComputedStyle&) const
{
    return false;
}

void LayoutTheme::adjustStyleUsingFallbackTheme(ComputedStyle& style, Element* e)
{
    ControlPart part = style.appearance();
    switch (part) {
    case CheckboxPart:
        return adjustCheckboxStyleUsingFallbackTheme(style, e);
    case RadioPart:
        return adjustRadioStyleUsingFallbackTheme(style, e);
    default:
        break;
    }
}

// static
void LayoutTheme::setSizeIfAuto(ComputedStyle& style, const IntSize& size)
{
    if (style.width().isIntrinsicOrAuto())
        style.setWidth(Length(size.width(), Fixed));
    if (style.height().isAuto())
        style.setHeight(Length(size.height(), Fixed));
}

void LayoutTheme::adjustCheckboxStyleUsingFallbackTheme(ComputedStyle& style, Element*) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style.width().isIntrinsicOrAuto() && !style.height().isAuto())
        return;

    IntSize size = Platform::current()->fallbackThemeEngine()->getSize(WebFallbackThemeEngine::PartCheckbox);
    float zoomLevel = style.effectiveZoom();
    size.setWidth(size.width() * zoomLevel);
    size.setHeight(size.height() * zoomLevel);
    setSizeIfAuto(style, size);

    // padding - not honored by WinIE, needs to be removed.
    style.resetPadding();

    // border - honored by WinIE, but looks terrible (just paints in the control box and turns off the Windows XP theme)
    // for now, we will not honor it.
    style.resetBorder();
}

void LayoutTheme::adjustRadioStyleUsingFallbackTheme(ComputedStyle& style, Element*) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style.width().isIntrinsicOrAuto() && !style.height().isAuto())
        return;

    IntSize size = Platform::current()->fallbackThemeEngine()->getSize(WebFallbackThemeEngine::PartRadio);
    float zoomLevel = style.effectiveZoom();
    size.setWidth(size.width() * zoomLevel);
    size.setHeight(size.height() * zoomLevel);
    setSizeIfAuto(style, size);

    // padding - not honored by WinIE, needs to be removed.
    style.resetPadding();

    // border - honored by WinIE, but looks terrible (just paints in the control box and turns off the Windows XP theme)
    // for now, we will not honor it.
    style.resetBorder();
}

} // namespace blink

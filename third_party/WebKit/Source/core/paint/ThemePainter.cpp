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
#include "core/paint/ThemePainter.h"

#include "core/InputTypeNames.h"
#include "core/html/HTMLDataListElement.h"
#include "core/html/HTMLDataListOptionsCollection.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/layout/LayoutMeter.h"
#include "core/layout/LayoutTheme.h"
#include "core/layout/LayoutView.h"
#include "core/paint/MediaControlsPainter.h"
#include "core/paint/PaintInfo.h"
#include "core/style/ComputedStyle.h"
#include "platform/graphics/GraphicsContextStateSaver.h"
#include "public/platform/Platform.h"
#include "public/platform/WebFallbackThemeEngine.h"
#include "public/platform/WebRect.h"

#if USE(NEW_THEME)
#include "platform/Theme.h"
#endif

// The methods in this file are shared by all themes on every platform.

namespace blink {

static WebFallbackThemeEngine::State getWebFallbackThemeState(const LayoutObject* o)
{
    if (!LayoutTheme::isEnabled(o))
        return WebFallbackThemeEngine::StateDisabled;
    if (LayoutTheme::isPressed(o))
        return WebFallbackThemeEngine::StatePressed;
    if (LayoutTheme::isHovered(o))
        return WebFallbackThemeEngine::StateHover;

    return WebFallbackThemeEngine::StateNormal;
}

bool ThemePainter::paint(LayoutObject* o, const PaintInfo& paintInfo, const IntRect& r)
{
    ControlPart part = o->styleRef().appearance();

    if (LayoutTheme::theme().shouldUseFallbackTheme(o->styleRef()))
        return paintUsingFallbackTheme(o, paintInfo, r);

#if USE(NEW_THEME)
    switch (part) {
    case CheckboxPart:
    case RadioPart:
    case PushButtonPart:
    case SquareButtonPart:
    case ButtonPart:
    case InnerSpinButtonPart:
        platformTheme()->paint(part, LayoutTheme::controlStatesForLayoutObject(o), const_cast<GraphicsContext*>(paintInfo.context), r, o->styleRef().effectiveZoom(), o->view()->frameView());
        return false;
    default:
        break;
    }
#endif

    // Call the appropriate paint method based off the appearance value.
    switch (part) {
#if !USE(NEW_THEME)
    case CheckboxPart:
        return paintCheckbox(o, paintInfo, r);
    case RadioPart:
        return paintRadio(o, paintInfo, r);
    case PushButtonPart:
    case SquareButtonPart:
    case ButtonPart:
        return paintButton(o, paintInfo, r);
    case InnerSpinButtonPart:
        return paintInnerSpinButton(o, paintInfo, r);
#endif
    case MenulistPart:
        return paintMenuList(o, paintInfo, r);
    case MeterPart:
    case RelevancyLevelIndicatorPart:
    case ContinuousCapacityLevelIndicatorPart:
    case DiscreteCapacityLevelIndicatorPart:
    case RatingLevelIndicatorPart:
        return paintMeter(o, paintInfo, r);
    case ProgressBarPart:
        return paintProgressBar(o, paintInfo, r);
    case SliderHorizontalPart:
    case SliderVerticalPart:
        return paintSliderTrack(o, paintInfo, r);
    case SliderThumbHorizontalPart:
    case SliderThumbVerticalPart:
        return paintSliderThumb(o, paintInfo, r);
    case MediaEnterFullscreenButtonPart:
    case MediaExitFullscreenButtonPart:
        return MediaControlsPainter::paintMediaFullscreenButton(o, paintInfo, r);
    case MediaPlayButtonPart:
        return MediaControlsPainter::paintMediaPlayButton(o, paintInfo, r);
    case MediaOverlayPlayButtonPart:
        return MediaControlsPainter::paintMediaOverlayPlayButton(o, paintInfo, r);
    case MediaMuteButtonPart:
        return MediaControlsPainter::paintMediaMuteButton(o, paintInfo, r);
    case MediaToggleClosedCaptionsButtonPart:
        return MediaControlsPainter::paintMediaToggleClosedCaptionsButton(o, paintInfo, r);
    case MediaSliderPart:
        return MediaControlsPainter::paintMediaSlider(o, paintInfo, r);
    case MediaSliderThumbPart:
        return MediaControlsPainter::paintMediaSliderThumb(o, paintInfo, r);
    case MediaVolumeSliderContainerPart:
        return true;
    case MediaVolumeSliderPart:
        return MediaControlsPainter::paintMediaVolumeSlider(o, paintInfo, r);
    case MediaVolumeSliderThumbPart:
        return MediaControlsPainter::paintMediaVolumeSliderThumb(o, paintInfo, r);
    case MediaFullScreenVolumeSliderPart:
    case MediaFullScreenVolumeSliderThumbPart:
    case MediaTimeRemainingPart:
    case MediaCurrentTimePart:
    case MediaControlsBackgroundPart:
        return true;
    case MediaCastOffButtonPart:
    case MediaOverlayCastOffButtonPart:
        return MediaControlsPainter::paintMediaCastButton(o, paintInfo, r);
    case MenulistButtonPart:
    case TextFieldPart:
    case TextAreaPart:
        return true;
    case SearchFieldPart:
        return paintSearchField(o, paintInfo, r);
    case SearchFieldCancelButtonPart:
        return paintSearchFieldCancelButton(o, paintInfo, r);
    case SearchFieldDecorationPart:
        return paintSearchFieldDecoration(o, paintInfo, r);
    case SearchFieldResultsDecorationPart:
        return paintSearchFieldResultsDecoration(o, paintInfo, r);
    default:
        break;
    }

    return true; // We don't support the appearance, so let the normal background/border paint.
}

bool ThemePainter::paintBorderOnly(LayoutObject* o, const PaintInfo& paintInfo, const IntRect& r)
{
    // Call the appropriate paint method based off the appearance value.
    switch (o->style()->appearance()) {
    case TextFieldPart:
        return paintTextField(o, paintInfo, r);
    case TextAreaPart:
        return paintTextArea(o, paintInfo, r);
    case MenulistButtonPart:
    case SearchFieldPart:
    case ListboxPart:
        return true;
    case CheckboxPart:
    case RadioPart:
    case PushButtonPart:
    case SquareButtonPart:
    case ButtonPart:
    case MenulistPart:
    case MeterPart:
    case RelevancyLevelIndicatorPart:
    case ContinuousCapacityLevelIndicatorPart:
    case DiscreteCapacityLevelIndicatorPart:
    case RatingLevelIndicatorPart:
    case ProgressBarPart:
    case SliderHorizontalPart:
    case SliderVerticalPart:
    case SliderThumbHorizontalPart:
    case SliderThumbVerticalPart:
    case SearchFieldCancelButtonPart:
    case SearchFieldDecorationPart:
    case SearchFieldResultsDecorationPart:
    default:
        break;
    }

    return false;
}

bool ThemePainter::paintDecorations(LayoutObject* o, const PaintInfo& paintInfo, const IntRect& r)
{
    // Call the appropriate paint method based off the appearance value.
    switch (o->style()->appearance()) {
    case MenulistButtonPart:
        return paintMenuListButton(o, paintInfo, r);
    case TextFieldPart:
    case TextAreaPart:
    case CheckboxPart:
    case RadioPart:
    case PushButtonPart:
    case SquareButtonPart:
    case ButtonPart:
    case MenulistPart:
    case MeterPart:
    case RelevancyLevelIndicatorPart:
    case ContinuousCapacityLevelIndicatorPart:
    case DiscreteCapacityLevelIndicatorPart:
    case RatingLevelIndicatorPart:
    case ProgressBarPart:
    case SliderHorizontalPart:
    case SliderVerticalPart:
    case SliderThumbHorizontalPart:
    case SliderThumbVerticalPart:
    case SearchFieldPart:
    case SearchFieldCancelButtonPart:
    case SearchFieldDecorationPart:
    case SearchFieldResultsDecorationPart:
    default:
        break;
    }

    return false;
}

bool ThemePainter::paintMeter(LayoutObject*, const PaintInfo&, const IntRect&)
{
    return true;
}

void ThemePainter::paintSliderTicks(LayoutObject* o, const PaintInfo& paintInfo, const IntRect& rect)
{
    Node* node = o->node();
    if (!isHTMLInputElement(node))
        return;

    HTMLInputElement* input = toHTMLInputElement(node);
    if (input->type() != InputTypeNames::range)
        return;

    HTMLDataListElement* dataList = input->dataList();
    if (!dataList)
        return;

    double min = input->minimum();
    double max = input->maximum();
    ControlPart part = o->style()->appearance();
    // We don't support ticks on alternate sliders like MediaVolumeSliders.
    if (part !=  SliderHorizontalPart && part != SliderVerticalPart)
        return;
    bool isHorizontal = part ==  SliderHorizontalPart;

    IntSize thumbSize;
    LayoutObject* thumbLayoutObject = input->userAgentShadowRoot()->getElementById(ShadowElementNames::sliderThumb())->layoutObject();
    if (thumbLayoutObject) {
        const ComputedStyle& thumbStyle = thumbLayoutObject->styleRef();
        int thumbWidth = thumbStyle.width().intValue();
        int thumbHeight = thumbStyle.height().intValue();
        thumbSize.setWidth(isHorizontal ? thumbWidth : thumbHeight);
        thumbSize.setHeight(isHorizontal ? thumbHeight : thumbWidth);
    }

    IntSize tickSize = LayoutTheme::theme().sliderTickSize();
    float zoomFactor = o->style()->effectiveZoom();
    FloatRect tickRect;
    int tickRegionSideMargin = 0;
    int tickRegionWidth = 0;
    IntRect trackBounds;
    LayoutObject* trackLayoutObject = input->userAgentShadowRoot()->getElementById(ShadowElementNames::sliderTrack())->layoutObject();
    // We can ignoring transforms because transform is handled by the graphics context.
    if (trackLayoutObject)
        trackBounds = trackLayoutObject->absoluteBoundingBoxRectIgnoringTransforms();
    IntRect sliderBounds = o->absoluteBoundingBoxRectIgnoringTransforms();

    // Make position relative to the transformed ancestor element.
    trackBounds.setX(trackBounds.x() - sliderBounds.x() + rect.x());
    trackBounds.setY(trackBounds.y() - sliderBounds.y() + rect.y());

    if (isHorizontal) {
        tickRect.setWidth(floor(tickSize.width() * zoomFactor));
        tickRect.setHeight(floor(tickSize.height() * zoomFactor));
        tickRect.setY(floor(rect.y() + rect.height() / 2.0 + LayoutTheme::theme().sliderTickOffsetFromTrackCenter() * zoomFactor));
        tickRegionSideMargin = trackBounds.x() + (thumbSize.width() - tickSize.width() * zoomFactor) / 2.0;
        tickRegionWidth = trackBounds.width() - thumbSize.width();
    } else {
        tickRect.setWidth(floor(tickSize.height() * zoomFactor));
        tickRect.setHeight(floor(tickSize.width() * zoomFactor));
        tickRect.setX(floor(rect.x() + rect.width() / 2.0 + LayoutTheme::theme().sliderTickOffsetFromTrackCenter() * zoomFactor));
        tickRegionSideMargin = trackBounds.y() + (thumbSize.width() - tickSize.width() * zoomFactor) / 2.0;
        tickRegionWidth = trackBounds.height() - thumbSize.width();
    }
    RefPtrWillBeRawPtr<HTMLDataListOptionsCollection> options = dataList->options();
    for (unsigned i = 0; HTMLOptionElement* optionElement = options->item(i); i++) {
        String value = optionElement->value();
        if (!input->isValidValue(value))
            continue;
        double parsedValue = parseToDoubleForNumberType(input->sanitizeValue(value));
        double tickFraction = (parsedValue - min) / (max - min);
        double tickRatio = isHorizontal && o->style()->isLeftToRightDirection() ? tickFraction : 1.0 - tickFraction;
        double tickPosition = round(tickRegionSideMargin + tickRegionWidth * tickRatio);
        if (isHorizontal)
            tickRect.setX(tickPosition);
        else
            tickRect.setY(tickPosition);
        paintInfo.context->fillRect(tickRect, o->resolveColor(CSSPropertyColor));
    }
}

bool ThemePainter::paintUsingFallbackTheme(LayoutObject* o, const PaintInfo& i, const IntRect& r)
{
    ControlPart part = o->style()->appearance();
    switch (part) {
    case CheckboxPart:
        return paintCheckboxUsingFallbackTheme(o, i, r);
    case RadioPart:
        return paintRadioUsingFallbackTheme(o, i, r);
    default:
        break;
    }
    return true;
}

bool ThemePainter::paintCheckboxUsingFallbackTheme(LayoutObject* o, const PaintInfo& i, const IntRect& r)
{
    WebFallbackThemeEngine::ExtraParams extraParams;
    WebCanvas* canvas = i.context->canvas();
    extraParams.button.checked = LayoutTheme::isChecked(o);
    extraParams.button.indeterminate = LayoutTheme::isIndeterminate(o);

    float zoomLevel = o->style()->effectiveZoom();
    GraphicsContextStateSaver stateSaver(*i.context);
    IntRect unzoomedRect = r;
    if (zoomLevel != 1) {
        unzoomedRect.setWidth(unzoomedRect.width() / zoomLevel);
        unzoomedRect.setHeight(unzoomedRect.height() / zoomLevel);
        i.context->translate(unzoomedRect.x(), unzoomedRect.y());
        i.context->scale(zoomLevel, zoomLevel);
        i.context->translate(-unzoomedRect.x(), -unzoomedRect.y());
    }

    Platform::current()->fallbackThemeEngine()->paint(canvas, WebFallbackThemeEngine::PartCheckbox, getWebFallbackThemeState(o), WebRect(unzoomedRect), &extraParams);
    return false;
}

bool ThemePainter::paintRadioUsingFallbackTheme(LayoutObject* o, const PaintInfo& i, const IntRect& r)
{
    WebFallbackThemeEngine::ExtraParams extraParams;
    WebCanvas* canvas = i.context->canvas();
    extraParams.button.checked = LayoutTheme::isChecked(o);
    extraParams.button.indeterminate = LayoutTheme::isIndeterminate(o);

    float zoomLevel = o->style()->effectiveZoom();
    GraphicsContextStateSaver stateSaver(*i.context);
    IntRect unzoomedRect = r;
    if (zoomLevel != 1) {
        unzoomedRect.setWidth(unzoomedRect.width() / zoomLevel);
        unzoomedRect.setHeight(unzoomedRect.height() / zoomLevel);
        i.context->translate(unzoomedRect.x(), unzoomedRect.y());
        i.context->scale(zoomLevel, zoomLevel);
        i.context->translate(-unzoomedRect.x(), -unzoomedRect.y());
    }

    Platform::current()->fallbackThemeEngine()->paint(canvas, WebFallbackThemeEngine::PartRadio, getWebFallbackThemeState(o), WebRect(unzoomedRect), &extraParams);
    return false;
}

} // namespace blink

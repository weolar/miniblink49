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
#include "core/paint/ThemePainterDefault.h"

#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutProgress.h"
#include "core/layout/LayoutTheme.h"
#include "core/paint/MediaControlsPainter.h"
#include "core/paint/PaintInfo.h"
#include "platform/LayoutTestSupport.h"
#include "platform/graphics/Color.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/GraphicsContextStateSaver.h"
#include "public/platform/Platform.h"
#include "public/platform/WebRect.h"
#include "public/platform/WebThemeEngine.h"

namespace blink {

namespace {

const unsigned defaultButtonBackgroundColor = 0xffdddddd;

bool useMockTheme()
{
    return LayoutTestSupport::isRunningLayoutTest();
}

WebThemeEngine::State getWebThemeState(const LayoutObject* o)
{
    if (!LayoutTheme::isEnabled(o))
        return WebThemeEngine::StateDisabled;
    if (useMockTheme() && LayoutTheme::isReadOnlyControl(o))
        return WebThemeEngine::StateReadonly;
    if (LayoutTheme::isPressed(o))
        return WebThemeEngine::StatePressed;
    if (useMockTheme() && LayoutTheme::isFocused(o))
        return WebThemeEngine::StateFocused;
    if (LayoutTheme::isHovered(o))
        return WebThemeEngine::StateHover;

    return WebThemeEngine::StateNormal;
}

class DirectionFlippingScope {
public:
    DirectionFlippingScope(LayoutObject*, const PaintInfo&, const IntRect&);
    ~DirectionFlippingScope();

private:
    bool m_needsFlipping;
    const PaintInfo& m_paintInfo;
};

DirectionFlippingScope::DirectionFlippingScope(LayoutObject* layoutObject, const PaintInfo& paintInfo, const IntRect& rect)
    : m_needsFlipping(!layoutObject->styleRef().isLeftToRightDirection())
    , m_paintInfo(paintInfo)
{
    if (!m_needsFlipping)
        return;
    m_paintInfo.context->save();
    m_paintInfo.context->translate(2 * rect.x() + rect.width(), 0);
    m_paintInfo.context->scale(-1, 1);
}

DirectionFlippingScope::~DirectionFlippingScope()
{
    if (!m_needsFlipping)
        return;
    m_paintInfo.context->restore();
}

IntRect determinateProgressValueRectFor(LayoutProgress* layoutProgress, const IntRect& rect)
{
    int dx = rect.width() * layoutProgress->position();
    return IntRect(rect.x(), rect.y(), dx, rect.height());
}

IntRect indeterminateProgressValueRectFor(LayoutProgress* layoutProgress, const IntRect& rect)
{
    // Value comes from default of GTK+.
    static const int progressActivityBlocks = 5;

    int valueWidth = rect.width() / progressActivityBlocks;
    int movableWidth = rect.width() - valueWidth;
    if (movableWidth <= 0)
        return IntRect();

    double progress = layoutProgress->animationProgress();
    if (progress < 0.5)
        return IntRect(rect.x() + progress * 2 * movableWidth, rect.y(), valueWidth, rect.height());
    return IntRect(rect.x() + (1.0 - progress) * 2 * movableWidth, rect.y(), valueWidth, rect.height());
}

IntRect progressValueRectFor(LayoutProgress* layoutProgress, const IntRect& rect)
{
    return layoutProgress->isDeterminate() ? determinateProgressValueRectFor(layoutProgress, rect) : indeterminateProgressValueRectFor(layoutProgress, rect);
}

IntRect convertToPaintingRect(LayoutObject* inputLayoutObject, const LayoutObject* partLayoutObject, LayoutRect partRect, const IntRect& localOffset)
{
    // Compute an offset between the partLayoutObject and the inputLayoutObject.
    LayoutSize offsetFromInputLayoutObject = -partLayoutObject->offsetFromAncestorContainer(inputLayoutObject);
    // Move the rect into partLayoutObject's coords.
    partRect.move(offsetFromInputLayoutObject);
    // Account for the local drawing offset.
    partRect.move(localOffset.x(), localOffset.y());

    return pixelSnappedIntRect(partRect);
}

} // namespace

bool ThemePainterDefault::paintCheckbox(LayoutObject* o, const PaintInfo& i, const IntRect& rect)
{
    WebThemeEngine::ExtraParams extraParams;
    WebCanvas* canvas = i.context->canvas();
    extraParams.button.checked = LayoutTheme::isChecked(o);
    extraParams.button.indeterminate = LayoutTheme::isIndeterminate(o);

    float zoomLevel = o->style()->effectiveZoom();
    GraphicsContextStateSaver stateSaver(*i.context, false);
    IntRect unzoomedRect = rect;
    if (zoomLevel != 1) {
        stateSaver.save();
        unzoomedRect.setWidth(unzoomedRect.width() / zoomLevel);
        unzoomedRect.setHeight(unzoomedRect.height() / zoomLevel);
        i.context->translate(unzoomedRect.x(), unzoomedRect.y());
        i.context->scale(zoomLevel, zoomLevel);
        i.context->translate(-unzoomedRect.x(), -unzoomedRect.y());
    }

    Platform::current()->themeEngine()->paint(canvas, WebThemeEngine::PartCheckbox, getWebThemeState(o), WebRect(unzoomedRect), &extraParams);
    return false;
}

bool ThemePainterDefault::paintRadio(LayoutObject* o, const PaintInfo& i, const IntRect& rect)
{
    WebThemeEngine::ExtraParams extraParams;
    WebCanvas* canvas = i.context->canvas();
    extraParams.button.checked = LayoutTheme::isChecked(o);

    Platform::current()->themeEngine()->paint(canvas, WebThemeEngine::PartRadio, getWebThemeState(o), WebRect(rect), &extraParams);
    return false;
}

bool ThemePainterDefault::paintButton(LayoutObject* o, const PaintInfo& i, const IntRect& rect)
{
    WebThemeEngine::ExtraParams extraParams;
    WebCanvas* canvas = i.context->canvas();
    extraParams.button.hasBorder = true;
    extraParams.button.backgroundColor = useMockTheme() ? 0xffc0c0c0 : defaultButtonBackgroundColor;
    if (o->hasBackground())
        extraParams.button.backgroundColor = o->resolveColor(CSSPropertyBackgroundColor).rgb();

    Platform::current()->themeEngine()->paint(canvas, WebThemeEngine::PartButton, getWebThemeState(o), WebRect(rect), &extraParams);
    return false;
}

bool ThemePainterDefault::paintTextField(LayoutObject* o, const PaintInfo& i, const IntRect& rect)
{
    // WebThemeEngine does not handle border rounded corner and background image
    // so return true to draw CSS border and background.
    if (o->style()->hasBorderRadius() || o->style()->hasBackgroundImage())
        return true;

    ControlPart part = o->style()->appearance();

    WebThemeEngine::ExtraParams extraParams;
    extraParams.textField.isTextArea = part == TextAreaPart;
    extraParams.textField.isListbox = part == ListboxPart;

    WebCanvas* canvas = i.context->canvas();

    Color backgroundColor = o->resolveColor(CSSPropertyBackgroundColor);
    extraParams.textField.backgroundColor = backgroundColor.rgb();

    Platform::current()->themeEngine()->paint(canvas, WebThemeEngine::PartTextField, getWebThemeState(o), WebRect(rect), &extraParams);
    return false;
}

bool ThemePainterDefault::paintMenuList(LayoutObject* o, const PaintInfo& i, const IntRect& rect)
{
    if (!o->isBox())
        return false;

    const int right = rect.x() + rect.width();
    const int middle = rect.y() + rect.height() / 2;

    WebThemeEngine::ExtraParams extraParams;
    extraParams.menuList.arrowY = middle;
    const LayoutBox* box = toLayoutBox(o);
    // Match Chromium Win behaviour of showing all borders if any are shown.
    extraParams.menuList.hasBorder = box->borderRight() || box->borderLeft() || box->borderTop() || box->borderBottom();
    extraParams.menuList.hasBorderRadius = o->style()->hasBorderRadius();
    // Fallback to transparent if the specified color object is invalid.
    Color backgroundColor(Color::transparent);
    if (o->hasBackground())
        backgroundColor = o->resolveColor(CSSPropertyBackgroundColor);
    extraParams.menuList.backgroundColor = backgroundColor.rgb();

    // If we have a background image, don't fill the content area to expose the
    // parent's background. Also, we shouldn't fill the content area if the
    // alpha of the color is 0. The API of Windows GDI ignores the alpha.
    // FIXME: the normal Aura theme doesn't care about this, so we should
    // investigate if we really need fillContentArea.
    extraParams.menuList.fillContentArea = !o->style()->hasBackgroundImage() && backgroundColor.alpha();

    if (useMockTheme()) {
        // The size and position of the drop-down button is different between
        // the mock theme and the regular aura theme.
        int spacingTop = box->borderTop() + box->paddingTop();
        int spacingBottom = box->borderBottom() + box->paddingBottom();
        int spacingRight = box->borderRight() + box->paddingRight();
        extraParams.menuList.arrowX = (o->style()->direction() == RTL) ? rect.x() + 4 + spacingRight: right - 13 - spacingRight;
        extraParams.menuList.arrowHeight = rect.height() - spacingBottom - spacingTop;
    } else {
        extraParams.menuList.arrowX = (o->style()->direction() == RTL) ? rect.x() + 7 : right - 13;
    }

    WebCanvas* canvas = i.context->canvas();

    Platform::current()->themeEngine()->paint(canvas, WebThemeEngine::PartMenuList, getWebThemeState(o), WebRect(rect), &extraParams);
    return false;
}

bool ThemePainterDefault::paintMenuListButton(LayoutObject* o, const PaintInfo& i, const IntRect& rect)
{
    if (!o->isBox())
        return false;

    const int right = rect.x() + rect.width();
    const int middle = rect.y() + rect.height() / 2;

    WebThemeEngine::ExtraParams extraParams;
    extraParams.menuList.arrowY = middle;
    extraParams.menuList.hasBorder = false;
    extraParams.menuList.hasBorderRadius = o->style()->hasBorderRadius();
    extraParams.menuList.backgroundColor = Color::transparent;
    extraParams.menuList.fillContentArea = false;

    if (useMockTheme()) {
        const LayoutBox* box = toLayoutBox(o);
        // The size and position of the drop-down button is different between
        // the mock theme and the regular aura theme.
        int spacingTop = box->borderTop() + box->paddingTop();
        int spacingBottom = box->borderBottom() + box->paddingBottom();
        int spacingRight = box->borderRight() + box->paddingRight();
        extraParams.menuList.arrowX = (o->style()->direction() == RTL) ? rect.x() + 4 + spacingRight: right - 13 - spacingRight;
        extraParams.menuList.arrowHeight = rect.height() - spacingBottom - spacingTop;
    } else {
        extraParams.menuList.arrowX = (o->style()->direction() == RTL) ? rect.x() + 7 : right - 13;
    }

    WebCanvas* canvas = i.context->canvas();

    Platform::current()->themeEngine()->paint(canvas, WebThemeEngine::PartMenuList, getWebThemeState(o), WebRect(rect), &extraParams);
    return false;
}

bool ThemePainterDefault::paintSliderTrack(LayoutObject* o, const PaintInfo& i, const IntRect& rect)
{
    WebThemeEngine::ExtraParams extraParams;
    WebCanvas* canvas = i.context->canvas();
    extraParams.slider.vertical = o->style()->appearance() == SliderVerticalPart;

    paintSliderTicks(o, i, rect);

    // FIXME: Mock theme doesn't handle zoomed sliders.
    float zoomLevel = useMockTheme() ? 1 : o->style()->effectiveZoom();
    GraphicsContextStateSaver stateSaver(*i.context, false);
    IntRect unzoomedRect = rect;
    if (zoomLevel != 1) {
        stateSaver.save();
        unzoomedRect.setWidth(unzoomedRect.width() / zoomLevel);
        unzoomedRect.setHeight(unzoomedRect.height() / zoomLevel);
        i.context->translate(unzoomedRect.x(), unzoomedRect.y());
        i.context->scale(zoomLevel, zoomLevel);
        i.context->translate(-unzoomedRect.x(), -unzoomedRect.y());
    }

    Platform::current()->themeEngine()->paint(canvas, WebThemeEngine::PartSliderTrack, getWebThemeState(o), WebRect(unzoomedRect), &extraParams);

    return false;
}

bool ThemePainterDefault::paintSliderThumb(LayoutObject* o, const PaintInfo& i, const IntRect& rect)
{
    WebThemeEngine::ExtraParams extraParams;
    WebCanvas* canvas = i.context->canvas();
    extraParams.slider.vertical = o->style()->appearance() == SliderThumbVerticalPart;
    extraParams.slider.inDrag = LayoutTheme::isPressed(o);

    // FIXME: Mock theme doesn't handle zoomed sliders.
    float zoomLevel = useMockTheme() ? 1 : o->style()->effectiveZoom();
    GraphicsContextStateSaver stateSaver(*i.context, false);
    IntRect unzoomedRect = rect;
    if (zoomLevel != 1) {
        stateSaver.save();
        unzoomedRect.setWidth(unzoomedRect.width() / zoomLevel);
        unzoomedRect.setHeight(unzoomedRect.height() / zoomLevel);
        i.context->translate(unzoomedRect.x(), unzoomedRect.y());
        i.context->scale(zoomLevel, zoomLevel);
        i.context->translate(-unzoomedRect.x(), -unzoomedRect.y());
    }

    Platform::current()->themeEngine()->paint(canvas, WebThemeEngine::PartSliderThumb, getWebThemeState(o), WebRect(unzoomedRect), &extraParams);
    return false;
}

bool ThemePainterDefault::paintInnerSpinButton(LayoutObject* o, const PaintInfo& i, const IntRect& rect)
{
    WebThemeEngine::ExtraParams extraParams;
    WebCanvas* canvas = i.context->canvas();
    extraParams.innerSpin.spinUp = (LayoutTheme::controlStatesForLayoutObject(o) & SpinUpControlState);
    extraParams.innerSpin.readOnly = LayoutTheme::isReadOnlyControl(o);

    Platform::current()->themeEngine()->paint(canvas, WebThemeEngine::PartInnerSpinButton, getWebThemeState(o), WebRect(rect), &extraParams);
    return false;
}

bool ThemePainterDefault::paintProgressBar(LayoutObject* o, const PaintInfo& i, const IntRect& rect)
{
    if (!o->isProgress())
        return true;

    LayoutProgress* layoutProgress = toLayoutProgress(o);
    IntRect valueRect = progressValueRectFor(layoutProgress, rect);

    WebThemeEngine::ExtraParams extraParams;
    extraParams.progressBar.determinate = layoutProgress->isDeterminate();
    extraParams.progressBar.valueRectX = valueRect.x();
    extraParams.progressBar.valueRectY = valueRect.y();
    extraParams.progressBar.valueRectWidth = valueRect.width();
    extraParams.progressBar.valueRectHeight = valueRect.height();

    DirectionFlippingScope scope(o, i, rect);
    WebCanvas* canvas = i.context->canvas();
    Platform::current()->themeEngine()->paint(canvas, WebThemeEngine::PartProgressBar, getWebThemeState(o), WebRect(rect), &extraParams);
    return false;
}

bool ThemePainterDefault::paintTextArea(LayoutObject* o, const PaintInfo& i, const IntRect& r)
{
    return paintTextField(o, i, r);
}

bool ThemePainterDefault::paintSearchField(LayoutObject* o, const PaintInfo& i, const IntRect& r)
{
    return paintTextField(o, i, r);
}

bool ThemePainterDefault::paintSearchFieldCancelButton(LayoutObject* cancelButtonObject, const PaintInfo& paintInfo, const IntRect& r)
{
    // Get the layoutObject of <input> element.
    if (!cancelButtonObject->node())
        return false;
    Node* input = cancelButtonObject->node()->shadowHost();
    LayoutObject* baseLayoutObject = input ? input->layoutObject() : cancelButtonObject;
    if (!baseLayoutObject->isBox())
        return false;
    LayoutBox* inputLayoutBox = toLayoutBox(baseLayoutObject);
    LayoutRect inputContentBox = inputLayoutBox->contentBoxRect();

    // Make sure the scaled button stays square and will fit in its parent's box.
    LayoutUnit cancelButtonSize = std::min(inputContentBox.width(), std::min<LayoutUnit>(inputContentBox.height(), r.height()));
    // Calculate cancel button's coordinates relative to the input element.
    // Center the button vertically.  Round up though, so if it has to be one pixel off-center, it will
    // be one pixel closer to the bottom of the field.  This tends to look better with the text.
    LayoutRect cancelButtonRect(cancelButtonObject->offsetFromAncestorContainer(inputLayoutBox).width(),
        inputContentBox.y() + (inputContentBox.height() - cancelButtonSize + 1) / 2,
        cancelButtonSize, cancelButtonSize);
    IntRect paintingRect = convertToPaintingRect(inputLayoutBox, cancelButtonObject, cancelButtonRect, r);

    DEFINE_STATIC_REF(Image, cancelImage, (Image::loadPlatformResource("searchCancel")));
    DEFINE_STATIC_REF(Image, cancelPressedImage, (Image::loadPlatformResource("searchCancelPressed")));
    paintInfo.context->drawImage(LayoutTheme::isPressed(cancelButtonObject) ? cancelPressedImage : cancelImage, paintingRect);
    return false;
}

bool ThemePainterDefault::paintSearchFieldResultsDecoration(LayoutObject* magnifierObject, const PaintInfo& paintInfo, const IntRect& r)
{
    // Get the layoutObject of <input> element.
    if (!magnifierObject->node())
        return false;
    Node* input = magnifierObject->node()->shadowHost();
    LayoutObject* baseLayoutObject = input ? input->layoutObject() : magnifierObject;
    if (!baseLayoutObject->isBox())
        return false;
    LayoutBox* inputLayoutBox = toLayoutBox(baseLayoutObject);
    LayoutRect inputContentBox = inputLayoutBox->contentBoxRect();

    // Make sure the scaled decoration stays square and will fit in its parent's box.
    LayoutUnit magnifierSize = std::min(inputContentBox.width(), std::min<LayoutUnit>(inputContentBox.height(), r.height()));
    // Calculate decoration's coordinates relative to the input element.
    // Center the decoration vertically.  Round up though, so if it has to be one pixel off-center, it will
    // be one pixel closer to the bottom of the field.  This tends to look better with the text.
    LayoutRect magnifierRect(magnifierObject->offsetFromAncestorContainer(inputLayoutBox).width(),
        inputContentBox.y() + (inputContentBox.height() - magnifierSize + 1) / 2,
        magnifierSize, magnifierSize);
    IntRect paintingRect = convertToPaintingRect(inputLayoutBox, magnifierObject, magnifierRect, r);

    DEFINE_STATIC_REF(Image, magnifierImage, (Image::loadPlatformResource("searchMagnifier")));
    paintInfo.context->drawImage(magnifierImage, paintingRect);
    return false;
}

} // namespace blink

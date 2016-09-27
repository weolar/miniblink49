/**
 * Copyright (C) 2006, 2007, 2010 Apple Inc. All rights reserved.
 *           (C) 2008 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "core/layout/LayoutTextControlSingleLine.h"

#include "core/CSSValueKeywords.h"
#include "core/InputTypeNames.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/FrameSelection.h"
#include "core/frame/LocalFrame.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutAnalyzer.h"
#include "core/layout/LayoutTheme.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/ThemePainter.h"
#include "platform/PlatformKeyboardEvent.h"
#include "platform/fonts/SimpleFontData.h"

namespace blink {

using namespace HTMLNames;

LayoutTextControlSingleLine::LayoutTextControlSingleLine(HTMLInputElement* element)
    : LayoutTextControl(element)
    , m_shouldDrawCapsLockIndicator(false)
    , m_desiredInnerEditorLogicalHeight(-1)
{
}

LayoutTextControlSingleLine::~LayoutTextControlSingleLine()
{
}

inline Element* LayoutTextControlSingleLine::containerElement() const
{
    return inputElement()->userAgentShadowRoot()->getElementById(ShadowElementNames::textFieldContainer());
}

inline Element* LayoutTextControlSingleLine::editingViewPortElement() const
{
    return inputElement()->userAgentShadowRoot()->getElementById(ShadowElementNames::editingViewPort());
}

inline HTMLElement* LayoutTextControlSingleLine::innerSpinButtonElement() const
{
    return toHTMLElement(inputElement()->userAgentShadowRoot()->getElementById(ShadowElementNames::spinButton()));
}

void LayoutTextControlSingleLine::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    LayoutTextControl::paint(paintInfo, paintOffset);

    if (paintInfo.phase == PaintPhaseBlockBackground && m_shouldDrawCapsLockIndicator) {
        if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, *this, paintInfo.phase))
            return;

        LayoutRect contentsRect = contentBoxRect();

        // Center in the block progression direction.
        if (isHorizontalWritingMode())
            contentsRect.setY((size().height() - contentsRect.height()) / 2);
        else
            contentsRect.setX((size().width() - contentsRect.width()) / 2);

        // Convert the rect into the coords used for painting the content
        contentsRect.moveBy(paintOffset + location());
        IntRect snappedRect = pixelSnappedIntRect(contentsRect);
        LayoutObjectDrawingRecorder recorder(*paintInfo.context, *this, paintInfo.phase, snappedRect);
        LayoutTheme::theme().painter().paintCapsLockIndicator(this, paintInfo, snappedRect);
    }
}

LayoutUnit LayoutTextControlSingleLine::computeLogicalHeightLimit() const
{
    return containerElement() ? contentLogicalHeight() : logicalHeight();
}

void LayoutTextControlSingleLine::layout()
{
    LayoutAnalyzer::Scope analyzer(*this);
    SubtreeLayoutScope layoutScope(*this);

    // FIXME: This code is madness (https://crbug.com/461117)
    // FIXME: We should remove the height-related hacks in layout() and
    // styleDidChange(). We need them because
    // - Center the inner elements vertically if the input height is taller than
    //   the intrinsic height of the inner elements.
    // - Shrink the inner elment heights if the input height is samller than the
    //   intrinsic heights of the inner elements.

    // We don't honor paddings and borders for textfields without decorations
    // and type=search if the text height is taller than the contentHeight()
    // because of compability.

    LayoutBox* innerEditorLayoutObject = innerEditorElement()->layoutBox();
    bool innerEditorLayoutObjectHadLayout = innerEditorLayoutObject && innerEditorLayoutObject->needsLayout();
    LayoutBox* viewPortLayoutObject = editingViewPortElement() ? editingViewPortElement()->layoutBox() : 0;

    // To ensure consistency between layouts, we need to reset any conditionally overriden height.
    if (innerEditorLayoutObject && !innerEditorLayoutObject->styleRef().logicalHeight().isAuto()) {
        innerEditorLayoutObject->mutableStyleRef().setLogicalHeight(Length(Auto));
        layoutScope.setNeedsLayout(innerEditorLayoutObject, LayoutInvalidationReason::TextControlChanged);
        HTMLElement* placeholderElement = inputElement()->placeholderElement();
        if (LayoutBox* placeholderBox = placeholderElement ? placeholderElement->layoutBox() : 0)
            layoutScope.setNeedsLayout(placeholderBox, LayoutInvalidationReason::TextControlChanged);
    }
    if (viewPortLayoutObject && !viewPortLayoutObject->styleRef().logicalHeight().isAuto()) {
        viewPortLayoutObject->mutableStyleRef().setLogicalHeight(Length(Auto));
        layoutScope.setNeedsLayout(viewPortLayoutObject, LayoutInvalidationReason::TextControlChanged);
    }

    LayoutBlockFlow::layoutBlock(false);

    Element* container = containerElement();
    LayoutBox* containerLayoutObject = container ? container->layoutBox() : 0;

    // Set the text block height
    LayoutUnit desiredLogicalHeight = textBlockLogicalHeight();
    LayoutUnit logicalHeightLimit = computeLogicalHeightLimit();
    if (innerEditorLayoutObject && innerEditorLayoutObject->logicalHeight() > logicalHeightLimit) {
        if (desiredLogicalHeight != innerEditorLayoutObject->logicalHeight())
            layoutScope.setNeedsLayout(this, LayoutInvalidationReason::TextControlChanged);

        m_desiredInnerEditorLogicalHeight = desiredLogicalHeight;

        innerEditorLayoutObject->mutableStyleRef().setLogicalHeight(Length(desiredLogicalHeight, Fixed));
        layoutScope.setNeedsLayout(innerEditorLayoutObject, LayoutInvalidationReason::TextControlChanged);
        if (viewPortLayoutObject) {
            viewPortLayoutObject->mutableStyleRef().setLogicalHeight(Length(desiredLogicalHeight, Fixed));
            layoutScope.setNeedsLayout(viewPortLayoutObject, LayoutInvalidationReason::TextControlChanged);
        }
    }
    // The container might be taller because of decoration elements.
    if (containerLayoutObject) {
        containerLayoutObject->layoutIfNeeded();
        LayoutUnit containerLogicalHeight = containerLayoutObject->logicalHeight();
        if (containerLogicalHeight > logicalHeightLimit) {
            containerLayoutObject->mutableStyleRef().setLogicalHeight(Length(logicalHeightLimit, Fixed));
            layoutScope.setNeedsLayout(this, LayoutInvalidationReason::TextControlChanged);
        } else if (containerLayoutObject->logicalHeight() < contentLogicalHeight()) {
            containerLayoutObject->mutableStyleRef().setLogicalHeight(Length(contentLogicalHeight(), Fixed));
            layoutScope.setNeedsLayout(this, LayoutInvalidationReason::TextControlChanged);
        } else {
            containerLayoutObject->mutableStyleRef().setLogicalHeight(Length(containerLogicalHeight, Fixed));
        }
    }

    // We ensure that the inner editor layoutObject is laid out at least once. This is
    // required as the logic below assumes that we don't carry over previous layout values.
    if (innerEditorLayoutObject && !innerEditorLayoutObjectHadLayout)
        layoutScope.setNeedsLayout(innerEditorLayoutObject, LayoutInvalidationReason::TextControlChanged);

    // If we need another layout pass, we have changed one of children's height so we need to relayout them.
    if (needsLayout())
        LayoutBlockFlow::layoutBlock(true);

    // Center the child block in the block progression direction (vertical centering for horizontal text fields).
    if (!container && innerEditorLayoutObject && innerEditorLayoutObject->size().height() != contentLogicalHeight()) {
        LayoutUnit logicalHeightDiff = innerEditorLayoutObject->logicalHeight() - contentLogicalHeight();
        innerEditorLayoutObject->setLogicalTop(innerEditorLayoutObject->logicalTop() - (logicalHeightDiff / 2 + layoutMod(logicalHeightDiff, 2)));
    } else {
        centerContainerIfNeeded(containerLayoutObject);
    }

    HTMLElement* placeholderElement = inputElement()->placeholderElement();
    if (LayoutBox* placeholderBox = placeholderElement ? placeholderElement->layoutBox() : 0) {
        LayoutSize innerEditorSize;

        if (innerEditorLayoutObject)
            innerEditorSize = innerEditorLayoutObject->size();
        placeholderBox->mutableStyleRef().setWidth(Length(innerEditorSize.width() - placeholderBox->borderAndPaddingWidth(), Fixed));
        placeholderBox->mutableStyleRef().setHeight(Length(innerEditorSize.height() - placeholderBox->borderAndPaddingHeight(), Fixed));
        bool neededLayout = placeholderBox->needsLayout();
        placeholderBox->layoutIfNeeded();
        LayoutPoint textOffset;
        if (innerEditorLayoutObject)
            textOffset = innerEditorLayoutObject->location();
        if (editingViewPortElement() && editingViewPortElement()->layoutBox())
            textOffset += toLayoutSize(editingViewPortElement()->layoutBox()->location());
        if (containerLayoutObject)
            textOffset += toLayoutSize(containerLayoutObject->location());
        placeholderBox->setLocation(textOffset);

        // The placeholder gets layout last, after the parent text control and its other children,
        // so in order to get the correct overflow from the placeholder we need to recompute it now.
        if (neededLayout)
            computeOverflow(clientLogicalBottom());
    }
}

bool LayoutTextControlSingleLine::nodeAtPoint(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction hitTestAction)
{
    if (!LayoutTextControl::nodeAtPoint(result, locationInContainer, accumulatedOffset, hitTestAction))
        return false;

    // Say that we hit the inner text element if
    //  - we hit a node inside the inner text element,
    //  - we hit the <input> element (e.g. we're over the border or padding), or
    //  - we hit regions not in any decoration buttons.
    Element* container = containerElement();
    if (result.innerNode()->isDescendantOf(innerEditorElement()) || result.innerNode() == node() || (container && container == result.innerNode())) {
        LayoutPoint pointInParent = locationInContainer.point();
        if (container && editingViewPortElement()) {
            if (editingViewPortElement()->layoutBox())
                pointInParent -= toLayoutSize(editingViewPortElement()->layoutBox()->location());
            if (container->layoutBox())
                pointInParent -= toLayoutSize(container->layoutBox()->location());
        }
        hitInnerEditorElement(result, pointInParent, accumulatedOffset);
    }
    return true;
}

void LayoutTextControlSingleLine::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    m_desiredInnerEditorLogicalHeight = -1;
    LayoutTextControl::styleDidChange(diff, oldStyle);

    // We may have set the width and the height in the old style in layout().
    // Reset them now to avoid getting a spurious layout hint.
    Element* viewPort = editingViewPortElement();
    if (LayoutObject* viewPortLayoutObject = viewPort ? viewPort->layoutObject() : 0) {
        viewPortLayoutObject->mutableStyleRef().setHeight(Length());
        viewPortLayoutObject->mutableStyleRef().setWidth(Length());
    }
    Element* container = containerElement();
    if (LayoutObject* containerLayoutObject = container ? container->layoutObject() : 0) {
        containerLayoutObject->mutableStyleRef().setHeight(Length());
        containerLayoutObject->mutableStyleRef().setWidth(Length());
    }
    LayoutObject* innerEditorLayoutObject = innerEditorElement()->layoutObject();
    if (innerEditorLayoutObject && diff.needsFullLayout())
        innerEditorLayoutObject->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::StyleChange);
    if (HTMLElement* placeholder = inputElement()->placeholderElement())
        placeholder->setInlineStyleProperty(CSSPropertyTextOverflow, textShouldBeTruncated() ? CSSValueEllipsis : CSSValueClip);
    setHasOverflowClip(false);
}

void LayoutTextControlSingleLine::capsLockStateMayHaveChanged()
{
    if (!node())
        return;

    // Only draw the caps lock indicator if these things are true:
    // 1) The field is a password field
    // 2) The frame is active
    // 3) The element is focused
    // 4) The caps lock is on
    bool shouldDrawCapsLockIndicator = false;

    if (LocalFrame* frame = document().frame())
        shouldDrawCapsLockIndicator = inputElement()->type() == InputTypeNames::password && frame->selection().isFocusedAndActive() && document().focusedElement() == node() && PlatformKeyboardEvent::currentCapsLockState();

    if (shouldDrawCapsLockIndicator != m_shouldDrawCapsLockIndicator) {
        m_shouldDrawCapsLockIndicator = shouldDrawCapsLockIndicator;
        setShouldDoFullPaintInvalidation();
    }
}

bool LayoutTextControlSingleLine::hasControlClip() const
{
    // Apply control clip for text fields with decorations.
    return !!containerElement();
}

LayoutRect LayoutTextControlSingleLine::controlClipRect(const LayoutPoint& additionalOffset) const
{
    ASSERT(hasControlClip());
    LayoutRect clipRect = contentBoxRect();
    if (containerElement()->layoutBox())
        clipRect = unionRect(clipRect, containerElement()->layoutBox()->frameRect());
    clipRect.moveBy(additionalOffset);
    return clipRect;
}

float LayoutTextControlSingleLine::getAvgCharWidth(AtomicString family)
{
    // Since Lucida Grande is the default font, we want this to match the width
    // of MS Shell Dlg, the default font for textareas in Firefox, Safari Win and
    // IE for some encodings (in IE, the default font is encoding specific).
    // 901 is the avgCharWidth value in the OS/2 table for MS Shell Dlg.
    if (family == "Lucida Grande")
        return scaleEmToUnits(901);

    return LayoutTextControl::getAvgCharWidth(family);
}

LayoutUnit LayoutTextControlSingleLine::preferredContentLogicalWidth(float charWidth) const
{
    int factor;
    bool includesDecoration = inputElement()->sizeShouldIncludeDecoration(factor);
    if (factor <= 0)
        factor = 20;

    LayoutUnit result = LayoutUnit::fromFloatCeil(charWidth * factor);

    float maxCharWidth = 0.f;
    AtomicString family = styleRef().font().fontDescription().family().family();
    // Since Lucida Grande is the default font, we want this to match the width
    // of MS Shell Dlg, the default font for textareas in Firefox, Safari Win and
    // IE for some encodings (in IE, the default font is encoding specific).
    // 4027 is the (xMax - xMin) value in the "head" font table for MS Shell Dlg.
    if (family == "Lucida Grande")
        maxCharWidth = scaleEmToUnits(4027);
    else if (hasValidAvgCharWidth(family))
        maxCharWidth = roundf(styleRef().font().primaryFont()->maxCharWidth());

    // For text inputs, IE adds some extra width.
    if (maxCharWidth > 0.f)
        result += maxCharWidth - charWidth;

    if (includesDecoration) {
        HTMLElement* spinButton = innerSpinButtonElement();
        if (LayoutBox* spinLayoutObject = spinButton ? spinButton->layoutBox() : 0) {
            result += spinLayoutObject->borderAndPaddingLogicalWidth();
            // Since the width of spinLayoutObject is not calculated yet, spinLayoutObject->logicalWidth() returns 0.
            // So ensureComputedStyle()->logicalWidth() is used instead.
            result += spinButton->ensureComputedStyle()->logicalWidth().value();
        }
    }

    return result;
}

LayoutUnit LayoutTextControlSingleLine::computeControlLogicalHeight(LayoutUnit lineHeight, LayoutUnit nonContentHeight) const
{
    return lineHeight + nonContentHeight;
}

PassRefPtr<ComputedStyle> LayoutTextControlSingleLine::createInnerEditorStyle(const ComputedStyle& startStyle) const
{
    RefPtr<ComputedStyle> textBlockStyle = ComputedStyle::create();
    textBlockStyle->inheritFrom(startStyle);
    adjustInnerEditorStyle(*textBlockStyle);

    textBlockStyle->setWhiteSpace(PRE);
    textBlockStyle->setOverflowWrap(NormalOverflowWrap);
    textBlockStyle->setOverflowX(OHIDDEN);
    textBlockStyle->setOverflowY(OHIDDEN);
    textBlockStyle->setTextOverflow(textShouldBeTruncated() ? TextOverflowEllipsis : TextOverflowClip);

    if (m_desiredInnerEditorLogicalHeight >= 0)
        textBlockStyle->setLogicalHeight(Length(m_desiredInnerEditorLogicalHeight, Fixed));
    // Do not allow line-height to be smaller than our default.
    if (textBlockStyle->fontMetrics().lineSpacing() > lineHeight(true, HorizontalLine, PositionOfInteriorLineBoxes))
        textBlockStyle->setLineHeight(ComputedStyle::initialLineHeight());

    textBlockStyle->setDisplay(BLOCK);
    textBlockStyle->setUnique();

    if (inputElement()->shouldRevealPassword())
        textBlockStyle->setTextSecurity(TSNONE);

    return textBlockStyle.release();
}

bool LayoutTextControlSingleLine::textShouldBeTruncated() const
{
    return document().focusedElement() != node() && styleRef().textOverflow() == TextOverflowEllipsis;
}

void LayoutTextControlSingleLine::autoscroll(const IntPoint& position)
{
    LayoutBox* layoutObject = innerEditorElement()->layoutBox();
    if (!layoutObject)
        return;

    layoutObject->autoscroll(position);
}

LayoutUnit LayoutTextControlSingleLine::scrollWidth() const
{
    if (LayoutBox* inner = innerEditorElement() ? innerEditorElement()->layoutBox() : 0) {
        // Adjust scrollWidth to inculde input element horizontal paddings and
        // decoration width
        LayoutUnit adjustment = clientWidth() - inner->clientWidth();
        // TODO(leviw): We floor to avoid breaking JS that tries to scroll to
        // scrollWidth - clientWidth.
        // TODO(leviw): These values are broken when zooming. crbug.com/471412
        return inner->scrollWidth().floor() + adjustment;
    }
    return LayoutBlockFlow::scrollWidth();
}

LayoutUnit LayoutTextControlSingleLine::scrollHeight() const
{
    if (LayoutBox* inner = innerEditorElement() ? innerEditorElement()->layoutBox() : 0) {
        // Adjust scrollHeight to include input element vertical paddings and
        // decoration height
        LayoutUnit adjustment = clientHeight() - inner->clientHeight();
        // TODO(leviw): We floor to avoid breaking JS that tries to scroll to
        // scrollHeight - clientHeight.
        // TODO(leviw): These values are broken when zooming. crbug.com/471412
        return inner->scrollHeight().floor() + adjustment;
    }
    return LayoutBlockFlow::scrollHeight();
}

LayoutUnit LayoutTextControlSingleLine::scrollLeft() const
{
    if (innerEditorElement())
        return innerEditorElement()->scrollLeft();
    return LayoutBlockFlow::scrollLeft();
}

LayoutUnit LayoutTextControlSingleLine::scrollTop() const
{
    if (innerEditorElement())
        return innerEditorElement()->scrollTop();
    return LayoutBlockFlow::scrollTop();
}

void LayoutTextControlSingleLine::setScrollLeft(LayoutUnit newLeft)
{
    if (innerEditorElement())
        innerEditorElement()->setScrollLeft(newLeft);
}

void LayoutTextControlSingleLine::setScrollTop(LayoutUnit newTop)
{
    if (innerEditorElement())
        innerEditorElement()->setScrollTop(newTop);
}

HTMLInputElement* LayoutTextControlSingleLine::inputElement() const
{
    return toHTMLInputElement(node());
}

}

/**
 * Copyright (C) 2006, 2007 Apple Inc. All rights reserved.
 *           (C) 2008 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
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
#include "core/layout/LayoutTextControl.h"

#include "core/html/HTMLTextFormControlElement.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutTheme.h"
#include "core/layout/TextRunConstructor.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "wtf/text/CharacterNames.h"

namespace blink {

LayoutTextControl::LayoutTextControl(HTMLTextFormControlElement* element)
    : LayoutBlockFlow(element)
{
    ASSERT(element);
}

LayoutTextControl::~LayoutTextControl()
{
}

HTMLTextFormControlElement* LayoutTextControl::textFormControlElement() const
{
    return toHTMLTextFormControlElement(node());
}

HTMLElement* LayoutTextControl::innerEditorElement() const
{
    return textFormControlElement()->innerEditorElement();
}

void LayoutTextControl::addChild(LayoutObject* newChild, LayoutObject* beforeChild)
{
    // FIXME: This is a terrible hack to get the caret over the placeholder text since it'll
    // make us paint the placeholder first. (See https://trac.webkit.org/changeset/118733)
    Node* node = newChild->node();
    if (node && node->isElementNode() && toElement(node)->shadowPseudoId() == "-webkit-input-placeholder")
        LayoutBlockFlow::addChild(newChild, firstChild());
    else
        LayoutBlockFlow::addChild(newChild, beforeChild);
}

void LayoutTextControl::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutBlockFlow::styleDidChange(diff, oldStyle);
    Element* innerEditor = innerEditorElement();
    if (!innerEditor)
        return;
    LayoutBlock* innerEditorLayoutObject = toLayoutBlock(innerEditor->layoutObject());
    if (innerEditorLayoutObject) {
        // We may have set the width and the height in the old style in layout().
        // Reset them now to avoid getting a spurious layout hint.
        innerEditorLayoutObject->mutableStyleRef().setHeight(Length());
        innerEditorLayoutObject->mutableStyleRef().setWidth(Length());
        innerEditorLayoutObject->setStyle(createInnerEditorStyle(styleRef()));
        innerEditor->setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::Control));
    }
    textFormControlElement()->updatePlaceholderVisibility(false);
}

static inline void updateUserModifyProperty(HTMLTextFormControlElement& node, ComputedStyle& style)
{
    style.setUserModify(node.isDisabledOrReadOnly() ? READ_ONLY : READ_WRITE_PLAINTEXT_ONLY);
}

void LayoutTextControl::adjustInnerEditorStyle(ComputedStyle& textBlockStyle) const
{
    // The inner block, if present, always has its direction set to LTR,
    // so we need to inherit the direction and unicode-bidi style from the element.
    textBlockStyle.setDirection(style()->direction());
    textBlockStyle.setUnicodeBidi(style()->unicodeBidi());

    updateUserModifyProperty(*textFormControlElement(), textBlockStyle);
}

int LayoutTextControl::textBlockLogicalHeight() const
{
    return logicalHeight() - borderAndPaddingLogicalHeight();
}

int LayoutTextControl::textBlockLogicalWidth() const
{
    Element* innerEditor = innerEditorElement();
    ASSERT(innerEditor);

    LayoutUnit unitWidth = logicalWidth() - borderAndPaddingLogicalWidth();
    if (innerEditor->layoutObject())
        unitWidth -= innerEditor->layoutBox()->paddingStart() + innerEditor->layoutBox()->paddingEnd();

    return unitWidth;
}

void LayoutTextControl::updateFromElement()
{
    Element* innerEditor = innerEditorElement();
    if (innerEditor && innerEditor->layoutObject())
        updateUserModifyProperty(*textFormControlElement(), innerEditor->layoutObject()->mutableStyleRef());
}

int LayoutTextControl::scrollbarThickness() const
{
    // FIXME: We should get the size of the scrollbar from the LayoutTheme instead.
    return ScrollbarTheme::theme()->scrollbarThickness();
}

void LayoutTextControl::computeLogicalHeight(LayoutUnit logicalHeight, LayoutUnit logicalTop, LogicalExtentComputedValues& computedValues) const
{
    HTMLElement* innerEditor = innerEditorElement();
    ASSERT(innerEditor);
    if (LayoutBox* innerEditorBox = innerEditor->layoutBox()) {
        LayoutUnit nonContentHeight = innerEditorBox->borderAndPaddingHeight() + innerEditorBox->marginHeight();
        logicalHeight = computeControlLogicalHeight(innerEditorBox->lineHeight(true, HorizontalLine, PositionOfInteriorLineBoxes), nonContentHeight);

        // We are able to have a horizontal scrollbar if the overflow style is scroll, or if its auto and there's no word wrap.
        if ((isHorizontalWritingMode() && (style()->overflowX() == OSCROLL ||  (style()->overflowX() == OAUTO && innerEditor->layoutObject()->style()->overflowWrap() == NormalOverflowWrap)))
            || (!isHorizontalWritingMode() && (style()->overflowY() == OSCROLL ||  (style()->overflowY() == OAUTO && innerEditor->layoutObject()->style()->overflowWrap() == NormalOverflowWrap))))
            logicalHeight += scrollbarThickness();

        // FIXME: The logical height of the inner text box should have been added before calling computeLogicalHeight to
        // avoid this hack.
        setIntrinsicContentLogicalHeight(logicalHeight);

        logicalHeight += borderAndPaddingHeight();
    }

    LayoutBox::computeLogicalHeight(logicalHeight, logicalTop, computedValues);
}

void LayoutTextControl::hitInnerEditorElement(HitTestResult& result, const LayoutPoint& pointInContainer, const LayoutPoint& accumulatedOffset)
{
    HTMLElement* innerEditor = innerEditorElement();
    if (!innerEditor->layoutObject())
        return;

    LayoutPoint adjustedLocation = accumulatedOffset + location();
    LayoutPoint localPoint = pointInContainer - toLayoutSize(adjustedLocation + innerEditor->layoutBox()->location());
    if (hasOverflowClip())
        localPoint += scrolledContentOffset();
    result.setNodeAndPosition(innerEditor, localPoint);
}

static const char* const fontFamiliesWithInvalidCharWidth[] = {
    "American Typewriter",
    "Arial Hebrew",
    "Chalkboard",
    "Cochin",
    "Corsiva Hebrew",
    "Courier",
    "Euphemia UCAS",
    "Geneva",
    "Gill Sans",
    "Hei",
    "Helvetica",
    "Hoefler Text",
    "InaiMathi",
    "Kai",
    "Lucida Grande",
    "Marker Felt",
    "Monaco",
    "Mshtakan",
    "New Peninim MT",
    "Osaka",
    "Raanana",
    "STHeiti",
    "Symbol",
    "Times",
    "Apple Braille",
    "Apple LiGothic",
    "Apple LiSung",
    "Apple Symbols",
    "AppleGothic",
    "AppleMyungjo",
    "#GungSeo",
    "#HeadLineA",
    "#PCMyungjo",
    "#PilGi",
};

// For font families where any of the fonts don't have a valid entry in the OS/2 table
// for avgCharWidth, fallback to the legacy webkit behavior of getting the avgCharWidth
// from the width of a '0'. This only seems to apply to a fixed number of Mac fonts,
// but, in order to get similar rendering across platforms, we do this check for
// all platforms.
bool LayoutTextControl::hasValidAvgCharWidth(AtomicString family)
{
    static HashSet<AtomicString>* fontFamiliesWithInvalidCharWidthMap = nullptr;

    if (family.isEmpty())
        return false;

    if (!fontFamiliesWithInvalidCharWidthMap) {
        fontFamiliesWithInvalidCharWidthMap = new HashSet<AtomicString>;

        for (size_t i = 0; i < WTF_ARRAY_LENGTH(fontFamiliesWithInvalidCharWidth); ++i)
            fontFamiliesWithInvalidCharWidthMap->add(AtomicString(fontFamiliesWithInvalidCharWidth[i]));
    }

    return !fontFamiliesWithInvalidCharWidthMap->contains(family);
}

float LayoutTextControl::getAvgCharWidth(AtomicString family)
{
    if (hasValidAvgCharWidth(family))
        return roundf(style()->font().primaryFont()->avgCharWidth());

    const UChar ch = '0';
    const String str = String(&ch, 1);
    const Font& font = style()->font();
    TextRun textRun = constructTextRun(this, font, str, styleRef(), TextRun::AllowTrailingExpansion);
    return font.width(textRun);
}

float LayoutTextControl::scaleEmToUnits(int x) const
{
    // This matches the unitsPerEm value for MS Shell Dlg and Courier New from the "head" font table.
    float unitsPerEm = 2048.0f;
    return roundf(style()->font().fontDescription().computedSize() * x / unitsPerEm);
}

void LayoutTextControl::computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const
{
    // Use average character width. Matches IE.
    AtomicString family = style()->font().fontDescription().family().family();
    maxLogicalWidth = preferredContentLogicalWidth(const_cast<LayoutTextControl*>(this)->getAvgCharWidth(family));
    if (LayoutBox* innerEditorLayoutBox = innerEditorElement()->layoutBox())
        maxLogicalWidth += innerEditorLayoutBox->paddingStart() + innerEditorLayoutBox->paddingEnd();
    if (!style()->logicalWidth().hasPercent())
        minLogicalWidth = maxLogicalWidth;
}

void LayoutTextControl::computePreferredLogicalWidths()
{
    ASSERT(preferredLogicalWidthsDirty());

    m_minPreferredLogicalWidth = 0;
    m_maxPreferredLogicalWidth = 0;
    const ComputedStyle& styleToUse = styleRef();

    if (styleToUse.logicalWidth().isFixed() && styleToUse.logicalWidth().value() >= 0)
        m_minPreferredLogicalWidth = m_maxPreferredLogicalWidth = adjustContentBoxLogicalWidthForBoxSizing(styleToUse.logicalWidth().value());
    else
        computeIntrinsicLogicalWidths(m_minPreferredLogicalWidth, m_maxPreferredLogicalWidth);

    if (styleToUse.logicalMinWidth().isFixed() && styleToUse.logicalMinWidth().value() > 0) {
        m_maxPreferredLogicalWidth = std::max(m_maxPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.logicalMinWidth().value()));
        m_minPreferredLogicalWidth = std::max(m_minPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.logicalMinWidth().value()));
    }

    if (styleToUse.logicalMaxWidth().isFixed()) {
        m_maxPreferredLogicalWidth = std::min(m_maxPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.logicalMaxWidth().value()));
        m_minPreferredLogicalWidth = std::min(m_minPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.logicalMaxWidth().value()));
    }

    LayoutUnit toAdd = borderAndPaddingLogicalWidth();

    m_minPreferredLogicalWidth += toAdd;
    m_maxPreferredLogicalWidth += toAdd;

    clearPreferredLogicalWidthsDirty();
}

void LayoutTextControl::addFocusRingRects(Vector<LayoutRect>& rects, const LayoutPoint& additionalOffset) const
{
    if (!size().isEmpty())
        rects.append(LayoutRect(additionalOffset, size()));
}

LayoutObject* LayoutTextControl::layoutSpecialExcludedChild(bool relayoutChildren, SubtreeLayoutScope& layoutScope)
{
    HTMLElement* placeholder = toHTMLTextFormControlElement(node())->placeholderElement();
    LayoutObject* placeholderLayoutObject = placeholder ? placeholder->layoutObject() : nullptr;
    if (!placeholderLayoutObject)
        return nullptr;
    if (relayoutChildren)
        layoutScope.setChildNeedsLayout(placeholderLayoutObject);
    return placeholderLayoutObject;
}

} // namespace blink

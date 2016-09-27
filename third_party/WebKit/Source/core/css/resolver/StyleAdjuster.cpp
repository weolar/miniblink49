/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 2004-2005 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2006, 2007 Nicholas Shanks (webkit@nickshanks.com)
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Alexey Proskuryakov <ap@webkit.org>
 * Copyright (C) 2007, 2008 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 * Copyright (C) Research In Motion Limited 2011. All rights reserved.
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
 */

#include "config.h"
#include "core/css/resolver/StyleAdjuster.h"

#include "core/HTMLNames.h"
#include "core/SVGNames.h"
#include "core/dom/ContainerNode.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/frame/FrameView.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLIFrameElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLPlugInElement.h"
#include "core/html/HTMLTableCellElement.h"
#include "core/html/HTMLTextAreaElement.h"
#include "core/layout/LayoutReplaced.h"
#include "core/layout/LayoutTheme.h"
#include "core/style/GridPosition.h"
#include "core/style/ComputedStyle.h"
#include "core/style/ComputedStyleConstants.h"
#include "core/svg/SVGSVGElement.h"
#include "platform/Length.h"
#include "platform/transforms/TransformOperations.h"
#include "wtf/Assertions.h"

namespace blink {

using namespace HTMLNames;

static EDisplay equivalentBlockDisplay(EDisplay display, bool isFloating, bool strictParsing)
{
    switch (display) {
    case BLOCK:
    case TABLE:
    case BOX:
    case FLEX:
    case GRID:
        return display;

    case LIST_ITEM:
        // It is a WinIE bug that floated list items lose their bullets, so we'll emulate the quirk, but only in quirks mode.
        if (!strictParsing && isFloating)
            return BLOCK;
        return display;
    case INLINE_TABLE:
        return TABLE;
    case INLINE_BOX:
        return BOX;
    case INLINE_FLEX:
        return FLEX;
    case INLINE_GRID:
        return GRID;

    case INLINE:
    case INLINE_BLOCK:
    case TABLE_ROW_GROUP:
    case TABLE_HEADER_GROUP:
    case TABLE_FOOTER_GROUP:
    case TABLE_ROW:
    case TABLE_COLUMN_GROUP:
    case TABLE_COLUMN:
    case TABLE_CELL:
    case TABLE_CAPTION:
        return BLOCK;
    case NONE:
        ASSERT_NOT_REACHED();
        return NONE;
    }
    ASSERT_NOT_REACHED();
    return BLOCK;
}

// CSS requires text-decoration to be reset at each DOM element for tables,
// inline blocks, inline tables, shadow DOM crossings, floating elements,
// and absolute or relatively positioned elements.
static bool doesNotInheritTextDecoration(const ComputedStyle& style, const Element* e)
{
    return style.display() == TABLE || style.display() == INLINE_TABLE
        || style.display() == INLINE_BLOCK || style.display() == INLINE_BOX || isAtShadowBoundary(e)
        || style.isFloating() || style.hasOutOfFlowPosition();
}

// FIXME: This helper is only needed because pseudoStyleForElement passes a null
// element to adjustComputedStyle, so we can't just use element->isInTopLayer().
static bool isInTopLayer(const Element* element, const ComputedStyle& style)
{
    return (element && element->isInTopLayer()) || style.styleType() == BACKDROP;
}

static bool parentStyleForcesZIndexToCreateStackingContext(const ComputedStyle& parentStyle)
{
    return parentStyle.isDisplayFlexibleOrGridBox();
}

static bool hasWillChangeThatCreatesStackingContext(const ComputedStyle& style)
{
    for (size_t i = 0; i < style.willChangeProperties().size(); ++i) {
        switch (style.willChangeProperties()[i]) {
        case CSSPropertyOpacity:
        case CSSPropertyTransform:
        case CSSPropertyAliasWebkitTransform:
        case CSSPropertyTransformStyle:
        case CSSPropertyAliasWebkitTransformStyle:
        case CSSPropertyPerspective:
        case CSSPropertyAliasWebkitPerspective:
        case CSSPropertyWebkitMask:
        case CSSPropertyWebkitMaskBoxImage:
        case CSSPropertyWebkitClipPath:
        case CSSPropertyWebkitBoxReflect:
        case CSSPropertyWebkitFilter:
        case CSSPropertyZIndex:
        case CSSPropertyPosition:
            return true;
        case CSSPropertyMixBlendMode:
        case CSSPropertyIsolation:
            if (RuntimeEnabledFeatures::cssCompositingEnabled())
                return true;
            break;
        default:
            break;
        }
    }
    return false;
}

void StyleAdjuster::adjustComputedStyle(ComputedStyle& style, const ComputedStyle& parentStyle, Element *e, const AuthorStyleInfo& authorStyle)
{
    if (style.display() != NONE) {
        if (e && e->isHTMLElement())
            adjustStyleForHTMLElement(style, parentStyle, toHTMLElement(*e));

        // Per the spec, position 'static' and 'relative' in the top layer compute to 'absolute'.
        if (isInTopLayer(e, style) && (style.position() == StaticPosition || style.position() == RelativePosition))
            style.setPosition(AbsolutePosition);

        // Absolute/fixed positioned elements, floating elements and the document element need block-like outside display.
        if (style.hasOutOfFlowPosition() || style.isFloating() || (e && e->document().documentElement() == e))
            style.setDisplay(equivalentBlockDisplay(style.display(), style.isFloating(), !m_useQuirksModeStyles));

        // We don't adjust the first letter style earlier because we may change the display setting in
        // adjustStyeForTagName() above.
        adjustStyleForFirstLetter(style);

        adjustStyleForDisplay(style, parentStyle);
    } else {
        adjustStyleForFirstLetter(style);
    }

    // Make sure our z-index value is only applied if the object is positioned.
    if (style.position() == StaticPosition && !parentStyleForcesZIndexToCreateStackingContext(parentStyle))
        style.setHasAutoZIndex();

    // Auto z-index becomes 0 for the root element and transparent objects. This prevents
    // cases where objects that should be blended as a single unit end up with a non-transparent
    // object wedged in between them. Auto z-index also becomes 0 for objects that specify transforms/masks/reflections.
    if (style.hasAutoZIndex() && ((e && e->document().documentElement() == e)
        || style.hasOpacity()
        || style.hasTransformRelatedProperty()
        || style.hasMask()
        || style.clipPath()
        || style.boxReflect()
        || style.hasFilter()
        || style.hasBlendMode()
        || style.hasIsolation()
        || style.position() == FixedPosition
        || isInTopLayer(e, style)
        || hasWillChangeThatCreatesStackingContext(style)))
        style.setZIndex(0);

    // will-change:transform should result in the same rendering behavior as having a transform,
    // including the creation of a containing block for fixed position descendants.
    // SVG elements can skip this because they implicitly have transforms.
    bool isSVGElement = e && e->isSVGElement();
    if (!isSVGElement && !style.hasTransform() && (style.willChangeProperties().contains(CSSPropertyAliasWebkitTransform) || style.willChangeProperties().contains(CSSPropertyTransform))) {
        bool makeIdentity = true;
        style.setTransform(TransformOperations(makeIdentity));
    }

    if (doesNotInheritTextDecoration(style, e))
        style.clearAppliedTextDecorations();

    style.applyTextDecorations();

    if (style.overflowX() != OVISIBLE || style.overflowY() != OVISIBLE)
        adjustOverflow(style);

    // Cull out any useless layers and also repeat patterns into additional layers.
    style.adjustBackgroundLayers();
    style.adjustMaskLayers();

    // Let the theme also have a crack at adjusting the style.
    if (style.hasAppearance())
        LayoutTheme::theme().adjustStyle(style, e, authorStyle);

    // If we have first-letter pseudo style, transitions, or animations, do not share this style.
    if (style.hasPseudoStyle(FIRST_LETTER) || style.transitions() || style.animations())
        style.setUnique();

    // FIXME: when dropping the -webkit prefix on transform-style, we should also have opacity < 1 cause flattening.
    if (style.preserves3D() && (style.overflowX() != OVISIBLE
        || style.overflowY() != OVISIBLE
        || style.hasFilter()))
        style.setTransformStyle3D(TransformStyle3DFlat);

    if (isSVGElement) {
        // Only the root <svg> element in an SVG document fragment tree honors css position
        if (!(isSVGSVGElement(*e) && e->parentNode() && !e->parentNode()->isSVGElement()))
            style.setPosition(ComputedStyle::initialPosition());

        // SVG text layout code expects us to be a block-level style element.
        if ((isSVGForeignObjectElement(*e) || isSVGTextElement(*e)) && style.isDisplayInlineType())
            style.setDisplay(BLOCK);

        // Columns don't apply to svg text elements.
        if (isSVGTextElement(*e))
            style.clearMultiCol();
    }
    adjustStyleForAlignment(style, parentStyle);
}

void StyleAdjuster::adjustStyleForFirstLetter(ComputedStyle& style)
{
    if (style.styleType() != FIRST_LETTER)
        return;

    // Force inline display (except for floating first-letters).
    style.setDisplay(style.isFloating() ? BLOCK : INLINE);

    // CSS2 says first-letter can't be positioned.
    style.setPosition(StaticPosition);
}

void StyleAdjuster::adjustStyleForAlignment(ComputedStyle& style, const ComputedStyle& parentStyle)
{
    bool isFlexOrGrid = style.isDisplayFlexibleOrGridBox();
    bool absolutePositioned = style.position() == AbsolutePosition;

    // If the inherited value of justify-items includes the legacy keyword, 'auto'
    // computes to the the inherited value.
    // Otherwise, auto computes to:
    //  - 'stretch' for flex containers and grid containers.
    //  - 'start' for everything else.
    if (style.justifyItemsPosition() == ItemPositionAuto) {
        if (parentStyle.justifyItemsPositionType() == LegacyPosition)
            style.setJustifyItems(parentStyle.justifyItems());
        else if (isFlexOrGrid)
            style.setJustifyItemsPosition(ItemPositionStretch);
    }

    // The 'auto' keyword computes to 'stretch' on absolutely-positioned elements,
    // and to the computed value of justify-items on the parent (minus
    // any legacy keywords) on all other boxes.
    if (style.justifySelfPosition() == ItemPositionAuto) {
        if (absolutePositioned)
            style.setJustifySelfPosition(ItemPositionStretch);
        else
            style.setJustifySelf(parentStyle.justifyItems());
    }

    // The 'auto' keyword computes to:
    //  - 'stretch' for flex containers and grid containers,
    //  - 'start' for everything else.
    if (style.alignItemsPosition() == ItemPositionAuto) {
        if (isFlexOrGrid)
            style.setAlignItemsPosition(ItemPositionStretch);
    }

    // The 'auto' keyword computes to 'stretch' on absolutely-positioned elements,
    // and to the computed value of align-items on the parent (minus
    // any 'legacy' keywords) on all other boxes.
    if (style.alignSelfPosition() == ItemPositionAuto) {
        if (absolutePositioned)
            style.setAlignSelfPosition(ItemPositionStretch);
        else
            style.setAlignSelf(parentStyle.alignItems());
    }

    // Block Containers: For table cells, the behavior of the 'auto' depends on the computed
    // value of 'vertical-align', otherwise behaves as 'start'.
    // Flex Containers: 'auto' computes to 'flex-start'.
    // Grid Containers: 'auto' computes to 'start', and 'stretch' behaves like 'start'.
    if ((style.justifyContentPosition() == ContentPositionAuto) && (style.justifyContentDistribution() == ContentDistributionDefault)) {
        if (style.isDisplayFlexibleOrGridBox()) {
            if (style.isDisplayFlexibleBox())
                style.setJustifyContentPosition(ContentPositionFlexStart);
            else
                style.setJustifyContentPosition(ContentPositionStart);
        }
    }

    // Block Containers: For table cells, the behavior of the 'auto' depends on the computed
    // value of 'vertical-align', otherwise behaves as 'start'.
    // Flex Containers: 'auto' computes to 'stretch'.
    // Grid Containers: 'auto' computes to 'start', and 'stretch' behaves like 'start'.
    if (style.alignContentPosition() == ContentPositionAuto && style.alignContentDistribution() == ContentDistributionDefault) {
        if (style.isDisplayFlexibleOrGridBox()) {
            if (style.isDisplayFlexibleBox())
                style.setAlignContentDistribution(ContentDistributionStretch);
            else
                style.setAlignContentPosition(ContentPositionStart);
        }
    }
}

void StyleAdjuster::adjustStyleForHTMLElement(ComputedStyle& style, const ComputedStyle& parentStyle, HTMLElement& element)
{
    // <div> and <span> are the most common elements on the web, we skip all the work for them.
    if (isHTMLDivElement(element) || isHTMLSpanElement(element))
        return;

    if (isHTMLTableCellElement(element)) {
        // If we have a <td> that specifies a float property, in quirks mode we just drop the float property.
        // FIXME: Why is this only <td> and not <th>?
        if (element.hasTagName(tdTag) && m_useQuirksModeStyles) {
            style.setDisplay(TABLE_CELL);
            style.setFloating(NoFloat);
        }
        // FIXME: We shouldn't be overriding start/-webkit-auto like this. Do it in html.css instead.
        // Table headers with a text-align of -webkit-auto will change the text-align to center.
        if (element.hasTagName(thTag) && style.textAlign() == TASTART)
            style.setTextAlign(CENTER);
        if (style.whiteSpace() == KHTML_NOWRAP) {
            // Figure out if we are really nowrapping or if we should just
            // use normal instead. If the width of the cell is fixed, then
            // we don't actually use NOWRAP.
            if (style.width().isFixed())
                style.setWhiteSpace(NORMAL);
            else
                style.setWhiteSpace(NOWRAP);
        }
        return;
    }

    if (isHTMLTableElement(element)) {
        // Sites commonly use display:inline/block on <td>s and <table>s. In quirks mode we force
        // these tags to retain their display types.
        if (m_useQuirksModeStyles)
            style.setDisplay(style.isDisplayInlineType() ? INLINE_TABLE : TABLE);
        // Tables never support the -webkit-* values for text-align and will reset back to the default.
        if (style.textAlign() == WEBKIT_LEFT || style.textAlign() == WEBKIT_CENTER || style.textAlign() == WEBKIT_RIGHT)
            style.setTextAlign(TASTART);
        return;
    }

    if (isHTMLFrameElement(element) || isHTMLFrameSetElement(element)) {
        // Frames and framesets never honor position:relative or position:absolute. This is necessary to
        // fix a crash where a site tries to position these objects. They also never honor display.
        style.setPosition(StaticPosition);
        style.setDisplay(BLOCK);
        return;
    }

    if (isHTMLRTElement(element)) {
        // Ruby text does not support float or position. This might change with evolution of the specification.
        style.setPosition(StaticPosition);
        style.setFloating(NoFloat);
        return;
    }

    if (isHTMLLegendElement(element)) {
        style.setDisplay(BLOCK);
        return;
    }

    if (isHTMLMarqueeElement(element)) {
        // For now, <marquee> requires an overflow clip to work properly.
        style.setOverflowX(OHIDDEN);
        style.setOverflowY(OHIDDEN);
        return;
    }

    if (isHTMLTextAreaElement(element)) {
        // Textarea considers overflow visible as auto.
        style.setOverflowX(style.overflowX() == OVISIBLE ? OAUTO : style.overflowX());
        style.setOverflowY(style.overflowY() == OVISIBLE ? OAUTO : style.overflowY());
        return;
    }

    if (isHTMLPlugInElement(element)) {
        style.setRequiresAcceleratedCompositingForExternalReasons(toHTMLPlugInElement(element).shouldAccelerate());

        // Plugins should get the standard replaced width/height instead of 'auto'.
        // Replaced layoutObjects get this for free, and fallback content doesn't count.
        if (toHTMLPlugInElement(element).usePlaceholderContent()) {
            if (style.width().isAuto())
                style.setWidth(Length(LayoutReplaced::defaultWidth, Fixed));
            if (style.height().isAuto())
                style.setHeight(Length(LayoutReplaced::defaultHeight, Fixed));
        }

        return;
    }
}

void StyleAdjuster::adjustOverflow(ComputedStyle& style)
{
    ASSERT(style.overflowX() != OVISIBLE || style.overflowY() != OVISIBLE);

    // If either overflow value is not visible, change to auto.
    if (style.overflowX() == OVISIBLE && style.overflowY() != OVISIBLE) {
        // FIXME: Once we implement pagination controls, overflow-x should default to hidden
        // if overflow-y is set to -webkit-paged-x or -webkit-page-y. For now, we'll let it
        // default to auto so we can at least scroll through the pages.
        style.setOverflowX(OAUTO);
    } else if (style.overflowY() == OVISIBLE && style.overflowX() != OVISIBLE) {
        style.setOverflowY(OAUTO);
    }

    // Table rows, sections and the table itself will support overflow:hidden and will ignore scroll/auto.
    // FIXME: Eventually table sections will support auto and scroll.
    if (style.display() == TABLE || style.display() == INLINE_TABLE
        || style.display() == TABLE_ROW_GROUP || style.display() == TABLE_ROW) {
        if (style.overflowX() != OVISIBLE && style.overflowX() != OHIDDEN)
            style.setOverflowX(OVISIBLE);
        if (style.overflowY() != OVISIBLE && style.overflowY() != OHIDDEN)
            style.setOverflowY(OVISIBLE);
    }

    // Menulists should have visible overflow
    if (style.appearance() == MenulistPart) {
        style.setOverflowX(OVISIBLE);
        style.setOverflowY(OVISIBLE);
    }
}

void StyleAdjuster::adjustStyleForDisplay(ComputedStyle& style, const ComputedStyle& parentStyle)
{
    if (style.display() == BLOCK && !style.isFloating())
        return;

    // FIXME: Don't support this mutation for pseudo styles like first-letter or first-line, since it's not completely
    // clear how that should work.
    if (style.display() == INLINE && style.styleType() == NOPSEUDO && style.writingMode() != parentStyle.writingMode())
        style.setDisplay(INLINE_BLOCK);

    // After performing the display mutation, check table rows. We do not honor position: relative table rows or cells.
    // This has been established for position: relative in CSS2.1 (and caused a crash in containingBlock()
    // on some sites).
    if ((style.display() == TABLE_HEADER_GROUP || style.display() == TABLE_ROW_GROUP
        || style.display() == TABLE_FOOTER_GROUP || style.display() == TABLE_ROW)
        && style.position() == RelativePosition)
        style.setPosition(StaticPosition);

    // writing-mode does not apply to table row groups, table column groups, table rows, and table columns.
    // FIXME: Table cells should be allowed to be perpendicular or flipped with respect to the table, though.
    if (style.display() == TABLE_COLUMN || style.display() == TABLE_COLUMN_GROUP || style.display() == TABLE_FOOTER_GROUP
        || style.display() == TABLE_HEADER_GROUP || style.display() == TABLE_ROW || style.display() == TABLE_ROW_GROUP
        || style.display() == TABLE_CELL)
        style.setWritingMode(parentStyle.writingMode());

    // FIXME: Since we don't support block-flow on flexible boxes yet, disallow setting
    // of block-flow to anything other than TopToBottomWritingMode.
    // https://bugs.webkit.org/show_bug.cgi?id=46418 - Flexible box support.
    if (style.writingMode() != TopToBottomWritingMode && (style.display() == BOX || style.display() == INLINE_BOX))
        style.setWritingMode(TopToBottomWritingMode);

    if (parentStyle.isDisplayFlexibleOrGridBox()) {
        style.setFloating(NoFloat);
        style.setDisplay(equivalentBlockDisplay(style.display(), style.isFloating(), !m_useQuirksModeStyles));
    }
}

}

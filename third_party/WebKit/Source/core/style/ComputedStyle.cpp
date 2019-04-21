/*
 * Copyright (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2011 Adobe Systems Incorporated. All rights reserved.
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
#include "core/style/ComputedStyle.h"

#include "core/css/resolver/StyleResolver.h"
#include "core/layout/LayoutTheme.h"
#include "core/layout/TextAutosizer.h"
#include "core/style/AppliedTextDecoration.h"
#include "core/style/BorderEdge.h"
#include "core/style/ContentData.h"
#include "core/style/DataEquivalency.h"
#include "core/style/ComputedStyleConstants.h"
#include "core/style/PathStyleMotionPath.h"
#include "core/style/QuotesData.h"
#include "core/style/ShadowList.h"
#include "core/style/StyleImage.h"
#include "core/style/StyleInheritedData.h"
#include "platform/LengthFunctions.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/fonts/Font.h"
#include "platform/fonts/FontSelector.h"
#include "platform/geometry/FloatRoundedRect.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/transforms/RotateTransformOperation.h"
#include "platform/transforms/ScaleTransformOperation.h"
#include "platform/transforms/TranslateTransformOperation.h"
#include "wtf/MathExtras.h"

#include <algorithm>

namespace blink {

struct SameSizeAsBorderValue {
    RGBA32 m_color;
    unsigned m_width;
};

static_assert(sizeof(BorderValue) == sizeof(SameSizeAsBorderValue), "BorderValue should stay small");

struct SameSizeAsComputedStyle : public RefCounted<SameSizeAsComputedStyle> {
    void* dataRefs[7];
    void* ownPtrs[1];
    void* dataRefSvgStyle;

    struct InheritedFlags {
        unsigned m_bitfields[2];
    } inherited_flags;

    struct NonInheritedFlags {
        unsigned m_bitfields[2];
    } noninherited_flags;
};

static_assert(sizeof(ComputedStyle) == sizeof(SameSizeAsComputedStyle), "ComputedStyle should stay small");

PassRefPtr<ComputedStyle> ComputedStyle::create()
{
    return adoptRef(new ComputedStyle());
}

PassRefPtr<ComputedStyle> ComputedStyle::createInitialStyle()
{
    return adoptRef(new ComputedStyle(InitialStyle));
}

PassRefPtr<ComputedStyle> ComputedStyle::createAnonymousStyleWithDisplay(const ComputedStyle& parentStyle, EDisplay display)
{
    RefPtr<ComputedStyle> newStyle = ComputedStyle::create();
    newStyle->inheritFrom(parentStyle);
    newStyle->inheritUnicodeBidiFrom(parentStyle);
    newStyle->setDisplay(display);
    return newStyle;
}

PassRefPtr<ComputedStyle> ComputedStyle::clone(const ComputedStyle& other)
{
    return adoptRef(new ComputedStyle(other));
}

ALWAYS_INLINE ComputedStyle::ComputedStyle()
    : m_box(initialStyle()->m_box)
    , visual(initialStyle()->visual)
    , m_background(initialStyle()->m_background)
    , surround(initialStyle()->surround)
    , rareNonInheritedData(initialStyle()->rareNonInheritedData)
    , rareInheritedData(initialStyle()->rareInheritedData)
    , inherited(initialStyle()->inherited)
    , m_svgStyle(initialStyle()->m_svgStyle)
{
    setBitDefaults(); // Would it be faster to copy this from the default style?
    static_assert((sizeof(InheritedFlags) <= 8), "InheritedFlags should not grow");
    static_assert((sizeof(NonInheritedFlags) <= 8), "NonInheritedFlags should not grow");
}

ALWAYS_INLINE ComputedStyle::ComputedStyle(InitialStyleTag)
{
    setBitDefaults();

    m_box.init();
    visual.init();
    m_background.init();
    surround.init();
    rareNonInheritedData.init();
    rareNonInheritedData.access()->m_deprecatedFlexibleBox.init();
    rareNonInheritedData.access()->m_flexibleBox.init();
    rareNonInheritedData.access()->m_multiCol.init();
    rareNonInheritedData.access()->m_transform.init();
    rareNonInheritedData.access()->m_willChange.init();
    rareNonInheritedData.access()->m_filter.init();
    rareNonInheritedData.access()->m_grid.init();
    rareNonInheritedData.access()->m_gridItem.init();
    rareNonInheritedData.access()->m_scrollSnap.init();
    rareInheritedData.init();
    inherited.init();
    m_svgStyle.init();
}

ALWAYS_INLINE ComputedStyle::ComputedStyle(const ComputedStyle& o)
    : RefCounted<ComputedStyle>()
    , m_box(o.m_box)
    , visual(o.visual)
    , m_background(o.m_background)
    , surround(o.surround)
    , rareNonInheritedData(o.rareNonInheritedData)
    , rareInheritedData(o.rareInheritedData)
    , inherited(o.inherited)
    , m_svgStyle(o.m_svgStyle)
    , inherited_flags(o.inherited_flags)
    , noninherited_flags(o.noninherited_flags)
{
}

static StyleRecalcChange diffPseudoStyles(const ComputedStyle& oldStyle, const ComputedStyle& newStyle)
{
    // If the pseudoStyles have changed, we want any StyleRecalcChange that is not NoChange
    // because setStyle will do the right thing with anything else.
    if (!oldStyle.hasAnyPublicPseudoStyles())
        return NoChange;
    for (PseudoId pseudoId = FIRST_PUBLIC_PSEUDOID; pseudoId < FIRST_INTERNAL_PSEUDOID; pseudoId = static_cast<PseudoId>(pseudoId + 1)) {
        if (!oldStyle.hasPseudoStyle(pseudoId))
            continue;
        const ComputedStyle* newPseudoStyle = newStyle.getCachedPseudoStyle(pseudoId);
        if (!newPseudoStyle)
            return NoInherit;
        const ComputedStyle* oldPseudoStyle = oldStyle.getCachedPseudoStyle(pseudoId);
        if (oldPseudoStyle && *oldPseudoStyle != *newPseudoStyle)
            return NoInherit;
    }
    return NoChange;
}

StyleRecalcChange ComputedStyle::stylePropagationDiff(const ComputedStyle* oldStyle, const ComputedStyle* newStyle)
{
    if ((!oldStyle && newStyle) || (oldStyle && !newStyle))
        return Reattach;

    if (!oldStyle && !newStyle)
        return NoChange;

    if (oldStyle->display() != newStyle->display()
        || oldStyle->hasPseudoStyle(FIRST_LETTER) != newStyle->hasPseudoStyle(FIRST_LETTER)
        || !oldStyle->contentDataEquivalent(newStyle)
        || oldStyle->hasTextCombine() != newStyle->hasTextCombine()
        || oldStyle->justifyItems() != newStyle->justifyItems()
        || oldStyle->alignItems() != newStyle->alignItems())
        return Reattach;

    if (oldStyle->inheritedNotEqual(*newStyle))
        return Inherit;

    if (*oldStyle == *newStyle)
        return diffPseudoStyles(*oldStyle, *newStyle);

    if (oldStyle->hasExplicitlyInheritedProperties())
        return Inherit;

    return NoInherit;
}

ItemPosition ComputedStyle::resolveAlignment(const ComputedStyle& parentStyle, const ComputedStyle& childStyle, ItemPosition resolvedAutoPositionForLayoutObject)
{
    // The auto keyword computes to the parent's align-items computed value, or to "stretch", if not set or "auto".
    if (childStyle.alignSelfPosition() == ItemPositionAuto)
        return (parentStyle.alignItemsPosition() == ItemPositionAuto) ? resolvedAutoPositionForLayoutObject : parentStyle.alignItemsPosition();
    return childStyle.alignSelfPosition();
}

ItemPosition ComputedStyle::resolveJustification(const ComputedStyle& parentStyle, const ComputedStyle& childStyle, ItemPosition resolvedAutoPositionForLayoutObject)
{
    if (childStyle.justifySelfPosition() == ItemPositionAuto)
        return (parentStyle.justifyItemsPosition() == ItemPositionAuto) ? resolvedAutoPositionForLayoutObject : parentStyle.justifyItemsPosition();
    return childStyle.justifySelfPosition();
}

void ComputedStyle::inheritFrom(const ComputedStyle& inheritParent, IsAtShadowBoundary isAtShadowBoundary)
{
    if (isAtShadowBoundary == AtShadowBoundary) {
        // Even if surrounding content is user-editable, shadow DOM should act as a single unit, and not necessarily be editable
        EUserModify currentUserModify = userModify();
        rareInheritedData = inheritParent.rareInheritedData;
        setUserModify(currentUserModify);
    } else {
        rareInheritedData = inheritParent.rareInheritedData;
    }
    inherited = inheritParent.inherited;
    inherited_flags = inheritParent.inherited_flags;
    if (m_svgStyle != inheritParent.m_svgStyle)
        m_svgStyle.access()->inheritFrom(inheritParent.m_svgStyle.get());
}

void ComputedStyle::copyNonInheritedFromCached(const ComputedStyle& other)
{
    m_box = other.m_box;
    visual = other.visual;
    m_background = other.m_background;
    surround = other.surround;
    rareNonInheritedData = other.rareNonInheritedData;

    // The flags are copied one-by-one because noninherited_flags contains a bunch of stuff other than real style data.
    // See comments for each skipped flag below.
    noninherited_flags.effectiveDisplay = other.noninherited_flags.effectiveDisplay;
    noninherited_flags.originalDisplay = other.noninherited_flags.originalDisplay;
    noninherited_flags.overflowX = other.noninherited_flags.overflowX;
    noninherited_flags.overflowY = other.noninherited_flags.overflowY;
    noninherited_flags.verticalAlign = other.noninherited_flags.verticalAlign;
    noninherited_flags.clear = other.noninherited_flags.clear;
    noninherited_flags.position = other.noninherited_flags.position;
    noninherited_flags.floating = other.noninherited_flags.floating;
    noninherited_flags.tableLayout = other.noninherited_flags.tableLayout;
    noninherited_flags.unicodeBidi = other.noninherited_flags.unicodeBidi;
    noninherited_flags.hasViewportUnits = other.noninherited_flags.hasViewportUnits;
    noninherited_flags.pageBreakBefore = other.noninherited_flags.pageBreakBefore;
    noninherited_flags.pageBreakAfter = other.noninherited_flags.pageBreakAfter;
    noninherited_flags.pageBreakInside = other.noninherited_flags.pageBreakInside;

    // Correctly set during selector matching:
    // noninherited_flags.styleType
    // noninherited_flags.pseudoBits

    // Set correctly while computing style for children:
    // noninherited_flags.explicitInheritance

    // unique() styles are not cacheable.
    ASSERT(!other.noninherited_flags.unique);

    // The following flags are set during matching before we decide that we get a
    // match in the MatchedPropertiesCache which in turn calls this method. The
    // reason why we don't copy these flags is that they're already correctly set
    // and that they may differ between elements which have the same set of matched
    // properties. For instance, given the rule:
    //
    // :-webkit-any(:hover, :focus) { background-color: green }"
    //
    // A hovered element, and a focused element may use the same cached matched
    // properties here, but the affectedBy flags will be set differently based on
    // the matching order of the :-webkit-any components.
    //
    // noninherited_flags.emptyState
    // noninherited_flags.affectedByFocus
    // noninherited_flags.affectedByHover
    // noninherited_flags.affectedByActive
    // noninherited_flags.affectedByDrag
    // noninherited_flags.isLink

    if (m_svgStyle != other.m_svgStyle)
        m_svgStyle.access()->copyNonInheritedFromCached(other.m_svgStyle.get());
    ASSERT(zoom() == initialZoom());
}

bool ComputedStyle::operator==(const ComputedStyle& o) const
{
    // compare everything except the pseudoStyle pointer
    return inherited_flags == o.inherited_flags
        && noninherited_flags == o.noninherited_flags
        && m_box == o.m_box
        && visual == o.visual
        && m_background == o.m_background
        && surround == o.surround
        && rareNonInheritedData == o.rareNonInheritedData
        && rareInheritedData == o.rareInheritedData
        && inherited == o.inherited
        && m_svgStyle == o.m_svgStyle;
}

bool ComputedStyle::isStyleAvailable() const
{
    return this != StyleResolver::styleNotYetAvailable();
}

bool ComputedStyle::hasUniquePseudoStyle() const
{
    if (!m_cachedPseudoStyles || styleType() != NOPSEUDO)
        return false;

    for (size_t i = 0; i < m_cachedPseudoStyles->size(); ++i) {
        const ComputedStyle& pseudoStyle = *m_cachedPseudoStyles->at(i);
        if (pseudoStyle.unique())
            return true;
    }

    return false;
}

ComputedStyle* ComputedStyle::getCachedPseudoStyle(PseudoId pid) const
{
    if (!m_cachedPseudoStyles || !m_cachedPseudoStyles->size())
        return 0;

    if (styleType() != NOPSEUDO)
        return 0;

    for (size_t i = 0; i < m_cachedPseudoStyles->size(); ++i) {
        ComputedStyle* pseudoStyle = m_cachedPseudoStyles->at(i).get();
        if (pseudoStyle->styleType() == pid)
            return pseudoStyle;
    }

    return 0;
}

ComputedStyle* ComputedStyle::addCachedPseudoStyle(PassRefPtr<ComputedStyle> pseudo)
{
    if (!pseudo)
        return 0;

    ASSERT(pseudo->styleType() > NOPSEUDO);

    ComputedStyle* result = pseudo.get();

    if (!m_cachedPseudoStyles)
        m_cachedPseudoStyles = adoptPtr(new PseudoStyleCache);

    m_cachedPseudoStyles->append(pseudo);

    return result;
}

void ComputedStyle::removeCachedPseudoStyle(PseudoId pid)
{
    if (!m_cachedPseudoStyles)
        return;
    for (size_t i = 0; i < m_cachedPseudoStyles->size(); ++i) {
        ComputedStyle* pseudoStyle = m_cachedPseudoStyles->at(i).get();
        if (pseudoStyle->styleType() == pid) {
            m_cachedPseudoStyles->remove(i);
            return;
        }
    }
}

bool ComputedStyle::inheritedNotEqual(const ComputedStyle& other) const
{
    return inherited_flags != other.inherited_flags
        || inherited != other.inherited
        || font().loadingCustomFonts() != other.font().loadingCustomFonts()
        || m_svgStyle->inheritedNotEqual(other.m_svgStyle.get())
        || rareInheritedData != other.rareInheritedData;
}

bool ComputedStyle::inheritedDataShared(const ComputedStyle& other) const
{
    // This is a fast check that only looks if the data structures are shared.
    return inherited_flags == other.inherited_flags
        && inherited.get() == other.inherited.get()
        && m_svgStyle.get() == other.m_svgStyle.get()
        && rareInheritedData.get() == other.rareInheritedData.get();
}

static bool dependenceOnContentHeightHasChanged(const ComputedStyle& a, const ComputedStyle& b)
{
    // If top or bottom become auto/non-auto then it means we either have to solve height based
    // on the content or stop doing so (http://www.w3.org/TR/CSS2/visudet.html#abs-non-replaced-height)
    // - either way requires a layout.
    return a.logicalTop().isAuto() != b.logicalTop().isAuto() || a.logicalBottom().isAuto() != b.logicalBottom().isAuto();
}

StyleDifference ComputedStyle::visualInvalidationDiff(const ComputedStyle& other) const
{
    // Note, we use .get() on each DataRef below because DataRef::operator== will do a deep
    // compare, which is duplicate work when we're going to compare each property inside
    // this function anyway.

    StyleDifference diff;
    if (m_svgStyle.get() != other.m_svgStyle.get())
        diff = m_svgStyle->diff(other.m_svgStyle.get());

    if ((!diff.needsFullLayout() || !diff.needsPaintInvalidation()) && diffNeedsFullLayoutAndPaintInvalidation(other)) {
        diff.setNeedsFullLayout();
        diff.setNeedsPaintInvalidationObject();
    }

    if (!diff.needsFullLayout() && diffNeedsFullLayout(other))
        diff.setNeedsFullLayout();

    if (!diff.needsFullLayout() && surround->margin != other.surround->margin) {
        // Relative-positioned elements collapse their margins so need a full layout.
        if (hasOutOfFlowPosition())
            diff.setNeedsPositionedMovementLayout();
        else
            diff.setNeedsFullLayout();
    }

    if (!diff.needsFullLayout() && position() != StaticPosition && surround->offset != other.surround->offset) {
        // Optimize for the case where a positioned layer is moving but not changing size.
        if (dependenceOnContentHeightHasChanged(*this, other))
            diff.setNeedsFullLayout();
        else
            diff.setNeedsPositionedMovementLayout();
    }

    if (diffNeedsPaintInvalidationLayer(other))
        diff.setNeedsPaintInvalidationLayer();
    else if (diffNeedsPaintInvalidationObject(other))
        diff.setNeedsPaintInvalidationObject();

    updatePropertySpecificDifferences(other, diff);

    // Cursors are not checked, since they will be set appropriately in response to mouse events,
    // so they don't need to cause any paint invalidation or layout.

    // Animations don't need to be checked either. We always set the new style on the layoutObject, so we will get a chance to fire off
    // the resulting transition properly.

    return diff;
}

bool ComputedStyle::diffNeedsFullLayoutAndPaintInvalidation(const ComputedStyle& other) const
{
    // FIXME: Not all cases in this method need both full layout and paint invalidation.
    // Should move cases into diffNeedsFullLayout() if
    // - don't need paint invalidation at all;
    // - or the layoutObject knows how to exactly invalidate paints caused by the layout change
    //   instead of forced full paint invalidation.

    if (surround.get() != other.surround.get()) {
        // If our border widths change, then we need to layout. Other changes to borders only necessitate a paint invalidation.
        if (borderLeftWidth() != other.borderLeftWidth()
            || borderTopWidth() != other.borderTopWidth()
            || borderBottomWidth() != other.borderBottomWidth()
            || borderRightWidth() != other.borderRightWidth())
            return true;
    }

    if (rareNonInheritedData.get() != other.rareNonInheritedData.get()) {
        if (rareNonInheritedData->m_appearance != other.rareNonInheritedData->m_appearance
            || rareNonInheritedData->marginBeforeCollapse != other.rareNonInheritedData->marginBeforeCollapse
            || rareNonInheritedData->marginAfterCollapse != other.rareNonInheritedData->marginAfterCollapse
            || rareNonInheritedData->lineClamp != other.rareNonInheritedData->lineClamp
            || rareNonInheritedData->textOverflow != other.rareNonInheritedData->textOverflow
            || rareNonInheritedData->m_wrapFlow != other.rareNonInheritedData->m_wrapFlow
            || rareNonInheritedData->m_wrapThrough != other.rareNonInheritedData->m_wrapThrough
            || rareNonInheritedData->m_shapeMargin != other.rareNonInheritedData->m_shapeMargin
            || rareNonInheritedData->m_order != other.rareNonInheritedData->m_order
            || rareNonInheritedData->m_grid.get() != other.rareNonInheritedData->m_grid.get()
            || rareNonInheritedData->m_gridItem.get() != other.rareNonInheritedData->m_gridItem.get()
            || rareNonInheritedData->hasFilters() != other.rareNonInheritedData->hasFilters())
            return true;

        if (rareNonInheritedData->m_deprecatedFlexibleBox.get() != other.rareNonInheritedData->m_deprecatedFlexibleBox.get()
            && *rareNonInheritedData->m_deprecatedFlexibleBox.get() != *other.rareNonInheritedData->m_deprecatedFlexibleBox.get())
            return true;

        if (rareNonInheritedData->m_flexibleBox.get() != other.rareNonInheritedData->m_flexibleBox.get()
            && *rareNonInheritedData->m_flexibleBox.get() != *other.rareNonInheritedData->m_flexibleBox.get())
            return true;

        // FIXME: We should add an optimized form of layout that just recomputes visual overflow.
        if (!rareNonInheritedData->shadowDataEquivalent(*other.rareNonInheritedData.get()))
            return true;

        if (!rareNonInheritedData->reflectionDataEquivalent(*other.rareNonInheritedData.get()))
            return true;

        if (rareNonInheritedData->m_multiCol.get() != other.rareNonInheritedData->m_multiCol.get()
            && *rareNonInheritedData->m_multiCol.get() != *other.rareNonInheritedData->m_multiCol.get())
            return true;

        // If the counter directives change, trigger a relayout to re-calculate counter values and rebuild the counter node tree.
        const CounterDirectiveMap* mapA = rareNonInheritedData->m_counterDirectives.get();
        const CounterDirectiveMap* mapB = other.rareNonInheritedData->m_counterDirectives.get();
        if (!(mapA == mapB || (mapA && mapB && *mapA == *mapB)))
            return true;

        // We only need do layout for opacity changes if adding or losing opacity could trigger a change
        // in us being a stacking context.
        if (hasAutoZIndex() != other.hasAutoZIndex() && rareNonInheritedData->hasOpacity() != other.rareNonInheritedData->hasOpacity()) {
            // FIXME: We would like to use SimplifiedLayout here, but we can't quite do that yet.
            // We need to make sure SimplifiedLayout can operate correctly on LayoutInlines (we will need
            // to add a selfNeedsSimplifiedLayout bit in order to not get confused and taint every line).
            // In addition we need to solve the floating object issue when layers come and go. Right now
            // a full layout is necessary to keep floating object lists sane.
            return true;
        }
    }

    if (rareInheritedData.get() != other.rareInheritedData.get()) {
        if (rareInheritedData->highlight != other.rareInheritedData->highlight
            || rareInheritedData->indent != other.rareInheritedData->indent
            || rareInheritedData->m_textAlignLast != other.rareInheritedData->m_textAlignLast
            || rareInheritedData->m_textIndentLine != other.rareInheritedData->m_textIndentLine
            || rareInheritedData->m_effectiveZoom != other.rareInheritedData->m_effectiveZoom
            || rareInheritedData->wordBreak != other.rareInheritedData->wordBreak
            || rareInheritedData->overflowWrap != other.rareInheritedData->overflowWrap
            || rareInheritedData->lineBreak != other.rareInheritedData->lineBreak
            || rareInheritedData->textSecurity != other.rareInheritedData->textSecurity
            || rareInheritedData->hyphens != other.rareInheritedData->hyphens
            || rareInheritedData->hyphenationLimitBefore != other.rareInheritedData->hyphenationLimitBefore
            || rareInheritedData->hyphenationLimitAfter != other.rareInheritedData->hyphenationLimitAfter
            || rareInheritedData->hyphenationString != other.rareInheritedData->hyphenationString
            || rareInheritedData->locale != other.rareInheritedData->locale
            || rareInheritedData->m_rubyPosition != other.rareInheritedData->m_rubyPosition
            || rareInheritedData->textEmphasisMark != other.rareInheritedData->textEmphasisMark
            || rareInheritedData->textEmphasisPosition != other.rareInheritedData->textEmphasisPosition
            || rareInheritedData->textEmphasisCustomMark != other.rareInheritedData->textEmphasisCustomMark
            || rareInheritedData->m_textJustify != other.rareInheritedData->m_textJustify
            || rareInheritedData->m_textOrientation != other.rareInheritedData->m_textOrientation
            || rareInheritedData->m_textCombine != other.rareInheritedData->m_textCombine
            || rareInheritedData->m_tabSize != other.rareInheritedData->m_tabSize
            || rareInheritedData->m_lineBoxContain != other.rareInheritedData->m_lineBoxContain
            || rareInheritedData->listStyleImage != other.rareInheritedData->listStyleImage
            || rareInheritedData->textStrokeWidth != other.rareInheritedData->textStrokeWidth)
            return true;

        if (!rareInheritedData->shadowDataEquivalent(*other.rareInheritedData.get()))
            return true;

        if (!rareInheritedData->quotesDataEquivalent(*other.rareInheritedData.get()))
            return true;
    }

    if (inherited->textAutosizingMultiplier != other.inherited->textAutosizingMultiplier)
        return true;

    if (inherited->font.loadingCustomFonts() != other.inherited->font.loadingCustomFonts())
        return true;

    if (inherited.get() != other.inherited.get()) {
        if (inherited->line_height != other.inherited->line_height
            || inherited->font != other.inherited->font
            || inherited->horizontal_border_spacing != other.inherited->horizontal_border_spacing
            || inherited->vertical_border_spacing != other.inherited->vertical_border_spacing)
            return true;
    }

    if (inherited_flags._box_direction != other.inherited_flags._box_direction
        || inherited_flags.m_rtlOrdering != other.inherited_flags.m_rtlOrdering
        || inherited_flags._text_align != other.inherited_flags._text_align
        || inherited_flags._text_transform != other.inherited_flags._text_transform
        || inherited_flags._direction != other.inherited_flags._direction
        || inherited_flags._white_space != other.inherited_flags._white_space
        || inherited_flags.m_writingMode != other.inherited_flags.m_writingMode)
        return true;

    if (noninherited_flags.overflowX != other.noninherited_flags.overflowX
        || noninherited_flags.overflowY != other.noninherited_flags.overflowY
        || noninherited_flags.clear != other.noninherited_flags.clear
        || noninherited_flags.unicodeBidi != other.noninherited_flags.unicodeBidi
        || noninherited_flags.floating != other.noninherited_flags.floating
        || noninherited_flags.originalDisplay != other.noninherited_flags.originalDisplay)
        return true;

    if (noninherited_flags.effectiveDisplay >= FIRST_TABLE_DISPLAY && noninherited_flags.effectiveDisplay <= LAST_TABLE_DISPLAY) {
        if (inherited_flags._border_collapse != other.inherited_flags._border_collapse
            || inherited_flags._empty_cells != other.inherited_flags._empty_cells
            || inherited_flags._caption_side != other.inherited_flags._caption_side
            || noninherited_flags.tableLayout != other.noninherited_flags.tableLayout)
            return true;

        // In the collapsing border model, 'hidden' suppresses other borders, while 'none'
        // does not, so these style differences can be width differences.
        if (inherited_flags._border_collapse
            && ((borderTopStyle() == BHIDDEN && other.borderTopStyle() == BNONE)
                || (borderTopStyle() == BNONE && other.borderTopStyle() == BHIDDEN)
                || (borderBottomStyle() == BHIDDEN && other.borderBottomStyle() == BNONE)
                || (borderBottomStyle() == BNONE && other.borderBottomStyle() == BHIDDEN)
                || (borderLeftStyle() == BHIDDEN && other.borderLeftStyle() == BNONE)
                || (borderLeftStyle() == BNONE && other.borderLeftStyle() == BHIDDEN)
                || (borderRightStyle() == BHIDDEN && other.borderRightStyle() == BNONE)
                || (borderRightStyle() == BNONE && other.borderRightStyle() == BHIDDEN)))
            return true;
    } else if (noninherited_flags.effectiveDisplay == LIST_ITEM) {
        if (inherited_flags._list_style_type != other.inherited_flags._list_style_type
            || inherited_flags._list_style_position != other.inherited_flags._list_style_position)
            return true;
    }

    if ((visibility() == COLLAPSE) != (other.visibility() == COLLAPSE))
        return true;

    if (!m_background->outline().visuallyEqual(other.m_background->outline())) {
        // FIXME: We only really need to recompute the overflow but we don't have an optimized layout for it.
        return true;
    }

    if (hasPseudoStyle(SCROLLBAR) != other.hasPseudoStyle(SCROLLBAR))
        return true;

    // Movement of non-static-positioned object is special cased in ComputedStyle::visualInvalidationDiff().

    return false;
}

bool ComputedStyle::diffNeedsFullLayout(const ComputedStyle& other) const
{
    if (m_box.get() != other.m_box.get()) {
        if (m_box->width() != other.m_box->width()
            || m_box->minWidth() != other.m_box->minWidth()
            || m_box->maxWidth() != other.m_box->maxWidth()
            || m_box->height() != other.m_box->height()
            || m_box->minHeight() != other.m_box->minHeight()
            || m_box->maxHeight() != other.m_box->maxHeight())
            return true;

        if (m_box->verticalAlign() != other.m_box->verticalAlign())
            return true;

        if (m_box->boxSizing() != other.m_box->boxSizing())
            return true;
    }

    if (noninherited_flags.verticalAlign != other.noninherited_flags.verticalAlign
        || noninherited_flags.position != other.noninherited_flags.position)
        return true;

    if (surround.get() != other.surround.get()) {
        if (surround->padding != other.surround->padding)
            return true;
    }

    if (rareNonInheritedData.get() != other.rareNonInheritedData.get()) {
        if (rareNonInheritedData->m_alignContent != other.rareNonInheritedData->m_alignContent
            || rareNonInheritedData->m_alignItems != other.rareNonInheritedData->m_alignItems
            || rareNonInheritedData->m_alignSelf != other.rareNonInheritedData->m_alignSelf
            || rareNonInheritedData->m_justifyContent != other.rareNonInheritedData->m_justifyContent
            || rareNonInheritedData->m_justifyItems != other.rareNonInheritedData->m_justifyItems
            || rareNonInheritedData->m_justifySelf != other.rareNonInheritedData->m_justifySelf)
            return true;
    }

    return false;
}

bool ComputedStyle::diffNeedsPaintInvalidationLayer(const ComputedStyle& other) const
{
    if (position() != StaticPosition && (visual->clip != other.visual->clip || visual->hasAutoClip != other.visual->hasAutoClip))
        return true;

    if (rareNonInheritedData.get() != other.rareNonInheritedData.get()) {
        if (RuntimeEnabledFeatures::cssCompositingEnabled()
            && (rareNonInheritedData->m_effectiveBlendMode != other.rareNonInheritedData->m_effectiveBlendMode
                || rareNonInheritedData->m_isolation != other.rareNonInheritedData->m_isolation))
            return true;

        if (rareNonInheritedData->m_mask != other.rareNonInheritedData->m_mask
            || rareNonInheritedData->m_maskBoxImage != other.rareNonInheritedData->m_maskBoxImage)
            return true;
    }

    return false;
}

bool ComputedStyle::diffNeedsPaintInvalidationObject(const ComputedStyle& other) const
{
    if (inherited_flags._visibility != other.inherited_flags._visibility
        || inherited_flags.m_printColorAdjust != other.inherited_flags.m_printColorAdjust
        || inherited_flags._insideLink != other.inherited_flags._insideLink
        || !surround->border.visuallyEqual(other.surround->border)
        || !m_background->visuallyEqual(*other.m_background))
        return true;

    if (rareInheritedData.get() != other.rareInheritedData.get()) {
        if (rareInheritedData->userModify != other.rareInheritedData->userModify
            || rareInheritedData->userSelect != other.rareInheritedData->userSelect
            || rareInheritedData->m_imageRendering != other.rareInheritedData->m_imageRendering)
            return true;
    }

    if (rareNonInheritedData.get() != other.rareNonInheritedData.get()) {
        if (rareNonInheritedData->userDrag != other.rareNonInheritedData->userDrag
            || rareNonInheritedData->m_objectFit != other.rareNonInheritedData->m_objectFit
            || rareNonInheritedData->m_objectPosition != other.rareNonInheritedData->m_objectPosition
            || !rareNonInheritedData->shapeOutsideDataEquivalent(*other.rareNonInheritedData.get())
            || !rareNonInheritedData->clipPathDataEquivalent(*other.rareNonInheritedData.get())
            || (visitedLinkBorderLeftColor() != other.visitedLinkBorderLeftColor() && borderLeftWidth())
            || (visitedLinkBorderRightColor() != other.visitedLinkBorderRightColor() && borderRightWidth())
            || (visitedLinkBorderBottomColor() != other.visitedLinkBorderBottomColor() && borderBottomWidth())
            || (visitedLinkBorderTopColor() != other.visitedLinkBorderTopColor() && borderTopWidth())
            || (visitedLinkOutlineColor() != other.visitedLinkOutlineColor() && outlineWidth())
            || (visitedLinkBackgroundColor() != other.visitedLinkBackgroundColor()))
            return true;
    }

    if (resize() != other.resize())
        return true;

    return false;
}

void ComputedStyle::updatePropertySpecificDifferences(const ComputedStyle& other, StyleDifference& diff) const
{
    // StyleAdjuster has ensured that zIndex is non-auto only if it's applicable.
    if (m_box->zIndex() != other.m_box->zIndex() || m_box->hasAutoZIndex() != other.m_box->hasAutoZIndex())
        diff.setZIndexChanged();

    if (rareNonInheritedData.get() != other.rareNonInheritedData.get()) {
        if (!transformDataEquivalent(other))
            diff.setTransformChanged();

        if (rareNonInheritedData->opacity != other.rareNonInheritedData->opacity)
            diff.setOpacityChanged();

        if (rareNonInheritedData->m_filter != other.rareNonInheritedData->m_filter)
            diff.setFilterChanged();
    }

    if (!diff.needsPaintInvalidation()) {
        if (inherited->color != other.inherited->color
            || inherited->visitedLinkColor != other.inherited->visitedLinkColor
            || inherited_flags.m_textUnderline != other.inherited_flags.m_textUnderline
            || visual->textDecoration != other.visual->textDecoration) {
            diff.setTextOrColorChanged();
        } else if (rareNonInheritedData.get() != other.rareNonInheritedData.get()
            && (rareNonInheritedData->m_textDecorationStyle != other.rareNonInheritedData->m_textDecorationStyle
                || rareNonInheritedData->m_textDecorationColor != other.rareNonInheritedData->m_textDecorationColor
                || rareNonInheritedData->m_visitedLinkTextDecorationColor != other.rareNonInheritedData->m_visitedLinkTextDecorationColor)) {
                diff.setTextOrColorChanged();
        } else if (rareInheritedData.get() != other.rareInheritedData.get()
            && (rareInheritedData->textFillColor() != other.rareInheritedData->textFillColor()
                || rareInheritedData->textStrokeColor() != other.rareInheritedData->textStrokeColor()
                || rareInheritedData->textEmphasisColor() != other.rareInheritedData->textEmphasisColor()
                || rareInheritedData->visitedLinkTextFillColor() != other.rareInheritedData->visitedLinkTextFillColor()
                || rareInheritedData->visitedLinkTextStrokeColor() != other.rareInheritedData->visitedLinkTextStrokeColor()
                || rareInheritedData->visitedLinkTextEmphasisColor() != other.rareInheritedData->visitedLinkTextEmphasisColor()
                || rareInheritedData->textEmphasisFill != other.rareInheritedData->textEmphasisFill
                || rareInheritedData->appliedTextDecorations != other.rareInheritedData->appliedTextDecorations)) {
                diff.setTextOrColorChanged();
        }
    }
}

void ComputedStyle::addCursor(PassRefPtr<StyleImage> image, bool hotSpotSpecified, const IntPoint& hotSpot)
{
    if (!rareInheritedData.access()->cursorData)
        rareInheritedData.access()->cursorData = CursorList::create();
    rareInheritedData.access()->cursorData->append(CursorData(image, hotSpotSpecified, hotSpot));
}

void ComputedStyle::setCursorList(PassRefPtr<CursorList> other)
{
    rareInheritedData.access()->cursorData = other;
}

void ComputedStyle::setQuotes(PassRefPtr<QuotesData> q)
{
    rareInheritedData.access()->quotes = q;
}

void ComputedStyle::clearCursorList()
{
    if (rareInheritedData->cursorData)
        rareInheritedData.access()->cursorData = nullptr;
}

void ComputedStyle::addCallbackSelector(const String& selector)
{
    if (!rareNonInheritedData->m_callbackSelectors.contains(selector))
        rareNonInheritedData.access()->m_callbackSelectors.append(selector);
}

void ComputedStyle::clearContent()
{
    if (rareNonInheritedData->m_content)
        rareNonInheritedData.access()->m_content = nullptr;
}

void ComputedStyle::appendContent(PassOwnPtr<ContentData> contentData)
{
    OwnPtr<ContentData>& content = rareNonInheritedData.access()->m_content;
    ContentData* lastContent = content.get();
    while (lastContent && lastContent->next())
        lastContent = lastContent->next();

    if (lastContent)
        lastContent->setNext(contentData);
    else
        content = contentData;
}

void ComputedStyle::setContent(PassRefPtr<StyleImage> image, bool add)
{
    if (!image)
        return;

    if (add) {
        appendContent(ContentData::create(image));
        return;
    }

    rareNonInheritedData.access()->m_content = ContentData::create(image);
}

void ComputedStyle::setContent(const String& string, bool add)
{
    OwnPtr<ContentData>& content = rareNonInheritedData.access()->m_content;
    if (add) {
        ContentData* lastContent = content.get();
        while (lastContent && lastContent->next())
            lastContent = lastContent->next();

        if (lastContent) {
            // We attempt to merge with the last ContentData if possible.
            if (lastContent->isText()) {
                TextContentData* textContent = toTextContentData(lastContent);
                textContent->setText(textContent->text() + string);
            } else {
                lastContent->setNext(ContentData::create(string));
            }

            return;
        }
    }

    content = ContentData::create(string);
}

void ComputedStyle::setContent(PassOwnPtr<CounterContent> counter, bool add)
{
    if (!counter)
        return;

    if (add) {
        appendContent(ContentData::create(counter));
        return;
    }

    rareNonInheritedData.access()->m_content = ContentData::create(counter);
}

void ComputedStyle::setContent(QuoteType quote, bool add)
{
    if (add) {
        appendContent(ContentData::create(quote));
        return;
    }

    rareNonInheritedData.access()->m_content = ContentData::create(quote);
}

bool ComputedStyle::hasWillChangeCompositingHint() const
{
    for (size_t i = 0; i < rareNonInheritedData->m_willChange->m_properties.size(); ++i) {
        switch (rareNonInheritedData->m_willChange->m_properties[i]) {
        case CSSPropertyOpacity:
        case CSSPropertyTransform:
        case CSSPropertyAliasWebkitTransform:
        case CSSPropertyTop:
        case CSSPropertyLeft:
        case CSSPropertyBottom:
        case CSSPropertyRight:
            return true;
        default:
            break;
        }
    }
    return false;
}

bool ComputedStyle::requireTransformOrigin(ApplyTransformOrigin applyOrigin, ApplyMotionPath applyMotionPath) const
{
    const Vector<RefPtr<TransformOperation>>& transformOperations = transform().operations();

    // transform-origin brackets the transform with translate operations.
    // Optimize for the case where the only transform is a translation, since the transform-origin is irrelevant
    // in that case.
    if (applyOrigin != IncludeTransformOrigin)
        return false;

    if (applyMotionPath == IncludeMotionPath)
        return true;

    unsigned size = transformOperations.size();
    for (unsigned i = 0; i < size; ++i) {
        TransformOperation::OperationType type = transformOperations[i]->type();
        if (type != TransformOperation::TranslateX
            && type != TransformOperation::TranslateY
            && type != TransformOperation::Translate
            && type != TransformOperation::TranslateZ
            && type != TransformOperation::Translate3D)
            return true;
    }

    return scale() || rotate();
}

void ComputedStyle::applyTransform(TransformationMatrix& result, const LayoutSize& borderBoxSize, ApplyTransformOrigin applyOrigin, ApplyMotionPath applyMotionPath, ApplyIndependentTransformProperties applyIndependentTransformProperties) const
{
    applyTransform(result, FloatRect(FloatPoint(), FloatSize(borderBoxSize)), applyOrigin, applyMotionPath, applyIndependentTransformProperties);
}

void ComputedStyle::applyTransform(TransformationMatrix& result, const FloatRect& boundingBox, ApplyTransformOrigin applyOrigin, ApplyMotionPath applyMotionPath, ApplyIndependentTransformProperties applyIndependentTransformProperties) const
{
    if (!hasMotionPath())
        applyMotionPath = ExcludeMotionPath;
    bool applyTransformOrigin = requireTransformOrigin(applyOrigin, applyMotionPath);

    float offsetX = transformOriginX().type() == Percent ? boundingBox.x() : 0;
    float offsetY = transformOriginY().type() == Percent ? boundingBox.y() : 0;

    float originX = 0;
    float originY = 0;
    float originZ = 0;

    if (applyTransformOrigin) {
        originX = floatValueForLength(transformOriginX(), boundingBox.width()) + offsetX;
        originY = floatValueForLength(transformOriginY(), boundingBox.height()) + offsetY;
        originZ = transformOriginZ();
        result.translate3d(originX, originY, originZ);
    }

    if (applyIndependentTransformProperties == IncludeIndependentTransformProperties) {
        if (translate())
            translate()->apply(result, boundingBox.size());

        if (rotate())
            rotate()->apply(result, boundingBox.size());

        if (scale())
            scale()->apply(result, boundingBox.size());
    }

    if (applyMotionPath == ComputedStyle::IncludeMotionPath)
        applyMotionPathTransform(originX, originY, result);

    const Vector<RefPtr<TransformOperation>>& transformOperations = transform().operations();
    unsigned size = transformOperations.size();
    for (unsigned i = 0; i < size; ++i)
        transformOperations[i]->apply(result, boundingBox.size());

    if (applyTransformOrigin) {
        result.translate3d(-originX, -originY, -originZ);
    }
}

void ComputedStyle::applyMotionPathTransform(float originX, float originY, TransformationMatrix& transform) const
{
    const StyleMotionData& motionData = rareNonInheritedData->m_transform->m_motion;
    ASSERT(motionData.m_path && motionData.m_path->isPathStyleMotionPath());
    const PathStyleMotionPath& motionPath = toPathStyleMotionPath(*motionData.m_path);
    float pathLength = motionPath.length();
    float distance = floatValueForLength(motionData.m_offset, pathLength);
    float computedDistance;
    if (motionPath.isClosed() && pathLength > 0) {
        computedDistance = fmod(distance, pathLength);
        if (computedDistance < 0)
            computedDistance += pathLength;
    } else {
        computedDistance = clampTo<float>(distance, 0, pathLength);
    }

    FloatPoint point;
    float angle;
    if (!motionPath.path().pointAndNormalAtLength(computedDistance, point, angle))
        return;
    if (motionData.m_rotationType == MotionRotationFixed)
        angle = 0;

    transform.translate(point.x() - originX, point.y() - originY);
    transform.rotate(angle + motionData.m_rotation);
}

void ComputedStyle::setTextShadow(PassRefPtr<ShadowList> s)
{
    rareInheritedData.access()->textShadow = s;
}

void ComputedStyle::setBoxShadow(PassRefPtr<ShadowList> s)
{
    rareNonInheritedData.access()->m_boxShadow = s;
}

static FloatRoundedRect::Radii calcRadiiFor(const BorderData& border, LayoutSize size)
{
    return FloatRoundedRect::Radii(
        IntSize(valueForLength(border.topLeft().width(), size.width()),
            valueForLength(border.topLeft().height(), size.height())),
        IntSize(valueForLength(border.topRight().width(), size.width()),
            valueForLength(border.topRight().height(), size.height())),
        IntSize(valueForLength(border.bottomLeft().width(), size.width()),
            valueForLength(border.bottomLeft().height(), size.height())),
        IntSize(valueForLength(border.bottomRight().width(), size.width()),
            valueForLength(border.bottomRight().height(), size.height())));
}

StyleImage* ComputedStyle::listStyleImage() const { return rareInheritedData->listStyleImage.get(); }
void ComputedStyle::setListStyleImage(PassRefPtr<StyleImage> v)
{
    if (rareInheritedData->listStyleImage != v)
        rareInheritedData.access()->listStyleImage = v;
}

Color ComputedStyle::color() const { return inherited->color; }
Color ComputedStyle::visitedLinkColor() const { return inherited->visitedLinkColor; }
void ComputedStyle::setColor(const Color& v) { SET_VAR(inherited, color, v); }
void ComputedStyle::setVisitedLinkColor(const Color& v) { SET_VAR(inherited, visitedLinkColor, v); }

short ComputedStyle::horizontalBorderSpacing() const { return inherited->horizontal_border_spacing; }
short ComputedStyle::verticalBorderSpacing() const { return inherited->vertical_border_spacing; }
void ComputedStyle::setHorizontalBorderSpacing(short v) { SET_VAR(inherited, horizontal_border_spacing, v); }
void ComputedStyle::setVerticalBorderSpacing(short v) { SET_VAR(inherited, vertical_border_spacing, v); }

FloatRoundedRect ComputedStyle::getRoundedBorderFor(const LayoutRect& borderRect,
    bool includeLogicalLeftEdge, bool includeLogicalRightEdge) const
{
    FloatRoundedRect roundedRect(pixelSnappedIntRect(borderRect));
    if (hasBorderRadius()) {
        FloatRoundedRect::Radii radii = calcRadiiFor(surround->border, borderRect.size());
        roundedRect.includeLogicalEdges(radii, isHorizontalWritingMode(), includeLogicalLeftEdge, includeLogicalRightEdge);
        roundedRect.constrainRadii();
    }
    return roundedRect;
}

FloatRoundedRect ComputedStyle::getRoundedInnerBorderFor(const LayoutRect& borderRect, bool includeLogicalLeftEdge, bool includeLogicalRightEdge) const
{
    bool horizontal = isHorizontalWritingMode();

    int leftWidth = (!horizontal || includeLogicalLeftEdge) ? borderLeftWidth() : 0;
    int rightWidth = (!horizontal || includeLogicalRightEdge) ? borderRightWidth() : 0;
    int topWidth = (horizontal || includeLogicalLeftEdge) ? borderTopWidth() : 0;
    int bottomWidth = (horizontal || includeLogicalRightEdge) ? borderBottomWidth() : 0;

    return getRoundedInnerBorderFor(borderRect,
        LayoutRectOutsets(-topWidth, -rightWidth, -bottomWidth, -leftWidth),
        includeLogicalLeftEdge, includeLogicalRightEdge);
}

FloatRoundedRect ComputedStyle::getRoundedInnerBorderFor(const LayoutRect& borderRect,
    const LayoutRectOutsets insets, bool includeLogicalLeftEdge, bool includeLogicalRightEdge) const
{
    LayoutRect innerRect(borderRect);
    innerRect.expand(insets);

    FloatRoundedRect roundedRect(pixelSnappedIntRect(innerRect));

    if (hasBorderRadius()) {
        FloatRoundedRect::Radii radii = getRoundedBorderFor(borderRect).radii();
        // Insets use negative values.
        radii.shrink(
            -insets.top().toFloat(),
            -insets.bottom().toFloat(),
            -insets.left().toFloat(),
            -insets.right().toFloat());
        roundedRect.includeLogicalEdges(radii, isHorizontalWritingMode(), includeLogicalLeftEdge, includeLogicalRightEdge);
    }
    return roundedRect;
}

static bool allLayersAreFixed(const FillLayer& layer)
{
    for (const FillLayer* currLayer = &layer; currLayer; currLayer = currLayer->next()) {
        if (!currLayer->image() || currLayer->attachment() != FixedBackgroundAttachment)
            return false;
    }

    return true;
}

bool ComputedStyle::hasEntirelyFixedBackground() const
{
    return allLayersAreFixed(backgroundLayers());
}

const CounterDirectiveMap* ComputedStyle::counterDirectives() const
{
    return rareNonInheritedData->m_counterDirectives.get();
}

CounterDirectiveMap& ComputedStyle::accessCounterDirectives()
{
    OwnPtr<CounterDirectiveMap>& map = rareNonInheritedData.access()->m_counterDirectives;
    if (!map)
        map = adoptPtr(new CounterDirectiveMap);
    return *map;
}

const CounterDirectives ComputedStyle::getCounterDirectives(const AtomicString& identifier) const
{
    if (const CounterDirectiveMap* directives = counterDirectives())
        return directives->get(identifier);
    return CounterDirectives();
}

void ComputedStyle::clearIncrementDirectives()
{
    if (!counterDirectives())
        return;

    // This makes us copy even if we may not be removing any items.
    CounterDirectiveMap& map = accessCounterDirectives();
    typedef CounterDirectiveMap::iterator Iterator;

    Iterator end = map.end();
    for (Iterator it = map.begin(); it != end; ++it)
        it->value.clearIncrement();
}

void ComputedStyle::clearResetDirectives()
{
    if (!counterDirectives())
        return;

    // This makes us copy even if we may not be removing any items.
    CounterDirectiveMap& map = accessCounterDirectives();
    typedef CounterDirectiveMap::iterator Iterator;

    Iterator end = map.end();
    for (Iterator it = map.begin(); it != end; ++it)
        it->value.clearReset();
}

const AtomicString& ComputedStyle::hyphenString() const
{
    const AtomicString& hyphenationString = rareInheritedData.get()->hyphenationString;
    if (!hyphenationString.isNull())
        return hyphenationString;

    // FIXME: This should depend on locale.
    DEFINE_STATIC_LOCAL(AtomicString, hyphenMinusString, (&hyphenMinusCharacter, 1));
    DEFINE_STATIC_LOCAL(AtomicString, hyphenString, (&hyphenCharacter, 1));
    const SimpleFontData* primaryFont = font().primaryFont();
    ASSERT(primaryFont);
    return primaryFont->glyphForCharacter(hyphenCharacter) ? hyphenString : hyphenMinusString;
}

const AtomicString& ComputedStyle::textEmphasisMarkString() const
{
    switch (textEmphasisMark()) {
    case TextEmphasisMarkNone:
        return nullAtom;
    case TextEmphasisMarkCustom:
        return textEmphasisCustomMark();
    case TextEmphasisMarkDot: {
        DEFINE_STATIC_LOCAL(AtomicString, filledDotString, (&bulletCharacter, 1));
        DEFINE_STATIC_LOCAL(AtomicString, openDotString, (&whiteBulletCharacter, 1));
        return textEmphasisFill() == TextEmphasisFillFilled ? filledDotString : openDotString;
    }
    case TextEmphasisMarkCircle: {
        DEFINE_STATIC_LOCAL(AtomicString, filledCircleString, (&blackCircleCharacter, 1));
        DEFINE_STATIC_LOCAL(AtomicString, openCircleString, (&whiteCircleCharacter, 1));
        return textEmphasisFill() == TextEmphasisFillFilled ? filledCircleString : openCircleString;
    }
    case TextEmphasisMarkDoubleCircle: {
        DEFINE_STATIC_LOCAL(AtomicString, filledDoubleCircleString, (&fisheyeCharacter, 1));
        DEFINE_STATIC_LOCAL(AtomicString, openDoubleCircleString, (&bullseyeCharacter, 1));
        return textEmphasisFill() == TextEmphasisFillFilled ? filledDoubleCircleString : openDoubleCircleString;
    }
    case TextEmphasisMarkTriangle: {
        DEFINE_STATIC_LOCAL(AtomicString, filledTriangleString, (&blackUpPointingTriangleCharacter, 1));
        DEFINE_STATIC_LOCAL(AtomicString, openTriangleString, (&whiteUpPointingTriangleCharacter, 1));
        return textEmphasisFill() == TextEmphasisFillFilled ? filledTriangleString : openTriangleString;
    }
    case TextEmphasisMarkSesame: {
        DEFINE_STATIC_LOCAL(AtomicString, filledSesameString, (&sesameDotCharacter, 1));
        DEFINE_STATIC_LOCAL(AtomicString, openSesameString, (&whiteSesameDotCharacter, 1));
        return textEmphasisFill() == TextEmphasisFillFilled ? filledSesameString : openSesameString;
    }
    case TextEmphasisMarkAuto:
        ASSERT_NOT_REACHED();
        return nullAtom;
    }

    ASSERT_NOT_REACHED();
    return nullAtom;
}

CSSAnimationData& ComputedStyle::accessAnimations()
{
    if (!rareNonInheritedData.access()->m_animations)
        rareNonInheritedData.access()->m_animations = CSSAnimationData::create();
    return *rareNonInheritedData->m_animations;
}

CSSTransitionData& ComputedStyle::accessTransitions()
{
    if (!rareNonInheritedData.access()->m_transitions)
        rareNonInheritedData.access()->m_transitions = CSSTransitionData::create();
    return *rareNonInheritedData->m_transitions;
}

const Font& ComputedStyle::font() const { return inherited->font; }
const FontMetrics& ComputedStyle::fontMetrics() const { return inherited->font.fontMetrics(); }
const FontDescription& ComputedStyle::fontDescription() const { return inherited->font.fontDescription(); }
float ComputedStyle::specifiedFontSize() const { return fontDescription().specifiedSize(); }
float ComputedStyle::computedFontSize() const { return fontDescription().computedSize(); }
int ComputedStyle::fontSize() const { return fontDescription().computedPixelSize(); }
float ComputedStyle::fontSizeAdjust() const { return fontDescription().sizeAdjust(); }
bool ComputedStyle::hasFontSizeAdjust() const { return fontDescription().hasSizeAdjust(); }
FontWeight ComputedStyle::fontWeight() const { return fontDescription().weight(); }
FontStretch ComputedStyle::fontStretch() const { return fontDescription().stretch(); }

TextDecoration ComputedStyle::textDecorationsInEffect() const
{
    int decorations = 0;

    const Vector<AppliedTextDecoration>& applied = appliedTextDecorations();

    for (size_t i = 0; i < applied.size(); ++i)
        decorations |= applied[i].line();

    return static_cast<TextDecoration>(decorations);
}

const Vector<AppliedTextDecoration>& ComputedStyle::appliedTextDecorations() const
{
    if (!inherited_flags.m_textUnderline && !rareInheritedData->appliedTextDecorations) {
        DEFINE_STATIC_LOCAL(Vector<AppliedTextDecoration>, empty, ());
        return empty;
    }
    if (inherited_flags.m_textUnderline) {
        DEFINE_STATIC_LOCAL(Vector<AppliedTextDecoration>, underline, (1, AppliedTextDecoration(TextDecorationUnderline)));
        return underline;
    }

    return rareInheritedData->appliedTextDecorations->vector();
}

StyleVariableData* ComputedStyle::variables() const
{
    ASSERT(RuntimeEnabledFeatures::cssVariablesEnabled());
    return rareInheritedData->variables.get();
}

void ComputedStyle::setVariable(const AtomicString& name, PassRefPtr<CSSVariableData> value)
{
    RefPtr<StyleVariableData>& variables = rareInheritedData.access()->variables;
    if (!variables)
        variables = StyleVariableData::create();
    else if (!variables->hasOneRef())
        variables = variables->copy();
    variables->setVariable(name, value);
}

void ComputedStyle::removeVariable(const AtomicString& name)
{
    RefPtr<StyleVariableData>& variables = rareInheritedData.access()->variables;
    if (!variables)
        return;
    if (!variables->hasOneRef())
        variables = variables->copy();
    variables->removeVariable(name);
}

float ComputedStyle::wordSpacing() const { return fontDescription().wordSpacing(); }
float ComputedStyle::letterSpacing() const { return fontDescription().letterSpacing(); }

bool ComputedStyle::setFontDescription(const FontDescription& v)
{
    if (inherited->font.fontDescription() != v) {
        inherited.access()->font = Font(v);
        return true;
    }
    return false;
}

void ComputedStyle::setFont(const Font& font)
{
    inherited.access()->font = font;
}

const Length& ComputedStyle::specifiedLineHeight() const { return inherited->line_height; }
Length ComputedStyle::lineHeight() const
{
    const Length& lh = inherited->line_height;
    // Unlike fontDescription().computedSize() and hence fontSize(), this is
    // recalculated on demand as we only store the specified line height.
    // FIXME: Should consider scaling the fixed part of any calc expressions
    // too, though this involves messily poking into CalcExpressionLength.
    float multiplier = textAutosizingMultiplier();
    if (multiplier > 1 && lh.isFixed())
        return Length(TextAutosizer::computeAutosizedFontSize(lh.value(), multiplier), Fixed);

    return lh;
}

void ComputedStyle::setLineHeight(const Length& specifiedLineHeight) { SET_VAR(inherited, line_height, specifiedLineHeight); }

int ComputedStyle::computedLineHeight() const
{
    const Length& lh = lineHeight();

    // Negative value means the line height is not set. Use the font's built-in spacing.
    if (lh.isNegative())
        return fontMetrics().lineSpacing();

    if (lh.hasPercent())
        return minimumValueForLength(lh, fontSize());

    return std::min(lh.value(), LayoutUnit::max().toFloat());
}

void ComputedStyle::setWordSpacing(float wordSpacing)
{
    FontSelector* currentFontSelector = font().fontSelector();
    FontDescription desc(fontDescription());
    desc.setWordSpacing(wordSpacing);
    setFontDescription(desc);
    font().update(currentFontSelector);
}

void ComputedStyle::setLetterSpacing(float letterSpacing)
{
    FontSelector* currentFontSelector = font().fontSelector();
    FontDescription desc(fontDescription());
    desc.setLetterSpacing(letterSpacing);
    setFontDescription(desc);
    font().update(currentFontSelector);
}

void ComputedStyle::setTextAutosizingMultiplier(float multiplier)
{
    SET_VAR(inherited, textAutosizingMultiplier, multiplier);

    float size = specifiedFontSize();

    ASSERT(std::isfinite(size));
    if (!std::isfinite(size) || size < 0)
        size = 0;
    else
        size = std::min(maximumAllowedFontSize, size);

    FontSelector* currentFontSelector = font().fontSelector();
    FontDescription desc(fontDescription());
    desc.setSpecifiedSize(size);
    desc.setComputedSize(size);

    if (multiplier > 1) {
        float autosizedFontSize = TextAutosizer::computeAutosizedFontSize(size, multiplier);
        desc.setComputedSize(std::min(maximumAllowedFontSize, autosizedFontSize));
    }

    setFontDescription(desc);
    font().update(currentFontSelector);
}

void ComputedStyle::addAppliedTextDecoration(const AppliedTextDecoration& decoration)
{
    RefPtr<AppliedTextDecorationList>& list = rareInheritedData.access()->appliedTextDecorations;

    if (!list)
        list = AppliedTextDecorationList::create();
    else if (!list->hasOneRef())
        list = list->copy();

    if (inherited_flags.m_textUnderline) {
        inherited_flags.m_textUnderline = false;
        list->append(AppliedTextDecoration(TextDecorationUnderline));
    }

    list->append(decoration);
}

void ComputedStyle::applyTextDecorations()
{
    if (textDecoration() == TextDecorationNone)
        return;

    TextDecorationStyle style = textDecorationStyle();
    StyleColor styleColor = decorationColorIncludingFallback(insideLink() == InsideVisitedLink);

    int decorations = textDecoration();

    if (decorations & TextDecorationUnderline) {
        // To save memory, we don't use AppliedTextDecoration objects in the
        // common case of a single simple underline.
        AppliedTextDecoration underline(TextDecorationUnderline, style, styleColor);

        if (!rareInheritedData->appliedTextDecorations && underline.isSimpleUnderline())
            inherited_flags.m_textUnderline = true;
        else
            addAppliedTextDecoration(underline);
    }
    if (decorations & TextDecorationOverline)
        addAppliedTextDecoration(AppliedTextDecoration(TextDecorationOverline, style, styleColor));
    if (decorations & TextDecorationLineThrough)
        addAppliedTextDecoration(AppliedTextDecoration(TextDecorationLineThrough, style, styleColor));
}

void ComputedStyle::clearAppliedTextDecorations()
{
    inherited_flags.m_textUnderline = false;

    if (rareInheritedData->appliedTextDecorations)
        rareInheritedData.access()->appliedTextDecorations = nullptr;
}

void ComputedStyle::clearMultiCol()
{
    rareNonInheritedData.access()->m_multiCol = nullptr;
    rareNonInheritedData.access()->m_multiCol.init();
}

StyleColor ComputedStyle::decorationColorIncludingFallback(bool visitedLink) const
{
    StyleColor styleColor = visitedLink ? visitedLinkTextDecorationColor() : textDecorationColor();

    if (!styleColor.isCurrentColor())
        return styleColor;

    if (textStrokeWidth()) {
        // Prefer stroke color if possible, but not if it's fully transparent.
        StyleColor textStrokeStyleColor = visitedLink ? visitedLinkTextStrokeColor() : textStrokeColor();
        if (!textStrokeStyleColor.isCurrentColor() && textStrokeStyleColor.color().alpha())
            return textStrokeStyleColor;
    }

    return visitedLink ? visitedLinkTextFillColor() : textFillColor();
}

Color ComputedStyle::colorIncludingFallback(int colorProperty, bool visitedLink) const
{
    StyleColor result(StyleColor::currentColor());
    EBorderStyle borderStyle = BNONE;
    switch (colorProperty) {
    case CSSPropertyBackgroundColor:
        result = visitedLink ? visitedLinkBackgroundColor() : backgroundColor();
        break;
    case CSSPropertyBorderLeftColor:
        result = visitedLink ? visitedLinkBorderLeftColor() : borderLeftColor();
        borderStyle = borderLeftStyle();
        break;
    case CSSPropertyBorderRightColor:
        result = visitedLink ? visitedLinkBorderRightColor() : borderRightColor();
        borderStyle = borderRightStyle();
        break;
    case CSSPropertyBorderTopColor:
        result = visitedLink ? visitedLinkBorderTopColor() : borderTopColor();
        borderStyle = borderTopStyle();
        break;
    case CSSPropertyBorderBottomColor:
        result = visitedLink ? visitedLinkBorderBottomColor() : borderBottomColor();
        borderStyle = borderBottomStyle();
        break;
    case CSSPropertyColor:
        result = visitedLink ? visitedLinkColor() : color();
        break;
    case CSSPropertyOutlineColor:
        result = visitedLink ? visitedLinkOutlineColor() : outlineColor();
        break;
    case CSSPropertyWebkitColumnRuleColor:
        result = visitedLink ? visitedLinkColumnRuleColor() : columnRuleColor();
        break;
    case CSSPropertyWebkitTextEmphasisColor:
        result = visitedLink ? visitedLinkTextEmphasisColor() : textEmphasisColor();
        break;
    case CSSPropertyWebkitTextFillColor:
        result = visitedLink ? visitedLinkTextFillColor() : textFillColor();
        break;
    case CSSPropertyWebkitTextStrokeColor:
        result = visitedLink ? visitedLinkTextStrokeColor() : textStrokeColor();
        break;
    case CSSPropertyFloodColor:
        result = floodColor();
        break;
    case CSSPropertyLightingColor:
        result = lightingColor();
        break;
    case CSSPropertyStopColor:
        result = stopColor();
        break;
    case CSSPropertyWebkitTapHighlightColor:
        result = tapHighlightColor();
        break;
    case CSSPropertyTextDecorationColor:
        result = decorationColorIncludingFallback(visitedLink);
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }

    if (!result.isCurrentColor())
        return result.color();

    // FIXME: Treating styled borders with initial color differently causes problems
    // See crbug.com/316559, crbug.com/276231
    if (!visitedLink && (borderStyle == INSET || borderStyle == OUTSET || borderStyle == RIDGE || borderStyle == GROOVE))
        return Color(238, 238, 238);
    return visitedLink ? visitedLinkColor() : color();
}

Color ComputedStyle::visitedDependentColor(int colorProperty) const
{
    Color unvisitedColor = colorIncludingFallback(colorProperty, false);
    if (insideLink() != InsideVisitedLink)
        return unvisitedColor;

    Color visitedColor = colorIncludingFallback(colorProperty, true);

    // FIXME: Technically someone could explicitly specify the color transparent, but for now we'll just
    // assume that if the background color is transparent that it wasn't set. Note that it's weird that
    // we're returning unvisited info for a visited link, but given our restriction that the alpha values
    // have to match, it makes more sense to return the unvisited background color if specified than it
    // does to return black. This behavior matches what Firefox 4 does as well.
    if (colorProperty == CSSPropertyBackgroundColor && visitedColor == Color::transparent)
        return unvisitedColor;

    // Take the alpha from the unvisited color, but get the RGB values from the visited color.
    return Color(visitedColor.red(), visitedColor.green(), visitedColor.blue(), unvisitedColor.alpha());
}

const BorderValue& ComputedStyle::borderBefore() const
{
    switch (writingMode()) {
    case TopToBottomWritingMode:
        return borderTop();
    case BottomToTopWritingMode:
        return borderBottom();
    case LeftToRightWritingMode:
        return borderLeft();
    case RightToLeftWritingMode:
        return borderRight();
    }
    ASSERT_NOT_REACHED();
    return borderTop();
}

const BorderValue& ComputedStyle::borderAfter() const
{
    switch (writingMode()) {
    case TopToBottomWritingMode:
        return borderBottom();
    case BottomToTopWritingMode:
        return borderTop();
    case LeftToRightWritingMode:
        return borderRight();
    case RightToLeftWritingMode:
        return borderLeft();
    }
    ASSERT_NOT_REACHED();
    return borderBottom();
}

const BorderValue& ComputedStyle::borderStart() const
{
    if (isHorizontalWritingMode())
        return isLeftToRightDirection() ? borderLeft() : borderRight();
    return isLeftToRightDirection() ? borderTop() : borderBottom();
}

const BorderValue& ComputedStyle::borderEnd() const
{
    if (isHorizontalWritingMode())
        return isLeftToRightDirection() ? borderRight() : borderLeft();
    return isLeftToRightDirection() ? borderBottom() : borderTop();
}

int ComputedStyle::borderBeforeWidth() const
{
    switch (writingMode()) {
    case TopToBottomWritingMode:
        return borderTopWidth();
    case BottomToTopWritingMode:
        return borderBottomWidth();
    case LeftToRightWritingMode:
        return borderLeftWidth();
    case RightToLeftWritingMode:
        return borderRightWidth();
    }
    ASSERT_NOT_REACHED();
    return borderTopWidth();
}

int ComputedStyle::borderAfterWidth() const
{
    switch (writingMode()) {
    case TopToBottomWritingMode:
        return borderBottomWidth();
    case BottomToTopWritingMode:
        return borderTopWidth();
    case LeftToRightWritingMode:
        return borderRightWidth();
    case RightToLeftWritingMode:
        return borderLeftWidth();
    }
    ASSERT_NOT_REACHED();
    return borderBottomWidth();
}

int ComputedStyle::borderStartWidth() const
{
    if (isHorizontalWritingMode())
        return isLeftToRightDirection() ? borderLeftWidth() : borderRightWidth();
    return isLeftToRightDirection() ? borderTopWidth() : borderBottomWidth();
}

int ComputedStyle::borderEndWidth() const
{
    if (isHorizontalWritingMode())
        return isLeftToRightDirection() ? borderRightWidth() : borderLeftWidth();
    return isLeftToRightDirection() ? borderBottomWidth() : borderTopWidth();
}

void ComputedStyle::setMarginStart(const Length& margin)
{
    if (isHorizontalWritingMode()) {
        if (isLeftToRightDirection())
            setMarginLeft(margin);
        else
            setMarginRight(margin);
    } else {
        if (isLeftToRightDirection())
            setMarginTop(margin);
        else
            setMarginBottom(margin);
    }
}

void ComputedStyle::setMarginEnd(const Length& margin)
{
    if (isHorizontalWritingMode()) {
        if (isLeftToRightDirection())
            setMarginRight(margin);
        else
            setMarginLeft(margin);
    } else {
        if (isLeftToRightDirection())
            setMarginBottom(margin);
        else
            setMarginTop(margin);
    }
}

void ComputedStyle::setMotionPath(PassRefPtr<StyleMotionPath> path)
{
    ASSERT(path);
    rareNonInheritedData.access()->m_transform.access()->m_motion.m_path = path;
}

void ComputedStyle::resetMotionPath()
{
    rareNonInheritedData.access()->m_transform.access()->m_motion.m_path = nullptr;
}

int ComputedStyle::outlineOutset() const
{
    if (!hasOutline())
        return 0;
    if (outlineStyleIsAuto())
        return GraphicsContext::focusRingOutsetExtent(outlineOffset(), outlineWidth());
    return outlineSize();
}

bool ComputedStyle::columnRuleEquivalent(const ComputedStyle* otherStyle) const
{
    return columnRuleStyle() == otherStyle->columnRuleStyle()
        && columnRuleWidth() == otherStyle->columnRuleWidth()
        && visitedDependentColor(CSSPropertyWebkitColumnRuleColor) == otherStyle->visitedDependentColor(CSSPropertyWebkitColumnRuleColor);
}

TextEmphasisMark ComputedStyle::textEmphasisMark() const
{
    TextEmphasisMark mark = static_cast<TextEmphasisMark>(rareInheritedData->textEmphasisMark);
    if (mark != TextEmphasisMarkAuto)
        return mark;

    if (isHorizontalWritingMode())
        return TextEmphasisMarkDot;

    return TextEmphasisMarkSesame;
}

Color ComputedStyle::initialTapHighlightColor()
{
    return LayoutTheme::tapHighlightColor();
}

#if ENABLE(OILPAN)
const FilterOperations& ComputedStyle::initialFilter()
{
    DEFINE_STATIC_LOCAL(Persistent<FilterOperationsWrapper>, ops, (FilterOperationsWrapper::create()));
    return ops->operations();
}
#endif

LayoutRectOutsets ComputedStyle::imageOutsets(const NinePieceImage& image) const
{
    return LayoutRectOutsets(
        NinePieceImage::computeOutset(image.outset().top(), borderTopWidth()),
        NinePieceImage::computeOutset(image.outset().right(), borderRightWidth()),
        NinePieceImage::computeOutset(image.outset().bottom(), borderBottomWidth()),
        NinePieceImage::computeOutset(image.outset().left(), borderLeftWidth()));
}

void ComputedStyle::setBorderImageSource(PassRefPtr<StyleImage> image)
{
    if (surround->border.m_image.image() == image.get())
        return;
    surround.access()->border.m_image.setImage(image);
}

void ComputedStyle::setBorderImageSlices(const LengthBox& slices)
{
    if (surround->border.m_image.imageSlices() == slices)
        return;
    surround.access()->border.m_image.setImageSlices(slices);
}

void ComputedStyle::setBorderImageSlicesFill(bool fill)
{
    if (surround->border.m_image.fill() == fill)
        return;
    surround.access()->border.m_image.setFill(fill);
}

void ComputedStyle::setBorderImageWidth(const BorderImageLengthBox& slices)
{
    if (surround->border.m_image.borderSlices() == slices)
        return;
    surround.access()->border.m_image.setBorderSlices(slices);
}

void ComputedStyle::setBorderImageOutset(const BorderImageLengthBox& outset)
{
    if (surround->border.m_image.outset() == outset)
        return;
    surround.access()->border.m_image.setOutset(outset);
}

bool ComputedStyle::borderObscuresBackground() const
{
    if (!hasBorder())
        return false;

    // Bail if we have any border-image for now. We could look at the image alpha to improve this.
    if (borderImage().image())
        return false;

    BorderEdge edges[4];
    getBorderEdgeInfo(edges);

    for (int i = BSTop; i <= BSLeft; ++i) {
        const BorderEdge& currEdge = edges[i];
        if (!currEdge.obscuresBackground())
            return false;
    }

    return true;
}

void ComputedStyle::getBorderEdgeInfo(BorderEdge edges[], bool includeLogicalLeftEdge, bool includeLogicalRightEdge) const
{
    bool horizontal = isHorizontalWritingMode();

    edges[BSTop] = BorderEdge(LayoutUnit(borderTopWidth()),
        visitedDependentColor(CSSPropertyBorderTopColor),
        borderTopStyle(),
        horizontal || includeLogicalLeftEdge);

    edges[BSRight] = BorderEdge(LayoutUnit(borderRightWidth()),
        visitedDependentColor(CSSPropertyBorderRightColor),
        borderRightStyle(),
        !horizontal || includeLogicalRightEdge);

    edges[BSBottom] = BorderEdge(LayoutUnit(borderBottomWidth()),
        visitedDependentColor(CSSPropertyBorderBottomColor),
        borderBottomStyle(),
        horizontal || includeLogicalRightEdge);

    edges[BSLeft] = BorderEdge(LayoutUnit(borderLeftWidth()),
        visitedDependentColor(CSSPropertyBorderLeftColor),
        borderLeftStyle(),
        !horizontal || includeLogicalLeftEdge);
}

void ComputedStyle::copyChildDependentFlagsFrom(const ComputedStyle& other)
{
    setEmptyState(other.emptyState());
    if (other.hasExplicitlyInheritedProperties())
        setHasExplicitlyInheritedProperties();
}

//////////////////////////////////////////////////////////////////////////

void ComputedStyle::setLeft(const Length& v) { SET_VAR(surround, offset.m_left, v); }
void ComputedStyle::setRight(const Length& v) { SET_VAR(surround, offset.m_right, v); }
void ComputedStyle::setTop(const Length& v) { SET_VAR(surround, offset.m_top, v); }
void ComputedStyle::setBottom(const Length& v) { SET_VAR(surround, offset.m_bottom, v); }

void ComputedStyle::setWidth(const Length& v)
{
    SET_VAR(m_box, m_width, v);
}

void ComputedStyle::setHeight(const Length& v) { SET_VAR(m_box, m_height, v); }

void ComputedStyle::setLogicalWidth(const Length& v)
{
    if (isHorizontalWritingMode()) {
        SET_VAR(m_box, m_width, v);
    } else {
        SET_VAR(m_box, m_height, v);
    }
}

void ComputedStyle::setLogicalHeight(const Length& v)
{
    if (isHorizontalWritingMode()) {
        SET_VAR(m_box, m_height, v);
    } else {
        SET_VAR(m_box, m_width, v);
    }
}

void ComputedStyle::setMinWidth(const Length& v) { SET_VAR(m_box, m_minWidth, v); }
void ComputedStyle::setMaxWidth(const Length& v) { SET_VAR(m_box, m_maxWidth, v); }
void ComputedStyle::setMinHeight(const Length& v) { SET_VAR(m_box, m_minHeight, v); }
void ComputedStyle::setMaxHeight(const Length& v) { SET_VAR(m_box, m_maxHeight, v); }

} // namespace blink

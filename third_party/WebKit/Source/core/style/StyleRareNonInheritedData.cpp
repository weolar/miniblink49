/*
 * Copyright (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
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
#include "core/style/StyleRareNonInheritedData.h"

#include "core/style/ContentData.h"
#include "core/style/DataEquivalency.h"
#include "core/style/ComputedStyle.h"
#include "core/style/ShadowList.h"
#include "core/style/StyleFilterData.h"
#include "core/style/StyleTransformData.h"
#include "core/layout/svg/ReferenceFilterBuilder.h"

namespace blink {

class SameSizeStyleRareNonInheritedData : public RefCounted<StyleRareNonInheritedData> {
public:
    float floats[3];
    int integers;

    LengthPoint lengthPoints[2];
    LineClampValue lineClamps;
    DraggableRegionMode draggableRegions;

    void* dataRefs[9];
    void* ownPtrs[4];
    void* refPtrs[4];

    FillLayer fillLayers;
    NinePieceImage ninePieces;
    FloatSize floatSize;
    Length lengths;

    StyleColor styleColors[8];

    Vector<String> m_callbackSelectors;

    StyleContentAlignmentData contentAlignmentData[2];
    StyleSelfAlignmentData selfAlignmentData[4];

    unsigned m_bitFields[2];
};

static_assert(sizeof(StyleRareNonInheritedData) == sizeof(SameSizeStyleRareNonInheritedData), "StyleRareNonInheritedData_should_stay_small");

StyleRareNonInheritedData::StyleRareNonInheritedData()
    : opacity(ComputedStyle::initialOpacity())
    , m_perspective(ComputedStyle::initialPerspective())
    , m_shapeImageThreshold(ComputedStyle::initialShapeImageThreshold())
    , m_order(ComputedStyle::initialOrder())
    , m_perspectiveOrigin(ComputedStyle::initialPerspectiveOrigin())
    , m_objectPosition(ComputedStyle::initialObjectPosition())
    , lineClamp(ComputedStyle::initialLineClamp())
    , m_draggableRegionMode(DraggableRegionNone)
    , m_shapeOutside(ComputedStyle::initialShapeOutside())
    , m_clipPath(ComputedStyle::initialClipPath())
    , m_mask(MaskFillLayer, true)
    , m_pageSize()
    , m_shapeMargin(ComputedStyle::initialShapeMargin())
    , m_textDecorationColor(StyleColor::currentColor())
    , m_visitedLinkTextDecorationColor(StyleColor::currentColor())
    , m_visitedLinkBackgroundColor(ComputedStyle::initialBackgroundColor())
    , m_visitedLinkOutlineColor(StyleColor::currentColor())
    , m_visitedLinkBorderLeftColor(StyleColor::currentColor())
    , m_visitedLinkBorderRightColor(StyleColor::currentColor())
    , m_visitedLinkBorderTopColor(StyleColor::currentColor())
    , m_visitedLinkBorderBottomColor(StyleColor::currentColor())
    , m_alignContent(ComputedStyle::initialContentAlignment())
    , m_alignItems(ComputedStyle::initialSelfAlignment())
    , m_alignSelf(ComputedStyle::initialSelfAlignment())
    , m_justifyContent(ComputedStyle::initialContentAlignment())
    , m_justifyItems(ComputedStyle::initialSelfAlignment())
    , m_justifySelf(ComputedStyle::initialSelfAlignment())
    , m_pageSizeType(PAGE_SIZE_AUTO)
    , m_transformStyle3D(ComputedStyle::initialTransformStyle3D())
    , m_backfaceVisibility(ComputedStyle::initialBackfaceVisibility())
    , userDrag(ComputedStyle::initialUserDrag())
    , textOverflow(ComputedStyle::initialTextOverflow())
    , marginBeforeCollapse(MCOLLAPSE)
    , marginAfterCollapse(MCOLLAPSE)
    , m_appearance(ComputedStyle::initialAppearance())
    , m_textDecorationStyle(ComputedStyle::initialTextDecorationStyle())
    , m_wrapFlow(ComputedStyle::initialWrapFlow())
    , m_wrapThrough(ComputedStyle::initialWrapThrough())
    , m_hasCurrentOpacityAnimation(false)
    , m_hasCurrentTransformAnimation(false)
    , m_hasCurrentFilterAnimation(false)
    , m_runningOpacityAnimationOnCompositor(false)
    , m_runningTransformAnimationOnCompositor(false)
    , m_runningFilterAnimationOnCompositor(false)
    , m_effectiveBlendMode(ComputedStyle::initialBlendMode())
    , m_touchAction(ComputedStyle::initialTouchAction())
    , m_objectFit(ComputedStyle::initialObjectFit())
    , m_isolation(ComputedStyle::initialIsolation())
    , m_scrollBehavior(ComputedStyle::initialScrollBehavior())
    , m_scrollBlocksOn(ComputedStyle::initialScrollBlocksOn())
    , m_scrollSnapType(ComputedStyle::initialScrollSnapType())
    , m_requiresAcceleratedCompositingForExternalReasons(false)
    , m_hasInlineTransform(false)
    , m_resize(ComputedStyle::initialResize())
    , m_hasCompositorProxy(false)
{
    m_maskBoxImage.setMaskDefaults();
}

StyleRareNonInheritedData::StyleRareNonInheritedData(const StyleRareNonInheritedData& o)
    : RefCounted<StyleRareNonInheritedData>()
    , opacity(o.opacity)
    , m_perspective(o.m_perspective)
    , m_shapeImageThreshold(o.m_shapeImageThreshold)
    , m_order(o.m_order)
    , m_perspectiveOrigin(o.m_perspectiveOrigin)
    , m_objectPosition(o.m_objectPosition)
    , lineClamp(o.lineClamp)
    , m_draggableRegionMode(o.m_draggableRegionMode)
    , m_deprecatedFlexibleBox(o.m_deprecatedFlexibleBox)
    , m_flexibleBox(o.m_flexibleBox)
    , m_multiCol(o.m_multiCol)
    , m_transform(o.m_transform)
    , m_willChange(o.m_willChange)
    , m_filter(o.m_filter)
    , m_grid(o.m_grid)
    , m_gridItem(o.m_gridItem)
    , m_scrollSnap(o.m_scrollSnap)
    , m_content(o.m_content ? o.m_content->clone() : nullptr)
    , m_counterDirectives(o.m_counterDirectives ? clone(*o.m_counterDirectives) : nullptr)
    , m_animations(o.m_animations ? CSSAnimationData::create(*o.m_animations) : nullptr)
    , m_transitions(o.m_transitions ? CSSTransitionData::create(*o.m_transitions) : nullptr)
    , m_boxShadow(o.m_boxShadow)
    , m_boxReflect(o.m_boxReflect)
    , m_shapeOutside(o.m_shapeOutside)
    , m_clipPath(o.m_clipPath)
    , m_mask(o.m_mask)
    , m_maskBoxImage(o.m_maskBoxImage)
    , m_pageSize(o.m_pageSize)
    , m_shapeMargin(o.m_shapeMargin)
    , m_textDecorationColor(o.m_textDecorationColor)
    , m_visitedLinkTextDecorationColor(o.m_visitedLinkTextDecorationColor)
    , m_visitedLinkBackgroundColor(o.m_visitedLinkBackgroundColor)
    , m_visitedLinkOutlineColor(o.m_visitedLinkOutlineColor)
    , m_visitedLinkBorderLeftColor(o.m_visitedLinkBorderLeftColor)
    , m_visitedLinkBorderRightColor(o.m_visitedLinkBorderRightColor)
    , m_visitedLinkBorderTopColor(o.m_visitedLinkBorderTopColor)
    , m_visitedLinkBorderBottomColor(o.m_visitedLinkBorderBottomColor)
    , m_alignContent(o.m_alignContent)
    , m_alignItems(o.m_alignItems)
    , m_alignSelf(o.m_alignSelf)
    , m_justifyContent(o.m_justifyContent)
    , m_justifyItems(o.m_justifyItems)
    , m_justifySelf(o.m_justifySelf)
    , m_pageSizeType(o.m_pageSizeType)
    , m_transformStyle3D(o.m_transformStyle3D)
    , m_backfaceVisibility(o.m_backfaceVisibility)
    , userDrag(o.userDrag)
    , textOverflow(o.textOverflow)
    , marginBeforeCollapse(o.marginBeforeCollapse)
    , marginAfterCollapse(o.marginAfterCollapse)
    , m_appearance(o.m_appearance)
    , m_textDecorationStyle(o.m_textDecorationStyle)
    , m_wrapFlow(o.m_wrapFlow)
    , m_wrapThrough(o.m_wrapThrough)
    , m_hasCurrentOpacityAnimation(o.m_hasCurrentOpacityAnimation)
    , m_hasCurrentTransformAnimation(o.m_hasCurrentTransformAnimation)
    , m_hasCurrentFilterAnimation(o.m_hasCurrentFilterAnimation)
    , m_runningOpacityAnimationOnCompositor(o.m_runningOpacityAnimationOnCompositor)
    , m_runningTransformAnimationOnCompositor(o.m_runningTransformAnimationOnCompositor)
    , m_runningFilterAnimationOnCompositor(o.m_runningFilterAnimationOnCompositor)
    , m_effectiveBlendMode(o.m_effectiveBlendMode)
    , m_touchAction(o.m_touchAction)
    , m_objectFit(o.m_objectFit)
    , m_isolation(o.m_isolation)
    , m_scrollBehavior(o.m_scrollBehavior)
    , m_scrollBlocksOn(o.m_scrollBlocksOn)
    , m_scrollSnapType(o.m_scrollSnapType)
    , m_requiresAcceleratedCompositingForExternalReasons(o.m_requiresAcceleratedCompositingForExternalReasons)
    , m_hasInlineTransform(o.m_hasInlineTransform)
    , m_resize(o.m_resize)
    , m_hasCompositorProxy(o.m_hasCompositorProxy)
{
}

StyleRareNonInheritedData::~StyleRareNonInheritedData()
{
    const FilterOperations& filterOperations = m_filter->m_operations;
    for (unsigned i = 0; i < filterOperations.size(); ++i)
        ReferenceFilterBuilder::clearDocumentResourceReference(filterOperations.at(i));
}

bool StyleRareNonInheritedData::operator==(const StyleRareNonInheritedData& o) const
{
    return opacity == o.opacity
        && m_perspective == o.m_perspective
        && m_shapeImageThreshold == o.m_shapeImageThreshold
        && m_order == o.m_order
        && m_perspectiveOrigin == o.m_perspectiveOrigin
        && m_objectPosition == o.m_objectPosition
        && lineClamp == o.lineClamp
        && m_draggableRegionMode == o.m_draggableRegionMode
        && m_deprecatedFlexibleBox == o.m_deprecatedFlexibleBox
        && m_flexibleBox == o.m_flexibleBox
        && m_multiCol == o.m_multiCol
        && m_transform == o.m_transform
        && m_willChange == o.m_willChange
        && m_filter == o.m_filter
        && m_grid == o.m_grid
        && m_gridItem == o.m_gridItem
        && m_scrollSnap == o.m_scrollSnap
        && contentDataEquivalent(o)
        && counterDataEquivalent(o)
        && shadowDataEquivalent(o)
        && reflectionDataEquivalent(o)
        && animationDataEquivalent(o)
        && transitionDataEquivalent(o)
        && shapeOutsideDataEquivalent(o)
        && m_mask == o.m_mask
        && m_maskBoxImage == o.m_maskBoxImage
        && m_pageSize == o.m_pageSize
        && m_shapeMargin == o.m_shapeMargin
        && clipPathDataEquivalent(o)
        && m_textDecorationColor == o.m_textDecorationColor
        && m_visitedLinkTextDecorationColor == o.m_visitedLinkTextDecorationColor
        && m_visitedLinkBackgroundColor == o.m_visitedLinkBackgroundColor
        && m_visitedLinkOutlineColor == o.m_visitedLinkOutlineColor
        && m_visitedLinkBorderLeftColor == o.m_visitedLinkBorderLeftColor
        && m_visitedLinkBorderRightColor == o.m_visitedLinkBorderRightColor
        && m_visitedLinkBorderTopColor == o.m_visitedLinkBorderTopColor
        && m_visitedLinkBorderBottomColor == o.m_visitedLinkBorderBottomColor
        && m_callbackSelectors == o.m_callbackSelectors
        && m_alignContent == o.m_alignContent
        && m_alignItems == o.m_alignItems
        && m_alignSelf == o.m_alignSelf
        && m_justifyContent == o.m_justifyContent
        && m_justifyItems == o.m_justifyItems
        && m_justifySelf == o.m_justifySelf
        && m_pageSizeType == o.m_pageSizeType
        && m_transformStyle3D == o.m_transformStyle3D
        && m_backfaceVisibility == o.m_backfaceVisibility
        && userDrag == o.userDrag
        && textOverflow == o.textOverflow
        && marginBeforeCollapse == o.marginBeforeCollapse
        && marginAfterCollapse == o.marginAfterCollapse
        && m_appearance == o.m_appearance
        && m_textDecorationStyle == o.m_textDecorationStyle
        && m_wrapFlow == o.m_wrapFlow
        && m_wrapThrough == o.m_wrapThrough
        && m_hasCurrentOpacityAnimation == o.m_hasCurrentOpacityAnimation
        && m_hasCurrentTransformAnimation == o.m_hasCurrentTransformAnimation
        && m_hasCurrentFilterAnimation == o.m_hasCurrentFilterAnimation
        && m_effectiveBlendMode == o.m_effectiveBlendMode
        && m_touchAction == o.m_touchAction
        && m_objectFit == o.m_objectFit
        && m_isolation == o.m_isolation
        && m_scrollBehavior == o.m_scrollBehavior
        && m_scrollBlocksOn == o.m_scrollBlocksOn
        && m_scrollBlocksOn == o.m_scrollSnapType
        && m_requiresAcceleratedCompositingForExternalReasons == o.m_requiresAcceleratedCompositingForExternalReasons
        && m_hasInlineTransform == o.m_hasInlineTransform
        && m_resize == o.m_resize
        && m_hasCompositorProxy == o.m_hasCompositorProxy;
}

bool StyleRareNonInheritedData::contentDataEquivalent(const StyleRareNonInheritedData& o) const
{
    ContentData* a = m_content.get();
    ContentData* b = o.m_content.get();

    while (a && b && *a == *b) {
        a = a->next();
        b = b->next();
    }

    return !a && !b;
}

bool StyleRareNonInheritedData::counterDataEquivalent(const StyleRareNonInheritedData& o) const
{
    return dataEquivalent(m_counterDirectives, o.m_counterDirectives);
}

bool StyleRareNonInheritedData::shadowDataEquivalent(const StyleRareNonInheritedData& o) const
{
    return dataEquivalent(m_boxShadow, o.m_boxShadow);
}

bool StyleRareNonInheritedData::reflectionDataEquivalent(const StyleRareNonInheritedData& o) const
{
    return dataEquivalent(m_boxReflect, o.m_boxReflect);
}

bool StyleRareNonInheritedData::animationDataEquivalent(const StyleRareNonInheritedData& o) const
{
    if (!m_animations && !o.m_animations)
        return true;
    if (!m_animations || !o.m_animations)
        return false;
    return m_animations->animationsMatchForStyleRecalc(*o.m_animations);
}

bool StyleRareNonInheritedData::transitionDataEquivalent(const StyleRareNonInheritedData& o) const
{
    if (!m_transitions && !o.m_transitions)
        return true;
    if (!m_transitions || !o.m_transitions)
        return false;
    return m_transitions->transitionsMatchForStyleRecalc(*o.m_transitions);
}

bool StyleRareNonInheritedData::hasFilters() const
{
    return m_filter.get() && !m_filter->m_operations.isEmpty();
}

bool StyleRareNonInheritedData::shapeOutsideDataEquivalent(const StyleRareNonInheritedData& o) const
{
    return dataEquivalent(m_shapeOutside, o.m_shapeOutside);
}

bool StyleRareNonInheritedData::clipPathDataEquivalent(const StyleRareNonInheritedData& o) const
{
    return dataEquivalent(m_clipPath, o.m_clipPath);
}

} // namespace blink

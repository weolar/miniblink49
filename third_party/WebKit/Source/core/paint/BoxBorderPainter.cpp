// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/BoxBorderPainter.h"

#include "core/paint/BoxPainter.h"
#include "core/paint/PaintInfo.h"
#include "core/style/BorderEdge.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/GraphicsContextStateSaver.h"
#include "wtf/Vector.h"
#include <algorithm>

namespace blink {

namespace {

enum BorderEdgeFlag {
    TopBorderEdge = 1 << BSTop,
    RightBorderEdge = 1 << BSRight,
    BottomBorderEdge = 1 << BSBottom,
    LeftBorderEdge = 1 << BSLeft,
    AllBorderEdges = TopBorderEdge | BottomBorderEdge | LeftBorderEdge | RightBorderEdge
};

inline BorderEdgeFlag edgeFlagForSide(BoxSide side)
{
    return static_cast<BorderEdgeFlag>(1 << side);
}

inline bool includesEdge(BorderEdgeFlags flags, BoxSide side)
{
    return flags & edgeFlagForSide(side);
}

inline bool includesAdjacentEdges(BorderEdgeFlags flags)
{
    // The set includes adjacent edges iff it contains at least one horizontal and one vertical edge.
    return (flags & (TopBorderEdge | BottomBorderEdge))
        && (flags & (LeftBorderEdge | RightBorderEdge));
}

inline bool styleRequiresClipPolygon(EBorderStyle style)
{
    // These are drawn with a stroke, so we have to clip to get corner miters.
    return style == DOTTED || style == DASHED;
}

inline bool borderStyleFillsBorderArea(EBorderStyle style)
{
    return !(style == DOTTED || style == DASHED || style == DOUBLE);
}

inline bool borderStyleHasInnerDetail(EBorderStyle style)
{
    return style == GROOVE || style == RIDGE || style == DOUBLE;
}

inline bool borderStyleIsDottedOrDashed(EBorderStyle style)
{
    return style == DOTTED || style == DASHED;
}

// OUTSET darkens the bottom and right (and maybe lightens the top and left)
// INSET darkens the top and left (and maybe lightens the bottom and right)
inline bool borderStyleHasUnmatchedColorsAtCorner(EBorderStyle style, BoxSide side, BoxSide adjacentSide)
{
    // These styles match at the top/left and bottom/right.
    if (style == INSET || style == GROOVE || style == RIDGE || style == OUTSET) {
        const BorderEdgeFlags topRightFlags = edgeFlagForSide(BSTop) | edgeFlagForSide(BSRight);
        const BorderEdgeFlags bottomLeftFlags = edgeFlagForSide(BSBottom) | edgeFlagForSide(BSLeft);

        BorderEdgeFlags flags = edgeFlagForSide(side) | edgeFlagForSide(adjacentSide);
        return flags == topRightFlags || flags == bottomLeftFlags;
    }
    return false;
}

inline bool colorsMatchAtCorner(BoxSide side, BoxSide adjacentSide, const BorderEdge edges[])
{
    if (!edges[adjacentSide].shouldRender())
        return false;

    if (!edges[side].sharesColorWith(edges[adjacentSide]))
        return false;

    return !borderStyleHasUnmatchedColorsAtCorner(edges[side].borderStyle(), side, adjacentSide);
}

inline bool borderWillArcInnerEdge(const FloatSize& firstRadius, const FloatSize& secondRadius)
{
    return !firstRadius.isZero() || !secondRadius.isZero();
}

inline bool willOverdraw(BoxSide side, EBorderStyle style, BorderEdgeFlags completedEdges)
{
    // If we're done with this side, it will obviously not overdraw any portion of the current edge.
    if (includesEdge(completedEdges, side))
        return false;

    // The side is still to be drawn. It overdraws the current edge iff it has a solid fill style.
    return borderStyleFillsBorderArea(style);
}

inline bool borderStylesRequireMiter(BoxSide side, BoxSide adjacentSide, EBorderStyle style, EBorderStyle adjacentStyle)
{
    if (style == DOUBLE || adjacentStyle == DOUBLE || adjacentStyle == GROOVE || adjacentStyle == RIDGE)
        return true;

    if (borderStyleIsDottedOrDashed(style) != borderStyleIsDottedOrDashed(adjacentStyle))
        return true;

    if (style != adjacentStyle)
        return true;

    return borderStyleHasUnmatchedColorsAtCorner(style, side, adjacentSide);
}

FloatRect calculateSideRect(const FloatRoundedRect& outerBorder, const BorderEdge& edge, int side)
{
    FloatRect sideRect = outerBorder.rect();
    int width = edge.width;

    if (side == BSTop)
        sideRect.setHeight(width);
    else if (side == BSBottom)
        sideRect.shiftYEdgeTo(sideRect.maxY() - width);
    else if (side == BSLeft)
        sideRect.setWidth(width);
    else
        sideRect.shiftXEdgeTo(sideRect.maxX() - width);

    return sideRect;
}

FloatRect calculateSideRectIncludingInner(const FloatRoundedRect& outerBorder, const BorderEdge edges[], BoxSide side)
{
    FloatRect sideRect = outerBorder.rect();
    int width;

    switch (side) {
    case BSTop:
        width = sideRect.height() - edges[BSBottom].width;
        sideRect.setHeight(width);
        break;
    case BSBottom:
        width = sideRect.height() - edges[BSTop].width;
        sideRect.shiftYEdgeTo(sideRect.maxY() - width);
        break;
    case BSLeft:
        width = sideRect.width() - edges[BSRight].width;
        sideRect.setWidth(width);
        break;
    case BSRight:
        width = sideRect.width() - edges[BSLeft].width;
        sideRect.shiftXEdgeTo(sideRect.maxX() - width);
        break;
    }

    return sideRect;
}

FloatRoundedRect calculateAdjustedInnerBorder(const FloatRoundedRect& innerBorder, BoxSide side)
{
    // Expand the inner border as necessary to make it a rounded rect (i.e. radii contained within each edge).
    // This function relies on the fact we only get radii not contained within each edge if one of the radii
    // for an edge is zero, so we can shift the arc towards the zero radius corner.
    FloatRoundedRect::Radii newRadii = innerBorder.radii();
    FloatRect newRect = innerBorder.rect();

    float overshoot;
    float maxRadii;

    switch (side) {
    case BSTop:
        overshoot = newRadii.topLeft().width() + newRadii.topRight().width() - newRect.width();
        // FIXME: once we start pixel-snapping rounded rects after this point, the overshoot concept
        // should disappear.
        if (overshoot > 0.1) {
            newRect.setWidth(newRect.width() + overshoot);
            if (!newRadii.topLeft().width())
                newRect.move(-overshoot, 0);
        }
        newRadii.setBottomLeft(IntSize(0, 0));
        newRadii.setBottomRight(IntSize(0, 0));
        maxRadii = std::max(newRadii.topLeft().height(), newRadii.topRight().height());
        if (maxRadii > newRect.height())
            newRect.setHeight(maxRadii);
        break;

    case BSBottom:
        overshoot = newRadii.bottomLeft().width() + newRadii.bottomRight().width() - newRect.width();
        if (overshoot > 0.1) {
            newRect.setWidth(newRect.width() + overshoot);
            if (!newRadii.bottomLeft().width())
                newRect.move(-overshoot, 0);
        }
        newRadii.setTopLeft(IntSize(0, 0));
        newRadii.setTopRight(IntSize(0, 0));
        maxRadii = std::max(newRadii.bottomLeft().height(), newRadii.bottomRight().height());
        if (maxRadii > newRect.height()) {
            newRect.move(0, newRect.height() - maxRadii);
            newRect.setHeight(maxRadii);
        }
        break;

    case BSLeft:
        overshoot = newRadii.topLeft().height() + newRadii.bottomLeft().height() - newRect.height();
        if (overshoot > 0.1) {
            newRect.setHeight(newRect.height() + overshoot);
            if (!newRadii.topLeft().height())
                newRect.move(0, -overshoot);
        }
        newRadii.setTopRight(IntSize(0, 0));
        newRadii.setBottomRight(IntSize(0, 0));
        maxRadii = std::max(newRadii.topLeft().width(), newRadii.bottomLeft().width());
        if (maxRadii > newRect.width())
            newRect.setWidth(maxRadii);
        break;

    case BSRight:
        overshoot = newRadii.topRight().height() + newRadii.bottomRight().height() - newRect.height();
        if (overshoot > 0.1) {
            newRect.setHeight(newRect.height() + overshoot);
            if (!newRadii.topRight().height())
                newRect.move(0, -overshoot);
        }
        newRadii.setTopLeft(IntSize(0, 0));
        newRadii.setBottomLeft(IntSize(0, 0));
        maxRadii = std::max(newRadii.topRight().width(), newRadii.bottomRight().width());
        if (maxRadii > newRect.width()) {
            newRect.move(newRect.width() - maxRadii, 0);
            newRect.setWidth(maxRadii);
        }
        break;
    }

    return FloatRoundedRect(newRect, newRadii);
}

LayoutRectOutsets doubleStripeInsets(const BorderEdge edges[], BorderEdge::DoubleBorderStripe stripe)
{
    // Insets are representes as negative outsets.
    return LayoutRectOutsets(
        -edges[BSTop].getDoubleBorderStripeWidth(stripe),
        -edges[BSRight].getDoubleBorderStripeWidth(stripe),
        -edges[BSBottom].getDoubleBorderStripeWidth(stripe),
        -edges[BSLeft].getDoubleBorderStripeWidth(stripe));
}

void drawSolidBorderRect(GraphicsContext* context, const FloatRect& borderRect,
    float borderWidth, const Color& color)
{
    FloatRect strokeRect(borderRect);
    strokeRect.inflate(-borderWidth / 2);

    bool wasAntialias = context->shouldAntialias();
    if (!wasAntialias)
        context->setShouldAntialias(true);

    context->setStrokeStyle(SolidStroke);
    context->setStrokeColor(color);
    context->strokeRect(strokeRect, borderWidth);

    if (!wasAntialias)
        context->setShouldAntialias(false);
}

void drawBleedAdjustedDRRect(GraphicsContext* context, BackgroundBleedAvoidance bleedAvoidance,
    const FloatRoundedRect& outer, const FloatRoundedRect& inner, Color color)
{
    switch (bleedAvoidance) {
    case BackgroundBleedClipLayer: {
        // BackgroundBleedClipLayer clips the outer rrect for the whole layer. Based on this,
        // we can avoid background bleeding by filling the *outside* of inner rrect, all the
        // way to the layer bounds (enclosing int rect for the clip, in device space).
        ASSERT(outer.isRounded());

        SkPath path;
        path.addRRect(inner);
        path.setFillType(SkPath::kInverseWinding_FillType);

        SkPaint paint;
        paint.setColor(color.rgb());
        paint.setStyle(SkPaint::kFill_Style);
        paint.setAntiAlias(true);
        context->drawPath(path, paint);

        break;
    }
    case BackgroundBleedClipOnly:
        if (outer.isRounded()) {
            // BackgroundBleedClipOnly clips the outer rrect corners for us.
            FloatRoundedRect adjustedOuter = outer;
            adjustedOuter.setRadii(FloatRoundedRect::Radii());
            context->fillDRRect(adjustedOuter, inner, color);
            break;
        }
        // fall through
    default:
        context->fillDRRect(outer, inner, color);
        break;
    }
}

bool bleedAvoidanceIsClipping(BackgroundBleedAvoidance bleedAvoidance)
{
    return bleedAvoidance == BackgroundBleedClipOnly || bleedAvoidance == BackgroundBleedClipLayer;
}

// The LUTs below assume specific enum values.
static_assert(BNONE == 0, "unexpected EBorderStyle value");
static_assert(BHIDDEN == 1, "unexpected EBorderStyle value");
static_assert(INSET == 2, "unexpected EBorderStyle value");
static_assert(GROOVE == 3, "unexpected EBorderStyle value");
static_assert(OUTSET == 4, "unexpected EBorderStyle value");
static_assert(RIDGE == 5, "unexpected EBorderStyle value");
static_assert(DOTTED == 6, "unexpected EBorderStyle value");
static_assert(DASHED == 7, "unexpected EBorderStyle value");
static_assert(SOLID == 8, "unexpected EBorderStyle value");
static_assert(DOUBLE == 9, "unexpected EBorderStyle value");

static_assert(BSTop == 0, "unexpected BoxSide value");
static_assert(BSRight == 1, "unexpected BoxSide value");
static_assert(BSBottom == 2, "unexpected BoxSide value");
static_assert(BSLeft == 3, "unexpected BoxSide value");

// Style-based paint order: non-solid edges (dashed/dotted/double) are painted before
// solid edges (inset/outset/groove/ridge/solid) to maximize overdraw opportunities.
const unsigned kStylePriority[] = {
    0 /* BNONE */,
    0 /* BHIDDEN */,
    2 /* INSET */,
    2 /* GROOVE */,
    2 /* OUTSET */,
    2 /* RIDGE */,
    1 /* DOTTED */,
    1 /* DASHED */,
    3 /* SOLID */,
    1 /* DOUBLE */
};

// Given the same style, prefer drawing in non-adjacent order to minimize the number of sides
// which require miters.
const unsigned kSidePriority[] = {
    0, /* BSTop */
    2, /* BSRight */
    1, /* BSBottom */
    3, /* BSLeft */
};

// Edges sharing the same opacity. Stores both a side list and an edge bitfield to support
// constant time iteration + membership tests.
struct OpacityGroup {
    OpacityGroup(unsigned alpha) : edgeFlags(0), alpha(alpha) { }

    Vector<BoxSide, 4> sides;
    BorderEdgeFlags edgeFlags;
    unsigned alpha;
};

} // anonymous namespace

// Holds edges grouped by opacity and sorted in paint order.
struct BoxBorderPainter::ComplexBorderInfo {
    ComplexBorderInfo(const BoxBorderPainter& borderPainter, bool antiAlias)
        : antiAlias(antiAlias)
    {
        Vector<BoxSide, 4> sortedSides;

        // First, collect all visible sides.
        for (unsigned i = borderPainter.m_firstVisibleEdge; i < 4; ++i) {
            BoxSide side = static_cast<BoxSide>(i);

            if (includesEdge(borderPainter.m_visibleEdgeSet, side))
                sortedSides.append(side);
        }
        ASSERT(!sortedSides.isEmpty());

        // Then sort them in paint order, based on three (prioritized) criteria: alpha, style, side.
        std::sort(sortedSides.begin(), sortedSides.end(),
            [&borderPainter] (BoxSide a, BoxSide b) -> bool {
            const BorderEdge& edgeA = borderPainter.m_edges[a];
            const BorderEdge& edgeB = borderPainter.m_edges[b];

            const unsigned alphaA = edgeA.color.alpha();
            const unsigned alphaB = edgeB.color.alpha();
            if (alphaA != alphaB)
                return alphaA < alphaB;

            const unsigned stylePriorityA = kStylePriority[edgeA.borderStyle()];
            const unsigned stylePriorityB = kStylePriority[edgeB.borderStyle()];
            if (stylePriorityA != stylePriorityB)
                return stylePriorityA < stylePriorityB;

            return kSidePriority[a] < kSidePriority[b];
        });

        // Finally, build the opacity group structures.
        buildOpacityGroups(borderPainter, sortedSides);

        if (borderPainter.m_isRounded)
            roundedBorderPath.addRoundedRect(borderPainter.m_outer);
    }

    Vector<OpacityGroup, 4> opacityGroups;

    // Potentially used when drawing rounded borders.
    Path roundedBorderPath;

    bool antiAlias;

private:
    void buildOpacityGroups(const BoxBorderPainter& borderPainter,
        const Vector<BoxSide, 4>& sortedSides)
    {
        unsigned currentAlpha = 0;
        for (BoxSide side : sortedSides) {
            const BorderEdge& edge = borderPainter.m_edges[side];
            const unsigned edgeAlpha = edge.color.alpha();

            ASSERT(edgeAlpha > 0);
            ASSERT(edgeAlpha >= currentAlpha);
            if (edgeAlpha != currentAlpha) {
                opacityGroups.append(OpacityGroup(edgeAlpha));
                currentAlpha = edgeAlpha;
            }

            ASSERT(!opacityGroups.isEmpty());
            OpacityGroup& currentGroup = opacityGroups.last();
            currentGroup.sides.append(side);
            currentGroup.edgeFlags |= edgeFlagForSide(side);
        }

        ASSERT(!opacityGroups.isEmpty());
    }
};

void BoxBorderPainter::drawDoubleBorder(GraphicsContext* context, const LayoutRect& borderRect) const
{
    ASSERT(m_isUniformColor);
    ASSERT(m_isUniformStyle);
    ASSERT(firstEdge().borderStyle() == DOUBLE);
    ASSERT(m_visibleEdgeSet == AllBorderEdges);

    const Color color = firstEdge().color;

    // outer stripe
    const LayoutRectOutsets outerThirdInsets =
        doubleStripeInsets(m_edges, BorderEdge::DoubleBorderStripeOuter);
    const FloatRoundedRect outerThirdRect = m_style.getRoundedInnerBorderFor(borderRect,
        outerThirdInsets, m_includeLogicalLeftEdge, m_includeLogicalRightEdge);
    drawBleedAdjustedDRRect(context, m_bleedAvoidance, m_outer, outerThirdRect, color);

    // inner stripe
    const LayoutRectOutsets innerThirdInsets =
        doubleStripeInsets(m_edges, BorderEdge::DoubleBorderStripeInner);
    const FloatRoundedRect innerThirdRect = m_style.getRoundedInnerBorderFor(borderRect,
        innerThirdInsets, m_includeLogicalLeftEdge, m_includeLogicalRightEdge);
    context->fillDRRect(innerThirdRect, m_inner, color);
}

bool BoxBorderPainter::paintBorderFastPath(GraphicsContext* context, const LayoutRect& borderRect) const
{
    if (!m_isUniformColor || !m_isUniformStyle || !m_inner.isRenderable())
        return false;

    if (firstEdge().borderStyle() != SOLID && firstEdge().borderStyle() != DOUBLE)
        return false;

    if (m_visibleEdgeSet == AllBorderEdges) {
        if (firstEdge().borderStyle() == SOLID) {
            if (m_isUniformWidth && !m_outer.isRounded()) {
                // 4-side, solid, uniform-width, rectangular border => one drawRect()
                drawSolidBorderRect(context, m_outer.rect(), firstEdge().width, firstEdge().color);
            } else {
                // 4-side, solid border => one drawDRRect()
                drawBleedAdjustedDRRect(context, m_bleedAvoidance, m_outer, m_inner, firstEdge().color);
            }
        } else {
            // 4-side, double border => 2x drawDRRect()
            ASSERT(firstEdge().borderStyle() == DOUBLE);
            drawDoubleBorder(context, borderRect);
        }

        return true;
    }

    // This is faster than the normal complex border path only if it avoids creating transparency
    // layers (when the border is translucent).
    if (firstEdge().borderStyle() == SOLID && !m_outer.isRounded() && m_hasAlpha) {
        ASSERT(m_visibleEdgeSet != AllBorderEdges);
        // solid, rectangular border => one drawPath()
        Path path;
        path.setWindRule(RULE_NONZERO);

        for (int i = BSTop; i <= BSLeft; ++i) {
            const BorderEdge& currEdge = m_edges[i];
            if (currEdge.shouldRender())
                path.addRect(calculateSideRect(m_outer, currEdge, i));
        }

        context->setFillColor(firstEdge().color);
        context->fillPath(path);
        return true;
    }

    return false;
}

BoxBorderPainter::BoxBorderPainter(const LayoutRect& borderRect, const ComputedStyle& style,
    const IntRect& clipRect, BackgroundBleedAvoidance bleedAvoidance, bool includeLogicalLeftEdge,
    bool includeLogicalRightEdge)
    : m_style(style)
    , m_bleedAvoidance(bleedAvoidance)
    , m_includeLogicalLeftEdge(includeLogicalLeftEdge)
    , m_includeLogicalRightEdge(includeLogicalRightEdge)
    , m_visibleEdgeCount(0)
    , m_firstVisibleEdge(0)
    , m_visibleEdgeSet(0)
    , m_isUniformStyle(true)
    , m_isUniformWidth(true)
    , m_isUniformColor(true)
    , m_isRounded(false)
    , m_hasAlpha(false)
{
    style.getBorderEdgeInfo(m_edges, includeLogicalLeftEdge, includeLogicalRightEdge);
    computeBorderProperties();

    // No need to compute the rrects if we don't have any borders to draw.
    if (!m_visibleEdgeSet)
        return;

    m_outer = m_style.getRoundedBorderFor(borderRect, includeLogicalLeftEdge, includeLogicalRightEdge);
    m_inner = m_style.getRoundedInnerBorderFor(borderRect, includeLogicalLeftEdge, includeLogicalRightEdge);

    // If no corner intersects the clip region, we can pretend the outer border is
    // rectangular to improve performance.
    // FIXME: why is this predicated on uniform style & solid edges?
    if (m_isUniformStyle
        && firstEdge().borderStyle() == SOLID
        && m_outer.isRounded()
        && BoxPainter::allCornersClippedOut(m_outer, clipRect))
        m_outer.setRadii(FloatRoundedRect::Radii());

    m_isRounded = m_outer.isRounded();
}

BoxBorderPainter::BoxBorderPainter(const ComputedStyle& style, const LayoutRect& outer,
    const LayoutRect& inner, const BorderEdge& uniformEdgeInfo)
    : m_style(style)
    , m_bleedAvoidance(BackgroundBleedNone)
    , m_includeLogicalLeftEdge(true)
    , m_includeLogicalRightEdge(true)
    , m_outer(outer)
    , m_inner(inner)
    , m_visibleEdgeCount(0)
    , m_firstVisibleEdge(0)
    , m_visibleEdgeSet(0)
    , m_isUniformStyle(true)
    , m_isUniformWidth(true)
    , m_isUniformColor(true)
    , m_isRounded(false)
    , m_hasAlpha(false)
{
    for (auto& edge : m_edges)
        edge = uniformEdgeInfo;

    computeBorderProperties();
}

void BoxBorderPainter::computeBorderProperties()
{
    for (unsigned i = 0; i < WTF_ARRAY_LENGTH(m_edges); ++i) {
        const BorderEdge& edge = m_edges[i];

        if (!edge.shouldRender()) {
            if (edge.presentButInvisible()) {
                m_isUniformWidth = false;
                m_isUniformColor = false;
            }

            continue;
        }

        ASSERT(edge.color.alpha() > 0);

        m_visibleEdgeCount++;
        m_visibleEdgeSet |= edgeFlagForSide(static_cast<BoxSide>(i));

        m_hasAlpha |= edge.color.hasAlpha();

        if (m_visibleEdgeCount == 1) {
            m_firstVisibleEdge = i;
            continue;
        }

        m_isUniformStyle &= edge.borderStyle() == m_edges[m_firstVisibleEdge].borderStyle();
        m_isUniformWidth &= edge.width == m_edges[m_firstVisibleEdge].width;
        m_isUniformColor &= edge.color == m_edges[m_firstVisibleEdge].color;
    }
}

void BoxBorderPainter::paintBorder(const PaintInfo& info, const LayoutRect& rect) const
{
    if (!m_visibleEdgeCount || m_outer.rect().isEmpty())
        return;

    GraphicsContext* graphicsContext = info.context;

    if (paintBorderFastPath(graphicsContext, rect))
        return;

    bool clipToOuterBorder = m_outer.isRounded();
    GraphicsContextStateSaver stateSaver(*graphicsContext, clipToOuterBorder);
    if (clipToOuterBorder) {
        // For BackgroundBleedClip{Only,Layer}, the outer rrect clip is already applied.
        if (!bleedAvoidanceIsClipping(m_bleedAvoidance))
            graphicsContext->clipRoundedRect(m_outer);

        if (m_inner.isRenderable() && !m_inner.isEmpty())
            graphicsContext->clipOutRoundedRect(m_inner);
    }

    bool antialias =
        RuntimeEnabledFeatures::slimmingPaintEnabled()
        || m_visibleEdgeCount == 1
        || BoxPainter::shouldAntialiasLines(graphicsContext);

    const ComplexBorderInfo borderInfo(*this, antialias);
    paintOpacityGroup(graphicsContext, borderInfo, 0, 1);
}

// In order to maximize the use of overdraw as a corner seam avoidance technique, we draw
// translucent border sides using the following algorithm:
//
//   1) cluster sides sharing the same opacity into "opacity groups" [ComplexBorderInfo]
//   2) sort groups in increasing opacity order [ComplexBorderInfo]
//   3) reverse-iterate over groups (decreasing opacity order), pushing nested transparency
//      layers with adjusted/relative opacity [paintOpacityGroup]
//   4) iterate over groups (increasing opacity order), painting actual group contents and
//      then ending their corresponding transparency layer [paintOpacityGroup]
//
// Layers are created in decreasing opacity order (top -> bottom), while actual border sides are
// drawn in increasing opacity order (bottom -> top). At each level, opacity is adjusted to acount
// for accumulated/ancestor layer alpha. Because opacity is applied via layers, the actual draw
// paint is opaque.
//
// As an example, let's consider a border with the following sides/opacities:
//
//   top:    1.0
//   right:  0.25
//   bottom: 0.5
//   left:   0.25
//
// These are grouped and sorted in ComplexBorderInfo as follows:
//
//   group[0]: { alpha: 1.0,  sides: top }
//   group[1]: { alpha: 0.5,  sides: bottom }
//   group[2]: { alpha: 0.25, sides: right, left }
//
// Applying the algorithm yields the following paint sequence:
//
//                                 // no layer needed for group 0 (alpha == 1)
//   beginLayer(0.5)               // layer for group 1
//     beginLayer(0.5)             // layer for group 2 (effective opacity: 0.5 * 0.5 == 0.25)
//       paintSides(right, left)   // paint group 2
//     endLayer
//     paintSides(bottom)          // paint group 1
//   endLayer
//   paintSides(top)               // paint group 0
//
// Note that we're always drawing using opaque paints on top of less-opaque content - hence
// we can use overdraw to mask portions of the previous sides.
//
BorderEdgeFlags BoxBorderPainter::paintOpacityGroup(GraphicsContext* context,
    const ComplexBorderInfo& borderInfo, unsigned index, float effectiveOpacity) const
{
    ASSERT(effectiveOpacity > 0 && effectiveOpacity <= 1);

    const size_t opacityGroupCount = borderInfo.opacityGroups.size();

    // For overdraw logic purposes, treat missing/transparent edges as completed.
    if (index >= opacityGroupCount)
        return ~m_visibleEdgeSet;

    // Groups are sorted in increasing opacity order, but we need to create layers in
    // decreasing opacity order - hence the reverse iteration.
    const OpacityGroup& group = borderInfo.opacityGroups[opacityGroupCount - index - 1];

    // Adjust this group's paint opacity to account for ancestor transparency layers
    // (needed in case we avoid creating a layer below).
    unsigned paintAlpha = group.alpha / effectiveOpacity;
    ASSERT(paintAlpha <= 255);

    // For the last (bottom) group, we can skip the layer even in the presence of opacity iff
    // it contains no adjecent edges (no in-group overdraw possibility).
    bool needsLayer = group.alpha != 255
        && (includesAdjacentEdges(group.edgeFlags) || (index + 1 < borderInfo.opacityGroups.size()));

    if (needsLayer) {
        const float groupOpacity = static_cast<float>(group.alpha) / 255;
        ASSERT(groupOpacity < effectiveOpacity);

        context->beginLayer(groupOpacity / effectiveOpacity);
        effectiveOpacity = groupOpacity;

        // Group opacity is applied via a layer => we draw the members using opaque paint.
        paintAlpha = 255;
    }

    // Recursion may seem unpalatable here, but
    //   a) it has an upper bound of 4
    //   b) only triggers at all when mixing border sides with different opacities
    //   c) it allows us to express the layer nesting algorithm more naturally
    BorderEdgeFlags completedEdges = paintOpacityGroup(context, borderInfo, index + 1, effectiveOpacity);

    // Paint the actual group edges with an alpha adjusted to account for ancenstor layers opacity.
    for (BoxSide side : group.sides) {
        paintSide(context, borderInfo, side, paintAlpha, completedEdges);
        completedEdges |= edgeFlagForSide(side);
    }

    if (needsLayer)
        context->endLayer();

    return completedEdges;
}

void BoxBorderPainter::paintSide(GraphicsContext* context, const ComplexBorderInfo& borderInfo,
    BoxSide side, unsigned alpha, BorderEdgeFlags completedEdges) const
{
    const BorderEdge& edge = m_edges[side];
    ASSERT(edge.shouldRender());
    const Color color(edge.color.red(), edge.color.green(), edge.color.blue(), alpha);

    FloatRect sideRect = m_outer.rect();
    const Path* path = nullptr;

    // TODO(fmalita): find a way to consolidate these without sacrificing readability.
    switch (side) {
    case BSTop: {
        bool usePath = m_isRounded && (borderStyleHasInnerDetail(edge.borderStyle())
            || borderWillArcInnerEdge(m_inner.radii().topLeft(), m_inner.radii().topRight()));
        if (usePath)
            path = &borderInfo.roundedBorderPath;
        else
            sideRect.setHeight(edge.width);

        paintOneBorderSide(context, sideRect, BSTop, BSLeft, BSRight, path, borderInfo.antiAlias,
            color, completedEdges);
        break;
    }
    case BSBottom: {
        bool usePath = m_isRounded && (borderStyleHasInnerDetail(edge.borderStyle())
            || borderWillArcInnerEdge(m_inner.radii().bottomLeft(), m_inner.radii().bottomRight()));
        if (usePath)
            path = &borderInfo.roundedBorderPath;
        else
            sideRect.shiftYEdgeTo(sideRect.maxY() - edge.width);

        paintOneBorderSide(context, sideRect, BSBottom, BSLeft, BSRight, path, borderInfo.antiAlias,
            color, completedEdges);
        break;
    }
    case BSLeft: {
        bool usePath = m_isRounded && (borderStyleHasInnerDetail(edge.borderStyle())
            || borderWillArcInnerEdge(m_inner.radii().bottomLeft(), m_inner.radii().topLeft()));
        if (usePath)
            path = &borderInfo.roundedBorderPath;
        else
            sideRect.setWidth(edge.width);

        paintOneBorderSide(context, sideRect, BSLeft, BSTop, BSBottom, path, borderInfo.antiAlias,
            color, completedEdges);
        break;
    }
    case BSRight: {
        bool usePath = m_isRounded && (borderStyleHasInnerDetail(edge.borderStyle())
            || borderWillArcInnerEdge(m_inner.radii().bottomRight(), m_inner.radii().topRight()));
        if (usePath)
            path = &borderInfo.roundedBorderPath;
        else
            sideRect.shiftXEdgeTo(sideRect.maxX() - edge.width);

        paintOneBorderSide(context, sideRect, BSRight, BSTop, BSBottom, path, borderInfo.antiAlias,
            color, completedEdges);
        break;
    }
    default:
        ASSERT_NOT_REACHED();
    }
}

BoxBorderPainter::MiterType BoxBorderPainter::computeMiter(BoxSide side, BoxSide adjacentSide,
    BorderEdgeFlags completedEdges, bool antialias) const
{
    const BorderEdge& adjacentEdge = m_edges[adjacentSide];

    // No miters for missing edges.
    if (!adjacentEdge.isPresent)
        return NoMiter;

    // The adjacent edge will overdraw this corner, resulting in a correct miter.
    if (willOverdraw(adjacentSide, adjacentEdge.borderStyle(), completedEdges))
        return NoMiter;

    // Color transitions require miters. Use miters compatible with the AA drawing mode to avoid
    // introducing extra clips.
    if (!colorsMatchAtCorner(side, adjacentSide, m_edges))
        return antialias ? SoftMiter : HardMiter;

    // Non-anti-aliased miters ensure correct same-color seaming when required by style.
    if (borderStylesRequireMiter(side, adjacentSide, m_edges[side].borderStyle(), adjacentEdge.borderStyle()))
        return HardMiter;

    // Overdraw the adjacent edge when the colors match and we have no style restrictions.
    return NoMiter;
}

bool BoxBorderPainter::mitersRequireClipping(MiterType miter1, MiterType miter2, EBorderStyle style,
    bool antialias)
{
    // Clipping is required if any of the present miters doesn't match the current AA mode.
    bool shouldClip = antialias
        ? miter1 == HardMiter || miter2 == HardMiter
        : miter1 == SoftMiter || miter2 == SoftMiter;

    // Some styles require clipping for any type of miter.
    shouldClip = shouldClip
        || ((miter1 != NoMiter || miter2 != NoMiter) && styleRequiresClipPolygon(style));

    return shouldClip;
}

void BoxBorderPainter::paintOneBorderSide(GraphicsContext* graphicsContext,
    const FloatRect& sideRect, BoxSide side, BoxSide adjacentSide1, BoxSide adjacentSide2,
    const Path* path, bool antialias, Color color, BorderEdgeFlags completedEdges) const
{
    const BorderEdge& edgeToRender = m_edges[side];
    ASSERT(edgeToRender.width);
    const BorderEdge& adjacentEdge1 = m_edges[adjacentSide1];
    const BorderEdge& adjacentEdge2 = m_edges[adjacentSide2];

    if (path) {
        MiterType miter1 = colorsMatchAtCorner(side, adjacentSide1, m_edges) ? HardMiter : SoftMiter;
        MiterType miter2 = colorsMatchAtCorner(side, adjacentSide2, m_edges) ? HardMiter : SoftMiter;

        GraphicsContextStateSaver stateSaver(*graphicsContext);
        if (m_inner.isRenderable())
            clipBorderSidePolygon(graphicsContext, side, miter1, miter2);
        else
            clipBorderSideForComplexInnerPath(graphicsContext, side);
        float thickness = std::max(std::max(edgeToRender.width, adjacentEdge1.width), adjacentEdge2.width);
        drawBoxSideFromPath(graphicsContext, LayoutRect(m_outer.rect()), *path, edgeToRender.width,
            thickness, side, color, edgeToRender.borderStyle());
    } else {
        MiterType miter1 = computeMiter(side, adjacentSide1, completedEdges, antialias);
        MiterType miter2 = computeMiter(side, adjacentSide2, completedEdges, antialias);
        bool shouldClip = mitersRequireClipping(miter1, miter2, edgeToRender.borderStyle(), antialias);

        GraphicsContextStateSaver clipStateSaver(*graphicsContext, shouldClip);
        if (shouldClip) {
            clipBorderSidePolygon(graphicsContext, side, miter1, miter2);

            // Miters are applied via clipping, no need to draw them.
            miter1 = miter2 = NoMiter;
        }

        ObjectPainter::drawLineForBoxSide(graphicsContext, sideRect.x(), sideRect.y(),
            sideRect.maxX(), sideRect.maxY(), side, color, edgeToRender.borderStyle(),
            miter1 != NoMiter ? adjacentEdge1.width : 0, miter2 != NoMiter ? adjacentEdge2.width : 0,
            antialias);
    }
}

void BoxBorderPainter::drawBoxSideFromPath(GraphicsContext* graphicsContext,
    const LayoutRect& borderRect, const Path& borderPath, float thickness, float drawThickness,
    BoxSide side, Color color, EBorderStyle borderStyle) const
{
    if (thickness <= 0)
        return;

    if (borderStyle == DOUBLE && thickness < 3)
        borderStyle = SOLID;

    switch (borderStyle) {
    case BNONE:
    case BHIDDEN:
        return;
    case DOTTED:
    case DASHED: {
        graphicsContext->setStrokeColor(color);

        // The stroke is doubled here because the provided path is the
        // outside edge of the border so half the stroke is clipped off.
        // The extra multiplier is so that the clipping mask can antialias
        // the edges to prevent jaggies.
        graphicsContext->setStrokeThickness(drawThickness * 2 * 1.1f);
        graphicsContext->setStrokeStyle(borderStyle == DASHED ? DashedStroke : DottedStroke);

        // If the number of dashes that fit in the path is odd and non-integral then we
        // will have an awkwardly-sized dash at the end of the path. To try to avoid that
        // here, we simply make the whitespace dashes ever so slightly bigger.
        // FIXME: This could be even better if we tried to manipulate the dash offset
        // and possibly the gapLength to get the corners dash-symmetrical.
        float dashLength = thickness * ((borderStyle == DASHED) ? 3.0f : 1.0f);
        float gapLength = dashLength;
        float numberOfDashes = borderPath.length() / dashLength;
        // Don't try to show dashes if we have less than 2 dashes + 2 gaps.
        // FIXME: should do this test per side.
        if (numberOfDashes >= 4) {
            bool evenNumberOfFullDashes = !((int)numberOfDashes % 2);
            bool integralNumberOfDashes = !(numberOfDashes - (int)numberOfDashes);
            if (!evenNumberOfFullDashes && !integralNumberOfDashes) {
                float numberOfGaps = numberOfDashes / 2;
                gapLength += (dashLength  / numberOfGaps);
            }

            DashArray lineDash;
            lineDash.append(dashLength);
            lineDash.append(gapLength);
            graphicsContext->setLineDash(lineDash, dashLength);
        }

        // FIXME: stroking the border path causes issues with tight corners:
        // https://bugs.webkit.org/show_bug.cgi?id=58711
        // Also, to get the best appearance we should stroke a path between the two borders.
        graphicsContext->strokePath(borderPath);
        return;
    }
    case DOUBLE: {
        // Draw inner border line
        {
            GraphicsContextStateSaver stateSaver(*graphicsContext);
            const LayoutRectOutsets innerInsets = doubleStripeInsets(m_edges, BorderEdge::DoubleBorderStripeInner);
            FloatRoundedRect innerClip = m_style.getRoundedInnerBorderFor(borderRect, innerInsets,
                m_includeLogicalLeftEdge, m_includeLogicalRightEdge);

            graphicsContext->clipRoundedRect(innerClip);
            drawBoxSideFromPath(graphicsContext, borderRect, borderPath, thickness, drawThickness,
                side, color, SOLID);
        }

        // Draw outer border line
        {
            GraphicsContextStateSaver stateSaver(*graphicsContext);
            LayoutRect outerRect = borderRect;
            LayoutRectOutsets outerInsets = doubleStripeInsets(m_edges, BorderEdge::DoubleBorderStripeOuter);

            if (bleedAvoidanceIsClipping(m_bleedAvoidance)) {
                outerRect.inflate(1);
                outerInsets.setTop(outerInsets.top() - 1);
                outerInsets.setRight(outerInsets.right() - 1);
                outerInsets.setBottom(outerInsets.bottom() - 1);
                outerInsets.setLeft(outerInsets.left() - 1);
            }

            FloatRoundedRect outerClip = m_style.getRoundedInnerBorderFor(outerRect, outerInsets,
                m_includeLogicalLeftEdge, m_includeLogicalRightEdge);
            graphicsContext->clipOutRoundedRect(outerClip);
            drawBoxSideFromPath(graphicsContext, borderRect, borderPath, thickness, drawThickness,
                side, color, SOLID);
        }
        return;
    }
    case RIDGE:
    case GROOVE:
    {
        EBorderStyle s1;
        EBorderStyle s2;
        if (borderStyle == GROOVE) {
            s1 = INSET;
            s2 = OUTSET;
        } else {
            s1 = OUTSET;
            s2 = INSET;
        }

        // Paint full border
        drawBoxSideFromPath(graphicsContext, borderRect, borderPath, thickness, drawThickness,
            side, color, s1);

        // Paint inner only
        GraphicsContextStateSaver stateSaver(*graphicsContext);
        LayoutUnit topWidth = m_edges[BSTop].usedWidth() / 2;
        LayoutUnit bottomWidth = m_edges[BSBottom].usedWidth() / 2;
        LayoutUnit leftWidth = m_edges[BSLeft].usedWidth() / 2;
        LayoutUnit rightWidth = m_edges[BSRight].usedWidth() / 2;

        FloatRoundedRect clipRect = m_style.getRoundedInnerBorderFor(borderRect,
            LayoutRectOutsets(-topWidth, -rightWidth, -bottomWidth, -leftWidth),
            m_includeLogicalLeftEdge, m_includeLogicalRightEdge);

        graphicsContext->clipRoundedRect(clipRect);
        drawBoxSideFromPath(graphicsContext, borderRect, borderPath, thickness, drawThickness,
            side, color, s2);
        return;
    }
    case INSET:
        if (side == BSTop || side == BSLeft)
            color = color.dark();
        break;
    case OUTSET:
        if (side == BSBottom || side == BSRight)
            color = color.dark();
        break;
    default:
        break;
    }

    graphicsContext->setStrokeStyle(NoStroke);
    graphicsContext->setFillColor(color);
    graphicsContext->drawRect(pixelSnappedIntRect(borderRect));
}

void BoxBorderPainter::clipBorderSideForComplexInnerPath(GraphicsContext* graphicsContext,
    BoxSide side) const
{
    graphicsContext->clip(calculateSideRectIncludingInner(m_outer, m_edges, side));
    FloatRoundedRect adjustedInnerRect = calculateAdjustedInnerBorder(m_inner, side);
    if (!adjustedInnerRect.isEmpty())
        graphicsContext->clipOutRoundedRect(adjustedInnerRect);
}

void BoxBorderPainter::clipBorderSidePolygon(GraphicsContext* graphicsContext, BoxSide side,
    MiterType firstMiter, MiterType secondMiter) const
{
    ASSERT(firstMiter != NoMiter || secondMiter != NoMiter);

    FloatPoint quad[4];

    const LayoutRect outerRect(m_outer.rect());
    const LayoutRect innerRect(m_inner.rect());

    // For each side, create a quad that encompasses all parts of that side that may draw,
    // including areas inside the innerBorder.
    //
    //         0----------------3
    //       0  \              /  0
    //       |\  1----------- 2  /|
    //       | 1                1 |
    //       | |                | |
    //       | |                | |
    //       | 2                2 |
    //       |/  1------------2  \|
    //       3  /              \  3
    //         0----------------3
    //
    switch (side) {
    case BSTop:
        quad[0] = FloatPoint(outerRect.minXMinYCorner());
        quad[1] = FloatPoint(innerRect.minXMinYCorner());
        quad[2] = FloatPoint(innerRect.maxXMinYCorner());
        quad[3] = FloatPoint(outerRect.maxXMinYCorner());

        if (!m_inner.radii().topLeft().isZero()) {
            findIntersection(quad[0], quad[1],
                FloatPoint(
                    quad[1].x() + m_inner.radii().topLeft().width(),
                    quad[1].y()),
                FloatPoint(
                    quad[1].x(),
                    quad[1].y() + m_inner.radii().topLeft().height()),
                quad[1]);
        }

        if (!m_inner.radii().topRight().isZero()) {
            findIntersection(quad[3], quad[2],
                FloatPoint(
                    quad[2].x() - m_inner.radii().topRight().width(),
                    quad[2].y()),
                FloatPoint(
                    quad[2].x(),
                    quad[2].y() + m_inner.radii().topRight().height()),
                quad[2]);
        }
        break;

    case BSLeft:
        quad[0] = FloatPoint(outerRect.minXMinYCorner());
        quad[1] = FloatPoint(innerRect.minXMinYCorner());
        quad[2] = FloatPoint(innerRect.minXMaxYCorner());
        quad[3] = FloatPoint(outerRect.minXMaxYCorner());

        if (!m_inner.radii().topLeft().isZero()) {
            findIntersection(quad[0], quad[1],
                FloatPoint(
                    quad[1].x() + m_inner.radii().topLeft().width(),
                    quad[1].y()),
                FloatPoint(
                    quad[1].x(),
                    quad[1].y() + m_inner.radii().topLeft().height()),
                quad[1]);
        }

        if (!m_inner.radii().bottomLeft().isZero()) {
            findIntersection(quad[3], quad[2],
                FloatPoint(
                    quad[2].x() + m_inner.radii().bottomLeft().width(),
                    quad[2].y()),
                FloatPoint(
                    quad[2].x(),
                    quad[2].y() - m_inner.radii().bottomLeft().height()),
                quad[2]);
        }
        break;

    case BSBottom:
        quad[0] = FloatPoint(outerRect.minXMaxYCorner());
        quad[1] = FloatPoint(innerRect.minXMaxYCorner());
        quad[2] = FloatPoint(innerRect.maxXMaxYCorner());
        quad[3] = FloatPoint(outerRect.maxXMaxYCorner());

        if (!m_inner.radii().bottomLeft().isZero()) {
            findIntersection(quad[0], quad[1],
                FloatPoint(
                    quad[1].x() + m_inner.radii().bottomLeft().width(),
                    quad[1].y()),
                FloatPoint(
                    quad[1].x(),
                    quad[1].y() - m_inner.radii().bottomLeft().height()),
                quad[1]);
        }

        if (!m_inner.radii().bottomRight().isZero()) {
            findIntersection(quad[3], quad[2],
                FloatPoint(
                    quad[2].x() - m_inner.radii().bottomRight().width(),
                    quad[2].y()),
                FloatPoint(
                    quad[2].x(),
                    quad[2].y() - m_inner.radii().bottomRight().height()),
                quad[2]);
        }
        break;

    case BSRight:
        quad[0] = FloatPoint(outerRect.maxXMinYCorner());
        quad[1] = FloatPoint(innerRect.maxXMinYCorner());
        quad[2] = FloatPoint(innerRect.maxXMaxYCorner());
        quad[3] = FloatPoint(outerRect.maxXMaxYCorner());

        if (!m_inner.radii().topRight().isZero()) {
            findIntersection(quad[0], quad[1],
                FloatPoint(
                    quad[1].x() - m_inner.radii().topRight().width(),
                    quad[1].y()),
                FloatPoint(
                    quad[1].x(),
                    quad[1].y() + m_inner.radii().topRight().height()),
                quad[1]);
        }

        if (!m_inner.radii().bottomRight().isZero()) {
            findIntersection(quad[3], quad[2],
                FloatPoint(
                    quad[2].x() - m_inner.radii().bottomRight().width(),
                    quad[2].y()),
                FloatPoint(
                    quad[2].x(),
                    quad[2].y() - m_inner.radii().bottomRight().height()),
                quad[2]);
        }
        break;
    }

    if (firstMiter == secondMiter) {
        graphicsContext->clipPolygon(4, quad, firstMiter == SoftMiter);
        return;
    }

    // If antialiasing settings for the first edge and second edge is different,
    // they have to be addressed separately. We do this by breaking the quad into
    // two parallelograms, made by moving quad[1] and quad[2].
    float ax = quad[1].x() - quad[0].x();
    float ay = quad[1].y() - quad[0].y();
    float bx = quad[2].x() - quad[1].x();
    float by = quad[2].y() - quad[1].y();
    float cx = quad[3].x() - quad[2].x();
    float cy = quad[3].y() - quad[2].y();

    const static float kEpsilon = 1e-2f;
    float r1, r2;
    if (fabsf(bx) < kEpsilon && fabsf(by) < kEpsilon) {
        // The quad was actually a triangle.
        r1 = r2 = 1.0f;
    } else {
        // Extend parallelogram a bit to hide calculation error
        const static float kExtendFill = 1e-2f;

        r1 = (-ax * by + ay * bx) / (cx * by - cy * bx) + kExtendFill;
        r2 = (-cx * by + cy * bx) / (ax * by - ay * bx) + kExtendFill;
    }

    if (firstMiter != NoMiter) {
        FloatPoint firstQuad[4];
        firstQuad[0] = quad[0];
        firstQuad[1] = quad[1];
        firstQuad[2] = FloatPoint(quad[3].x() + r2 * ax, quad[3].y() + r2 * ay);
        firstQuad[3] = quad[3];
        graphicsContext->clipPolygon(4, firstQuad, firstMiter == SoftMiter);
    }

    if (secondMiter != NoMiter) {
        FloatPoint secondQuad[4];
        secondQuad[0] = quad[0];
        secondQuad[1] = FloatPoint(quad[0].x() - r1 * cx, quad[0].y() - r1 * cy);
        secondQuad[2] = quad[2];
        secondQuad[3] = quad[3];
        graphicsContext->clipPolygon(4, secondQuad, secondMiter == SoftMiter);
    }
}

} // namespace blink

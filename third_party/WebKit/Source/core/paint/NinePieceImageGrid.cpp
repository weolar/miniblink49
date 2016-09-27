// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/NinePieceImageGrid.h"

#include "core/style/ComputedStyle.h"
#include "core/style/NinePieceImage.h"
#include "platform/LengthFunctions.h"
#include "platform/geometry/FloatSize.h"
#include "platform/geometry/IntSize.h"

namespace blink {

static LayoutUnit computeEdgeWidth(const BorderImageLength& borderSlice, int borderSide, const LayoutUnit& imageSide,
    const LayoutUnit& boxExtent)
{
    if (borderSlice.isNumber())
        return borderSlice.number() * borderSide;
    if (borderSlice.length().isAuto())
        return imageSide;
    return valueForLength(borderSlice.length(), boxExtent);
}

static int computeEdgeSlice(const Length& slice, LayoutUnit maximum)
{
    return std::min<int>(maximum, valueForLength(slice, maximum));
}

NinePieceImageGrid::NinePieceImageGrid(const NinePieceImage& ninePieceImage, IntSize imageSize, IntRect borderImageArea,
    const IntRectOutsets& borderWidths)
    : m_borderImageArea(borderImageArea)
    , m_imageSize(imageSize)
    , m_horizontalTileRule((Image::TileRule)ninePieceImage.horizontalRule())
    , m_verticalTileRule((Image::TileRule)ninePieceImage.verticalRule())
    , m_fill(ninePieceImage.fill())
{
    StyleImage* styleImage = ninePieceImage.image();
    ASSERT(styleImage);

    float imageScaleFactor = styleImage->imageScaleFactor();
    m_top.slice = computeEdgeSlice(ninePieceImage.imageSlices().top(), imageSize.height()) * imageScaleFactor;
    m_right.slice = computeEdgeSlice(ninePieceImage.imageSlices().right(), imageSize.width()) * imageScaleFactor;
    m_bottom.slice = computeEdgeSlice(ninePieceImage.imageSlices().bottom(), imageSize.height()) * imageScaleFactor;
    m_left.slice = computeEdgeSlice(ninePieceImage.imageSlices().left(), imageSize.width()) * imageScaleFactor;

    m_top.width = computeEdgeWidth(ninePieceImage.borderSlices().top(), borderWidths.top(), m_top.slice,
        borderImageArea.height());
    m_right.width = computeEdgeWidth(ninePieceImage.borderSlices().right(), borderWidths.right(), m_right.slice,
        borderImageArea.width());
    m_bottom.width = computeEdgeWidth(ninePieceImage.borderSlices().bottom(), borderWidths.bottom(), m_bottom.slice,
        borderImageArea.height());
    m_left.width = computeEdgeWidth(ninePieceImage.borderSlices().left(), borderWidths.left(), m_left.slice,
        borderImageArea.width());

    // The spec says: Given Lwidth as the width of the border image area, Lheight as its height, and Wside as the border
    // image width offset for the side, let f = min(Lwidth/(Wleft+Wright), Lheight/(Wtop+Wbottom)). If f < 1, then all W
    // are reduced by multiplying them by f.
    int borderSideWidth = std::max(1, m_left.width + m_right.width);
    int borderSideHeight = std::max(1, m_top.width + m_bottom.width);
    float borderSideScaleFactor = std::min((float)borderImageArea.width() / borderSideWidth,
        (float)borderImageArea.height() / borderSideHeight);
    if (borderSideScaleFactor < 1) {
        m_top.width *= borderSideScaleFactor;
        m_right.width *= borderSideScaleFactor;
        m_bottom.width *= borderSideScaleFactor;
        m_left.width *= borderSideScaleFactor;
    }
}

// Given a rectangle, construct a subrectangle using offset, width and height. Negative offsets are relative to the
// extent of the given rectangle.
static FloatRect subrect(IntRect rect, float offsetX, float offsetY, float width, float height)
{
    float baseX = rect.x();
    if (offsetX < 0)
        baseX = rect.maxX();

    float baseY = rect.y();
    if (offsetY < 0)
        baseY = rect.maxY();

    return FloatRect(baseX + offsetX, baseY + offsetY, width, height);
}

static FloatRect subrect(IntSize size, float offsetX, float offsetY, float width, float height)
{
    return subrect(IntRect(IntPoint(), size), offsetX, offsetY, width, height);
}

static inline void setCornerPiece(NinePieceImageGrid::NinePieceDrawInfo& drawInfo, bool isDrawable,
    const FloatRect& source, const FloatRect& destination)
{
    drawInfo.isDrawable = isDrawable;
    if (drawInfo.isDrawable) {
        drawInfo.source = source;
        drawInfo.destination = destination;
    }
}

void NinePieceImageGrid::setDrawInfoCorner(NinePieceDrawInfo& drawInfo, NinePiece piece) const
{
    switch (piece) {
    case TopLeftPiece:
        setCornerPiece(drawInfo, m_top.isDrawable() && m_left.isDrawable(),
            subrect(m_imageSize, 0, 0, m_left.slice, m_top.slice),
            subrect(m_borderImageArea, 0, 0, m_left.width, m_top.width));
        break;
    case BottomLeftPiece:
        setCornerPiece(drawInfo, m_bottom.isDrawable() && m_left.isDrawable(),
            subrect(m_imageSize, 0, -m_bottom.slice, m_left.slice, m_bottom.slice),
            subrect(m_borderImageArea, 0, -m_bottom.width, m_left.width, m_bottom.width));
        break;
    case TopRightPiece:
        setCornerPiece(drawInfo, m_top.isDrawable() && m_right.isDrawable(),
            subrect(m_imageSize, -m_right.slice, 0, m_right.slice, m_top.slice),
            subrect(m_borderImageArea, -m_right.width, 0, m_right.width, m_top.width));
        break;
    case BottomRightPiece:
        setCornerPiece(drawInfo, m_bottom.isDrawable() && m_right.isDrawable(),
            subrect(m_imageSize, -m_right.slice, -m_bottom.slice, m_right.slice, m_bottom.slice),
            subrect(m_borderImageArea, -m_right.width, -m_bottom.width, m_right.width, m_bottom.width));
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }
}

static inline void setHorizontalEdge(NinePieceImageGrid::NinePieceDrawInfo& drawInfo,
    const NinePieceImageGrid::Edge& edge, const FloatRect& source, const FloatRect& destination,
    Image::TileRule tileRule)
{
    drawInfo.isDrawable = edge.isDrawable() && source.width() > 0;
    if (drawInfo.isDrawable) {
        drawInfo.source = source;
        drawInfo.destination = destination;
        drawInfo.tileScale = FloatSize(edge.scale(), edge.scale());
        drawInfo.tileRule = { tileRule, Image::StretchTile };
    }
}

static inline void setVerticalEdge(NinePieceImageGrid::NinePieceDrawInfo& drawInfo,
    const NinePieceImageGrid::Edge& edge, const FloatRect& source, const FloatRect& destination,
    Image::TileRule tileRule)
{
    drawInfo.isDrawable = edge.isDrawable() && source.height() > 0;
    if (drawInfo.isDrawable) {
        drawInfo.source = source;
        drawInfo.destination = destination;
        drawInfo.tileScale = FloatSize(edge.scale(), edge.scale());
        drawInfo.tileRule = { Image::StretchTile, tileRule };
    }
}

void NinePieceImageGrid::setDrawInfoEdge(NinePieceDrawInfo& drawInfo, NinePiece piece) const
{
    IntSize edgeSourceSize = m_imageSize - IntSize(m_left.slice + m_right.slice, m_top.slice + m_bottom.slice);
    IntSize edgeDestinationSize = m_borderImageArea.size() - IntSize(m_left.width + m_right.width, m_top.width + m_bottom.width);

    switch (piece) {
    case LeftPiece:
        setVerticalEdge(drawInfo, m_left,
            subrect(m_imageSize, 0, m_top.slice, m_left.slice, edgeSourceSize.height()),
            subrect(m_borderImageArea, 0, m_top.width, m_left.width, edgeDestinationSize.height()),
            m_verticalTileRule);
        break;
    case RightPiece:
        setVerticalEdge(drawInfo, m_right,
            subrect(m_imageSize, -m_right.slice, m_top.slice, m_right.slice, edgeSourceSize.height()),
            subrect(m_borderImageArea, -m_right.width, m_top.width, m_right.width, edgeDestinationSize.height()),
            m_verticalTileRule);
        break;
    case TopPiece:
        setHorizontalEdge(drawInfo, m_top,
            subrect(m_imageSize, m_left.slice, 0, edgeSourceSize.width(), m_top.slice),
            subrect(m_borderImageArea, m_left.width, 0, edgeDestinationSize.width(), m_top.width),
            m_horizontalTileRule);
        break;
    case BottomPiece:
        setHorizontalEdge(drawInfo, m_bottom,
            subrect(m_imageSize, m_left.slice, -m_bottom.slice, edgeSourceSize.width(), m_bottom.slice),
            subrect(m_borderImageArea, m_left.width, -m_bottom.width, edgeDestinationSize.width(), m_bottom.width),
            m_horizontalTileRule);
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }
}

void NinePieceImageGrid::setDrawInfoMiddle(NinePieceDrawInfo& drawInfo) const
{
    IntSize sourceSize = m_imageSize - IntSize(m_left.slice + m_right.slice, m_top.slice + m_bottom.slice);
    IntSize destinationSize =
        m_borderImageArea.size() - IntSize(m_left.width + m_right.width, m_top.width + m_bottom.width);

    drawInfo.isDrawable = m_fill && !sourceSize.isEmpty() && !destinationSize.isEmpty();
    if (!drawInfo.isDrawable)
        return;

    drawInfo.source = subrect(m_imageSize, m_left.slice, m_top.slice, sourceSize.width(), sourceSize.height());
    drawInfo.destination = subrect(m_borderImageArea, m_left.width, m_top.width,
        destinationSize.width(), destinationSize.height());

    FloatSize middleScaleFactor(1, 1);

    if (m_top.isDrawable())
        middleScaleFactor.setWidth(m_top.scale());
    else if (m_bottom.isDrawable())
        middleScaleFactor.setWidth(m_bottom.scale());

    if (m_left.isDrawable())
        middleScaleFactor.setHeight(m_left.scale());
    else if (m_right.isDrawable())
        middleScaleFactor.setHeight(m_right.scale());

    if (!sourceSize.isEmpty()) {
        // For "stretch" rules, just override the scale factor and replace. We only have to do this for the center tile,
        // since sides don't even use the scale factor unless they have a rule other than "stretch". The middle however
        // can have "stretch" specified in one axis but not the other, so we have to correct the scale here.
        if (m_horizontalTileRule == (Image::TileRule)StretchImageRule)
            middleScaleFactor.setWidth((float) destinationSize.width() / sourceSize.width());

        if (m_verticalTileRule == (Image::TileRule)StretchImageRule)
            middleScaleFactor.setHeight((float) destinationSize.height() / sourceSize.height());
    }

    drawInfo.tileScale = middleScaleFactor;
    drawInfo.tileRule = { m_horizontalTileRule, m_verticalTileRule };
}

NinePieceImageGrid::NinePieceDrawInfo NinePieceImageGrid::getNinePieceDrawInfo(NinePiece piece) const
{
    NinePieceDrawInfo drawInfo;
    drawInfo.isCornerPiece =
        piece == TopLeftPiece || piece == TopRightPiece || piece == BottomLeftPiece || piece == BottomRightPiece;

    if (drawInfo.isCornerPiece)
        setDrawInfoCorner(drawInfo, piece);
    else if (piece != MiddlePiece)
        setDrawInfoEdge(drawInfo, piece);
    else
        setDrawInfoMiddle(drawInfo);

    return drawInfo;
}

} // namespace blink

/*
 * Copyright (C) 2012 Adobe Systems Incorporated. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/shapes/ShapeOutsideInfo.h"

#include "core/inspector/ConsoleMessage.h"
#include "core/layout/FloatingObjects.h"
#include "core/layout/LayoutBlockFlow.h"
#include "core/layout/LayoutBox.h"
#include "core/layout/LayoutImage.h"
#include "platform/LengthFunctions.h"
#include "public/platform/Platform.h"

namespace blink {

CSSBoxType referenceBox(const ShapeValue& shapeValue)
{
    if (shapeValue.cssBox() == BoxMissing)
        return MarginBox;
    return shapeValue.cssBox();
}

void ShapeOutsideInfo::setReferenceBoxLogicalSize(LayoutSize newReferenceBoxLogicalSize)
{
    bool isHorizontalWritingMode = m_layoutBox.containingBlock()->style()->isHorizontalWritingMode();
    switch (referenceBox(*m_layoutBox.style()->shapeOutside())) {
    case MarginBox:
        if (isHorizontalWritingMode)
            newReferenceBoxLogicalSize.expand(m_layoutBox.marginWidth(), m_layoutBox.marginHeight());
        else
            newReferenceBoxLogicalSize.expand(m_layoutBox.marginHeight(), m_layoutBox.marginWidth());
        break;
    case BorderBox:
        break;
    case PaddingBox:
        if (isHorizontalWritingMode)
            newReferenceBoxLogicalSize.shrink(m_layoutBox.borderWidth(), m_layoutBox.borderHeight());
        else
            newReferenceBoxLogicalSize.shrink(m_layoutBox.borderHeight(), m_layoutBox.borderWidth());
        break;
    case ContentBox:
        if (isHorizontalWritingMode)
            newReferenceBoxLogicalSize.shrink(m_layoutBox.borderAndPaddingWidth(), m_layoutBox.borderAndPaddingHeight());
        else
            newReferenceBoxLogicalSize.shrink(m_layoutBox.borderAndPaddingHeight(), m_layoutBox.borderAndPaddingWidth());
        break;
    case BoxMissing:
        ASSERT_NOT_REACHED();
        break;
    }

    newReferenceBoxLogicalSize.clampNegativeToZero();

    if (m_referenceBoxLogicalSize == newReferenceBoxLogicalSize)
        return;
    markShapeAsDirty();
    m_referenceBoxLogicalSize = newReferenceBoxLogicalSize;
}

static bool checkShapeImageOrigin(Document& document, const StyleImage& styleImage)
{
    if (styleImage.isGeneratedImage())
        return true;

    ASSERT(styleImage.cachedImage());
    ImageResource& imageResource = *(styleImage.cachedImage());
    if (imageResource.isAccessAllowed(document.securityOrigin()))
        return true;

    const KURL& url = imageResource.url();
    String urlString = url.isNull() ? "''" : url.elidedString();
    document.addConsoleMessage(ConsoleMessage::create(SecurityMessageSource, ErrorMessageLevel, "Unsafe attempt to load URL " + urlString + "."));

    return false;
}

static LayoutRect getShapeImageMarginRect(const LayoutBox& layoutBox, const LayoutSize& referenceBoxLogicalSize)
{
    LayoutPoint marginBoxOrigin(-layoutBox.marginLogicalLeft() - layoutBox.borderAndPaddingLogicalLeft(), -layoutBox.marginBefore() - layoutBox.borderBefore() - layoutBox.paddingBefore());
    LayoutSize marginBoxSizeDelta(layoutBox.marginLogicalWidth() + layoutBox.borderAndPaddingLogicalWidth(), layoutBox.marginLogicalHeight() + layoutBox.borderAndPaddingLogicalHeight());
    LayoutSize marginRectSize(referenceBoxLogicalSize + marginBoxSizeDelta);
    marginRectSize.clampNegativeToZero();
    return LayoutRect(marginBoxOrigin, marginRectSize);
}

static bool isValidRasterShapeRect(const LayoutRect& rect)
{
    static double maxImageSizeBytes = 0;
    if (!maxImageSizeBytes) {
        size_t size32MaxBytes =  0xFFFFFFFF / 4; // Some platforms don't limit maxDecodedImageBytes.
        maxImageSizeBytes = std::min(size32MaxBytes, Platform::current()->maxDecodedImageBytes());
    }
    return (rect.width().toFloat() * rect.height().toFloat() * 4.0) < maxImageSizeBytes;
}

PassOwnPtr<Shape> ShapeOutsideInfo::createShapeForImage(StyleImage* styleImage, float shapeImageThreshold, WritingMode writingMode, float margin) const
{
    const IntSize& imageSize = m_layoutBox.calculateImageIntrinsicDimensions(styleImage, roundedIntSize(m_referenceBoxLogicalSize), LayoutImage::ScaleByEffectiveZoom);
    styleImage->setContainerSizeForLayoutObject(&m_layoutBox, imageSize, m_layoutBox.style()->effectiveZoom());

    const LayoutRect& marginRect = getShapeImageMarginRect(m_layoutBox, m_referenceBoxLogicalSize);
    const LayoutRect& imageRect = (m_layoutBox.isLayoutImage())
        ? toLayoutImage(m_layoutBox).replacedContentRect()
        : LayoutRect(LayoutPoint(), LayoutSize(imageSize));

    if (!isValidRasterShapeRect(marginRect) || !isValidRasterShapeRect(imageRect)) {
        m_layoutBox.document().addConsoleMessage(ConsoleMessage::create(RenderingMessageSource, ErrorMessageLevel, "The shape-outside image is too large."));
        return Shape::createEmptyRasterShape(writingMode, margin);
    }

    ASSERT(!styleImage->isPendingImage());
    RefPtr<Image> image = styleImage->image(const_cast<LayoutBox*>(&m_layoutBox), imageSize);

    return Shape::createRasterShape(image.get(), shapeImageThreshold, imageRect, marginRect, writingMode, margin);
}

const Shape& ShapeOutsideInfo::computedShape() const
{
    if (Shape* shape = m_shape.get())
        return *shape;

    TemporaryChange<bool> isInComputingShape(m_isComputingShape, true);

    const ComputedStyle& style = *m_layoutBox.style();
    ASSERT(m_layoutBox.containingBlock());
    const ComputedStyle& containingBlockStyle = *m_layoutBox.containingBlock()->style();

    WritingMode writingMode = containingBlockStyle.writingMode();
    // Make sure contentWidth is not negative. This can happen when containing block has a vertical scrollbar and
    // its content is smaller than the scrollbar width.
    LayoutUnit maximumValue = m_layoutBox.containingBlock() ? std::max(LayoutUnit(), m_layoutBox.containingBlock()->contentWidth()) : LayoutUnit();
    float margin = floatValueForLength(m_layoutBox.style()->shapeMargin(), maximumValue.toFloat());

    float shapeImageThreshold = style.shapeImageThreshold();
    ASSERT(style.shapeOutside());
    const ShapeValue& shapeValue = *style.shapeOutside();

    switch (shapeValue.type()) {
    case ShapeValue::Shape:
        ASSERT(shapeValue.shape());
        m_shape = Shape::createShape(shapeValue.shape(), m_referenceBoxLogicalSize, writingMode, margin);
        break;
    case ShapeValue::Image:
        ASSERT(shapeValue.isImageValid());
        m_shape = createShapeForImage(shapeValue.image(), shapeImageThreshold, writingMode, margin);
        break;
    case ShapeValue::Box: {
        const FloatRoundedRect& shapeRect = style.getRoundedBorderFor(LayoutRect(LayoutPoint(), m_referenceBoxLogicalSize), m_layoutBox.view());
        m_shape = Shape::createLayoutBoxShape(shapeRect, writingMode, margin);
        break;
    }
    }

    ASSERT(m_shape);
    return *m_shape;
}

inline LayoutUnit borderBeforeInWritingMode(const LayoutBox& layoutBox, WritingMode writingMode)
{
    switch (writingMode) {
    case TopToBottomWritingMode: return layoutBox.borderTop();
    case BottomToTopWritingMode: return layoutBox.borderBottom();
    case LeftToRightWritingMode: return layoutBox.borderLeft();
    case RightToLeftWritingMode: return layoutBox.borderRight();
    }

    ASSERT_NOT_REACHED();
    return layoutBox.borderBefore();
}

inline LayoutUnit borderAndPaddingBeforeInWritingMode(const LayoutBox& layoutBox, WritingMode writingMode)
{
    switch (writingMode) {
    case TopToBottomWritingMode: return layoutBox.borderTop() + layoutBox.paddingTop();
    case BottomToTopWritingMode: return layoutBox.borderBottom() + layoutBox.paddingBottom();
    case LeftToRightWritingMode: return layoutBox.borderLeft() + layoutBox.paddingLeft();
    case RightToLeftWritingMode: return layoutBox.borderRight() + layoutBox.paddingRight();
    }

    ASSERT_NOT_REACHED();
    return layoutBox.borderAndPaddingBefore();
}

LayoutUnit ShapeOutsideInfo::logicalTopOffset() const
{
    switch (referenceBox(*m_layoutBox.style()->shapeOutside())) {
    case MarginBox: return -m_layoutBox.marginBefore(m_layoutBox.containingBlock()->style());
    case BorderBox: return LayoutUnit();
    case PaddingBox: return borderBeforeInWritingMode(m_layoutBox, m_layoutBox.containingBlock()->style()->writingMode());
    case ContentBox: return borderAndPaddingBeforeInWritingMode(m_layoutBox, m_layoutBox.containingBlock()->style()->writingMode());
    case BoxMissing: break;
    }

    ASSERT_NOT_REACHED();
    return LayoutUnit();
}

inline LayoutUnit borderStartWithStyleForWritingMode(const LayoutBox& layoutBox, const ComputedStyle* style)
{
    if (style->isHorizontalWritingMode()) {
        if (style->isLeftToRightDirection())
            return layoutBox.borderLeft();

        return layoutBox.borderRight();
    }
    if (style->isLeftToRightDirection())
        return layoutBox.borderTop();

    return layoutBox.borderBottom();
}

inline LayoutUnit borderAndPaddingStartWithStyleForWritingMode(const LayoutBox& layoutBox, const ComputedStyle* style)
{
    if (style->isHorizontalWritingMode()) {
        if (style->isLeftToRightDirection())
            return layoutBox.borderLeft() + layoutBox.paddingLeft();

        return layoutBox.borderRight() + layoutBox.paddingRight();
    }
    if (style->isLeftToRightDirection())
        return layoutBox.borderTop() + layoutBox.paddingTop();

    return layoutBox.borderBottom() + layoutBox.paddingBottom();
}

LayoutUnit ShapeOutsideInfo::logicalLeftOffset() const
{
    switch (referenceBox(*m_layoutBox.style()->shapeOutside())) {
    case MarginBox: return -m_layoutBox.marginStart(m_layoutBox.containingBlock()->style());
    case BorderBox: return LayoutUnit();
    case PaddingBox: return borderStartWithStyleForWritingMode(m_layoutBox, m_layoutBox.containingBlock()->style());
    case ContentBox: return borderAndPaddingStartWithStyleForWritingMode(m_layoutBox, m_layoutBox.containingBlock()->style());
    case BoxMissing: break;
    }

    ASSERT_NOT_REACHED();
    return LayoutUnit();
}


bool ShapeOutsideInfo::isEnabledFor(const LayoutBox& box)
{
    ShapeValue* shapeValue = box.style()->shapeOutside();
    if (!box.isFloating() || !shapeValue)
        return false;

    switch (shapeValue->type()) {
    case ShapeValue::Shape:
        return shapeValue->shape();
    case ShapeValue::Image:
        return shapeValue->isImageValid() && checkShapeImageOrigin(box.document(), *(shapeValue->image()));
    case ShapeValue::Box:
        return true;
    }

    return false;
}

ShapeOutsideDeltas ShapeOutsideInfo::computeDeltasForContainingBlockLine(const LineLayoutBlockFlow& containingBlock, const FloatingObject& floatingObject, LayoutUnit lineTop, LayoutUnit lineHeight)
{
    ASSERT(lineHeight >= 0);

    LayoutUnit borderBoxTop = containingBlock.logicalTopForFloat(floatingObject) + containingBlock.marginBeforeForChild(m_layoutBox);
    LayoutUnit borderBoxLineTop = lineTop - borderBoxTop;

    if (isShapeDirty() || !m_shapeOutsideDeltas.isForLine(borderBoxLineTop, lineHeight)) {
        LayoutUnit referenceBoxLineTop = borderBoxLineTop - logicalTopOffset();
        LayoutUnit floatMarginBoxWidth = std::max(containingBlock.logicalWidthForFloat(floatingObject), LayoutUnit());

        if (computedShape().lineOverlapsShapeMarginBounds(referenceBoxLineTop, lineHeight)) {
            LineSegment segment = computedShape().getExcludedInterval((borderBoxLineTop - logicalTopOffset()), std::min(lineHeight, shapeLogicalBottom() - borderBoxLineTop));
            if (segment.isValid) {
                LayoutUnit logicalLeftMargin = containingBlock.style()->isLeftToRightDirection() ? containingBlock.marginStartForChild(m_layoutBox) : containingBlock.marginEndForChild(m_layoutBox);
                LayoutUnit rawLeftMarginBoxDelta = segment.logicalLeft + logicalLeftOffset() + logicalLeftMargin;
                LayoutUnit leftMarginBoxDelta = clampTo<LayoutUnit>(rawLeftMarginBoxDelta, LayoutUnit(), floatMarginBoxWidth);

                LayoutUnit logicalRightMargin = containingBlock.style()->isLeftToRightDirection() ? containingBlock.marginEndForChild(m_layoutBox) : containingBlock.marginStartForChild(m_layoutBox);
                LayoutUnit rawRightMarginBoxDelta = segment.logicalRight + logicalLeftOffset() - containingBlock.logicalWidthForChild(m_layoutBox) - logicalRightMargin;
                LayoutUnit rightMarginBoxDelta = clampTo<LayoutUnit>(rawRightMarginBoxDelta, -floatMarginBoxWidth, LayoutUnit());

                m_shapeOutsideDeltas = ShapeOutsideDeltas(leftMarginBoxDelta, rightMarginBoxDelta, true, borderBoxLineTop, lineHeight);
                return m_shapeOutsideDeltas;
            }
        }

        // Lines that do not overlap the shape should act as if the float
        // wasn't there for layout purposes. So we set the deltas to remove the
        // entire width of the float.
        m_shapeOutsideDeltas = ShapeOutsideDeltas(floatMarginBoxWidth, -floatMarginBoxWidth, false, borderBoxLineTop, lineHeight);
    }

    return m_shapeOutsideDeltas;
}

LayoutRect ShapeOutsideInfo::computedShapePhysicalBoundingBox() const
{
    LayoutRect physicalBoundingBox = computedShape().shapeMarginLogicalBoundingBox();
    physicalBoundingBox.setX(physicalBoundingBox.x() + logicalLeftOffset());

    if (m_layoutBox.style()->isFlippedBlocksWritingMode())
        physicalBoundingBox.setY(m_layoutBox.logicalHeight() - physicalBoundingBox.maxY());
    else
        physicalBoundingBox.setY(physicalBoundingBox.y() + logicalTopOffset());

    if (!m_layoutBox.style()->isHorizontalWritingMode())
        physicalBoundingBox = physicalBoundingBox.transposedRect();
    else
        physicalBoundingBox.setY(physicalBoundingBox.y() + logicalTopOffset());

    return physicalBoundingBox;
}

FloatPoint ShapeOutsideInfo::shapeToLayoutObjectPoint(FloatPoint point) const
{
    FloatPoint result = FloatPoint(point.x() + logicalLeftOffset(), point.y() + logicalTopOffset());
    if (m_layoutBox.style()->isFlippedBlocksWritingMode())
        result.setY(m_layoutBox.logicalHeight() - result.y());
    if (!m_layoutBox.style()->isHorizontalWritingMode())
        result = result.transposedPoint();
    return result;
}

FloatSize ShapeOutsideInfo::shapeToLayoutObjectSize(FloatSize size) const
{
    if (!m_layoutBox.style()->isHorizontalWritingMode())
        return size.transposedSize();
    return size;
}

} // namespace blink

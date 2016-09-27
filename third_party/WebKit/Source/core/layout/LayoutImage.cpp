/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 *           (C) 2006 Allan Sandfeld Jensen (kde@carewolf.com)
 *           (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
 * Copyright (C) 2003, 2004, 2005, 2006, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) Research In Motion Limited 2011-2012. All rights reserved.
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
#include "core/layout/LayoutImage.h"

#include "core/HTMLNames.h"
#include "core/editing/FrameSelection.h"
#include "core/fetch/ImageResource.h"
#include "core/fetch/ResourceLoadPriorityOptimizer.h"
#include "core/fetch/ResourceLoader.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLAreaElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLMapElement.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutView.h"
#include "core/layout/TextRunConstructor.h"
#include "core/page/Page.h"
#include "core/paint/ImagePainter.h"
#include "core/svg/graphics/SVGImage.h"
#include "platform/fonts/Font.h"
#include "platform/fonts/FontCache.h"

namespace blink {

using namespace HTMLNames;

LayoutImage::LayoutImage(Element* element)
    : LayoutReplaced(element, LayoutSize())
    , m_didIncrementVisuallyNonEmptyPixelCount(false)
    , m_isGeneratedContent(false)
    , m_imageDevicePixelRatio(1.0f)
{
    ResourceLoadPriorityOptimizer::resourceLoadPriorityOptimizer()->addLayoutObject(this);
}

LayoutImage* LayoutImage::createAnonymous(Document* document)
{
    LayoutImage* image = new LayoutImage(nullptr);
    image->setDocumentForAnonymous(document);
    return image;
}

LayoutImage::~LayoutImage()
{
}

void LayoutImage::willBeDestroyed()
{
    ASSERT(m_imageResource);
    m_imageResource->shutdown();
    LayoutReplaced::willBeDestroyed();
}

void LayoutImage::setImageResource(PassOwnPtr<LayoutImageResource> imageResource)
{
    ASSERT(!m_imageResource);
    m_imageResource = imageResource;
    m_imageResource->initialize(this);
}

void LayoutImage::imageChanged(WrappedImagePtr newImage, const IntRect* rect)
{
    if (documentBeingDestroyed())
        return;

    if (hasBoxDecorationBackground() || hasMask() || hasShapeOutside())
        LayoutReplaced::imageChanged(newImage, rect);

    if (!m_imageResource)
        return;

    if (newImage != m_imageResource->imagePtr())
        return;

    // Per the spec, we let the server-sent header override srcset/other sources of dpr.
    // https://github.com/igrigorik/http-client-hints/blob/master/draft-grigorik-http-client-hints-01.txt#L255
    if (m_imageResource->cachedImage() && m_imageResource->cachedImage()->hasDevicePixelRatioHeaderValue())
        m_imageDevicePixelRatio = 1 / m_imageResource->cachedImage()->devicePixelRatioHeaderValue();

    if (!m_didIncrementVisuallyNonEmptyPixelCount) {
        // At a zoom level of 1 the image is guaranteed to have an integer size.
        view()->frameView()->incrementVisuallyNonEmptyPixelCount(flooredIntSize(m_imageResource->imageSize(1.0f)));
        m_didIncrementVisuallyNonEmptyPixelCount = true;
    }

    invalidatePaintAndMarkForLayoutIfNeeded();
}

void LayoutImage::updateIntrinsicSizeIfNeeded(const LayoutSize& newSize)
{
    if (m_imageResource->errorOccurred() || !m_imageResource->hasImage())
        return;
    setIntrinsicSize(newSize);
}

void LayoutImage::updateInnerContentRect()
{
    // Propagate container size to the image resource.
    LayoutRect containerRect = replacedContentRect();
    IntSize containerSize(containerRect.width(), containerRect.height());
    if (!containerSize.isEmpty())
        m_imageResource->setContainerSizeForLayoutObject(containerSize);
}

void LayoutImage::invalidatePaintAndMarkForLayoutIfNeeded()
{
    LayoutSize oldIntrinsicSize = intrinsicSize();
    LayoutSize newIntrinsicSize = m_imageResource->intrinsicSize(style()->effectiveZoom());
    updateIntrinsicSizeIfNeeded(newIntrinsicSize);

    // In the case of generated image content using :before/:after/content, we might not be
    // in the layout tree yet. In that case, we just need to update our intrinsic size.
    // layout() will be called after we are inserted in the tree which will take care of
    // what we are doing here.
    if (!containingBlock())
        return;

    bool imageSourceHasChangedSize = oldIntrinsicSize != newIntrinsicSize;
    if (imageSourceHasChangedSize)
        setPreferredLogicalWidthsDirty();

    // If the actual area occupied by the image has changed and it is not constrained by style then a layout is required.
    bool imageSizeIsConstrained = style()->logicalWidth().isSpecified() && style()->logicalHeight().isSpecified();

    // FIXME: We only need to recompute the containing block's preferred size if the containing block's size
    // depends on the image's size (i.e., the container uses shrink-to-fit sizing).
    // There's no easy way to detect that shrink-to-fit is needed, always force a layout.
    bool containingBlockNeedsToRecomputePreferredSize = style()->logicalWidth().hasPercent() || style()->logicalMaxWidth().hasPercent()  || style()->logicalMinWidth().hasPercent();

    if (imageSourceHasChangedSize && (!imageSizeIsConstrained || containingBlockNeedsToRecomputePreferredSize)) {
        setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::SizeChanged);
        return;
    }

    // The image hasn't changed in size or its style constrains its size, so a paint invalidation will suffice.
    if (everHadLayout() && !selfNeedsLayout()) {
        // The inner content rectangle is calculated during layout, but may need an update now
        // (unless the box has already been scheduled for layout). In order to calculate it, we
        // may need values from the containing block, though, so make sure that we're not too
        // early. It may be that layout hasn't even taken place once yet.
        updateInnerContentRect();
    }

    if (imageResource() && imageResource()->image() && imageResource()->image()->maybeAnimated())
        setShouldDoFullPaintInvalidation(PaintInvalidationDelayedFull);
    else
        setShouldDoFullPaintInvalidation(PaintInvalidationFull);

    // Tell any potential compositing layers that the image needs updating.
    contentChanged(ImageChanged);
}

void LayoutImage::notifyFinished(Resource* newImage)
{
    if (!m_imageResource)
        return;

    if (documentBeingDestroyed())
        return;

    invalidateBackgroundObscurationStatus();

    if (newImage == m_imageResource->cachedImage()) {
        // tell any potential compositing layers
        // that the image is done and they can reference it directly.
        contentChanged(ImageChanged);
    }
}

void LayoutImage::paintReplaced(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    ImagePainter(*this).paintReplaced(paintInfo, paintOffset);
}

void LayoutImage::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    ImagePainter(*this).paint(paintInfo, paintOffset);
}

void LayoutImage::areaElementFocusChanged(HTMLAreaElement* areaElement)
{
    ASSERT(areaElement->imageElement() == node());

    Path path = areaElement->computePath(this);
    if (path.isEmpty())
        return;

    invalidatePaintAndMarkForLayoutIfNeeded();
}

bool LayoutImage::boxShadowShouldBeAppliedToBackground(BackgroundBleedAvoidance bleedAvoidance, InlineFlowBox*) const
{
    if (!LayoutBoxModelObject::boxShadowShouldBeAppliedToBackground(bleedAvoidance))
        return false;

    return !const_cast<LayoutImage*>(this)->boxDecorationBackgroundIsKnownToBeObscured();
}

bool LayoutImage::foregroundIsKnownToBeOpaqueInRect(const LayoutRect& localRect, unsigned) const
{
    if (!m_imageResource->hasImage() || m_imageResource->errorOccurred())
        return false;
    if (m_imageResource->cachedImage() && !m_imageResource->cachedImage()->isLoaded())
        return false;
    if (!contentBoxRect().contains(localRect))
        return false;
    EFillBox backgroundClip = style()->backgroundClip();
    // Background paints under borders.
    if (backgroundClip == BorderFillBox && style()->hasBorder() && !style()->borderObscuresBackground())
        return false;
    // Background shows in padding area.
    if ((backgroundClip == BorderFillBox || backgroundClip == PaddingFillBox) && style()->hasPadding())
        return false;
    // Object-position may leave parts of the content box empty, regardless of the value of object-fit.
    if (style()->objectPosition() != ComputedStyle::initialObjectPosition())
        return false;
    // Object-fit may leave parts of the content box empty.
    ObjectFit objectFit = style()->objectFit();
    if (objectFit != ObjectFitFill && objectFit != ObjectFitCover)
        return false;
    // Check for image with alpha.
    return m_imageResource->cachedImage() && m_imageResource->cachedImage()->currentFrameKnownToBeOpaque(this);
}

bool LayoutImage::computeBackgroundIsKnownToBeObscured()
{
    if (!hasBackground())
        return false;

    LayoutRect paintedExtent;
    if (!getBackgroundPaintedExtent(paintedExtent))
        return false;
    return foregroundIsKnownToBeOpaqueInRect(paintedExtent, 0);
}

LayoutUnit LayoutImage::minimumReplacedHeight() const
{
    return m_imageResource->errorOccurred() ? intrinsicSize().height() : LayoutUnit();
}

HTMLMapElement* LayoutImage::imageMap() const
{
    HTMLImageElement* i = isHTMLImageElement(node()) ? toHTMLImageElement(node()) : 0;
    return i ? i->treeScope().getImageMap(i->fastGetAttribute(usemapAttr)) : 0;
}

bool LayoutImage::nodeAtPoint(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction hitTestAction)
{
    HitTestResult tempResult(result.hitTestRequest(), result.hitTestLocation());
    bool inside = LayoutReplaced::nodeAtPoint(tempResult, locationInContainer, accumulatedOffset, hitTestAction);

    if (!inside && result.hitTestRequest().listBased())
        result.append(tempResult);
    if (inside)
        result = tempResult;
    return inside;
}

void LayoutImage::layout()
{
    LayoutReplaced::layout();
    updateInnerContentRect();
}

bool LayoutImage::updateImageLoadingPriorities()
{
    if (!m_imageResource || !m_imageResource->cachedImage() || m_imageResource->cachedImage()->isLoaded())
        return false;

    LayoutRect viewBounds = viewRect();
    LayoutRect objectBounds = LayoutRect(absoluteContentBox());

    // The object bounds might be empty right now, so intersects will fail since it doesn't deal
    // with empty rects. Use LayoutRect::contains in that case.
    bool isVisible;
    if (!objectBounds.isEmpty())
        isVisible =  viewBounds.intersects(objectBounds);
    else
        isVisible = viewBounds.contains(objectBounds);

    ResourceLoadPriorityOptimizer::VisibilityStatus status = isVisible ?
        ResourceLoadPriorityOptimizer::Visible : ResourceLoadPriorityOptimizer::NotVisible;

    LayoutRect screenArea;
    if (!objectBounds.isEmpty()) {
        screenArea = viewBounds;
        screenArea.intersect(objectBounds);
    }

    ResourceLoadPriorityOptimizer::resourceLoadPriorityOptimizer()->notifyImageResourceVisibility(m_imageResource->cachedImage(), status, screenArea);

    return true;
}

void LayoutImage::computeIntrinsicRatioInformation(FloatSize& intrinsicSize, double& intrinsicRatio) const
{
    LayoutReplaced::computeIntrinsicRatioInformation(intrinsicSize, intrinsicRatio);

    // Our intrinsicSize is empty if we're laying out generated images with relative width/height. Figure out the right intrinsic size to use.
    if (intrinsicSize.isEmpty() && (m_imageResource->imageHasRelativeWidth() || m_imageResource->imageHasRelativeHeight())) {
        LayoutObject* containingBlock = isOutOfFlowPositioned() ? container() : this->containingBlock();
        if (containingBlock->isBox()) {
            LayoutBox* box = toLayoutBox(containingBlock);
            intrinsicSize.setWidth(box->availableLogicalWidth().toFloat());
            intrinsicSize.setHeight(box->availableLogicalHeight(IncludeMarginBorderPadding).toFloat());
        }
    }
    // Don't compute an intrinsic ratio to preserve historical WebKit behavior if we're painting alt text and/or a broken image.
    // Video is excluded from this behavior because video elements have a default aspect ratio that a failed poster image load should not override.
    if (m_imageResource && m_imageResource->errorOccurred() && !isVideo()) {
        intrinsicRatio = 1;
        return;
    }
}

bool LayoutImage::needsPreferredWidthsRecalculation() const
{
    if (LayoutReplaced::needsPreferredWidthsRecalculation())
        return true;
    return embeddedContentBox();
}

LayoutBox* LayoutImage::embeddedContentBox() const
{
    if (!m_imageResource)
        return nullptr;

    ImageResource* cachedImage = m_imageResource->cachedImage();
    if (cachedImage && cachedImage->image() && cachedImage->image()->isSVGImage())
        return toSVGImage(cachedImage->image())->embeddedContentBox();

    return nullptr;
}

} // namespace blink

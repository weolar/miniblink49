/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/style/StyleFetchedImageSet.h"

#include "core/css/CSSImageSetValue.h"
#include "core/fetch/ImageResource.h"
#include "core/layout/LayoutObject.h"

namespace blink {

StyleFetchedImageSet::StyleFetchedImageSet(ImageResource* image, float imageScaleFactor, CSSImageSetValue* value)
    : m_bestFitImage(image)
    , m_imageScaleFactor(imageScaleFactor)
    , m_imageSetValue(value)
{
    m_isImageResourceSet = true;
    m_bestFitImage->addClient(this);
}


StyleFetchedImageSet::~StyleFetchedImageSet()
{
    m_bestFitImage->removeClient(this);
}

PassRefPtrWillBeRawPtr<CSSValue> StyleFetchedImageSet::cssValue() const
{
    return m_imageSetValue;
}

bool StyleFetchedImageSet::canRender(const LayoutObject& layoutObject, float multiplier) const
{
    return m_bestFitImage->canRender(layoutObject, multiplier);
}

bool StyleFetchedImageSet::isLoaded() const
{
    return m_bestFitImage->isLoaded();
}

bool StyleFetchedImageSet::errorOccurred() const
{
    return m_bestFitImage->errorOccurred();
}

LayoutSize StyleFetchedImageSet::imageSize(const LayoutObject* layoutObject, float multiplier) const
{
    LayoutSize scaledImageSize = m_bestFitImage->imageSizeForLayoutObject(layoutObject, multiplier);
    scaledImageSize.scale(1 / m_imageScaleFactor);
    return scaledImageSize;
}

bool StyleFetchedImageSet::imageHasRelativeWidth() const
{
    return m_bestFitImage->imageHasRelativeWidth();
}

bool StyleFetchedImageSet::imageHasRelativeHeight() const
{
    return m_bestFitImage->imageHasRelativeHeight();
}

void StyleFetchedImageSet::computeIntrinsicDimensions(const LayoutObject*, Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio)
{
    m_bestFitImage->computeIntrinsicDimensions(intrinsicWidth, intrinsicHeight, intrinsicRatio);
}

bool StyleFetchedImageSet::usesImageContainerSize() const
{
    return m_bestFitImage->usesImageContainerSize();
}

void StyleFetchedImageSet::setContainerSizeForLayoutObject(const LayoutObject* layoutObject, const IntSize& imageContainerSize, float imageContainerZoomFactor)
{
    m_bestFitImage->setContainerSizeForLayoutObject(layoutObject, imageContainerSize, imageContainerZoomFactor);
}

void StyleFetchedImageSet::addClient(LayoutObject* layoutObject)
{
    m_bestFitImage->addClient(layoutObject);
}

void StyleFetchedImageSet::removeClient(LayoutObject* layoutObject)
{
    m_bestFitImage->removeClient(layoutObject);
}

PassRefPtr<Image> StyleFetchedImageSet::image(LayoutObject* layoutObject, const IntSize&) const
{
    return m_bestFitImage->imageForLayoutObject(layoutObject);
}

bool StyleFetchedImageSet::knownToBeOpaque(const LayoutObject* layoutObject) const
{
    return m_bestFitImage->currentFrameKnownToBeOpaque(layoutObject);
}

} // namespace blink

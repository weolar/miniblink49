/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
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
#include "core/style/StyleFetchedImage.h"

#include "core/css/CSSImageValue.h"
#include "core/fetch/ImageResource.h"
#include "core/layout/LayoutObject.h"
#include "core/svg/graphics/SVGImage.h"
#include "wtf/RefCountedLeakCounter.h"
#ifndef NDEBUG
#include <set>
std::set<void*>* g_activatingStyleFetchedImage = nullptr;
#endif


namespace blink {

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, styleFetchedImageCounter, ("StyleFetchedImageCounter"));
int gStyleFetchedImageCreate = 0;
int gStyleFetchedImageNotifyFinished = 0;
#endif

StyleFetchedImage::StyleFetchedImage(ImageResource* image, Document* document)
    : m_image(image)
    , m_document(document)
{
    m_isImageResource = true;
    m_image->addClient(this);

#ifndef NDEBUG
    styleFetchedImageCounter.increment();
    ++gStyleFetchedImageCreate;
    if (!g_activatingStyleFetchedImage)
        g_activatingStyleFetchedImage = new std::set<void*>();
    g_activatingStyleFetchedImage->insert(this);
#endif
}

StyleFetchedImage::~StyleFetchedImage()
{
    m_image->removeClient(this);

#ifndef NDEBUG
    styleFetchedImageCounter.decrement();
    g_activatingStyleFetchedImage->erase(this);
#endif
}

PassRefPtrWillBeRawPtr<CSSValue> StyleFetchedImage::cssValue() const
{
    return CSSImageValue::create(m_image->url(), const_cast<StyleFetchedImage*>(this));
}

bool StyleFetchedImage::canRender(const LayoutObject& layoutObject, float multiplier) const
{
    return m_image->canRender(layoutObject, multiplier);
}

bool StyleFetchedImage::isLoaded() const
{
    return m_image->isLoaded();
}

bool StyleFetchedImage::errorOccurred() const
{
    return m_image->errorOccurred();
}

LayoutSize StyleFetchedImage::imageSize(const LayoutObject* layoutObject, float multiplier) const
{
    return m_image->imageSizeForLayoutObject(layoutObject, multiplier);
}

bool StyleFetchedImage::imageHasRelativeWidth() const
{
    return m_image->imageHasRelativeWidth();
}

bool StyleFetchedImage::imageHasRelativeHeight() const
{
    return m_image->imageHasRelativeHeight();
}

void StyleFetchedImage::computeIntrinsicDimensions(const LayoutObject*, Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio)
{
    m_image->computeIntrinsicDimensions(intrinsicWidth, intrinsicHeight, intrinsicRatio);
}

bool StyleFetchedImage::usesImageContainerSize() const
{
    return m_image->usesImageContainerSize();
}

void StyleFetchedImage::setContainerSizeForLayoutObject(const LayoutObject* layoutObject, const IntSize& imageContainerSize, float imageContainerZoomFactor)
{
    m_image->setContainerSizeForLayoutObject(layoutObject, imageContainerSize, imageContainerZoomFactor);
}

void StyleFetchedImage::addClient(LayoutObject* layoutObject)
{
    m_image->addClient(layoutObject);
}

void StyleFetchedImage::removeClient(LayoutObject* layoutObject)
{
    m_image->removeClient(layoutObject);
}

void StyleFetchedImage::notifyFinished(Resource* resource)
{
    if (m_document && m_image && m_image->image() && m_image->image()->isSVGImage())
        toSVGImage(m_image->image())->updateUseCounters(*m_document);
    // Oilpan: do not prolong the Document's lifetime.
    m_document.clear();
#ifndef NDEBUG
    ++gStyleFetchedImageNotifyFinished;
#endif
}

PassRefPtr<Image> StyleFetchedImage::image(LayoutObject* layoutObject, const IntSize&) const
{
    return m_image->imageForLayoutObject(layoutObject);
}

bool StyleFetchedImage::knownToBeOpaque(const LayoutObject* layoutObject) const
{
    return m_image->currentFrameKnownToBeOpaque(layoutObject);
}

}

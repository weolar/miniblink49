/*
 * Copyright (C) 2008 Apple Inc.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/css/CSSImageGeneratorValue.h"

#include "core/css/CSSCanvasValue.h"
#include "core/css/CSSCrossfadeValue.h"
#include "core/css/CSSGradientValue.h"
#include "platform/graphics/Image.h"

namespace blink {

CSSImageGeneratorValue::CSSImageGeneratorValue(ClassType classType)
    : CSSValue(classType)
{
}

CSSImageGeneratorValue::~CSSImageGeneratorValue()
{
}

void CSSImageGeneratorValue::addClient(LayoutObject* layoutObject, const IntSize& size)
{
    ASSERT(layoutObject);
#if !ENABLE(OILPAN)
    ref();
#else
    if (m_clients.isEmpty()) {
        ASSERT(!m_keepAlive);
        m_keepAlive = adoptPtr(new Persistent<CSSImageGeneratorValue>(this));
    }
#endif

    if (!size.isEmpty())
        m_sizes.add(size);

    LayoutObjectSizeCountMap::iterator it = m_clients.find(layoutObject);
    if (it == m_clients.end())
        m_clients.add(layoutObject, SizeAndCount(size, 1));
    else {
        SizeAndCount& sizeCount = it->value;
        ++sizeCount.count;
    }
}

void CSSImageGeneratorValue::removeClient(LayoutObject* layoutObject)
{
    ASSERT(layoutObject);
    LayoutObjectSizeCountMap::iterator it = m_clients.find(layoutObject);
    ASSERT_WITH_SECURITY_IMPLICATION(it != m_clients.end());

    IntSize removedImageSize;
    SizeAndCount& sizeCount = it->value;
    IntSize size = sizeCount.size;
    if (!size.isEmpty()) {
        m_sizes.remove(size);
        if (!m_sizes.contains(size))
            m_images.remove(size);
    }

    if (!--sizeCount.count)
        m_clients.remove(layoutObject);

#if !ENABLE(OILPAN)
    deref();
#else
    if (m_clients.isEmpty()) {
        ASSERT(m_keepAlive);
        m_keepAlive = nullptr;
    }
#endif
}

Image* CSSImageGeneratorValue::getImage(LayoutObject* layoutObject, const IntSize& size)
{
    LayoutObjectSizeCountMap::iterator it = m_clients.find(layoutObject);
    if (it != m_clients.end()) {
        SizeAndCount& sizeCount = it->value;
        IntSize oldSize = sizeCount.size;
        if (oldSize != size) {
            RefPtrWillBeRawPtr<CSSImageGeneratorValue> protect(this);
            removeClient(layoutObject);
            addClient(layoutObject, size);
        }
    }

    // Don't generate an image for empty sizes.
    if (size.isEmpty())
        return 0;

    // Look up the image in our cache.
    return m_images.get(size);
}

void CSSImageGeneratorValue::putImage(const IntSize& size, PassRefPtr<Image> image)
{
    m_images.add(size, image);
}

PassRefPtr<Image> CSSImageGeneratorValue::image(LayoutObject* layoutObject, const IntSize& size)
{
    switch (classType()) {
    case CanvasClass:
        return toCSSCanvasValue(this)->image(layoutObject, size);
    case CrossfadeClass:
        return toCSSCrossfadeValue(this)->image(layoutObject, size);
    case LinearGradientClass:
        return toCSSLinearGradientValue(this)->image(layoutObject, size);
    case RadialGradientClass:
        return toCSSRadialGradientValue(this)->image(layoutObject, size);
    default:
        ASSERT_NOT_REACHED();
    }
    return nullptr;
}

bool CSSImageGeneratorValue::isFixedSize() const
{
    switch (classType()) {
    case CanvasClass:
        return toCSSCanvasValue(this)->isFixedSize();
    case CrossfadeClass:
        return toCSSCrossfadeValue(this)->isFixedSize();
    case LinearGradientClass:
        return toCSSLinearGradientValue(this)->isFixedSize();
    case RadialGradientClass:
        return toCSSRadialGradientValue(this)->isFixedSize();
    default:
        ASSERT_NOT_REACHED();
    }
    return false;
}

IntSize CSSImageGeneratorValue::fixedSize(const LayoutObject* layoutObject)
{
    switch (classType()) {
    case CanvasClass:
        return toCSSCanvasValue(this)->fixedSize(layoutObject);
    case CrossfadeClass:
        return toCSSCrossfadeValue(this)->fixedSize(layoutObject);
    case LinearGradientClass:
        return toCSSLinearGradientValue(this)->fixedSize(layoutObject);
    case RadialGradientClass:
        return toCSSRadialGradientValue(this)->fixedSize(layoutObject);
    default:
        ASSERT_NOT_REACHED();
    }
    return IntSize();
}

bool CSSImageGeneratorValue::isPending() const
{
    switch (classType()) {
    case CrossfadeClass:
        return toCSSCrossfadeValue(this)->isPending();
    case CanvasClass:
        return toCSSCanvasValue(this)->isPending();
    case LinearGradientClass:
        return toCSSLinearGradientValue(this)->isPending();
    case RadialGradientClass:
        return toCSSRadialGradientValue(this)->isPending();
    default:
        ASSERT_NOT_REACHED();
    }
    return false;
}

bool CSSImageGeneratorValue::knownToBeOpaque(const LayoutObject* layoutObject) const
{
    switch (classType()) {
    case CrossfadeClass:
        return toCSSCrossfadeValue(this)->knownToBeOpaque(layoutObject);
    case CanvasClass:
        return false;
    case LinearGradientClass:
        return toCSSLinearGradientValue(this)->knownToBeOpaque(layoutObject);
    case RadialGradientClass:
        return toCSSRadialGradientValue(this)->knownToBeOpaque(layoutObject);
    default:
        ASSERT_NOT_REACHED();
    }
    return false;
}

void CSSImageGeneratorValue::loadSubimages(Document* document)
{
    switch (classType()) {
    case CrossfadeClass:
        toCSSCrossfadeValue(this)->loadSubimages(document);
        break;
    case CanvasClass:
        toCSSCanvasValue(this)->loadSubimages(document);
        break;
    case LinearGradientClass:
        toCSSLinearGradientValue(this)->loadSubimages(document);
        break;
    case RadialGradientClass:
        toCSSRadialGradientValue(this)->loadSubimages(document);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

} // namespace blink

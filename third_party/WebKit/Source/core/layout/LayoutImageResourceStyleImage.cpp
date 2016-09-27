/*
 * Copyright (C) 1999 Lars Knoll <knoll@kde.org>
 * Copyright (C) 1999 Antti Koivisto <koivisto@kde.org>
 * Copyright (C) 2000 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 Allan Sandfeld Jensen <kde@carewolf.com>
 * Copyright (C) 2006 Samuel Weinig <sam.weinig@gmail.com>
 * Copyright (C) 2003, 2004, 2005, 2006, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) 2010 Patrick Gansterer <paroga@paroga.com>
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
#include "core/layout/LayoutImageResourceStyleImage.h"

#include "core/fetch/ImageResource.h"
#include "core/layout/LayoutObject.h"
#include "core/style/StyleFetchedImage.h"

namespace blink {

LayoutImageResourceStyleImage::LayoutImageResourceStyleImage(StyleImage* styleImage)
    : m_styleImage(styleImage)
{
    ASSERT(m_styleImage);
}

LayoutImageResourceStyleImage::~LayoutImageResourceStyleImage()
{
}

void LayoutImageResourceStyleImage::initialize(LayoutObject* layoutObject)
{
    LayoutImageResource::initialize(layoutObject);

    if (m_styleImage->isImageResource())
        m_cachedImage = toStyleFetchedImage(m_styleImage)->cachedImage();

    m_styleImage->addClient(m_layoutObject);
}

void LayoutImageResourceStyleImage::shutdown()
{
    ASSERT(m_layoutObject);
    m_styleImage->removeClient(m_layoutObject);
    m_cachedImage = 0;
}

PassRefPtr<Image> LayoutImageResourceStyleImage::image(int width, int height) const
{
    // Generated content may trigger calls to image() while we're still pending, don't assert but gracefully exit.
    if (m_styleImage->isPendingImage())
        return nullptr;
    return m_styleImage->image(m_layoutObject, IntSize(width, height));
}

void LayoutImageResourceStyleImage::setContainerSizeForLayoutObject(const IntSize& size)
{
    ASSERT(m_layoutObject);
    m_styleImage->setContainerSizeForLayoutObject(m_layoutObject, size, m_layoutObject->style()->effectiveZoom());
}

} // namespace blink

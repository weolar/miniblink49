/*
 * Copyright (C) 1999 Lars Knoll <knoll@kde.org>
 * Copyright (C) 1999 Antti Koivisto <koivisto@kde.org>
 * Copyright (C) 2006 Allan Sandfeld Jensen <kde@carewolf.com>
 * Copyright (C) 2006 Samuel Weinig <sam.weinig@gmail.com>
 * Copyright (C) 2004, 2005, 2006, 2007, 2009, 2010 Apple Inc. All rights reserved.
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

#ifndef LayoutImageResourceStyleImage_h
#define LayoutImageResourceStyleImage_h

#include "core/layout/LayoutImageResource.h"
#include "core/style/StyleImage.h"
#include "wtf/RefPtr.h"

namespace blink {

class LayoutObject;

class LayoutImageResourceStyleImage final : public LayoutImageResource {
public:
    virtual ~LayoutImageResourceStyleImage();

    static PassOwnPtr<LayoutImageResource> create(StyleImage* styleImage)
    {
        return adoptPtr(new LayoutImageResourceStyleImage(styleImage));
    }
    virtual void initialize(LayoutObject*) override;
    virtual void shutdown() override;

    virtual bool hasImage() const override { return true; }
    virtual PassRefPtr<Image> image(int width = 0, int height = 0) const override;
    virtual bool errorOccurred() const override { return m_styleImage->errorOccurred(); }

    virtual void setContainerSizeForLayoutObject(const IntSize&) override;
    virtual bool imageHasRelativeWidth() const override { return m_styleImage->imageHasRelativeWidth(); }
    virtual bool imageHasRelativeHeight() const override { return m_styleImage->imageHasRelativeHeight(); }

    virtual LayoutSize imageSize(float multiplier) const override { return m_styleImage->imageSize(m_layoutObject, multiplier); }
    virtual LayoutSize intrinsicSize(float multiplier) const override { return m_styleImage->imageSize(m_layoutObject, multiplier); }

    virtual WrappedImagePtr imagePtr() const override { return m_styleImage->data(); }

private:
    LayoutImageResourceStyleImage(StyleImage*);
    RefPtr<StyleImage> m_styleImage;
};

} // namespace blink

#endif // LayoutImageStyleImage_h

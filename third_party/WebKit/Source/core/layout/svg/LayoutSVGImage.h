/*
 * Copyright (C) 2006 Alexander Kellett <lypanov@kde.org>
 * Copyright (C) 2006, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2009 Google, Inc.
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
 */

#ifndef LayoutSVGImage_h
#define LayoutSVGImage_h

#include "core/layout/svg/LayoutSVGModelObject.h"

class SkPicture;

namespace blink {

class LayoutImageResource;
class SVGImageElement;

class LayoutSVGImage final : public LayoutSVGModelObject {
public:
    explicit LayoutSVGImage(SVGImageElement*);
    virtual ~LayoutSVGImage();

    virtual void setNeedsBoundariesUpdate() override { m_needsBoundariesUpdate = true; }
    virtual void setNeedsTransformUpdate() override { m_needsTransformUpdate = true; }

    LayoutImageResource* imageResource() { return m_imageResource.get(); }

    virtual const AffineTransform& localToParentTransform() const override { return m_localTransform; }
    RefPtr<const SkPicture>& bufferedForeground() { return m_bufferedForeground; }

    virtual FloatRect objectBoundingBox() const override { return m_objectBoundingBox; }
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectSVGImage || LayoutSVGModelObject::isOfType(type); }

    virtual const char* name() const override { return "LayoutSVGImage"; }

protected:
    virtual void willBeDestroyed() override;

private:
    virtual FloatRect strokeBoundingBox() const override { return m_objectBoundingBox; }

    virtual void addFocusRingRects(Vector<LayoutRect>&, const LayoutPoint& additionalOffset) const override;

    virtual void imageChanged(WrappedImagePtr, const IntRect* = nullptr) override;

    virtual void layout() override;
    virtual void paint(const PaintInfo&, const LayoutPoint&) override;

    void updateBoundingBox();
    void updateImageContainerSize();
    FloatSize computeImageViewportSize(ImageResource&) const;

    virtual bool nodeAtFloatPoint(HitTestResult&, const FloatPoint& pointInParent, HitTestAction) override;

    virtual AffineTransform localTransform() const override { return m_localTransform; }

    bool m_needsBoundariesUpdate : 1;
    bool m_needsTransformUpdate : 1;
    AffineTransform m_localTransform;
    FloatRect m_objectBoundingBox;
    OwnPtr<LayoutImageResource> m_imageResource;

    RefPtr<const SkPicture> m_bufferedForeground;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutSVGImage, isSVGImage());

} // namespace blink

#endif // LayoutSVGImage_h

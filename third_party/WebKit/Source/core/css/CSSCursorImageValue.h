/*
 * Copyright (C) 2006 Rob Buis <buis@kde.org>
 * Copyright (C) 2008 Apple Inc. All right reserved.
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

#ifndef CSSCursorImageValue_h
#define CSSCursorImageValue_h

#include "core/css/CSSImageValue.h"
#include "platform/geometry/IntPoint.h"
#include "wtf/HashSet.h"

namespace blink {

class Element;
class SVGElement;

class CSSCursorImageValue : public CSSValue {
public:
    static PassRefPtrWillBeRawPtr<CSSCursorImageValue> create(PassRefPtrWillBeRawPtr<CSSValue> imageValue, bool hotSpotSpecified, const IntPoint& hotSpot)
    {
        return adoptRefWillBeNoop(new CSSCursorImageValue(imageValue, hotSpotSpecified, hotSpot));
    }

    ~CSSCursorImageValue();

    bool hotSpotSpecified() const { return m_hotSpotSpecified; }

    IntPoint hotSpot() const { return m_hotSpot; }

    String customCSSText() const;

    bool updateIfSVGCursorIsUsed(Element*);
    StyleImage* cachedImage(Document*, float deviceScaleFactor);
    StyleImage* cachedOrPendingImage(float deviceScaleFactor);

#if !ENABLE(OILPAN)
    void removeReferencedElement(SVGElement*);
#endif

    bool equals(const CSSCursorImageValue&) const;

    DECLARE_TRACE_AFTER_DISPATCH();

private:
    CSSCursorImageValue(PassRefPtrWillBeRawPtr<CSSValue> imageValue, bool hotSpotSpecified, const IntPoint& hotSpot);

    bool isSVGCursor() const;
    String cachedImageURL();
    void clearImageResource();

    RefPtrWillBeMember<CSSValue> m_imageValue;

    bool m_hotSpotSpecified;
    IntPoint m_hotSpot;
    RefPtr<StyleImage> m_image;
    bool m_accessedImage;

#if !ENABLE(OILPAN)
    HashSet<SVGElement*> m_referencedElements;
#endif
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSCursorImageValue, isCursorImageValue());

} // namespace blink

#endif // CSSCursorImageValue_h

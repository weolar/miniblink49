/*
 * Copyright (C) 2007 Rob Buis <buis@kde.org>
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

#ifndef SVGViewSpec_h
#define SVGViewSpec_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/svg/SVGFitToViewBox.h"
#include "core/svg/SVGSVGElement.h"
#include "core/svg/SVGZoomAndPan.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGViewSpec final : public RefCountedWillBeGarbageCollectedFinalized<SVGViewSpec>, public ScriptWrappable, public SVGZoomAndPan, public SVGFitToViewBox {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(SVGViewSpec);
public:
#if !ENABLE(OILPAN)
    using RefCounted<SVGViewSpec>::ref;
    using RefCounted<SVGViewSpec>::deref;
#endif

    static PassRefPtrWillBeRawPtr<SVGViewSpec> create(SVGSVGElement* contextElement)
    {
        return adoptRefWillBeNoop(new SVGViewSpec(contextElement));
    }

    bool parseViewSpec(const String&);
    void reset();
    void detachContextElement();
    template<typename T> void inheritViewAttributesFromElement(T*);

    // JS API
    SVGTransformList* transform() { return m_transform ? m_transform->baseValue() : 0; }
    PassRefPtrWillBeRawPtr<SVGTransformListTearOff> transformFromJavascript() { return m_transform ? m_transform->baseVal() : 0; }
    SVGElement* viewTarget() const;
    String viewBoxString() const;
    String preserveAspectRatioString() const;
    String transformString() const;
    String viewTargetString() const { return m_viewTargetString; }
    // override SVGZoomAndPan.setZoomAndPan so can throw exception on write
    void setZoomAndPan(unsigned short value) { } // read only
    void setZoomAndPan(unsigned short value, ExceptionState&);

    DECLARE_VIRTUAL_TRACE();

    SVGSVGElement* contextElement() { return m_contextElement.get(); }

private:
    explicit SVGViewSpec(SVGSVGElement*);

    template<typename CharType>
    bool parseViewSpecInternal(const CharType* ptr, const CharType* end);

    RawPtrWillBeMember<SVGSVGElement> m_contextElement;
    RefPtrWillBeMember<SVGAnimatedTransformList> m_transform;
    String m_viewTargetString;
};

template <typename T>
void SVGViewSpec::inheritViewAttributesFromElement(T* inheritFromElement)
{
    if (!inheritFromElement->hasEmptyViewBox())
        viewBox()->baseValue()->setValue(inheritFromElement->viewBox()->currentValue()->value());

    if (inheritFromElement->preserveAspectRatio()->isSpecified()) {
        preserveAspectRatio()->baseValue()->setAlign(inheritFromElement->preserveAspectRatio()->currentValue()->align());
        preserveAspectRatio()->baseValue()->setMeetOrSlice(inheritFromElement->preserveAspectRatio()->currentValue()->meetOrSlice());
    }

    if (inheritFromElement->hasAttribute(SVGNames::zoomAndPanAttr))
        setZoomAndPan(inheritFromElement->zoomAndPan());
}

} // namespace blink

#endif // SVGViewSpec_h

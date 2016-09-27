/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SVGPathSegListTearOff_h
#define SVGPathSegListTearOff_h

#include "core/SVGNames.h"
#include "core/svg/SVGPathSegList.h"
#include "core/svg/properties/SVGListPropertyTearOffHelper.h"

namespace blink {

template<>
class ListItemPropertyTraits<SVGPathSeg> {
public:
    typedef SVGPathSeg ItemPropertyType;
    // FIXME: Currently SVGPathSegitself is a tear-off.
    typedef SVGPathSeg ItemTearOffType;

    static PassRefPtrWillBeRawPtr<ItemPropertyType> getValueForInsertionFromTearOff(PassRefPtrWillBeRawPtr<ItemTearOffType> passNewItem, SVGElement* contextElement, const QualifiedName& attributeName)
    {
        ASSERT(attributeName == SVGNames::dAttr);
        RefPtrWillBeRawPtr<ItemTearOffType> newItem = passNewItem;

        // Spec: If newItem is already in a list, then a new SVGPathSeg object is created with the same values as newItem and this item is inserted into the list.
        // Otherwise, newItem itself is inserted into the list.
        // https://svgwg.org/svg2-draft/paths.html#InterfaceSVGPathSegList
        if (newItem->ownerList())
            newItem = newItem->clone();

        newItem->setContextElement(contextElement);
        return newItem.release();
    }

    static PassRefPtrWillBeRawPtr<ItemTearOffType> createTearOff(PassRefPtrWillBeRawPtr<ItemPropertyType> passValue, SVGElement* contextElement, PropertyIsAnimValType, const QualifiedName&)
    {
        RefPtrWillBeRawPtr<SVGPathSeg> value = passValue;
        value->setContextElement(contextElement);
        return value.release();
    }
};

class SVGPathSegListTearOff final
    : public SVGListPropertyTearOffHelper<SVGPathSegListTearOff, SVGPathSegList>
    , public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<SVGPathSegListTearOff> create(PassRefPtrWillBeRawPtr<SVGPathSegList> target, SVGElement* contextElement, PropertyIsAnimValType propertyIsAnimVal, const QualifiedName& attributeName = QualifiedName::null())
    {
        return adoptRefWillBeNoop(new SVGPathSegListTearOff(target, contextElement, propertyIsAnimVal, attributeName));
    }

private:
    SVGPathSegListTearOff(PassRefPtrWillBeRawPtr<SVGPathSegList> target, SVGElement* contextElement, PropertyIsAnimValType propertyIsAnimVal, const QualifiedName& attributeName = QualifiedName::null())
        : SVGListPropertyTearOffHelper<SVGPathSegListTearOff, SVGPathSegList>(target, contextElement, propertyIsAnimVal, attributeName) { }
};

} // namespace blink

#endif // SVGPathSegListTearOff_h

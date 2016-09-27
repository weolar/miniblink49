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

#ifndef SVGPathSegList_h
#define SVGPathSegList_h

#include "core/svg/SVGPathByteStream.h"
#include "core/svg/SVGPathSeg.h"
#include "core/svg/properties/SVGAnimatedProperty.h"
#include "core/svg/properties/SVGListPropertyHelper.h"

namespace blink {

class SVGPathElement;
class SVGPathSegListTearOff;

class SVGPathSegList : public SVGListPropertyHelper<SVGPathSegList, SVGPathSeg> {
public:
    typedef void PrimitiveType;
    typedef SVGPathSeg ItemPropertyType;
    typedef SVGPathSegListTearOff TearOffType;
    typedef SVGListPropertyHelper<SVGPathSegList, SVGPathSeg> Base;

    static PassRefPtrWillBeRawPtr<SVGPathSegList> create(SVGPathElement* contextElement)
    {
        return adoptRefWillBeNoop(new SVGPathSegList(contextElement));
    }
    static PassRefPtrWillBeRawPtr<SVGPathSegList> create()
    {
        ASSERT_NOT_REACHED();
        return nullptr;
    }

    ~SVGPathSegList() override;

    const SVGPathByteStream* byteStream() const;
    void clearByteStream() { m_byteStream.clear(); }

    // SVGListPropertyHelper methods with |m_byteStream| sync:

    ItemPropertyType* at(size_t index)
    {
        updateListFromByteStream();
        return Base::at(index);
    }

    size_t length()
    {
        updateListFromByteStream();
        return Base::length();
    }

    bool isEmpty() const
    {
        if (m_listSyncedToByteStream)
            return Base::isEmpty();

        return !m_byteStream || m_byteStream->isEmpty();
    }

    void clear()
    {
        clearByteStream();
        Base::clear();
    }

    void append(PassRefPtrWillBeRawPtr<ItemPropertyType> passNewItem)
    {
        updateListFromByteStream();
        clearByteStream();
        Base::append(passNewItem);
    }

    PassRefPtrWillBeRawPtr<ItemPropertyType> initialize(PassRefPtrWillBeRawPtr<ItemPropertyType> passItem)
    {
        clearByteStream();
        return Base::initialize(passItem);
    }

    PassRefPtrWillBeRawPtr<ItemPropertyType> getItem(size_t index, ExceptionState& exceptionState)
    {
        updateListFromByteStream();
        return Base::getItem(index, exceptionState);
    }

    PassRefPtrWillBeRawPtr<ItemPropertyType> insertItemBefore(PassRefPtrWillBeRawPtr<ItemPropertyType> passItem, size_t index)
    {
        updateListFromByteStream();
        clearByteStream();
        return Base::insertItemBefore(passItem, index);
    }

    PassRefPtrWillBeRawPtr<ItemPropertyType> replaceItem(PassRefPtrWillBeRawPtr<ItemPropertyType> passItem, size_t index, ExceptionState& exceptionState)
    {
        updateListFromByteStream();
        clearByteStream();
        return Base::replaceItem(passItem, index, exceptionState);
    }

    PassRefPtrWillBeRawPtr<ItemPropertyType> removeItem(size_t index, ExceptionState& exceptionState)
    {
        updateListFromByteStream();
        clearByteStream();
        return Base::removeItem(index, exceptionState);
    }

    PassRefPtrWillBeRawPtr<ItemPropertyType> appendItem(PassRefPtrWillBeRawPtr<ItemPropertyType> passItem);

    // SVGPropertyBase:
    PassRefPtrWillBeRawPtr<SVGPropertyBase> cloneForAnimation(const String&) const override;
    PassRefPtrWillBeRawPtr<SVGPathSegList> clone() override;
    String valueAsString() const override;
    void setValueAsString(const String&, ExceptionState&);

    void add(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*) override;
    void calculateAnimatedValue(SVGAnimationElement*, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> fromValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement*) override;
    float calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement*) override;

    static AnimatedPropertyType classType() { return AnimatedPath; }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGPathSegList(SVGPathElement*);
    SVGPathSegList(SVGPathElement*, PassOwnPtr<SVGPathByteStream>);

    friend class SVGPathSegListBuilder;
    // This is only to be called from SVGPathSegListBuilder.
    void appendWithoutByteStreamSync(PassRefPtrWillBeRawPtr<ItemPropertyType> passNewItem)
    {
        Base::append(passNewItem);
    }

    void updateListFromByteStream();
    void invalidateList();

    // FIXME: This pointer should be removed after SVGPathSeg has a tear-off.
    //
    // SVGPathSegList is either owned by SVGAnimatedPath or SVGPathSegListTearOff.
    // Both keep |contextElement| alive, so this ptr is always valid.
    RawPtrWillBeMember<SVGPathElement> m_contextElement;

    mutable OwnPtr<SVGPathByteStream> m_byteStream;
    bool m_listSyncedToByteStream;
};

inline PassRefPtrWillBeRawPtr<SVGPathSegList> toSVGPathSegList(PassRefPtrWillBeRawPtr<SVGPropertyBase> passBase)
{
    RefPtrWillBeRawPtr<SVGPropertyBase> base = passBase;
    ASSERT(base->type() == SVGPathSegList::classType());
    return static_pointer_cast<SVGPathSegList>(base.release());
}

} // namespace blink

#endif

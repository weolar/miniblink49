/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef SVGProperty_h
#define SVGProperty_h

#include "core/svg/properties/SVGPropertyInfo.h"
#include "platform/heap/Handle.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class SVGElement;
class SVGAnimationElement;

class SVGPropertyBase : public RefCountedWillBeGarbageCollectedFinalized<SVGPropertyBase> {
    WTF_MAKE_NONCOPYABLE(SVGPropertyBase);

public:
    // Properties do not have a primitive type by default
    typedef void PrimitiveType;

    virtual ~SVGPropertyBase()
    {
#if !ENABLE(OILPAN)
        // Oilpan: a property can legitimately be swept out along with its list,
        // hence this cannot be made to hold.
        ASSERT(!m_ownerList);
#endif
    }

    // FIXME: remove this in WebAnimations transition.
    // This is used from SVGAnimatedNewPropertyAnimator for its animate-by-string implementation.
    virtual PassRefPtrWillBeRawPtr<SVGPropertyBase> cloneForAnimation(const String&) const = 0;

    virtual String valueAsString() const = 0;

    // FIXME: remove below and just have this inherit AnimatableValue in WebAnimations transition.
    virtual void add(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*) = 0;
    virtual void calculateAnimatedValue(SVGAnimationElement*, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> from, PassRefPtrWillBeRawPtr<SVGPropertyBase> to, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement*) = 0;
    virtual float calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement*) = 0;

    AnimatedPropertyType type()
    {
        return m_type;
    }

    SVGPropertyBase* ownerList() const
    {
        return m_ownerList;
    }

    void setOwnerList(SVGPropertyBase* ownerList)
    {
        // Previous owner list must be cleared before setting new owner list.
        ASSERT((!ownerList && m_ownerList) || (ownerList && !m_ownerList));

        m_ownerList = ownerList;
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_ownerList);
    }

protected:
    explicit SVGPropertyBase(AnimatedPropertyType type)
        : m_type(type)
        , m_ownerList(nullptr)
    {
    }

private:
    const AnimatedPropertyType m_type;

    RawPtrWillBeMember<SVGPropertyBase> m_ownerList;
};

}

#endif // SVGProperty_h

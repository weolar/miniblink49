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

#ifndef SVGIntegerOptionalInteger_h
#define SVGIntegerOptionalInteger_h

#include "core/svg/SVGAnimatedInteger.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGIntegerOptionalInteger : public SVGPropertyBase {
public:
    // Tearoff of SVGIntegerOptionalInteger is never created.
    typedef void TearOffType;
    typedef void PrimitiveType;

    static PassRefPtrWillBeRawPtr<SVGIntegerOptionalInteger> create(PassRefPtrWillBeRawPtr<SVGInteger> firstInteger, PassRefPtrWillBeRawPtr<SVGInteger> secondInteger)
    {
        return adoptRefWillBeNoop(new SVGIntegerOptionalInteger(firstInteger, secondInteger));
    }

    PassRefPtrWillBeRawPtr<SVGIntegerOptionalInteger> clone() const;
    PassRefPtrWillBeRawPtr<SVGPropertyBase> cloneForAnimation(const String&) const override;

    String valueAsString() const override;
    void setValueAsString(const String&, ExceptionState&);

    void add(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*) override;
    void calculateAnimatedValue(SVGAnimationElement*, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> from, PassRefPtrWillBeRawPtr<SVGPropertyBase> to, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement* contextElement) override;
    float calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement* contextElement) override;

    static AnimatedPropertyType classType() { return AnimatedIntegerOptionalInteger; }

    PassRefPtrWillBeRawPtr<SVGInteger> firstInteger() { return m_firstInteger; }
    PassRefPtrWillBeRawPtr<SVGInteger> secondInteger() { return m_secondInteger; }

    DECLARE_VIRTUAL_TRACE();

protected:
    SVGIntegerOptionalInteger(PassRefPtrWillBeRawPtr<SVGInteger> firstInteger, PassRefPtrWillBeRawPtr<SVGInteger> secondInteger);

    RefPtrWillBeMember<SVGInteger> m_firstInteger;
    RefPtrWillBeMember<SVGInteger> m_secondInteger;
};

inline PassRefPtrWillBeRawPtr<SVGIntegerOptionalInteger> toSVGIntegerOptionalInteger(PassRefPtrWillBeRawPtr<SVGPropertyBase> passBase)
{
    RefPtrWillBeRawPtr<SVGPropertyBase> base = passBase;
    ASSERT(base->type() == SVGIntegerOptionalInteger::classType());
    return static_pointer_cast<SVGIntegerOptionalInteger>(base.release());
}

} // namespace blink

#endif // SVGIntegerOptionalInteger_h

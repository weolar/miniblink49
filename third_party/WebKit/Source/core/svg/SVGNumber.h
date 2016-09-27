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

#ifndef SVGNumber_h
#define SVGNumber_h

#include "bindings/core/v8/ExceptionMessages.h"
#include "core/svg/properties/SVGPropertyHelper.h"

namespace blink {

class SVGNumberTearOff;

class SVGNumber : public SVGPropertyHelper<SVGNumber> {
public:
    // SVGNumber has a tear-off type, but SVGAnimatedNumber uses primitive type.
    typedef SVGNumberTearOff TearOffType;
    typedef float PrimitiveType;

    static PassRefPtrWillBeRawPtr<SVGNumber> create(float value = 0.0f)
    {
        return adoptRefWillBeNoop(new SVGNumber(value));
    }

    virtual PassRefPtrWillBeRawPtr<SVGNumber> clone() const;

    float value() const { return m_value; }
    void setValue(float value) { m_value = value; }

    String valueAsString() const override;
    virtual void setValueAsString(const String&, ExceptionState&);

    void add(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*) override;
    void calculateAnimatedValue(SVGAnimationElement*, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> from, PassRefPtrWillBeRawPtr<SVGPropertyBase> to, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement* contextElement) override;
    float calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement* contextElement) override;

    static AnimatedPropertyType classType() { return AnimatedNumber; }

protected:
    explicit SVGNumber(float);

    template<typename CharType>
    bool parse(const CharType*& ptr, const CharType* end);

    float m_value;
};

inline PassRefPtrWillBeRawPtr<SVGNumber> toSVGNumber(PassRefPtrWillBeRawPtr<SVGPropertyBase> passBase)
{
    RefPtrWillBeRawPtr<SVGPropertyBase> base = passBase;
    ASSERT(base->type() == SVGNumber::classType());
    return static_pointer_cast<SVGNumber>(base.release());
}

// SVGNumber which also accepts percentage as its value.
// This is used for <stop> "offset"
// Spec: http://www.w3.org/TR/SVG11/pservers.html#GradientStops
//   offset = "<number> | <percentage>"
class SVGNumberAcceptPercentage final : public SVGNumber {
public:
    static PassRefPtrWillBeRawPtr<SVGNumberAcceptPercentage> create(float value = 0)
    {
        return adoptRefWillBeNoop(new SVGNumberAcceptPercentage(value));
    }

    PassRefPtrWillBeRawPtr<SVGNumber> clone() const override;
    void setValueAsString(const String&, ExceptionState&) override;

private:
    explicit SVGNumberAcceptPercentage(float);
};

} // namespace blink

#endif // SVGNumber_h

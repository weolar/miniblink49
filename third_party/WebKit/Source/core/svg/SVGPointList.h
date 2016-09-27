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

#ifndef SVGPointList_h
#define SVGPointList_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/svg/SVGPoint.h"
#include "core/svg/properties/SVGListPropertyHelper.h"

namespace blink {

class SVGPointListTearOff;

class SVGPointList final : public SVGListPropertyHelper<SVGPointList, SVGPoint> {
public:
    typedef SVGPointListTearOff TearOffType;

    static PassRefPtrWillBeRawPtr<SVGPointList> create()
    {
        return adoptRefWillBeNoop(new SVGPointList());
    }

    ~SVGPointList() override;

    void setValueAsString(const String&, ExceptionState&);

    // SVGPropertyBase:
    String valueAsString() const override;

    void add(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*) override;
    void calculateAnimatedValue(SVGAnimationElement*, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> fromValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement*) override;
    float calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement*) override;

    static AnimatedPropertyType classType() { return AnimatedPoints; }

private:
    SVGPointList();

    template <typename CharType>
    bool parse(const CharType*& ptr, const CharType* end);
};

} // namespace blink

#endif // SVGPointList_h

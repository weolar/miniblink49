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

#ifndef SVGTransformList_h
#define SVGTransformList_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/svg/SVGTransform.h"
#include "core/svg/properties/SVGListPropertyHelper.h"

namespace blink {

class SVGTransformListTearOff;

class SVGTransformList final : public SVGListPropertyHelper<SVGTransformList, SVGTransform> {
public:
    typedef SVGTransformListTearOff TearOffType;

    static PassRefPtrWillBeRawPtr<SVGTransformList> create()
    {
        return adoptRefWillBeNoop(new SVGTransformList());
    }

    static PassRefPtrWillBeRawPtr<SVGTransformList> create(SVGTransformType, const String&);

    ~SVGTransformList() override;

    PassRefPtrWillBeRawPtr<SVGTransform> consolidate();

    bool concatenate(AffineTransform& result) const;

    // SVGPropertyBase:
    PassRefPtrWillBeRawPtr<SVGPropertyBase> cloneForAnimation(const String&) const override;
    String valueAsString() const override;
    void setValueAsString(const String&, ExceptionState&);
    bool parse(const UChar*& ptr, const UChar* end);
    bool parse(const LChar*& ptr, const LChar* end);

    void add(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*) override;
    void calculateAnimatedValue(SVGAnimationElement*, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> fromValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement*) override;
    float calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement*) override;

    static AnimatedPropertyType classType() { return AnimatedTransformList; }

private:
    SVGTransformList();

    template <typename CharType>
    bool parseInternal(const CharType*& ptr, const CharType* end);
};

} // namespace blink

#endif // SVGTransformList_h

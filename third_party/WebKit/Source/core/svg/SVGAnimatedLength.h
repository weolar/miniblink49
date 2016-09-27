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

#ifndef SVGAnimatedLength_h
#define SVGAnimatedLength_h

#include "core/svg/SVGLengthTearOff.h"
#include "core/svg/properties/SVGAnimatedProperty.h"

namespace blink {

class SVGAnimatedLength : public SVGAnimatedProperty<SVGLength> {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<SVGAnimatedLength> create(SVGElement* contextElement, const QualifiedName& attributeName, PassRefPtrWillBeRawPtr<SVGLength> initialValue, SVGLengthNegativeValuesMode negativeValuesMode)
    {
        return adoptRefWillBeNoop(new SVGAnimatedLength(contextElement, attributeName, initialValue, negativeValuesMode));
    }

    void setDefaultValueAsString(const String&);
    void setBaseValueAsString(const String&, SVGParsingError&) override;

    SVGLengthNegativeValuesMode negativeValuesMode() const { return m_negativeValuesMode; }

protected:
    SVGAnimatedLength(SVGElement* contextElement, const QualifiedName& attributeName, PassRefPtrWillBeRawPtr<SVGLength> initialValue, SVGLengthNegativeValuesMode negativeValuesMode)
        : SVGAnimatedProperty<SVGLength>(contextElement, attributeName, initialValue)
        , m_negativeValuesMode(negativeValuesMode)
    {
    }

private:
    SVGLengthNegativeValuesMode m_negativeValuesMode;
};

} // namespace blink

#endif // SVGAnimatedLength_h

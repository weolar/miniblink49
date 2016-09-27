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

#ifndef SVGAnimatedNumber_h
#define SVGAnimatedNumber_h

#include "core/svg/SVGNumberTearOff.h"
#include "core/svg/properties/SVGAnimatedProperty.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGAnimatedNumberOptionalNumber;

// SVG Spec: http://www.w3.org/TR/SVG11/types.html#InterfaceSVGAnimatedNumber
class SVGAnimatedNumber : public SVGAnimatedProperty<SVGNumber> {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<SVGAnimatedNumber> create(SVGElement* contextElement, const QualifiedName& attributeName, PassRefPtrWillBeRawPtr<SVGNumber> initialValue)
    {
        return adoptRefWillBeNoop(new SVGAnimatedNumber(contextElement, attributeName, initialValue));
    }

    void synchronizeAttribute() override;

    void setParentOptionalNumber(SVGAnimatedNumberOptionalNumber* numberOptionalNumber)
    {
        m_parentNumberOptionalNumber = numberOptionalNumber;
    }

    DECLARE_VIRTUAL_TRACE();

protected:
    SVGAnimatedNumber(SVGElement* contextElement, const QualifiedName& attributeName, PassRefPtrWillBeRawPtr<SVGNumber> initialValue)
        : SVGAnimatedProperty<SVGNumber>(contextElement, attributeName, initialValue)
        , m_parentNumberOptionalNumber(nullptr)
    {
    }

    RawPtrWillBeMember<SVGAnimatedNumberOptionalNumber> m_parentNumberOptionalNumber;
};

} // namespace blink

#endif // SVGAnimatedNumber_h

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

#include "config.h"

#include "core/svg/SVGAnimatedIntegerOptionalInteger.h"

#include "core/svg/SVGElement.h"

namespace blink {

SVGAnimatedIntegerOptionalInteger::SVGAnimatedIntegerOptionalInteger(SVGElement* contextElement, const QualifiedName& attributeName, float initialFirstValue, float initialSecondValue)
    : SVGAnimatedPropertyCommon<SVGIntegerOptionalInteger>(contextElement, attributeName,
        SVGIntegerOptionalInteger::create(SVGInteger::create(initialFirstValue), SVGInteger::create(initialSecondValue)))
    , m_firstInteger(SVGAnimatedInteger::create(contextElement, attributeName, baseValue()->firstInteger()))
    , m_secondInteger(SVGAnimatedInteger::create(contextElement, attributeName, baseValue()->secondInteger()))
{
    m_firstInteger->setParentOptionalInteger(this);
    m_secondInteger->setParentOptionalInteger(this);
}

DEFINE_TRACE(SVGAnimatedIntegerOptionalInteger)
{
    visitor->trace(m_firstInteger);
    visitor->trace(m_secondInteger);
    SVGAnimatedPropertyCommon<SVGIntegerOptionalInteger>::trace(visitor);
}

void SVGAnimatedIntegerOptionalInteger::setAnimatedValue(PassRefPtrWillBeRawPtr<SVGPropertyBase> value)
{
    SVGAnimatedPropertyCommon<SVGIntegerOptionalInteger>::setAnimatedValue(value);
    m_firstInteger->setAnimatedValue(currentValue()->firstInteger());
    m_secondInteger->setAnimatedValue(currentValue()->secondInteger());
}

void SVGAnimatedIntegerOptionalInteger::animationEnded()
{
    SVGAnimatedPropertyCommon<SVGIntegerOptionalInteger>::animationEnded();
    m_firstInteger->animationEnded();
    m_secondInteger->animationEnded();
}

bool SVGAnimatedIntegerOptionalInteger::needsSynchronizeAttribute()
{
    return m_firstInteger->needsSynchronizeAttribute()
        || m_secondInteger->needsSynchronizeAttribute();
}

}

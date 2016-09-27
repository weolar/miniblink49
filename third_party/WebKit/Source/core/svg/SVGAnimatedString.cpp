// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/svg/SVGAnimatedString.h"

#include "core/XLinkNames.h"
#include "core/frame/UseCounter.h"
#include "core/svg/SVGElement.h"

namespace blink {

String SVGAnimatedString::baseVal()
{
    if (this->attributeName() == XLinkNames::hrefAttr)
        UseCounter::count(this->contextElement()->document(), UseCounter::SVGHrefBaseVal);
    return SVGAnimatedProperty<SVGString>::baseVal();
}

void SVGAnimatedString::setBaseVal(String value, ExceptionState& exceptionState)
{
    if (this->attributeName() == XLinkNames::hrefAttr)
        UseCounter::count(this->contextElement()->document(), UseCounter::SVGHrefBaseVal);
    return SVGAnimatedProperty<SVGString>::setBaseVal(value, exceptionState);
}

String SVGAnimatedString::animVal()
{
    if (this->attributeName() == XLinkNames::hrefAttr)
        UseCounter::count(this->contextElement()->document(), UseCounter::SVGHrefAnimVal);
    return SVGAnimatedProperty<SVGString>::animVal();
}

} // namespace blink

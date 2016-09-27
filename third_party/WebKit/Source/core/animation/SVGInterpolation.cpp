// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/SVGInterpolation.h"

#include "core/svg/SVGAnimateElement.h" // findElementInstances

namespace blink {

void SVGInterpolation::apply(SVGElement& targetElement) const
{
    SVGElement::InstanceUpdateBlocker blocker(&targetElement);
    RefPtrWillBeRawPtr<SVGPropertyBase> value = interpolatedValue(targetElement);
    SVGElementInstances instances = SVGAnimateElement::findElementInstances(&targetElement);

    for (SVGElement* element : instances) {
        RefPtrWillBeRawPtr<SVGAnimatedPropertyBase> animatedProperty =
            element->propertyFromAttribute(attributeName());
        if (animatedProperty) {
            animatedProperty->setAnimatedValue(value);
            element->invalidateSVGAttributes();
            element->svgAttributeChanged(attributeName());
        }
    }
}

}

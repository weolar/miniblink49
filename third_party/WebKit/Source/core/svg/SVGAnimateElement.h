/*
 * Copyright (C) 2004, 2005 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 * Copyright (C) Research In Motion Limited 2011. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SVGAnimateElement_h
#define SVGAnimateElement_h

#include "core/CoreExport.h"
#include "core/SVGNames.h"
#include "core/svg/SVGAnimatedTypeAnimator.h"
#include "core/svg/SVGAnimationElement.h"
#include "platform/heap/Handle.h"
#include "wtf/OwnPtr.h"
//#include <base/gtest_prod_util.h>

namespace blink {

class SVGAnimatedTypeAnimator;

class CORE_EXPORT SVGAnimateElement : public SVGAnimationElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<SVGAnimateElement> create(Document&);
    ~SVGAnimateElement() override;

    DECLARE_VIRTUAL_TRACE();

    bool isSVGAnimationAttributeSettingJavaScriptURL(const Attribute&) const override;

    AnimatedPropertyType animatedPropertyType();
    bool animatedPropertyTypeSupportsAddition();

    static SVGElementInstances findElementInstances(SVGElement* targetElement);

protected:
    SVGAnimateElement(const QualifiedName&, Document&);

    void resetAnimatedType() final;
    void clearAnimatedType() final;

    bool calculateToAtEndOfDurationValue(const String& toAtEndOfDurationString) final;
    bool calculateFromAndToValues(const String& fromString, const String& toString) final;
    bool calculateFromAndByValues(const String& fromString, const String& byString) final;
    void calculateAnimatedValue(float percentage, unsigned repeatCount, SVGSMILElement* resultElement) final;
    void applyResultsToTarget() final;
    float calculateDistance(const String& fromString, const String& toString) final;
    bool isAdditive() final;

    void setTargetElement(SVGElement*) final;
    void setAttributeName(const QualifiedName&) final;

    //FRIEND_TEST_ALL_PREFIXES(UnsafeSVGAttributeSanitizationTest, stringsShouldNotSupportAddition);

private:
    void resetAnimatedPropertyType();

    bool hasValidAttributeType() override;

    RefPtrWillBeMember<SVGPropertyBase> m_fromProperty;
    RefPtrWillBeMember<SVGPropertyBase> m_toProperty;
    RefPtrWillBeMember<SVGPropertyBase> m_toAtEndOfDurationProperty;
    RefPtrWillBeMember<SVGPropertyBase> m_animatedProperty;

    SVGAnimatedTypeAnimator m_animator;
};

inline bool isSVGAnimateElement(const SVGElement& element)
{
    return element.hasTagName(SVGNames::animateTag)
        || element.hasTagName(SVGNames::animateTransformTag)
        || element.hasTagName(SVGNames::setTag);
}

DEFINE_SVGELEMENT_TYPE_CASTS_WITH_FUNCTION(SVGAnimateElement);

} // namespace blink

#endif // SVGAnimateElement_h

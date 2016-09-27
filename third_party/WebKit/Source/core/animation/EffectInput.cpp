/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
#include "core/animation/EffectInput.h"

#include "bindings/core/v8/Dictionary.h"
#include "bindings/core/v8/UnionTypesCore.h"
#include "core/HTMLNames.h"
#include "core/SVGNames.h"
#include "core/XLinkNames.h"
#include "core/animation/AnimationInputHelpers.h"
#include "core/animation/KeyframeEffectModel.h"
#include "core/animation/StringKeyframe.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/svg/animation/SVGSMILElement.h"
#include "wtf/ASCIICType.h"
#include "wtf/HashSet.h"
#include "wtf/NonCopyingSort.h"

namespace blink {

namespace {

bool svgPrefixed(const String& property)
{
    return property.length() >= 4 && property.startsWith("svg") && isASCIIUpper(property[3]);
}

QualifiedName svgAttributeName(String property)
{
    // Replace 'svgTransform' with 'transform', etc.
    ASSERT(svgPrefixed(property));
    UChar first = toASCIILower(property[3]);
    property.remove(0, 4);
    property.insert(&first, 1, 0);

    if (property == "href")
        return XLinkNames::hrefAttr;

    return QualifiedName(nullAtom, AtomicString(property), SVGNames::amplitudeAttr.namespaceURI());
}

const QualifiedName* supportedSVGAttribute(const String& property, SVGElement* svgElement)
{
    typedef HashMap<QualifiedName, const QualifiedName*> AttributeNameMap;
    DEFINE_STATIC_LOCAL(AttributeNameMap, supportedAttributes, ());
    if (supportedAttributes.isEmpty()) {
        // Fill the set for the first use.
        // Animatable attributes from http://www.w3.org/TR/SVG/attindex.html
        const QualifiedName* attributes[] = {
            &HTMLNames::classAttr,
            &SVGNames::amplitudeAttr,
            &SVGNames::azimuthAttr,
            &SVGNames::baseFrequencyAttr,
            &SVGNames::biasAttr,
            &SVGNames::clipPathUnitsAttr,
            &SVGNames::cxAttr,
            &SVGNames::cyAttr,
            &SVGNames::dAttr,
            &SVGNames::diffuseConstantAttr,
            &SVGNames::divisorAttr,
            &SVGNames::dxAttr,
            &SVGNames::dyAttr,
            &SVGNames::edgeModeAttr,
            &SVGNames::elevationAttr,
            &SVGNames::exponentAttr,
            &SVGNames::filterUnitsAttr,
            &SVGNames::fxAttr,
            &SVGNames::fyAttr,
            &SVGNames::gradientTransformAttr,
            &SVGNames::gradientUnitsAttr,
            &SVGNames::heightAttr,
            &SVGNames::in2Attr,
            &SVGNames::inAttr,
            &SVGNames::interceptAttr,
            &SVGNames::k1Attr,
            &SVGNames::k2Attr,
            &SVGNames::k3Attr,
            &SVGNames::k4Attr,
            &SVGNames::kernelMatrixAttr,
            &SVGNames::kernelUnitLengthAttr,
            &SVGNames::lengthAdjustAttr,
            &SVGNames::limitingConeAngleAttr,
            &SVGNames::markerHeightAttr,
            &SVGNames::markerUnitsAttr,
            &SVGNames::markerWidthAttr,
            &SVGNames::maskContentUnitsAttr,
            &SVGNames::maskUnitsAttr,
            &SVGNames::methodAttr,
            &SVGNames::modeAttr,
            &SVGNames::numOctavesAttr,
            &SVGNames::offsetAttr,
            &SVGNames::operatorAttr,
            &SVGNames::orderAttr,
            &SVGNames::orientAttr,
            &SVGNames::pathLengthAttr,
            &SVGNames::patternContentUnitsAttr,
            &SVGNames::patternTransformAttr,
            &SVGNames::patternUnitsAttr,
            &SVGNames::pointsAtXAttr,
            &SVGNames::pointsAtYAttr,
            &SVGNames::pointsAtZAttr,
            &SVGNames::pointsAttr,
            &SVGNames::preserveAlphaAttr,
            &SVGNames::preserveAspectRatioAttr,
            &SVGNames::primitiveUnitsAttr,
            &SVGNames::rAttr,
            &SVGNames::radiusAttr,
            &SVGNames::refXAttr,
            &SVGNames::refYAttr,
            &SVGNames::resultAttr,
            &SVGNames::rotateAttr,
            &SVGNames::rxAttr,
            &SVGNames::ryAttr,
            &SVGNames::scaleAttr,
            &SVGNames::seedAttr,
            &SVGNames::slopeAttr,
            &SVGNames::spacingAttr,
            &SVGNames::specularConstantAttr,
            &SVGNames::specularExponentAttr,
            &SVGNames::spreadMethodAttr,
            &SVGNames::startOffsetAttr,
            &SVGNames::stdDeviationAttr,
            &SVGNames::stitchTilesAttr,
            &SVGNames::surfaceScaleAttr,
            &SVGNames::tableValuesAttr,
            &SVGNames::targetAttr,
            &SVGNames::targetXAttr,
            &SVGNames::targetYAttr,
            &SVGNames::textLengthAttr,
            &SVGNames::transformAttr,
            &SVGNames::typeAttr,
            &SVGNames::valuesAttr,
            &SVGNames::viewBoxAttr,
            &SVGNames::widthAttr,
            &SVGNames::x1Attr,
            &SVGNames::x2Attr,
            &SVGNames::xAttr,
            &SVGNames::xChannelSelectorAttr,
            &SVGNames::y1Attr,
            &SVGNames::y2Attr,
            &SVGNames::yAttr,
            &SVGNames::yChannelSelectorAttr,
            &SVGNames::zAttr,
            &XLinkNames::hrefAttr,
        };
        for (size_t i = 0; i < WTF_ARRAY_LENGTH(attributes); i++)
            supportedAttributes.set(*attributes[i], attributes[i]);
    }

    if (isSVGSMILElement(*svgElement))
        return nullptr;

    QualifiedName attributeName = svgAttributeName(property);

    auto iter = supportedAttributes.find(attributeName);
    if (iter == supportedAttributes.end() || !svgElement->propertyFromAttribute(*iter->value))
        return nullptr;

    return iter->value;
}

} // namespace

PassRefPtrWillBeRawPtr<EffectModel> EffectInput::convert(Element* element, const Vector<Dictionary>& keyframeDictionaryVector, ExceptionState& exceptionState)
{
    if (!element)
        return nullptr;

    // TODO(alancutter): Remove this once composited animations no longer depend on AnimatableValues.
    if (element->inActiveDocument())
        element->document().updateLayoutTreeForNodeIfNeeded(element);

    StyleSheetContents* styleSheetContents = element->document().elementSheet().contents();
    StringKeyframeVector keyframes;
    double lastOffset = 0;

    for (const auto& keyframeDictionary : keyframeDictionaryVector) {
        RefPtrWillBeRawPtr<StringKeyframe> keyframe = StringKeyframe::create();

        ScriptValue scriptValue;
        bool frameHasOffset = DictionaryHelper::get(keyframeDictionary, "offset", scriptValue) && !scriptValue.isNull();

        if (frameHasOffset) {
            double offset;
            DictionaryHelper::get(keyframeDictionary, "offset", offset);

            // Keyframes with offsets outside the range [0.0, 1.0] are an error.
            if (std::isnan(offset)) {
                exceptionState.throwDOMException(InvalidModificationError, "Non numeric offset provided");
            }

            if (offset < 0 || offset > 1) {
                exceptionState.throwDOMException(InvalidModificationError, "Offsets provided outside the range [0, 1]");
                return nullptr;
            }

            if (offset < lastOffset) {
                exceptionState.throwDOMException(InvalidModificationError, "Keyframes with specified offsets are not sorted");
                return nullptr;
            }

            lastOffset = offset;

            keyframe->setOffset(offset);
        }
        keyframes.append(keyframe);

        String compositeString;
        DictionaryHelper::get(keyframeDictionary, "composite", compositeString);
        if (compositeString == "add")
            keyframe->setComposite(EffectModel::CompositeAdd);

        String timingFunctionString;
        if (DictionaryHelper::get(keyframeDictionary, "easing", timingFunctionString)) {
            if (RefPtr<TimingFunction> timingFunction = AnimationInputHelpers::parseTimingFunction(timingFunctionString))
                keyframe->setEasing(timingFunction);
        }

        Vector<String> keyframeProperties;
        keyframeDictionary.getPropertyNames(keyframeProperties);
        for (const auto& property : keyframeProperties) {
            String value;
            DictionaryHelper::get(keyframeDictionary, property, value);
            CSSPropertyID id = AnimationInputHelpers::keyframeAttributeToCSSPropertyID(property);
            if (id != CSSPropertyInvalid) {
                keyframe->setPropertyValue(id, value, element, styleSheetContents);
                continue;
            }

            if (property == "offset"
                || property == "composite"
                || property == "easing") {
                continue;
            }

            if (!RuntimeEnabledFeatures::webAnimationsSVGEnabled() || !element->isSVGElement() || !svgPrefixed(property))
                continue;

            SVGElement* svgElement = toSVGElement(element);
            const QualifiedName* qualifiedName = supportedSVGAttribute(property, svgElement);

            if (qualifiedName)
                keyframe->setPropertyValue(*qualifiedName, value, svgElement);
        }
    }

    RefPtrWillBeRawPtr<StringKeyframeEffectModel> keyframeEffectModel = StringKeyframeEffectModel::create(keyframes);
    if (keyframeEffectModel->hasSyntheticKeyframes()) {
        exceptionState.throwDOMException(NotSupportedError, "Partial keyframes are not supported.");
        return nullptr;
    }
    if (!keyframeEffectModel->isReplaceOnly()) {
        exceptionState.throwDOMException(NotSupportedError, "Additive animations are not supported.");
        return nullptr;
    }
    keyframeEffectModel->forceConversionsToAnimatableValues(*element, element->computedStyle());

    return keyframeEffectModel;
}

PassRefPtrWillBeRawPtr<EffectModel> EffectInput::convert(Element* element, const EffectModelOrDictionarySequence& effectInput, ExceptionState& exceptionState)
{
    if (effectInput.isEffectModel())
        return effectInput.getAsEffectModel();
    if (effectInput.isDictionarySequence())
        return convert(element, effectInput.getAsDictionarySequence(), exceptionState);
    return nullptr;
}

} // namespace blink

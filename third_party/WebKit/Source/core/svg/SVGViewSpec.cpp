/*
 * Copyright (C) 2007, 2010 Rob Buis <buis@kde.org>
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

#include "config.h"
#include "core/svg/SVGViewSpec.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/SVGNames.h"
#include "core/dom/ExceptionCode.h"
#include "core/svg/SVGAnimatedTransformList.h"
#include "core/svg/SVGParserUtilities.h"

namespace blink {

SVGViewSpec::SVGViewSpec(SVGSVGElement* contextElement)
    // Note: addToPropertyMap is not needed, as SVGViewSpec do not correspond to an element.
    // Note: We make tear-offs' contextElement the target element of SVGViewSpec.
    // This contextElement will be only used for keeping this alive from the tearoff.
    // SVGSVGElement holds a strong-ref to this SVGViewSpec, so this is kept alive as:
    // AnimatedProperty tearoff -(contextElement)-> SVGSVGElement -(RefPtr)-> SVGViewSpec.
    : SVGFitToViewBox(contextElement, PropertyMapPolicySkip)
    , m_contextElement(contextElement)
    , m_transform(SVGAnimatedTransformList::create(contextElement, SVGNames::transformAttr, SVGTransformList::create()))
{
    ASSERT(m_contextElement);

    viewBox()->setReadOnly();
    preserveAspectRatio()->setReadOnly();
    m_transform->setReadOnly();
    // Note: addToPropertyMap is not needed, as SVGViewSpec do not correspond to an element.
}

DEFINE_TRACE(SVGViewSpec)
{
    visitor->trace(m_contextElement);
    visitor->trace(m_transform);
    SVGFitToViewBox::trace(visitor);
}

bool SVGViewSpec::parseViewSpec(const String& spec)
{
    if (spec.isEmpty() || !m_contextElement)
        return false;
    if (spec.is8Bit()) {
        const LChar* ptr = spec.characters8();
        const LChar* end = ptr + spec.length();
        return parseViewSpecInternal(ptr, end);
    }
    const UChar* ptr = spec.characters16();
    const UChar* end = ptr + spec.length();
    return parseViewSpecInternal(ptr, end);
}

void SVGViewSpec::reset()
{
    resetZoomAndPan();
    m_transform->baseValue()->clear();
    updateViewBox(FloatRect());
    ASSERT(preserveAspectRatio());
    preserveAspectRatio()->baseValue()->setAlign(SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMID);
    preserveAspectRatio()->baseValue()->setMeetOrSlice(SVGPreserveAspectRatio::SVG_MEETORSLICE_MEET);
    m_viewTargetString = emptyString();
}

void SVGViewSpec::detachContextElement()
{
    m_transform = nullptr;
    clearViewBox();
    clearPreserveAspectRatio();
    m_contextElement = nullptr;
}

SVGElement* SVGViewSpec::viewTarget() const
{
    if (!m_contextElement)
        return nullptr;
    Element* element = m_contextElement->treeScope().getElementById(AtomicString(m_viewTargetString));
    if (!element || !element->isSVGElement())
        return nullptr;
    return toSVGElement(element);
}

String SVGViewSpec::viewBoxString() const
{
    if (!viewBox())
        return String();

    return viewBox()->currentValue()->valueAsString();
}

String SVGViewSpec::preserveAspectRatioString() const
{
    if (!preserveAspectRatio())
        return String();

    return preserveAspectRatio()->baseValue()->valueAsString();
}

String SVGViewSpec::transformString() const
{
    if (!m_transform)
        return String();

    return m_transform->baseValue()->valueAsString();
}

void SVGViewSpec::setZoomAndPan(unsigned short, ExceptionState& exceptionState)
{
    // SVGViewSpec and all of its content is read-only.
    exceptionState.throwDOMException(NoModificationAllowedError, ExceptionMessages::readOnly());
}

static const LChar svgViewSpec[] = {'s', 'v', 'g', 'V', 'i', 'e', 'w'};
static const LChar viewBoxSpec[] = {'v', 'i', 'e', 'w', 'B', 'o', 'x'};
static const LChar preserveAspectRatioSpec[] = {'p', 'r', 'e', 's', 'e', 'r', 'v', 'e', 'A', 's', 'p', 'e', 'c', 't', 'R', 'a', 't', 'i', 'o'};
static const LChar transformSpec[] = {'t', 'r', 'a', 'n', 's', 'f', 'o', 'r', 'm'};
static const LChar zoomAndPanSpec[] = {'z', 'o', 'o', 'm', 'A', 'n', 'd', 'P', 'a', 'n'};
static const LChar viewTargetSpec[] =  {'v', 'i', 'e', 'w', 'T', 'a', 'r', 'g', 'e', 't'};

template<typename CharType>
bool SVGViewSpec::parseViewSpecInternal(const CharType* ptr, const CharType* end)
{
    if (!skipString(ptr, end, svgViewSpec, WTF_ARRAY_LENGTH(svgViewSpec)))
        return false;

    if (ptr >= end || *ptr != '(')
        return false;
    ptr++;

    while (ptr < end && *ptr != ')') {
        if (*ptr == 'v') {
            if (skipString(ptr, end, viewBoxSpec, WTF_ARRAY_LENGTH(viewBoxSpec))) {
                if (ptr >= end || *ptr != '(')
                    return false;
                ptr++;
                float x = 0.0f;
                float y = 0.0f;
                float width = 0.0f;
                float height = 0.0f;
                if (!(parseNumber(ptr, end, x) && parseNumber(ptr, end, y) && parseNumber(ptr, end, width) && parseNumber(ptr, end, height, DisallowWhitespace)))
                    return false;
                updateViewBox(FloatRect(x, y, width, height));
                if (ptr >= end || *ptr != ')')
                    return false;
                ptr++;
            } else if (skipString(ptr, end, viewTargetSpec, WTF_ARRAY_LENGTH(viewTargetSpec))) {
                if (ptr >= end || *ptr != '(')
                    return false;
                const CharType* viewTargetStart = ++ptr;
                while (ptr < end && *ptr != ')')
                    ptr++;
                if (ptr >= end)
                    return false;
                m_viewTargetString = String(viewTargetStart, ptr - viewTargetStart);
                ptr++;
            } else
                return false;
        } else if (*ptr == 'z') {
            if (!skipString(ptr, end, zoomAndPanSpec, WTF_ARRAY_LENGTH(zoomAndPanSpec)))
                return false;
            if (ptr >= end || *ptr != '(')
                return false;
            ptr++;
            if (!parseZoomAndPan(ptr, end))
                return false;
            if (ptr >= end || *ptr != ')')
                return false;
            ptr++;
        } else if (*ptr == 'p') {
            if (!skipString(ptr, end, preserveAspectRatioSpec, WTF_ARRAY_LENGTH(preserveAspectRatioSpec)))
                return false;
            if (ptr >= end || *ptr != '(')
                return false;
            ptr++;
            if (!preserveAspectRatio()->baseValue()->parse(ptr, end, false))
                return false;
            if (ptr >= end || *ptr != ')')
                return false;
            ptr++;
        } else if (*ptr == 't') {
            if (!skipString(ptr, end, transformSpec, WTF_ARRAY_LENGTH(transformSpec)))
                return false;
            if (ptr >= end || *ptr != '(')
                return false;
            ptr++;
            m_transform->baseValue()->parse(ptr, end);
            if (ptr >= end || *ptr != ')')
                return false;
            ptr++;
        } else
            return false;

        if (ptr < end && *ptr == ';')
            ptr++;
    }

    if (ptr >= end || *ptr != ')')
        return false;

    return true;
}

}

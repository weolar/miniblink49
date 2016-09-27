/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
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

#include "core/svg/SVGPathSegList.h"

#include "core/SVGNames.h"
#include "core/svg/SVGAnimationElement.h"
#include "core/svg/SVGPathBlender.h"
#include "core/svg/SVGPathByteStreamBuilder.h"
#include "core/svg/SVGPathByteStreamSource.h"
#include "core/svg/SVGPathElement.h"
#include "core/svg/SVGPathParser.h"
#include "core/svg/SVGPathSegListBuilder.h"
#include "core/svg/SVGPathSegListSource.h"
#include "core/svg/SVGPathUtilities.h"

namespace blink {

SVGPathSegList::SVGPathSegList(SVGPathElement* contextElement)
    : m_contextElement(contextElement)
    , m_listSyncedToByteStream(true)
{
    ASSERT(contextElement);
}

SVGPathSegList::SVGPathSegList(SVGPathElement* contextElement, PassOwnPtr<SVGPathByteStream> byteStream)
    : m_contextElement(contextElement)
    , m_byteStream(byteStream)
    , m_listSyncedToByteStream(true)
{
    ASSERT(contextElement);
}

SVGPathSegList::~SVGPathSegList()
{
}

DEFINE_TRACE(SVGPathSegList)
{
    visitor->trace(m_contextElement);
    SVGListPropertyHelper<SVGPathSegList, SVGPathSeg>::trace(visitor);
}

PassRefPtrWillBeRawPtr<SVGPathSegList> SVGPathSegList::clone()
{
    RefPtrWillBeRawPtr<SVGPathSegList> svgPathSegList = adoptRefWillBeNoop(new SVGPathSegList(m_contextElement, byteStream()->copy()));
    svgPathSegList->invalidateList();
    return svgPathSegList.release();
}

PassRefPtrWillBeRawPtr<SVGPropertyBase> SVGPathSegList::cloneForAnimation(const String& value) const
{
    RefPtrWillBeRawPtr<SVGPathSegList> svgPathSegList = SVGPathSegList::create(m_contextElement);
    svgPathSegList->setValueAsString(value, IGNORE_EXCEPTION);
    return svgPathSegList;
}

const SVGPathByteStream* SVGPathSegList::byteStream() const
{
    if (!m_byteStream) {
        m_byteStream = SVGPathByteStream::create();

        if (!Base::isEmpty()) {
            SVGPathByteStreamBuilder builder(*m_byteStream);
            SVGPathSegListSource source(begin(), end());
            SVGPathParser parser(&source, &builder);
            parser.parsePathDataFromSource(UnalteredParsing);
        }
    }

    return m_byteStream.get();
}

void SVGPathSegList::updateListFromByteStream()
{
    if (m_listSyncedToByteStream)
        return;

    Base::clear();

    if (m_byteStream && !m_byteStream->isEmpty()) {
        SVGPathSegListBuilder builder(m_contextElement, this);
        SVGPathByteStreamSource source(*m_byteStream);
        SVGPathParser parser(&source, &builder);
        parser.parsePathDataFromSource(UnalteredParsing);
    }

    m_listSyncedToByteStream = true;
}

void SVGPathSegList::invalidateList()
{
    m_listSyncedToByteStream = false;
    Base::clear();
}

PassRefPtrWillBeRawPtr<SVGPathSeg> SVGPathSegList::appendItem(PassRefPtrWillBeRawPtr<SVGPathSeg> passItem)
{
    updateListFromByteStream();
    RefPtrWillBeRawPtr<SVGPathSeg> item = Base::appendItem(passItem);

    if (m_byteStream) {
        SVGPathByteStreamBuilder builder(*m_byteStream);
        SVGPathSegListSource source(lastAppended(), end());
        SVGPathParser parser(&source, &builder);
        parser.parsePathDataFromSource(UnalteredParsing, false);
    }

    return item.release();
}

String SVGPathSegList::valueAsString() const
{
    String string;
    buildStringFromByteStream(*byteStream(), string, UnalteredParsing);
    return string;
}

void SVGPathSegList::setValueAsString(const String& string, ExceptionState& exceptionState)
{
    invalidateList();
    if (!m_byteStream)
        m_byteStream = SVGPathByteStream::create();
    if (!buildSVGPathByteStreamFromString(string, *m_byteStream, UnalteredParsing))
        exceptionState.throwDOMException(SyntaxError, "Problem parsing path \"" + string + "\"");
}

void SVGPathSegList::add(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement*)
{
    RefPtrWillBeRawPtr<SVGPathSegList> otherList = toSVGPathSegList(other);
    if (length() != otherList->length())
        return;

    byteStream(); // create |m_byteStream| if it does not exist.
    addToSVGPathByteStream(*m_byteStream, *otherList->byteStream());
    invalidateList();
}

void SVGPathSegList::calculateAnimatedValue(SVGAnimationElement* animationElement, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> fromValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement*)
{
    invalidateList();

    ASSERT(animationElement);
    bool isToAnimation = animationElement->animationMode() == ToAnimation;

    const RefPtrWillBeRawPtr<SVGPathSegList> from = toSVGPathSegList(fromValue);
    const RefPtrWillBeRawPtr<SVGPathSegList> to = toSVGPathSegList(toValue);
    const RefPtrWillBeRawPtr<SVGPathSegList> toAtEndOfDuration = toSVGPathSegList(toAtEndOfDurationValue);

    const SVGPathByteStream* toStream = to->byteStream();
    const SVGPathByteStream* fromStream = from->byteStream();
    OwnPtr<SVGPathByteStream> copy;

    // If no 'to' value is given, nothing to animate.
    if (!toStream->size())
        return;

    if (isToAnimation) {
        copy = byteStream()->copy();
        fromStream = copy.get();
    }

    // If the 'from' value is given and it's length doesn't match the 'to' value list length, fallback to a discrete animation.
    if (fromStream->size() != toStream->size() && fromStream->size()) {
        if (percentage < 0.5) {
            if (!isToAnimation) {
                m_byteStream = fromStream->copy();
                return;
            }
        } else {
            m_byteStream = toStream->copy();
            return;
        }
    }

    OwnPtr<SVGPathByteStream> lastAnimatedStream = m_byteStream.release();

    m_byteStream = SVGPathByteStream::create();
    SVGPathByteStreamBuilder builder(*m_byteStream);

    SVGPathByteStreamSource fromSource(*fromStream);
    SVGPathByteStreamSource toSource(*toStream);

    SVGPathBlender blender(&fromSource, &toSource, &builder);
    blender.blendAnimatedPath(percentage);

    // Handle additive='sum'.
    if (!fromStream->size() || (animationElement->isAdditive() && !isToAnimation))
        addToSVGPathByteStream(*m_byteStream, *lastAnimatedStream);

    // Handle accumulate='sum'.
    if (animationElement->isAccumulated() && repeatCount) {
        const SVGPathByteStream* toAtEndOfDurationStream = toAtEndOfDuration->byteStream();
        addToSVGPathByteStream(*m_byteStream, *toAtEndOfDurationStream, repeatCount);
    }
}

float SVGPathSegList::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement*)
{
    // FIXME: Support paced animations.
    return -1;
}

}

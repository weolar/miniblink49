/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2010 Dirk Schulze <krit@webkit.org>
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
#include "core/svg/SVGPreserveAspectRatio.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/dom/ExceptionCode.h"
#include "core/svg/SVGAnimationElement.h"
#include "core/svg/SVGParserUtilities.h"
#include "platform/geometry/FloatRect.h"
#include "platform/transforms/AffineTransform.h"
#include "wtf/text/WTFString.h"

namespace blink {

SVGPreserveAspectRatio::SVGPreserveAspectRatio()
{
    setDefault();
}

void SVGPreserveAspectRatio::setDefault()
{
    m_align = SVG_PRESERVEASPECTRATIO_XMIDYMID;
    m_meetOrSlice = SVG_MEETORSLICE_MEET;
}

PassRefPtrWillBeRawPtr<SVGPreserveAspectRatio> SVGPreserveAspectRatio::clone() const
{
    RefPtrWillBeRawPtr<SVGPreserveAspectRatio> preserveAspectRatio = create();

    preserveAspectRatio->m_align = m_align;
    preserveAspectRatio->m_meetOrSlice = m_meetOrSlice;

    return preserveAspectRatio.release();
}

template<typename CharType>
bool SVGPreserveAspectRatio::parseInternal(const CharType*& ptr, const CharType* end, bool validate)
{
    SVGPreserveAspectRatioType align = SVG_PRESERVEASPECTRATIO_XMIDYMID;
    SVGMeetOrSliceType meetOrSlice = SVG_MEETORSLICE_MEET;

    setAlign(align);
    setMeetOrSlice(meetOrSlice);

    if (!skipOptionalSVGSpaces(ptr, end))
        return false;

    if (*ptr == 'n') {
        if (!skipString(ptr, end, "none"))
            return false;
        align = SVG_PRESERVEASPECTRATIO_NONE;
        skipOptionalSVGSpaces(ptr, end);
    } else if (*ptr == 'x') {
        if ((end - ptr) < 8)
            return false;
        if (ptr[1] != 'M' || ptr[4] != 'Y' || ptr[5] != 'M')
            return false;
        if (ptr[2] == 'i') {
            if (ptr[3] == 'n') {
                if (ptr[6] == 'i') {
                    if (ptr[7] == 'n')
                        align = SVG_PRESERVEASPECTRATIO_XMINYMIN;
                    else if (ptr[7] == 'd')
                        align = SVG_PRESERVEASPECTRATIO_XMINYMID;
                    else
                        return false;
                } else if (ptr[6] == 'a' && ptr[7] == 'x') {
                    align = SVG_PRESERVEASPECTRATIO_XMINYMAX;
                } else {
                    return false;
                }
            } else if (ptr[3] == 'd') {
                if (ptr[6] == 'i') {
                    if (ptr[7] == 'n')
                        align = SVG_PRESERVEASPECTRATIO_XMIDYMIN;
                    else if (ptr[7] == 'd')
                        align = SVG_PRESERVEASPECTRATIO_XMIDYMID;
                    else
                        return false;
                } else if (ptr[6] == 'a' && ptr[7] == 'x') {
                    align = SVG_PRESERVEASPECTRATIO_XMIDYMAX;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else if (ptr[2] == 'a' && ptr[3] == 'x') {
            if (ptr[6] == 'i') {
                if (ptr[7] == 'n')
                    align = SVG_PRESERVEASPECTRATIO_XMAXYMIN;
                else if (ptr[7] == 'd')
                    align = SVG_PRESERVEASPECTRATIO_XMAXYMID;
                else
                    return false;
            } else if (ptr[6] == 'a' && ptr[7] == 'x') {
                align = SVG_PRESERVEASPECTRATIO_XMAXYMAX;
            } else {
                return false;
            }
        } else {
            return false;
        }
        ptr += 8;
        skipOptionalSVGSpaces(ptr, end);
    } else {
        return false;
    }

    if (ptr < end) {
        if (*ptr == 'm') {
            if (!skipString(ptr, end, "meet"))
                return false;
            skipOptionalSVGSpaces(ptr, end);
        } else if (*ptr == 's') {
            if (!skipString(ptr, end, "slice"))
                return false;
            skipOptionalSVGSpaces(ptr, end);
            if (align != SVG_PRESERVEASPECTRATIO_NONE)
                meetOrSlice = SVG_MEETORSLICE_SLICE;
        }
    }

    if (end != ptr && validate)
        return false;

    setAlign(align);
    setMeetOrSlice(meetOrSlice);

    return true;
}

void SVGPreserveAspectRatio::setValueAsString(const String& string, ExceptionState& exceptionState)
{
    setDefault();

    if (string.isEmpty())
        return;

    bool valid = false;
    if (string.is8Bit()) {
        const LChar* ptr = string.characters8();
        const LChar* end = ptr + string.length();
        valid = parseInternal(ptr, end, true);
    } else {
        const UChar* ptr = string.characters16();
        const UChar* end = ptr + string.length();
        valid = parseInternal(ptr, end, true);
    }

    if (!valid) {
        exceptionState.throwDOMException(SyntaxError, "The value provided ('" + string + "') is invalid.");
    }
}

bool SVGPreserveAspectRatio::parse(const LChar*& ptr, const LChar* end, bool validate)
{
    return parseInternal(ptr, end, validate);
}

bool SVGPreserveAspectRatio::parse(const UChar*& ptr, const UChar* end, bool validate)
{
    return parseInternal(ptr, end, validate);
}

void SVGPreserveAspectRatio::transformRect(FloatRect& destRect, FloatRect& srcRect)
{
    if (m_align == SVG_PRESERVEASPECTRATIO_NONE)
        return;

    FloatSize imageSize = srcRect.size();
    float origDestWidth = destRect.width();
    float origDestHeight = destRect.height();
    switch (m_meetOrSlice) {
    case SVGPreserveAspectRatio::SVG_MEETORSLICE_UNKNOWN:
        break;
    case SVGPreserveAspectRatio::SVG_MEETORSLICE_MEET: {
        float widthToHeightMultiplier = srcRect.height() / srcRect.width();
        if (origDestHeight > origDestWidth * widthToHeightMultiplier) {
            destRect.setHeight(origDestWidth * widthToHeightMultiplier);
            switch (m_align) {
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMINYMID:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMID:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMID:
                destRect.setY(destRect.y() + origDestHeight / 2 - destRect.height() / 2);
                break;
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMINYMAX:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMAX:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMAX:
                destRect.setY(destRect.y() + origDestHeight - destRect.height());
                break;
            default:
                break;
            }
        }
        if (origDestWidth > origDestHeight / widthToHeightMultiplier) {
            destRect.setWidth(origDestHeight / widthToHeightMultiplier);
            switch (m_align) {
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMIN:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMID:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMAX:
                destRect.setX(destRect.x() + origDestWidth / 2 - destRect.width() / 2);
                break;
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMIN:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMID:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMAX:
                destRect.setX(destRect.x() + origDestWidth - destRect.width());
                break;
            default:
                break;
            }
        }
        break;
    }
    case SVGPreserveAspectRatio::SVG_MEETORSLICE_SLICE: {
        float widthToHeightMultiplier = srcRect.height() / srcRect.width();
        // if the destination height is less than the height of the image we'll be drawing
        if (origDestHeight < origDestWidth * widthToHeightMultiplier) {
            float destToSrcMultiplier = srcRect.width() / destRect.width();
            srcRect.setHeight(destRect.height() * destToSrcMultiplier);
            switch (m_align) {
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMINYMID:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMID:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMID:
                srcRect.setY(srcRect.y() + imageSize.height() / 2 - srcRect.height() / 2);
                break;
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMINYMAX:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMAX:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMAX:
                srcRect.setY(srcRect.y() + imageSize.height() - srcRect.height());
                break;
            default:
                break;
            }
        }
        // if the destination width is less than the width of the image we'll be drawing
        if (origDestWidth < origDestHeight / widthToHeightMultiplier) {
            float destToSrcMultiplier = srcRect.height() / destRect.height();
            srcRect.setWidth(destRect.width() * destToSrcMultiplier);
            switch (m_align) {
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMIN:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMID:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMIDYMAX:
                srcRect.setX(srcRect.x() + imageSize.width() / 2 - srcRect.width() / 2);
                break;
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMIN:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMID:
            case SVGPreserveAspectRatio::SVG_PRESERVEASPECTRATIO_XMAXYMAX:
                srcRect.setX(srcRect.x() + imageSize.width() - srcRect.width());
                break;
            default:
                break;
            }
        }
        break;
    }
    }
}

AffineTransform SVGPreserveAspectRatio::getCTM(float logicalX, float logicalY, float logicalWidth, float logicalHeight, float physicalWidth, float physicalHeight) const
{
    ASSERT(logicalWidth);
    ASSERT(logicalHeight);
    ASSERT(physicalWidth);
    ASSERT(physicalHeight);

    AffineTransform transform;
    if (m_align == SVG_PRESERVEASPECTRATIO_UNKNOWN)
        return transform;

    double extendedLogicalX = logicalX;
    double extendedLogicalY = logicalY;
    double extendedLogicalWidth = logicalWidth;
    double extendedLogicalHeight = logicalHeight;
    double extendedPhysicalWidth = physicalWidth;
    double extendedPhysicalHeight = physicalHeight;
    double logicalRatio = extendedLogicalWidth / extendedLogicalHeight;
    double physicalRatio = extendedPhysicalWidth / extendedPhysicalHeight;

    if (m_align == SVG_PRESERVEASPECTRATIO_NONE) {
        transform.scaleNonUniform(extendedPhysicalWidth / extendedLogicalWidth, extendedPhysicalHeight / extendedLogicalHeight);
        transform.translate(-extendedLogicalX, -extendedLogicalY);
        return transform;
    }

    if ((logicalRatio < physicalRatio && (m_meetOrSlice == SVG_MEETORSLICE_MEET)) || (logicalRatio >= physicalRatio && (m_meetOrSlice == SVG_MEETORSLICE_SLICE))) {
        transform.scaleNonUniform(extendedPhysicalHeight / extendedLogicalHeight, extendedPhysicalHeight / extendedLogicalHeight);

        if (m_align == SVG_PRESERVEASPECTRATIO_XMINYMIN || m_align == SVG_PRESERVEASPECTRATIO_XMINYMID || m_align == SVG_PRESERVEASPECTRATIO_XMINYMAX)
            transform.translate(-extendedLogicalX, -extendedLogicalY);
        else if (m_align == SVG_PRESERVEASPECTRATIO_XMIDYMIN || m_align == SVG_PRESERVEASPECTRATIO_XMIDYMID || m_align == SVG_PRESERVEASPECTRATIO_XMIDYMAX)
            transform.translate(-extendedLogicalX - (extendedLogicalWidth - extendedPhysicalWidth * extendedLogicalHeight / extendedPhysicalHeight) / 2, -extendedLogicalY);
        else
            transform.translate(-extendedLogicalX - (extendedLogicalWidth - extendedPhysicalWidth * extendedLogicalHeight / extendedPhysicalHeight), -extendedLogicalY);

        return transform;
    }

    transform.scaleNonUniform(extendedPhysicalWidth / extendedLogicalWidth, extendedPhysicalWidth / extendedLogicalWidth);

    if (m_align == SVG_PRESERVEASPECTRATIO_XMINYMIN || m_align == SVG_PRESERVEASPECTRATIO_XMIDYMIN || m_align == SVG_PRESERVEASPECTRATIO_XMAXYMIN)
        transform.translate(-extendedLogicalX, -extendedLogicalY);
    else if (m_align == SVG_PRESERVEASPECTRATIO_XMINYMID || m_align == SVG_PRESERVEASPECTRATIO_XMIDYMID || m_align == SVG_PRESERVEASPECTRATIO_XMAXYMID)
        transform.translate(-extendedLogicalX, -extendedLogicalY - (extendedLogicalHeight - extendedPhysicalHeight * extendedLogicalWidth / extendedPhysicalWidth) / 2);
    else
        transform.translate(-extendedLogicalX, -extendedLogicalY - (extendedLogicalHeight - extendedPhysicalHeight * extendedLogicalWidth / extendedPhysicalWidth));

    return transform;
}

String SVGPreserveAspectRatio::valueAsString() const
{
    String alignType;

    switch (m_align) {
    case SVG_PRESERVEASPECTRATIO_NONE:
        alignType = "none";
        break;
    case SVG_PRESERVEASPECTRATIO_XMINYMIN:
        alignType = "xMinYMin";
        break;
    case SVG_PRESERVEASPECTRATIO_XMIDYMIN:
        alignType = "xMidYMin";
        break;
    case SVG_PRESERVEASPECTRATIO_XMAXYMIN:
        alignType = "xMaxYMin";
        break;
    case SVG_PRESERVEASPECTRATIO_XMINYMID:
        alignType = "xMinYMid";
        break;
    case SVG_PRESERVEASPECTRATIO_XMIDYMID:
        alignType = "xMidYMid";
        break;
    case SVG_PRESERVEASPECTRATIO_XMAXYMID:
        alignType = "xMaxYMid";
        break;
    case SVG_PRESERVEASPECTRATIO_XMINYMAX:
        alignType = "xMinYMax";
        break;
    case SVG_PRESERVEASPECTRATIO_XMIDYMAX:
        alignType = "xMidYMax";
        break;
    case SVG_PRESERVEASPECTRATIO_XMAXYMAX:
        alignType = "xMaxYMax";
        break;
    case SVG_PRESERVEASPECTRATIO_UNKNOWN:
        alignType = "unknown";
        break;
    };

    switch (m_meetOrSlice) {
    default:
    case SVG_MEETORSLICE_UNKNOWN:
        return alignType;
    case SVG_MEETORSLICE_MEET:
        return alignType + " meet";
    case SVG_MEETORSLICE_SLICE:
        return alignType + " slice";
    }
}

void SVGPreserveAspectRatio::add(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement*)
{
    ASSERT_NOT_REACHED();
}

void SVGPreserveAspectRatio::calculateAnimatedValue(SVGAnimationElement* animationElement, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> fromValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*)
{
    ASSERT(animationElement);

    bool useToValue;
    animationElement->animateDiscreteType(percentage, false, true, useToValue);

    RefPtrWillBeRawPtr<SVGPreserveAspectRatio> preserveAspectRatioToUse = useToValue ? toSVGPreserveAspectRatio(toValue) : toSVGPreserveAspectRatio(fromValue);

    m_align = preserveAspectRatioToUse->m_align;
    m_meetOrSlice = preserveAspectRatioToUse->m_meetOrSlice;
}

float SVGPreserveAspectRatio::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, SVGElement* contextElement)
{
    // No paced animations for SVGPreserveAspectRatio.
    return -1;
}

}

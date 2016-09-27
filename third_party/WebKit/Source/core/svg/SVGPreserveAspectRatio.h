/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
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

#ifndef SVGPreserveAspectRatio_h
#define SVGPreserveAspectRatio_h

#include "core/svg/properties/SVGPropertyHelper.h"

namespace blink {

class AffineTransform;
class FloatRect;
class SVGPreserveAspectRatioTearOff;

class SVGPreserveAspectRatio : public SVGPropertyHelper<SVGPreserveAspectRatio> {
public:
    enum SVGPreserveAspectRatioType {
        SVG_PRESERVEASPECTRATIO_UNKNOWN = 0,
        SVG_PRESERVEASPECTRATIO_NONE = 1,
        SVG_PRESERVEASPECTRATIO_XMINYMIN = 2,
        SVG_PRESERVEASPECTRATIO_XMIDYMIN = 3,
        SVG_PRESERVEASPECTRATIO_XMAXYMIN = 4,
        SVG_PRESERVEASPECTRATIO_XMINYMID = 5,
        SVG_PRESERVEASPECTRATIO_XMIDYMID = 6,
        SVG_PRESERVEASPECTRATIO_XMAXYMID = 7,
        SVG_PRESERVEASPECTRATIO_XMINYMAX = 8,
        SVG_PRESERVEASPECTRATIO_XMIDYMAX = 9,
        SVG_PRESERVEASPECTRATIO_XMAXYMAX = 10
    };

    enum SVGMeetOrSliceType {
        SVG_MEETORSLICE_UNKNOWN = 0,
        SVG_MEETORSLICE_MEET = 1,
        SVG_MEETORSLICE_SLICE = 2
    };

    typedef SVGPreserveAspectRatioTearOff TearOffType;

    static PassRefPtrWillBeRawPtr<SVGPreserveAspectRatio> create()
    {
        return adoptRefWillBeNoop(new SVGPreserveAspectRatio());
    }

    virtual PassRefPtrWillBeRawPtr<SVGPreserveAspectRatio> clone() const;

    bool operator==(const SVGPreserveAspectRatio&) const;
    bool operator!=(const SVGPreserveAspectRatio& other) const { return !operator==(other); }

    void setAlign(SVGPreserveAspectRatioType align) { m_align = align; }
    SVGPreserveAspectRatioType align() const { return m_align; }

    void setMeetOrSlice(SVGMeetOrSliceType meetOrSlice) { m_meetOrSlice = meetOrSlice; }
    SVGMeetOrSliceType meetOrSlice() const { return m_meetOrSlice; }

    void transformRect(FloatRect& destRect, FloatRect& srcRect);

    AffineTransform getCTM(float logicX, float logicY,
                           float logicWidth, float logicHeight,
                           float physWidth, float physHeight) const;

    String valueAsString() const override;
    virtual void setValueAsString(const String&, ExceptionState&);
    bool parse(const UChar*& ptr, const UChar* end, bool validate);
    bool parse(const LChar*& ptr, const LChar* end, bool validate);

    void add(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*) override;
    void calculateAnimatedValue(SVGAnimationElement*, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> from, PassRefPtrWillBeRawPtr<SVGPropertyBase> to, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement* contextElement) override;
    float calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement* contextElement) override;

    static AnimatedPropertyType classType() { return AnimatedPreserveAspectRatio; }

private:
    SVGPreserveAspectRatio();

    void setDefault();
    template<typename CharType>
    bool parseInternal(const CharType*& ptr, const CharType* end, bool validate);

    SVGPreserveAspectRatioType m_align;
    SVGMeetOrSliceType m_meetOrSlice;
};

inline PassRefPtrWillBeRawPtr<SVGPreserveAspectRatio> toSVGPreserveAspectRatio(PassRefPtrWillBeRawPtr<SVGPropertyBase> passBase)
{
    RefPtrWillBeRawPtr<SVGPropertyBase> base = passBase;
    ASSERT(base->type() == SVGPreserveAspectRatio::classType());
    return static_pointer_cast<SVGPreserveAspectRatio>(base.release());
}

} // namespace blink

#endif // SVGPreserveAspectRatio_h

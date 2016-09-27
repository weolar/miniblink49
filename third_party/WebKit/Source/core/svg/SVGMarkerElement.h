/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
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

#ifndef SVGMarkerElement_h
#define SVGMarkerElement_h

#include "bindings/core/v8/ExceptionState.h"
#include "core/svg/SVGAnimatedAngle.h"
#include "core/svg/SVGAnimatedBoolean.h"
#include "core/svg/SVGAnimatedEnumeration.h"
#include "core/svg/SVGAnimatedLength.h"
#include "core/svg/SVGElement.h"
#include "core/svg/SVGFitToViewBox.h"
#include "platform/heap/Handle.h"

namespace blink {

enum SVGMarkerUnitsType {
    SVGMarkerUnitsUnknown = 0,
    SVGMarkerUnitsUserSpaceOnUse,
    SVGMarkerUnitsStrokeWidth
};
template<> const SVGEnumerationStringEntries& getStaticStringEntries<SVGMarkerUnitsType>();

class SVGMarkerElement final : public SVGElement,
                               public SVGFitToViewBox {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(SVGMarkerElement);
public:
    // Forward declare enumerations in the W3C naming scheme, for IDL generation.
    enum {
        SVG_MARKERUNITS_UNKNOWN = SVGMarkerUnitsUnknown,
        SVG_MARKERUNITS_USERSPACEONUSE = SVGMarkerUnitsUserSpaceOnUse,
        SVG_MARKERUNITS_STROKEWIDTH = SVGMarkerUnitsStrokeWidth
    };

    enum {
        SVG_MARKER_ORIENT_UNKNOWN = SVGMarkerOrientUnknown,
        SVG_MARKER_ORIENT_AUTO = SVGMarkerOrientAuto,
        SVG_MARKER_ORIENT_ANGLE = SVGMarkerOrientAngle
    };

    DECLARE_NODE_FACTORY(SVGMarkerElement);

    AffineTransform viewBoxToViewTransform(float viewWidth, float viewHeight) const;

    void setOrientToAuto();
    void setOrientToAngle(PassRefPtrWillBeRawPtr<SVGAngleTearOff>);

    SVGAnimatedLength* refX() const { return m_refX.get(); }
    SVGAnimatedLength* refY() const { return m_refY.get(); }
    SVGAnimatedLength* markerWidth() const { return m_markerWidth.get(); }
    SVGAnimatedLength* markerHeight() const { return m_markerHeight.get(); }
    SVGAnimatedEnumeration<SVGMarkerUnitsType>* markerUnits() { return m_markerUnits.get(); }
    SVGAnimatedAngle* orientAngle() { return m_orientAngle.get(); }
    SVGAnimatedEnumeration<SVGMarkerOrientType>* orientType() { return m_orientAngle->orientType(); }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGMarkerElement(Document&);

    bool needsPendingResourceHandling() const override { return false; }

    void svgAttributeChanged(const QualifiedName&) override;
    void childrenChanged(const ChildrenChange&) override;

    LayoutObject* createLayoutObject(const ComputedStyle&) override;
    bool layoutObjectIsNeeded(const ComputedStyle&) override { return true; }

    bool selfHasRelativeLengths() const override;

    RefPtrWillBeMember<SVGAnimatedLength> m_refX;
    RefPtrWillBeMember<SVGAnimatedLength> m_refY;
    RefPtrWillBeMember<SVGAnimatedLength> m_markerWidth;
    RefPtrWillBeMember<SVGAnimatedLength> m_markerHeight;
    RefPtrWillBeMember<SVGAnimatedAngle> m_orientAngle;
    RefPtrWillBeMember<SVGAnimatedEnumeration<SVGMarkerUnitsType>> m_markerUnits;
};

} // namespace blink

#endif // SVGMarkerElement_h

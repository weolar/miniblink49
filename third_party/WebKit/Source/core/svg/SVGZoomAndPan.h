/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
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

#ifndef SVGZoomAndPan_h
#define SVGZoomAndPan_h

#include "core/SVGNames.h"
#include "core/dom/QualifiedName.h"
#include "wtf/HashSet.h"

namespace blink {

class ExceptionState;

enum SVGZoomAndPanType {
    SVGZoomAndPanUnknown = 0,
    SVGZoomAndPanDisable,
    SVGZoomAndPanMagnify
};

class SVGZoomAndPan {
public:
    // Forward declare enumerations in the W3C naming scheme, for IDL generation.
    enum {
        SVG_ZOOMANDPAN_UNKNOWN = SVGZoomAndPanUnknown,
        SVG_ZOOMANDPAN_DISABLE = SVGZoomAndPanDisable,
        SVG_ZOOMANDPAN_MAGNIFY = SVGZoomAndPanMagnify
    };

    virtual ~SVGZoomAndPan() { }

    static bool isKnownAttribute(const QualifiedName&);
    static void addSupportedAttributes(HashSet<QualifiedName>&);

    static SVGZoomAndPanType parseFromNumber(unsigned short number)
    {
        if (!number || number > SVGZoomAndPanMagnify)
            return SVGZoomAndPanUnknown;
        return static_cast<SVGZoomAndPanType>(number);
    }

    bool parseZoomAndPan(const LChar*& start, const LChar* end);
    bool parseZoomAndPan(const UChar*& start, const UChar* end);

    bool parseAttribute(const QualifiedName& name, const AtomicString& value)
    {
        if (name == SVGNames::zoomAndPanAttr) {
            m_zoomAndPan = SVGZoomAndPanUnknown;
            if (!value.isEmpty()) {
                if (value.is8Bit()) {
                    const LChar* start = value.characters8();
                    parseZoomAndPan(start, start + value.length());
                } else {
                    const UChar* start = value.characters16();
                    parseZoomAndPan(start, start + value.length());
                }
            }
            return true;
        }

        return false;
    }

    // JS API
    SVGZoomAndPanType zoomAndPan() const { return m_zoomAndPan; }
    virtual void setZoomAndPan(unsigned short value) { m_zoomAndPan = parseFromNumber(value); }
    virtual void setZoomAndPan(unsigned short value, ExceptionState&) { setZoomAndPan(value); }

protected:
    SVGZoomAndPan();
    void resetZoomAndPan();

private:
    SVGZoomAndPanType m_zoomAndPan;
};

} // namespace blink

#endif // SVGZoomAndPan_h

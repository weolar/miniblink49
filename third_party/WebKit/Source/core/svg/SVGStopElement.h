/*
 * Copyright (C) 2004, 2005, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
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

#ifndef SVGStopElement_h
#define SVGStopElement_h

#include "core/SVGNames.h"
#include "core/svg/SVGAnimatedNumber.h"
#include "core/svg/SVGElement.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGStopElement final : public SVGElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(SVGStopElement);

    Color stopColorIncludingOpacity() const;

    SVGAnimatedNumber* offset() { return m_offset.get(); }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGStopElement(Document&);

    void svgAttributeChanged(const QualifiedName&) override;

    LayoutObject* createLayoutObject(const ComputedStyle&) override;
    bool layoutObjectIsNeeded(const ComputedStyle&) override;

    RefPtrWillBeMember<SVGAnimatedNumber> m_offset;
};

} // namespace blink

#endif // SVGStopElement_h

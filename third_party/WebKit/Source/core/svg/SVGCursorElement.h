/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
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

#ifndef SVGCursorElement_h
#define SVGCursorElement_h

#include "core/SVGNames.h"
#include "core/svg/SVGAnimatedLength.h"
#include "core/svg/SVGElement.h"
#include "core/svg/SVGTests.h"
#include "core/svg/SVGURIReference.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGCursorElement final : public SVGElement,
                               public SVGTests,
                               public SVGURIReference {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(SVGCursorElement);
public:
    DECLARE_NODE_FACTORY(SVGCursorElement);

    ~SVGCursorElement() override;

    void addClient(SVGElement*);
#if !ENABLE(OILPAN)
    void removeClient(SVGElement*);
#endif
    void removeReferencedElement(SVGElement*);

    SVGAnimatedLength* x() const { return m_x.get(); }
    SVGAnimatedLength* y() const { return m_y.get(); }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGCursorElement(Document&);

    bool isValid() const override { return SVGTests::isValid(document()); }

    void svgAttributeChanged(const QualifiedName&) override;

    bool layoutObjectIsNeeded(const ComputedStyle&) override { return false; }

    RefPtrWillBeMember<SVGAnimatedLength> m_x;
    RefPtrWillBeMember<SVGAnimatedLength> m_y;

    WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>> m_clients;
};

} // namespace blink

#endif // SVGCursorElement_h

/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2007 Rob Buis <buis@kde.org>
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

#ifndef SVGViewElement_h
#define SVGViewElement_h

#include "core/SVGNames.h"
#include "core/svg/SVGAnimatedBoolean.h"
#include "core/svg/SVGElement.h"
#include "core/svg/SVGFitToViewBox.h"
#include "core/svg/SVGStaticStringList.h"
#include "core/svg/SVGZoomAndPan.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGViewElement final : public SVGElement,
                             public SVGFitToViewBox,
                             public SVGZoomAndPan {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(SVGViewElement);
public:
    DECLARE_NODE_FACTORY(SVGViewElement);

#if !ENABLE(OILPAN)
    using SVGElement::ref;
    using SVGElement::deref;
#endif

    SVGStringListTearOff* viewTarget() { return m_viewTarget->tearOff(); }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGViewElement(Document&);

    void parseAttribute(const QualifiedName&, const AtomicString&) override;

    bool layoutObjectIsNeeded(const ComputedStyle&) override { return false; }

    RefPtrWillBeMember<SVGStaticStringList> m_viewTarget;
};

} // namespace blink

#endif // SVGViewElement_h

/*
 * Copyright (C) 2004, 2005, 2006, 2007 Nikolas Zimmermann <zimmermann@kde.org>
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

#ifndef SVGFilterPrimitiveStandardAttributes_h
#define SVGFilterPrimitiveStandardAttributes_h

#include "core/svg/SVGAnimatedLength.h"
#include "core/svg/SVGAnimatedString.h"
#include "core/svg/SVGElement.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class Filter;
class FilterEffect;
class SVGFilterBuilder;

class SVGFilterPrimitiveStandardAttributes : public SVGElement {
    // No DEFINE_WRAPPERTYPEINFO() here because a) this class is never
    // instantiated, and b) we don't generate corresponding V8T.h or V8T.cpp.
    // The subclasses must write DEFINE_WRAPPERTYPEINFO().
public:
    void setStandardAttributes(FilterEffect*) const;

    virtual PassRefPtrWillBeRawPtr<FilterEffect> build(SVGFilterBuilder*, Filter*) = 0;
    // Returns true, if the new value is different from the old one.
    virtual bool setFilterEffectAttribute(FilterEffect*, const QualifiedName&);

    // JS API
    SVGAnimatedLength* x() const { return m_x.get(); }
    SVGAnimatedLength* y() const { return m_y.get(); }
    SVGAnimatedLength* width() const { return m_width.get(); }
    SVGAnimatedLength* height() const { return m_height.get(); }
    SVGAnimatedString* result() const { return m_result.get(); }

    DECLARE_VIRTUAL_TRACE();

protected:
    SVGFilterPrimitiveStandardAttributes(const QualifiedName&, Document&);

    void svgAttributeChanged(const QualifiedName&) override;
    void childrenChanged(const ChildrenChange&) override;

    void invalidate();
    void primitiveAttributeChanged(const QualifiedName&);

private:
    bool isFilterEffect() const final { return true; }

    LayoutObject* createLayoutObject(const ComputedStyle&) override;
    bool layoutObjectIsNeeded(const ComputedStyle&) final;

    RefPtrWillBeMember<SVGAnimatedLength> m_x;
    RefPtrWillBeMember<SVGAnimatedLength> m_y;
    RefPtrWillBeMember<SVGAnimatedLength> m_width;
    RefPtrWillBeMember<SVGAnimatedLength> m_height;
    RefPtrWillBeMember<SVGAnimatedString> m_result;
};

void invalidateFilterPrimitiveParent(SVGElement*);

} // namespace blink

#endif // SVGFilterPrimitiveStandardAttributes_h

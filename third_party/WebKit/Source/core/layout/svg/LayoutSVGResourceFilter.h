/*
 * Copyright (C) 2004, 2005, 2006, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
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

#ifndef LayoutSVGResourceFilter_h
#define LayoutSVGResourceFilter_h

#include "core/layout/svg/LayoutSVGResourceContainer.h"
#include "core/svg/SVGFilterElement.h"
#include "core/svg/graphics/filters/SVGFilter.h"
#include "core/svg/graphics/filters/SVGFilterBuilder.h"

namespace blink {

class FilterData final : public NoBaseWillBeGarbageCollectedFinalized<FilterData> {
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(FilterData);
public:
    /*
     * The state transitions should follow the following:
     * Initial -> RecordingContent -> ReadyToPaint -> PaintingFilter -> ReadyToPaint
     *               |     ^                              |     ^
     *               v     |                              v     |
     *     RecordingContentCycleDetected            PaintingFilterCycle
     */
    enum FilterDataState {
        Initial,
        RecordingContent,
        RecordingContentCycleDetected,
        ReadyToPaint,
        PaintingFilter,
        PaintingFilterCycleDetected
    };

    static PassOwnPtrWillBeRawPtr<FilterData> create()
    {
        return adoptPtrWillBeNoop(new FilterData());
    }

    void dispose();

    DECLARE_TRACE();

    RefPtrWillBeMember<SVGFilter> filter;
    RefPtrWillBeMember<SVGFilterBuilder> builder;
    FilterDataState m_state;

private:
    FilterData() : m_state(Initial) { }
};

class LayoutSVGResourceFilter final : public LayoutSVGResourceContainer {
public:
    explicit LayoutSVGResourceFilter(SVGFilterElement*);

    virtual ~LayoutSVGResourceFilter();

    virtual bool isChildAllowed(LayoutObject*, const ComputedStyle&) const override;

    virtual const char* name() const override { return "LayoutSVGResourceFilter"; }
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectSVGResourceFilter || LayoutSVGResourceContainer::isOfType(type); }

    virtual void removeAllClientsFromCache(bool markForInvalidation = true) override;
    virtual void removeClientFromCache(LayoutObject*, bool markForInvalidation = true) override;

    FloatRect resourceBoundingBox(const LayoutObject*);

    PassRefPtrWillBeRawPtr<SVGFilterBuilder> buildPrimitives(SVGFilter*);

    SVGUnitTypes::SVGUnitType filterUnits() const { return toSVGFilterElement(element())->filterUnits()->currentValue()->enumValue(); }
    SVGUnitTypes::SVGUnitType primitiveUnits() const { return toSVGFilterElement(element())->primitiveUnits()->currentValue()->enumValue(); }

    void primitiveAttributeChanged(LayoutObject*, const QualifiedName&);

    static const LayoutSVGResourceType s_resourceType = FilterResourceType;
    virtual LayoutSVGResourceType resourceType() const override { return s_resourceType; }

    FilterData* getFilterDataForLayoutObject(LayoutObject* object) { return m_filter.get(object); }
    void setFilterDataForLayoutObject(LayoutObject* object, PassOwnPtrWillBeRawPtr<FilterData> filterData) { m_filter.set(object, filterData); }

protected:
    virtual void willBeDestroyed() override;

private:
    void disposeFilterMap();

    using FilterMap = WillBePersistentHeapHashMap<LayoutObject*, OwnPtrWillBeMember<FilterData>>;
    FilterMap m_filter;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutSVGResourceFilter, isSVGResourceFilter());

}

#endif

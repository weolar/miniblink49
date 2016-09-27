/*
 * Copyright (C) 2008 Alex Mathews <possessedpenguinbob@gmail.com>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
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

#ifndef SVGFilterBuilder_h
#define SVGFilterBuilder_h

#include "platform/graphics/filters/FilterEffect.h"
#include "platform/heap/Handle.h"
#include "wtf/HashMap.h"
#include "wtf/HashSet.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/AtomicStringHash.h"
#include "wtf/text/WTFString.h"

namespace blink {

class LayoutObject;

class SVGFilterBuilder final : public RefCountedWillBeGarbageCollectedFinalized<SVGFilterBuilder> {
public:
    typedef WillBeHeapHashSet<RawPtrWillBeMember<FilterEffect>> FilterEffectSet;

    static PassRefPtrWillBeRawPtr<SVGFilterBuilder> create(PassRefPtrWillBeRawPtr<FilterEffect> sourceGraphic)
    {
        return adoptRefWillBeNoop(new SVGFilterBuilder(sourceGraphic));
    }

    void add(const AtomicString& id, PassRefPtrWillBeRawPtr<FilterEffect>);

    FilterEffect* getEffectById(const AtomicString& id) const;
    FilterEffect* lastEffect() const { return m_lastEffect.get(); }

    void appendEffectToEffectReferences(PassRefPtrWillBeRawPtr<FilterEffect>, LayoutObject*);

    inline FilterEffectSet& effectReferences(FilterEffect* effect)
    {
        // Only allowed for effects belongs to this builder.
        ASSERT(m_effectReferences.contains(effect));
        return m_effectReferences.find(effect)->value;
    }

    // Required to change the attributes of a filter during an svgAttributeChanged.
    inline FilterEffect* effectByRenderer(LayoutObject* object) { return m_effectRenderer.get(object); }

    void clearEffects();
    void clearResultsRecursive(FilterEffect*);

    DECLARE_TRACE();

private:
    SVGFilterBuilder(PassRefPtrWillBeRawPtr<FilterEffect> sourceGraphic);

    inline void addBuiltinEffects()
    {
        for (const auto& entry : m_builtinEffects)
            m_effectReferences.add(entry.value, FilterEffectSet());
    }

    typedef WillBeHeapHashMap<AtomicString, RefPtrWillBeMember<FilterEffect>> NamedFilterEffectMap;

    NamedFilterEffectMap m_builtinEffects;
    NamedFilterEffectMap m_namedEffects;
    // The value is a list, which contains those filter effects,
    // which depends on the key filter effect.
    WillBeHeapHashMap<RefPtrWillBeMember<FilterEffect>, FilterEffectSet> m_effectReferences;
    WillBeHeapHashMap<LayoutObject*, RawPtrWillBeMember<FilterEffect>> m_effectRenderer;

    RefPtrWillBeMember<FilterEffect> m_lastEffect;
};

} // namespace blink

#endif // SVGFilterBuilder_h

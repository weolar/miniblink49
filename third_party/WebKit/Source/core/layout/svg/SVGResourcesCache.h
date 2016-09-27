/*
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

#ifndef SVGResourcesCache_h
#define SVGResourcesCache_h

#include "core/style/StyleDifference.h"
#include "wtf/FastAllocBase.h"
#include "wtf/HashMap.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"

namespace blink {

class LayoutObject;
class ComputedStyle;
class LayoutSVGResourceContainer;
class SVGResources;

class SVGResourcesCache {
    WTF_MAKE_NONCOPYABLE(SVGResourcesCache); WTF_MAKE_FAST_ALLOCATED(SVGResourcesCache);
public:
    SVGResourcesCache();
    ~SVGResourcesCache();

    static SVGResources* cachedResourcesForLayoutObject(const LayoutObject*);

    // Called from all SVG layoutObjects addChild() methods.
    static void clientWasAddedToTree(LayoutObject*, const ComputedStyle& newStyle);

    // Called from all SVG layoutObjects removeChild() methods.
    static void clientWillBeRemovedFromTree(LayoutObject*);

    // Called from all SVG layoutObjects destroy() methods - except for LayoutSVGResourceContainer.
    static void clientDestroyed(LayoutObject*);

    // Called from all SVG layoutObjects layout() methods.
    static void clientLayoutChanged(LayoutObject*);

    // Called from all SVG layoutObjects styleDidChange() methods.
    static void clientStyleChanged(LayoutObject*, StyleDifference, const ComputedStyle& newStyle);

    // Called from LayoutSVGResourceContainer::willBeDestroyed().
    static void resourceDestroyed(LayoutSVGResourceContainer*);

private:
    void addResourcesFromLayoutObject(LayoutObject*, const ComputedStyle&);
    void removeResourcesFromLayoutObject(LayoutObject*);

    typedef HashMap<const LayoutObject*, OwnPtr<SVGResources>> CacheMap;
    CacheMap m_cache;
};

}

#endif

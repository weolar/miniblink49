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

#include "config.h"
#include "core/layout/svg/SVGResourcesCycleSolver.h"

// Set to a value > 0, to debug the resource cache.
#define DEBUG_CYCLE_DETECTION 0

#include "core/layout/svg/LayoutSVGResourceClipper.h"
#include "core/layout/svg/LayoutSVGResourceFilter.h"
#include "core/layout/svg/LayoutSVGResourceMarker.h"
#include "core/layout/svg/LayoutSVGResourceMasker.h"
#include "core/layout/svg/LayoutSVGResourcePaintServer.h"
#include "core/layout/svg/SVGResources.h"
#include "core/layout/svg/SVGResourcesCache.h"

namespace blink {

SVGResourcesCycleSolver::SVGResourcesCycleSolver(LayoutObject* layoutObject, SVGResources* resources)
    : m_layoutObject(layoutObject)
    , m_resources(resources)
{
    ASSERT(m_layoutObject);
    ASSERT(m_resources);
}

SVGResourcesCycleSolver::~SVGResourcesCycleSolver()
{
}

struct ActiveFrame {
    typedef SVGResourcesCycleSolver::ResourceSet ResourceSet;

    ActiveFrame(ResourceSet& activeSet, LayoutSVGResourceContainer* resource)
        : m_activeSet(activeSet)
        , m_resource(resource)
    {
        m_activeSet.add(m_resource);
    }
    ~ActiveFrame()
    {
        m_activeSet.remove(m_resource);
    }

    ResourceSet& m_activeSet;
    LayoutSVGResourceContainer* m_resource;
};

bool SVGResourcesCycleSolver::resourceContainsCycles(LayoutSVGResourceContainer* resource)
{
    // If we've traversed this sub-graph before and no cycles were observed, then
    // reuse that result.
    if (m_dagCache.contains(resource))
        return false;

    ActiveFrame frame(m_activeResources, resource);

    LayoutObject* node = resource;
    while (node) {
        // Skip subtrees which are themselves resources. (They will be
        // processed - if needed - when they are actually referenced.)
        if (node != resource && node->isSVGResourceContainer()) {
            node = node->nextInPreOrderAfterChildren(resource);
            continue;
        }
        if (SVGResources* nodeResources = SVGResourcesCache::cachedResourcesForLayoutObject(node)) {
            // Fetch all the resources referenced by |node|.
            ResourceSet nodeSet;
            nodeResources->buildSetOfResources(nodeSet);

            // Iterate resources referenced by |node|.
            for (auto* node : nodeSet) {
                if (m_activeResources.contains(node) || resourceContainsCycles(node))
                    return true;
            }
        }
        node = node->nextInPreOrder(resource);
    }

    // No cycles found in (or from) this resource. Add it to the "DAG cache".
    m_dagCache.add(resource);
    return false;
}

void SVGResourcesCycleSolver::resolveCycles()
{
    ASSERT(m_activeResources.isEmpty());

    // If the starting LayoutObject is a resource container itself, then add it
    // to the active set (to break direct self-references.)
    if (m_layoutObject->isSVGResourceContainer())
        m_activeResources.add(toLayoutSVGResourceContainer(m_layoutObject));

    ResourceSet localResources;
    m_resources->buildSetOfResources(localResources);

    // This performs a depth-first search for a back-edge in all the
    // (potentially disjoint) graphs formed by the resources referenced by
    // |m_layoutObject|.
    for (auto* localResource : localResources) {
        if (m_activeResources.contains(localResource) || resourceContainsCycles(localResource))
            breakCycle(localResource);
    }

    m_activeResources.clear();
}

void SVGResourcesCycleSolver::breakCycle(LayoutSVGResourceContainer* resourceLeadingToCycle)
{
    ASSERT(resourceLeadingToCycle);
    if (resourceLeadingToCycle == m_resources->linkedResource()) {
        m_resources->resetLinkedResource();
        return;
    }

    switch (resourceLeadingToCycle->resourceType()) {
    case MaskerResourceType:
        ASSERT(resourceLeadingToCycle == m_resources->masker());
        m_resources->resetMasker();
        break;
    case MarkerResourceType:
        ASSERT(resourceLeadingToCycle == m_resources->markerStart() || resourceLeadingToCycle == m_resources->markerMid() || resourceLeadingToCycle == m_resources->markerEnd());
        if (m_resources->markerStart() == resourceLeadingToCycle)
            m_resources->resetMarkerStart();
        if (m_resources->markerMid() == resourceLeadingToCycle)
            m_resources->resetMarkerMid();
        if (m_resources->markerEnd() == resourceLeadingToCycle)
            m_resources->resetMarkerEnd();
        break;
    case PatternResourceType:
    case LinearGradientResourceType:
    case RadialGradientResourceType:
        ASSERT(resourceLeadingToCycle == m_resources->fill() || resourceLeadingToCycle == m_resources->stroke());
        if (m_resources->fill() == resourceLeadingToCycle)
            m_resources->resetFill();
        if (m_resources->stroke() == resourceLeadingToCycle)
            m_resources->resetStroke();
        break;
    case FilterResourceType:
        ASSERT(resourceLeadingToCycle == m_resources->filter());
        m_resources->resetFilter();
        break;
    case ClipperResourceType:
        ASSERT(resourceLeadingToCycle == m_resources->clipper());
        m_resources->resetClipper();
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }
}

}

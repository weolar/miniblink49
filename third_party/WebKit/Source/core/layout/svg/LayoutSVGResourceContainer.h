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

#ifndef LayoutSVGResourceContainer_h
#define LayoutSVGResourceContainer_h

#include "core/layout/svg/LayoutSVGHiddenContainer.h"
#include "core/svg/SVGDocumentExtensions.h"

namespace blink {

enum LayoutSVGResourceType {
    MaskerResourceType,
    MarkerResourceType,
    PatternResourceType,
    LinearGradientResourceType,
    RadialGradientResourceType,
    FilterResourceType,
    ClipperResourceType
};

class DeprecatedPaintLayer;

class LayoutSVGResourceContainer : public LayoutSVGHiddenContainer {
public:
    explicit LayoutSVGResourceContainer(SVGElement*);
    virtual ~LayoutSVGResourceContainer();

    virtual void removeAllClientsFromCache(bool markForInvalidation = true) = 0;
    virtual void removeClientFromCache(LayoutObject*, bool markForInvalidation = true) = 0;

    virtual void layout() override;
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override final;
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectSVGResourceContainer || LayoutSVGHiddenContainer::isOfType(type); }

    virtual LayoutSVGResourceType resourceType() const = 0;

    bool isSVGPaintServer() const
    {
        LayoutSVGResourceType resourceType = this->resourceType();
        return resourceType == PatternResourceType
            || resourceType == LinearGradientResourceType
            || resourceType == RadialGradientResourceType;
    }

    void idChanged();
    void addClientLayer(Node*);
    void addClientLayer(DeprecatedPaintLayer*);
    void removeClientLayer(DeprecatedPaintLayer*);

    void invalidateCacheAndMarkForLayout(SubtreeLayoutScope* = nullptr);

    static void markForLayoutAndParentResourceInvalidation(LayoutObject*, bool needsLayout = true);

    void clearInvalidationMask() { m_invalidationMask = 0; }

protected:
    // When adding modes, make sure we don't overflow m_invalidationMask below.
    enum InvalidationMode {
        LayoutAndBoundariesInvalidation = 1 << 0,
        BoundariesInvalidation = 1 << 1,
        PaintInvalidation = 1 << 2,
        ParentOnlyInvalidation = 1 << 3
    };

    // Used from the invalidateClient/invalidateClients methods from classes, inheriting from us.
    void markAllClientsForInvalidation(InvalidationMode);
    void markAllClientLayersForInvalidation();
    void markClientForInvalidation(LayoutObject*, InvalidationMode);

    virtual void willBeDestroyed() override;

    bool m_isInLayout;

private:
    friend class SVGResourcesCache;
    void addClient(LayoutObject*);
    void removeClient(LayoutObject*);

    void registerResource();

    AtomicString m_id;
    // Track global (markAllClientsForInvalidation) invals to avoid redundant crawls.
    unsigned m_invalidationMask : 8;

    unsigned m_registered : 1;
    unsigned m_isInvalidating : 1;
    // 22 padding bits available

    HashSet<LayoutObject*> m_clients;
    HashSet<DeprecatedPaintLayer*> m_clientLayers;
};

inline LayoutSVGResourceContainer* getLayoutSVGResourceContainerById(TreeScope& treeScope, const AtomicString& id)
{
    if (id.isEmpty())
        return nullptr;

    if (LayoutSVGResourceContainer* layoutResource = treeScope.document().accessSVGExtensions().resourceById(id))
        return layoutResource;

    return nullptr;
}

template<typename Layout>
Layout* getLayoutSVGResourceById(TreeScope& treeScope, const AtomicString& id)
{
    if (LayoutSVGResourceContainer* container = getLayoutSVGResourceContainerById(treeScope, id)) {
        if (container->resourceType() == Layout::s_resourceType)
            return static_cast<Layout*>(container);
    }
    return nullptr;
}

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutSVGResourceContainer, isSVGResourceContainer());

#define DEFINE_LAYOUT_SVG_RESOURCE_TYPE_CASTS(thisType, typeName) \
    DEFINE_TYPE_CASTS(thisType, LayoutSVGResourceContainer, resource, resource->resourceType() == typeName, resource.resourceType() == typeName)

}

#endif

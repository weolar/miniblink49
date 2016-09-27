/*
 * Copyright (C) 2012 Adobe Systems Incorporated. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"
#include "core/paint/DeprecatedPaintLayerFilterInfo.h"

#include "core/fetch/DocumentResourceReference.h"
#include "core/layout/svg/LayoutSVGResourceContainer.h"
#include "core/layout/svg/ReferenceFilterBuilder.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/FilterEffectBuilder.h"
#include "core/svg/SVGFilterElement.h"

namespace blink {

DeprecatedPaintLayerFilterInfoMap* DeprecatedPaintLayerFilterInfo::s_filterMap = 0;

DeprecatedPaintLayerFilterInfo* DeprecatedPaintLayerFilterInfo::filterInfoForLayer(const DeprecatedPaintLayer* layer)
{
    if (!s_filterMap)
        return 0;
    DeprecatedPaintLayerFilterInfoMap::iterator iter = s_filterMap->find(layer);
    return (iter != s_filterMap->end()) ? iter->value : 0;
}

DeprecatedPaintLayerFilterInfo* DeprecatedPaintLayerFilterInfo::createFilterInfoForLayerIfNeeded(DeprecatedPaintLayer* layer)
{
    if (!s_filterMap)
        s_filterMap = new DeprecatedPaintLayerFilterInfoMap();

    DeprecatedPaintLayerFilterInfoMap::iterator iter = s_filterMap->find(layer);
    if (iter != s_filterMap->end()) {
        ASSERT(layer->hasFilterInfo());
        return iter->value;
    }

    DeprecatedPaintLayerFilterInfo* filter = new DeprecatedPaintLayerFilterInfo(layer);
    s_filterMap->set(layer, filter);
    layer->setHasFilterInfo(true);
    return filter;
}

void DeprecatedPaintLayerFilterInfo::removeFilterInfoForLayer(DeprecatedPaintLayer* layer)
{
    if (!s_filterMap)
        return;
    DeprecatedPaintLayerFilterInfo* filter = s_filterMap->take(layer);
    if (s_filterMap->isEmpty()) {
        delete s_filterMap;
        s_filterMap = 0;
    }
    if (!filter) {
        ASSERT(!layer->hasFilterInfo());
        return;
    }
    layer->setHasFilterInfo(false);
    delete filter;
}

DeprecatedPaintLayerFilterInfo::DeprecatedPaintLayerFilterInfo(DeprecatedPaintLayer* layer)
    : m_layer(layer)
{
}

DeprecatedPaintLayerFilterInfo::~DeprecatedPaintLayerFilterInfo()
{
    removeReferenceFilterClients();
}

void DeprecatedPaintLayerFilterInfo::setBuilder(PassRefPtrWillBeRawPtr<FilterEffectBuilder> builder)
{
    m_builder = builder;
}

void DeprecatedPaintLayerFilterInfo::notifyFinished(Resource*)
{
    LayoutObject* layoutObject = m_layer->layoutObject();
    // FIXME: This caller of scheduleSVGFilterLayerUpdateHack() is not correct. It's using the layer update
    // system to trigger a Layer to go through the filter updating logic, but that might not
    // even happen if this element is style sharing and LayoutObject::setStyle() returns early.
    // Filters need to find a better way to hook into the system.
    toElement(layoutObject->node())->scheduleSVGFilterLayerUpdateHack();
    layoutObject->setShouldDoFullPaintInvalidation();
}

void DeprecatedPaintLayerFilterInfo::updateReferenceFilterClients(const FilterOperations& operations)
{
    removeReferenceFilterClients();
    for (size_t i = 0; i < operations.size(); ++i) {
        RefPtrWillBeRawPtr<FilterOperation> filterOperation = operations.operations().at(i);
        if (filterOperation->type() != FilterOperation::REFERENCE)
            continue;
        ReferenceFilterOperation* referenceFilterOperation = toReferenceFilterOperation(filterOperation.get());
        DocumentResourceReference* documentReference = ReferenceFilterBuilder::documentResourceReference(referenceFilterOperation);
        DocumentResource* cachedSVGDocument = documentReference ? documentReference->document() : 0;

        if (cachedSVGDocument) {
            // Reference is external; wait for notifyFinished().
            cachedSVGDocument->addClient(this);
            m_externalSVGReferences.append(cachedSVGDocument);
        } else {
            // Reference is internal; add layer as a client so we can trigger
            // filter paint invalidation on SVG attribute change.
            Element* filter = m_layer->layoutObject()->node()->document().getElementById(referenceFilterOperation->fragment());
            if (!isSVGFilterElement(filter))
                continue;
            if (filter->layoutObject())
                toLayoutSVGResourceContainer(filter->layoutObject())->addClientLayer(m_layer);
            else
                toSVGFilterElement(filter)->addClient(m_layer->layoutObject()->node());
            m_internalSVGReferences.append(filter);
        }
    }
}

void DeprecatedPaintLayerFilterInfo::removeReferenceFilterClients()
{
    for (size_t i = 0; i < m_externalSVGReferences.size(); ++i)
        m_externalSVGReferences.at(i)->removeClient(this);
    m_externalSVGReferences.clear();
    for (size_t i = 0; i < m_internalSVGReferences.size(); ++i) {
        Element* filter = m_internalSVGReferences.at(i).get();
        if (filter->layoutObject())
            toLayoutSVGResourceContainer(filter->layoutObject())->removeClientLayer(m_layer);
        else
            toSVGFilterElement(filter)->removeClient(m_layer->layoutObject()->node());
    }
    m_internalSVGReferences.clear();
}

} // namespace blink


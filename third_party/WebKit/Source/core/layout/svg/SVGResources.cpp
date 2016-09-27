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
#include "core/layout/svg/SVGResources.h"

#include "core/SVGNames.h"
#include "core/layout/svg/LayoutSVGResourceClipper.h"
#include "core/layout/svg/LayoutSVGResourceFilter.h"
#include "core/layout/svg/LayoutSVGResourceMarker.h"
#include "core/layout/svg/LayoutSVGResourceMasker.h"
#include "core/layout/svg/LayoutSVGResourcePaintServer.h"
#include "core/style/SVGComputedStyle.h"
#include "core/svg/SVGFilterElement.h"
#include "core/svg/SVGGradientElement.h"
#include "core/svg/SVGPatternElement.h"
#include "core/svg/SVGURIReference.h"

#ifndef NDEBUG
#include <stdio.h>
#endif

namespace blink {

using namespace SVGNames;

SVGResources::SVGResources()
    : m_linkedResource(nullptr)
{
}

static HashSet<AtomicString>& clipperFilterMaskerTags()
{
    DEFINE_STATIC_LOCAL(HashSet<AtomicString>, s_tagList, ());
    if (s_tagList.isEmpty()) {
        // "container elements": http://www.w3.org/TR/SVG11/intro.html#TermContainerElement
        // "graphics elements" : http://www.w3.org/TR/SVG11/intro.html#TermGraphicsElement
        s_tagList.add(aTag.localName());
        s_tagList.add(circleTag.localName());
        s_tagList.add(ellipseTag.localName());
        s_tagList.add(gTag.localName());
        s_tagList.add(imageTag.localName());
        s_tagList.add(lineTag.localName());
        s_tagList.add(markerTag.localName());
        s_tagList.add(maskTag.localName());
        s_tagList.add(pathTag.localName());
        s_tagList.add(polygonTag.localName());
        s_tagList.add(polylineTag.localName());
        s_tagList.add(rectTag.localName());
        s_tagList.add(svgTag.localName());
        s_tagList.add(textTag.localName());
        s_tagList.add(useTag.localName());

        // Not listed in the definitions is the clipPath element, the SVG spec says though:
        // The "clipPath" element or any of its children can specify property "clip-path".
        // So we have to add clipPathTag here, otherwhise clip-path on clipPath will fail.
        // (Already mailed SVG WG, waiting for a solution)
        s_tagList.add(clipPathTag.localName());

        // Not listed in the definitions are the text content elements, though filter/clipper/masker on tspan/text/.. is allowed.
        // (Already mailed SVG WG, waiting for a solution)
        s_tagList.add(textPathTag.localName());
        s_tagList.add(tspanTag.localName());

        // Not listed in the definitions is the foreignObject element, but clip-path
        // is a supported attribute.
        s_tagList.add(foreignObjectTag.localName());

        // Elements that we ignore, as it doesn't make any sense.
        // defs, pattern, switch (FIXME: Mail SVG WG about these)
        // symbol (is converted to a svg element, when referenced by use, we can safely ignore it.)
    }

    return s_tagList;
}

bool SVGResources::supportsMarkers(const SVGElement& element)
{
    DEFINE_STATIC_LOCAL(HashSet<AtomicString>, s_tagList, ());
    if (s_tagList.isEmpty()) {
        s_tagList.add(lineTag.localName());
        s_tagList.add(pathTag.localName());
        s_tagList.add(polygonTag.localName());
        s_tagList.add(polylineTag.localName());
    }

    return s_tagList.contains(element.localName());
}

static HashSet<AtomicString>& fillAndStrokeTags()
{
    DEFINE_STATIC_LOCAL(HashSet<AtomicString>, s_tagList, ());
    if (s_tagList.isEmpty()) {
        s_tagList.add(circleTag.localName());
        s_tagList.add(ellipseTag.localName());
        s_tagList.add(lineTag.localName());
        s_tagList.add(pathTag.localName());
        s_tagList.add(polygonTag.localName());
        s_tagList.add(polylineTag.localName());
        s_tagList.add(rectTag.localName());
        s_tagList.add(textTag.localName());
        s_tagList.add(textPathTag.localName());
        s_tagList.add(tspanTag.localName());
    }

    return s_tagList;
}

static HashSet<AtomicString>& chainableResourceTags()
{
    DEFINE_STATIC_LOCAL(HashSet<AtomicString>, s_tagList, ());
    if (s_tagList.isEmpty()) {
        s_tagList.add(linearGradientTag.localName());
        s_tagList.add(filterTag.localName());
        s_tagList.add(patternTag.localName());
        s_tagList.add(radialGradientTag.localName());
    }

    return s_tagList;
}

static inline AtomicString targetReferenceFromResource(SVGElement& element)
{
    String target;
    if (isSVGPatternElement(element))
        target = toSVGPatternElement(element).href()->currentValue()->value();
    else if (isSVGGradientElement(element))
        target = toSVGGradientElement(element).href()->currentValue()->value();
    else if (isSVGFilterElement(element))
        target = toSVGFilterElement(element).href()->currentValue()->value();
    else
        ASSERT_NOT_REACHED();

    return SVGURIReference::fragmentIdentifierFromIRIString(target, element.treeScope());
}

static inline bool svgPaintTypeHasURL(SVGPaintType paintType)
{
    switch (paintType) {
    case SVG_PAINTTYPE_URI_NONE:
    case SVG_PAINTTYPE_URI_CURRENTCOLOR:
    case SVG_PAINTTYPE_URI_RGBCOLOR:
    case SVG_PAINTTYPE_URI:
        return true;
    default:
        break;
    }
    return false;
}

static inline LayoutSVGResourcePaintServer* paintingResourceFromSVGPaint(TreeScope& treeScope, const SVGPaintType& paintType, const String& paintUri, AtomicString& id, bool& hasPendingResource)
{
    if (!svgPaintTypeHasURL(paintType))
        return nullptr;

    id = SVGURIReference::fragmentIdentifierFromIRIString(paintUri, treeScope);
    LayoutSVGResourceContainer* container = getLayoutSVGResourceContainerById(treeScope, id);
    if (!container) {
        hasPendingResource = true;
        return nullptr;
    }

    if (!container->isSVGPaintServer())
        return nullptr;

    return toLayoutSVGResourcePaintServer(container);
}

static inline void registerPendingResource(SVGDocumentExtensions& extensions, const AtomicString& id, SVGElement* element)
{
    ASSERT(element);
    extensions.addPendingResource(id, element);
}

bool SVGResources::hasResourceData() const
{
    return m_clipperFilterMaskerData
        || m_markerData
        || m_fillStrokeData
        || m_linkedResource;
}

static inline SVGResources& ensureResources(OwnPtr<SVGResources>& resources)
{
    if (!resources)
        resources = adoptPtr(new SVGResources);

    return *resources.get();
}

PassOwnPtr<SVGResources> SVGResources::buildResources(const LayoutObject* object, const SVGComputedStyle& style)
{
    ASSERT(object);

    Node* node = object->node();
    ASSERT(node);
    ASSERT_WITH_SECURITY_IMPLICATION(node->isSVGElement());

    SVGElement* element = toSVGElement(node);
    ASSERT(element);

    const AtomicString& tagName = element->localName();
    ASSERT(!tagName.isNull());

    TreeScope& treeScope = element->treeScope();
    SVGDocumentExtensions& extensions = element->document().accessSVGExtensions();

    OwnPtr<SVGResources> resources;
    if (clipperFilterMaskerTags().contains(tagName)) {
        if (style.hasClipper()) {
            AtomicString id = style.clipperResource();
            if (!ensureResources(resources).setClipper(getLayoutSVGResourceById<LayoutSVGResourceClipper>(treeScope, id)))
                registerPendingResource(extensions, id, element);
        }

        if (style.hasFilter()) {
            AtomicString id = style.filterResource();
            if (!ensureResources(resources).setFilter(getLayoutSVGResourceById<LayoutSVGResourceFilter>(treeScope, id)))
                registerPendingResource(extensions, id, element);
        }

        if (style.hasMasker()) {
            AtomicString id = style.maskerResource();
            if (!ensureResources(resources).setMasker(getLayoutSVGResourceById<LayoutSVGResourceMasker>(treeScope, id)))
                registerPendingResource(extensions, id, element);
        }
    }

    if (style.hasMarkers() && supportsMarkers(*element)) {
        const AtomicString& markerStartId = style.markerStartResource();
        if (!ensureResources(resources).setMarkerStart(getLayoutSVGResourceById<LayoutSVGResourceMarker>(treeScope, markerStartId)))
            registerPendingResource(extensions, markerStartId, element);

        const AtomicString& markerMidId = style.markerMidResource();
        if (!ensureResources(resources).setMarkerMid(getLayoutSVGResourceById<LayoutSVGResourceMarker>(treeScope, markerMidId)))
            registerPendingResource(extensions, markerMidId, element);

        const AtomicString& markerEndId = style.markerEndResource();
        if (!ensureResources(resources).setMarkerEnd(getLayoutSVGResourceById<LayoutSVGResourceMarker>(treeScope, style.markerEndResource())))
            registerPendingResource(extensions, markerEndId, element);
    }

    if (fillAndStrokeTags().contains(tagName)) {
        if (style.hasFill()) {
            bool hasPendingResource = false;
            AtomicString id;
            LayoutSVGResourcePaintServer* resource = paintingResourceFromSVGPaint(treeScope, style.fillPaintType(), style.fillPaintUri(), id, hasPendingResource);
            if (!ensureResources(resources).setFill(resource) && hasPendingResource) {
                registerPendingResource(extensions, id, element);
            }
        }

        if (style.hasStroke()) {
            bool hasPendingResource = false;
            AtomicString id;
            LayoutSVGResourcePaintServer* resource = paintingResourceFromSVGPaint(treeScope, style.strokePaintType(), style.strokePaintUri(), id, hasPendingResource);
            if (!ensureResources(resources).setStroke(resource) && hasPendingResource) {
                registerPendingResource(extensions, id, element);
            }
        }
    }

    if (chainableResourceTags().contains(tagName)) {
        AtomicString id = targetReferenceFromResource(*element);
        if (!ensureResources(resources).setLinkedResource(getLayoutSVGResourceContainerById(treeScope, id)))
            registerPendingResource(extensions, id, element);
    }

    return (!resources || !resources->hasResourceData()) ? nullptr : resources.release();
}

void SVGResources::layoutIfNeeded()
{
    if (m_clipperFilterMaskerData) {
        if (LayoutSVGResourceClipper* clipper = m_clipperFilterMaskerData->clipper)
            clipper->layoutIfNeeded();
        if (LayoutSVGResourceMasker* masker = m_clipperFilterMaskerData->masker)
            masker->layoutIfNeeded();
        if (LayoutSVGResourceFilter* filter = m_clipperFilterMaskerData->filter)
            filter->layoutIfNeeded();
    }

    if (m_markerData) {
        if (LayoutSVGResourceMarker* marker = m_markerData->markerStart)
            marker->layoutIfNeeded();
        if (LayoutSVGResourceMarker* marker = m_markerData->markerMid)
            marker->layoutIfNeeded();
        if (LayoutSVGResourceMarker* marker = m_markerData->markerEnd)
            marker->layoutIfNeeded();
    }

    if (m_fillStrokeData) {
        if (LayoutSVGResourcePaintServer* fill = m_fillStrokeData->fill)
            fill->layoutIfNeeded();
        if (LayoutSVGResourcePaintServer* stroke = m_fillStrokeData->stroke)
            stroke->layoutIfNeeded();
    }

    if (m_linkedResource)
        m_linkedResource->layoutIfNeeded();
}

void SVGResources::removeClientFromCache(LayoutObject* object, bool markForInvalidation) const
{
    if (!hasResourceData())
        return;

    if (m_linkedResource) {
        ASSERT(!m_clipperFilterMaskerData);
        ASSERT(!m_markerData);
        ASSERT(!m_fillStrokeData);
        m_linkedResource->removeClientFromCache(object, markForInvalidation);
        return;
    }

    if (m_clipperFilterMaskerData) {
        if (m_clipperFilterMaskerData->clipper)
            m_clipperFilterMaskerData->clipper->removeClientFromCache(object, markForInvalidation);
        if (m_clipperFilterMaskerData->filter)
            m_clipperFilterMaskerData->filter->removeClientFromCache(object, markForInvalidation);
        if (m_clipperFilterMaskerData->masker)
            m_clipperFilterMaskerData->masker->removeClientFromCache(object, markForInvalidation);
    }

    if (m_markerData) {
        if (m_markerData->markerStart)
            m_markerData->markerStart->removeClientFromCache(object, markForInvalidation);
        if (m_markerData->markerMid)
            m_markerData->markerMid->removeClientFromCache(object, markForInvalidation);
        if (m_markerData->markerEnd)
            m_markerData->markerEnd->removeClientFromCache(object, markForInvalidation);
    }

    if (m_fillStrokeData) {
        if (m_fillStrokeData->fill)
            m_fillStrokeData->fill->removeClientFromCache(object, markForInvalidation);
        if (m_fillStrokeData->stroke)
            m_fillStrokeData->stroke->removeClientFromCache(object, markForInvalidation);
    }
}

void SVGResources::resourceDestroyed(LayoutSVGResourceContainer* resource)
{
    ASSERT(resource);
    if (!hasResourceData())
        return;

    if (m_linkedResource == resource) {
        ASSERT(!m_clipperFilterMaskerData);
        ASSERT(!m_markerData);
        ASSERT(!m_fillStrokeData);
        m_linkedResource->removeAllClientsFromCache();
        m_linkedResource = nullptr;
        return;
    }

    switch (resource->resourceType()) {
    case MaskerResourceType:
        if (!m_clipperFilterMaskerData)
            break;
        if (m_clipperFilterMaskerData->masker == resource) {
            m_clipperFilterMaskerData->masker->removeAllClientsFromCache();
            m_clipperFilterMaskerData->masker = nullptr;
        }
        break;
    case MarkerResourceType:
        if (!m_markerData)
            break;
        if (m_markerData->markerStart == resource) {
            m_markerData->markerStart->removeAllClientsFromCache();
            m_markerData->markerStart = nullptr;
        }
        if (m_markerData->markerMid == resource) {
            m_markerData->markerMid->removeAllClientsFromCache();
            m_markerData->markerMid = nullptr;
        }
        if (m_markerData->markerEnd == resource) {
            m_markerData->markerEnd->removeAllClientsFromCache();
            m_markerData->markerEnd = nullptr;
        }
        break;
    case PatternResourceType:
    case LinearGradientResourceType:
    case RadialGradientResourceType:
        if (!m_fillStrokeData)
            break;
        if (m_fillStrokeData->fill == resource) {
            m_fillStrokeData->fill->removeAllClientsFromCache();
            m_fillStrokeData->fill = nullptr;
        }
        if (m_fillStrokeData->stroke == resource) {
            m_fillStrokeData->stroke->removeAllClientsFromCache();
            m_fillStrokeData->stroke = nullptr;
        }
        break;
    case FilterResourceType:
        if (!m_clipperFilterMaskerData)
            break;
        if (m_clipperFilterMaskerData->filter == resource) {
            m_clipperFilterMaskerData->filter->removeAllClientsFromCache();
            m_clipperFilterMaskerData->filter = nullptr;
        }
        break;
    case ClipperResourceType:
        if (!m_clipperFilterMaskerData)
            break;
        if (m_clipperFilterMaskerData->clipper == resource) {
            m_clipperFilterMaskerData->clipper->removeAllClientsFromCache();
            m_clipperFilterMaskerData->clipper = nullptr;
        }
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

void SVGResources::buildSetOfResources(HashSet<LayoutSVGResourceContainer*>& set)
{
    if (!hasResourceData())
        return;

    if (m_linkedResource) {
        ASSERT(!m_clipperFilterMaskerData);
        ASSERT(!m_markerData);
        ASSERT(!m_fillStrokeData);
        set.add(m_linkedResource);
        return;
    }

    if (m_clipperFilterMaskerData) {
        if (m_clipperFilterMaskerData->clipper)
            set.add(m_clipperFilterMaskerData->clipper);
        if (m_clipperFilterMaskerData->filter)
            set.add(m_clipperFilterMaskerData->filter);
        if (m_clipperFilterMaskerData->masker)
            set.add(m_clipperFilterMaskerData->masker);
    }

    if (m_markerData) {
        if (m_markerData->markerStart)
            set.add(m_markerData->markerStart);
        if (m_markerData->markerMid)
            set.add(m_markerData->markerMid);
        if (m_markerData->markerEnd)
            set.add(m_markerData->markerEnd);
    }

    if (m_fillStrokeData) {
        if (m_fillStrokeData->fill)
            set.add(m_fillStrokeData->fill);
        if (m_fillStrokeData->stroke)
            set.add(m_fillStrokeData->stroke);
    }
}

bool SVGResources::setClipper(LayoutSVGResourceClipper* clipper)
{
    if (!clipper)
        return false;

    ASSERT(clipper->resourceType() == ClipperResourceType);

    if (!m_clipperFilterMaskerData)
        m_clipperFilterMaskerData = ClipperFilterMaskerData::create();

    m_clipperFilterMaskerData->clipper = clipper;
    return true;
}

void SVGResources::resetClipper()
{
    ASSERT(m_clipperFilterMaskerData);
    ASSERT(m_clipperFilterMaskerData->clipper);
    m_clipperFilterMaskerData->clipper = nullptr;
}

bool SVGResources::setFilter(LayoutSVGResourceFilter* filter)
{
    if (!filter)
        return false;

    ASSERT(filter->resourceType() == FilterResourceType);

    if (!m_clipperFilterMaskerData)
        m_clipperFilterMaskerData = ClipperFilterMaskerData::create();

    m_clipperFilterMaskerData->filter = filter;
    return true;
}

void SVGResources::resetFilter()
{
    ASSERT(m_clipperFilterMaskerData);
    ASSERT(m_clipperFilterMaskerData->filter);
    m_clipperFilterMaskerData->filter = nullptr;
}

bool SVGResources::setMarkerStart(LayoutSVGResourceMarker* markerStart)
{
    if (!markerStart)
        return false;

    ASSERT(markerStart->resourceType() == MarkerResourceType);

    if (!m_markerData)
        m_markerData = MarkerData::create();

    m_markerData->markerStart = markerStart;
    return true;
}

void SVGResources::resetMarkerStart()
{
    ASSERT(m_markerData);
    ASSERT(m_markerData->markerStart);
    m_markerData->markerStart = nullptr;
}

bool SVGResources::setMarkerMid(LayoutSVGResourceMarker* markerMid)
{
    if (!markerMid)
        return false;

    ASSERT(markerMid->resourceType() == MarkerResourceType);

    if (!m_markerData)
        m_markerData = MarkerData::create();

    m_markerData->markerMid = markerMid;
    return true;
}

void SVGResources::resetMarkerMid()
{
    ASSERT(m_markerData);
    ASSERT(m_markerData->markerMid);
    m_markerData->markerMid = nullptr;
}

bool SVGResources::setMarkerEnd(LayoutSVGResourceMarker* markerEnd)
{
    if (!markerEnd)
        return false;

    ASSERT(markerEnd->resourceType() == MarkerResourceType);

    if (!m_markerData)
        m_markerData = MarkerData::create();

    m_markerData->markerEnd = markerEnd;
    return true;
}

void SVGResources::resetMarkerEnd()
{
    ASSERT(m_markerData);
    ASSERT(m_markerData->markerEnd);
    m_markerData->markerEnd = nullptr;
}

bool SVGResources::setMasker(LayoutSVGResourceMasker* masker)
{
    if (!masker)
        return false;

    ASSERT(masker->resourceType() == MaskerResourceType);

    if (!m_clipperFilterMaskerData)
        m_clipperFilterMaskerData = ClipperFilterMaskerData::create();

    m_clipperFilterMaskerData->masker = masker;
    return true;
}

void SVGResources::resetMasker()
{
    ASSERT(m_clipperFilterMaskerData);
    ASSERT(m_clipperFilterMaskerData->masker);
    m_clipperFilterMaskerData->masker = nullptr;
}

bool SVGResources::setFill(LayoutSVGResourcePaintServer* fill)
{
    if (!fill)
        return false;

    if (!m_fillStrokeData)
        m_fillStrokeData = FillStrokeData::create();

    m_fillStrokeData->fill = fill;
    return true;
}

void SVGResources::resetFill()
{
    ASSERT(m_fillStrokeData);
    ASSERT(m_fillStrokeData->fill);
    m_fillStrokeData->fill = nullptr;
}

bool SVGResources::setStroke(LayoutSVGResourcePaintServer* stroke)
{
    if (!stroke)
        return false;

    if (!m_fillStrokeData)
        m_fillStrokeData = FillStrokeData::create();

    m_fillStrokeData->stroke = stroke;
    return true;
}

void SVGResources::resetStroke()
{
    ASSERT(m_fillStrokeData);
    ASSERT(m_fillStrokeData->stroke);
    m_fillStrokeData->stroke = nullptr;
}

bool SVGResources::setLinkedResource(LayoutSVGResourceContainer* linkedResource)
{
    if (!linkedResource)
        return false;

    m_linkedResource = linkedResource;
    return true;
}

void SVGResources::resetLinkedResource()
{
    ASSERT(m_linkedResource);
    m_linkedResource = nullptr;
}

#ifndef NDEBUG
void SVGResources::dump(const LayoutObject* object)
{
    ASSERT(object);
    ASSERT(object->node());

    fprintf(stderr, "-> this=%p, SVGResources(layoutObject=%p, node=%p)\n", this, object, object->node());
    fprintf(stderr, " | DOM Tree:\n");
    object->node()->showTreeForThis();

    fprintf(stderr, "\n | List of resources:\n");
    if (m_clipperFilterMaskerData) {
        if (LayoutSVGResourceClipper* clipper = m_clipperFilterMaskerData->clipper)
            fprintf(stderr, " |-> Clipper    : %p (node=%p)\n", clipper, clipper->element());
        if (LayoutSVGResourceFilter* filter = m_clipperFilterMaskerData->filter)
            fprintf(stderr, " |-> Filter     : %p (node=%p)\n", filter, filter->element());
        if (LayoutSVGResourceMasker* masker = m_clipperFilterMaskerData->masker)
            fprintf(stderr, " |-> Masker     : %p (node=%p)\n", masker, masker->element());
    }

    if (m_markerData) {
        if (LayoutSVGResourceMarker* markerStart = m_markerData->markerStart)
            fprintf(stderr, " |-> MarkerStart: %p (node=%p)\n", markerStart, markerStart->element());
        if (LayoutSVGResourceMarker* markerMid = m_markerData->markerMid)
            fprintf(stderr, " |-> MarkerMid  : %p (node=%p)\n", markerMid, markerMid->element());
        if (LayoutSVGResourceMarker* markerEnd = m_markerData->markerEnd)
            fprintf(stderr, " |-> MarkerEnd  : %p (node=%p)\n", markerEnd, markerEnd->element());
    }

    if (m_fillStrokeData) {
        if (LayoutSVGResourcePaintServer* fill = m_fillStrokeData->fill)
            fprintf(stderr, " |-> Fill       : %p (node=%p)\n", fill, fill->element());
        if (LayoutSVGResourcePaintServer* stroke = m_fillStrokeData->stroke)
            fprintf(stderr, " |-> Stroke     : %p (node=%p)\n", stroke, stroke->element());
    }

    if (m_linkedResource)
        fprintf(stderr, " |-> xlink:href : %p (node=%p)\n", m_linkedResource, m_linkedResource->element());
}
#endif

}

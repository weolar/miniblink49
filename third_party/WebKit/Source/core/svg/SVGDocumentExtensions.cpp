/*
 * Copyright (C) 2006 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2007 Rob Buis <buis@kde.org>
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
#include "core/svg/SVGDocumentExtensions.h"

#include "core/dom/Document.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/layout/svg/SVGResourcesCache.h"
#include "core/svg/SVGSVGElement.h"
#include "core/svg/animation/SMILTimeContainer.h"
#include "wtf/TemporaryChange.h"
#include "wtf/text/AtomicString.h"

namespace blink {

SVGDocumentExtensions::SVGDocumentExtensions(Document* document)
    : m_document(document)
    , m_resourcesCache(adoptPtr(new SVGResourcesCache))
#if ENABLE(ASSERT)
    , m_inRelativeLengthSVGRootsInvalidation(false)
#endif
{
}

SVGDocumentExtensions::~SVGDocumentExtensions()
{
}

void SVGDocumentExtensions::addTimeContainer(SVGSVGElement* element)
{
    m_timeContainers.add(element);
}

void SVGDocumentExtensions::removeTimeContainer(SVGSVGElement* element)
{
    m_timeContainers.remove(element);
}

void SVGDocumentExtensions::addResource(const AtomicString& id, LayoutSVGResourceContainer* resource)
{
    ASSERT(resource);

    if (id.isEmpty())
        return;

    // Replaces resource if already present, to handle potential id changes
    m_resources.set(id, resource);
}

void SVGDocumentExtensions::removeResource(const AtomicString& id)
{
    if (id.isEmpty())
        return;

    m_resources.remove(id);
}

LayoutSVGResourceContainer* SVGDocumentExtensions::resourceById(const AtomicString& id) const
{
    if (id.isEmpty())
        return nullptr;

    return m_resources.get(id);
}

void SVGDocumentExtensions::serviceOnAnimationFrame(Document& document, double monotonicAnimationStartTime)
{
    if (!document.svgExtensions())
        return;
    document.accessSVGExtensions().serviceAnimations(monotonicAnimationStartTime);
}

void SVGDocumentExtensions::serviceAnimations(double monotonicAnimationStartTime)
{
    WillBeHeapVector<RefPtrWillBeMember<SVGSVGElement>> timeContainers;
    copyToVector(m_timeContainers, timeContainers);
    for (const auto& container : timeContainers)
        container->timeContainer()->serviceAnimations(monotonicAnimationStartTime);
}

void SVGDocumentExtensions::startAnimations()
{
    // FIXME: Eventually every "Time Container" will need a way to latch on to some global timer
    // starting animations for a document will do this "latching"
    // FIXME: We hold a ref pointers to prevent a shadow tree from getting removed out from underneath us.
    // In the future we should refactor the use-element to avoid this. See https://webkit.org/b/53704
    WillBeHeapVector<RefPtrWillBeMember<SVGSVGElement>> timeContainers;
    copyToVector(m_timeContainers, timeContainers);
    for (const auto& container : timeContainers) {
        SMILTimeContainer* timeContainer = container->timeContainer();
        if (!timeContainer->isStarted())
            timeContainer->begin();
    }
}

void SVGDocumentExtensions::pauseAnimations()
{
    for (SVGSVGElement* element : m_timeContainers)
        element->pauseAnimations();
}

void SVGDocumentExtensions::dispatchSVGLoadEventToOutermostSVGElements()
{
    WillBeHeapVector<RefPtrWillBeMember<SVGSVGElement>> timeContainers;
    copyToVector(m_timeContainers, timeContainers);
    for (const auto& container : timeContainers) {
        SVGSVGElement* outerSVG = container.get();
        if (!outerSVG->isOutermostSVGSVGElement())
            continue;

        // don't dispatch the load event document is not wellformed (for XML/standalone svg)
        if (outerSVG->document().wellFormed() || !outerSVG->document().isSVGDocument())
            outerSVG->sendSVGLoadEventIfPossible();
    }
}

static void reportMessage(Document* document, MessageLevel level, const String& message)
{
    if (document->frame())
        document->addConsoleMessage(ConsoleMessage::create(RenderingMessageSource, level, message));
}

void SVGDocumentExtensions::reportWarning(const String& message)
{
    reportMessage(m_document, WarningMessageLevel, "Warning: " + message);
}

void SVGDocumentExtensions::reportError(const String& message)
{
    reportMessage(m_document, ErrorMessageLevel, "Error: " + message);
}

void SVGDocumentExtensions::addPendingResource(const AtomicString& id, Element* element)
{
    ASSERT(element);
    ASSERT(element->inDocument());

    if (id.isEmpty())
        return;

    WillBeHeapHashMap<AtomicString, OwnPtrWillBeMember<SVGPendingElements>>::AddResult result = m_pendingResources.add(id, nullptr);
    if (result.isNewEntry)
        result.storedValue->value = adoptPtrWillBeNoop(new SVGPendingElements);
    result.storedValue->value->add(element);

    element->setHasPendingResources();
}

bool SVGDocumentExtensions::hasPendingResource(const AtomicString& id) const
{
    if (id.isEmpty())
        return false;

    return m_pendingResources.contains(id);
}

bool SVGDocumentExtensions::isElementPendingResources(Element* element) const
{
    // This algorithm takes time proportional to the number of pending resources and need not.
    // If performance becomes an issue we can keep a counted set of elements and answer the question efficiently.

    ASSERT(element);

    for (const auto& entry : m_pendingResources) {
        SVGPendingElements* elements = entry.value.get();
        ASSERT(elements);

        if (elements->contains(element))
            return true;
    }
    return false;
}

bool SVGDocumentExtensions::isElementPendingResource(Element* element, const AtomicString& id) const
{
    ASSERT(element);

    if (!hasPendingResource(id))
        return false;

    return m_pendingResources.get(id)->contains(element);
}

void SVGDocumentExtensions::clearHasPendingResourcesIfPossible(Element* element)
{
    if (!isElementPendingResources(element))
        element->clearHasPendingResources();
}

void SVGDocumentExtensions::removeElementFromPendingResources(Element* element)
{
    ASSERT(element);

    // Remove the element from pending resources.
    if (!m_pendingResources.isEmpty() && element->hasPendingResources()) {
        Vector<AtomicString> toBeRemoved;
        for (const auto& entry : m_pendingResources) {
            SVGPendingElements* elements = entry.value.get();
            ASSERT(elements);
            ASSERT(!elements->isEmpty());

            elements->remove(element);
            if (elements->isEmpty())
                toBeRemoved.append(entry.key);
        }

        clearHasPendingResourcesIfPossible(element);

        // We use the removePendingResource function here because it deals with set lifetime correctly.
        for (const AtomicString& id : toBeRemoved)
            removePendingResource(id);
    }

    // Remove the element from pending resources that were scheduled for removal.
    if (!m_pendingResourcesForRemoval.isEmpty()) {
        Vector<AtomicString> toBeRemoved;
        for (const auto& entry : m_pendingResourcesForRemoval) {
            SVGPendingElements* elements = entry.value.get();
            ASSERT(elements);
            ASSERT(!elements->isEmpty());

            elements->remove(element);
            if (elements->isEmpty())
                toBeRemoved.append(entry.key);
        }

        // We use the removePendingResourceForRemoval function here because it deals with set lifetime correctly.
        for (const AtomicString& id : toBeRemoved)
            removePendingResourceForRemoval(id);
    }
}

PassOwnPtrWillBeRawPtr<SVGDocumentExtensions::SVGPendingElements> SVGDocumentExtensions::removePendingResource(const AtomicString& id)
{
    ASSERT(m_pendingResources.contains(id));
    return m_pendingResources.take(id);
}

PassOwnPtrWillBeRawPtr<SVGDocumentExtensions::SVGPendingElements> SVGDocumentExtensions::removePendingResourceForRemoval(const AtomicString& id)
{
    ASSERT(m_pendingResourcesForRemoval.contains(id));
    return m_pendingResourcesForRemoval.take(id);
}

void SVGDocumentExtensions::markPendingResourcesForRemoval(const AtomicString& id)
{
    if (id.isEmpty())
        return;

    ASSERT(!m_pendingResourcesForRemoval.contains(id));

    OwnPtrWillBeMember<SVGPendingElements> existing = m_pendingResources.take(id);
    if (existing && !existing->isEmpty())
        m_pendingResourcesForRemoval.add(id, existing.release());
}

Element* SVGDocumentExtensions::removeElementFromPendingResourcesForRemoval(const AtomicString& id)
{
    if (id.isEmpty())
        return nullptr;

    SVGPendingElements* resourceSet = m_pendingResourcesForRemoval.get(id);
    if (!resourceSet || resourceSet->isEmpty())
        return nullptr;

    SVGPendingElements::iterator firstElement = resourceSet->begin();
    Element* element = *firstElement;

    resourceSet->remove(firstElement);

    if (resourceSet->isEmpty())
        removePendingResourceForRemoval(id);

    return element;
}

void SVGDocumentExtensions::addSVGRootWithRelativeLengthDescendents(SVGSVGElement* svgRoot)
{
    ASSERT(!m_inRelativeLengthSVGRootsInvalidation);
    m_relativeLengthSVGRoots.add(svgRoot);
}

void SVGDocumentExtensions::removeSVGRootWithRelativeLengthDescendents(SVGSVGElement* svgRoot)
{
    ASSERT(!m_inRelativeLengthSVGRootsInvalidation);
    m_relativeLengthSVGRoots.remove(svgRoot);
}

bool SVGDocumentExtensions::isSVGRootWithRelativeLengthDescendents(SVGSVGElement* svgRoot) const
{
    return m_relativeLengthSVGRoots.contains(svgRoot);
}

void SVGDocumentExtensions::invalidateSVGRootsWithRelativeLengthDescendents(SubtreeLayoutScope* scope)
{
    ASSERT(!m_inRelativeLengthSVGRootsInvalidation);
#if ENABLE(ASSERT)
    TemporaryChange<bool> inRelativeLengthSVGRootsChange(m_inRelativeLengthSVGRootsInvalidation, true);
#endif

    for (SVGSVGElement* element : m_relativeLengthSVGRoots)
        element->invalidateRelativeLengthClients(scope);
}

bool SVGDocumentExtensions::zoomAndPanEnabled() const
{
    if (SVGSVGElement* svg = rootElement(*m_document))
        return svg->zoomAndPanEnabled();
    return false;
}

void SVGDocumentExtensions::startPan(const FloatPoint& start)
{
    if (SVGSVGElement* svg = rootElement(*m_document))
        m_translate = FloatPoint(start.x() - svg->currentTranslate().x(), start.y() - svg->currentTranslate().y());
}

void SVGDocumentExtensions::updatePan(const FloatPoint& pos) const
{
    if (SVGSVGElement* svg = rootElement(*m_document))
        svg->setCurrentTranslate(FloatPoint(pos.x() - m_translate.x(), pos.y() - m_translate.y()));
}

SVGSVGElement* SVGDocumentExtensions::rootElement(const Document& document)
{
    Element* elem = document.documentElement();
    return isSVGSVGElement(elem) ? toSVGSVGElement(elem) : 0;
}

SVGSVGElement* SVGDocumentExtensions::rootElement() const
{
    ASSERT(m_document);
    return rootElement(*m_document);
}

DEFINE_TRACE(SVGDocumentExtensions)
{
#if ENABLE(OILPAN)
    visitor->trace(m_document);
    visitor->trace(m_timeContainers);
    visitor->trace(m_relativeLengthSVGRoots);
    visitor->trace(m_pendingResources);
    visitor->trace(m_pendingResourcesForRemoval);
#endif
}

}

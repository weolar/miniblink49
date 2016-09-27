/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) Research In Motion Limited 2009-2010. All rights reserved.
 * Copyright (C) 2011 Torch Mobile (Beijing) Co. Ltd. All rights reserved.
 * Copyright (C) 2012 University of Szeged
 * Copyright (C) 2012 Renata Hodovan <reni@webkit.org>
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

#include "core/svg/SVGUseElement.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/SVGNames.h"
#include "core/XLinkNames.h"
#include "core/dom/Document.h"
#include "core/dom/ElementTraversal.h"
#include "core/events/Event.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/fetch/FetchRequest.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/layout/svg/LayoutSVGTransformableContainer.h"
#include "core/svg/SVGGElement.h"
#include "core/svg/SVGLengthContext.h"
#include "core/svg/SVGSVGElement.h"
#include "core/xml/parser/XMLDocumentParser.h"

namespace blink {

static SVGUseEventSender& svgUseLoadEventSender()
{
    DEFINE_STATIC_LOCAL(SVGUseEventSender, sharedLoadEventSender, (EventTypeNames::load));
    return sharedLoadEventSender;
}

inline SVGUseElement::SVGUseElement(Document& document)
    : SVGGraphicsElement(SVGNames::useTag, document)
    , SVGURIReference(this)
    , m_x(SVGAnimatedLength::create(this, SVGNames::xAttr, SVGLength::create(SVGLengthMode::Width), AllowNegativeLengths))
    , m_y(SVGAnimatedLength::create(this, SVGNames::yAttr, SVGLength::create(SVGLengthMode::Height), AllowNegativeLengths))
    , m_width(SVGAnimatedLength::create(this, SVGNames::widthAttr, SVGLength::create(SVGLengthMode::Width), ForbidNegativeLengths))
    , m_height(SVGAnimatedLength::create(this, SVGNames::heightAttr, SVGLength::create(SVGLengthMode::Height), ForbidNegativeLengths))
    , m_haveFiredLoadEvent(false)
    , m_needsShadowTreeRecreation(false)
{
    ASSERT(hasCustomStyleCallbacks());

    addToPropertyMap(m_x);
    addToPropertyMap(m_y);
    addToPropertyMap(m_width);
    addToPropertyMap(m_height);
}

PassRefPtrWillBeRawPtr<SVGUseElement> SVGUseElement::create(Document& document)
{
    // Always build a user agent #shadow-root for SVGUseElement.
    RefPtrWillBeRawPtr<SVGUseElement> use = adoptRefWillBeNoop(new SVGUseElement(document));
    use->ensureUserAgentShadowRoot();
    return use.release();
}

SVGUseElement::~SVGUseElement()
{
    setDocumentResource(0);
#if !ENABLE(OILPAN)
    clearResourceReferences();
#endif
    svgUseLoadEventSender().cancelEvent(this);
}

DEFINE_TRACE(SVGUseElement)
{
    visitor->trace(m_x);
    visitor->trace(m_y);
    visitor->trace(m_width);
    visitor->trace(m_height);
    visitor->trace(m_targetElementInstance);
    SVGGraphicsElement::trace(visitor);
    SVGURIReference::trace(visitor);
}

#if ENABLE(ASSERT)
static inline bool isWellFormedDocument(Document* document)
{
    if (document->isXMLDocument())
        return static_cast<XMLDocumentParser*>(document->parser())->wellFormed();
    return true;
}
#endif

Node::InsertionNotificationRequest SVGUseElement::insertedInto(ContainerNode* rootParent)
{
    // This functions exists to assure assumptions made in the code regarding SVGElementInstance creation/destruction are satisfied.
    SVGGraphicsElement::insertedInto(rootParent);
    if (!rootParent->inDocument())
        return InsertionDone;
    ASSERT(!m_targetElementInstance || !isWellFormedDocument(&document()));
    ASSERT(!hasPendingResources() || !isWellFormedDocument(&document()));
    invalidateShadowTree();
    return InsertionDone;
}

void SVGUseElement::removedFrom(ContainerNode* rootParent)
{
    SVGGraphicsElement::removedFrom(rootParent);
    if (rootParent->inDocument())
        clearResourceReferences();
}

TreeScope* SVGUseElement::referencedScope() const
{
    if (!isExternalURIReference(hrefString(), document()))
        return &treeScope();
    return externalDocument();
}

Document* SVGUseElement::externalDocument() const
{
    if (m_resource && m_resource->isLoaded()) {
        // Gracefully handle error condition.
        if (m_resource->errorOccurred())
            return nullptr;
        ASSERT(m_resource->document());
        return m_resource->document();
    }
    return nullptr;
}

void transferUseWidthAndHeightIfNeeded(const SVGUseElement& use, SVGElement* shadowElement, const SVGElement& originalElement)
{
    DEFINE_STATIC_LOCAL(const AtomicString, hundredPercentString, ("100%", AtomicString::ConstructFromLiteral));
    ASSERT(shadowElement);
    if (isSVGSymbolElement(*shadowElement)) {
        // Spec (<use> on <symbol>): This generated 'svg' will always have explicit values for attributes width and height.
        // If attributes width and/or height are provided on the 'use' element, then these attributes
        // will be transferred to the generated 'svg'. If attributes width and/or height are not specified,
        // the generated 'svg' element will use values of 100% for these attributes.
        shadowElement->setAttribute(SVGNames::widthAttr, use.width()->isSpecified() ? AtomicString(use.width()->currentValue()->valueAsString()) : hundredPercentString);
        shadowElement->setAttribute(SVGNames::heightAttr, use.height()->isSpecified() ? AtomicString(use.height()->currentValue()->valueAsString()) : hundredPercentString);
    } else if (isSVGSVGElement(*shadowElement)) {
        // Spec (<use> on <svg>): If attributes width and/or height are provided on the 'use' element, then these
        // values will override the corresponding attributes on the 'svg' in the generated tree.
        if (use.width()->isSpecified())
            shadowElement->setAttribute(SVGNames::widthAttr, AtomicString(use.width()->currentValue()->valueAsString()));
        else
            shadowElement->setAttribute(SVGNames::widthAttr, originalElement.getAttribute(SVGNames::widthAttr));
        if (use.height()->isSpecified())
            shadowElement->setAttribute(SVGNames::heightAttr, AtomicString(use.height()->currentValue()->valueAsString()));
        else
            shadowElement->setAttribute(SVGNames::heightAttr, originalElement.getAttribute(SVGNames::heightAttr));
    }
}

bool SVGUseElement::isPresentationAttribute(const QualifiedName& attrName) const
{
    if (attrName == SVGNames::xAttr || attrName == SVGNames::yAttr)
        return true;
    return SVGGraphicsElement::isPresentationAttribute(attrName);
}

bool SVGUseElement::isPresentationAttributeWithSVGDOM(const QualifiedName& attrName) const
{
    if (attrName == SVGNames::xAttr || attrName == SVGNames::yAttr)
        return true;
    return SVGGraphicsElement::isPresentationAttributeWithSVGDOM(attrName);
}

void SVGUseElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    RefPtrWillBeRawPtr<SVGAnimatedPropertyBase> property = propertyFromAttribute(name);
    if (property == m_x)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyX, *m_x->currentValue());
    else if (property == m_y)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyY, *m_y->currentValue());
    else
        SVGGraphicsElement::collectStyleForPresentationAttribute(name, value, style);
}

void SVGUseElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::xAttr
        || attrName == SVGNames::yAttr
        || attrName == SVGNames::widthAttr
        || attrName == SVGNames::heightAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);

        if (attrName == SVGNames::xAttr
            || attrName == SVGNames::yAttr) {
            invalidateSVGPresentationAttributeStyle();
            setNeedsStyleRecalc(LocalStyleChange,
                StyleChangeReasonForTracing::fromAttribute(attrName));
        }

        updateRelativeLengthsInformation();
        if (m_targetElementInstance) {
            ASSERT(m_targetElementInstance->correspondingElement());
            transferUseWidthAndHeightIfNeeded(*this, m_targetElementInstance.get(), *m_targetElementInstance->correspondingElement());
        }

        LayoutObject* object = this->layoutObject();
        if (object)
            markForLayoutAndParentResourceInvalidation(object);
        return;
    }

    if (SVGURIReference::isKnownAttribute(attrName)) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        bool isExternalReference = isExternalURIReference(hrefString(), document());
        if (isExternalReference) {
            KURL url = document().completeURL(hrefString());
            if (url.hasFragmentIdentifier()) {
                FetchRequest request(ResourceRequest(url), localName());
                setDocumentResource(DocumentResource::fetchSVGDocument(request, document().fetcher()));
            }
        } else {
            setDocumentResource(0);
        }

        invalidateShadowTree();

        return;
    }

    SVGGraphicsElement::svgAttributeChanged(attrName);
}

static bool isDisallowedElement(Node* node)
{
    // Spec: "Any 'svg', 'symbol', 'g', graphics element or other 'use' is potentially a template object that can be re-used
    // (i.e., "instanced") in the SVG document via a 'use' element."
    // "Graphics Element" is defined as 'circle', 'ellipse', 'image', 'line', 'path', 'polygon', 'polyline', 'rect', 'text'
    // Excluded are anything that is used by reference or that only make sense to appear once in a document.
    // We must also allow the shadow roots of other use elements.
    if (node->isShadowRoot() || node->isTextNode())
        return false;

    if (!node->isSVGElement())
        return true;

    Element* element = toElement(node);

    DEFINE_STATIC_LOCAL(HashSet<QualifiedName>, allowedElementTags, ());
    if (allowedElementTags.isEmpty()) {
        allowedElementTags.add(SVGNames::aTag);
        allowedElementTags.add(SVGNames::circleTag);
        allowedElementTags.add(SVGNames::descTag);
        allowedElementTags.add(SVGNames::ellipseTag);
        allowedElementTags.add(SVGNames::gTag);
        allowedElementTags.add(SVGNames::imageTag);
        allowedElementTags.add(SVGNames::lineTag);
        allowedElementTags.add(SVGNames::metadataTag);
        allowedElementTags.add(SVGNames::pathTag);
        allowedElementTags.add(SVGNames::polygonTag);
        allowedElementTags.add(SVGNames::polylineTag);
        allowedElementTags.add(SVGNames::rectTag);
        allowedElementTags.add(SVGNames::svgTag);
        allowedElementTags.add(SVGNames::switchTag);
        allowedElementTags.add(SVGNames::symbolTag);
        allowedElementTags.add(SVGNames::textTag);
        allowedElementTags.add(SVGNames::textPathTag);
        allowedElementTags.add(SVGNames::titleTag);
        allowedElementTags.add(SVGNames::tspanTag);
        allowedElementTags.add(SVGNames::useTag);
    }
    return !allowedElementTags.contains<SVGAttributeHashTranslator>(element->tagQName());
}

static bool subtreeContainsDisallowedElement(Node* start)
{
    if (isDisallowedElement(start))
        return true;

    for (Node* cur = start->firstChild(); cur; cur = cur->nextSibling()) {
        if (subtreeContainsDisallowedElement(cur))
            return true;
    }

    return false;
}

void SVGUseElement::scheduleShadowTreeRecreation()
{
    if (!referencedScope() || inUseShadowTree())
        return;
    m_needsShadowTreeRecreation = true;
    document().scheduleUseShadowTreeUpdate(*this);
}

void SVGUseElement::clearResourceReferences()
{
    if (m_targetElementInstance)
        m_targetElementInstance = nullptr;

    // FIXME: We should try to optimize this, to at least allow partial reclones.
    if (ShadowRoot* shadowTreeRootElement = userAgentShadowRoot())
        shadowTreeRootElement->removeChildren(OmitSubtreeModifiedEvent);

    m_needsShadowTreeRecreation = false;
    document().unscheduleUseShadowTreeUpdate(*this);

    removeAllOutgoingReferences();
}

void SVGUseElement::buildPendingResource()
{
    if (!referencedScope() || inUseShadowTree())
        return;
    clearResourceReferences();
    if (!inDocument())
        return;

    AtomicString id;
    Element* target = SVGURIReference::targetElementFromIRIString(hrefString(), treeScope(), &id, externalDocument());
    if (!target || !target->inDocument()) {
        // If we can't find the target of an external element, just give up.
        // We can't observe if the target somewhen enters the external document, nor should we do it.
        if (externalDocument())
            return;
        if (id.isEmpty())
            return;

        referencedScope()->document().accessSVGExtensions().addPendingResource(id, this);
        ASSERT(hasPendingResources());
        return;
    }

    if (target->isSVGElement()) {
        buildShadowAndInstanceTree(toSVGElement(target));
        invalidateDependentShadowTrees();
    }

    ASSERT(!m_needsShadowTreeRecreation);
}

static PassRefPtrWillBeRawPtr<Node> cloneNodeAndAssociate(Node& toClone)
{
    RefPtrWillBeRawPtr<Node> clone = toClone.cloneNode(false);
    if (!clone->isSVGElement())
        return clone.release();

    SVGElement& svgElement = toSVGElement(toClone);
    ASSERT(!svgElement.correspondingElement());
    toSVGElement(clone.get())->setCorrespondingElement(&svgElement);
    if (EventTargetData* data = toClone.eventTargetData())
        data->eventListenerMap.copyEventListenersNotCreatedFromMarkupToTarget(clone.get());
    TrackExceptionState exceptionState;
    for (Node* node = toClone.firstChild(); node && !exceptionState.hadException(); node = node->nextSibling())
        clone->appendChild(cloneNodeAndAssociate(*node), exceptionState);
    return clone.release();
}

void SVGUseElement::buildShadowAndInstanceTree(SVGElement* target)
{
    ASSERT(!m_targetElementInstance);

    // <use> creates a "user agent" shadow root. Do not build the shadow/instance tree for <use>
    // elements living in a user agent shadow tree because they will get expanded in a second
    // pass -- see expandUseElementsInShadowTree().
    if (inUseShadowTree())
        return;

    // Do not allow self-referencing.
    // 'target' may be null, if it's a non SVG namespaced element.
    if (!target || target == this || isDisallowedElement(target))
        return;

    // Set up root SVG element in shadow tree.
    RefPtrWillBeRawPtr<Element> newChild = target->cloneElementWithoutChildren();
    m_targetElementInstance = toSVGElement(newChild.get());
    ShadowRoot* shadowTreeRootElement = userAgentShadowRoot();
    shadowTreeRootElement->appendChild(newChild.release());

    // Clone the target subtree into the shadow tree, not handling <use> and <symbol> yet.

    // SVG specification does not say a word about <use> & cycles. My view on this is: just ignore it!
    // Non-appearing <use> content is easier to debug, then half-appearing content.
    if (!buildShadowTree(target, m_targetElementInstance.get(), false)) {
        clearResourceReferences();
        return;
    }

    if (instanceTreeIsLoading(m_targetElementInstance.get()))
        return;

    // Assure shadow tree building was successfull
    ASSERT(m_targetElementInstance);
    ASSERT(m_targetElementInstance->correspondingUseElement() == this);
    ASSERT(m_targetElementInstance->correspondingElement() == target);

    // Expand all <use> elements in the shadow tree.
    // Expand means: replace the actual <use> element by what it references.
    if (!expandUseElementsInShadowTree(m_targetElementInstance.get())) {
        clearResourceReferences();
        return;
    }

    // Expand all <symbol> elements in the shadow tree.
    // Expand means: replace the actual <symbol> element by the <svg> element.
    expandSymbolElementsInShadowTree(toSVGElement(shadowTreeRootElement->firstChild()));

    m_targetElementInstance = toSVGElement(shadowTreeRootElement->firstChild());
    transferUseWidthAndHeightIfNeeded(*this, m_targetElementInstance.get(), *m_targetElementInstance->correspondingElement());

    ASSERT(m_targetElementInstance->parentNode() == shadowTreeRootElement);

    // Update relative length information.
    updateRelativeLengthsInformation();
}

LayoutObject* SVGUseElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutSVGTransformableContainer(this);
}

static bool isDirectReference(const SVGElement& element)
{
    return isSVGPathElement(element)
        || isSVGRectElement(element)
        || isSVGCircleElement(element)
        || isSVGEllipseElement(element)
        || isSVGPolygonElement(element)
        || isSVGPolylineElement(element)
        || isSVGTextElement(element);
}

void SVGUseElement::toClipPath(Path& path) const
{
    ASSERT(path.isEmpty());

    const SVGGraphicsElement* element = targetGraphicsElementForClipping();

    if (!element)
        return;

    if (element->isSVGGeometryElement()) {
        toSVGGeometryElement(*element).toClipPath(path);
        // FIXME: Avoid manual resolution of x/y here. Its potentially harmful.
        SVGLengthContext lengthContext(this);
        path.translate(FloatSize(m_x->currentValue()->value(lengthContext), m_y->currentValue()->value(lengthContext)));
        path.transform(calculateAnimatedLocalTransform());
    }
}

SVGGraphicsElement* SVGUseElement::targetGraphicsElementForClipping() const
{
    Node* n = userAgentShadowRoot()->firstChild();
    if (!n || !n->isSVGElement())
        return nullptr;

    SVGElement& element = toSVGElement(*n);

    if (!element.isSVGGraphicsElement())
        return nullptr;

    // Spec: "If a <use> element is a child of a clipPath element, it must directly
    // reference <path>, <text> or basic shapes elements. Indirect references are an
    // error and the clipPath element must be ignored."
    // http://dev.w3.org/fxtf/css-masking-1/#the-clip-path
    if (!isDirectReference(element)) {
        // Spec: Indirect references are an error (14.3.5)
        document().accessSVGExtensions().reportError("Not allowed to use indirect reference in <clip-path>");
        return nullptr;
    }

    return &toSVGGraphicsElement(element);
}

bool SVGUseElement::buildShadowTree(SVGElement* target, SVGElement* targetInstance, bool foundUse)
{
    ASSERT(target);
    ASSERT(targetInstance);

    // Spec: If the referenced object is itself a 'use', or if there are 'use' subelements within the referenced
    // object, the instance tree will contain recursive expansion of the indirect references to form a complete tree.
    if (isSVGUseElement(*target)) {
        // We only need to track first degree <use> dependencies. Indirect references are handled
        // as the invalidation bubbles up the dependency chain.
        if (!foundUse && !isStructurallyExternal()) {
            addReferenceTo(target);
            foundUse = true;
        }
    } else if (isDisallowedElement(target)) {
        return false;
    }

    targetInstance->setCorrespondingElement(target);
    if (EventTargetData* data = target->eventTargetData())
        data->eventListenerMap.copyEventListenersNotCreatedFromMarkupToTarget(targetInstance);

    for (Node* child = target->firstChild(); child; child = child->nextSibling()) {
        // Skip any disallowed element.
        if (isDisallowedElement(child))
            continue;

        RefPtrWillBeRawPtr<Node> newChild = child->cloneNode(false);
        targetInstance->appendChild(newChild.get());
        if (newChild->isSVGElement()) {
            // Enter recursion, appending new instance tree nodes to the "instance" object.
            if (!buildShadowTree(toSVGElement(child), toSVGElement(newChild), foundUse))
                return false;
        }
    }
    return true;
}

bool SVGUseElement::hasCycleUseReferencing(SVGUseElement* use, ContainerNode* targetInstance, SVGElement*& newTarget)
{
    ASSERT(referencedScope());
    Element* targetElement = SVGURIReference::targetElementFromIRIString(use->hrefString(), *referencedScope());
    newTarget = 0;
    if (targetElement && targetElement->isSVGElement())
        newTarget = toSVGElement(targetElement);

    if (!newTarget)
        return false;

    // Shortcut for self-references
    if (newTarget == this)
        return true;

    AtomicString targetId = newTarget->getIdAttribute();
    ContainerNode* instance = targetInstance->parentNode();
    while (instance && instance->isSVGElement()) {
        SVGElement* element = toSVGElement(instance);
        if (element->hasID() && element->getIdAttribute() == targetId && element->document() == newTarget->document())
            return true;

        instance = instance->parentNode();
    }
    return false;
}

static inline void removeDisallowedElementsFromSubtree(Element& subtree)
{
    ASSERT(!subtree.inDocument());
    Element* element = ElementTraversal::firstWithin(subtree);
    while (element) {
        if (isDisallowedElement(element)) {
            Element* next = ElementTraversal::nextSkippingChildren(*element, &subtree);
            // The subtree is not in document so this won't generate events that could mutate the tree.
            element->parentNode()->removeChild(element);
            element = next;
        } else {
            element = ElementTraversal::next(*element, &subtree);
        }
    }
}

bool SVGUseElement::expandUseElementsInShadowTree(SVGElement* element)
{
    ASSERT(element);
    // Why expand the <use> elements in the shadow tree here, and not just
    // do this directly in buildShadowTree, if we encounter a <use> element?
    //
    // Short answer: Because we may miss to expand some elements. For example, if a <symbol>
    // contains <use> tags, we'd miss them. So once we're done with setting up the
    // actual shadow tree (after the special case modification for svg/symbol) we have
    // to walk it completely and expand all <use> elements.
    if (isSVGUseElement(*element)) {
        SVGUseElement* use = toSVGUseElement(element);
        ASSERT(!use->resourceIsStillLoading());

        SVGElement* target = 0;
        if (hasCycleUseReferencing(toSVGUseElement(use->correspondingElement()), use, target))
            return false;

        if (target && isDisallowedElement(target))
            return false;
        // Don't ASSERT(target) here, it may be "pending", too.
        // Setup sub-shadow tree root node
        RefPtrWillBeRawPtr<SVGGElement> cloneParent = SVGGElement::create(referencedScope()->document());
        cloneParent->setCorrespondingElement(use->correspondingElement());

        // Move already cloned elements to the new <g> element
        for (Node* child = use->firstChild(); child; ) {
            Node* nextChild = child->nextSibling();
            cloneParent->appendChild(child);
            child = nextChild;
        }

        // Spec: In the generated content, the 'use' will be replaced by 'g', where all attributes from the
        // 'use' element except for x, y, width, height and xlink:href are transferred to the generated 'g' element.
        transferUseAttributesToReplacedElement(use, cloneParent.get());

        if (target) {
            RefPtrWillBeRawPtr<Node> newChild = cloneNodeAndAssociate(*target);
            ASSERT(newChild->isSVGElement());
            transferUseWidthAndHeightIfNeeded(*use, toSVGElement(newChild.get()), *target);
            cloneParent->appendChild(newChild.release());
        }

        // We don't walk the target tree element-by-element, and clone each element,
        // but instead use cloneElementWithChildren(). This is an optimization for the common
        // case where <use> doesn't contain disallowed elements (ie. <foreignObject>).
        // Though if there are disallowed elements in the subtree, we have to remove them.
        // For instance: <use> on <g> containing <foreignObject> (indirect case).
        if (subtreeContainsDisallowedElement(cloneParent.get()))
            removeDisallowedElementsFromSubtree(*cloneParent);

        RefPtrWillBeRawPtr<SVGElement> replacingElement(cloneParent.get());

        // Replace <use> with referenced content.
        ASSERT(use->parentNode());
        use->parentNode()->replaceChild(cloneParent.release(), use);

        // Expand the siblings because the *element* is replaced and we will
        // lose the sibling chain when we are back from recursion.
        element = replacingElement.get();
        for (RefPtrWillBeRawPtr<SVGElement> sibling = Traversal<SVGElement>::nextSibling(*element); sibling; sibling = Traversal<SVGElement>::nextSibling(*sibling)) {
            if (!expandUseElementsInShadowTree(sibling.get()))
                return false;
        }
    }

    for (RefPtrWillBeRawPtr<SVGElement> child = Traversal<SVGElement>::firstChild(*element); child; child = Traversal<SVGElement>::nextSibling(*child)) {
        if (!expandUseElementsInShadowTree(child.get()))
            return false;
    }
    return true;
}

void SVGUseElement::expandSymbolElementsInShadowTree(SVGElement* element)
{
    ASSERT(element);
    if (isSVGSymbolElement(*element)) {
        // Spec: The referenced 'symbol' and its contents are deep-cloned into the generated tree,
        // with the exception that the 'symbol' is replaced by an 'svg'. This generated 'svg' will
        // always have explicit values for attributes width and height. If attributes width and/or
        // height are provided on the 'use' element, then these attributes will be transferred to
        // the generated 'svg'. If attributes width and/or height are not specified, the generated
        // 'svg' element will use values of 100% for these attributes.
        ASSERT(referencedScope());
        RefPtrWillBeRawPtr<SVGSVGElement> svgElement = SVGSVGElement::create(referencedScope()->document());
        // Transfer all data (attributes, etc.) from <symbol> to the new <svg> element.
        svgElement->cloneDataFromElement(*element);
        svgElement->setCorrespondingElement(element->correspondingElement());

        // Move already cloned elements to the new <svg> element
        for (Node* child = element->firstChild(); child; ) {
            Node* nextChild = child->nextSibling();
            svgElement->appendChild(child);
            child = nextChild;
        }

        // We don't walk the target tree element-by-element, and clone each element,
        // but instead use cloneNode(deep=true). This is an optimization for the common
        // case where <use> doesn't contain disallowed elements (ie. <foreignObject>).
        // Though if there are disallowed elements in the subtree, we have to remove them.
        // For instance: <use> on <g> containing <foreignObject> (indirect case).
        if (subtreeContainsDisallowedElement(svgElement.get()))
            removeDisallowedElementsFromSubtree(*svgElement);

        RefPtrWillBeRawPtr<SVGElement> replacingElement(svgElement.get());

        // Replace <symbol> with <svg>.
        ASSERT(element->parentNode());
        element->parentNode()->replaceChild(svgElement.release(), element);

        // Expand the siblings because the *element* is replaced and we will
        // lose the sibling chain when we are back from recursion.
        element = replacingElement.get();
    }

    for (RefPtrWillBeRawPtr<SVGElement> child = Traversal<SVGElement>::firstChild(*element); child; child = Traversal<SVGElement>::nextSibling(*child))
        expandSymbolElementsInShadowTree(child.get());
}

void SVGUseElement::invalidateShadowTree()
{
    if (!inActiveDocument() || m_needsShadowTreeRecreation)
        return;
    scheduleShadowTreeRecreation();
    invalidateDependentShadowTrees();
}

void SVGUseElement::invalidateDependentShadowTrees()
{
    // Recursively invalidate dependent <use> shadow trees
    const WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>>& instances = instancesForElement();
    for (SVGElement* instance : instances) {
        if (SVGUseElement* element = instance->correspondingUseElement()) {
            ASSERT(element->inDocument());
            element->invalidateShadowTree();
        }
    }
}

void SVGUseElement::transferUseAttributesToReplacedElement(SVGElement* from, SVGElement* to) const
{
    ASSERT(from);
    ASSERT(to);

    to->cloneDataFromElement(*from);

    to->removeAttribute(SVGNames::xAttr);
    to->removeAttribute(SVGNames::yAttr);
    to->removeAttribute(SVGNames::widthAttr);
    to->removeAttribute(SVGNames::heightAttr);
    to->removeAttribute(XLinkNames::hrefAttr);
}

bool SVGUseElement::selfHasRelativeLengths() const
{
    if (m_x->currentValue()->isRelative()
        || m_y->currentValue()->isRelative()
        || m_width->currentValue()->isRelative()
        || m_height->currentValue()->isRelative())
        return true;

    if (!m_targetElementInstance)
        return false;

    return m_targetElementInstance->hasRelativeLengths();
}

void SVGUseElement::dispatchPendingEvent(SVGUseEventSender* eventSender)
{
    ASSERT_UNUSED(eventSender, eventSender == &svgUseLoadEventSender());
    ASSERT(isStructurallyExternal() && m_haveFiredLoadEvent);
    dispatchEvent(Event::create(EventTypeNames::load));
}

void SVGUseElement::notifyFinished(Resource* resource)
{
    if (!inDocument())
        return;

    invalidateShadowTree();
    if (resource->errorOccurred())
        dispatchEvent(Event::create(EventTypeNames::error));
    else if (!resource->wasCanceled()) {
        if (m_haveFiredLoadEvent)
            return;
        if (!isStructurallyExternal())
            return;
        ASSERT(!m_haveFiredLoadEvent);
        m_haveFiredLoadEvent = true;
        svgUseLoadEventSender().dispatchEventSoon(this);
    }
}

bool SVGUseElement::resourceIsStillLoading()
{
    if (m_resource && m_resource->isLoading())
        return true;
    return false;
}

bool SVGUseElement::instanceTreeIsLoading(SVGElement* targetInstance)
{
    for (SVGElement* element = Traversal<SVGElement>::firstChild(*targetInstance); element; element = Traversal<SVGElement>::nextSibling(*element)) {
        if (SVGUseElement* use = element->correspondingUseElement()) {
            if (use->resourceIsStillLoading())
                return true;
        }
        if (element->hasChildren() && instanceTreeIsLoading(element))
            return true;
    }
    return false;
}

void SVGUseElement::setDocumentResource(ResourcePtr<DocumentResource> resource)
{
    if (m_resource == resource)
        return;

    if (m_resource)
        m_resource->removeClient(this);

    m_resource = resource;
    if (m_resource)
        m_resource->addClient(this);
}

}

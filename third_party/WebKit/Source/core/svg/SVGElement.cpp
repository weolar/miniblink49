/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2008 Rob Buis <buis@kde.org>
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Alp Toker <alp@atoker.com>
 * Copyright (C) 2009 Cameron McCormack <cam@mcc.id.au>
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

#include "core/svg/SVGElement.h"

#include "bindings/core/v8/ScriptEventListener.h"
#include "core/HTMLNames.h"
#include "core/SVGNames.h"
#include "core/XLinkNames.h"
#include "core/XMLNames.h"
#include "core/css/CSSCursorImageValue.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/dom/Document.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/events/Event.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLElement.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/svg/LayoutSVGResourceContainer.h"
#include "core/svg/SVGCursorElement.h"
#include "core/svg/SVGDocumentExtensions.h"
#include "core/svg/SVGElementRareData.h"
#include "core/svg/SVGGraphicsElement.h"
#include "core/svg/SVGSVGElement.h"
#include "core/svg/SVGTitleElement.h"
#include "core/svg/SVGUseElement.h"
#include "platform/JSONValues.h"
#include "wtf/TemporaryChange.h"

namespace blink {

using namespace HTMLNames;
using namespace SVGNames;

SVGElement::SVGElement(const QualifiedName& tagName, Document& document, ConstructionType constructionType)
    : Element(tagName, &document, constructionType)
#if ENABLE(ASSERT)
    , m_inRelativeLengthClientsInvalidation(false)
#endif
    , m_SVGRareData(nullptr)
    , m_className(SVGAnimatedString::create(this, HTMLNames::classAttr, SVGString::create()))
{
    addToPropertyMap(m_className);
    setHasCustomStyleCallbacks();
}

SVGElement::~SVGElement()
{
    ASSERT(inDocument() || !hasRelativeLengths());

    // The below teardown is all handled by weak pointer processing in oilpan.
#if !ENABLE(OILPAN)
    if (hasSVGRareData()) {
        if (SVGCursorElement* cursorElement = svgRareData()->cursorElement())
            cursorElement->removeReferencedElement(this);
        if (CSSCursorImageValue* cursorImageValue = svgRareData()->cursorImageValue())
            cursorImageValue->removeReferencedElement(this);

        // With Oilpan, either removedFrom has been called or the document is dead
        // as well and there is no reason to clear out the references.
        rebuildAllIncomingReferences();
        removeAllIncomingReferences();
    }
#endif
}

void SVGElement::detach(const AttachContext& context)
{
    Element::detach(context);
    if (SVGElement* element = correspondingElement())
        element->removeInstanceMapping(this);
}

void SVGElement::attach(const AttachContext& context)
{
    Element::attach(context);
    if (SVGElement* element = correspondingElement())
        element->mapInstanceToElement(this);
}

short SVGElement::tabIndex() const
{
    if (supportsFocus())
        return Element::tabIndex();
    return -1;
}

void SVGElement::willRecalcStyle(StyleRecalcChange change)
{
    if (!hasSVGRareData())
        return;
    // If the style changes because of a regular property change (not induced by SMIL animations themselves)
    // reset the "computed style without SMIL style properties", so the base value change gets reflected.
    if (change > NoChange || needsStyleRecalc())
        svgRareData()->setNeedsOverrideComputedStyleUpdate();
}

void SVGElement::buildPendingResourcesIfNeeded()
{
    Document& document = this->document();
    if (!needsPendingResourceHandling() || !inDocument() || inUseShadowTree())
        return;

    SVGDocumentExtensions& extensions = document.accessSVGExtensions();
    AtomicString resourceId = getIdAttribute();
    if (!extensions.hasPendingResource(resourceId))
        return;

    // Mark pending resources as pending for removal.
    extensions.markPendingResourcesForRemoval(resourceId);

    // Rebuild pending resources for each client of a pending resource that is being removed.
    while (Element* clientElement = extensions.removeElementFromPendingResourcesForRemoval(resourceId)) {
        ASSERT(clientElement->hasPendingResources());
        if (clientElement->hasPendingResources()) {
            // FIXME: Ideally we'd always resolve pending resources async instead of inside
            // insertedInto and svgAttributeChanged. For now we only do it for <use> since
            // that would stamp out DOM.
            if (isSVGUseElement(clientElement))
                toSVGUseElement(clientElement)->invalidateShadowTree();
            else
                clientElement->buildPendingResource();
            extensions.clearHasPendingResourcesIfPossible(clientElement);
        }
    }
}

SVGElementRareData* SVGElement::ensureSVGRareData()
{
    if (hasSVGRareData())
        return svgRareData();

    m_SVGRareData = adoptPtrWillBeNoop(new SVGElementRareData(this));
    return m_SVGRareData.get();
}

bool SVGElement::isOutermostSVGSVGElement() const
{
    if (!isSVGSVGElement(*this))
        return false;

    // Element may not be in the document, pretend we're outermost for viewport(), getCTM(), etc.
    if (!parentNode())
        return true;

    // We act like an outermost SVG element, if we're a direct child of a <foreignObject> element.
    if (isSVGForeignObjectElement(*parentNode()))
        return true;

    // If we're living in a shadow tree, we're a <svg> element that got created as replacement
    // for a <symbol> element or a cloned <svg> element in the referenced tree. In that case
    // we're always an inner <svg> element.
    if (inUseShadowTree() && parentOrShadowHostElement() && parentOrShadowHostElement()->isSVGElement())
        return false;

    // This is true whenever this is the outermost SVG, even if there are HTML elements outside it
    return !parentNode()->isSVGElement();
}

void SVGElement::reportAttributeParsingError(SVGParsingError error, const QualifiedName& name, const AtomicString& value)
{
    if (error == NoError)
        return;

    String errorString = "<" + tagName() + "> attribute " + name.toString() + "=" + JSONValue::quoteString(value);
    SVGDocumentExtensions& extensions = document().accessSVGExtensions();

    if (error == NegativeValueForbiddenError) {
        extensions.reportError("Invalid negative value for " + errorString);
        return;
    }

    if (error == ParsingAttributeFailedError) {
        extensions.reportError("Invalid value for " + errorString);
        return;
    }

    ASSERT_NOT_REACHED();
}

String SVGElement::title() const
{
    // According to spec, we should not return titles when hovering over root <svg> elements (those
    // <title> elements are the title of the document, not a tooltip) so we instantly return.
    if (isOutermostSVGSVGElement())
        return String();

    if (inUseShadowTree()) {
        String useTitle(shadowHost()->title());
        if (!useTitle.isEmpty())
            return useTitle;
    }

    // If we aren't an instance in a <use> or the <use> title was not found, then find the first
    // <title> child of this element.
    // If a title child was found, return the text contents.
    if (Element* titleElement = Traversal<SVGTitleElement>::firstChild(*this))
        return titleElement->innerText();

    // Otherwise return a null/empty string.
    return String();
}

bool SVGElement::instanceUpdatesBlocked() const
{
    return hasSVGRareData() && svgRareData()->instanceUpdatesBlocked();
}

void SVGElement::setInstanceUpdatesBlocked(bool value)
{
    if (hasSVGRareData())
        svgRareData()->setInstanceUpdatesBlocked(value);
}

AffineTransform SVGElement::localCoordinateSpaceTransform(CTMScope) const
{
    // To be overriden by SVGGraphicsElement (or as special case SVGTextElement and SVGPatternElement)
    return AffineTransform();
}

Node::InsertionNotificationRequest SVGElement::insertedInto(ContainerNode* rootParent)
{
    Element::insertedInto(rootParent);
    updateRelativeLengthsInformation();
    buildPendingResourcesIfNeeded();
    return InsertionDone;
}

void SVGElement::removedFrom(ContainerNode* rootParent)
{
    bool wasInDocument = rootParent->inDocument();

    if (wasInDocument && hasRelativeLengths()) {
        // The root of the subtree being removed should take itself out from its parent's relative
        // length set. For the other nodes in the subtree we don't need to do anything: they will
        // get their own removedFrom() notification and just clear their sets.
        if (rootParent->isSVGElement() && !parentNode()) {
            ASSERT(toSVGElement(rootParent)->m_elementsWithRelativeLengths.contains(this));
            toSVGElement(rootParent)->updateRelativeLengthsInformation(false, this);
        }

        m_elementsWithRelativeLengths.clear();
    }

    ASSERT_WITH_SECURITY_IMPLICATION(!rootParent->isSVGElement() || !toSVGElement(rootParent)->m_elementsWithRelativeLengths.contains(this));

    Element::removedFrom(rootParent);

    if (wasInDocument) {
        rebuildAllIncomingReferences();
        removeAllIncomingReferences();
    }

    invalidateInstances();
}

void SVGElement::childrenChanged(const ChildrenChange& change)
{
    Element::childrenChanged(change);

    // Invalidate all instances associated with us.
    if (!change.byParser)
        invalidateInstances();
}

CSSPropertyID SVGElement::cssPropertyIdForSVGAttributeName(const QualifiedName& attrName)
{
    if (!attrName.namespaceURI().isNull())
        return CSSPropertyInvalid;

    static HashMap<StringImpl*, CSSPropertyID>* propertyNameToIdMap = 0;
    if (!propertyNameToIdMap) {
        propertyNameToIdMap = new HashMap<StringImpl*, CSSPropertyID>;
        // This is a list of all base CSS and SVG CSS properties which are exposed as SVG XML attributes
        const QualifiedName* const attrNames[] = {
            &alignment_baselineAttr,
            &baseline_shiftAttr,
            &buffered_renderingAttr,
            &clipAttr,
            &clip_pathAttr,
            &clip_ruleAttr,
            &SVGNames::colorAttr,
            &color_interpolationAttr,
            &color_interpolation_filtersAttr,
            &color_renderingAttr,
            &cursorAttr,
            &SVGNames::directionAttr,
            &displayAttr,
            &dominant_baselineAttr,
            // &enable_backgroundAttr,
            &fillAttr,
            &fill_opacityAttr,
            &fill_ruleAttr,
            &filterAttr,
            &flood_colorAttr,
            &flood_opacityAttr,
            &font_familyAttr,
            &font_sizeAttr,
            &font_stretchAttr,
            &font_styleAttr,
            &font_variantAttr,
            &font_weightAttr,
//             &glyph_orientation_horizontalAttr,
//             &glyph_orientation_verticalAttr,
            &image_renderingAttr,
            &letter_spacingAttr,
            &lighting_colorAttr,
            &marker_endAttr,
            &marker_midAttr,
            &marker_startAttr,
            &maskAttr,
            &mask_typeAttr,
            &opacityAttr,
            &overflowAttr,
            &paint_orderAttr,
            &pointer_eventsAttr,
            &shape_renderingAttr,
            &stop_colorAttr,
            &stop_opacityAttr,
            &strokeAttr,
            &stroke_dasharrayAttr,
            &stroke_dashoffsetAttr,
            &stroke_linecapAttr,
            &stroke_linejoinAttr,
            &stroke_miterlimitAttr,
            &stroke_opacityAttr,
            &stroke_widthAttr,
            &text_anchorAttr,
            &text_decorationAttr,
            &text_renderingAttr,
            &transform_originAttr,
            &unicode_bidiAttr,
            &vector_effectAttr,
            &visibilityAttr,
            &word_spacingAttr,
            &writing_modeAttr,
        };
        for (size_t i = 0; i < WTF_ARRAY_LENGTH(attrNames); i++) {
            CSSPropertyID propertyId = cssPropertyID(attrNames[i]->localName());
            ASSERT(propertyId > 0);
            propertyNameToIdMap->set(attrNames[i]->localName().impl(), propertyId);
        }
    }

    return propertyNameToIdMap->get(attrName.localName().impl());
}

void SVGElement::updateRelativeLengthsInformation(bool clientHasRelativeLengths, SVGElement* clientElement)
{
    ASSERT(clientElement);

    // If we're not yet in a document, this function will be called again from insertedInto(). Do nothing now.
    if (!inDocument())
        return;

    // An element wants to notify us that its own relative lengths state changed.
    // Register it in the relative length map, and register us in the parent relative length map.
    // Register the parent in the grandparents map, etc. Repeat procedure until the root of the SVG tree.
    for (ContainerNode* currentNode = this; currentNode && currentNode->isSVGElement(); currentNode = currentNode->parentNode()) {
        SVGElement* currentElement = toSVGElement(currentNode);
        ASSERT(!currentElement->m_inRelativeLengthClientsInvalidation);

        bool hadRelativeLengths = currentElement->hasRelativeLengths();
        if (clientHasRelativeLengths)
            currentElement->m_elementsWithRelativeLengths.add(clientElement);
        else
            currentElement->m_elementsWithRelativeLengths.remove(clientElement);

        // If the relative length state hasn't changed, we can stop propagating the notification.
        if (hadRelativeLengths == currentElement->hasRelativeLengths())
            return;

        clientElement = currentElement;
        clientHasRelativeLengths = clientElement->hasRelativeLengths();
    }

    // Register root SVG elements for top level viewport change notifications.
    if (isSVGSVGElement(*clientElement)) {
        SVGDocumentExtensions& svgExtensions = accessDocumentSVGExtensions();
        if (clientElement->hasRelativeLengths())
            svgExtensions.addSVGRootWithRelativeLengthDescendents(toSVGSVGElement(clientElement));
        else
            svgExtensions.removeSVGRootWithRelativeLengthDescendents(toSVGSVGElement(clientElement));
    }
}

void SVGElement::invalidateRelativeLengthClients(SubtreeLayoutScope* layoutScope)
{
    if (!inDocument())
        return;

    ASSERT(!m_inRelativeLengthClientsInvalidation);
#if ENABLE(ASSERT)
    TemporaryChange<bool> inRelativeLengthClientsInvalidationChange(m_inRelativeLengthClientsInvalidation, true);
#endif

    if (LayoutObject* layoutObject = this->layoutObject()) {
        if (hasRelativeLengths() && layoutObject->isSVGResourceContainer())
            toLayoutSVGResourceContainer(layoutObject)->invalidateCacheAndMarkForLayout(layoutScope);
        else if (selfHasRelativeLengths())
            layoutObject->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::Unknown, MarkContainerChain, layoutScope);
    }

    for (SVGElement* element : m_elementsWithRelativeLengths) {
        if (element != this)
            element->invalidateRelativeLengthClients(layoutScope);
    }
}

SVGSVGElement* SVGElement::ownerSVGElement() const
{
    ContainerNode* n = parentOrShadowHostNode();
    while (n) {
        if (isSVGSVGElement(*n))
            return toSVGSVGElement(n);

        n = n->parentOrShadowHostNode();
    }

    return nullptr;
}

SVGElement* SVGElement::viewportElement() const
{
    // This function needs shadow tree support - as LayoutSVGContainer uses this function
    // to determine the "overflow" property. <use> on <symbol> wouldn't work otherwhise.
    ContainerNode* n = parentOrShadowHostNode();
    while (n) {
        if (isSVGSVGElement(*n) || isSVGImageElement(*n) || isSVGSymbolElement(*n))
            return toSVGElement(n);

        n = n->parentOrShadowHostNode();
    }

    return nullptr;
}

SVGDocumentExtensions& SVGElement::accessDocumentSVGExtensions()
{
    // This function is provided for use by SVGAnimatedProperty to avoid
    // global inclusion of core/dom/Document.h in SVG code.
    return document().accessSVGExtensions();
}

void SVGElement::mapInstanceToElement(SVGElement* instance)
{
    ASSERT(instance);
    ASSERT(instance->inUseShadowTree());

    WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>>& instances = ensureSVGRareData()->elementInstances();
    ASSERT(!instances.contains(instance));

    instances.add(instance);
}

void SVGElement::removeInstanceMapping(SVGElement* instance)
{
    ASSERT(instance);
    ASSERT(instance->inUseShadowTree());

    if (!hasSVGRareData())
        return;

    WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>>& instances = svgRareData()->elementInstances();

    instances.remove(instance);
}

static WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>>& emptyInstances()
{
    DEFINE_STATIC_LOCAL(OwnPtrWillBePersistent<WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>>>, emptyInstances, (adoptPtrWillBeNoop(new WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>>())));
    return *emptyInstances;
}

const WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>>& SVGElement::instancesForElement() const
{
    if (!hasSVGRareData())
        return emptyInstances();
    return svgRareData()->elementInstances();
}

bool SVGElement::getBoundingBox(FloatRect& rect)
{
    if (!isSVGGraphicsElement())
        return false;

    rect = toSVGGraphicsElement(this)->getBBox();
    return true;
}

void SVGElement::setCursorElement(SVGCursorElement* cursorElement)
{
    SVGElementRareData* rareData = ensureSVGRareData();
    if (SVGCursorElement* oldCursorElement = rareData->cursorElement()) {
        if (cursorElement == oldCursorElement)
            return;
        oldCursorElement->removeReferencedElement(this);
    }
    rareData->setCursorElement(cursorElement);
}

#if !ENABLE(OILPAN)
void SVGElement::cursorElementRemoved()
{
    svgRareData()->setCursorElement(0);
}
#endif

void SVGElement::setCursorImageValue(CSSCursorImageValue* cursorImageValue)
{
    SVGElementRareData* rareData = ensureSVGRareData();
#if !ENABLE(OILPAN)
    if (CSSCursorImageValue* oldCursorImageValue = rareData->cursorImageValue()) {
        if (cursorImageValue == oldCursorImageValue)
            return;
        oldCursorImageValue->removeReferencedElement(this);
    }
#endif
    rareData->setCursorImageValue(cursorImageValue);
}

#if !ENABLE(OILPAN)
void SVGElement::cursorImageValueRemoved()
{
    svgRareData()->setCursorImageValue(0);
}
#endif

SVGElement* SVGElement::correspondingElement()
{
    ASSERT(!hasSVGRareData() || !svgRareData()->correspondingElement() || containingShadowRoot());
    return hasSVGRareData() ? svgRareData()->correspondingElement() : 0;
}

SVGUseElement* SVGElement::correspondingUseElement() const
{
    if (ShadowRoot* root = containingShadowRoot()) {
        if (isSVGUseElement(root->host()) && (root->type() == ShadowRootType::UserAgent))
            return toSVGUseElement(root->host());
    }
    return nullptr;
}

void SVGElement::setCorrespondingElement(SVGElement* correspondingElement)
{
    ensureSVGRareData()->setCorrespondingElement(correspondingElement);
}

bool SVGElement::inUseShadowTree() const
{
    return correspondingUseElement();
}

void SVGElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    RefPtrWillBeRawPtr<SVGAnimatedPropertyBase> property = propertyFromAttribute(name);
    if (property) {
        SVGParsingError parseError = NoError;
        property->setBaseValueAsString(value, parseError);
        reportAttributeParsingError(parseError, name, value);

        return;
    }

    if (name == HTMLNames::classAttr) {
        // SVG animation has currently requires special storage of values so we set
        // the className here. svgAttributeChanged actually causes the resulting
        // style updates (instead of Element::parseAttribute). We don't
        // tell Element about the change to avoid parsing the class list twice
        SVGParsingError parseError = NoError;
        m_className->setBaseValueAsString(value, parseError);
        reportAttributeParsingError(parseError, name, value);
    } else if (name == tabindexAttr) {
        Element::parseAttribute(name, value);
    } else {
        // standard events
        const AtomicString& eventName = HTMLElement::eventNameForAttributeName(name);
        if (!eventName.isNull())
            setAttributeEventListener(eventName, createAttributeEventListener(this, name, value, eventParameterName()));
        else
            Element::parseAttribute(name, value);
    }
}

typedef HashMap<QualifiedName, AnimatedPropertyType> AttributeToPropertyTypeMap;
AnimatedPropertyType SVGElement::animatedPropertyTypeForCSSAttribute(const QualifiedName& attributeName)
{
    DEFINE_STATIC_LOCAL(AttributeToPropertyTypeMap, cssPropertyMap, ());

    if (cssPropertyMap.isEmpty()) {
        // Fill the map for the first use.
        struct AttrToTypeEntry {
            const QualifiedName& attr;
            const AnimatedPropertyType propType;
        };
        const AttrToTypeEntry attrToTypes[] = {
            { alignment_baselineAttr, AnimatedString },
            { baseline_shiftAttr, AnimatedString },
            { buffered_renderingAttr, AnimatedString },
            { clip_pathAttr, AnimatedString },
            { clip_ruleAttr, AnimatedString },
            { SVGNames::colorAttr, AnimatedColor },
            { color_interpolationAttr, AnimatedString },
            { color_interpolation_filtersAttr, AnimatedString },
            { color_renderingAttr, AnimatedString },
            { cursorAttr, AnimatedString },
            { displayAttr, AnimatedString },
            { dominant_baselineAttr, AnimatedString },
            { fillAttr, AnimatedColor },
            { fill_opacityAttr, AnimatedNumber },
            { fill_ruleAttr, AnimatedString },
            { filterAttr, AnimatedString },
            { flood_colorAttr, AnimatedColor },
            { flood_opacityAttr, AnimatedNumber },
            { font_familyAttr, AnimatedString },
            { font_sizeAttr, AnimatedLength },
            { font_stretchAttr, AnimatedString },
            { font_styleAttr, AnimatedString },
            { font_variantAttr, AnimatedString },
            { font_weightAttr, AnimatedString },
            { image_renderingAttr, AnimatedString },
            { letter_spacingAttr, AnimatedLength },
            { lighting_colorAttr, AnimatedColor },
            { marker_endAttr, AnimatedString },
            { marker_midAttr, AnimatedString },
            { marker_startAttr, AnimatedString },
            { maskAttr, AnimatedString },
            { mask_typeAttr, AnimatedString },
            { opacityAttr, AnimatedNumber },
            { overflowAttr, AnimatedString },
            { paint_orderAttr, AnimatedString },
            { pointer_eventsAttr, AnimatedString },
            { shape_renderingAttr, AnimatedString },
            { stop_colorAttr, AnimatedColor },
            { stop_opacityAttr, AnimatedNumber },
            { strokeAttr, AnimatedColor },
            { stroke_dasharrayAttr, AnimatedLengthList },
            { stroke_dashoffsetAttr, AnimatedLength },
            { stroke_linecapAttr, AnimatedString },
            { stroke_linejoinAttr, AnimatedString },
            { stroke_miterlimitAttr, AnimatedNumber },
            { stroke_opacityAttr, AnimatedNumber },
            { stroke_widthAttr, AnimatedLength },
            { text_anchorAttr, AnimatedString },
            { text_decorationAttr, AnimatedString },
            { text_renderingAttr, AnimatedString },
            { vector_effectAttr, AnimatedString },
            { visibilityAttr, AnimatedString },
            { word_spacingAttr, AnimatedLength },
        };
        for (size_t i = 0; i < WTF_ARRAY_LENGTH(attrToTypes); i++)
            cssPropertyMap.set(attrToTypes[i].attr, attrToTypes[i].propType);
    }

    if (cssPropertyMap.contains(attributeName))
        return cssPropertyMap.get(attributeName);

    return AnimatedUnknown;
}

void SVGElement::addToPropertyMap(PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> passProperty)
{
    RefPtrWillBeRawPtr<SVGAnimatedPropertyBase> property(passProperty);
    QualifiedName attributeName = property->attributeName();
    m_attributeToPropertyMap.set(attributeName, property.release());
}

PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> SVGElement::propertyFromAttribute(const QualifiedName& attributeName)
{
    AttributeToPropertyMap::iterator it = m_attributeToPropertyMap.find<SVGAttributeHashTranslator>(attributeName);
    if (it == m_attributeToPropertyMap.end())
        return nullptr;

    return it->value;
}

bool SVGElement::isAnimatableCSSProperty(const QualifiedName& attrName)
{
    return animatedPropertyTypeForCSSAttribute(attrName) != AnimatedUnknown;
}

bool SVGElement::isPresentationAttribute(const QualifiedName& name) const
{
    return cssPropertyIdForSVGAttributeName(name) > 0;
}

void SVGElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    CSSPropertyID propertyID = cssPropertyIdForSVGAttributeName(name);
    if (propertyID > 0)
        addPropertyToPresentationAttributeStyle(style, propertyID, value);
}

void SVGElement::addSVGLengthPropertyToPresentationAttributeStyle(MutableStylePropertySet* style, CSSPropertyID property, SVGLength& length)
{
    addPropertyToPresentationAttributeStyle(style, property,
        length.valueInSpecifiedUnits(),
        length.cssUnitTypeQuirk());
}

bool SVGElement::haveLoadedRequiredResources()
{
    for (SVGElement* child = Traversal<SVGElement>::firstChild(*this); child; child = Traversal<SVGElement>::nextSibling(*child)) {
        if (!child->haveLoadedRequiredResources())
            return false;
    }
    return true;
}

static inline void collectInstancesForSVGElement(SVGElement* element, WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>>& instances)
{
    ASSERT(element);
    if (element->containingShadowRoot())
        return;

    ASSERT(!element->instanceUpdatesBlocked());

    instances = element->instancesForElement();
}

bool SVGElement::addEventListener(const AtomicString& eventType, PassRefPtr<EventListener> prpListener, bool useCapture)
{
    RefPtr<EventListener> listener = prpListener;

    // Add event listener to regular DOM element
    if (!Node::addEventListener(eventType, listener, useCapture))
        return false;

    // Add event listener to all shadow tree DOM element instances
    WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>> instances;
    collectInstancesForSVGElement(this, instances);
    for (SVGElement* element : instances) {
        bool result = element->Node::addEventListener(eventType, listener, useCapture);
        ASSERT_UNUSED(result, result);
    }

    return true;
}

bool SVGElement::removeEventListener(const AtomicString& eventType, PassRefPtr<EventListener> prpListener, bool useCapture)
{
    RefPtr<EventListener> listener = prpListener;

    // Remove event listener from regular DOM element
    if (!Node::removeEventListener(eventType, listener, useCapture))
        return false;

    // Remove event listener from all shadow tree DOM element instances
    WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>> instances;
    collectInstancesForSVGElement(this, instances);
    for (SVGElement* shadowTreeElement : instances) {
        ASSERT(shadowTreeElement);

        shadowTreeElement->Node::removeEventListener(eventType, listener, useCapture);
    }

    return true;
}

static bool hasLoadListener(Element* element)
{
    if (element->hasEventListeners(EventTypeNames::load))
        return true;

    for (element = element->parentOrShadowHostElement(); element; element = element->parentOrShadowHostElement()) {
        const EventListenerVector& entry = element->getEventListeners(EventTypeNames::load);
        for (size_t i = 0; i < entry.size(); ++i) {
            if (entry[i].useCapture)
                return true;
        }
    }

    return false;
}

bool SVGElement::sendSVGLoadEventIfPossible()
{
    if (!haveLoadedRequiredResources())
        return false;
    if ((isStructurallyExternal() || isSVGSVGElement(*this)) && hasLoadListener(this))
        dispatchEvent(Event::create(EventTypeNames::load));
    return true;
}

void SVGElement::sendSVGLoadEventToSelfAndAncestorChainIfPossible()
{
    // Let Document::implicitClose() dispatch the 'load' to the outermost SVG root.
    if (isOutermostSVGSVGElement())
        return;

    // Save the next parent to dispatch to in case dispatching the event mutates the tree.
    RefPtrWillBeRawPtr<Element> parent = parentOrShadowHostElement();
    if (!sendSVGLoadEventIfPossible())
        return;

    // If document/window 'load' has been sent already, then only deliver to
    // the element in question.
    if (document().loadEventFinished())
        return;

    if (!parent || !parent->isSVGElement())
        return;

    toSVGElement(parent)->sendSVGLoadEventToSelfAndAncestorChainIfPossible();
}

void SVGElement::attributeChanged(const QualifiedName& name, const AtomicString& newValue, AttributeModificationReason)
{
    Element::attributeChanged(name, newValue);

    if (name == HTMLNames::idAttr)
        rebuildAllIncomingReferences();

    // Changes to the style attribute are processed lazily (see Element::getAttribute() and related methods),
    // so we don't want changes to the style attribute to result in extra work here.
    if (name != HTMLNames::styleAttr)
        svgAttributeChanged(name);
}

void SVGElement::svgAttributeChanged(const QualifiedName& attrName)
{
    CSSPropertyID propId = SVGElement::cssPropertyIdForSVGAttributeName(attrName);
    if (propId > 0) {
        invalidateInstances();
        return;
    }

    if (attrName == HTMLNames::classAttr) {
        classAttributeChanged(AtomicString(m_className->currentValue()->value()));
        invalidateInstances();
        return;
    }

    if (attrName == HTMLNames::idAttr) {
        LayoutObject* object = layoutObject();
        // Notify resources about id changes, this is important as we cache resources by id in SVGDocumentExtensions
        if (object && object->isSVGResourceContainer())
            toLayoutSVGResourceContainer(object)->idChanged();
        if (inDocument())
            buildPendingResourcesIfNeeded();
        invalidateInstances();
        return;
    }
}

void SVGElement::synchronizeAnimatedSVGAttribute(const QualifiedName& name) const
{
    if (!elementData() || !elementData()->m_animatedSVGAttributesAreDirty)
        return;

    if (name == anyQName()) {
        AttributeToPropertyMap::const_iterator::Values it = m_attributeToPropertyMap.values().begin();
        AttributeToPropertyMap::const_iterator::Values end = m_attributeToPropertyMap.values().end();
        for (; it != end; ++it) {
            if ((*it)->needsSynchronizeAttribute())
                (*it)->synchronizeAttribute();
        }

        elementData()->m_animatedSVGAttributesAreDirty = false;
    } else {
        RefPtrWillBeRawPtr<SVGAnimatedPropertyBase> property = m_attributeToPropertyMap.get(name);
        if (property && property->needsSynchronizeAttribute())
            property->synchronizeAttribute();
    }
}

PassRefPtr<ComputedStyle> SVGElement::customStyleForLayoutObject()
{
    if (!correspondingElement())
        return document().ensureStyleResolver().styleForElement(this);

    const ComputedStyle* style = nullptr;
    if (Element* parent = parentOrShadowHostElement()) {
        if (LayoutObject* layoutObject = parent->layoutObject())
            style = layoutObject->style();
    }

    return document().ensureStyleResolver().styleForElement(correspondingElement(), style, DisallowStyleSharing);
}

MutableStylePropertySet* SVGElement::animatedSMILStyleProperties() const
{
    if (hasSVGRareData())
        return svgRareData()->animatedSMILStyleProperties();
    return nullptr;
}

MutableStylePropertySet* SVGElement::ensureAnimatedSMILStyleProperties()
{
    return ensureSVGRareData()->ensureAnimatedSMILStyleProperties();
}

void SVGElement::setUseOverrideComputedStyle(bool value)
{
    if (hasSVGRareData())
        svgRareData()->setUseOverrideComputedStyle(value);
}

const ComputedStyle* SVGElement::ensureComputedStyle(PseudoId pseudoElementSpecifier)
{
    if (!hasSVGRareData() || !svgRareData()->useOverrideComputedStyle())
        return Element::ensureComputedStyle(pseudoElementSpecifier);

    const ComputedStyle* parentStyle = nullptr;
    if (Element* parent = parentOrShadowHostElement()) {
        if (LayoutObject* layoutObject = parent->layoutObject())
            parentStyle = layoutObject->style();
    }

    return svgRareData()->overrideComputedStyle(this, parentStyle);
}

bool SVGElement::hasFocusEventListeners() const
{
    return hasEventListeners(EventTypeNames::focusin) || hasEventListeners(EventTypeNames::focusout)
        || hasEventListeners(EventTypeNames::focus) || hasEventListeners(EventTypeNames::blur);
}

void SVGElement::markForLayoutAndParentResourceInvalidation(LayoutObject* layoutObject)
{
    ASSERT(layoutObject);
    LayoutSVGResourceContainer::markForLayoutAndParentResourceInvalidation(layoutObject, true);
}

void SVGElement::invalidateInstances()
{
    if (instanceUpdatesBlocked())
        return;

    const WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>>& set = instancesForElement();
    if (set.isEmpty())
        return;

    // Mark all use elements referencing 'element' for rebuilding
    for (SVGElement* instance : set) {
        instance->setCorrespondingElement(0);

        if (SVGUseElement* element = instance->correspondingUseElement()) {
            if (element->inDocument())
                element->invalidateShadowTree();
        }
    }

    svgRareData()->elementInstances().clear();

    if (inDocument())
        document().updateLayoutTreeIfNeeded();
}

SVGElement::InstanceUpdateBlocker::InstanceUpdateBlocker(SVGElement* targetElement)
    : m_targetElement(targetElement)
{
    if (m_targetElement)
        m_targetElement->setInstanceUpdatesBlocked(true);
}

SVGElement::InstanceUpdateBlocker::~InstanceUpdateBlocker()
{
    if (m_targetElement)
        m_targetElement->setInstanceUpdatesBlocked(false);
}

#if ENABLE(ASSERT)
bool SVGElement::isAnimatableAttribute(const QualifiedName& name) const
{
    DEFINE_STATIC_LOCAL(HashSet<QualifiedName>, animatableAttributes, ());

    if (animatableAttributes.isEmpty()) {
        const QualifiedName* const animatableAttrs[] = {
            &XLinkNames::hrefAttr,
            &SVGNames::amplitudeAttr,
            &SVGNames::azimuthAttr,
            &SVGNames::baseFrequencyAttr,
            &SVGNames::biasAttr,
            &SVGNames::clipPathUnitsAttr,
            &SVGNames::cxAttr,
            &SVGNames::cyAttr,
            &SVGNames::diffuseConstantAttr,
            &SVGNames::divisorAttr,
            &SVGNames::dxAttr,
            &SVGNames::dyAttr,
            &SVGNames::edgeModeAttr,
            &SVGNames::elevationAttr,
            &SVGNames::exponentAttr,
            &SVGNames::filterUnitsAttr,
            &SVGNames::fxAttr,
            &SVGNames::fyAttr,
            &SVGNames::gradientTransformAttr,
            &SVGNames::gradientUnitsAttr,
            &SVGNames::heightAttr,
            &SVGNames::in2Attr,
            &SVGNames::inAttr,
            &SVGNames::interceptAttr,
            &SVGNames::k1Attr,
            &SVGNames::k2Attr,
            &SVGNames::k3Attr,
            &SVGNames::k4Attr,
            &SVGNames::kernelMatrixAttr,
            &SVGNames::kernelUnitLengthAttr,
            &SVGNames::lengthAdjustAttr,
            &SVGNames::limitingConeAngleAttr,
            &SVGNames::markerHeightAttr,
            &SVGNames::markerUnitsAttr,
            &SVGNames::markerWidthAttr,
            &SVGNames::maskContentUnitsAttr,
            &SVGNames::maskUnitsAttr,
            &SVGNames::methodAttr,
            &SVGNames::modeAttr,
            &SVGNames::numOctavesAttr,
            &SVGNames::offsetAttr,
            &SVGNames::operatorAttr,
            &SVGNames::orderAttr,
            &SVGNames::orientAttr,
            &SVGNames::pathLengthAttr,
            &SVGNames::patternContentUnitsAttr,
            &SVGNames::patternTransformAttr,
            &SVGNames::patternUnitsAttr,
            &SVGNames::pointsAtXAttr,
            &SVGNames::pointsAtYAttr,
            &SVGNames::pointsAtZAttr,
            &SVGNames::preserveAlphaAttr,
            &SVGNames::preserveAspectRatioAttr,
            &SVGNames::primitiveUnitsAttr,
            &SVGNames::radiusAttr,
            &SVGNames::rAttr,
            &SVGNames::refXAttr,
            &SVGNames::refYAttr,
            &SVGNames::resultAttr,
            &SVGNames::rotateAttr,
            &SVGNames::rxAttr,
            &SVGNames::ryAttr,
            &SVGNames::scaleAttr,
            &SVGNames::seedAttr,
            &SVGNames::slopeAttr,
            &SVGNames::spacingAttr,
            &SVGNames::specularConstantAttr,
            &SVGNames::specularExponentAttr,
            &SVGNames::spreadMethodAttr,
            &SVGNames::startOffsetAttr,
            &SVGNames::stdDeviationAttr,
            &SVGNames::stitchTilesAttr,
            &SVGNames::surfaceScaleAttr,
            &SVGNames::tableValuesAttr,
            &SVGNames::targetAttr,
            &SVGNames::targetXAttr,
            &SVGNames::targetYAttr,
            &SVGNames::transformAttr,
            &SVGNames::typeAttr,
            &SVGNames::valuesAttr,
            &SVGNames::viewBoxAttr,
            &SVGNames::widthAttr,
            &SVGNames::x1Attr,
            &SVGNames::x2Attr,
            &SVGNames::xAttr,
            &SVGNames::xChannelSelectorAttr,
            &SVGNames::y1Attr,
            &SVGNames::y2Attr,
            &SVGNames::yAttr,
            &SVGNames::yChannelSelectorAttr,
            &SVGNames::zAttr,
        };
        for (size_t i = 0; i < WTF_ARRAY_LENGTH(animatableAttrs); i++)
            animatableAttributes.add(*animatableAttrs[i]);
    }

    if (name == classAttr)
        return true;

    return animatableAttributes.contains(name);
}
#endif

SVGElementSet* SVGElement::setOfIncomingReferences() const
{
    if (!hasSVGRareData())
        return nullptr;
    return &svgRareData()->incomingReferences();
}

void SVGElement::addReferenceTo(SVGElement* targetElement)
{
    ASSERT(targetElement);

    ensureSVGRareData()->outgoingReferences().add(targetElement);
    targetElement->ensureSVGRareData()->incomingReferences().add(this);
}

void SVGElement::rebuildAllIncomingReferences()
{
    if (!hasSVGRareData())
        return;

    const SVGElementSet& incomingReferences = svgRareData()->incomingReferences();

    // Iterate on a snapshot as |incomingReferences| may be altered inside loop.
    WillBeHeapVector<RawPtrWillBeMember<SVGElement>> incomingReferencesSnapshot;
    copyToVector(incomingReferences, incomingReferencesSnapshot);

    // Force rebuilding the |sourceElement| so it knows about this change.
    for (SVGElement* sourceElement : incomingReferencesSnapshot) {
        // Before rebuilding |sourceElement| ensure it was not removed from under us.
        if (incomingReferences.contains(sourceElement))
            sourceElement->svgAttributeChanged(XLinkNames::hrefAttr);
    }
}

void SVGElement::removeAllIncomingReferences()
{
    if (!hasSVGRareData())
        return;

    SVGElementSet& incomingReferences = svgRareData()->incomingReferences();
    for (SVGElement* sourceElement: incomingReferences) {
        ASSERT(sourceElement->hasSVGRareData());
        sourceElement->ensureSVGRareData()->outgoingReferences().remove(this);
    }
    incomingReferences.clear();
}

void SVGElement::removeAllOutgoingReferences()
{
    if (!hasSVGRareData())
        return;

    SVGElementSet& outgoingReferences = svgRareData()->outgoingReferences();
    for (SVGElement* targetElement : outgoingReferences) {
        ASSERT(targetElement->hasSVGRareData());
        targetElement->ensureSVGRareData()->incomingReferences().remove(this);
    }
    outgoingReferences.clear();
}

DEFINE_TRACE(SVGElement)
{
#if ENABLE(OILPAN)
    visitor->trace(m_elementsWithRelativeLengths);
    visitor->trace(m_attributeToPropertyMap);
    visitor->trace(m_SVGRareData);
    visitor->trace(m_className);
#endif
    Element::trace(visitor);
}

const AtomicString& SVGElement::eventParameterName()
{
    DEFINE_STATIC_LOCAL(const AtomicString, evtString, ("evt", AtomicString::ConstructFromLiteral));
    return evtString;
}

} // namespace blink

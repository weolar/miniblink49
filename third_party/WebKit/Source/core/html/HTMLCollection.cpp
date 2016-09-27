/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003-2008, 2011, 2012, 2014 Apple Inc. All rights reserved.
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
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
 *
 */

#include "config.h"
#include "core/html/HTMLCollection.h"

#include "core/HTMLNames.h"
#include "core/dom/ClassCollection.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/NodeRareData.h"
#include "core/html/DocumentNameCollection.h"
#include "core/html/HTMLDataListOptionsCollection.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLObjectElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/HTMLOptionsCollection.h"
#include "core/html/HTMLTagCollection.h"
#include "core/html/WindowNameCollection.h"
#include "wtf/HashSet.h"

namespace blink {

using namespace HTMLNames;

static bool shouldTypeOnlyIncludeDirectChildren(CollectionType type)
{
    switch (type) {
    case ClassCollectionType:
    case TagCollectionType:
    case HTMLTagCollectionType:
    case DocAll:
    case DocAnchors:
    case DocApplets:
    case DocEmbeds:
    case DocForms:
    case DocImages:
    case DocLinks:
    case DocScripts:
    case DocumentNamedItems:
    case MapAreas:
    case TableRows:
    case SelectOptions:
    case SelectedOptions:
    case DataListOptions:
    case WindowNamedItems:
    case FormControls:
        return false;
    case NodeChildren:
    case TRCells:
    case TSectionRows:
    case TableTBodies:
        return true;
    case NameNodeListType:
    case RadioNodeListType:
    case RadioImgNodeListType:
    case LabelsNodeListType:
        break;
    }
    ASSERT_NOT_REACHED();
    return false;
}

static NodeListRootType rootTypeFromCollectionType(CollectionType type)
{
    switch (type) {
    case DocImages:
    case DocApplets:
    case DocEmbeds:
    case DocForms:
    case DocLinks:
    case DocAnchors:
    case DocScripts:
    case DocAll:
    case WindowNamedItems:
    case DocumentNamedItems:
    case FormControls:
        return NodeListIsRootedAtDocument;
    case ClassCollectionType:
    case TagCollectionType:
    case HTMLTagCollectionType:
    case NodeChildren:
    case TableTBodies:
    case TSectionRows:
    case TableRows:
    case TRCells:
    case SelectOptions:
    case SelectedOptions:
    case DataListOptions:
    case MapAreas:
        return NodeListIsRootedAtNode;
    case NameNodeListType:
    case RadioNodeListType:
    case RadioImgNodeListType:
    case LabelsNodeListType:
        break;
    }
    ASSERT_NOT_REACHED();
    return NodeListIsRootedAtNode;
}

static NodeListInvalidationType invalidationTypeExcludingIdAndNameAttributes(CollectionType type)
{
    switch (type) {
    case TagCollectionType:
    case HTMLTagCollectionType:
    case DocImages:
    case DocEmbeds:
    case DocForms:
    case DocScripts:
    case DocAll:
    case NodeChildren:
    case TableTBodies:
    case TSectionRows:
    case TableRows:
    case TRCells:
    case SelectOptions:
    case MapAreas:
        return DoNotInvalidateOnAttributeChanges;
    case DocApplets:
    case SelectedOptions:
    case DataListOptions:
        // FIXME: We can do better some day.
        return InvalidateOnAnyAttrChange;
    case DocAnchors:
        return InvalidateOnNameAttrChange;
    case DocLinks:
        return InvalidateOnHRefAttrChange;
    case WindowNamedItems:
        return InvalidateOnIdNameAttrChange;
    case DocumentNamedItems:
        return InvalidateOnIdNameAttrChange;
    case FormControls:
        return InvalidateForFormControls;
    case ClassCollectionType:
        return InvalidateOnClassAttrChange;
    case NameNodeListType:
    case RadioNodeListType:
    case RadioImgNodeListType:
    case LabelsNodeListType:
        break;
    }
    ASSERT_NOT_REACHED();
    return DoNotInvalidateOnAttributeChanges;
}

HTMLCollection::HTMLCollection(ContainerNode& ownerNode, CollectionType type, ItemAfterOverrideType itemAfterOverrideType)
    : LiveNodeListBase(ownerNode, rootTypeFromCollectionType(type), invalidationTypeExcludingIdAndNameAttributes(type), type)
    , m_overridesItemAfter(itemAfterOverrideType == OverridesItemAfter)
    , m_shouldOnlyIncludeDirectChildren(shouldTypeOnlyIncludeDirectChildren(type))
{
}

PassRefPtrWillBeRawPtr<HTMLCollection> HTMLCollection::create(ContainerNode& base, CollectionType type)
{
    return adoptRefWillBeNoop(new HTMLCollection(base, type, DoesNotOverrideItemAfter));
}

HTMLCollection::~HTMLCollection()
{
#if !ENABLE(OILPAN)
    if (hasValidIdNameCache())
        unregisterIdNameCacheFromDocument(document());
    // Named HTMLCollection types remove cache by themselves.
    if (isUnnamedHTMLCollectionType(type()))
        ownerNode().nodeLists()->removeCache(this, type());
#endif
}

void HTMLCollection::invalidateCache(Document* oldDocument) const
{
    m_collectionItemsCache.invalidate();
    invalidateIdNameCacheMaps(oldDocument);
}

unsigned HTMLCollection::length() const
{
    return m_collectionItemsCache.nodeCount(*this);
}

Element* HTMLCollection::item(unsigned offset) const
{
    return m_collectionItemsCache.nodeAt(*this, offset);
}

static inline bool isMatchingHTMLElement(const HTMLCollection& htmlCollection, const HTMLElement& element)
{
    switch (htmlCollection.type()) {
    case DocImages:
        return element.hasTagName(imgTag);
    case DocScripts:
        return element.hasTagName(scriptTag);
    case DocForms:
        return element.hasTagName(formTag);
    case DocumentNamedItems:
        return toDocumentNameCollection(htmlCollection).elementMatches(element);
    case TableTBodies:
        return element.hasTagName(tbodyTag);
    case TRCells:
        return element.hasTagName(tdTag) || element.hasTagName(thTag);
    case TSectionRows:
        return element.hasTagName(trTag);
    case SelectOptions:
        return toHTMLOptionsCollection(htmlCollection).elementMatches(element);
    case SelectedOptions:
        return isHTMLOptionElement(element) && toHTMLOptionElement(element).selected();
    case DataListOptions:
        return toHTMLDataListOptionsCollection(htmlCollection).elementMatches(element);
    case MapAreas:
        return element.hasTagName(areaTag);
    case DocApplets:
        return element.hasTagName(appletTag) || (isHTMLObjectElement(element) && toHTMLObjectElement(element).containsJavaApplet());
    case DocEmbeds:
        return element.hasTagName(embedTag);
    case DocLinks:
        return (element.hasTagName(aTag) || element.hasTagName(areaTag)) && element.fastHasAttribute(hrefAttr);
    case DocAnchors:
        return element.hasTagName(aTag) && element.fastHasAttribute(nameAttr);
    case ClassCollectionType:
    case TagCollectionType:
    case HTMLTagCollectionType:
    case DocAll:
    case NodeChildren:
    case FormControls:
    case TableRows:
    case WindowNamedItems:
    case NameNodeListType:
    case RadioNodeListType:
    case RadioImgNodeListType:
    case LabelsNodeListType:
        ASSERT_NOT_REACHED();
    }
    return false;
}

inline bool HTMLCollection::elementMatches(const Element& element) const
{
    // These collections apply to any kind of Elements, not just HTMLElements.
    switch (type()) {
    case DocAll:
    case NodeChildren:
        return true;
    case ClassCollectionType:
        return toClassCollection(*this).elementMatches(element);
    case TagCollectionType:
        return toTagCollection(*this).elementMatches(element);
    case HTMLTagCollectionType:
        return toHTMLTagCollection(*this).elementMatches(element);
    case WindowNamedItems:
        return toWindowNameCollection(*this).elementMatches(element);
    default:
        break;
    }

    // The following only applies to HTMLElements.
    return element.isHTMLElement() && isMatchingHTMLElement(*this, toHTMLElement(element));
}

namespace {

template <class HTMLCollectionType>
class IsMatch {
public:
    IsMatch(const HTMLCollectionType& list)
        : m_list(list)
    { }

    bool operator() (const Element& element) const
    {
        return m_list.elementMatches(element);
    }

private:
    const HTMLCollectionType& m_list;
};

} // namespace

template <class HTMLCollectionType>
static inline IsMatch<HTMLCollectionType> makeIsMatch(const HTMLCollectionType& list) { return IsMatch<HTMLCollectionType>(list); }

Element* HTMLCollection::virtualItemAfter(Element*) const
{
    ASSERT_NOT_REACHED();
    return nullptr;
}

static inline bool nameShouldBeVisibleInDocumentAll(const HTMLElement& element)
{
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/common-dom-interfaces.html#dom-htmlallcollection-nameditem:
    // The document.all collection returns only certain types of elements by name,
    // although it returns any type of element by id.
    return element.hasTagName(aTag)
        || element.hasTagName(appletTag)
        || element.hasTagName(areaTag)
        || element.hasTagName(embedTag)
        || element.hasTagName(formTag)
        || element.hasTagName(frameTag)
        || element.hasTagName(framesetTag)
        || element.hasTagName(iframeTag)
        || element.hasTagName(imgTag)
        || element.hasTagName(inputTag)
        || element.hasTagName(objectTag)
        || element.hasTagName(selectTag);
}

Element* HTMLCollection::traverseToFirst() const
{
    switch (type()) {
    case HTMLTagCollectionType:
        return ElementTraversal::firstWithin(rootNode(), makeIsMatch(toHTMLTagCollection(*this)));
    case ClassCollectionType:
        return ElementTraversal::firstWithin(rootNode(), makeIsMatch(toClassCollection(*this)));
    default:
        if (overridesItemAfter())
            return virtualItemAfter(0);
        if (shouldOnlyIncludeDirectChildren())
            return ElementTraversal::firstChild(rootNode(), makeIsMatch(*this));
        return ElementTraversal::firstWithin(rootNode(), makeIsMatch(*this));
    }
}

Element* HTMLCollection::traverseToLast() const
{
    ASSERT(canTraverseBackward());
    if (shouldOnlyIncludeDirectChildren())
        return ElementTraversal::lastChild(rootNode(), makeIsMatch(*this));
    return ElementTraversal::lastWithin(rootNode(), makeIsMatch(*this));
}

Element* HTMLCollection::traverseForwardToOffset(unsigned offset, Element& currentElement, unsigned& currentOffset) const
{
    ASSERT(currentOffset < offset);
    switch (type()) {
    case HTMLTagCollectionType:
        return traverseMatchingElementsForwardToOffset(currentElement, &rootNode(), offset, currentOffset, makeIsMatch(toHTMLTagCollection(*this)));
    case ClassCollectionType:
        return traverseMatchingElementsForwardToOffset(currentElement, &rootNode(), offset, currentOffset, makeIsMatch(toClassCollection(*this)));
    default:
        if (overridesItemAfter()) {
            for (Element* next = virtualItemAfter(&currentElement); next; next = virtualItemAfter(next)) {
                if (++currentOffset == offset)
                    return next;
            }
            return nullptr;
        }
        if (shouldOnlyIncludeDirectChildren()) {
            IsMatch<HTMLCollection> isMatch(*this);
            for (Element* next = ElementTraversal::nextSibling(currentElement, isMatch); next; next = ElementTraversal::nextSibling(*next, isMatch)) {
                if (++currentOffset == offset)
                    return next;
            }
            return nullptr;
        }
        return traverseMatchingElementsForwardToOffset(currentElement, &rootNode(), offset, currentOffset, makeIsMatch(*this));
    }
}

Element* HTMLCollection::traverseBackwardToOffset(unsigned offset, Element& currentElement, unsigned& currentOffset) const
{
    ASSERT(currentOffset > offset);
    ASSERT(canTraverseBackward());
    if (shouldOnlyIncludeDirectChildren()) {
        IsMatch<HTMLCollection> isMatch(*this);
        for (Element* previous = ElementTraversal::previousSibling(currentElement, isMatch); previous; previous = ElementTraversal::previousSibling(*previous, isMatch)) {
            if (--currentOffset == offset)
                return previous;
        }
        return nullptr;
    }
    return traverseMatchingElementsBackwardToOffset(currentElement, &rootNode(), offset, currentOffset, makeIsMatch(*this));
}

Element* HTMLCollection::namedItem(const AtomicString& name) const
{
    // http://msdn.microsoft.com/workshop/author/dhtml/reference/methods/nameditem.asp
    // This method first searches for an object with a matching id
    // attribute. If a match is not found, the method then searches for an
    // object with a matching name attribute, but only on those elements
    // that are allowed a name attribute.
    updateIdNameCache();

    const NamedItemCache& cache = namedItemCache();
    WillBeHeapVector<RawPtrWillBeMember<Element>>* idResults = cache.getElementsById(name);
    if (idResults && !idResults->isEmpty())
        return idResults->first();

    WillBeHeapVector<RawPtrWillBeMember<Element>>* nameResults = cache.getElementsByName(name);
    if (nameResults && !nameResults->isEmpty())
        return nameResults->first();

    return nullptr;
}

bool HTMLCollection::namedPropertyQuery(const AtomicString& name, ExceptionState&)
{
    return namedItem(name);
}

void HTMLCollection::supportedPropertyNames(Vector<String>& names)
{
    // As per the specification (http://dom.spec.whatwg.org/#htmlcollection):
    // The supported property names are the values from the list returned by these steps:
    // 1. Let result be an empty list.
    // 2. For each element represented by the collection, in tree order, run these substeps:
    //   1. If element has an ID which is neither the empty string nor is in result, append element's ID to result.
    //   2. If element is in the HTML namespace and has a name attribute whose value is neither the empty string
    //      nor is in result, append element's name attribute value to result.
    // 3. Return result.
    HashSet<AtomicString> existingNames;
    unsigned length = this->length();
    for (unsigned i = 0; i < length; ++i) {
        Element* element = item(i);
        const AtomicString& idAttribute = element->getIdAttribute();
        if (!idAttribute.isEmpty()) {
            HashSet<AtomicString>::AddResult addResult = existingNames.add(idAttribute);
            if (addResult.isNewEntry)
                names.append(idAttribute);
        }
        if (!element->isHTMLElement())
            continue;
        const AtomicString& nameAttribute = element->getNameAttribute();
        if (!nameAttribute.isEmpty() && (type() != DocAll || nameShouldBeVisibleInDocumentAll(toHTMLElement(*element)))) {
            HashSet<AtomicString>::AddResult addResult = existingNames.add(nameAttribute);
            if (addResult.isNewEntry)
                names.append(nameAttribute);
        }
    }
}

void HTMLCollection::namedPropertyEnumerator(Vector<String>& names, ExceptionState&)
{
    supportedPropertyNames(names);
}

void HTMLCollection::updateIdNameCache() const
{
    if (hasValidIdNameCache())
        return;

    OwnPtrWillBeRawPtr<NamedItemCache> cache = NamedItemCache::create();
    unsigned length = this->length();
    for (unsigned i = 0; i < length; ++i) {
        Element* element = item(i);
        const AtomicString& idAttrVal = element->getIdAttribute();
        if (!idAttrVal.isEmpty())
            cache->addElementWithId(idAttrVal, element);
        if (!element->isHTMLElement())
            continue;
        const AtomicString& nameAttrVal = element->getNameAttribute();
        if (!nameAttrVal.isEmpty() && idAttrVal != nameAttrVal && (type() != DocAll || nameShouldBeVisibleInDocumentAll(toHTMLElement(*element))))
            cache->addElementWithName(nameAttrVal, element);
    }
    // Set the named item cache last as traversing the tree may cause cache invalidation.
    setNamedItemCache(cache.release());
}

void HTMLCollection::namedItems(const AtomicString& name, WillBeHeapVector<RefPtrWillBeMember<Element>>& result) const
{
    ASSERT(result.isEmpty());
    if (name.isEmpty())
        return;

    updateIdNameCache();

    const NamedItemCache& cache = namedItemCache();
    if (WillBeHeapVector<RawPtrWillBeMember<Element>>* idResults = cache.getElementsById(name)) {
        for (unsigned i = 0; i < idResults->size(); ++i)
            result.append(idResults->at(i));
    }
    if (WillBeHeapVector<RawPtrWillBeMember<Element>>* nameResults = cache.getElementsByName(name)) {
        for (unsigned i = 0; i < nameResults->size(); ++i)
            result.append(nameResults->at(i));
    }
}

HTMLCollection::NamedItemCache::NamedItemCache()
{
}

DEFINE_TRACE(HTMLCollection)
{
    visitor->trace(m_namedItemCache);
    visitor->trace(m_collectionItemsCache);
    LiveNodeListBase::trace(visitor);
}

} // namespace blink

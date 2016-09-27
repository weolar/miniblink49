/*
 * Copyright (C) 2011, 2013 Apple Inc. All rights reserved.
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/dom/SelectorQuery.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/css/SelectorChecker.h"
#include "core/css/parser/CSSParser.h"
#include "core/dom/Document.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/Node.h"
#include "core/dom/StaticNodeList.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/dom/shadow/ShadowRoot.h"

namespace blink {

struct SingleElementSelectorQueryTrait {
    typedef Element* OutputType;
    static const bool shouldOnlyMatchFirstElement = true;
    ALWAYS_INLINE static void appendElement(OutputType& output, Element& element)
    {
        ASSERT(!output);
        output = &element;
    }
};

struct AllElementsSelectorQueryTrait {
    typedef WillBeHeapVector<RefPtrWillBeMember<Element>> OutputType;
    static const bool shouldOnlyMatchFirstElement = false;
    ALWAYS_INLINE static void appendElement(OutputType& output, Element& element)
    {
        output.append(&element);
    }
};

enum ClassElementListBehavior { AllElements, OnlyRoots };

template <ClassElementListBehavior onlyRoots>
class ClassElementList {
    STACK_ALLOCATED();
public:
    ClassElementList(ContainerNode& rootNode, const AtomicString& className)
        : m_className(className)
        , m_rootNode(&rootNode)
        , m_currentElement(nextInternal(ElementTraversal::firstWithin(rootNode))) { }

    bool isEmpty() const { return !m_currentElement; }

    Element* next()
    {
        Element* current = m_currentElement;
        ASSERT(current);
        if (onlyRoots)
            m_currentElement = nextInternal(ElementTraversal::nextSkippingChildren(*m_currentElement, m_rootNode));
        else
            m_currentElement = nextInternal(ElementTraversal::next(*m_currentElement, m_rootNode));
        return current;
    }

private:
    Element* nextInternal(Element* element)
    {
        for (; element; element = ElementTraversal::next(*element, m_rootNode)) {
            if (element->hasClass() && element->classNames().contains(m_className))
                return element;
        }
        return 0;
    }

    const AtomicString& m_className;
    RawPtrWillBeMember<ContainerNode> m_rootNode;
    RawPtrWillBeMember<Element> m_currentElement;
};

void SelectorDataList::initialize(const CSSSelectorList& selectorList)
{
    ASSERT(m_selectors.isEmpty());

    unsigned selectorCount = 0;
    for (const CSSSelector* selector = selectorList.first(); selector; selector = CSSSelectorList::next(*selector))
        selectorCount++;

    m_crossesTreeBoundary = false;
    m_needsUpdatedDistribution = false;
    m_selectors.reserveInitialCapacity(selectorCount);
    unsigned index = 0;
    for (const CSSSelector* selector = selectorList.first(); selector; selector = CSSSelectorList::next(*selector), ++index) {
        m_selectors.uncheckedAppend(selector);
        m_crossesTreeBoundary |= selectorList.selectorCrossesTreeScopes(index);
        m_needsUpdatedDistribution |= selectorList.selectorNeedsUpdatedDistribution(index);
    }
}

inline bool SelectorDataList::selectorMatches(const CSSSelector& selector, Element& element, const ContainerNode& rootNode) const
{
    SelectorChecker selectorChecker(SelectorChecker::QueryingRules);
    SelectorChecker::SelectorCheckingContext selectorCheckingContext(&element, SelectorChecker::VisitedMatchDisabled);
    selectorCheckingContext.selector = &selector;
    selectorCheckingContext.scope = !rootNode.isDocumentNode() ? &rootNode : 0;
    if (selectorCheckingContext.scope)
        selectorCheckingContext.scopeContainsLastMatchedElement = true;
    return selectorChecker.match(selectorCheckingContext);
}

bool SelectorDataList::matches(Element& targetElement) const
{
    if (m_needsUpdatedDistribution)
        targetElement.updateDistribution();

    unsigned selectorCount = m_selectors.size();
    for (unsigned i = 0; i < selectorCount; ++i) {
        if (selectorMatches(*m_selectors[i], targetElement, targetElement))
            return true;
    }

    return false;
}

Element* SelectorDataList::closest(Element& targetElement) const
{
    if (m_needsUpdatedDistribution)
        targetElement.updateDistribution();

    unsigned selectorCount = m_selectors.size();
    for (Element* currentElement = &targetElement; currentElement; currentElement = currentElement->parentElement()) {
        for (unsigned i = 0; i < selectorCount; ++i) {
            if (selectorMatches(*m_selectors[i], *currentElement, targetElement))
                return currentElement;
        }
    }
    return nullptr;
}

PassRefPtrWillBeRawPtr<StaticElementList> SelectorDataList::queryAll(ContainerNode& rootNode) const
{
    WillBeHeapVector<RefPtrWillBeMember<Element>> result;
    execute<AllElementsSelectorQueryTrait>(rootNode, result);
    return StaticElementList::adopt(result);
}

PassRefPtrWillBeRawPtr<Element> SelectorDataList::queryFirst(ContainerNode& rootNode) const
{
    Element* matchedElement = 0;
    execute<SingleElementSelectorQueryTrait>(rootNode, matchedElement);
    return matchedElement;
}

template <typename SelectorQueryTrait>
void SelectorDataList::collectElementsByClassName(ContainerNode& rootNode, const AtomicString& className,  typename SelectorQueryTrait::OutputType& output) const
{
    for (Element& element : ElementTraversal::descendantsOf(rootNode)) {
        if (element.hasClass() && element.classNames().contains(className)) {
            SelectorQueryTrait::appendElement(output, element);
            if (SelectorQueryTrait::shouldOnlyMatchFirstElement)
                return;
        }
    }
}

inline bool matchesTagName(const QualifiedName& tagName, const Element& element)
{
    if (tagName == anyQName())
        return true;
    if (element.hasLocalName(tagName.localName()))
        return true;
    // Non-html elements in html documents are normalized to their camel-cased
    // version during parsing if applicable. Yet, type selectors are lower-cased
    // for selectors in html documents. Compare the upper case converted names
    // instead to allow matching SVG elements like foreignObject.
    if (!element.isHTMLElement() && element.document().isHTMLDocument())
        return element.tagQName().localNameUpper() == tagName.localNameUpper();
    return false;
}

template <typename SelectorQueryTrait>
void SelectorDataList::collectElementsByTagName(ContainerNode& rootNode, const QualifiedName& tagName,  typename SelectorQueryTrait::OutputType& output) const
{
    for (Element& element : ElementTraversal::descendantsOf(rootNode)) {
        // querySelector*() doesn't allow namespaces and throws before it gets
        // here so we can ignore them.
        ASSERT(tagName.namespaceURI() == starAtom);
        if (matchesTagName(tagName, element)) {
            SelectorQueryTrait::appendElement(output, element);
            if (SelectorQueryTrait::shouldOnlyMatchFirstElement)
                return;
        }
    }
}

inline bool SelectorDataList::canUseFastQuery(const ContainerNode& rootNode) const
{
    if (m_crossesTreeBoundary)
        return false;
    if (m_needsUpdatedDistribution)
        return false;
    if (rootNode.document().inQuirksMode())
        return false;
    if (!rootNode.inDocument())
        return false;
    return m_selectors.size() == 1;
}

inline bool ancestorHasClassName(ContainerNode& rootNode, const AtomicString& className)
{
    if (!rootNode.isElementNode())
        return false;

    for (Element* element = &toElement(rootNode); element; element = element->parentElement()) {
        if (element->hasClass() && element->classNames().contains(className))
            return true;
    }
    return false;
}


// If returns true, traversalRoots has the elements that may match the selector query.
//
// If returns false, traversalRoots has the rootNode parameter or descendants of rootNode representing
// the subtree for which we can limit the querySelector traversal.
//
// The travseralRoots may be empty, regardless of the returned bool value, if this method finds that the selectors won't
// match any element.
template <typename SelectorQueryTrait>
void SelectorDataList::findTraverseRootsAndExecute(ContainerNode& rootNode, typename SelectorQueryTrait::OutputType& output) const
{
    // We need to return the matches in document order. To use id lookup while there is possiblity of multiple matches
    // we would need to sort the results. For now, just traverse the document in that case.
    ASSERT(m_selectors.size() == 1);

    bool isRightmostSelector = true;
    bool startFromParent = false;

    for (const CSSSelector* selector = m_selectors[0]; selector; selector = selector->tagHistory()) {
        if (selector->match() == CSSSelector::Id && !rootNode.document().containsMultipleElementsWithId(selector->value())) {
            Element* element = rootNode.treeScope().getElementById(selector->value());
            ContainerNode* adjustedNode = &rootNode;
            if (element && (isTreeScopeRoot(rootNode) || element->isDescendantOf(&rootNode)))
                adjustedNode = element;
            else if (!element || isRightmostSelector)
                adjustedNode = 0;
            if (isRightmostSelector) {
                executeForTraverseRoot<SelectorQueryTrait>(*m_selectors[0], adjustedNode, MatchesTraverseRoots, rootNode, output);
                return;
            }

            if (startFromParent && adjustedNode)
                adjustedNode = adjustedNode->parentNode();

            executeForTraverseRoot<SelectorQueryTrait>(*m_selectors[0], adjustedNode, DoesNotMatchTraverseRoots, rootNode, output);
            return;
        }

        // If we have both CSSSelector::Id and CSSSelector::Class at the same time, we should use Id
        // to find traverse root.
        if (!SelectorQueryTrait::shouldOnlyMatchFirstElement && !startFromParent && selector->match() == CSSSelector::Class) {
            if (isRightmostSelector) {
                ClassElementList<AllElements> traverseRoots(rootNode, selector->value());
                executeForTraverseRoots<SelectorQueryTrait>(*m_selectors[0], traverseRoots, MatchesTraverseRoots, rootNode, output);
                return;
            }
            // Since there exists some ancestor element which has the class name, we need to see all children of rootNode.
            if (ancestorHasClassName(rootNode, selector->value())) {
                executeForTraverseRoot<SelectorQueryTrait>(*m_selectors[0], &rootNode, DoesNotMatchTraverseRoots, rootNode, output);
                return;
            }

            ClassElementList<OnlyRoots> traverseRoots(rootNode, selector->value());
            executeForTraverseRoots<SelectorQueryTrait>(*m_selectors[0], traverseRoots, DoesNotMatchTraverseRoots, rootNode, output);
            return;
        }

        if (selector->relation() == CSSSelector::SubSelector)
            continue;
        isRightmostSelector = false;
        if (selector->relation() == CSSSelector::DirectAdjacent || selector->relation() == CSSSelector::IndirectAdjacent)
            startFromParent = true;
        else
            startFromParent = false;
    }

    executeForTraverseRoot<SelectorQueryTrait>(*m_selectors[0], &rootNode, DoesNotMatchTraverseRoots, rootNode, output);
}

template <typename SelectorQueryTrait>
void SelectorDataList::executeForTraverseRoot(const CSSSelector& selector, ContainerNode* traverseRoot, MatchTraverseRootState matchTraverseRoot, ContainerNode& rootNode, typename SelectorQueryTrait::OutputType& output) const
{
    if (!traverseRoot)
        return;

    if (matchTraverseRoot) {
        if (selectorMatches(selector, toElement(*traverseRoot), rootNode))
            SelectorQueryTrait::appendElement(output, toElement(*traverseRoot));
        return;
    }

    for (Element& element : ElementTraversal::descendantsOf(*traverseRoot)) {
        if (selectorMatches(selector, element, rootNode)) {
            SelectorQueryTrait::appendElement(output, element);
            if (SelectorQueryTrait::shouldOnlyMatchFirstElement)
                return;
        }
    }
}

template <typename SelectorQueryTrait, typename SimpleElementListType>
void SelectorDataList::executeForTraverseRoots(const CSSSelector& selector, SimpleElementListType& traverseRoots, MatchTraverseRootState matchTraverseRoots, ContainerNode& rootNode, typename SelectorQueryTrait::OutputType& output) const
{
    if (traverseRoots.isEmpty())
        return;

    if (matchTraverseRoots) {
        while (!traverseRoots.isEmpty()) {
            Element& element = *traverseRoots.next();
            if (selectorMatches(selector, element, rootNode)) {
                SelectorQueryTrait::appendElement(output, element);
                if (SelectorQueryTrait::shouldOnlyMatchFirstElement)
                    return;
            }
        }
        return;
    }

    while (!traverseRoots.isEmpty()) {
        for (Element& element : ElementTraversal::descendantsOf(*traverseRoots.next())) {
            if (selectorMatches(selector, element, rootNode)) {
                SelectorQueryTrait::appendElement(output, element);
                if (SelectorQueryTrait::shouldOnlyMatchFirstElement)
                    return;
            }
        }
    }
}

template <typename SelectorQueryTrait>
bool SelectorDataList::selectorListMatches(ContainerNode& rootNode, Element& element, typename SelectorQueryTrait::OutputType& output) const
{
    for (unsigned i = 0; i < m_selectors.size(); ++i) {
        if (selectorMatches(*m_selectors[i], element, rootNode)) {
            SelectorQueryTrait::appendElement(output, element);
            return true;
        }
    }
    return false;
}

template <typename SelectorQueryTrait>
void SelectorDataList::executeSlow(ContainerNode& rootNode, typename SelectorQueryTrait::OutputType& output) const
{
    for (Element& element : ElementTraversal::descendantsOf(rootNode)) {
        if (selectorListMatches<SelectorQueryTrait>(rootNode, element, output) && SelectorQueryTrait::shouldOnlyMatchFirstElement)
            return;
    }
}

// FIXME: Move the following helper functions, authorShadowRootOf, firstWithinTraversingShadowTree,
// nextTraversingShadowTree to the best place, e.g. NodeTraversal.
static ShadowRoot* authorShadowRootOf(const ContainerNode& node)
{
    if (!node.isElementNode() || !isShadowHost(&node))
        return 0;

    ElementShadow* shadow = toElement(node).shadow();
    ASSERT(shadow);
    for (ShadowRoot* shadowRoot = shadow->oldestShadowRoot(); shadowRoot; shadowRoot = shadowRoot->youngerShadowRoot()) {
        if (shadowRoot->type() == ShadowRootType::Open)
            return shadowRoot;
    }
    return 0;
}

static ContainerNode* firstWithinTraversingShadowTree(const ContainerNode& rootNode)
{
    if (ShadowRoot* shadowRoot = authorShadowRootOf(rootNode))
        return shadowRoot;
    return ElementTraversal::firstWithin(rootNode);
}

static ContainerNode* nextTraversingShadowTree(const ContainerNode& node, const ContainerNode* rootNode)
{
    if (ShadowRoot* shadowRoot = authorShadowRootOf(node))
        return shadowRoot;

    const ContainerNode* current = &node;
    while (current) {
        if (Element* next = ElementTraversal::next(*current, rootNode))
            return next;

        if (!current->isInShadowTree())
            return 0;

        ShadowRoot* shadowRoot = current->containingShadowRoot();
        if (shadowRoot == rootNode)
            return 0;
        if (ShadowRoot* youngerShadowRoot = shadowRoot->youngerShadowRoot()) {
            // Should not obtain any elements in user-agent shadow root.
            ASSERT(youngerShadowRoot->type() == ShadowRootType::Open);
            return youngerShadowRoot;
        }

        current = shadowRoot->host();
    }
    return 0;
}

template <typename SelectorQueryTrait>
void SelectorDataList::executeSlowTraversingShadowTree(ContainerNode& rootNode, typename SelectorQueryTrait::OutputType& output) const
{
    for (ContainerNode* node = firstWithinTraversingShadowTree(rootNode); node; node = nextTraversingShadowTree(*node, &rootNode)) {
        if (!node->isElementNode())
            continue;
        Element* element = toElement(node);
        if (selectorListMatches<SelectorQueryTrait>(rootNode, *element, output) && SelectorQueryTrait::shouldOnlyMatchFirstElement)
            return;
    }
}

const CSSSelector* SelectorDataList::selectorForIdLookup(const CSSSelector& firstSelector) const
{
    for (const CSSSelector* selector = &firstSelector; selector; selector = selector->tagHistory()) {
        if (selector->match() == CSSSelector::Id)
            return selector;
        if (selector->relation() != CSSSelector::SubSelector)
            break;
    }
    return 0;
}

template <typename SelectorQueryTrait>
void SelectorDataList::execute(ContainerNode& rootNode, typename SelectorQueryTrait::OutputType& output) const
{
    if (!canUseFastQuery(rootNode)) {
        if (m_needsUpdatedDistribution)
            rootNode.updateDistribution();
        if (m_crossesTreeBoundary) {
            executeSlowTraversingShadowTree<SelectorQueryTrait>(rootNode, output);
        } else {
            executeSlow<SelectorQueryTrait>(rootNode, output);
        }
        return;
    }

    ASSERT(m_selectors.size() == 1);

    const CSSSelector& selector = *m_selectors[0];
    const CSSSelector& firstSelector = selector;

    // Fast path for querySelector*('#id'), querySelector*('tag#id').
    if (const CSSSelector* idSelector = selectorForIdLookup(firstSelector)) {
        const AtomicString& idToMatch = idSelector->value();
        if (rootNode.treeScope().containsMultipleElementsWithId(idToMatch)) {
            const WillBeHeapVector<RawPtrWillBeMember<Element>>& elements = rootNode.treeScope().getAllElementsById(idToMatch);
            size_t count = elements.size();
            for (size_t i = 0; i < count; ++i) {
                Element& element = *elements[i];
                if (!(isTreeScopeRoot(rootNode) || element.isDescendantOf(&rootNode)))
                    continue;
                if (selectorMatches(selector, element, rootNode)) {
                    SelectorQueryTrait::appendElement(output, element);
                    if (SelectorQueryTrait::shouldOnlyMatchFirstElement)
                        return;
                }
            }
            return;
        }
        Element* element = rootNode.treeScope().getElementById(idToMatch);
        if (!element || !(isTreeScopeRoot(rootNode) || element->isDescendantOf(&rootNode)))
            return;
        if (selectorMatches(selector, *element, rootNode))
            SelectorQueryTrait::appendElement(output, *element);
        return;
    }

    if (!firstSelector.tagHistory()) {
        // Fast path for querySelector*('.foo'), and querySelector*('div').
        switch (firstSelector.match()) {
        case CSSSelector::Class:
            collectElementsByClassName<SelectorQueryTrait>(rootNode, firstSelector.value(), output);
            return;
        case CSSSelector::Tag:
            collectElementsByTagName<SelectorQueryTrait>(rootNode, firstSelector.tagQName(), output);
            return;
        default:
            break; // If we need another fast path, add here.
        }
    }

    findTraverseRootsAndExecute<SelectorQueryTrait>(rootNode, output);
}

PassOwnPtr<SelectorQuery> SelectorQuery::adopt(CSSSelectorList& selectorList)
{
    return adoptPtr(new SelectorQuery(selectorList));
}

SelectorQuery::SelectorQuery(CSSSelectorList& selectorList)
{
    m_selectorList.adopt(selectorList);
    m_selectors.initialize(m_selectorList);
}

bool SelectorQuery::matches(Element& element) const
{
    return m_selectors.matches(element);
}

Element* SelectorQuery::closest(Element& element) const
{
    return m_selectors.closest(element);
}

PassRefPtrWillBeRawPtr<StaticElementList> SelectorQuery::queryAll(ContainerNode& rootNode) const
{
    return m_selectors.queryAll(rootNode);
}

PassRefPtrWillBeRawPtr<Element> SelectorQuery::queryFirst(ContainerNode& rootNode) const
{
    return m_selectors.queryFirst(rootNode);
}

SelectorQuery* SelectorQueryCache::add(const AtomicString& selectors, const Document& document, ExceptionState& exceptionState)
{
    HashMap<AtomicString, OwnPtr<SelectorQuery>>::iterator it = m_entries.find(selectors);
    if (it != m_entries.end())
        return it->value.get();

    CSSSelectorList selectorList;
    CSSParser::parseSelector(CSSParserContext(document, 0), selectors, selectorList);

    if (!selectorList.first()) {
        exceptionState.throwDOMException(SyntaxError, "'" + selectors + "' is not a valid selector.");
        return 0;
    }

    // throw a NamespaceError if the selector includes any namespace prefixes.
    if (selectorList.selectorsNeedNamespaceResolution()) {
        exceptionState.throwDOMException(NamespaceError, "'" + selectors + "' contains namespaces, which are not supported.");
        return 0;
    }

    const unsigned maximumSelectorQueryCacheSize = 256;
    if (m_entries.size() == maximumSelectorQueryCacheSize)
        m_entries.remove(m_entries.begin());

    return m_entries.add(selectors, SelectorQuery::adopt(selectorList)).storedValue->value.get();
}

void SelectorQueryCache::invalidate()
{
    m_entries.clear();
}

}

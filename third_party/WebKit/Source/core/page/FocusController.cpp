/*
 * Copyright (C) 2006, 2007 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nuanti Ltd.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/page/FocusController.h"

#include "core/HTMLNames.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/Range.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/Editor.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/htmlediting.h" // For firstPositionInOrBeforeNode
#include "core/events/Event.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLAreaElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLPlugInElement.h"
#include "core/html/HTMLShadowElement.h"
#include "core/html/HTMLTextFormControlElement.h"
#include "core/input/EventHandler.h"
#include "core/page/ChromeClient.h"
#include "core/page/FrameTree.h"
#include "core/page/Page.h"
#include "core/frame/Settings.h"
#include "core/layout/HitTestResult.h"
#include "core/page/SpatialNavigation.h"
#include <limits>

namespace blink {

using namespace HTMLNames;

namespace {

inline bool isShadowInsertionPointFocusScopeOwner(Element& element)
{
    return isActiveShadowInsertionPoint(element) && toHTMLShadowElement(element).olderShadowRoot();
}

class FocusNavigationScope {
    STACK_ALLOCATED();
public:
    Node* rootNode() const;
    Element* owner() const;
    static FocusNavigationScope focusNavigationScopeOf(const Node&);
    static FocusNavigationScope ownedByNonFocusableFocusScopeOwner(Element&);
    static FocusNavigationScope ownedByShadowHost(const Element&);
    static FocusNavigationScope ownedByShadowInsertionPoint(HTMLShadowElement&);
    static FocusNavigationScope ownedByIFrame(const HTMLFrameOwnerElement&);

private:
    explicit FocusNavigationScope(TreeScope*);
    RawPtrWillBeMember<TreeScope> m_rootTreeScope;
};

FocusNavigationScope::FocusNavigationScope(TreeScope* treeScope)
    : m_rootTreeScope(treeScope)
{
    ASSERT(treeScope);
}

Node* FocusNavigationScope::rootNode() const
{
    return &m_rootTreeScope->rootNode();
}

Element* FocusNavigationScope::owner() const
{
    Node* root = rootNode();
    if (root->isShadowRoot()) {
        ShadowRoot* shadowRoot = toShadowRoot(root);
        return shadowRoot->isYoungest() ? shadowRoot->host() : shadowRoot->shadowInsertionPointOfYoungerShadowRoot();
    }
    // FIXME: Figure out the right thing for OOPI here.
    if (Frame* frame = root->document().frame())
        return frame->deprecatedLocalOwner();
    return nullptr;
}

FocusNavigationScope FocusNavigationScope::focusNavigationScopeOf(const Node& node)
{
    const Node* root = &node;
    for (const Node* n = &node; n; n = n->parentNode())
        root = n;
    // The result is not always a ShadowRoot nor a DocumentNode since
    // a starting node is in an orphaned tree in composed shadow tree.
    return FocusNavigationScope(&root->treeScope());
}

FocusNavigationScope FocusNavigationScope::ownedByNonFocusableFocusScopeOwner(Element& element)
{
    if (isShadowHost(element))
        return FocusNavigationScope::ownedByShadowHost(element);
    ASSERT(isShadowInsertionPointFocusScopeOwner(element));
    return FocusNavigationScope::ownedByShadowInsertionPoint(toHTMLShadowElement(element));
}

FocusNavigationScope FocusNavigationScope::ownedByShadowHost(const Element& element)
{
    ASSERT(isShadowHost(element));
    return FocusNavigationScope(element.shadow()->youngestShadowRoot());
}

FocusNavigationScope FocusNavigationScope::ownedByIFrame(const HTMLFrameOwnerElement& frame)
{
    ASSERT(frame.contentFrame());
    ASSERT(frame.contentFrame()->isLocalFrame());
    return FocusNavigationScope(toLocalFrame(frame.contentFrame())->document());
}

FocusNavigationScope FocusNavigationScope::ownedByShadowInsertionPoint(HTMLShadowElement& shadowInsertionPoint)
{
    ASSERT(isShadowInsertionPointFocusScopeOwner(shadowInsertionPoint));
    return FocusNavigationScope(shadowInsertionPoint.olderShadowRoot());
}

inline void dispatchBlurEvent(const Document& document, Element& focusedElement)
{
    focusedElement.dispatchBlurEvent(nullptr, WebFocusTypePage);
    if (focusedElement == document.focusedElement()) {
        focusedElement.dispatchFocusOutEvent(EventTypeNames::focusout, nullptr);
        if (focusedElement == document.focusedElement())
            focusedElement.dispatchFocusOutEvent(EventTypeNames::DOMFocusOut, nullptr);
    }
}

inline void dispatchFocusEvent(const Document& document, Element& focusedElement)
{
    focusedElement.dispatchFocusEvent(0, WebFocusTypePage);
    if (focusedElement == document.focusedElement()) {
        focusedElement.dispatchFocusInEvent(EventTypeNames::focusin, nullptr, WebFocusTypePage);
        if (focusedElement == document.focusedElement())
            focusedElement.dispatchFocusInEvent(EventTypeNames::DOMFocusIn, nullptr, WebFocusTypePage);
    }
}

inline void dispatchEventsOnWindowAndFocusedElement(Document* document, bool focused)
{
    ASSERT(document);
    // If we have a focused element we should dispatch blur on it before we blur the window.
    // If we have a focused element we should dispatch focus on it after we focus the window.
    // https://bugs.webkit.org/show_bug.cgi?id=27105

    if (document->focusedElement() && isHTMLPlugInElement(document->focusedElement()))
        toHTMLPlugInElement(document->focusedElement())->setPluginFocus(focused);

    // Do not fire events while modal dialogs are up.  See https://bugs.webkit.org/show_bug.cgi?id=33962
    if (Page* page = document->page()) {
        if (page->defersLoading())
            return;
    }

    if (!focused && document->focusedElement()) {
        RefPtrWillBeRawPtr<Element> focusedElement(document->focusedElement());
        focusedElement->setFocus(false);
        dispatchBlurEvent(*document, *focusedElement);
    }

    if (LocalDOMWindow* window = document->domWindow())
        window->dispatchEvent(Event::create(focused ? EventTypeNames::focus : EventTypeNames::blur));
    if (focused && document->focusedElement()) {
        RefPtrWillBeRawPtr<Element> focusedElement(document->focusedElement());
        focusedElement->setFocus(true);
        dispatchFocusEvent(*document, *focusedElement);
    }
}

inline bool hasCustomFocusLogic(const Element& element)
{
    return element.isHTMLElement() && toHTMLElement(element).hasCustomFocusLogic();
}

inline bool isShadowHostWithoutCustomFocusLogic(const Element& element)
{
    return isShadowHost(element) && !hasCustomFocusLogic(element);
}

#if ENABLE(ASSERT)
inline bool isNonFocusableShadowHost(const Node& node)
{
    if (!node.isElementNode())
        return false;
    const Element& element = toElement(node);
    return isShadowHostWithoutCustomFocusLogic(element) && !element.isFocusable();
}
#endif

inline bool isNonKeyboardFocusableShadowHost(const Element& element)
{
    return isShadowHostWithoutCustomFocusLogic(element) && !element.isKeyboardFocusable();
}

inline bool isKeyboardFocusableShadowHost(const Element& element)
{
    return isShadowHostWithoutCustomFocusLogic(element) && element.isKeyboardFocusable();
}

inline bool isNonFocusableFocusScopeOwner(Element& element)
{
    return isNonKeyboardFocusableShadowHost(element) || isShadowInsertionPointFocusScopeOwner(element);
}

inline bool isShadowHostDelegatesFocus(const Element& element)
{
    return element.shadowRoot() && element.shadowRoot()->delegatesFocus();
}

inline int adjustedTabIndex(Node& node)
{
    return node.isElementNode() && isNonFocusableFocusScopeOwner(toElement(node)) ? 0 : node.tabIndex();
}

inline bool shouldVisit(Node& node)
{
    if (!node.isElementNode())
        return false;
    Element& element = toElement(node);
    return element.isKeyboardFocusable() || isNonFocusableFocusScopeOwner(element);
}

Element* findElementWithExactTabIndex(Node* start, int tabIndex, WebFocusType type)
{
    // Search is inclusive of start
    for (Node* node = start; node; node = type == WebFocusTypeForward ? ElementTraversal::next(*node) : ElementTraversal::previous(*node)) {
        if (shouldVisit(*node) && adjustedTabIndex(*node) == tabIndex)
            return toElement(node);
    }
    return nullptr;
}

Element* nextElementWithGreaterTabIndex(Node* start, int tabIndex)
{
    // Search is inclusive of start
    int winningTabIndex = std::numeric_limits<short>::max() + 1;
    Node* winner = nullptr;
    for (Node& node : NodeTraversal::startsAt(start)) {
        int currentTabIndex = adjustedTabIndex(node);
        if (shouldVisit(node) && currentTabIndex > tabIndex && currentTabIndex < winningTabIndex) {
            winner = &node;
            winningTabIndex = currentTabIndex;
        }
    }
    ASSERT(!winner || winner->isElementNode());
    return toElement(winner);
}

Element* previousElementWithLowerTabIndex(Node* start, int tabIndex)
{
    // Search is inclusive of start
    int winningTabIndex = 0;
    Node* winner = nullptr;
    for (Node* node = start; node; node = ElementTraversal::previous(*node)) {
        int currentTabIndex = adjustedTabIndex(*node);
        if (shouldVisit(*node) && currentTabIndex < tabIndex && currentTabIndex > winningTabIndex) {
            winner = node;
            winningTabIndex = currentTabIndex;
        }
    }
    ASSERT(!winner || winner->isElementNode());
    return toElement(winner);
}

Element* nextFocusableElement(const FocusNavigationScope& scope, Node* start)
{
    if (start) {
        int tabIndex = adjustedTabIndex(*start);
        // If a node is excluded from the normal tabbing cycle, the next focusable node is determined by tree order
        if (tabIndex < 0) {
            for (Node& node : NodeTraversal::startsAfter(*start)) {
                if (shouldVisit(node) && adjustedTabIndex(node) >= 0)
                    return &toElement(node);
            }
        } else {
            // First try to find a node with the same tabindex as start that comes after start in the scope.
            if (Element* winner = findElementWithExactTabIndex(ElementTraversal::next(*start), tabIndex, WebFocusTypeForward))
                return winner;
        }
        if (!tabIndex) {
            // We've reached the last node in the document with a tabindex of 0. This is the end of the tabbing order.
            return nullptr;
        }
    }

    // Look for the first node in the scope that:
    // 1) has the lowest tabindex that is higher than start's tabindex (or 0, if start is null), and
    // 2) comes first in the scope, if there's a tie.
    if (Element* winner = nextElementWithGreaterTabIndex(scope.rootNode(), start ? adjustedTabIndex(*start) : 0))
        return winner;

    // There are no nodes with a tabindex greater than start's tabindex,
    // so find the first node with a tabindex of 0.
    return findElementWithExactTabIndex(scope.rootNode(), 0, WebFocusTypeForward);
}

Element* previousFocusableElement(const FocusNavigationScope& scope, Node* start)
{
    Node* last = nullptr;
    for (Node* node = scope.rootNode(); node; node = node->lastChild())
        last = node;
    ASSERT(last);

    // First try to find the last node in the scope that comes before start and has the same tabindex as start.
    // If start is null, find the last node in the scope with a tabindex of 0.
    Node* startingNode;
    int startingTabIndex;
    if (start) {
        startingNode = ElementTraversal::previous(*start);
        startingTabIndex = adjustedTabIndex(*start);
    } else {
        startingNode = last;
        startingTabIndex = 0;
    }

    // However, if a node is excluded from the normal tabbing cycle, the previous focusable node is determined by tree order
    if (startingTabIndex < 0) {
        for (Node* node = startingNode; node; node = ElementTraversal::previous(*node)) {
            if (shouldVisit(*node) && adjustedTabIndex(*node) >= 0)
                return toElement(node);
        }
    } else {
        if (Element* winner = findElementWithExactTabIndex(startingNode, startingTabIndex, WebFocusTypeBackward))
            return winner;
    }

    // There are no nodes before start with the same tabindex as start, so look for a node that:
    // 1) has the highest non-zero tabindex (that is less than start's tabindex), and
    // 2) comes last in the scope, if there's a tie.
    startingTabIndex = (start && startingTabIndex) ? startingTabIndex : std::numeric_limits<short>::max();
    return previousElementWithLowerTabIndex(last, startingTabIndex);
}

// Searches through the given tree scope, starting from start node, for the next/previous
// selectable element that comes after/before start node.
// The order followed is as specified in the HTML spec[1], which is elements with tab indexes
// first (from lowest to highest), and then elements without tab indexes (in document order).
// The search algorithm also conforms the Shadow DOM spec[2], which inserts sequence in a shadow
// tree into its host.
//
// @param start The node from which to start searching. The node after this will be focused.
//        May be null.
// @return The focus element that comes after/before start node.
//
// [1] https://html.spec.whatwg.org/multipage/interaction.html#sequential-focus-navigation
// [2] https://w3c.github.io/webcomponents/spec/shadow/#focus-navigation
inline Element* findFocusableElementInternal(WebFocusType type, const FocusNavigationScope& scope, Node* node)
{
    Element* found = (type == WebFocusTypeForward) ? nextFocusableElement(scope, node) : previousFocusableElement(scope, node);
    return found;
}

Element* findFocusableElementRecursivelyForward(const FocusNavigationScope& scope, Node* start)
{
    // Starting node is exclusive.
    Element* found = findFocusableElementInternal(WebFocusTypeForward, scope, start);
    while (found) {
        if (isShadowHostDelegatesFocus(*found)) {
            // If tabindex is positive, find focusable node inside its shadow tree.
            if (found->tabIndex() >= 0 && isShadowHostWithoutCustomFocusLogic(*found)) {
                FocusNavigationScope innerScope = FocusNavigationScope::ownedByShadowHost(*found);
                if (Element* foundInInnerFocusScope = findFocusableElementRecursivelyForward(innerScope, nullptr))
                    return foundInInnerFocusScope;
            }
            // Skip to the next node in the same scope.
            found = findFocusableElementInternal(WebFocusTypeForward, scope, found);
            continue;
        }
        if (!isNonFocusableFocusScopeOwner(*found))
            return found;

        // Now |found| is on a non focusable scope owner (either shadow host or <shadow>)
        // Find inside the inward scope and return it if found. Otherwise continue searching in the same
        // scope.
        FocusNavigationScope innerScope = FocusNavigationScope::ownedByNonFocusableFocusScopeOwner(*found);
        if (Element* foundInInnerFocusScope = findFocusableElementRecursivelyForward(innerScope, nullptr))
            return foundInInnerFocusScope;

        found = findFocusableElementInternal(WebFocusTypeForward, scope, found);
    }
    return nullptr;
}

Element* findFocusableElementRecursivelyBackward(const FocusNavigationScope& scope, Node* start)
{
    // Starting node is exclusive.
    Element* found = findFocusableElementInternal(WebFocusTypeBackward, scope, start);
    while (found) {
        // Now |found| is on a focusable shadow host.
        // Find inside shadow backwards. If any focusable element is found, return it, otherwise return
        // the host itself.
        if (isKeyboardFocusableShadowHost(*found)) {
            FocusNavigationScope innerScope = FocusNavigationScope::ownedByShadowHost(*found);
            Element* foundInInnerFocusScope = findFocusableElementRecursivelyBackward(innerScope, nullptr);
            if (foundInInnerFocusScope)
                return foundInInnerFocusScope;
            if (isShadowHostDelegatesFocus(*found)) {
                found = findFocusableElementInternal(WebFocusTypeBackward, scope, found);
                continue;
            }
            return found;
        }

        // If delegatesFocus is true and tabindex is negative, skip the whole shadow tree under the
        // shadow host.
        if (isShadowHostDelegatesFocus(*found) && found->tabIndex() < 0) {
            found = findFocusableElementInternal(WebFocusTypeBackward, scope, found);
            continue;
        }

        // Now |found| is on a non focusable scope owner (either shadow host or <shadow>).
        // Find focusable node in descendant scope. If not found, find next focusable node within the
        // current scope.
        if (isNonFocusableFocusScopeOwner(*found)) {
            FocusNavigationScope innerScope = FocusNavigationScope::ownedByNonFocusableFocusScopeOwner(*found);
            Element* foundInInnerFocusScope = findFocusableElementRecursivelyBackward(innerScope, nullptr);
            if (foundInInnerFocusScope)
                return foundInInnerFocusScope;
            found = findFocusableElementInternal(WebFocusTypeBackward, scope, found);
            continue;
        }
        if (!isShadowHostDelegatesFocus(*found))
            return found;
        found = findFocusableElementInternal(WebFocusTypeBackward, scope, found);
    }
    return nullptr;
}

Element* findFocusableElementRecursively(WebFocusType type, const FocusNavigationScope& scope, Node* start)
{
    return (type == WebFocusTypeForward) ?
        findFocusableElementRecursivelyForward(scope, start) :
        findFocusableElementRecursivelyBackward(scope, start);
}

Element* findFocusableElementDescendingDownIntoFrameDocument(WebFocusType type, Element* element)
{
    // The element we found might be a HTMLFrameOwnerElement, so descend down the tree until we find either:
    // 1) a focusable element, or
    // 2) the deepest-nested HTMLFrameOwnerElement.
    while (element && element->isFrameOwnerElement()) {
        HTMLFrameOwnerElement& owner = toHTMLFrameOwnerElement(*element);
        if (!owner.contentFrame() || !owner.contentFrame()->isLocalFrame())
            break;
        toLocalFrame(owner.contentFrame())->document()->updateLayoutIgnorePendingStylesheets();
        Element* foundElement = findFocusableElementInternal(type, FocusNavigationScope::ownedByIFrame(owner), nullptr);
        if (!foundElement)
            break;
        ASSERT(element != foundElement);
        element = foundElement;
    }
    return element;
}

Element* findFocusableElementAcrossFocusScopesForward(const FocusNavigationScope& scope, Node* currentNode)
{
    ASSERT(!currentNode || !isNonFocusableShadowHost(*currentNode));
    Element* found;
    if (currentNode && currentNode->isElementNode() && isShadowHostWithoutCustomFocusLogic(*toElement(currentNode))) {
        FocusNavigationScope innerScope = FocusNavigationScope::ownedByShadowHost(*toElement(currentNode));
        Element* foundInInnerFocusScope = findFocusableElementRecursivelyForward(innerScope, nullptr);
        found = foundInInnerFocusScope ? foundInInnerFocusScope : findFocusableElementRecursivelyForward(scope, currentNode);
    } else {
        found = findFocusableElementRecursivelyForward(scope, currentNode);
    }

    // If there's no focusable node to advance to, move up the focus scopes until we find one.
    FocusNavigationScope currentScope = scope;
    while (!found) {
        Element* owner = currentScope.owner();
        if (!owner)
            break;
        currentScope = FocusNavigationScope::focusNavigationScopeOf(*owner);
        found = findFocusableElementRecursivelyForward(currentScope, owner);
    }
    return findFocusableElementDescendingDownIntoFrameDocument(WebFocusTypeForward, found);
}

Element* findFocusableElementAcrossFocusScopesBackward(const FocusNavigationScope& scope, Node* currentNode)
{
    ASSERT(!currentNode || !isNonFocusableShadowHost(*currentNode));
    Element* found = findFocusableElementRecursivelyBackward(scope, currentNode);

    // If there's no focusable node to advance to, move up the focus scopes until we find one.
    FocusNavigationScope currentScope = scope;
    while (!found) {
        Element* owner = currentScope.owner();
        if (!owner)
            break;
        currentScope = FocusNavigationScope::focusNavigationScopeOf(*owner);
        if (isKeyboardFocusableShadowHost(*owner) && !isShadowHostDelegatesFocus(*owner)) {
            found = owner;
            break;
        }
        found = findFocusableElementRecursivelyBackward(currentScope, owner);
    }
    return findFocusableElementDescendingDownIntoFrameDocument(WebFocusTypeBackward, found);
}

Element* findFocusableElementAcrossFocusScopes(WebFocusType type, const FocusNavigationScope& scope, Node* currentNode)
{
    return (type == WebFocusTypeForward) ?
        findFocusableElementAcrossFocusScopesForward(scope, currentNode) :
        findFocusableElementAcrossFocusScopesBackward(scope, currentNode);
}

} // anonymous namespace

FocusController::FocusController(Page* page)
    : m_page(page)
    , m_isActive(false)
    , m_isFocused(false)
    , m_isChangingFocusedFrame(false)
{
}

PassOwnPtrWillBeRawPtr<FocusController> FocusController::create(Page* page)
{
    return adoptPtrWillBeNoop(new FocusController(page));
}

void FocusController::setFocusedFrame(PassRefPtrWillBeRawPtr<Frame> frame)
{
    ASSERT(!frame || frame->page() == m_page);
    if (m_focusedFrame == frame || m_isChangingFocusedFrame)
        return;

    m_isChangingFocusedFrame = true;

    RefPtrWillBeRawPtr<LocalFrame> oldFrame = (m_focusedFrame && m_focusedFrame->isLocalFrame()) ? toLocalFrame(m_focusedFrame.get()) : nullptr;

    RefPtrWillBeRawPtr<LocalFrame> newFrame = (frame && frame->isLocalFrame()) ? toLocalFrame(frame.get()) : nullptr;

    m_focusedFrame = frame.get();

    // Now that the frame is updated, fire events and update the selection focused states of both frames.
    if (oldFrame && oldFrame->view()) {
        oldFrame->selection().setFocused(false);
        oldFrame->domWindow()->dispatchEvent(Event::create(EventTypeNames::blur));
    }

    if (newFrame && newFrame->view() && isFocused()) {
        newFrame->selection().setFocused(true);
        newFrame->domWindow()->dispatchEvent(Event::create(EventTypeNames::focus));
    }

    m_isChangingFocusedFrame = false;

    m_page->chromeClient().focusedFrameChanged(newFrame.get());
}

void FocusController::focusDocumentView(PassRefPtrWillBeRawPtr<Frame> frame)
{
    ASSERT(!frame || frame->page() == m_page);
    if (m_focusedFrame == frame)
        return;

    RefPtrWillBeRawPtr<LocalFrame> focusedFrame = (m_focusedFrame && m_focusedFrame->isLocalFrame()) ? toLocalFrame(m_focusedFrame.get()) : nullptr;
    if (focusedFrame && focusedFrame->view()) {
        RefPtrWillBeRawPtr<Document> document = focusedFrame->document();
        Element* focusedElement = document ? document->focusedElement() : nullptr;
        if (focusedElement)
            dispatchBlurEvent(*document, *focusedElement);
    }

    RefPtrWillBeRawPtr<LocalFrame> newFocusedFrame = (frame && frame->isLocalFrame()) ? toLocalFrame(frame.get()) : nullptr;
    if (newFocusedFrame && newFocusedFrame->view()) {
        RefPtrWillBeRawPtr<Document> document = newFocusedFrame->document();
        Element* focusedElement = document ? document->focusedElement() : nullptr;
        if (focusedElement)
            dispatchFocusEvent(*document, *focusedElement);
    }

    setFocusedFrame(frame);
}

Frame* FocusController::focusedOrMainFrame() const
{
    if (Frame* frame = focusedFrame())
        return frame;

    // FIXME: This is a temporary hack to ensure that we return a LocalFrame, even when the mainFrame is remote.
    // FocusController needs to be refactored to deal with RemoteFrames cross-process focus transfers.
    for (Frame* frame = m_page->mainFrame()->tree().top(); frame; frame = frame->tree().traverseNext()) {
        if (frame->isLocalRoot())
            return frame;
    }

    return m_page->mainFrame();
}

void FocusController::setFocused(bool focused)
{
    if (isFocused() == focused)
        return;

    m_isFocused = focused;

    if (!m_isFocused && focusedOrMainFrame()->isLocalFrame())
        toLocalFrame(focusedOrMainFrame())->eventHandler().stopAutoscroll();

    if (!m_focusedFrame)
        setFocusedFrame(m_page->mainFrame());

    // setFocusedFrame above might reject to update m_focusedFrame, or
    // m_focusedFrame might be changed by blur/focus event handlers.
    if (m_focusedFrame && m_focusedFrame->isLocalFrame() && toLocalFrame(m_focusedFrame.get())->view()) {
        toLocalFrame(m_focusedFrame.get())->selection().setFocused(focused);
        dispatchEventsOnWindowAndFocusedElement(toLocalFrame(m_focusedFrame.get())->document(), focused);
    }
}

bool FocusController::setInitialFocus(WebFocusType type)
{
    bool didAdvanceFocus = advanceFocus(type, true);

    // If focus is being set initially, accessibility needs to be informed that system focus has moved
    // into the web area again, even if focus did not change within WebCore. PostNotification is called instead
    // of handleFocusedUIElementChanged, because this will send the notification even if the element is the same.
    if (focusedOrMainFrame()->isLocalFrame()) {
        Document* document = toLocalFrame(focusedOrMainFrame())->document();
        if (AXObjectCache* cache = document->existingAXObjectCache())
            cache->handleInitialFocus();
    }

    return didAdvanceFocus;
}

bool FocusController::advanceFocus(WebFocusType type, bool initialFocus)
{
    switch (type) {
    case WebFocusTypeForward:
    case WebFocusTypeBackward:
        return advanceFocusInDocumentOrder(type, initialFocus);
    case WebFocusTypeLeft:
    case WebFocusTypeRight:
    case WebFocusTypeUp:
    case WebFocusTypeDown:
        return advanceFocusDirectionally(type);
    default:
        ASSERT_NOT_REACHED();
    }

    return false;
}

bool FocusController::advanceFocusInDocumentOrder(WebFocusType type, bool initialFocus)
{
    // FIXME: Focus advancement won't work with externally rendered frames until after
    // inter-frame focus control is moved out of Blink.
    if (!focusedOrMainFrame()->isLocalFrame())
        return false;
    LocalFrame* frame = toLocalFrame(focusedOrMainFrame());
    ASSERT(frame);
    Document* document = frame->document();

    Node* currentNode = document->focusedElement();
    // FIXME: Not quite correct when it comes to focus transitions leaving/entering the WebView itself
    bool caretBrowsing = frame->settings() && frame->settings()->caretBrowsingEnabled();

    if (caretBrowsing && !currentNode)
        currentNode = frame->selection().start().deprecatedNode();

    document->updateLayoutIgnorePendingStylesheets();

    RefPtrWillBeRawPtr<Element> element = findFocusableElementAcrossFocusScopes(type, FocusNavigationScope::focusNavigationScopeOf(currentNode ? *currentNode : *document), currentNode);

    if (!element) {
        // We didn't find an element to focus, so we should try to pass focus to Chrome.
        if (!initialFocus && m_page->chromeClient().canTakeFocus(type)) {
            document->setFocusedElement(nullptr);
            setFocusedFrame(nullptr);
            m_page->chromeClient().takeFocus(type);
            return true;
        }

        // Chrome doesn't want focus, so we should wrap focus.
        if (!m_page->mainFrame()->isLocalFrame())
            return false;
        element = findFocusableElementRecursively(type, FocusNavigationScope::focusNavigationScopeOf(*m_page->deprecatedLocalMainFrame()->document()), nullptr);
        element = findFocusableElementDescendingDownIntoFrameDocument(type, element.get());

        if (!element)
            return false;
    }

    ASSERT(element);

    if (element == document->focusedElement())
        // Focus wrapped around to the same node.
        return true;

    if (element->isFrameOwnerElement() && (!isHTMLPlugInElement(*element) || !element->isKeyboardFocusable())) {
        // We focus frames rather than frame owners.
        // FIXME: We should not focus frames that have no scrollbars, as focusing them isn't useful to the user.
        HTMLFrameOwnerElement* owner = toHTMLFrameOwnerElement(element);
        if (!owner->contentFrame())
            return false;

        document->setFocusedElement(nullptr);
        setFocusedFrame(owner->contentFrame());
        return true;
    }

    // FIXME: It would be nice to just be able to call setFocusedElement(node)
    // here, but we can't do that because some elements (e.g. HTMLInputElement
    // and HTMLTextAreaElement) do extra work in their focus() methods.
    Document& newDocument = element->document();

    if (&newDocument != document) {
        // Focus is going away from this document, so clear the focused node.
        document->setFocusedElement(nullptr);
    }

    setFocusedFrame(newDocument.frame());

    if (caretBrowsing) {
        Position position = firstPositionInOrBeforeNode(element.get());
        VisibleSelection newSelection(position, position, DOWNSTREAM);
        frame->selection().setSelection(newSelection);
    }

    element->focus(false, type);
    return true;
}

Element* FocusController::findFocusableElement(WebFocusType type, Node& node)
{
    // FIXME: No spacial navigation code yet.
    ASSERT(type == WebFocusTypeForward || type == WebFocusTypeBackward);
    Element* found = findFocusableElementAcrossFocusScopes(type, FocusNavigationScope::focusNavigationScopeOf(node), &node);
    return found;
}

static bool relinquishesEditingFocus(const Element& element)
{
    ASSERT(element.hasEditableStyle());
    return element.document().frame() && element.rootEditableElement();
}

static void clearSelectionIfNeeded(LocalFrame* oldFocusedFrame, LocalFrame* newFocusedFrame, Element* newFocusedElement)
{
    if (!oldFocusedFrame || !newFocusedFrame)
        return;

    if (oldFocusedFrame->document() != newFocusedFrame->document())
        return;

    FrameSelection& selection = oldFocusedFrame->selection();
    if (selection.isNone())
        return;

    bool caretBrowsing = oldFocusedFrame->settings()->caretBrowsingEnabled();
    if (caretBrowsing)
        return;

    Node* selectionStartNode = selection.selection().start().deprecatedNode();
    if (selectionStartNode == newFocusedElement || selectionStartNode->isDescendantOf(newFocusedElement))
        return;

    if (!enclosingTextFormControl(selectionStartNode))
        return;

    if (selectionStartNode->isInShadowTree() && selectionStartNode->shadowHost() == newFocusedElement)
        return;

    selection.clear();
}

bool FocusController::setFocusedElement(Element* element, PassRefPtrWillBeRawPtr<Frame> newFocusedFrame, WebFocusType type)
{
    RefPtrWillBeRawPtr<LocalFrame> oldFocusedFrame = toLocalFrame(focusedFrame());
    RefPtrWillBeRawPtr<Document> oldDocument = oldFocusedFrame ? oldFocusedFrame->document() : nullptr;

    Element* oldFocusedElement = oldDocument ? oldDocument->focusedElement() : nullptr;
    if (element && oldFocusedElement == element)
        return true;

    // FIXME: Might want to disable this check for caretBrowsing
    if (oldFocusedElement && oldFocusedElement->isRootEditableElement() && !relinquishesEditingFocus(*oldFocusedElement))
        return false;

    m_page->chromeClient().willSetInputMethodState();

    RefPtrWillBeRawPtr<Document> newDocument = nullptr;
    if (element)
        newDocument = &element->document();
    else if (newFocusedFrame && newFocusedFrame->isLocalFrame())
        newDocument = toLocalFrame(newFocusedFrame.get())->document();

    if (newDocument && oldDocument == newDocument && newDocument->focusedElement() == element)
        return true;

    clearSelectionIfNeeded(oldFocusedFrame.get(), toLocalFrame(newFocusedFrame.get()), element);

    if (oldDocument && oldDocument != newDocument)
        oldDocument->setFocusedElement(nullptr);

    if (newFocusedFrame && !newFocusedFrame->page()) {
        setFocusedFrame(nullptr);
        return false;
    }
    setFocusedFrame(newFocusedFrame);

    // Setting the focused node can result in losing our last reft to node when JS event handlers fire.
    RefPtrWillBeRawPtr<Element> protect = element;
    ALLOW_UNUSED_LOCAL(protect);
    if (newDocument) {
        bool successfullyFocused = newDocument->setFocusedElement(element, type);
        if (!successfullyFocused)
            return false;
    }

    return true;
}

void FocusController::setActive(bool active)
{
    if (m_isActive == active)
        return;

    m_isActive = active;

    Frame* frame = focusedOrMainFrame();
    if (frame->isLocalFrame()) {
        // Invalidate all custom scrollbars because they support the CSS
        // window-active attribute. This should be applied to the entire page so
        // we invalidate from the root FrameView instead of just the focused.
        if (FrameView* view = toLocalFrame(frame)->localFrameRoot()->document()->view())
            view->invalidateAllCustomScrollbarsOnActiveChanged();
        toLocalFrame(frame)->selection().pageActivationChanged();
    }
}

static void updateFocusCandidateIfNeeded(WebFocusType type, const FocusCandidate& current, FocusCandidate& candidate, FocusCandidate& closest)
{
    ASSERT(candidate.visibleNode->isElementNode());
    ASSERT(candidate.visibleNode->layoutObject());

    // Ignore iframes that don't have a src attribute
    if (frameOwnerElement(candidate) && (!frameOwnerElement(candidate)->contentFrame() || candidate.rect.isEmpty()))
        return;

    // Ignore off screen child nodes of containers that do not scroll (overflow:hidden)
    if (candidate.isOffscreen && !canBeScrolledIntoView(type, candidate))
        return;

    distanceDataForNode(type, current, candidate);
    if (candidate.distance == maxDistance())
        return;

    if (candidate.isOffscreenAfterScrolling)
        return;

    if (closest.isNull()) {
        closest = candidate;
        return;
    }

    LayoutRect intersectionRect = intersection(candidate.rect, closest.rect);
    if (!intersectionRect.isEmpty() && !areElementsOnSameLine(closest, candidate)
        && intersectionRect == candidate.rect) {
        // If 2 nodes are intersecting, do hit test to find which node in on top.
        LayoutUnit x = intersectionRect.x() + intersectionRect.width() / 2;
        LayoutUnit y = intersectionRect.y() + intersectionRect.height() / 2;
        if (!candidate.visibleNode->document().page()->mainFrame()->isLocalFrame())
            return;
        HitTestResult result = candidate.visibleNode->document().page()->deprecatedLocalMainFrame()->eventHandler().hitTestResultAtPoint(IntPoint(x, y), HitTestRequest::ReadOnly | HitTestRequest::Active | HitTestRequest::IgnoreClipping);
        if (candidate.visibleNode->contains(result.innerNode())) {
            closest = candidate;
            return;
        }
        if (closest.visibleNode->contains(result.innerNode()))
            return;
    }

    if (candidate.distance < closest.distance)
        closest = candidate;
}

void FocusController::findFocusCandidateInContainer(Node& container, const LayoutRect& startingRect, WebFocusType type, FocusCandidate& closest)
{
    Element* focusedElement = (focusedFrame() && toLocalFrame(focusedFrame())->document()) ? toLocalFrame(focusedFrame())->document()->focusedElement() : nullptr;

    Element* element = ElementTraversal::firstWithin(container);
    FocusCandidate current;
    current.rect = startingRect;
    current.focusableNode = focusedElement;
    current.visibleNode = focusedElement;

    for (; element; element = (element->isFrameOwnerElement() || canScrollInDirection(element, type))
        ? ElementTraversal::nextSkippingChildren(*element, &container)
        : ElementTraversal::next(*element, &container)) {
        if (element == focusedElement)
            continue;

        if (!element->isKeyboardFocusable() && !element->isFrameOwnerElement() && !canScrollInDirection(element, type))
            continue;

        FocusCandidate candidate = FocusCandidate(element, type);
        if (candidate.isNull())
            continue;

        candidate.enclosingScrollableBox = &container;
        updateFocusCandidateIfNeeded(type, current, candidate, closest);
    }
}

bool FocusController::advanceFocusDirectionallyInContainer(Node* container, const LayoutRect& startingRect, WebFocusType type)
{
    if (!container)
        return false;

    LayoutRect newStartingRect = startingRect;

    if (startingRect.isEmpty())
        newStartingRect = virtualRectForDirection(type, nodeRectInAbsoluteCoordinates(container));

    // Find the closest node within current container in the direction of the navigation.
    FocusCandidate focusCandidate;
    findFocusCandidateInContainer(*container, newStartingRect, type, focusCandidate);

    if (focusCandidate.isNull()) {
        // Nothing to focus, scroll if possible.
        // NOTE: If no scrolling is performed (i.e. scrollInDirection returns false), the
        // spatial navigation algorithm will skip this container.
        return scrollInDirection(container, type);
    }

    HTMLFrameOwnerElement* frameElement = frameOwnerElement(focusCandidate);
    // If we have an iframe without the src attribute, it will not have a contentFrame().
    // We ASSERT here to make sure that
    // updateFocusCandidateIfNeeded() will never consider such an iframe as a candidate.
    ASSERT(!frameElement || frameElement->contentFrame());
    if (frameElement && frameElement->contentFrame()->isLocalFrame()) {
        if (focusCandidate.isOffscreenAfterScrolling) {
            scrollInDirection(&focusCandidate.visibleNode->document(), type);
            return true;
        }
        // Navigate into a new frame.
        LayoutRect rect;
        Element* focusedElement = toLocalFrame(focusedOrMainFrame())->document()->focusedElement();
        if (focusedElement && !hasOffscreenRect(focusedElement))
            rect = nodeRectInAbsoluteCoordinates(focusedElement, true /* ignore border */);
        toLocalFrame(frameElement->contentFrame())->document()->updateLayoutIgnorePendingStylesheets();
        if (!advanceFocusDirectionallyInContainer(toLocalFrame(frameElement->contentFrame())->document(), rect, type)) {
            // The new frame had nothing interesting, need to find another candidate.
            return advanceFocusDirectionallyInContainer(container, nodeRectInAbsoluteCoordinates(focusCandidate.visibleNode, true), type);
        }
        return true;
    }

    if (canScrollInDirection(focusCandidate.visibleNode, type)) {
        if (focusCandidate.isOffscreenAfterScrolling) {
            scrollInDirection(focusCandidate.visibleNode, type);
            return true;
        }
        // Navigate into a new scrollable container.
        LayoutRect startingRect;
        Element* focusedElement = toLocalFrame(focusedOrMainFrame())->document()->focusedElement();
        if (focusedElement && !hasOffscreenRect(focusedElement))
            startingRect = nodeRectInAbsoluteCoordinates(focusedElement, true);
        return advanceFocusDirectionallyInContainer(focusCandidate.visibleNode, startingRect, type);
    }
    if (focusCandidate.isOffscreenAfterScrolling) {
        Node* container = focusCandidate.enclosingScrollableBox;
        scrollInDirection(container, type);
        return true;
    }

    // We found a new focus node, navigate to it.
    Element* element = toElement(focusCandidate.focusableNode);
    ASSERT(element);

    element->focus(false, type);
    return true;
}

bool FocusController::advanceFocusDirectionally(WebFocusType type)
{
    // FIXME: Directional focus changes don't yet work with RemoteFrames.
    if (!focusedOrMainFrame()->isLocalFrame())
        return false;
    LocalFrame* curFrame = toLocalFrame(focusedOrMainFrame());
    ASSERT(curFrame);

    Document* focusedDocument = curFrame->document();
    if (!focusedDocument)
        return false;

    Element* focusedElement = focusedDocument->focusedElement();
    Node* container = focusedDocument;

    if (container->isDocumentNode())
        toDocument(container)->updateLayoutIgnorePendingStylesheets();

    // Figure out the starting rect.
    LayoutRect startingRect;
    if (focusedElement) {
        if (!hasOffscreenRect(focusedElement)) {
            container = scrollableEnclosingBoxOrParentFrameForNodeInDirection(type, focusedElement);
            startingRect = nodeRectInAbsoluteCoordinates(focusedElement, true /* ignore border */);
        } else if (isHTMLAreaElement(*focusedElement)) {
            HTMLAreaElement& area = toHTMLAreaElement(*focusedElement);
            container = scrollableEnclosingBoxOrParentFrameForNodeInDirection(type, area.imageElement());
            startingRect = virtualRectForAreaElementAndDirection(area, type);
        }
    }

    bool consumed = false;
    do {
        consumed = advanceFocusDirectionallyInContainer(container, startingRect, type);
        startingRect = nodeRectInAbsoluteCoordinates(container, true /* ignore border */);
        container = scrollableEnclosingBoxOrParentFrameForNodeInDirection(type, container);
        if (container && container->isDocumentNode())
            toDocument(container)->updateLayoutIgnorePendingStylesheets();
    } while (!consumed && container);

    return consumed;
}

DEFINE_TRACE(FocusController)
{
    visitor->trace(m_page);
    visitor->trace(m_focusedFrame);
}

} // namespace blink

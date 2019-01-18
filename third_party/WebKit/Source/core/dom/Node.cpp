/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
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
#include "core/dom/Node.h"

#include "bindings/core/v8/DOMDataStore.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/V8DOMWrapper.h"
#include "core/HTMLNames.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/Attr.h"
#include "core/dom/Attribute.h"
#include "core/dom/ChildListMutationScope.h"
#include "core/dom/ChildNodeList.h"
#include "core/dom/DOMNodeIds.h"
#include "core/dom/Document.h"
#include "core/dom/DocumentFragment.h"
#include "core/dom/DocumentMarkerController.h"
#include "core/dom/DocumentType.h"
#include "core/dom/Element.h"
#include "core/dom/ElementRareData.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/LayoutTreeBuilderTraversal.h"
#include "core/dom/NodeRareData.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/ProcessingInstruction.h"
#include "core/dom/Range.h"
#include "core/dom/StaticNodeList.h"
#include "core/dom/StyleEngine.h"
#include "core/dom/TemplateContentDocumentFragment.h"
#include "core/dom/Text.h"
#include "core/dom/TreeScopeAdopter.h"
#include "core/dom/UserActionElementSet.h"
#include "core/dom/shadow/ComposedTreeTraversal.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/dom/shadow/InsertionPoint.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/htmlediting.h"
#include "core/events/Event.h"
#include "core/events/EventDispatchMediator.h"
#include "core/events/EventDispatcher.h"
#include "core/events/EventListener.h"
#include "core/events/GestureEvent.h"
#include "core/events/KeyboardEvent.h"
#include "core/events/MouseEvent.h"
#include "core/events/MutationEvent.h"
#include "core/events/PointerEvent.h"
#include "core/events/TextEvent.h"
#include "core/events/TouchEvent.h"
#include "core/events/UIEvent.h"
#include "core/events/WheelEvent.h"
#include "core/frame/EventHandlerRegistry.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLDialogElement.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/input/EventHandler.h"
#include "core/layout/LayoutBox.h"
#include "core/page/ContextMenuController.h"
#include "core/page/Page.h"
#include "core/page/ChromeClient.h"
#include "core/svg/graphics/SVGImage.h"
#include "platform/EventDispatchForbiddenScope.h"
#include "platform/TraceEvent.h"
#include "platform/TracedValue.h"
#include "wtf/HashSet.h"
#include "wtf/Partitions.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefCountedLeakCounter.h"
#include "wtf/Vector.h"
#include "wtf/text/CString.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

using namespace HTMLNames;

struct SameSizeAsNode : NODE_BASE_CLASSES {
    uint32_t m_nodeFlags;
    void* m_pointer[5];
};

static_assert(sizeof(Node) <= sizeof(SameSizeAsNode), "Node should stay small");

#if !ENABLE(OILPAN)
void* Node::operator new(size_t size)
{
    ASSERT(isMainThread());
    return partitionAlloc(WTF::Partitions::objectModelPartition(), size);
}

void Node::operator delete(void* ptr)
{
    ASSERT(isMainThread());
    partitionFree(ptr);
}
#endif

#if DUMP_NODE_STATISTICS
using WeakNodeSet = WillBeHeapHashSet<RawPtrWillBeWeakMember<Node>>;
static WeakNodeSet& liveNodeSet()
{
    DEFINE_STATIC_LOCAL(OwnPtrWillBePersistent<WeakNodeSet>, set, (adoptPtrWillBeNoop(new WeakNodeSet())));
    return *set;
}
#endif

void Node::dumpStatistics()
{
#if DUMP_NODE_STATISTICS
    size_t nodesWithRareData = 0;

    size_t elementNodes = 0;
    size_t attrNodes = 0;
    size_t textNodes = 0;
    size_t cdataNodes = 0;
    size_t commentNodes = 0;
    size_t piNodes = 0;
    size_t documentNodes = 0;
    size_t docTypeNodes = 0;
    size_t fragmentNodes = 0;
    size_t shadowRootNodes = 0;

    HashMap<String, size_t> perTagCount;

    size_t attributes = 0;
    size_t elementsWithAttributeStorage = 0;
    size_t elementsWithRareData = 0;
    size_t elementsWithNamedNodeMap = 0;

    for (Node* node : liveNodeSet()) {
        if (node->hasRareData()) {
            ++nodesWithRareData;
            if (node->isElementNode()) {
                ++elementsWithRareData;
                if (toElement(node)->hasNamedNodeMap())
                    ++elementsWithNamedNodeMap;
            }
        }

        switch (node->nodeType()) {
        case ELEMENT_NODE: {
            ++elementNodes;

            // Tag stats
            Element* element = toElement(node);
            HashMap<String, size_t>::AddResult result = perTagCount.add(element->tagName(), 1);
            if (!result.isNewEntry)
                result.storedValue->value++;

            if (const ElementData* elementData = element->elementData()) {
                attributes += elementData->attributes().size();
                ++elementsWithAttributeStorage;
            }
            break;
        }
        case ATTRIBUTE_NODE: {
            ++attrNodes;
            break;
        }
        case TEXT_NODE: {
            ++textNodes;
            break;
        }
        case CDATA_SECTION_NODE: {
            ++cdataNodes;
            break;
        }
        case COMMENT_NODE: {
            ++commentNodes;
            break;
        }
        case PROCESSING_INSTRUCTION_NODE: {
            ++piNodes;
            break;
        }
        case DOCUMENT_NODE: {
            ++documentNodes;
            break;
        }
        case DOCUMENT_TYPE_NODE: {
            ++docTypeNodes;
            break;
        }
        case DOCUMENT_FRAGMENT_NODE: {
            if (node->isShadowRoot())
                ++shadowRootNodes;
            else
                ++fragmentNodes;
            break;
        }
        }
    }

    printf("Number of Nodes: %d\n\n", liveNodeSet().size());
    printf("Number of Nodes with RareData: %zu\n\n", nodesWithRareData);

    printf("NodeType distribution:\n");
    printf("  Number of Element nodes: %zu\n", elementNodes);
    printf("  Number of Attribute nodes: %zu\n", attrNodes);
    printf("  Number of Text nodes: %zu\n", textNodes);
    printf("  Number of CDATASection nodes: %zu\n", cdataNodes);
    printf("  Number of Comment nodes: %zu\n", commentNodes);
    printf("  Number of ProcessingInstruction nodes: %zu\n", piNodes);
    printf("  Number of Document nodes: %zu\n", documentNodes);
    printf("  Number of DocumentType nodes: %zu\n", docTypeNodes);
    printf("  Number of DocumentFragment nodes: %zu\n", fragmentNodes);
    printf("  Number of ShadowRoot nodes: %zu\n", shadowRootNodes);

    printf("Element tag name distibution:\n");
    for (const auto& entry : perTagCount)
        printf("  Number of <%s> tags: %zu\n", entry.key.utf8().data(), entry.value);

    printf("Attributes:\n");
    printf("  Number of Attributes (non-Node and Node): %zu [%zu]\n", attributes, sizeof(Attribute));
    printf("  Number of Elements with attribute storage: %zu [%zu]\n", elementsWithAttributeStorage, sizeof(ElementData));
    printf("  Number of Elements with RareData: %zu\n", elementsWithRareData);
    printf("  Number of Elements with NamedNodeMap: %zu [%zu]\n", elementsWithNamedNodeMap, sizeof(NamedNodeMap));
#endif
}

DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, nodeCounter, ("WebCoreNode"));

void Node::trackForDebugging()
{
#ifndef NDEBUG
    nodeCounter.increment();
#endif

#if DUMP_NODE_STATISTICS
    liveNodeSet().add(this);
#endif
}

Node::Node(TreeScope* treeScope, ConstructionType type)
    : m_nodeFlags(type)
    , m_parentOrShadowHostNode(nullptr)
    , m_treeScope(treeScope)
    , m_previous(nullptr)
    , m_next(nullptr)
{
    ASSERT(m_treeScope || type == CreateDocument || type == CreateShadowRoot);
#if !ENABLE(OILPAN)
    if (m_treeScope)
        m_treeScope->guardRef();
#endif

#if !defined(NDEBUG) || (defined(DUMP_NODE_STATISTICS) && DUMP_NODE_STATISTICS)
    trackForDebugging();
#endif
    InstanceCounters::incrementCounter(InstanceCounters::NodeCounter);
}

Node::~Node()
{
#ifndef NDEBUG
    nodeCounter.decrement();
#endif

#if !ENABLE(OILPAN)
#if DUMP_NODE_STATISTICS
    liveNodeSet().remove(this);
#endif

    if (hasRareData())
        clearRareData();

    RELEASE_ASSERT(!layoutObject());

    if (!isContainerNode())
        willBeDeletedFromDocument();

    if (m_previous)
        m_previous->setNextSibling(nullptr);
    if (m_next)
        m_next->setPreviousSibling(nullptr);

    if (m_treeScope)
        m_treeScope->guardDeref();

    if (getFlag(HasWeakReferencesFlag))
        WeakIdentifierMap<Node>::notifyObjectDestroyed(this);

    // clearEventTargetData() must be always done,
    // or eventTargetDataMap() may keep a raw pointer to a deleted object.
    ASSERT(!hasEventTargetData());
#else
    // With Oilpan, the rare data finalizer also asserts for
    // this condition (we cannot directly access it here.)
    RELEASE_ASSERT(hasRareData() || !layoutObject());
#endif

    InstanceCounters::decrementCounter(InstanceCounters::NodeCounter);
}

#if !ENABLE(OILPAN)
// With Oilpan all of this is handled with weak processing of the document.
void Node::willBeDeletedFromDocument()
{
    if (hasEventTargetData())
        clearEventTargetData();

    if (!isTreeScopeInitialized())
        return;

    Document& document = this->document();

    if (document.frameHost())
        document.frameHost()->eventHandlerRegistry().didRemoveAllEventHandlers(*this);

    document.markers().removeMarkers(this);
}
#endif

NodeRareData* Node::rareData() const
{
    ASSERT_WITH_SECURITY_IMPLICATION(hasRareData());
    return static_cast<NodeRareData*>(m_data.m_rareData);
}

NodeRareData& Node::ensureRareData()
{
    if (hasRareData())
        return *rareData();

    if (isElementNode())
        m_data.m_rareData = ElementRareData::create(m_data.m_layoutObject);
    else
        m_data.m_rareData = NodeRareData::create(m_data.m_layoutObject);

    ASSERT(m_data.m_rareData);

    setFlag(HasRareDataFlag);
    return *rareData();
}

#if !ENABLE(OILPAN)
void Node::clearRareData()
{
    ASSERT(hasRareData());
    ASSERT(!transientMutationObserverRegistry() || transientMutationObserverRegistry()->isEmpty());

    LayoutObject* layoutObject = m_data.m_rareData->layoutObject();
    if (isElementNode())
        delete static_cast<ElementRareData*>(m_data.m_rareData);
    else
        delete static_cast<NodeRareData*>(m_data.m_rareData);
    m_data.m_layoutObject = layoutObject;
    clearFlag(HasRareDataFlag);
}
#endif

Node* Node::toNode()
{
    return this;
}

short Node::tabIndex() const
{
    return 0;
}

String Node::nodeValue() const
{
    return String();
}

void Node::setNodeValue(const String&)
{
    // By default, setting nodeValue has no effect.
}

PassRefPtrWillBeRawPtr<NodeList> Node::childNodes()
{
    if (isContainerNode())
        return ensureRareData().ensureNodeLists().ensureChildNodeList(toContainerNode(*this));
    return ensureRareData().ensureNodeLists().ensureEmptyChildNodeList(*this);
}

Node* Node::pseudoAwarePreviousSibling() const
{
    if (parentElement() && !previousSibling()) {
        Element* parent = parentElement();
        if (isAfterPseudoElement() && parent->lastChild())
            return parent->lastChild();
        if (!isBeforePseudoElement())
            return parent->pseudoElement(BEFORE);
    }
    return previousSibling();
}

Node* Node::pseudoAwareNextSibling() const
{
    if (parentElement() && !nextSibling()) {
        Element* parent = parentElement();
        if (isBeforePseudoElement() && parent->hasChildren())
            return parent->firstChild();
        if (!isAfterPseudoElement())
            return parent->pseudoElement(AFTER);
    }
    return nextSibling();
}

Node* Node::pseudoAwareFirstChild() const
{
    if (isElementNode()) {
        const Element* currentElement = toElement(this);
        Node* first = currentElement->pseudoElement(BEFORE);
        if (first)
            return first;
        first = currentElement->firstChild();
        if (!first)
            first = currentElement->pseudoElement(AFTER);
        return first;
    }

    return firstChild();
}

Node* Node::pseudoAwareLastChild() const
{
    if (isElementNode()) {
        const Element* currentElement = toElement(this);
        Node* last = currentElement->pseudoElement(AFTER);
        if (last)
            return last;
        last = currentElement->lastChild();
        if (!last)
            last = currentElement->pseudoElement(BEFORE);
        return last;
    }

    return lastChild();
}

PassRefPtrWillBeRawPtr<Node> Node::insertBefore(PassRefPtrWillBeRawPtr<Node> newChild, Node* refChild, ExceptionState& exceptionState)
{
    if (isContainerNode())
        return toContainerNode(this)->insertBefore(newChild, refChild, exceptionState);

    exceptionState.throwDOMException(HierarchyRequestError, "This node type does not support this method.");
    return nullptr;
}

PassRefPtrWillBeRawPtr<Node> Node::replaceChild(PassRefPtrWillBeRawPtr<Node> newChild, PassRefPtrWillBeRawPtr<Node> oldChild, ExceptionState& exceptionState)
{
    if (isContainerNode())
        return toContainerNode(this)->replaceChild(newChild, oldChild, exceptionState);

    exceptionState.throwDOMException(HierarchyRequestError,  "This node type does not support this method.");
    return nullptr;
}

PassRefPtrWillBeRawPtr<Node> Node::removeChild(PassRefPtrWillBeRawPtr<Node> oldChild, ExceptionState& exceptionState)
{
    if (isContainerNode())
        return toContainerNode(this)->removeChild(oldChild, exceptionState);

    exceptionState.throwDOMException(NotFoundError, "This node type does not support this method.");
    return nullptr;
}

PassRefPtrWillBeRawPtr<Node> Node::appendChild(PassRefPtrWillBeRawPtr<Node> newChild, ExceptionState& exceptionState)
{
    if (isContainerNode())
        return toContainerNode(this)->appendChild(newChild, exceptionState);

    exceptionState.throwDOMException(HierarchyRequestError, "This node type does not support this method.");
    return nullptr;
}

static bool isNodeInNodes(const Node& node, const HeapVector<NodeOrString>& nodes)
{
    for (const NodeOrString& nodeOrString : nodes) {
        if (nodeOrString.isNode() && nodeOrString.getAsNode() == &node)
            return true;
    }
    return false;
}

static Node* findViablePreviousSibling(const Node& node, const HeapVector<NodeOrString>& nodes)
{
    for (Node* sibling = node.previousSibling(); sibling; sibling = sibling->previousSibling()) {
        if (!isNodeInNodes(*sibling, nodes))
            return sibling;
    }
    return nullptr;
}

static Node* findViableNextSibling(const Node& node, const HeapVector<NodeOrString>& nodes)
{
    for (Node* sibling = node.nextSibling(); sibling; sibling = sibling->nextSibling()) {
        if (!isNodeInNodes(*sibling, nodes))
            return sibling;
    }
    return nullptr;
}

static PassRefPtrWillBeRawPtr<Node> nodeOrStringToNode(const NodeOrString& nodeOrString, Document& document)
{
    if (nodeOrString.isNode())
        return nodeOrString.getAsNode();
    return Text::create(document, nodeOrString.getAsString());
}

static PassRefPtrWillBeRawPtr<Node> convertNodesIntoNode(const HeapVector<NodeOrString>& nodes, Document& document)
{
    if (nodes.size() == 1)
        return nodeOrStringToNode(nodes[0], document);

    RefPtrWillBeRawPtr<Node> fragment = DocumentFragment::create(document);
    for (const NodeOrString& nodeOrString : nodes)
        fragment->appendChild(nodeOrStringToNode(nodeOrString, document), ASSERT_NO_EXCEPTION);
    return fragment.release();
}

void Node::prepend(const HeapVector<NodeOrString>& nodes, ExceptionState& exceptionState)
{
    RefPtrWillBeRawPtr<Node> node = convertNodesIntoNode(nodes, document());
    insertBefore(node, firstChild(), exceptionState);
}

void Node::append(const HeapVector<NodeOrString>& nodes, ExceptionState& exceptionState)
{
    RefPtrWillBeRawPtr<Node> node = convertNodesIntoNode(nodes, document());
    appendChild(node, exceptionState);
}

void Node::before(const HeapVector<NodeOrString>& nodes, ExceptionState& exceptionState)
{
    Node* parent = parentNode();
    if (!parent)
        return;
    Node* viablePreviousSibling = findViablePreviousSibling(*this, nodes);
    RefPtrWillBeRawPtr<Node> node = convertNodesIntoNode(nodes, document());
    parent->insertBefore(node, viablePreviousSibling ? viablePreviousSibling->nextSibling() : parent->firstChild(), exceptionState);
}

void Node::after(const HeapVector<NodeOrString>& nodes, ExceptionState& exceptionState)
{
    Node* parent = parentNode();
    if (!parent)
        return;
    Node* viableNextSibling = findViableNextSibling(*this, nodes);
    RefPtrWillBeRawPtr<Node> node = convertNodesIntoNode(nodes, document());
    parent->insertBefore(node, viableNextSibling, exceptionState);
}

void Node::replaceWith(const HeapVector<NodeOrString>& nodes, ExceptionState& exceptionState)
{
    Node* parent = parentNode();
    if (!parent)
        return;
    Node* viableNextSibling = findViableNextSibling(*this, nodes);
    RefPtrWillBeRawPtr<Node> node = convertNodesIntoNode(nodes, document());
    if (parent == parentNode())
        parent->replaceChild(node, this, exceptionState);
    else
        parent->insertBefore(node, viableNextSibling, exceptionState);
}

void Node::remove(ExceptionState& exceptionState)
{
    if (ContainerNode* parent = parentNode())
        parent->removeChild(this, exceptionState);
}

void Node::normalize()
{
    updateDistribution();

    // Go through the subtree beneath us, normalizing all nodes. This means that
    // any two adjacent text nodes are merged and any empty text nodes are removed.

    RefPtrWillBeRawPtr<Node> node = this;
    while (Node* firstChild = node->firstChild())
        node = firstChild;
    while (node) {
        if (node == this)
            break;

        if (node->nodeType() == TEXT_NODE)
            node = toText(node)->mergeNextSiblingNodesIfPossible();
        else
            node = NodeTraversal::nextPostOrder(*node);
    }
}

const AtomicString& Node::localName() const
{
    return nullAtom;
}

const AtomicString& Node::namespaceURI() const
{
    return nullAtom;
}

bool Node::isContentEditable(UserSelectAllTreatment treatment)
{
    document().updateLayoutTreeIfNeeded();
    return hasEditableStyle(Editable, treatment);
}

bool Node::isContentRichlyEditable()
{
    document().updateLayoutTreeIfNeeded();
    return hasEditableStyle(RichlyEditable, UserSelectAllIsAlwaysNonEditable);
}

bool Node::hasEditableStyle(EditableLevel editableLevel, UserSelectAllTreatment treatment) const
{
    if (isPseudoElement())
        return false;

    // Ideally we'd call ASSERT(!needsStyleRecalc()) here, but
    // ContainerNode::setFocus() calls setNeedsStyleRecalc(), so the assertion
    // would fire in the middle of Document::setFocusedNode().

    for (const Node* node = this; node; node = node->parentNode()) {
        if ((node->isHTMLElement() || node->isDocumentNode()) && node->layoutObject()) {
            // Elements with user-select: all style are considered atomic
            // therefore non editable.
            if (Position::nodeIsUserSelectAll(node) && treatment == UserSelectAllIsAlwaysNonEditable)
                return false;
            switch (node->layoutObject()->style()->userModify()) {
            case READ_ONLY:
                return false;
            case READ_WRITE:
                return true;
            case READ_WRITE_PLAINTEXT_ONLY:
                return editableLevel != RichlyEditable;
            }
            ASSERT_NOT_REACHED();
            return false;
        }
    }

    return false;
}

bool Node::isEditableToAccessibility(EditableLevel editableLevel) const
{
    if (hasEditableStyle(editableLevel))
        return true;

    // FIXME: Respect editableLevel for ARIA editable elements.
    if (editableLevel == RichlyEditable)
        return false;

    // FIXME(dmazzoni): support ScopedAXObjectCache (crbug/489851).
    if (AXObjectCache* cache = document().existingAXObjectCache())
        return cache->rootAXEditableElement(this);

    return false;
}

LayoutBox* Node::layoutBox() const
{
    LayoutObject* layoutObject = this->layoutObject();
    return layoutObject && layoutObject->isBox() ? toLayoutBox(layoutObject) : nullptr;
}

LayoutBoxModelObject* Node::layoutBoxModelObject() const
{
    LayoutObject* layoutObject = this->layoutObject();
    return layoutObject && layoutObject->isBoxModelObject() ? toLayoutBoxModelObject(layoutObject) : nullptr;
}

LayoutRect Node::boundingBox() const
{
    if (layoutObject())
        return LayoutRect(layoutObject()->absoluteBoundingBoxRect());
    return LayoutRect();
}

bool Node::hasNonEmptyBoundingBox() const
{
    // Before calling absoluteRects, check for the common case where the layoutObject
    // is non-empty, since this is a faster check and almost always returns true.
    LayoutBoxModelObject* box = layoutBoxModelObject();
    if (!box)
        return false;
    if (!box->borderBoundingBox().isEmpty())
        return true;

    Vector<IntRect> rects;
    FloatPoint absPos = layoutObject()->localToAbsolute();
    layoutObject()->absoluteRects(rects, flooredLayoutPoint(absPos));
    size_t n = rects.size();
    for (size_t i = 0; i < n; ++i) {
        if (!rects[i].isEmpty()) {
            return true;
        }
    }

    return false;
}

#ifndef NDEBUG
inline static ShadowRoot* oldestShadowRootFor(const Node* node)
{
    if (!node->isElementNode())
        return nullptr;
    if (ElementShadow* shadow = toElement(node)->shadow())
        return shadow->oldestShadowRoot();
    return nullptr;
}
#endif

inline static Node& rootInTreeOfTrees(const Node& node)
{
    if (node.inDocument())
        return node.document();
    Node* root = const_cast<Node*>(&node);
    while (Node* host = root->shadowHost())
        root = host;
    while (Node* ancestor = root->parentNode())
        root = ancestor;
    ASSERT(!root->shadowHost());
    return *root;
}

#if ENABLE(ASSERT)
bool Node::needsDistributionRecalc() const
{
    return rootInTreeOfTrees(*this).childNeedsDistributionRecalc();
}
#endif

void Node::updateDistribution()
{
    TRACE_EVENT0("blink", "Node::updateDistribution");
    ScriptForbiddenScope forbidScript;
    rootInTreeOfTrees(*this).recalcDistribution();
}

void Node::recalcDistribution()
{
    if (isElementNode()) {
        if (ElementShadow* shadow = toElement(this)->shadow())
            shadow->distributeIfNeeded();
    }

    for (Node* child = firstChild(); child; child = child->nextSibling()) {
        if (child->childNeedsDistributionRecalc())
            child->recalcDistribution();
    }

    for (ShadowRoot* root = youngestShadowRoot(); root; root = root->olderShadowRoot()) {
        if (root->childNeedsDistributionRecalc())
            root->recalcDistribution();
    }

    clearChildNeedsDistributionRecalc();
}

void Node::setIsLink(bool isLink)
{
    setFlag(isLink && !SVGImage::isInSVGImage(toElement(this)), IsLinkFlag);
}

void Node::setNeedsStyleInvalidation()
{
    ASSERT(isElementNode());
    setFlag(NeedsStyleInvalidationFlag);
    markAncestorsWithChildNeedsStyleInvalidation();
}

void Node::markAncestorsWithChildNeedsStyleInvalidation()
{
    for (Node* node = parentOrShadowHostNode(); node && !node->childNeedsStyleInvalidation(); node = node->parentOrShadowHostNode())
        node->setChildNeedsStyleInvalidation();
    document().scheduleLayoutTreeUpdateIfNeeded();
}

void Node::markAncestorsWithChildNeedsDistributionRecalc()
{
    for (Node* node = this; node && !node->childNeedsDistributionRecalc(); node = node->parentOrShadowHostNode())
        node->setChildNeedsDistributionRecalc();
    document().scheduleLayoutTreeUpdateIfNeeded();
}

inline void Node::setStyleChange(StyleChangeType changeType)
{
    m_nodeFlags = (m_nodeFlags & ~StyleChangeMask) | changeType;
}

void Node::markAncestorsWithChildNeedsStyleRecalc()
{
    for (ContainerNode* p = parentOrShadowHostNode(); p && !p->childNeedsStyleRecalc(); p = p->parentOrShadowHostNode())
        p->setChildNeedsStyleRecalc();
    document().scheduleLayoutTreeUpdateIfNeeded();
    document().incStyleVersion();
}

void Node::setNeedsStyleRecalc(StyleChangeType changeType, const StyleChangeReasonForTracing& reason)
{
    ASSERT(changeType != NoStyleChange);
    if (!inActiveDocument())
        return;

    TRACE_EVENT_INSTANT1(
        TRACE_DISABLED_BY_DEFAULT("devtools.timeline.invalidationTracking"),
        "StyleRecalcInvalidationTracking",
        TRACE_EVENT_SCOPE_THREAD,
        "data",
        InspectorStyleRecalcInvalidationTrackingEvent::data(this, reason));

    StyleChangeType existingChangeType = styleChangeType();
    if (changeType > existingChangeType)
        setStyleChange(changeType);

    if (existingChangeType == NoStyleChange)
        markAncestorsWithChildNeedsStyleRecalc();

    if (isElementNode() && hasRareData())
        toElement(*this).setAnimationStyleChange(false);
}

void Node::clearNeedsStyleRecalc()
{
    m_nodeFlags &= ~StyleChangeMask;

    clearSVGFilterNeedsLayerUpdate();

    if (isElementNode() && hasRareData())
        toElement(*this).setAnimationStyleChange(false);
}

bool Node::inActiveDocument() const
{
    return inDocument() && document().isActive();
}

Node* Node::focusDelegate()
{
    return this;
}

bool Node::shouldHaveFocusAppearance() const
{
    ASSERT(focused());
    return true;
}

bool Node::isInert() const
{
    const HTMLDialogElement* dialog = document().activeModalDialog();
    if (dialog && this != document() && (!canParticipateInComposedTree() || !ComposedTreeTraversal::containsIncludingPseudoElement(*dialog, *this)))
        return true;
    return document().ownerElement() && document().ownerElement()->isInert();
}

unsigned Node::nodeIndex() const
{
    Node* tempNode = previousSibling();
    unsigned count = 0;
    for (count = 0; tempNode; count++)
        tempNode = tempNode->previousSibling();
    return count;
}

NodeListsNodeData* Node::nodeLists()
{
    return hasRareData() ? rareData()->nodeLists() : nullptr;
}

void Node::clearNodeLists()
{
    rareData()->clearNodeLists();
}

bool Node::isDescendantOf(const Node *other) const
{
    // Return true if other is an ancestor of this, otherwise false
    if (!other || !other->hasChildren() || inDocument() != other->inDocument())
        return false;
    if (other->treeScope() != treeScope())
        return false;
    if (other->isTreeScope())
        return !isTreeScope();
    for (const ContainerNode* n = parentNode(); n; n = n->parentNode()) {
        if (n == other)
            return true;
    }
    return false;
}

bool Node::contains(const Node* node) const
{
    if (!node)
        return false;
    return this == node || node->isDescendantOf(this);
}

bool Node::containsIncludingShadowDOM(const Node* node) const
{
    if (!node)
        return false;

    if (this == node)
        return true;

    if (document() != node->document())
        return false;

    if (inDocument() != node->inDocument())
        return false;

    bool hasChildren = isContainerNode() && toContainerNode(this)->hasChildren();
    bool hasShadow = isElementNode() && toElement(this)->shadow();
    if (!hasChildren && !hasShadow)
        return false;

    for (; node; node = node->shadowHost()) {
        if (treeScope() == node->treeScope())
            return contains(node);
    }

    return false;
}

bool Node::containsIncludingHostElements(const Node& node) const
{
    const Node* current = &node;
    do {
        if (current == this)
            return true;
        if (current->isDocumentFragment() && toDocumentFragment(current)->isTemplateContent())
            current = static_cast<const TemplateContentDocumentFragment*>(current)->host();
        else
            current = current->parentOrShadowHostNode();
    } while (current);
    return false;
}

Node* Node::commonAncestor(const Node& other, ContainerNode* (*parent)(const Node&)) const
{
    if (this == other)
        return const_cast<Node*>(this);
    if (document() != other.document())
        return nullptr;
    int thisDepth = 0;
    for (const Node* node = this; node; node = parent(*node)) {
        if (node == &other)
            return const_cast<Node*>(node);
        thisDepth++;
    }
    int otherDepth = 0;
    for (const Node* node = &other; node; node = parent(*node)) {
        if (node == this)
            return const_cast<Node*>(this);
        otherDepth++;
    }
    const Node* thisIterator = this;
    const Node* otherIterator = &other;
    if (thisDepth > otherDepth) {
        for (int i = thisDepth; i > otherDepth; --i)
            thisIterator = parent(*thisIterator);
    } else if (otherDepth > thisDepth) {
        for (int i = otherDepth; i > thisDepth; --i)
            otherIterator = parent(*otherIterator);
    }
    while (thisIterator) {
        if (thisIterator == otherIterator)
            return const_cast<Node*>(thisIterator);
        thisIterator = parent(*thisIterator);
        otherIterator = parent(*otherIterator);
    }
    ASSERT(!otherIterator);
    return nullptr;
}

void Node::reattach(const AttachContext& context)
{
    AttachContext reattachContext(context);
    reattachContext.performingReattach = true;

    // We only need to detach if the node has already been through attach().
    if (styleChangeType() < NeedsReattachStyleChange)
        detach(reattachContext);
    attach(reattachContext);
}

void Node::attach(const AttachContext&)
{
    ASSERT(document().inStyleRecalc() || isDocumentNode());
    ASSERT(!document().lifecycle().inDetach());
    ASSERT(needsAttach());
    ASSERT(!layoutObject() || (layoutObject()->style() && (layoutObject()->parent() || layoutObject()->isLayoutView())));

    clearNeedsStyleRecalc();

    if (AXObjectCache* cache = document().axObjectCache())
        cache->updateCacheAfterNodeIsAttached(this);
}

void Node::detach(const AttachContext& context)
{
    ASSERT(document().lifecycle().stateAllowsDetach());
    DocumentLifecycle::DetachScope willDetach(document().lifecycle());

    if (layoutObject())
        layoutObject()->destroyAndCleanupAnonymousWrappers();
    setLayoutObject(nullptr);

    setStyleChange(NeedsReattachStyleChange);
    clearChildNeedsStyleInvalidation();
}

void Node::reattachWhitespaceSiblingsIfNeeded(Text* start)
{
    for (Node* sibling = start; sibling; sibling = sibling->nextSibling()) {
        if (sibling->isTextNode() && toText(sibling)->containsOnlyWhitespace()) {
            bool hadLayoutObject = !!sibling->layoutObject();
            toText(sibling)->reattachIfNeeded();
            // If sibling's layout object status didn't change we don't need to continue checking
            // other siblings since their layout object status won't change either.
            if (!!sibling->layoutObject() == hadLayoutObject)
                return;
        } else if (sibling->layoutObject()) {
            return;
        }
    }
}

const ComputedStyle* Node::virtualEnsureComputedStyle(PseudoId pseudoElementSpecifier)
{
    return parentOrShadowHostNode() ? parentOrShadowHostNode()->ensureComputedStyle(pseudoElementSpecifier) : nullptr;
}

int Node::maxCharacterOffset() const
{
    ASSERT_NOT_REACHED();
    return 0;
}

// FIXME: Shouldn't these functions be in the editing code?  Code that asks questions about HTML in the core DOM class
// is obviously misplaced.
bool Node::canStartSelection() const
{
    if (hasEditableStyle())
        return true;

    if (layoutObject()) {
        const ComputedStyle& style = layoutObject()->styleRef();
        // We allow selections to begin within an element that has -webkit-user-select: none set,
        // but if the element is draggable then dragging should take priority over selection.
        if (style.userDrag() == DRAG_ELEMENT && style.userSelect() == SELECT_NONE)
            return false;
    }
    return parentOrShadowHostNode() ? parentOrShadowHostNode()->canStartSelection() : true;
}

bool Node::canParticipateInComposedTree() const
{
    return !isShadowRoot() && !isActiveInsertionPoint(*this);
}

Element* Node::shadowHost() const
{
    if (ShadowRoot* root = containingShadowRoot())
        return root->host();
    return nullptr;
}

ShadowRoot* Node::containingShadowRoot() const
{
    Node& root = treeScope().rootNode();
    return root.isShadowRoot() ? toShadowRoot(&root) : nullptr;
}

Node* Node::nonBoundaryShadowTreeRootNode()
{
    ASSERT(!isShadowRoot());
    Node* root = this;
    while (root) {
        if (root->isShadowRoot())
            return root;
        Node* parent = root->parentOrShadowHostNode();
        if (parent && parent->isShadowRoot())
            return root;
        root = parent;
    }
    return nullptr;
}

ContainerNode* Node::nonShadowBoundaryParentNode() const
{
    ContainerNode* parent = parentNode();
    return parent && !parent->isShadowRoot() ? parent : nullptr;
}

Element* Node::parentOrShadowHostElement() const
{
    ContainerNode* parent = parentOrShadowHostNode();
    if (!parent)
        return nullptr;

    if (parent->isShadowRoot())
        return toShadowRoot(parent)->host();

    if (!parent->isElementNode())
        return nullptr;

    return toElement(parent);
}

ContainerNode* Node::parentOrShadowHostOrTemplateHostNode() const
{
    if (isDocumentFragment() && toDocumentFragment(this)->isTemplateContent())
        return static_cast<const TemplateContentDocumentFragment*>(this)->host();
    return parentOrShadowHostNode();
}

bool Node::isRootEditableElement() const
{
    return hasEditableStyle() && isElementNode() && (!parentNode() || !parentNode()->hasEditableStyle()
        || !parentNode()->isElementNode() || isHTMLBodyElement((*this)));
}

Element* Node::rootEditableElement(EditableType editableType) const
{
    if (editableType == HasEditableAXRole) {
        if (AXObjectCache* cache = document().existingAXObjectCache())
            return const_cast<Element*>(cache->rootAXEditableElement(this));
    }

    return rootEditableElement();
}

Element* Node::rootEditableElement() const
{
    Element* result = nullptr;
    for (Node* n = const_cast<Node*>(this); n && n->hasEditableStyle(); n = n->parentNode()) {
        if (n->isElementNode())
            result = toElement(n);
        if (isHTMLBodyElement(*n))
            break;
    }
    return result;
}

// FIXME: End of obviously misplaced HTML editing functions.  Try to move these out of Node.

Document* Node::ownerDocument() const
{
    Document* doc = &document();
    return doc == this ? nullptr : doc;
}

KURL Node::baseURI() const
{
    return parentNode() ? parentNode()->baseURI() : KURL();
}

bool Node::isEqualNode(Node* other) const
{
    if (!other)
        return false;

    NodeType nodeType = this->nodeType();
    if (nodeType != other->nodeType())
        return false;

    if (nodeName() != other->nodeName())
        return false;

    if (localName() != other->localName())
        return false;

    if (namespaceURI() != other->namespaceURI())
        return false;

    if (nodeValue() != other->nodeValue())
        return false;

    if (isElementNode() && !toElement(this)->hasEquivalentAttributes(toElement(other)))
        return false;

    Node* child = firstChild();
    Node* otherChild = other->firstChild();

    while (child) {
        if (!child->isEqualNode(otherChild))
            return false;

        child = child->nextSibling();
        otherChild = otherChild->nextSibling();
    }

    if (otherChild)
        return false;

    if (isDocumentTypeNode()) {
        const DocumentType* documentTypeThis = toDocumentType(this);
        const DocumentType* documentTypeOther = toDocumentType(other);

        if (documentTypeThis->publicId() != documentTypeOther->publicId())
            return false;

        if (documentTypeThis->systemId() != documentTypeOther->systemId())
            return false;
    }

    return true;
}

bool Node::isDefaultNamespace(const AtomicString& namespaceURIMaybeEmpty) const
{
    const AtomicString& namespaceURI = namespaceURIMaybeEmpty.isEmpty() ? nullAtom : namespaceURIMaybeEmpty;

    switch (nodeType()) {
    case ELEMENT_NODE: {
        const Element& element = toElement(*this);

        if (element.prefix().isNull())
            return element.namespaceURI() == namespaceURI;

        AttributeCollection attributes = element.attributes();
        for (const Attribute& attr : attributes) {
            if (attr.localName() == xmlnsAtom)
                return attr.value() == namespaceURI;
        }

        if (Element* parent = parentElement())
            return parent->isDefaultNamespace(namespaceURI);

        return false;
    }
    case DOCUMENT_NODE:
        if (Element* de = toDocument(this)->documentElement())
            return de->isDefaultNamespace(namespaceURI);
        return false;
    case DOCUMENT_TYPE_NODE:
    case DOCUMENT_FRAGMENT_NODE:
        return false;
    case ATTRIBUTE_NODE: {
        const Attr* attr = toAttr(this);
        if (attr->ownerElement())
            return attr->ownerElement()->isDefaultNamespace(namespaceURI);
        return false;
    }
    default:
        if (Element* parent = parentElement())
            return parent->isDefaultNamespace(namespaceURI);
        return false;
    }
}

const AtomicString& Node::lookupPrefix(const AtomicString& namespaceURI) const
{
    // Implemented according to
    // http://dom.spec.whatwg.org/#dom-node-lookupprefix

    if (namespaceURI.isEmpty() || namespaceURI.isNull())
        return nullAtom;

    const Element* context;

    switch (nodeType()) {
    case ELEMENT_NODE:
        context = toElement(this);
        break;
    case DOCUMENT_NODE:
        context = toDocument(this)->documentElement();
        break;
    case DOCUMENT_FRAGMENT_NODE:
    case DOCUMENT_TYPE_NODE:
        context = nullptr;
        break;
    // FIXME: Remove this when Attr no longer extends Node (CR305105)
    case ATTRIBUTE_NODE:
        context = toAttr(this)->ownerElement();
        break;
    default:
        context = parentElement();
        break;
    }

    if (!context)
        return nullAtom;

    return context->locateNamespacePrefix(namespaceURI);
}

const AtomicString& Node::lookupNamespaceURI(const String& prefix) const
{
    // Implemented according to
    // http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/namespaces-algorithms.html#lookupNamespaceURIAlgo

    if (!prefix.isNull() && prefix.isEmpty())
        return nullAtom;

    switch (nodeType()) {
    case ELEMENT_NODE: {
        const Element& element = toElement(*this);

        if (!element.namespaceURI().isNull() && element.prefix() == prefix)
            return element.namespaceURI();

        AttributeCollection attributes = element.attributes();
        for (const Attribute& attr : attributes) {
            if (attr.prefix() == xmlnsAtom && attr.localName() == prefix) {
                if (!attr.value().isEmpty())
                    return attr.value();
                return nullAtom;
            }
            if (attr.localName() == xmlnsAtom && prefix.isNull()) {
                if (!attr.value().isEmpty())
                    return attr.value();
                return nullAtom;
            }
        }

        if (Element* parent = parentElement())
            return parent->lookupNamespaceURI(prefix);
        return nullAtom;
    }
    case DOCUMENT_NODE:
        if (Element* de = toDocument(this)->documentElement())
            return de->lookupNamespaceURI(prefix);
        return nullAtom;
    case DOCUMENT_TYPE_NODE:
    case DOCUMENT_FRAGMENT_NODE:
        return nullAtom;
    case ATTRIBUTE_NODE: {
        const Attr *attr = toAttr(this);
        if (attr->ownerElement())
            return attr->ownerElement()->lookupNamespaceURI(prefix);
        return nullAtom;
    }
    default:
        if (Element* parent = parentElement())
            return parent->lookupNamespaceURI(prefix);
        return nullAtom;
    }
}

String Node::textContent(bool convertBRsToNewlines) const
{
    // This covers ProcessingInstruction and Comment that should return their
    // value when .textContent is accessed on them, but should be ignored when
    // iterated over as a descendant of a ContainerNode.
    if (isCharacterDataNode())
        return toCharacterData(this)->data();

    // Documents and non-container nodes (that are not CharacterData)
    // have null textContent.
    if (isDocumentNode() || !isContainerNode())
        return String();

    StringBuilder content;
    for (Node& node : NodeTraversal::inclusiveDescendantsOf(*this)) {
        if (isHTMLBRElement(node) && convertBRsToNewlines) {
            content.append('\n');
        } else if (node.isTextNode()) {
            content.append(toText(node).data());
        }
    }
    return content.toString();
}

void Node::setTextContent(const String& text)
{
    switch (nodeType()) {
    case TEXT_NODE:
    case CDATA_SECTION_NODE:
    case COMMENT_NODE:
    case PROCESSING_INSTRUCTION_NODE:
        setNodeValue(text);
        return;
    case ELEMENT_NODE:
    case DOCUMENT_FRAGMENT_NODE: {
        // FIXME: Merge this logic into replaceChildrenWithText.
        RefPtrWillBeRawPtr<ContainerNode> container = toContainerNode(this);

        // Note: This is an intentional optimization.
        // See crbug.com/352836 also.
        // No need to do anything if the text is identical.
        if (container->hasOneTextChild() && toText(container->firstChild())->data() == text)
            return;

        ChildListMutationScope mutation(*this);
        // Note: This API will not insert empty text nodes:
        // http://dom.spec.whatwg.org/#dom-node-textcontent
        if (text.isEmpty()) {
            container->removeChildren(DispatchSubtreeModifiedEvent);
        } else {
            container->removeChildren(OmitSubtreeModifiedEvent);
            container->appendChild(document().createTextNode(text), ASSERT_NO_EXCEPTION);
        }
        return;
    }
    case ATTRIBUTE_NODE:
    case DOCUMENT_NODE:
    case DOCUMENT_TYPE_NODE:
        // Do nothing.
        return;
    }
    ASSERT_NOT_REACHED();
}

bool Node::offsetInCharacters() const
{
    return isCharacterDataNode();
}

unsigned short Node::compareDocumentPosition(const Node* otherNode, ShadowTreesTreatment treatment) const
{
    if (otherNode == this)
        return DOCUMENT_POSITION_EQUIVALENT;

    const Attr* attr1 = nodeType() == ATTRIBUTE_NODE ? toAttr(this) : nullptr;
    const Attr* attr2 = otherNode->nodeType() == ATTRIBUTE_NODE ? toAttr(otherNode) : nullptr;

    const Node* start1 = attr1 ? attr1->ownerElement() : this;
    const Node* start2 = attr2 ? attr2->ownerElement() : otherNode;

    // If either of start1 or start2 is null, then we are disconnected, since one of the nodes is
    // an orphaned attribute node.
    if (!start1 || !start2) {
        unsigned short direction = (this > otherNode) ? DOCUMENT_POSITION_PRECEDING : DOCUMENT_POSITION_FOLLOWING;
        return DOCUMENT_POSITION_DISCONNECTED | DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC | direction;
    }

    Vector<const Node*, 16> chain1;
    Vector<const Node*, 16> chain2;
    if (attr1)
        chain1.append(attr1);
    if (attr2)
        chain2.append(attr2);

    if (attr1 && attr2 && start1 == start2 && start1) {
        // We are comparing two attributes on the same node. Crawl our attribute map and see which one we hit first.
        const Element* owner1 = attr1->ownerElement();
        AttributeCollection attributes = owner1->attributes();
        for (const Attribute& attr : attributes) {
            // If neither of the two determining nodes is a child node and nodeType is the same for both determining nodes, then an
            // implementation-dependent order between the determining nodes is returned. This order is stable as long as no nodes of
            // the same nodeType are inserted into or removed from the direct container. This would be the case, for example,
            // when comparing two attributes of the same element, and inserting or removing additional attributes might change
            // the order between existing attributes.
            if (attr1->qualifiedName() == attr.name())
                return DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC | DOCUMENT_POSITION_FOLLOWING;
            if (attr2->qualifiedName() == attr.name())
                return DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC | DOCUMENT_POSITION_PRECEDING;
        }

        ASSERT_NOT_REACHED();
        return DOCUMENT_POSITION_DISCONNECTED;
    }

    // If one node is in the document and the other is not, we must be disconnected.
    // If the nodes have different owning documents, they must be disconnected.  Note that we avoid
    // comparing Attr nodes here, since they return false from inDocument() all the time (which seems like a bug).
    if (start1->inDocument() != start2->inDocument() || (treatment == TreatShadowTreesAsDisconnected && start1->treeScope() != start2->treeScope())) {
        unsigned short direction = (this > otherNode) ? DOCUMENT_POSITION_PRECEDING : DOCUMENT_POSITION_FOLLOWING;
        return DOCUMENT_POSITION_DISCONNECTED | DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC | direction;
    }

    // We need to find a common ancestor container, and then compare the indices of the two immediate children.
    const Node* current;
    for (current = start1; current; current = current->parentOrShadowHostNode())
        chain1.append(current);
    for (current = start2; current; current = current->parentOrShadowHostNode())
        chain2.append(current);

    unsigned index1 = chain1.size();
    unsigned index2 = chain2.size();

    // If the two elements don't have a common root, they're not in the same tree.
    if (chain1[index1 - 1] != chain2[index2 - 1]) {
        unsigned short direction = (this > otherNode) ? DOCUMENT_POSITION_PRECEDING : DOCUMENT_POSITION_FOLLOWING;
        return DOCUMENT_POSITION_DISCONNECTED | DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC | direction;
    }

    unsigned connection = start1->treeScope() != start2->treeScope() ? DOCUMENT_POSITION_DISCONNECTED | DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC : 0;

    // Walk the two chains backwards and look for the first difference.
    for (unsigned i = std::min(index1, index2); i; --i) {
        const Node* child1 = chain1[--index1];
        const Node* child2 = chain2[--index2];
        if (child1 != child2) {
            // If one of the children is an attribute, it wins.
            if (child1->nodeType() == ATTRIBUTE_NODE)
                return DOCUMENT_POSITION_FOLLOWING | connection;
            if (child2->nodeType() == ATTRIBUTE_NODE)
                return DOCUMENT_POSITION_PRECEDING | connection;

            // If one of the children is a shadow root,
            if (child1->isShadowRoot() || child2->isShadowRoot()) {
                if (!child2->isShadowRoot())
                    return Node::DOCUMENT_POSITION_FOLLOWING | connection;
                if (!child1->isShadowRoot())
                    return Node::DOCUMENT_POSITION_PRECEDING | connection;

                for (ShadowRoot* child = toShadowRoot(child2)->olderShadowRoot(); child; child = child->olderShadowRoot()) {
                    if (child == child1) {
                        return Node::DOCUMENT_POSITION_FOLLOWING | connection;
                    }
                }

                return Node::DOCUMENT_POSITION_PRECEDING | connection;
            }

            if (!child2->nextSibling())
                return DOCUMENT_POSITION_FOLLOWING | connection;
            if (!child1->nextSibling())
                return DOCUMENT_POSITION_PRECEDING | connection;

            // Otherwise we need to see which node occurs first.  Crawl backwards from child2 looking for child1.
            for (Node* child = child2->previousSibling(); child; child = child->previousSibling()) {
                if (child == child1)
                    return DOCUMENT_POSITION_FOLLOWING | connection;
            }
            return DOCUMENT_POSITION_PRECEDING | connection;
        }
    }

    // There was no difference between the two parent chains, i.e., one was a subset of the other.  The shorter
    // chain is the ancestor.
    return index1 < index2 ?
        DOCUMENT_POSITION_FOLLOWING | DOCUMENT_POSITION_CONTAINED_BY | connection :
        DOCUMENT_POSITION_PRECEDING | DOCUMENT_POSITION_CONTAINS | connection;
}

String Node::debugName() const
{
    StringBuilder name;
    name.append(nodeName());

    if (isElementNode()) {
        const Element& thisElement = toElement(*this);
        if (thisElement.hasID()) {
            name.appendLiteral(" id=\'");
            name.append(thisElement.getIdAttribute());
            name.append('\'');
        }

        if (thisElement.hasClass()) {
            name.appendLiteral(" class=\'");
            for (size_t i = 0; i < thisElement.classNames().size(); ++i) {
                if (i > 0)
                    name.append(' ');
                name.append(thisElement.classNames()[i]);
            }
            name.append('\'');
        }
    }

    return name.toString();
}

#ifndef NDEBUG

static void appendAttributeDesc(const Node* node, StringBuilder& stringBuilder, const QualifiedName& name, const char* attrDesc)
{
    if (!node->isElementNode())
        return;

    String attr = toElement(node)->getAttribute(name);
    if (attr.isEmpty())
        return;

    stringBuilder.append(attrDesc);
    stringBuilder.appendLiteral("=\"");
    stringBuilder.append(attr);
    stringBuilder.appendLiteral("\"");
}

void Node::showNode(const char* prefix) const
{
    if (!prefix)
        prefix = "";
    if (isTextNode()) {
        String value = nodeValue();
        value.replaceWithLiteral('\\', "\\\\");
        value.replaceWithLiteral('\n', "\\n");
        WTFLogAlways("%s%s\t%p \"%s\"\n", prefix, nodeName().utf8().data(), this, value.utf8().data());
    } else {
        StringBuilder attrs;
        appendAttributeDesc(this, attrs, idAttr, " ID");
        appendAttributeDesc(this, attrs, classAttr, " CLASS");
        appendAttributeDesc(this, attrs, styleAttr, " STYLE");
        WTFLogAlways("%s%s\t%p%s\n", prefix, nodeName().utf8().data(), this, attrs.toString().utf8().data());
    }
}

void Node::showTreeForThis() const
{
    showTreeAndMark(this, "*");
}

void Node::showTreeForThisInComposedTree() const
{
    showTreeAndMarkInComposedTree(this, "*");
}

void Node::showNodePathForThis() const
{
    Vector<const Node*, 16> chain;
    const Node* node = this;
    while (node->parentOrShadowHostNode()) {
        chain.append(node);
        node = node->parentOrShadowHostNode();
    }
    for (unsigned index = chain.size(); index > 0; --index) {
        const Node* node = chain[index - 1];
        if (node->isShadowRoot()) {
            int count = 0;
            for (ShadowRoot* shadowRoot = toShadowRoot(node)->olderShadowRoot(); shadowRoot; shadowRoot = shadowRoot->olderShadowRoot())
                ++count;
            WTFLogAlways("/#shadow-root[%d]", count);
            continue;
        }

        switch (node->nodeType()) {
        case ELEMENT_NODE: {
            WTFLogAlways("/%s", node->nodeName().utf8().data());

            const Element* element = toElement(node);
            const AtomicString& idattr = element->getIdAttribute();
            bool hasIdAttr = !idattr.isNull() && !idattr.isEmpty();
            if (node->previousSibling() || node->nextSibling()) {
                int count = 0;
                for (Node* previous = node->previousSibling(); previous; previous = previous->previousSibling()) {
                    if (previous->nodeName() == node->nodeName()) {
                        ++count;
                    }
                }
                if (hasIdAttr)
                    WTFLogAlways("[@id=\"%s\" and position()=%d]", idattr.utf8().data(), count);
                else
                    WTFLogAlways("[%d]", count);
            } else if (hasIdAttr) {
                WTFLogAlways("[@id=\"%s\"]", idattr.utf8().data());
            }
            break;
        }
        case TEXT_NODE:
            WTFLogAlways("/text()");
            break;
        case ATTRIBUTE_NODE:
            WTFLogAlways("/@%s", node->nodeName().utf8().data());
            break;
        default:
            break;
        }
    }
    WTFLogAlways("\n");
}

static void traverseTreeAndMark(const String& baseIndent, const Node* rootNode, const Node* markedNode1, const char* markedLabel1, const Node* markedNode2, const char* markedLabel2)
{
    for (Node& node : NodeTraversal::inclusiveDescendantsOf(*rootNode)) {
        StringBuilder indent;
        if (node == markedNode1)
            indent.append(markedLabel1);
        if (node == markedNode2)
            indent.append(markedLabel2);
        indent.append(baseIndent);
        for (const Node* tmpNode = &node; tmpNode && tmpNode != rootNode; tmpNode = tmpNode->parentOrShadowHostNode())
            indent.append('\t');
        node.showNode(indent.toString().utf8().data());
        indent.append('\t');

        if (node.isElementNode()) {
            const Element& element = toElement(node);
            if (Element* pseudo = element.pseudoElement(BEFORE))
                traverseTreeAndMark(indent.toString(), pseudo, markedNode1, markedLabel1, markedNode2, markedLabel2);
            if (Element* pseudo = element.pseudoElement(AFTER))
                traverseTreeAndMark(indent.toString(), pseudo, markedNode1, markedLabel1, markedNode2, markedLabel2);
            if (Element* pseudo = element.pseudoElement(FIRST_LETTER))
                traverseTreeAndMark(indent.toString(), pseudo, markedNode1, markedLabel1, markedNode2, markedLabel2);
            if (Element* pseudo = element.pseudoElement(BACKDROP))
                traverseTreeAndMark(indent.toString(), pseudo, markedNode1, markedLabel1, markedNode2, markedLabel2);
        }

        if (node.isShadowRoot()) {
            if (ShadowRoot* youngerShadowRoot = toShadowRoot(node).youngerShadowRoot())
                traverseTreeAndMark(indent.toString(), youngerShadowRoot, markedNode1, markedLabel1, markedNode2, markedLabel2);
        } else if (ShadowRoot* oldestShadowRoot = oldestShadowRootFor(&node)) {
            traverseTreeAndMark(indent.toString(), oldestShadowRoot, markedNode1, markedLabel1, markedNode2, markedLabel2);
        }
    }
}

static void traverseTreeAndMarkInComposedTree(const String& baseIndent, const Node* rootNode, const Node* markedNode1, const char* markedLabel1, const Node* markedNode2, const char* markedLabel2)
{
    for (const Node* node = rootNode; node; node = ComposedTreeTraversal::nextSibling(*node)) {
        StringBuilder indent;
        if (node == markedNode1)
            indent.append(markedLabel1);
        if (node == markedNode2)
            indent.append(markedLabel2);
        indent.append(baseIndent);
        node->showNode(indent.toString().utf8().data());
        indent.append('\t');

        Node* child = ComposedTreeTraversal::firstChild(*node);
        if (child)
            traverseTreeAndMarkInComposedTree(indent.toString(), child, markedNode1, markedLabel1, markedNode2, markedLabel2);
    }
}

void Node::showTreeAndMark(const Node* markedNode1, const char* markedLabel1, const Node* markedNode2, const char* markedLabel2) const
{
    const Node* rootNode;
    const Node* node = this;
    while (node->parentOrShadowHostNode() && !isHTMLBodyElement(*node))
        node = node->parentOrShadowHostNode();
    rootNode = node;

    String startingIndent;
    traverseTreeAndMark(startingIndent, rootNode, markedNode1, markedLabel1, markedNode2, markedLabel2);
}

void Node::showTreeAndMarkInComposedTree(const Node* markedNode1, const char* markedLabel1, const Node* markedNode2, const char* markedLabel2) const
{
    const Node* rootNode;
    const Node* node = this;
    while (node->parentOrShadowHostNode() && !isHTMLBodyElement(*node))
        node = node->parentOrShadowHostNode();
    rootNode = node;

    String startingIndent;
    traverseTreeAndMarkInComposedTree(startingIndent, rootNode, markedNode1, markedLabel1, markedNode2, markedLabel2);
}

void Node::formatForDebugger(char* buffer, unsigned length) const
{
    String result;
    String s;

    s = nodeName();
    if (s.isEmpty())
        result = "<none>";
    else
        result = s;

    strncpy(buffer, result.utf8().data(), length - 1);
}

static ContainerNode* parentOrShadowHostOrFrameOwner(const Node* node)
{
    ContainerNode* parent = node->parentOrShadowHostNode();
    if (!parent && node->document().frame())
        parent = node->document().frame()->deprecatedLocalOwner();
    return parent;
}

static void showSubTreeAcrossFrame(const Node* node, const Node* markedNode, const String& indent)
{
    if (node == markedNode)
        fputs("*", stderr);
    fputs(indent.utf8().data(), stderr);
    node->showNode();
    if (node->isShadowRoot()) {
        if (ShadowRoot* youngerShadowRoot = toShadowRoot(node)->youngerShadowRoot())
            showSubTreeAcrossFrame(youngerShadowRoot, markedNode, indent + "\t");
    } else {
        if (node->isFrameOwnerElement())
            showSubTreeAcrossFrame(toHTMLFrameOwnerElement(node)->contentDocument(), markedNode, indent + "\t");
        if (ShadowRoot* oldestShadowRoot = oldestShadowRootFor(node))
            showSubTreeAcrossFrame(oldestShadowRoot, markedNode, indent + "\t");
    }
    for (Node* child = node->firstChild(); child; child = child->nextSibling())
        showSubTreeAcrossFrame(child, markedNode, indent + "\t");
}

void Node::showTreeForThisAcrossFrame() const
{
    Node* rootNode = const_cast<Node*>(this);
    while (parentOrShadowHostOrFrameOwner(rootNode))
        rootNode = parentOrShadowHostOrFrameOwner(rootNode);
    showSubTreeAcrossFrame(rootNode, this, "");
}

#endif

// --------

Element* Node::enclosingLinkEventParentOrSelf() const
{
    for (Node* node = const_cast<Node*>(this); node; node = ComposedTreeTraversal::parent(*node)) {
        // For imagemaps, the enclosing link node is the associated area element not the image itself.
        // So we don't let images be the enclosingLinkNode, even though isLink sometimes returns true
        // for them.
        if (node->isLink() && !isHTMLImageElement(*node)) {
            // Casting to Element is safe because only HTMLAnchorElement, HTMLImageElement and
            // SVGAElement can return true for isLink().
            return toElement(node);
        }
    }

    return nullptr;
}

const AtomicString& Node::interfaceName() const
{
    return EventTargetNames::Node;
}

ExecutionContext* Node::executionContext() const
{
    return document().contextDocument().get();
}

void Node::didMoveToNewDocument(Document& oldDocument)
{
    TreeScopeAdopter::ensureDidMoveToNewDocumentWasCalled(oldDocument);

    if (const EventTargetData* eventTargetData = this->eventTargetData()) {
        const EventListenerMap& listenerMap = eventTargetData->eventListenerMap;
        if (!listenerMap.isEmpty()) {
            Vector<AtomicString> types = listenerMap.eventTypes();
            for (unsigned i = 0; i < types.size(); ++i)
                document().addListenerTypeIfNeeded(types[i]);
        }
    }

    oldDocument.markers().removeMarkers(this);
    oldDocument.updateRangesAfterNodeMovedToAnotherDocument(*this);
    if (oldDocument.frameHost() && !document().frameHost())
        oldDocument.frameHost()->eventHandlerRegistry().didMoveOutOfFrameHost(*this);
    else if (document().frameHost() && !oldDocument.frameHost())
        document().frameHost()->eventHandlerRegistry().didMoveIntoFrameHost(*this);
    else if (oldDocument.frameHost() != document().frameHost())
        EventHandlerRegistry::didMoveBetweenFrameHosts(*this, oldDocument.frameHost(), document().frameHost());

    if (WillBeHeapVector<OwnPtrWillBeMember<MutationObserverRegistration>>* registry = mutationObserverRegistry()) {
        for (size_t i = 0; i < registry->size(); ++i) {
            document().addMutationObserverTypes(registry->at(i)->mutationTypes());
        }
    }

    if (transientMutationObserverRegistry()) {
        for (MutationObserverRegistration* registration : *transientMutationObserverRegistry())
            document().addMutationObserverTypes(registration->mutationTypes());
    }
}

static inline bool tryAddEventListener(Node* targetNode, const AtomicString& eventType, PassRefPtr<EventListener> listener, bool useCapture)
{
    if (!targetNode->EventTarget::addEventListener(eventType, listener, useCapture))
        return false;

    Document& document = targetNode->document();
    document.addListenerTypeIfNeeded(eventType);
    if (document.frameHost())
        document.frameHost()->eventHandlerRegistry().didAddEventHandler(*targetNode, eventType);

    return true;
}

bool Node::addEventListener(const AtomicString& eventType, PassRefPtr<EventListener> listener, bool useCapture)
{
    return tryAddEventListener(this, eventType, listener, useCapture);
}

static inline bool tryRemoveEventListener(Node* targetNode, const AtomicString& eventType, PassRefPtr<EventListener> listener, bool useCapture)
{
    if (!targetNode->EventTarget::removeEventListener(eventType, listener, useCapture))
        return false;

    // FIXME: Notify Document that the listener has vanished. We need to keep track of a number of
    // listeners for each type, not just a bool - see https://bugs.webkit.org/show_bug.cgi?id=33861
    Document& document = targetNode->document();
    if (document.frameHost())
        document.frameHost()->eventHandlerRegistry().didRemoveEventHandler(*targetNode, eventType);

    return true;
}

bool Node::removeEventListener(const AtomicString& eventType, PassRefPtr<EventListener> listener, bool useCapture)
{
    return tryRemoveEventListener(this, eventType, listener, useCapture);
}

void Node::removeAllEventListeners()
{
    if (hasEventListeners() && document().frameHost())
        document().frameHost()->eventHandlerRegistry().didRemoveAllEventHandlers(*this);
    EventTarget::removeAllEventListeners();
}

void Node::removeAllEventListenersRecursively()
{
    for (Node& node : NodeTraversal::startsAt(this)) {
        node.removeAllEventListeners();
        for (ShadowRoot* root = node.youngestShadowRoot(); root; root = root->olderShadowRoot())
            root->removeAllEventListenersRecursively();
    }
}

using EventTargetDataMap = WillBeHeapHashMap<RawPtrWillBeWeakMember<Node>, OwnPtr<EventTargetData>>;
static EventTargetDataMap& eventTargetDataMap()
{
    DEFINE_STATIC_LOCAL(OwnPtrWillBePersistent<EventTargetDataMap>, map, (adoptPtrWillBeNoop(new EventTargetDataMap())));
    return *map;
}

EventTargetData* Node::eventTargetData()
{
    return hasEventTargetData() ? eventTargetDataMap().get(this) : nullptr;
}

EventTargetData& Node::ensureEventTargetData()
{
    if (hasEventTargetData())
        return *eventTargetDataMap().get(this);
    ASSERT(!eventTargetDataMap().contains(this));
    setHasEventTargetData(true);
    EventTargetData* data = new EventTargetData;
    eventTargetDataMap().set(this, adoptPtr(data));
    return *data;
}

#if !ENABLE(OILPAN)
void Node::clearEventTargetData()
{
    eventTargetDataMap().remove(this);
#if ENABLE(ASSERT)
    setHasEventTargetData(false);
#endif
}
#endif

WillBeHeapVector<OwnPtrWillBeMember<MutationObserverRegistration>>* Node::mutationObserverRegistry()
{
    if (!hasRareData())
        return nullptr;
    NodeMutationObserverData* data = rareData()->mutationObserverData();
    if (!data)
        return nullptr;
    return &data->registry;
}

WillBeHeapHashSet<RawPtrWillBeMember<MutationObserverRegistration>>* Node::transientMutationObserverRegistry()
{
    if (!hasRareData())
        return nullptr;
    NodeMutationObserverData* data = rareData()->mutationObserverData();
    if (!data)
        return nullptr;
    return &data->transientRegistry;
}

template<typename Registry>
static inline void collectMatchingObserversForMutation(WillBeHeapHashMap<RawPtrWillBeMember<MutationObserver>, MutationRecordDeliveryOptions>& observers, Registry* registry, Node& target, MutationObserver::MutationType type, const QualifiedName* attributeName)
{
    if (!registry)
        return;

    for (const auto& registration : *registry) {
        if (registration->shouldReceiveMutationFrom(target, type, attributeName)) {
            MutationRecordDeliveryOptions deliveryOptions = registration->deliveryOptions();
            WillBeHeapHashMap<RawPtrWillBeMember<MutationObserver>, MutationRecordDeliveryOptions>::AddResult result = observers.add(&registration->observer(), deliveryOptions);
            if (!result.isNewEntry)
                result.storedValue->value |= deliveryOptions;
        }
    }
}

void Node::getRegisteredMutationObserversOfType(WillBeHeapHashMap<RawPtrWillBeMember<MutationObserver>, MutationRecordDeliveryOptions>& observers, MutationObserver::MutationType type, const QualifiedName* attributeName)
{
    ASSERT((type == MutationObserver::Attributes && attributeName) || !attributeName);
    collectMatchingObserversForMutation(observers, mutationObserverRegistry(), *this, type, attributeName);
    collectMatchingObserversForMutation(observers, transientMutationObserverRegistry(), *this, type, attributeName);
    for (Node* node = parentNode(); node; node = node->parentNode()) {
        collectMatchingObserversForMutation(observers, node->mutationObserverRegistry(), *this, type, attributeName);
        collectMatchingObserversForMutation(observers, node->transientMutationObserverRegistry(), *this, type, attributeName);
    }
}

void Node::registerMutationObserver(MutationObserver& observer, MutationObserverOptions options, const HashSet<AtomicString>& attributeFilter)
{
    MutationObserverRegistration* registration = nullptr;
    WillBeHeapVector<OwnPtrWillBeMember<MutationObserverRegistration>>& registry = ensureRareData().ensureMutationObserverData().registry;
    for (size_t i = 0; i < registry.size(); ++i) {
        if (&registry[i]->observer() == &observer) {
            registration = registry[i].get();
            registration->resetObservation(options, attributeFilter);
        }
    }

    if (!registration) {
        registry.append(MutationObserverRegistration::create(observer, this, options, attributeFilter));
        registration = registry.last().get();
    }

    document().addMutationObserverTypes(registration->mutationTypes());
}

void Node::unregisterMutationObserver(MutationObserverRegistration* registration)
{
    WillBeHeapVector<OwnPtrWillBeMember<MutationObserverRegistration>>* registry = mutationObserverRegistry();
    ASSERT(registry);
    if (!registry)
        return;

    size_t index = registry->find(registration);
    ASSERT(index != kNotFound);
    if (index == kNotFound)
        return;

    // Deleting the registration may cause this node to be derefed, so we must make sure the Vector operation completes
    // before that, in case |this| is destroyed (see MutationObserverRegistration::m_registrationNodeKeepAlive).
    // FIXME: Simplify the registration/transient registration logic to make this understandable by humans.
    RefPtrWillBeRawPtr<Node> protect(this);
#if ENABLE(OILPAN)
    // The explicit dispose() is needed to have the registration
    // object unregister itself promptly.
    registration->dispose();
#endif
    registry->remove(index);
}

void Node::registerTransientMutationObserver(MutationObserverRegistration* registration)
{
    ensureRareData().ensureMutationObserverData().transientRegistry.add(registration);
}

void Node::unregisterTransientMutationObserver(MutationObserverRegistration* registration)
{
    WillBeHeapHashSet<RawPtrWillBeMember<MutationObserverRegistration>>* transientRegistry = transientMutationObserverRegistry();
    ASSERT(transientRegistry);
    if (!transientRegistry)
        return;

    ASSERT(transientRegistry->contains(registration));
    transientRegistry->remove(registration);
}

void Node::notifyMutationObserversNodeWillDetach()
{
    if (!document().hasMutationObservers())
        return;

    for (Node* node = parentNode(); node; node = node->parentNode()) {
        if (WillBeHeapVector<OwnPtrWillBeMember<MutationObserverRegistration>>* registry = node->mutationObserverRegistry()) {
            const size_t size = registry->size();
            for (size_t i = 0; i < size; ++i)
                registry->at(i)->observedSubtreeNodeWillDetach(*this);
        }

        if (WillBeHeapHashSet<RawPtrWillBeMember<MutationObserverRegistration>>* transientRegistry = node->transientMutationObserverRegistry()) {
            for (auto& registration : *transientRegistry)
                registration->observedSubtreeNodeWillDetach(*this);
        }
    }
}

void Node::handleLocalEvents(Event& event)
{
#ifndef MINIBLINK_NO_DRAG_REGION
    const AtomicString& eventType = event.type();
    
    if (EventTypeNames::mousedown == eventType && event.isMouseEvent() && event.eventPhase() == Event::CAPTURING_PHASE) {
        MouseEvent* mouseEvent = toMouseEvent(&event);
        if (mouseEvent->buttonDown() && LeftButton == mouseEvent->button()) {
            if (LocalFrame* frame = document().frame())
                frame->chromeClient().onMouseDown(this);
        }
    }
#endif

    if (!hasEventTargetData())
        return;

    if (isDisabledFormControl(this) && event.isMouseEvent())
        return;

    fireEventListeners(&event);
}

void Node::dispatchScopedEvent(PassRefPtrWillBeRawPtr<Event> event)
{
    dispatchScopedEventDispatchMediator(EventDispatchMediator::create(event));
}

void Node::dispatchScopedEventDispatchMediator(PassRefPtrWillBeRawPtr<EventDispatchMediator> eventDispatchMediator)
{
    EventDispatcher::dispatchScopedEvent(*this, eventDispatchMediator);
}

bool Node::dispatchEvent(PassRefPtrWillBeRawPtr<Event> event)
{
    if (event->isMouseEvent())
        return EventDispatcher::dispatchEvent(*this, MouseEventDispatchMediator::create(static_pointer_cast<MouseEvent>(event), MouseEventDispatchMediator::SyntheticMouseEvent));
    if (event->isTouchEvent())
        return dispatchTouchEvent(static_pointer_cast<TouchEvent>(event));
    if (event->isPointerEvent())
        return dispatchPointerEvent(static_pointer_cast<PointerEvent>(event));

    return EventDispatcher::dispatchEvent(*this, EventDispatchMediator::create(event));
}

void Node::dispatchSubtreeModifiedEvent()
{
    if (isInShadowTree())
        return;

    ASSERT(!EventDispatchForbiddenScope::isEventDispatchForbidden());

    if (!document().hasListenerType(Document::DOMSUBTREEMODIFIED_LISTENER))
        return;

    dispatchScopedEvent(MutationEvent::create(EventTypeNames::DOMSubtreeModified, true));
}

bool Node::dispatchDOMActivateEvent(int detail, PassRefPtrWillBeRawPtr<Event> underlyingEvent)
{
    ASSERT(!EventDispatchForbiddenScope::isEventDispatchForbidden());
    RefPtrWillBeRawPtr<UIEvent> event = UIEvent::create(EventTypeNames::DOMActivate, true, true, document().domWindow(), detail);
    event->setUnderlyingEvent(underlyingEvent);
    dispatchScopedEvent(event);
    return event->defaultHandled();
}

bool Node::dispatchKeyEvent(const PlatformKeyboardEvent& event)
{
    return EventDispatcher::dispatchEvent(*this, KeyboardEventDispatchMediator::create(KeyboardEvent::create(event, document().domWindow())));
}

bool Node::dispatchMouseEvent(const PlatformMouseEvent& event, const AtomicString& eventType, int detail, Node* relatedTarget)
{
    return EventDispatcher::dispatchEvent(*this, MouseEventDispatchMediator::create(MouseEvent::create(eventType, document().domWindow(), event, detail, relatedTarget)));
}

bool Node::dispatchGestureEvent(const PlatformGestureEvent& event)
{
    RefPtrWillBeRawPtr<GestureEvent> gestureEvent = GestureEvent::create(document().domWindow(), event);
    if (!gestureEvent.get())
        return false;
    return EventDispatcher::dispatchEvent(*this, GestureEventDispatchMediator::create(gestureEvent));
}

bool Node::dispatchTouchEvent(PassRefPtrWillBeRawPtr<TouchEvent> event)
{
    return EventDispatcher::dispatchEvent(*this, TouchEventDispatchMediator::create(event));
}

bool Node::dispatchPointerEvent(PassRefPtrWillBeRawPtr<PointerEvent> event)
{
    return EventDispatcher::dispatchEvent(*this, PointerEventDispatchMediator::create(event));
}

void Node::dispatchSimulatedClick(Event* underlyingEvent, SimulatedClickMouseEventOptions eventOptions)
{
    EventDispatcher::dispatchSimulatedClick(*this, underlyingEvent, eventOptions);
}

bool Node::dispatchWheelEvent(const PlatformWheelEvent& event)
{
    return EventDispatcher::dispatchEvent(*this, WheelEventDispatchMediator::create(event, document().domWindow()));
}

void Node::dispatchInputEvent()
{
    dispatchScopedEvent(Event::createBubble(EventTypeNames::input));
}

void Node::defaultEventHandler(Event* event)
{
    if (event->target() != this)
        return;
    const AtomicString& eventType = event->type();
    if (eventType == EventTypeNames::keydown || eventType == EventTypeNames::keypress) {
        if (event->isKeyboardEvent()) {
            if (LocalFrame* frame = document().frame())
                frame->eventHandler().defaultKeyboardEventHandler(toKeyboardEvent(event));
        }
    } else if (eventType == EventTypeNames::click) {
        int detail = event->isUIEvent() ? static_cast<UIEvent*>(event)->detail() : 0;
        if (dispatchDOMActivateEvent(detail, event))
            event->setDefaultHandled();
    } else if (eventType == EventTypeNames::contextmenu) {
        if (Page* page = document().page())
            page->contextMenuController().handleContextMenuEvent(event);
    } else if (eventType == EventTypeNames::textInput) {
        if (event->hasInterface(EventNames::TextEvent)) {
            if (LocalFrame* frame = document().frame())
                frame->eventHandler().defaultTextInputEventHandler(toTextEvent(event));
        }
#if OS(WIN)
    } else if (eventType == EventTypeNames::mousedown && event->isMouseEvent()) {
        MouseEvent* mouseEvent = toMouseEvent(event);
        if (mouseEvent->button() == MiddleButton) {
            if (enclosingLinkEventParentOrSelf())
                return;

            // Avoid that canBeScrolledAndHasScrollableArea changes layout tree
            // structure.
            // FIXME: We should avoid synchronous layout if possible. We can
            // remove this synchronous layout if we avoid synchronous layout in
            // LayoutTextControlSingleLine::scrollHeight
            document().updateLayoutIgnorePendingStylesheets();
            LayoutObject* layoutObject = this->layoutObject();
            while (layoutObject && (!layoutObject->isBox() || !toLayoutBox(layoutObject)->canBeScrolledAndHasScrollableArea()))
                layoutObject = layoutObject->parent();

            if (layoutObject) {
                if (LocalFrame* frame = document().frame())
                    frame->eventHandler().startPanScrolling(layoutObject);
            }
        }
#endif
    } else if ((eventType == EventTypeNames::wheel || eventType == EventTypeNames::mousewheel) && event->hasInterface(EventNames::WheelEvent)) {
        WheelEvent* wheelEvent = toWheelEvent(event);

        // If we don't have a layoutObject, send the wheel event to the first node we find with a layoutObject.
        // This is needed for <option> and <optgroup> elements so that <select>s get a wheel scroll.
        Node* startNode = this;
        while (startNode && !startNode->layoutObject())
            startNode = startNode->parentOrShadowHostNode();

        if (startNode && startNode->layoutObject()) {
            if (LocalFrame* frame = document().frame())
                frame->eventHandler().defaultWheelEventHandler(startNode, wheelEvent);
        }
    } else if (event->type() == EventTypeNames::webkitEditableContentChanged) {
        dispatchInputEvent();
    }
}

void Node::willCallDefaultEventHandler(const Event&)
{
}

bool Node::willRespondToMouseMoveEvents()
{
    if (isDisabledFormControl(this))
        return false;
    return hasEventListeners(EventTypeNames::mousemove) || hasEventListeners(EventTypeNames::mouseover) || hasEventListeners(EventTypeNames::mouseout);
}

bool Node::willRespondToMouseClickEvents()
{
    if (isDisabledFormControl(this))
        return false;
    return isContentEditable(UserSelectAllIsAlwaysNonEditable) || hasEventListeners(EventTypeNames::mouseup) || hasEventListeners(EventTypeNames::mousedown) || hasEventListeners(EventTypeNames::click) || hasEventListeners(EventTypeNames::DOMActivate);
}

bool Node::willRespondToTouchEvents()
{
    if (isDisabledFormControl(this))
        return false;
    return hasEventListeners(EventTypeNames::touchstart) || hasEventListeners(EventTypeNames::touchmove) || hasEventListeners(EventTypeNames::touchcancel) || hasEventListeners(EventTypeNames::touchend);
}

#if !ENABLE(OILPAN)
// This is here for inlining
inline void TreeScope::removedLastRefToScope()
{
    ASSERT_WITH_SECURITY_IMPLICATION(!deletionHasBegun());
    if (m_guardRefCount) {
        // If removing a child removes the last self-only ref, we don't
        // want the scope to be destructed until after
        // removeDetachedChildren returns, so we guard ourselves with an
        // extra self-only ref.
        guardRef();
        dispose();
#if ENABLE(ASSERT)
        // We need to do this right now since guardDeref() can delete this.
        rootNode().m_inRemovedLastRefFunction = false;
#endif
        guardDeref();
    } else {
#if ENABLE(ASSERT)
        rootNode().m_inRemovedLastRefFunction = false;
#endif
#if ENABLE(SECURITY_ASSERT)
        beginDeletion();
#endif
        delete this;
    }
}

// It's important not to inline removedLastRef, because we don't want to inline the code to
// delete a Node at each deref call site.
void Node::removedLastRef()
{
    // An explicit check for Document here is better than a virtual function since it is
    // faster for non-Document nodes, and because the call to removedLastRef that is inlined
    // at all deref call sites is smaller if it's a non-virtual function.
    if (isTreeScope()) {
        treeScope().removedLastRefToScope();
        return;
    }

#if ENABLE(SECURITY_ASSERT)
    m_deletionHasBegun = true;
#endif
    delete this;
}
#endif

unsigned Node::connectedSubframeCount() const
{
    return hasRareData() ? rareData()->connectedSubframeCount() : 0;
}

void Node::incrementConnectedSubframeCount(unsigned amount)
{
    ASSERT(isContainerNode());
    ensureRareData().incrementConnectedSubframeCount(amount);
}

void Node::decrementConnectedSubframeCount(unsigned amount)
{
    rareData()->decrementConnectedSubframeCount(amount);
}

void Node::updateAncestorConnectedSubframeCountForInsertion() const
{
    unsigned count = connectedSubframeCount();

    if (!count)
        return;

    for (Node* node = parentOrShadowHostNode(); node; node = node->parentOrShadowHostNode())
        node->incrementConnectedSubframeCount(count);
}

PassRefPtrWillBeRawPtr<StaticNodeList> Node::getDestinationInsertionPoints()
{
    updateDistribution();
    WillBeHeapVector<RawPtrWillBeMember<InsertionPoint>, 8> insertionPoints;
    collectDestinationInsertionPoints(*this, insertionPoints);
    WillBeHeapVector<RefPtrWillBeMember<Node>> filteredInsertionPoints;
    for (size_t i = 0; i < insertionPoints.size(); ++i) {
        InsertionPoint* insertionPoint = insertionPoints[i];
        ASSERT(insertionPoint->containingShadowRoot());
        if (insertionPoint->containingShadowRoot()->type() != ShadowRootType::UserAgent)
            filteredInsertionPoints.append(insertionPoint);
    }
    return StaticNodeList::adopt(filteredInsertionPoints);
}

void Node::setFocus(bool flag)
{
    document().userActionElements().setFocused(this, flag);
}

void Node::setActive(bool flag)
{
    document().userActionElements().setActive(this, flag);
}

void Node::setHovered(bool flag)
{
    document().userActionElements().setHovered(this, flag);
}

bool Node::isUserActionElementActive() const
{
    ASSERT(isUserActionElement());
    return document().userActionElements().isActive(this);
}

bool Node::isUserActionElementInActiveChain() const
{
    ASSERT(isUserActionElement());
    return document().userActionElements().isInActiveChain(this);
}

bool Node::isUserActionElementHovered() const
{
    ASSERT(isUserActionElement());
    return document().userActionElements().isHovered(this);
}

bool Node::isUserActionElementFocused() const
{
    ASSERT(isUserActionElement());
    return document().userActionElements().isFocused(this);
}

void Node::setCustomElementState(CustomElementState newState)
{
    CustomElementState oldState = customElementState();

    switch (newState) {
    case NotCustomElement:
        ASSERT_NOT_REACHED(); // Everything starts in this state
        return;

    case WaitingForUpgrade:
        ASSERT(NotCustomElement == oldState);
        break;

    case Upgraded:
        ASSERT(WaitingForUpgrade == oldState);
        break;
    }

    ASSERT(isHTMLElement() || isSVGElement());
    setFlag(CustomElementFlag);
    setFlag(newState == Upgraded, CustomElementUpgradedFlag);

    if (oldState == NotCustomElement || newState == Upgraded)
        setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::createWithExtraData(StyleChangeReason::PseudoClass, StyleChangeExtraData::Unresolved)); // :unresolved has changed
}

DEFINE_TRACE(Node)
{
#if ENABLE(OILPAN)
    visitor->trace(m_parentOrShadowHostNode);
    visitor->trace(m_previous);
    visitor->trace(m_next);
    // rareData() and m_data.m_layoutObject share their storage. We have to trace
    // only one of them.
    if (hasRareData())
        visitor->trace(rareData());

    visitor->trace(m_treeScope);
#endif
    EventTarget::trace(visitor);
}

unsigned Node::lengthOfContents() const
{
    // This switch statement must be consistent with that of Range::processContentsBetweenOffsets.
    switch (nodeType()) {
    case Node::TEXT_NODE:
    case Node::CDATA_SECTION_NODE:
    case Node::COMMENT_NODE:
        return toCharacterData(this)->length();
    case Node::PROCESSING_INSTRUCTION_NODE:
        return toProcessingInstruction(this)->data().length();
    case Node::ELEMENT_NODE:
    case Node::DOCUMENT_NODE:
    case Node::DOCUMENT_FRAGMENT_NODE:
        return toContainerNode(this)->countChildren();
    case Node::ATTRIBUTE_NODE:
    case Node::DOCUMENT_TYPE_NODE:
        return 0;
    }
    ASSERT_NOT_REACHED();
    return 0;
}

v8::Local<v8::Object> Node::wrap(v8::Isolate* isolate, v8::Local<v8::Object> creationContext)
{
    // It's possible that no one except for the new wrapper owns this object at
    // this moment, so we have to prevent GC to collect this object until the
    // object gets associated with the wrapper.
    RefPtrWillBeRawPtr<Node> protect(this);

    ASSERT(!DOMDataStore::containsWrapper(this, isolate));

    const WrapperTypeInfo* wrapperType = wrapperTypeInfo();

    v8::Local<v8::Object> wrapper = V8DOMWrapper::createWrapper(isolate, creationContext, wrapperType, this);
    if (UNLIKELY(wrapper.IsEmpty()))
        return wrapper;

    wrapperType->installConditionallyEnabledProperties(wrapper, isolate);
    return associateWithWrapper(isolate, wrapperType, wrapper);
}

v8::Local<v8::Object> Node::associateWithWrapper(v8::Isolate* isolate, const WrapperTypeInfo* wrapperType, v8::Local<v8::Object> wrapper)
{
    return V8DOMWrapper::associateObjectWithWrapper(isolate, this, wrapperType, wrapper);
}

} // namespace blink

#ifndef NDEBUG

void showNode(const blink::Node* node)
{
    if (node)
        node->showNode("");
    else
        fprintf(stderr, "Cannot showNode for (nil)\n");
}

void showTree(const blink::Node* node)
{
    if (node)
        node->showTreeForThis();
    else
        fprintf(stderr, "Cannot showTree for (nil)\n");
}

void showNodePath(const blink::Node* node)
{
    if (node)
        node->showNodePathForThis();
    else
        fprintf(stderr, "Cannot showNodePath for (nil)\n");
}

#endif

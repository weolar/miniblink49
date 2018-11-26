/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2011 Google Inc. All rights reserved.
 * Copyright (C) 2009 Joseph Pecoraro
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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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
#include "core/inspector/InspectorDOMAgent.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/InputTypeNames.h"
#include "core/dom/Attr.h"
#include "core/dom/CharacterData.h"
#include "core/dom/ContainerNode.h"
#include "core/dom/DOMException.h"
#include "core/dom/DOMNodeIds.h"
#include "core/dom/Document.h"
#include "core/dom/DocumentFragment.h"
#include "core/dom/DocumentType.h"
#include "core/dom/Element.h"
#include "core/dom/Node.h"
#include "core/dom/PseudoElement.h"
#include "core/dom/StaticNodeList.h"
#include "core/dom/Text.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/dom/shadow/InsertionPoint.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/markup.h"
#include "core/events/EventListener.h"
#include "core/events/EventTarget.h"
#include "core/fileapi/File.h"
#include "core/fileapi/FileList.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLLinkElement.h"
#include "core/html/HTMLTemplateElement.h"
#include "core/html/imports/HTMLImportChild.h"
#include "core/html/imports/HTMLImportLoader.h"
#include "core/inspector/DOMEditor.h"
#include "core/inspector/DOMPatchSupport.h"
#include "core/inspector/IdentifiersFactory.h"
#include "core/inspector/InjectedScriptHost.h"
#include "core/inspector/InjectedScriptManager.h"
#include "core/inspector/InspectorHighlight.h"
#include "core/inspector/InspectorHistory.h"
#include "core/inspector/InspectorOverlay.h"
#include "core/inspector/InspectorPageAgent.h"
#include "core/inspector/InspectorState.h"
#include "core/inspector/InstrumentingAgents.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutView.h"
#include "core/loader/DocumentLoader.h"
#include "core/page/FrameTree.h"
#include "core/page/Page.h"
#include "core/xml/DocumentXPathEvaluator.h"
#include "core/xml/XPathResult.h"
#include "platform/PlatformGestureEvent.h"
#include "platform/PlatformMouseEvent.h"
#include "platform/PlatformTouchEvent.h"
#include "wtf/ListHashSet.h"
#include "wtf/text/CString.h"
#include "wtf/text/WTFString.h"

namespace blink {

using namespace HTMLNames;

namespace DOMAgentState {
static const char domAgentEnabled[] = "domAgentEnabled";
};

static const size_t maxTextSize = 10000;
static const UChar ellipsisUChar[] = { 0x2026, 0 };

static Color parseColor(const RefPtr<JSONObject>* colorObject)
{
    if (!colorObject || !(*colorObject))
        return Color::transparent;

    int r;
    int g;
    int b;
    bool success = (*colorObject)->getNumber("r", &r);
    success |= (*colorObject)->getNumber("g", &g);
    success |= (*colorObject)->getNumber("b", &b);
    if (!success)
        return Color::transparent;

    double a;
    success = (*colorObject)->getNumber("a", &a);
    if (!success)
        return Color(r, g, b);

    // Clamp alpha to the [0..1] range.
    if (a < 0)
        a = 0;
    else if (a > 1)
        a = 1;

    return Color(r, g, b, static_cast<int>(a * 255));
}

static Color parseConfigColor(const String& fieldName, JSONObject* configObject)
{
    const RefPtr<JSONObject> colorObject = configObject->getObject(fieldName);
    return parseColor(&colorObject);
}

static bool parseQuad(const RefPtr<JSONArray>& quadArray, FloatQuad* quad)
{
    if (!quadArray)
        return false;
    const size_t coordinatesInQuad = 8;
    double coordinates[coordinatesInQuad];
    if (quadArray->length() != coordinatesInQuad)
        return false;
    for (size_t i = 0; i < coordinatesInQuad; ++i) {
        if (!quadArray->get(i)->asNumber(coordinates + i))
            return false;
    }
    quad->setP1(FloatPoint(coordinates[0], coordinates[1]));
    quad->setP2(FloatPoint(coordinates[2], coordinates[3]));
    quad->setP3(FloatPoint(coordinates[4], coordinates[5]));
    quad->setP4(FloatPoint(coordinates[6], coordinates[7]));

    return true;
}

static Node* hoveredNodeForPoint(LocalFrame* frame, const IntPoint& pointInRootFrame, bool ignorePointerEventsNone)
{
    HitTestRequest::HitTestRequestType hitType = HitTestRequest::Move | HitTestRequest::ReadOnly | HitTestRequest::AllowChildFrameContent;
    if (ignorePointerEventsNone)
        hitType |= HitTestRequest::IgnorePointerEventsNone;
    HitTestRequest request(hitType);
    HitTestResult result(request, frame->view()->rootFrameToContents(pointInRootFrame));
    frame->contentLayoutObject()->hitTest(result);
    Node* node = result.innerPossiblyPseudoNode();
    while (node && node->nodeType() == Node::TEXT_NODE)
        node = node->parentNode();
    return node;
}

static Node* hoveredNodeForEvent(LocalFrame* frame, const PlatformGestureEvent& event, bool ignorePointerEventsNone)
{
    return hoveredNodeForPoint(frame, event.position(), ignorePointerEventsNone);
}

static Node* hoveredNodeForEvent(LocalFrame* frame, const PlatformMouseEvent& event, bool ignorePointerEventsNone)
{
    return hoveredNodeForPoint(frame, event.position(), ignorePointerEventsNone);
}

static Node* hoveredNodeForEvent(LocalFrame* frame, const PlatformTouchEvent& event, bool ignorePointerEventsNone)
{
    const Vector<PlatformTouchPoint>& points = event.touchPoints();
    if (!points.size())
        return nullptr;
    return hoveredNodeForPoint(frame, roundedIntPoint(points[0].pos()), ignorePointerEventsNone);
}

class InspectorRevalidateDOMTask final : public NoBaseWillBeGarbageCollectedFinalized<InspectorRevalidateDOMTask> {
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(InspectorRevalidateDOMTask);
public:
    explicit InspectorRevalidateDOMTask(InspectorDOMAgent*);
    void scheduleStyleAttrRevalidationFor(Element*);
    void reset() { m_timer.stop(); }
    void onTimer(Timer<InspectorRevalidateDOMTask>*);
    DECLARE_TRACE();

private:
    RawPtrWillBeMember<InspectorDOMAgent> m_domAgent;
    Timer<InspectorRevalidateDOMTask> m_timer;
    WillBeHeapHashSet<RefPtrWillBeMember<Element> > m_styleAttrInvalidatedElements;
};

InspectorRevalidateDOMTask::InspectorRevalidateDOMTask(InspectorDOMAgent* domAgent)
    : m_domAgent(domAgent)
    , m_timer(this, &InspectorRevalidateDOMTask::onTimer)
{
}

void InspectorRevalidateDOMTask::scheduleStyleAttrRevalidationFor(Element* element)
{
    m_styleAttrInvalidatedElements.add(element);
    if (!m_timer.isActive())
        m_timer.startOneShot(0, FROM_HERE);
}

void InspectorRevalidateDOMTask::onTimer(Timer<InspectorRevalidateDOMTask>*)
{
    // The timer is stopped on m_domAgent destruction, so this method will never be called after m_domAgent has been destroyed.
    WillBeHeapVector<RawPtrWillBeMember<Element> > elements;
    for (auto& attribute : m_styleAttrInvalidatedElements)
        elements.append(attribute.get());
    m_domAgent->styleAttributeInvalidated(elements);
    m_styleAttrInvalidatedElements.clear();
}

DEFINE_TRACE(InspectorRevalidateDOMTask)
{
    visitor->trace(m_domAgent);
#if ENABLE(OILPAN)
    visitor->trace(m_styleAttrInvalidatedElements);
#endif
}

String InspectorDOMAgent::toErrorString(ExceptionState& exceptionState)
{
    if (exceptionState.hadException())
        return DOMException::getErrorName(exceptionState.code()) + " " + exceptionState.message();
    return "";
}

bool InspectorDOMAgent::getPseudoElementType(PseudoId pseudoId, TypeBuilder::DOM::PseudoType::Enum* type)
{
    switch (pseudoId) {
    case FIRST_LINE:
        *type = TypeBuilder::DOM::PseudoType::First_line;
        return true;
    case FIRST_LETTER:
        *type = TypeBuilder::DOM::PseudoType::First_letter;
        return true;
    case BEFORE:
        *type = TypeBuilder::DOM::PseudoType::Before;
        return true;
    case AFTER:
        *type = TypeBuilder::DOM::PseudoType::After;
        return true;
    case BACKDROP:
        *type = TypeBuilder::DOM::PseudoType::Backdrop;
        return true;
    case SELECTION:
        *type = TypeBuilder::DOM::PseudoType::Selection;
        return true;
    case FIRST_LINE_INHERITED:
        *type = TypeBuilder::DOM::PseudoType::First_line_inherited;
        return true;
    case SCROLLBAR:
        *type = TypeBuilder::DOM::PseudoType::Scrollbar;
        return true;
    case SCROLLBAR_THUMB:
        *type = TypeBuilder::DOM::PseudoType::Scrollbar_thumb;
        return true;
    case SCROLLBAR_BUTTON:
        *type = TypeBuilder::DOM::PseudoType::Scrollbar_button;
        return true;
    case SCROLLBAR_TRACK:
        *type = TypeBuilder::DOM::PseudoType::Scrollbar_track;
        return true;
    case SCROLLBAR_TRACK_PIECE:
        *type = TypeBuilder::DOM::PseudoType::Scrollbar_track_piece;
        return true;
    case SCROLLBAR_CORNER:
        *type = TypeBuilder::DOM::PseudoType::Scrollbar_corner;
        return true;
    case RESIZER:
        *type = TypeBuilder::DOM::PseudoType::Resizer;
        return true;
    case INPUT_LIST_BUTTON:
        *type = TypeBuilder::DOM::PseudoType::Input_list_button;
        return true;
    default:
        return false;
    }
}

InspectorDOMAgent::InspectorDOMAgent(InspectorPageAgent* pageAgent, InjectedScriptManager* injectedScriptManager, InspectorOverlay* overlay)
    : InspectorBaseAgent<InspectorDOMAgent, InspectorFrontend::DOM>("DOM")
    , m_pageAgent(pageAgent)
    , m_injectedScriptManager(injectedScriptManager)
    , m_overlay(overlay)
    , m_domListener(nullptr)
    , m_documentNodeToIdMap(adoptPtrWillBeNoop(new NodeToIdMap()))
    , m_lastNodeId(1)
    , m_searchingForNode(NotSearching)
    , m_suppressAttributeModifiedEvent(false)
    , m_listener(nullptr)
    , m_backendNodeIdToInspect(0)
{
}

InspectorDOMAgent::~InspectorDOMAgent()
{
#if !ENABLE(OILPAN)
    setDocument(nullptr);
    ASSERT(m_searchingForNode == NotSearching);
#endif
}

void InspectorDOMAgent::restore()
{
    if (!enabled())
        return;
    innerEnable();
}

WillBeHeapVector<RawPtrWillBeMember<Document> > InspectorDOMAgent::documents()
{
    WillBeHeapVector<RawPtrWillBeMember<Document> > result;
    for (Frame* frame = m_document->frame(); frame; frame = frame->tree().traverseNext()) {
        if (!frame->isLocalFrame())
            continue;
        Document* document = toLocalFrame(frame)->document();
        if (!document)
            continue;
        result.append(document);
    }
    return result;
}

void InspectorDOMAgent::setDOMListener(DOMListener* listener)
{
    m_domListener = listener;
}

void InspectorDOMAgent::setDocument(Document* doc)
{
    if (doc == m_document.get())
        return;

    discardFrontendBindings();
    m_document = doc;

    if (!enabled())
        return;

    // Immediately communicate 0 document or document that has finished loading.
    if (!doc || !doc->parsing())
        frontend()->documentUpdated();
}

void InspectorDOMAgent::releaseDanglingNodes()
{
    m_danglingNodeToIdMaps.clear();
}

int InspectorDOMAgent::bind(Node* node, NodeToIdMap* nodesMap)
{
    int id = nodesMap->get(node);
    if (id)
        return id;
    id = m_lastNodeId++;
    nodesMap->set(node, id);
    m_idToNode.set(id, node);
    m_idToNodesMap.set(id, nodesMap);
    return id;
}

void InspectorDOMAgent::unbind(Node* node, NodeToIdMap* nodesMap)
{
    int id = nodesMap->get(node);
    if (!id)
        return;

    m_idToNode.remove(id);
    m_idToNodesMap.remove(id);

    if (node->isFrameOwnerElement()) {
        Document* contentDocument = toHTMLFrameOwnerElement(node)->contentDocument();
        if (m_domListener)
            m_domListener->didRemoveDocument(contentDocument);
        if (contentDocument)
            unbind(contentDocument, nodesMap);
    }

    for (ShadowRoot* root = node->youngestShadowRoot(); root; root = root->olderShadowRoot())
        unbind(root, nodesMap);

    if (node->isElementNode()) {
        Element* element = toElement(node);
        if (element->pseudoElement(BEFORE))
            unbind(element->pseudoElement(BEFORE), nodesMap);
        if (element->pseudoElement(AFTER))
            unbind(element->pseudoElement(AFTER), nodesMap);

        if (isHTMLLinkElement(*element)) {
            HTMLLinkElement& linkElement = toHTMLLinkElement(*element);
            if (linkElement.isImport() && linkElement.import())
                unbind(linkElement.import(), nodesMap);
        }
    }

    nodesMap->remove(node);
    if (m_domListener)
        m_domListener->didRemoveDOMNode(node);

    bool childrenRequested = m_childrenRequested.contains(id);
    if (childrenRequested) {
        // Unbind subtree known to client recursively.
        m_childrenRequested.remove(id);
        Node* child = innerFirstChild(node);
        while (child) {
            unbind(child, nodesMap);
            child = innerNextSibling(child);
        }
    }
    if (nodesMap == m_documentNodeToIdMap.get())
        m_cachedChildCount.remove(id);
}

Node* InspectorDOMAgent::assertNode(ErrorString* errorString, int nodeId)
{
    Node* node = nodeForId(nodeId);
    if (!node) {
        *errorString = "Could not find node with given id";
        return nullptr;
    }
    return node;
}

Document* InspectorDOMAgent::assertDocument(ErrorString* errorString, int nodeId)
{
    Node* node = assertNode(errorString, nodeId);
    if (!node)
        return nullptr;

    if (!(node->isDocumentNode())) {
        *errorString = "Document is not available";
        return nullptr;
    }
    return toDocument(node);
}

Element* InspectorDOMAgent::assertElement(ErrorString* errorString, int nodeId)
{
    Node* node = assertNode(errorString, nodeId);
    if (!node)
        return nullptr;

    if (!node->isElementNode()) {
        *errorString = "Node is not an Element";
        return nullptr;
    }
    return toElement(node);
}

static ShadowRoot* userAgentShadowRoot(Node* node)
{
    if (!node || !node->isInShadowTree())
        return nullptr;

    Node* candidate = node;
    while (candidate && !candidate->isShadowRoot())
        candidate = candidate->parentOrShadowHostNode();
    ASSERT(candidate);
    ShadowRoot* shadowRoot = toShadowRoot(candidate);

    return shadowRoot->type() == ShadowRootType::UserAgent ? shadowRoot : nullptr;
}

Node* InspectorDOMAgent::assertEditableNode(ErrorString* errorString, int nodeId)
{
    Node* node = assertNode(errorString, nodeId);
    if (!node)
        return nullptr;

    if (node->isInShadowTree()) {
        if (node->isShadowRoot()) {
            *errorString = "Cannot edit shadow roots";
            return nullptr;
        }
        if (userAgentShadowRoot(node)) {
            *errorString = "Cannot edit nodes from user-agent shadow trees";
            return nullptr;
        }
    }

    if (node->isPseudoElement()) {
        *errorString = "Cannot edit pseudo elements";
        return nullptr;
    }

    return node;
}

Node* InspectorDOMAgent::assertEditableChildNode(ErrorString* errorString, Element* parentElement, int nodeId)
{
    Node* node = assertEditableNode(errorString, nodeId);
    if (!node)
        return nullptr;
    if (node->parentNode() != parentElement) {
        *errorString = "Anchor node must be child of the target element";
        return nullptr;
    }
    return node;
}

Element* InspectorDOMAgent::assertEditableElement(ErrorString* errorString, int nodeId)
{
    Element* element = assertElement(errorString, nodeId);
    if (!element)
        return nullptr;

    if (element->isInShadowTree() && userAgentShadowRoot(element)) {
        *errorString = "Cannot edit elements from user-agent shadow trees";
        return nullptr;
    }

    if (element->isPseudoElement()) {
        *errorString = "Cannot edit pseudo elements";
        return nullptr;
    }

    return element;
}

void InspectorDOMAgent::innerEnable()
{
    m_state->setBoolean(DOMAgentState::domAgentEnabled, true);
    m_history = adoptPtrWillBeNoop(new InspectorHistory());
    m_domEditor = adoptPtrWillBeNoop(new DOMEditor(m_history.get()));
    m_document = m_pageAgent->inspectedFrame()->document();
    m_instrumentingAgents->setInspectorDOMAgent(this);
    if (m_listener)
        m_listener->domAgentWasEnabled();
    if (m_backendNodeIdToInspect)
        frontend()->inspectNodeRequested(m_backendNodeIdToInspect);
    m_backendNodeIdToInspect = 0;
}

void InspectorDOMAgent::enable(ErrorString*)
{
    if (enabled())
        return;
    innerEnable();
}

bool InspectorDOMAgent::enabled() const
{
    return m_state->getBoolean(DOMAgentState::domAgentEnabled);
}

void InspectorDOMAgent::disable(ErrorString* errorString)
{
    if (!enabled()) {
        if (errorString)
            *errorString = "DOM agent hasn't been enabled";
        return;
    }
    m_state->setBoolean(DOMAgentState::domAgentEnabled, false);
    setSearchingForNode(errorString, NotSearching, nullptr);
    m_instrumentingAgents->setInspectorDOMAgent(nullptr);
    m_history.clear();
    m_domEditor.clear();
    setDocument(nullptr);
    if (m_listener)
        m_listener->domAgentWasDisabled();
}

void InspectorDOMAgent::getDocument(ErrorString* errorString, RefPtr<TypeBuilder::DOM::Node>& root)
{
    // Backward compatibility. Mark agent as enabled when it requests document.
    if (!enabled())
        innerEnable();

    if (!m_document) {
        *errorString = "Document is not available";
        return;
    }

    discardFrontendBindings();

    root = buildObjectForNode(m_document.get(), 2, m_documentNodeToIdMap.get());
}

void InspectorDOMAgent::pushChildNodesToFrontend(int nodeId, int depth)
{
    Node* node = nodeForId(nodeId);
    if (!node || (!node->isElementNode() && !node->isDocumentNode() && !node->isDocumentFragment()))
        return;

    NodeToIdMap* nodeMap = m_idToNodesMap.get(nodeId);

    if (m_childrenRequested.contains(nodeId)) {
        if (depth <= 1)
            return;

        depth--;

        for (node = innerFirstChild(node); node; node = innerNextSibling(node)) {
            int childNodeId = nodeMap->get(node);
            ASSERT(childNodeId);
            pushChildNodesToFrontend(childNodeId, depth);
        }

        return;
    }

    RefPtr<TypeBuilder::Array<TypeBuilder::DOM::Node> > children = buildArrayForContainerChildren(node, depth, nodeMap);
    frontend()->setChildNodes(nodeId, children.release());
}

void InspectorDOMAgent::discardFrontendBindings()
{
    if (m_history)
        m_history->reset();
    m_searchResults.clear();
    m_documentNodeToIdMap->clear();
    m_idToNode.clear();
    m_idToNodesMap.clear();
    releaseDanglingNodes();
    m_childrenRequested.clear();
    m_cachedChildCount.clear();
    if (m_revalidateTask)
        m_revalidateTask->reset();
}

Node* InspectorDOMAgent::nodeForId(int id)
{
    if (!id)
        return nullptr;

    WillBeHeapHashMap<int, RawPtrWillBeMember<Node> >::iterator it = m_idToNode.find(id);
    if (it != m_idToNode.end())
        return it->value;
    return nullptr;
}

void InspectorDOMAgent::requestChildNodes(ErrorString* errorString, int nodeId, const int* depth)
{
    int sanitizedDepth;

    if (!depth)
        sanitizedDepth = 1;
    else if (*depth == -1)
        sanitizedDepth = INT_MAX;
    else if (*depth > 0)
        sanitizedDepth = *depth;
    else {
        *errorString = "Please provide a positive integer as a depth or -1 for entire subtree";
        return;
    }

    pushChildNodesToFrontend(nodeId, sanitizedDepth);
}

void InspectorDOMAgent::querySelector(ErrorString* errorString, int nodeId, const String& selectors, int* elementId)
{
    *elementId = 0;
    Node* node = assertNode(errorString, nodeId);
    if (!node || !node->isContainerNode())
        return;

    TrackExceptionState exceptionState;
    RefPtrWillBeRawPtr<Element> element = toContainerNode(node)->querySelector(AtomicString(selectors), exceptionState);
    if (exceptionState.hadException()) {
        *errorString = "DOM Error while querying";
        return;
    }

    if (element)
        *elementId = pushNodePathToFrontend(element.get());
}

void InspectorDOMAgent::querySelectorAll(ErrorString* errorString, int nodeId, const String& selectors, RefPtr<TypeBuilder::Array<int> >& result)
{
    Node* node = assertNode(errorString, nodeId);
    if (!node || !node->isContainerNode())
        return;

    TrackExceptionState exceptionState;
    RefPtrWillBeRawPtr<StaticElementList> elements = toContainerNode(node)->querySelectorAll(AtomicString(selectors), exceptionState);
    if (exceptionState.hadException()) {
        *errorString = "DOM Error while querying";
        return;
    }

    result = TypeBuilder::Array<int>::create();

    for (unsigned i = 0; i < elements->length(); ++i)
        result->addItem(pushNodePathToFrontend(elements->item(i)));
}

int InspectorDOMAgent::pushNodePathToFrontend(Node* nodeToPush, NodeToIdMap* nodeMap)
{
    ASSERT(nodeToPush); // Invalid input
    // InspectorDOMAgent might have been resetted already. See crbug.com/450491
    if (!m_document)
        return 0;
    if (!m_documentNodeToIdMap->contains(m_document))
        return 0;

    // Return id in case the node is known.
    int result = nodeMap->get(nodeToPush);
    if (result)
        return result;

    Node* node = nodeToPush;
    WillBeHeapVector<RawPtrWillBeMember<Node> > path;

    while (true) {
        Node* parent = innerParentNode(node);
        if (!parent)
            return 0;
        path.append(parent);
        if (nodeMap->get(parent))
            break;
        node = parent;
    }

    for (int i = path.size() - 1; i >= 0; --i) {
        int nodeId = nodeMap->get(path.at(i).get());
        ASSERT(nodeId);
        pushChildNodesToFrontend(nodeId);
    }
    return nodeMap->get(nodeToPush);
}

int InspectorDOMAgent::pushNodePathToFrontend(Node* nodeToPush)
{
    if (!m_document)
        return 0;

    int nodeId = pushNodePathToFrontend(nodeToPush, m_documentNodeToIdMap.get());
    if (nodeId)
        return nodeId;

    Node* node = nodeToPush;
    while (Node* parent = innerParentNode(node))
        node = parent;

    // Node being pushed is detached -> push subtree root.
    OwnPtrWillBeRawPtr<NodeToIdMap> newMap = adoptPtrWillBeNoop(new NodeToIdMap);
    NodeToIdMap* danglingMap = newMap.get();
    m_danglingNodeToIdMaps.append(newMap.release());
    RefPtr<TypeBuilder::Array<TypeBuilder::DOM::Node> > children = TypeBuilder::Array<TypeBuilder::DOM::Node>::create();
    children->addItem(buildObjectForNode(node, 0, danglingMap));
    frontend()->setChildNodes(0, children);

    return pushNodePathToFrontend(nodeToPush, danglingMap);
}

int InspectorDOMAgent::boundNodeId(Node* node)
{
    return m_documentNodeToIdMap->get(node);
}

void InspectorDOMAgent::setAttributeValue(ErrorString* errorString, int elementId, const String& name, const String& value)
{
    Element* element = assertEditableElement(errorString, elementId);
    if (!element)
        return;

    m_domEditor->setAttribute(element, name, value, errorString);
}

void InspectorDOMAgent::setAttributesAsText(ErrorString* errorString, int elementId, const String& text, const String* const name)
{
    Element* element = assertEditableElement(errorString, elementId);
    if (!element)
        return;

    String markup = "<span " + text + "></span>";
    RefPtrWillBeRawPtr<DocumentFragment> fragment = element->document().createDocumentFragment();

    bool shouldIgnoreCase = element->document().isHTMLDocument() && element->isHTMLElement();
    // Not all elements can represent the context (i.e. IFRAME), hence using document.body.
    if (shouldIgnoreCase && element->document().body())
        fragment->parseHTML(markup, element->document().body(), AllowScriptingContent);
    else
        fragment->parseXML(markup, 0, AllowScriptingContent);

    Element* parsedElement = fragment->firstChild() && fragment->firstChild()->isElementNode() ? toElement(fragment->firstChild()) : nullptr;
    if (!parsedElement) {
        *errorString = "Could not parse value as attributes";
        return;
    }

    String caseAdjustedName = name ? (shouldIgnoreCase ? name->lower() : *name) : String();

    AttributeCollection attributes = parsedElement->attributes();
    if (attributes.isEmpty() && name) {
        m_domEditor->removeAttribute(element, caseAdjustedName, errorString);
        return;
    }

    bool foundOriginalAttribute = false;
    for (auto& attribute : attributes) {
        // Add attribute pair
        String attributeName = attribute.name().toString();
        if (shouldIgnoreCase)
            attributeName = attributeName.lower();
        foundOriginalAttribute |= name && attributeName == caseAdjustedName;
        if (!m_domEditor->setAttribute(element, attributeName, attribute.value(), errorString))
            return;
    }

    if (!foundOriginalAttribute && name && !name->stripWhiteSpace().isEmpty())
        m_domEditor->removeAttribute(element, caseAdjustedName, errorString);
}

void InspectorDOMAgent::removeAttribute(ErrorString* errorString, int elementId, const String& name)
{
    Element* element = assertEditableElement(errorString, elementId);
    if (!element)
        return;

    m_domEditor->removeAttribute(element, name, errorString);
}

void InspectorDOMAgent::removeNode(ErrorString* errorString, int nodeId)
{
    Node* node = assertEditableNode(errorString, nodeId);
    if (!node)
        return;

    ContainerNode* parentNode = node->parentNode();
    if (!parentNode) {
        *errorString = "Cannot remove detached node";
        return;
    }

    m_domEditor->removeChild(parentNode, node, errorString);
}

void InspectorDOMAgent::setNodeName(ErrorString* errorString, int nodeId, const String& tagName, int* newId)
{
    *newId = 0;

    Node* oldNode = nodeForId(nodeId);
    if (!oldNode || !oldNode->isElementNode())
        return;

    TrackExceptionState exceptionState;
    RefPtrWillBeRawPtr<Element> newElem = oldNode->document().createElement(AtomicString(tagName), exceptionState);
    if (exceptionState.hadException())
        return;

    // Copy over the original node's attributes.
    newElem->cloneAttributesFromElement(*toElement(oldNode));

    // Copy over the original node's children.
    for (Node* child = oldNode->firstChild(); child; child = oldNode->firstChild()) {
        if (!m_domEditor->insertBefore(newElem.get(), child, 0, errorString))
            return;
    }

    // Replace the old node with the new node
    ContainerNode* parent = oldNode->parentNode();
    if (!m_domEditor->insertBefore(parent, newElem.get(), oldNode->nextSibling(), errorString))
        return;
    if (!m_domEditor->removeChild(parent, oldNode, errorString))
        return;

    *newId = pushNodePathToFrontend(newElem.get());
    if (m_childrenRequested.contains(nodeId))
        pushChildNodesToFrontend(*newId);
}

void InspectorDOMAgent::getOuterHTML(ErrorString* errorString, int nodeId, WTF::String* outerHTML)
{
    Node* node = assertNode(errorString, nodeId);
    if (!node)
        return;

    *outerHTML = createMarkup(node);
}

void InspectorDOMAgent::setOuterHTML(ErrorString* errorString, int nodeId, const String& outerHTML)
{
    if (!nodeId) {
        ASSERT(m_document);
        DOMPatchSupport domPatchSupport(m_domEditor.get(), *m_document.get());
        domPatchSupport.patchDocument(outerHTML);
        return;
    }

    Node* node = assertEditableNode(errorString, nodeId);
    if (!node)
        return;

    Document* document = node->isDocumentNode() ? toDocument(node) : node->ownerDocument();
    if (!document || (!document->isHTMLDocument() && !document->isXMLDocument())) {
        *errorString = "Not an HTML/XML document";
        return;
    }

    Node* newNode = nullptr;
    if (!m_domEditor->setOuterHTML(node, outerHTML, &newNode, errorString))
        return;

    if (!newNode) {
        // The only child node has been deleted.
        return;
    }

    int newId = pushNodePathToFrontend(newNode);

    bool childrenRequested = m_childrenRequested.contains(nodeId);
    if (childrenRequested)
        pushChildNodesToFrontend(newId);
}

void InspectorDOMAgent::setNodeValue(ErrorString* errorString, int nodeId, const String& value)
{
    Node* node = assertEditableNode(errorString, nodeId);
    if (!node)
        return;

    if (node->nodeType() != Node::TEXT_NODE) {
        *errorString = "Can only set value of text nodes";
        return;
    }

    m_domEditor->replaceWholeText(toText(node), value, errorString);
}

static Node* nextNodeWithShadowDOMInMind(const Node& current, const Node* stayWithin, bool includeUserAgentShadowDOM)
{
    // At first traverse the subtree.
    if (current.isElementNode()) {
        const Element& element = toElement(current);
        ElementShadow* elementShadow = element.shadow();
        if (elementShadow) {
            ShadowRoot* shadowRoot = elementShadow->youngestShadowRoot();
            if (shadowRoot) {
                if (shadowRoot->type() == ShadowRootType::Open || includeUserAgentShadowDOM)
                    return shadowRoot;
            }
        }
    }
    if (current.hasChildren())
        return current.firstChild();

    // Then traverse siblings of the node itself and its ancestors.
    const Node* node = &current;
    do {
        if (node == stayWithin)
            return nullptr;
        if (node->isShadowRoot()) {
            const ShadowRoot* shadowRoot = toShadowRoot(node);
            if (shadowRoot->olderShadowRoot())
                return shadowRoot->olderShadowRoot();
            Node* host = shadowRoot->host();
            if (host && host->hasChildren())
                return host->firstChild();
        }
        if (node->nextSibling())
            return node->nextSibling();
        node = node->isShadowRoot() ? toShadowRoot(node)->host() : node->parentNode();
    } while (node);

    return nullptr;
}

void InspectorDOMAgent::performSearch(ErrorString*, const String& whitespaceTrimmedQuery, const bool* optionalIncludeUserAgentShadowDOM, String* searchId, int* resultCount)
{
    // FIXME: Few things are missing here:
    // 1) Search works with node granularity - number of matches within node is not calculated.
    // 2) There is no need to push all search results to the front-end at a time, pushing next / previous result
    //    is sufficient.

    bool includeUserAgentShadowDOM = optionalIncludeUserAgentShadowDOM ? *optionalIncludeUserAgentShadowDOM : false;

    unsigned queryLength = whitespaceTrimmedQuery.length();
    bool startTagFound = !whitespaceTrimmedQuery.find('<');
    bool endTagFound = whitespaceTrimmedQuery.reverseFind('>') + 1 == queryLength;
    bool startQuoteFound = !whitespaceTrimmedQuery.find('"');
    bool endQuoteFound = whitespaceTrimmedQuery.reverseFind('"') + 1 == queryLength;
    bool exactAttributeMatch = startQuoteFound && endQuoteFound;

    String tagNameQuery = whitespaceTrimmedQuery;
    String attributeQuery = whitespaceTrimmedQuery;
    if (startTagFound)
        tagNameQuery = tagNameQuery.right(tagNameQuery.length() - 1);
    if (endTagFound)
        tagNameQuery = tagNameQuery.left(tagNameQuery.length() - 1);
    if (startQuoteFound)
        attributeQuery = attributeQuery.right(attributeQuery.length() - 1);
    if (endQuoteFound)
        attributeQuery = attributeQuery.left(attributeQuery.length() - 1);

    WillBeHeapVector<RawPtrWillBeMember<Document> > docs = documents();
    WillBeHeapListHashSet<RawPtrWillBeMember<Node> > resultCollector;

    for (Document* document : docs) {
        Node* documentElement = document->documentElement();
        Node* node = documentElement;
        if (!node)
            continue;

        // Manual plain text search.
        for (; node; node = nextNodeWithShadowDOMInMind(*node, documentElement, includeUserAgentShadowDOM)) {
            switch (node->nodeType()) {
            case Node::TEXT_NODE:
            case Node::COMMENT_NODE:
            case Node::CDATA_SECTION_NODE: {
                String text = node->nodeValue();
                if (text.findIgnoringCase(whitespaceTrimmedQuery) != kNotFound)
                    resultCollector.add(node);
                break;
            }
            case Node::ELEMENT_NODE: {
                if ((!startTagFound && !endTagFound && (node->nodeName().findIgnoringCase(tagNameQuery) != kNotFound))
                    || (startTagFound && endTagFound && equalIgnoringCase(node->nodeName(), tagNameQuery))
                    || (startTagFound && !endTagFound && node->nodeName().startsWith(tagNameQuery, TextCaseInsensitive))
                    || (!startTagFound && endTagFound && node->nodeName().endsWith(tagNameQuery, TextCaseInsensitive))) {
                    resultCollector.add(node);
                    break;
                }
                // Go through all attributes and serialize them.
                const Element* element = toElement(node);
                AttributeCollection attributes = element->attributes();
                for (auto& attribute : attributes) {
                    // Add attribute pair
                    if (attribute.localName().find(whitespaceTrimmedQuery, 0, TextCaseInsensitive) != kNotFound) {
                        resultCollector.add(node);
                        break;
                    }
                    size_t foundPosition = attribute.value().find(attributeQuery, 0, TextCaseInsensitive);
                    if (foundPosition != kNotFound) {
                        if (!exactAttributeMatch || (!foundPosition && attribute.value().length() == attributeQuery.length())) {
                            resultCollector.add(node);
                            break;
                        }
                    }
                }
                break;
            }
            default:
                break;
            }
        }

        // XPath evaluation
#ifdef MINIBLINK_NOT_IMPLEMENTED
        for (Document* document : docs) {
            ASSERT(document);
            TrackExceptionState exceptionState;
            XPathResult* result = DocumentXPathEvaluator::evaluate(*document, whitespaceTrimmedQuery, document, nullptr, XPathResult::ORDERED_NODE_SNAPSHOT_TYPE, ScriptValue(), exceptionState);
            if (exceptionState.hadException() || !result)
                continue;

            unsigned long size = result->snapshotLength(exceptionState);
            for (unsigned long i = 0; !exceptionState.hadException() && i < size; ++i) {
                Node* node = result->snapshotItem(i, exceptionState);
                if (exceptionState.hadException())
                    break;

                if (node->nodeType() == Node::ATTRIBUTE_NODE)
                    node = toAttr(node)->ownerElement();
                resultCollector.add(node);
            }
        }
#endif

        // Selector evaluation
        for (Document* document : docs) {
            TrackExceptionState exceptionState;
            RefPtrWillBeRawPtr<StaticElementList> elementList = document->querySelectorAll(AtomicString(whitespaceTrimmedQuery), exceptionState);
            if (exceptionState.hadException() || !elementList)
                continue;

            unsigned size = elementList->length();
            for (unsigned i = 0; i < size; ++i)
                resultCollector.add(elementList->item(i));
        }
    }

    *searchId = IdentifiersFactory::createIdentifier();
    WillBeHeapVector<RefPtrWillBeMember<Node> >* resultsIt = &m_searchResults.add(*searchId, WillBeHeapVector<RefPtrWillBeMember<Node> >()).storedValue->value;

    for (auto& result : resultCollector)
        resultsIt->append(result);

    *resultCount = resultsIt->size();
}

void InspectorDOMAgent::getSearchResults(ErrorString* errorString, const String& searchId, int fromIndex, int toIndex, RefPtr<TypeBuilder::Array<int> >& nodeIds)
{
    SearchResults::iterator it = m_searchResults.find(searchId);
    if (it == m_searchResults.end()) {
        *errorString = "No search session with given id found";
        return;
    }

    int size = it->value.size();
    if (fromIndex < 0 || toIndex > size || fromIndex >= toIndex) {
        *errorString = "Invalid search result range";
        return;
    }

    nodeIds = TypeBuilder::Array<int>::create();
    for (int i = fromIndex; i < toIndex; ++i)
        nodeIds->addItem(pushNodePathToFrontend((it->value)[i].get()));
}

void InspectorDOMAgent::discardSearchResults(ErrorString*, const String& searchId)
{
    m_searchResults.remove(searchId);
}

bool InspectorDOMAgent::handleMousePress()
{
    if (m_searchingForNode == NotSearching)
        return false;

    if (m_hoveredNodeForInspectMode) {
        inspect(m_hoveredNodeForInspectMode.get());
        m_hoveredNodeForInspectMode.clear();
        return true;
    }
    return false;
}

bool InspectorDOMAgent::handleGestureEvent(LocalFrame* frame, const PlatformGestureEvent& event)
{
    if (m_searchingForNode == NotSearching || event.type() != PlatformEvent::GestureTap)
        return false;
    Node* node = hoveredNodeForEvent(frame, event, false);
    if (node && m_inspectModeHighlightConfig) {
        m_overlay->highlightNode(node, 0 /* eventTarget */, *m_inspectModeHighlightConfig, false);
        inspect(node);
        return true;
    }
    return false;
}

bool InspectorDOMAgent::handleTouchEvent(LocalFrame* frame, const PlatformTouchEvent& event)
{
    if (m_searchingForNode == NotSearching)
        return false;
    Node* node = hoveredNodeForEvent(frame, event, false);
    if (node && m_inspectModeHighlightConfig) {
        m_overlay->highlightNode(node, 0 /* eventTarget */, *m_inspectModeHighlightConfig, false);
        inspect(node);
        return true;
    }
    return false;
}

void InspectorDOMAgent::inspect(Node* inspectedNode)
{
    if (!inspectedNode)
        return;

    Node* node = inspectedNode;
    while (node && !node->isElementNode() && !node->isDocumentNode() && !node->isDocumentFragment())
        node = node->parentOrShadowHostNode();
    if (!node)
        return;

    int backendNodeId = DOMNodeIds::idForNode(node);
    if (!frontend() || !enabled()) {
        m_backendNodeIdToInspect = backendNodeId;
        return;
    }

    frontend()->inspectNodeRequested(backendNodeId);
}

bool InspectorDOMAgent::handleMouseMove(LocalFrame* frame, const PlatformMouseEvent& event)
{
    if (m_searchingForNode == NotSearching)
        return false;

    if (!frame->view() || !frame->contentLayoutObject())
        return true;
    Node* node = hoveredNodeForEvent(frame, event, event.shiftKey());

    // Do not highlight within user agent shadow root unless requested.
    if (m_searchingForNode != SearchingForUAShadow) {
        ShadowRoot* shadowRoot = userAgentShadowRoot(node);
        if (shadowRoot)
            node = shadowRoot->host();
    }

    // Shadow roots don't have boxes - use host element instead.
    if (node && node->isShadowRoot())
        node = node->parentOrShadowHostNode();

    if (!node)
        return true;

    Node* eventTarget = event.shiftKey() ? hoveredNodeForEvent(frame, event, false) : nullptr;
    if (eventTarget == node)
        eventTarget = 0;

    if (node && m_inspectModeHighlightConfig) {
        m_hoveredNodeForInspectMode = node;
        m_overlay->highlightNode(node, eventTarget, *m_inspectModeHighlightConfig, event.ctrlKey() || event.metaKey());
    }
    return true;
}

void InspectorDOMAgent::setSearchingForNode(ErrorString* errorString, SearchMode searchMode, JSONObject* highlightInspectorObject)
{
    if (m_searchingForNode == searchMode)
        return;

    m_searchingForNode = searchMode;
    m_overlay->setInspectModeEnabled(searchMode != NotSearching);
    if (searchMode != NotSearching) {
        m_inspectModeHighlightConfig = highlightConfigFromInspectorObject(errorString, highlightInspectorObject);
        if (!m_inspectModeHighlightConfig)
            return;
    } else {
        m_hoveredNodeForInspectMode.clear();
        hideHighlight(errorString);
    }
}

PassOwnPtr<InspectorHighlightConfig> InspectorDOMAgent::highlightConfigFromInspectorObject(ErrorString* errorString, JSONObject* highlightInspectorObject)
{
    if (!highlightInspectorObject) {
        *errorString = "Internal error: highlight configuration parameter is missing";
        return nullptr;
    }

    OwnPtr<InspectorHighlightConfig> highlightConfig = adoptPtr(new InspectorHighlightConfig());
    bool showInfo = false; // Default: false (do not show a tooltip).
    highlightInspectorObject->getBoolean("showInfo", &showInfo);
    highlightConfig->showInfo = showInfo;
    bool showRulers = false; // Default: false (do not show rulers).
    highlightInspectorObject->getBoolean("showRulers", &showRulers);
    highlightConfig->showRulers = showRulers;
    bool showExtensionLines = false; // Default: false (do not show extension lines).
    highlightInspectorObject->getBoolean("showExtensionLines", &showExtensionLines);
    highlightConfig->showExtensionLines = showExtensionLines;
    bool showLayoutEditor = false;
    highlightInspectorObject->getBoolean("showLayoutEditor", &showLayoutEditor);
    highlightConfig->showLayoutEditor = showLayoutEditor;
    highlightConfig->content = parseConfigColor("contentColor", highlightInspectorObject);
    highlightConfig->contentOutline = parseConfigColor("contentOutlineColor", highlightInspectorObject);
    highlightConfig->padding = parseConfigColor("paddingColor", highlightInspectorObject);
    highlightConfig->border = parseConfigColor("borderColor", highlightInspectorObject);
    highlightConfig->margin = parseConfigColor("marginColor", highlightInspectorObject);
    highlightConfig->eventTarget = parseConfigColor("eventTargetColor", highlightInspectorObject);
    highlightConfig->shape = parseConfigColor("shapeColor", highlightInspectorObject);
    highlightConfig->shapeMargin = parseConfigColor("shapeMarginColor", highlightInspectorObject);

    return highlightConfig.release();
}

void InspectorDOMAgent::setInspectModeEnabled(ErrorString* errorString, bool enabled, const bool* inspectUAShadowDOM, const RefPtr<JSONObject>* highlightConfig)
{
    if (enabled && !pushDocumentUponHandlelessOperation(errorString))
        return;
    SearchMode searchMode = enabled ? (asBool(inspectUAShadowDOM) ? SearchingForUAShadow : SearchingForNormal) : NotSearching;
    setSearchingForNode(errorString, searchMode, highlightConfig ? highlightConfig->get() : nullptr);
}

void InspectorDOMAgent::highlightRect(ErrorString*, int x, int y, int width, int height, const RefPtr<JSONObject>* color, const RefPtr<JSONObject>* outlineColor)
{
    OwnPtr<FloatQuad> quad = adoptPtr(new FloatQuad(FloatRect(x, y, width, height)));
    innerHighlightQuad(quad.release(), color, outlineColor);
}

void InspectorDOMAgent::highlightQuad(ErrorString* errorString, const RefPtr<JSONArray>& quadArray, const RefPtr<JSONObject>* color, const RefPtr<JSONObject>* outlineColor)
{
    OwnPtr<FloatQuad> quad = adoptPtr(new FloatQuad());
    if (!parseQuad(quadArray, quad.get())) {
        *errorString = "Invalid Quad format";
        return;
    }
    innerHighlightQuad(quad.release(), color, outlineColor);
}

void InspectorDOMAgent::innerHighlightQuad(PassOwnPtr<FloatQuad> quad, const RefPtr<JSONObject>* color, const RefPtr<JSONObject>* outlineColor)
{
    OwnPtr<InspectorHighlightConfig> highlightConfig = adoptPtr(new InspectorHighlightConfig());
    highlightConfig->content = parseColor(color);
    highlightConfig->contentOutline = parseColor(outlineColor);
    m_overlay->highlightQuad(quad, *highlightConfig);
}

void InspectorDOMAgent::highlightNode(ErrorString* errorString, const RefPtr<JSONObject>& highlightInspectorObject, const int* nodeId, const int* backendNodeId, const String* objectId)
{
    Node* node = nullptr;
    if (nodeId) {
        node = assertNode(errorString, *nodeId);
    } else if (backendNodeId) {
        node = DOMNodeIds::nodeForId(*backendNodeId);
    } else if (objectId) {
        InjectedScript injectedScript = m_injectedScriptManager->injectedScriptForObjectId(*objectId);
        node = injectedScript.nodeForObjectId(*objectId);
        if (!node)
            *errorString = "Node for given objectId not found";
    } else
        *errorString = "Either nodeId or objectId must be specified";

    if (!node)
        return;

    OwnPtr<InspectorHighlightConfig> highlightConfig = highlightConfigFromInspectorObject(errorString, highlightInspectorObject.get());
    if (!highlightConfig)
        return;

    m_overlay->highlightNode(node, 0 /* eventTarget */, *highlightConfig, false);
}

void InspectorDOMAgent::highlightFrame(
    ErrorString*,
    const String& frameId,
    const RefPtr<JSONObject>* color,
    const RefPtr<JSONObject>* outlineColor)
{
    LocalFrame* frame = m_pageAgent->frameForId(frameId);
    // FIXME: Inspector doesn't currently work cross process.
    if (frame && frame->deprecatedLocalOwner()) {
        OwnPtr<InspectorHighlightConfig> highlightConfig = adoptPtr(new InspectorHighlightConfig());
        highlightConfig->showInfo = true; // Always show tooltips for frames.
        highlightConfig->content = parseColor(color);
        highlightConfig->contentOutline = parseColor(outlineColor);
        m_overlay->highlightNode(frame->deprecatedLocalOwner(), 0 /* eventTarget */, *highlightConfig, false);
    }
}

void InspectorDOMAgent::hideHighlight(ErrorString*)
{
    m_overlay->hideHighlight();
}

void InspectorDOMAgent::copyTo(ErrorString* errorString, int nodeId, int targetElementId, const int* const anchorNodeId, int* newNodeId)
{
    Node* node = assertEditableNode(errorString, nodeId);
    if (!node)
        return;

    Element* targetElement = assertEditableElement(errorString, targetElementId);
    if (!targetElement)
        return;

    Node* anchorNode = nullptr;
    if (anchorNodeId && *anchorNodeId) {
        anchorNode = assertEditableChildNode(errorString, targetElement, *anchorNodeId);
        if (!anchorNode)
            return;
    }

    // The clone is deep by default.
    RefPtrWillBeRawPtr<Node> clonedNode = node->cloneNode(true);
    if (!clonedNode) {
        *errorString = "Failed to clone node";
        return;
    }
    if (!m_domEditor->insertBefore(targetElement, clonedNode, anchorNode, errorString))
        return;

    *newNodeId = pushNodePathToFrontend(clonedNode.get());
}

void InspectorDOMAgent::moveTo(ErrorString* errorString, int nodeId, int targetElementId, const int* const anchorNodeId, int* newNodeId)
{
    Node* node = assertEditableNode(errorString, nodeId);
    if (!node)
        return;

    Element* targetElement = assertEditableElement(errorString, targetElementId);
    if (!targetElement)
        return;

    Node* current = targetElement;
    while (current) {
        if (current == node) {
            *errorString = "Unable to move node into self or descendant";
            return;
        }
        current = current->parentNode();
    }

    Node* anchorNode = nullptr;
    if (anchorNodeId && *anchorNodeId) {
        anchorNode = assertEditableChildNode(errorString, targetElement, *anchorNodeId);
        if (!anchorNode)
            return;
    }

    if (!m_domEditor->insertBefore(targetElement, node, anchorNode, errorString))
        return;

    *newNodeId = pushNodePathToFrontend(node);
}

void InspectorDOMAgent::undo(ErrorString* errorString)
{
    TrackExceptionState exceptionState;
    m_history->undo(exceptionState);
    *errorString = InspectorDOMAgent::toErrorString(exceptionState);
}

void InspectorDOMAgent::redo(ErrorString* errorString)
{
    TrackExceptionState exceptionState;
    m_history->redo(exceptionState);
    *errorString = InspectorDOMAgent::toErrorString(exceptionState);
}

void InspectorDOMAgent::markUndoableState(ErrorString*)
{
    m_history->markUndoableState();
}

void InspectorDOMAgent::focus(ErrorString* errorString, int nodeId)
{
    Element* element = assertElement(errorString, nodeId);
    if (!element)
        return;

    element->document().updateLayoutIgnorePendingStylesheets();
    if (!element->isFocusable()) {
        *errorString = "Element is not focusable";
        return;
    }
    element->focus();
}

void InspectorDOMAgent::setFileInputFiles(ErrorString* errorString, int nodeId, const RefPtr<JSONArray>& files)
{
    Node* node = assertNode(errorString, nodeId);
    if (!node)
        return;
    if (!isHTMLInputElement(*node) || toHTMLInputElement(*node).type() != InputTypeNames::file) {
        *errorString = "Node is not a file input element";
        return;
    }

    FileList* fileList = FileList::create();
    for (const auto& file : *files) {
        String path;
        if (!(file)->asString(&path)) {
            *errorString = "Files must be strings";
            return;
        }
        fileList->append(File::create(path));
    }
    toHTMLInputElement(node)->setFiles(fileList);
}

void InspectorDOMAgent::getBoxModel(ErrorString* errorString, int nodeId, RefPtr<TypeBuilder::DOM::BoxModel>& model)
{
    Node* node = assertNode(errorString, nodeId);
    if (!node)
        return;

    bool result = InspectorHighlight::getBoxModel(node, model);
    if (!result)
        *errorString = "Could not compute box model.";
}

void InspectorDOMAgent::getNodeForLocation(ErrorString* errorString, int x, int y, int* nodeId)
{
    if (!pushDocumentUponHandlelessOperation(errorString))
        return;
    HitTestRequest request(HitTestRequest::Move | HitTestRequest::ReadOnly | HitTestRequest::AllowChildFrameContent);
    HitTestResult result(request, IntPoint(x, y));
    m_document->frame()->contentLayoutObject()->hitTest(result);
    Node* node = result.innerPossiblyPseudoNode();
    while (node && node->nodeType() == Node::TEXT_NODE)
        node = node->parentNode();
    if (!node) {
        *errorString = "No node found at given location";
        return;
    }
    *nodeId = pushNodePathToFrontend(node);
}

void InspectorDOMAgent::resolveNode(ErrorString* errorString, int nodeId, const String* const objectGroup, RefPtr<TypeBuilder::Runtime::RemoteObject>& result)
{
    String objectGroupName = objectGroup ? *objectGroup : "";
    Node* node = nodeForId(nodeId);
    if (!node) {
        *errorString = "No node with given id found";
        return;
    }
    RefPtr<TypeBuilder::Runtime::RemoteObject> object = resolveNode(node, objectGroupName);
    if (!object) {
        *errorString = "Node with given id does not belong to the document";
        return;
    }
    result = object;
}

void InspectorDOMAgent::getAttributes(ErrorString* errorString, int nodeId, RefPtr<TypeBuilder::Array<String> >& result)
{
    Element* element = assertElement(errorString, nodeId);
    if (!element)
        return;

    result = buildArrayForElementAttributes(element);
}

void InspectorDOMAgent::requestNode(ErrorString*, const String& objectId, int* nodeId)
{
    InjectedScript injectedScript = m_injectedScriptManager->injectedScriptForObjectId(objectId);
    Node* node = injectedScript.nodeForObjectId(objectId);
    if (node)
        *nodeId = pushNodePathToFrontend(node);
    else
        *nodeId = 0;
}

// static
String InspectorDOMAgent::documentURLString(Document* document)
{
    if (!document || document->url().isNull())
        return "";
    return document->url().string();
}

static String documentBaseURLString(Document* document)
{
    return document->completeURL("").string();
}

static TypeBuilder::DOM::ShadowRootType::Enum shadowRootType(ShadowRoot* shadowRoot)
{
    switch (shadowRoot->type()) {
    case ShadowRootType::UserAgent:
        return TypeBuilder::DOM::ShadowRootType::User_agent;
    case ShadowRootType::Open:
        return TypeBuilder::DOM::ShadowRootType::Author;
    }
    ASSERT_NOT_REACHED();
    return TypeBuilder::DOM::ShadowRootType::User_agent;
}

PassRefPtr<TypeBuilder::DOM::Node> InspectorDOMAgent::buildObjectForNode(Node* node, int depth, NodeToIdMap* nodesMap)
{
    int id = bind(node, nodesMap);
    String localName;
    String nodeValue;

    switch (node->nodeType()) {
    case Node::TEXT_NODE:
    case Node::COMMENT_NODE:
    case Node::CDATA_SECTION_NODE:
        nodeValue = node->nodeValue();
        if (nodeValue.length() > maxTextSize)
            nodeValue = nodeValue.left(maxTextSize) + ellipsisUChar;
        break;
    case Node::ATTRIBUTE_NODE:
    case Node::DOCUMENT_FRAGMENT_NODE:
    case Node::DOCUMENT_NODE:
    case Node::ELEMENT_NODE:
    default:
        localName = node->localName();
        break;
    }

    RefPtr<TypeBuilder::DOM::Node> value = TypeBuilder::DOM::Node::create()
        .setNodeId(id)
        .setNodeType(static_cast<int>(node->nodeType()))
        .setNodeName(node->nodeName())
        .setLocalName(localName)
        .setNodeValue(nodeValue);

    bool forcePushChildren = false;
    if (node->isElementNode()) {
        Element* element = toElement(node);
        value->setAttributes(buildArrayForElementAttributes(element));

        if (node->isFrameOwnerElement()) {
            HTMLFrameOwnerElement* frameOwner = toHTMLFrameOwnerElement(node);
            if (LocalFrame* frame = frameOwner->contentFrame() && frameOwner->contentFrame()->isLocalFrame() ? toLocalFrame(frameOwner->contentFrame()) : nullptr)
                value->setFrameId(IdentifiersFactory::frameId(frame));
            if (Document* doc = frameOwner->contentDocument())
                value->setContentDocument(buildObjectForNode(doc, 0, nodesMap));
        }

        ElementShadow* shadow = element->shadow();
        if (shadow) {
            RefPtr<TypeBuilder::Array<TypeBuilder::DOM::Node> > shadowRoots = TypeBuilder::Array<TypeBuilder::DOM::Node>::create();
            for (ShadowRoot* root = shadow->youngestShadowRoot(); root; root = root->olderShadowRoot())
                shadowRoots->addItem(buildObjectForNode(root, 0, nodesMap));
            value->setShadowRoots(shadowRoots);
            forcePushChildren = true;
        }

        if (isHTMLLinkElement(*element)) {
            HTMLLinkElement& linkElement = toHTMLLinkElement(*element);
            if (linkElement.isImport() && linkElement.import() && innerParentNode(linkElement.import()) == linkElement)
                value->setImportedDocument(buildObjectForNode(linkElement.import(), 0, nodesMap));
            forcePushChildren = true;
        }

        if (isHTMLTemplateElement(*element)) {
            value->setTemplateContent(buildObjectForNode(toHTMLTemplateElement(*element).content(), 0, nodesMap));
            forcePushChildren = true;
        }

        if (element->pseudoId()) {
            TypeBuilder::DOM::PseudoType::Enum pseudoType;
            if (InspectorDOMAgent::getPseudoElementType(element->pseudoId(), &pseudoType))
                value->setPseudoType(pseudoType);
        } else {
            RefPtr<TypeBuilder::Array<TypeBuilder::DOM::Node> > pseudoElements = buildArrayForPseudoElements(element, nodesMap);
            if (pseudoElements) {
                value->setPseudoElements(pseudoElements.release());
                forcePushChildren = true;
            }
        }

        if (element->isInsertionPoint()) {
            value->setDistributedNodes(buildArrayForDistributedNodes(toInsertionPoint(element)));
            forcePushChildren = true;
        }

    } else if (node->isDocumentNode()) {
        Document* document = toDocument(node);
        value->setDocumentURL(documentURLString(document));
        value->setBaseURL(documentBaseURLString(document));
        value->setXmlVersion(document->xmlVersion());
    } else if (node->isDocumentTypeNode()) {
        DocumentType* docType = toDocumentType(node);
        value->setPublicId(docType->publicId());
        value->setSystemId(docType->systemId());
    } else if (node->isAttributeNode()) {
        Attr* attribute = toAttr(node);
        value->setName(attribute->name());
        value->setValue(attribute->value());
    } else if (node->isShadowRoot()) {
        value->setShadowRootType(shadowRootType(toShadowRoot(node)));
    }

    if (node->isContainerNode()) {
        int nodeCount = innerChildNodeCount(node);
        value->setChildNodeCount(nodeCount);
        if (nodesMap == m_documentNodeToIdMap)
            m_cachedChildCount.set(id, nodeCount);
        if (forcePushChildren && !depth)
            depth = 1;
        RefPtr<TypeBuilder::Array<TypeBuilder::DOM::Node> > children = buildArrayForContainerChildren(node, depth, nodesMap);
        if (children->length() > 0 || depth) // Push children along with shadow in any case.
            value->setChildren(children.release());
    }

    return value.release();
}

PassRefPtr<TypeBuilder::Array<String> > InspectorDOMAgent::buildArrayForElementAttributes(Element* element)
{
    RefPtr<TypeBuilder::Array<String> > attributesValue = TypeBuilder::Array<String>::create();
    // Go through all attributes and serialize them.
    AttributeCollection attributes = element->attributes();
    for (auto& attribute : attributes) {
        // Add attribute pair
        attributesValue->addItem(attribute.name().toString());
        attributesValue->addItem(attribute.value());
    }
    return attributesValue.release();
}

PassRefPtr<TypeBuilder::Array<TypeBuilder::DOM::Node> > InspectorDOMAgent::buildArrayForContainerChildren(Node* container, int depth, NodeToIdMap* nodesMap)
{
    RefPtr<TypeBuilder::Array<TypeBuilder::DOM::Node> > children = TypeBuilder::Array<TypeBuilder::DOM::Node>::create();
    if (depth == 0) {
        // Special-case the only text child - pretend that container's children have been requested.
        Node* firstChild = container->firstChild();
        if (firstChild && firstChild->nodeType() == Node::TEXT_NODE && !firstChild->nextSibling()) {
            children->addItem(buildObjectForNode(firstChild, 0, nodesMap));
            m_childrenRequested.add(bind(container, nodesMap));
        }
        return children.release();
    }

    Node* child = innerFirstChild(container);
    depth--;
    m_childrenRequested.add(bind(container, nodesMap));

    while (child) {
        children->addItem(buildObjectForNode(child, depth, nodesMap));
        child = innerNextSibling(child);
    }
    return children.release();
}

PassRefPtr<TypeBuilder::Array<TypeBuilder::DOM::Node> > InspectorDOMAgent::buildArrayForPseudoElements(Element* element, NodeToIdMap* nodesMap)
{
    if (!element->pseudoElement(BEFORE) && !element->pseudoElement(AFTER))
        return nullptr;

    RefPtr<TypeBuilder::Array<TypeBuilder::DOM::Node> > pseudoElements = TypeBuilder::Array<TypeBuilder::DOM::Node>::create();
    if (element->pseudoElement(BEFORE))
        pseudoElements->addItem(buildObjectForNode(element->pseudoElement(BEFORE), 0, nodesMap));
    if (element->pseudoElement(AFTER))
        pseudoElements->addItem(buildObjectForNode(element->pseudoElement(AFTER), 0, nodesMap));
    return pseudoElements.release();
}

PassRefPtr<TypeBuilder::Array<TypeBuilder::DOM::BackendNode>> InspectorDOMAgent::buildArrayForDistributedNodes(InsertionPoint* insertionPoint)
{
    RefPtr<TypeBuilder::Array<TypeBuilder::DOM::BackendNode>> distributedNodes = TypeBuilder::Array<TypeBuilder::DOM::BackendNode>::create();
    for (size_t i = 0; i < insertionPoint->distributedNodesSize(); ++i) {
        Node* distributedNode = insertionPoint->distributedNodeAt(i);
        if (isWhitespace(distributedNode))
            continue;

        RefPtr<TypeBuilder::DOM::BackendNode> backendNode = TypeBuilder::DOM::BackendNode::create()
            .setNodeType(distributedNode->nodeType())
            .setNodeName(distributedNode->nodeName())
            .setBackendNodeId(DOMNodeIds::idForNode(distributedNode));
        distributedNodes->addItem(backendNode.release());
    }
    return distributedNodes.release();
}

Node* InspectorDOMAgent::innerFirstChild(Node* node)
{
    node = node->firstChild();
    while (isWhitespace(node))
        node = node->nextSibling();
    return node;
}

Node* InspectorDOMAgent::innerNextSibling(Node* node)
{
    do {
        node = node->nextSibling();
    } while (isWhitespace(node));
    return node;
}

Node* InspectorDOMAgent::innerPreviousSibling(Node* node)
{
    do {
        node = node->previousSibling();
    } while (isWhitespace(node));
    return node;
}

unsigned InspectorDOMAgent::innerChildNodeCount(Node* node)
{
    unsigned count = 0;
    Node* child = innerFirstChild(node);
    while (child) {
        count++;
        child = innerNextSibling(child);
    }
    return count;
}

Node* InspectorDOMAgent::innerParentNode(Node* node)
{
    if (node->isDocumentNode()) {
        Document* document = toDocument(node);
        if (HTMLImportLoader* loader = document->importLoader())
            return loader->firstImport()->link();
        return document->ownerElement();
    }
    return node->parentOrShadowHostNode();
}

bool InspectorDOMAgent::isWhitespace(Node* node)
{
    //TODO: pull ignoreWhitespace setting from the frontend and use here.
    return node && node->nodeType() == Node::TEXT_NODE && node->nodeValue().stripWhiteSpace().length() == 0;
}

void InspectorDOMAgent::domContentLoadedEventFired(LocalFrame* frame)
{
    if (frame != m_pageAgent->inspectedFrame())
        return;

    // Re-push document once it is loaded.
    discardFrontendBindings();
    if (enabled())
        frontend()->documentUpdated();
}

void InspectorDOMAgent::invalidateFrameOwnerElement(LocalFrame* frame)
{
    HTMLFrameOwnerElement* frameOwner = frame->document()->ownerElement();
    if (!frameOwner)
        return;

    int frameOwnerId = m_documentNodeToIdMap->get(frameOwner);
    if (!frameOwnerId)
        return;

    // Re-add frame owner element together with its new children.
    int parentId = m_documentNodeToIdMap->get(innerParentNode(frameOwner));
    frontend()->childNodeRemoved(parentId, frameOwnerId);
    unbind(frameOwner, m_documentNodeToIdMap.get());

    RefPtr<TypeBuilder::DOM::Node> value = buildObjectForNode(frameOwner, 0, m_documentNodeToIdMap.get());
    Node* previousSibling = innerPreviousSibling(frameOwner);
    int prevId = previousSibling ? m_documentNodeToIdMap->get(previousSibling) : 0;
    frontend()->childNodeInserted(parentId, prevId, value.release());
}

void InspectorDOMAgent::didCommitLoad(LocalFrame*, DocumentLoader* loader)
{
    LocalFrame* inspectedFrame = m_pageAgent->inspectedFrame();
    if (loader->frame() != inspectedFrame) {
        invalidateFrameOwnerElement(loader->frame());
        return;
    }

    setDocument(inspectedFrame->document());
}

void InspectorDOMAgent::didInsertDOMNode(Node* node)
{
    if (isWhitespace(node))
        return;

    // We could be attaching existing subtree. Forget the bindings.
    unbind(node, m_documentNodeToIdMap.get());

    ContainerNode* parent = node->parentNode();
    if (!parent)
        return;
    int parentId = m_documentNodeToIdMap->get(parent);
    // Return if parent is not mapped yet.
    if (!parentId)
        return;

    if (!m_childrenRequested.contains(parentId)) {
        // No children are mapped yet -> only notify on changes of child count.
        int count = m_cachedChildCount.get(parentId) + 1;
        m_cachedChildCount.set(parentId, count);
        frontend()->childNodeCountUpdated(parentId, count);
    } else {
        // Children have been requested -> return value of a new child.
        Node* prevSibling = innerPreviousSibling(node);
        int prevId = prevSibling ? m_documentNodeToIdMap->get(prevSibling) : 0;
        RefPtr<TypeBuilder::DOM::Node> value = buildObjectForNode(node, 0, m_documentNodeToIdMap.get());
        frontend()->childNodeInserted(parentId, prevId, value.release());
    }
}

void InspectorDOMAgent::willRemoveDOMNode(Node* node)
{
    if (isWhitespace(node))
        return;

    ContainerNode* parent = node->parentNode();

    // If parent is not mapped yet -> ignore the event.
    if (!m_documentNodeToIdMap->contains(parent))
        return;

    int parentId = m_documentNodeToIdMap->get(parent);

    if (!m_childrenRequested.contains(parentId)) {
        // No children are mapped yet -> only notify on changes of child count.
        int count = m_cachedChildCount.get(parentId) - 1;
        m_cachedChildCount.set(parentId, count);
        frontend()->childNodeCountUpdated(parentId, count);
    } else {
        frontend()->childNodeRemoved(parentId, m_documentNodeToIdMap->get(node));
    }
    unbind(node, m_documentNodeToIdMap.get());
}

void InspectorDOMAgent::willModifyDOMAttr(Element*, const AtomicString& oldValue, const AtomicString& newValue)
{
    m_suppressAttributeModifiedEvent = (oldValue == newValue);
}

void InspectorDOMAgent::didModifyDOMAttr(Element* element, const QualifiedName& name, const AtomicString& value)
{
    bool shouldSuppressEvent = m_suppressAttributeModifiedEvent;
    m_suppressAttributeModifiedEvent = false;
    if (shouldSuppressEvent)
        return;

    int id = boundNodeId(element);
    // If node is not mapped yet -> ignore the event.
    if (!id)
        return;

    if (m_domListener)
        m_domListener->didModifyDOMAttr(element);

    frontend()->attributeModified(id, name.toString(), value);
}

void InspectorDOMAgent::didRemoveDOMAttr(Element* element, const QualifiedName& name)
{
    int id = boundNodeId(element);
    // If node is not mapped yet -> ignore the event.
    if (!id)
        return;

    if (m_domListener)
        m_domListener->didModifyDOMAttr(element);

    frontend()->attributeRemoved(id, name.toString());
}

void InspectorDOMAgent::styleAttributeInvalidated(const WillBeHeapVector<RawPtrWillBeMember<Element> >& elements)
{
    RefPtr<TypeBuilder::Array<int> > nodeIds = TypeBuilder::Array<int>::create();
    for (unsigned i = 0, size = elements.size(); i < size; ++i) {
        Element* element = elements.at(i);
        int id = boundNodeId(element);
        // If node is not mapped yet -> ignore the event.
        if (!id)
            continue;

        if (m_domListener)
            m_domListener->didModifyDOMAttr(element);
        nodeIds->addItem(id);
    }
    frontend()->inlineStyleInvalidated(nodeIds.release());
}

void InspectorDOMAgent::characterDataModified(CharacterData* characterData)
{
    int id = m_documentNodeToIdMap->get(characterData);
    if (!id) {
        // Push text node if it is being created.
        didInsertDOMNode(characterData);
        return;
    }
    frontend()->characterDataModified(id, characterData->data());
}

RawPtrWillBeMember<InspectorRevalidateDOMTask> InspectorDOMAgent::revalidateTask()
{
    if (!m_revalidateTask)
        m_revalidateTask = adoptPtrWillBeNoop(new InspectorRevalidateDOMTask(this));
    return m_revalidateTask.get();
}

void InspectorDOMAgent::didInvalidateStyleAttr(Node* node)
{
    int id = m_documentNodeToIdMap->get(node);
    // If node is not mapped yet -> ignore the event.
    if (!id)
        return;

    revalidateTask()->scheduleStyleAttrRevalidationFor(toElement(node));
}

void InspectorDOMAgent::didPushShadowRoot(Element* host, ShadowRoot* root)
{
    if (!host->ownerDocument())
        return;

    int hostId = m_documentNodeToIdMap->get(host);
    if (!hostId)
        return;

    pushChildNodesToFrontend(hostId, 1);
    frontend()->shadowRootPushed(hostId, buildObjectForNode(root, 0, m_documentNodeToIdMap.get()));
}

void InspectorDOMAgent::willPopShadowRoot(Element* host, ShadowRoot* root)
{
    if (!host->ownerDocument())
        return;

    int hostId = m_documentNodeToIdMap->get(host);
    int rootId = m_documentNodeToIdMap->get(root);
    if (hostId && rootId)
        frontend()->shadowRootPopped(hostId, rootId);
}

void InspectorDOMAgent::didPerformElementShadowDistribution(Element* shadowHost)
{
    int shadowHostId = m_documentNodeToIdMap->get(shadowHost);
    if (!shadowHostId)
        return;

    for (ShadowRoot* root = shadowHost->youngestShadowRoot(); root; root = root->olderShadowRoot()) {
        const WillBeHeapVector<RefPtrWillBeMember<InsertionPoint>>& insertionPoints = root->descendantInsertionPoints();
        for (const auto& it : insertionPoints) {
            InsertionPoint* insertionPoint = it.get();
            int insertionPointId = m_documentNodeToIdMap->get(insertionPoint);
            if (insertionPointId)
                frontend()->distributedNodesUpdated(insertionPointId, buildArrayForDistributedNodes(insertionPoint));
        }
    }
}

void InspectorDOMAgent::frameDocumentUpdated(LocalFrame* frame)
{
    Document* document = frame->document();
    if (!document)
        return;

    if (frame != m_pageAgent->inspectedFrame())
        return;

    // Only update the main frame document, nested frame document updates are not required
    // (will be handled by invalidateFrameOwnerElement()).
    setDocument(document);
}

void InspectorDOMAgent::pseudoElementCreated(PseudoElement* pseudoElement)
{
    Element* parent = pseudoElement->parentOrShadowHostElement();
    if (!parent)
        return;
    int parentId = m_documentNodeToIdMap->get(parent);
    if (!parentId)
        return;

    pushChildNodesToFrontend(parentId, 1);
    frontend()->pseudoElementAdded(parentId, buildObjectForNode(pseudoElement, 0, m_documentNodeToIdMap.get()));
}

void InspectorDOMAgent::pseudoElementDestroyed(PseudoElement* pseudoElement)
{
    int pseudoElementId = m_documentNodeToIdMap->get(pseudoElement);
    if (!pseudoElementId)
        return;

    // If a PseudoElement is bound, its parent element must be bound, too.
    Element* parent = pseudoElement->parentOrShadowHostElement();
    ASSERT(parent);
    int parentId = m_documentNodeToIdMap->get(parent);
    ASSERT(parentId);

    unbind(pseudoElement, m_documentNodeToIdMap.get());
    frontend()->pseudoElementRemoved(parentId, pseudoElementId);
}

static ShadowRoot* shadowRootForNode(Node* node, const String& type)
{
    if (!node->isElementNode())
        return nullptr;
    if (type == "a")
        return toElement(node)->shadowRoot();
    if (type == "u")
        return toElement(node)->userAgentShadowRoot();
    return nullptr;
}

Node* InspectorDOMAgent::nodeForPath(const String& path)
{
    // The path is of form "1,HTML,2,BODY,1,DIV" (<index> and <nodeName> interleaved).
    // <index> may also be "a" (author shadow root) or "u" (user-agent shadow root),
    // in which case <nodeName> MUST be "#document-fragment".
    if (!m_document)
        return nullptr;

    Node* node = m_document.get();
    Vector<String> pathTokens;
    path.split(',', pathTokens);
    if (!pathTokens.size())
        return nullptr;

    for (size_t i = 0; i < pathTokens.size() - 1; i += 2) {
        bool success = true;
        String& indexValue = pathTokens[i];
        unsigned childNumber = indexValue.toUInt(&success);
        Node* child;
        if (!success) {
            child = shadowRootForNode(node, indexValue);
        } else {
            if (childNumber >= innerChildNodeCount(node))
                return nullptr;

            child = innerFirstChild(node);
        }
        String childName = pathTokens[i + 1];
        for (size_t j = 0; child && j < childNumber; ++j)
            child = innerNextSibling(child);

        if (!child || child->nodeName() != childName)
            return nullptr;
        node = child;
    }
    return node;
}

void InspectorDOMAgent::pushNodeByPathToFrontend(ErrorString* errorString, const String& path, int* nodeId)
{
    if (Node* node = nodeForPath(path))
        *nodeId = pushNodePathToFrontend(node);
    else
        *errorString = "No node with given path found";
}

void InspectorDOMAgent::pushNodesByBackendIdsToFrontend(ErrorString* errorString, const RefPtr<JSONArray>& backendNodeIds, RefPtr<TypeBuilder::Array<int> >& result)
{
    result = TypeBuilder::Array<int>::create();
    for (const auto& backendNode : *backendNodeIds) {
        int backendNodeId;

        if (!(backendNode)->asNumber(&backendNodeId)) {
            *errorString = "Invalid argument type";
            return;
        }

        Node* node = DOMNodeIds::nodeForId(backendNodeId);
        if (node && node->document().frame() && node->document().frame()->instrumentingAgents() == m_pageAgent->inspectedFrame()->instrumentingAgents())
            result->addItem(pushNodePathToFrontend(node));
        else
            result->addItem(0);
    }
}

class InspectableNode final : public InjectedScriptHost::InspectableObject {
public:
    explicit InspectableNode(Node* node) : m_node(node) { }
    virtual ScriptValue get(ScriptState* state) override
    {
        return InjectedScriptHost::nodeAsScriptValue(state, m_node);
    }
private:
    Node* m_node;
};

void InspectorDOMAgent::setInspectedNode(ErrorString* errorString, int nodeId)
{
    Node* node = assertNode(errorString, nodeId);
    if (!node)
        return;
    m_injectedScriptManager->injectedScriptHost()->addInspectedObject(adoptPtr(new InspectableNode(node)));
}

void InspectorDOMAgent::getRelayoutBoundary(ErrorString* errorString, int nodeId, int* relayoutBoundaryNodeId)
{
    Node* node = assertNode(errorString, nodeId);
    if (!node)
        return;
    LayoutObject* layoutObject = node->layoutObject();
    if (!layoutObject) {
        *errorString = "No layout object for node, perhaps orphan or hidden node";
        return;
    }
    while (layoutObject && !layoutObject->isDocumentElement() && !layoutObject->isRelayoutBoundaryForInspector())
        layoutObject = layoutObject->container();
    Node* resultNode = layoutObject ? layoutObject->generatingNode() : node->ownerDocument();
    *relayoutBoundaryNodeId = pushNodePathToFrontend(resultNode);
}

void InspectorDOMAgent::getHighlightObjectForTest(ErrorString* errorString, int nodeId, RefPtr<JSONObject>& result)
{
    Node* node = assertNode(errorString, nodeId);
    if (!node)
        return;
    InspectorHighlight highlight(node, InspectorHighlight::defaultConfig(), true);
    result = highlight.asJSONObject();
}

PassRefPtr<TypeBuilder::Runtime::RemoteObject> InspectorDOMAgent::resolveNode(Node* node, const String& objectGroup)
{
    Document* document = node->isDocumentNode() ? &node->document() : node->ownerDocument();
    LocalFrame* frame = document ? document->frame() : nullptr;
    if (!frame)
        return nullptr;

    InjectedScript injectedScript = m_injectedScriptManager->injectedScriptFor(ScriptState::forMainWorld(frame));
    if (injectedScript.isEmpty())
        return nullptr;

    return injectedScript.wrapNode(node, objectGroup);
}

bool InspectorDOMAgent::pushDocumentUponHandlelessOperation(ErrorString* errorString)
{
    if (!m_documentNodeToIdMap->contains(m_document)) {
        RefPtr<TypeBuilder::DOM::Node> root;
        getDocument(errorString, root);
        return errorString->isEmpty();
    }
    return true;
}

DEFINE_TRACE(InspectorDOMAgent)
{
    visitor->trace(m_domListener);
    visitor->trace(m_pageAgent);
    visitor->trace(m_injectedScriptManager);
    visitor->trace(m_overlay);
#if ENABLE(OILPAN)
    visitor->trace(m_documentNodeToIdMap);
    visitor->trace(m_danglingNodeToIdMaps);
    visitor->trace(m_idToNode);
    visitor->trace(m_idToNodesMap);
    visitor->trace(m_document);
    visitor->trace(m_revalidateTask);
    visitor->trace(m_searchResults);
#endif
    visitor->trace(m_hoveredNodeForInspectMode);
    visitor->trace(m_history);
    visitor->trace(m_domEditor);
    visitor->trace(m_listener);
    InspectorBaseAgent::trace(visitor);
}

} // namespace blink

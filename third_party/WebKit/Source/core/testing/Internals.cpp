/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 * Copyright (C) 2013 Apple Inc. All rights reserved.
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
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/testing/Internals.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptFunction.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/SerializedScriptValue.h"
#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "bindings/core/v8/V8IteratorResultValue.h"
#include "bindings/core/v8/V8ThrowException.h"
#include "core/HTMLNames.h"
#include "core/SVGNames.h"
#include "core/animation/AnimationTimeline.h"
#include "core/css/StyleSheetContents.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/css/resolver/StyleResolverStats.h"
#include "core/css/resolver/ViewportStyleResolver.h"
#include "core/dom/ClientRect.h"
#include "core/dom/ClientRectList.h"
#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/DOMPoint.h"
#include "core/dom/DOMStringList.h"
#include "core/dom/Document.h"
#include "core/dom/DocumentMarker.h"
#include "core/dom/DocumentMarkerController.h"
#include "core/dom/Element.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/Iterator.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/dom/PseudoElement.h"
#include "core/dom/Range.h"
#include "core/dom/StaticNodeList.h"
#include "core/dom/StyleEngine.h"
#include "core/dom/TreeScope.h"
#include "core/dom/ViewportDescription.h"
#include "core/dom/shadow/ComposedTreeTraversal.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/dom/shadow/SelectRuleFeatureSet.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/Editor.h"
#include "core/editing/PlainTextRange.h"
#include "core/editing/SpellCheckRequester.h"
#include "core/editing/SpellChecker.h"
#include "core/editing/SurroundingText.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/editing/markup.h"
#include "core/fetch/MemoryCache.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/frame/EventHandlerRegistry.h"
#include "core/frame/FrameConsole.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLContentElement.h"
#include "core/html/HTMLIFrameElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/HTMLPlugInElement.h"
#include "core/html/HTMLSelectElement.h"
#include "core/html/HTMLTextAreaElement.h"
#include "core/html/canvas/CanvasFontCache.h"
#include "core/html/canvas/CanvasRenderingContext.h"
#include "core/html/forms/FormController.h"
#include "core/html/shadow/PluginPlaceholderElement.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/html/shadow/TextControlInnerElements.h"
#include "core/input/EventHandler.h"
#include "core/inspector/ConsoleMessageStorage.h"
#include "core/inspector/InspectorConsoleAgent.h"
#include "core/inspector/InspectorFrontendChannel.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorOverlay.h"
#include "core/inspector/InstanceCounters.h"
#include "core/inspector/InstrumentingAgents.h"
#include "core/layout/LayoutMenuList.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutTreeAsText.h"
#include "core/layout/LayoutView.h"
#include "core/layout/compositing/CompositedDeprecatedPaintLayerMapping.h"
#include "core/layout/compositing/DeprecatedPaintLayerCompositor.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/HistoryItem.h"
#include "core/page/ChromeClient.h"
#include "core/page/FocusController.h"
#include "core/page/NetworkStateNotifier.h"
#include "core/page/Page.h"
#include "core/page/PrintContext.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/plugins/testing/DictionaryPluginPlaceholder.h"
#include "core/plugins/testing/DocumentFragmentPluginPlaceholder.h"
#include "core/svg/SVGImageElement.h"
#include "core/testing/DictionaryTest.h"
#include "core/testing/GCObservation.h"
#include "core/testing/InternalRuntimeFlags.h"
#include "core/testing/InternalSettings.h"
#include "core/testing/LayerRect.h"
#include "core/testing/LayerRectList.h"
#include "core/testing/PluginPlaceholderOptions.h"
#include "core/testing/PrivateScriptTest.h"
#include "core/testing/TypeConversions.h"
#include "core/testing/UnionTypesTest.h"
#include "core/workers/WorkerThread.h"
#include "platform/Cursor.h"
#include "platform/Language.h"
#include "platform/PlatformKeyboardEvent.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "platform/geometry/IntRect.h"
#include "platform/geometry/LayoutRect.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/graphics/filters/FilterOperation.h"
#include "platform/graphics/filters/FilterOperations.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/SchemeRegistry.h"
#include "public/platform/Platform.h"
#include "public/platform/WebConnectionType.h"
#include "public/platform/WebGraphicsContext3D.h"
#include "public/platform/WebGraphicsContext3DProvider.h"
#include "public/platform/WebLayer.h"
#include "wtf/InstanceCounter.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/dtoa.h"
#include "wtf/text/StringBuffer.h"
#include <v8.h>

namespace blink {

namespace {

class InternalsIterationSource final : public ValueIterable<int>::IterationSource {
public:
    bool next(ScriptState* scriptState, int& value, ExceptionState& exceptionState) override
    {
        if (m_index >= 5)
            return false;
        value = m_index * m_index;
        return true;
    }
};

} // namespace

static bool markerTypesFrom(const String& markerType, DocumentMarker::MarkerTypes& result)
{
    if (markerType.isEmpty() || equalIgnoringCase(markerType, "all"))
        result = DocumentMarker::AllMarkers();
    else if (equalIgnoringCase(markerType, "Spelling"))
        result =  DocumentMarker::Spelling;
    else if (equalIgnoringCase(markerType, "Grammar"))
        result =  DocumentMarker::Grammar;
    else if (equalIgnoringCase(markerType, "TextMatch"))
        result =  DocumentMarker::TextMatch;
    else
        return false;

    return true;
}

static SpellCheckRequester* spellCheckRequester(Document* document)
{
    if (!document || !document->frame())
        return 0;
    return &document->frame()->spellChecker().spellCheckRequester();
}

const char* Internals::internalsId = "internals";

Internals* Internals::create(ScriptState* scriptState)
{
    return new Internals(scriptState);
}

Internals::~Internals()
{
}

void Internals::resetToConsistentState(Page* page)
{
    ASSERT(page);

    page->setDeviceScaleFactor(1);
    page->setIsCursorVisible(true);
    page->setPageScaleFactor(1, IntPoint(0, 0));
    overrideUserPreferredLanguages(Vector<AtomicString>());
    if (!page->deprecatedLocalMainFrame()->spellChecker().isContinuousSpellCheckingEnabled())
        page->deprecatedLocalMainFrame()->spellChecker().toggleContinuousSpellChecking();
    if (page->deprecatedLocalMainFrame()->editor().isOverwriteModeEnabled())
        page->deprecatedLocalMainFrame()->editor().toggleOverwriteModeEnabled();

    if (ScrollingCoordinator* scrollingCoordinator = page->scrollingCoordinator())
        scrollingCoordinator->reset();

    page->deprecatedLocalMainFrame()->view()->clear();
    PlatformKeyboardEvent::setCurrentCapsLockState(PlatformKeyboardEvent::OverrideCapsLockState::Default);
}

Internals::Internals(ScriptState* scriptState)
    : ContextLifecycleObserver(scriptState->executionContext())
    , m_runtimeFlags(InternalRuntimeFlags::create())
{
}

Document* Internals::contextDocument() const
{
    return toDocument(executionContext());
}

LocalFrame* Internals::frame() const
{
    if (!contextDocument())
        return 0;
    return contextDocument()->frame();
}

InternalSettings* Internals::settings() const
{
    Document* document = contextDocument();
    if (!document)
        return 0;
    Page* page = document->page();
    if (!page)
        return 0;
    return InternalSettings::from(*page);
}

InternalRuntimeFlags* Internals::runtimeFlags() const
{
    return m_runtimeFlags.get();
}

unsigned Internals::workerThreadCount() const
{
    return WorkerThread::workerThreadCount();
}

String Internals::address(Node* node)
{
    char buf[32];
    sprintf(buf, "%p", node);

    return String(buf);
}

GCObservation* Internals::observeGC(ScriptValue scriptValue)
{
    v8::Local<v8::Value> observedValue = scriptValue.v8Value();
    ASSERT(!observedValue.IsEmpty());
    if (observedValue->IsNull() || observedValue->IsUndefined()) {
        V8ThrowException::throwTypeError(v8::Isolate::GetCurrent(), "value to observe is null or undefined");
        return nullptr;
    }

    return GCObservation::create(observedValue);
}

unsigned Internals::updateStyleAndReturnAffectedElementCount(ExceptionState& exceptionState) const
{
    Document* document = contextDocument();
    if (!document) {
        exceptionState.throwDOMException(InvalidAccessError, "No context document is available.");
        return 0;
    }

    unsigned beforeCount = document->styleEngine().resolverAccessCount();
    document->updateLayoutTreeIfNeeded();
    return document->styleEngine().resolverAccessCount() - beforeCount;
}

unsigned Internals::needsLayoutCount(ExceptionState& exceptionState) const
{
    LocalFrame* contextFrame = frame();
    if (!contextFrame) {
        exceptionState.throwDOMException(InvalidAccessError, "No context frame is available.");
        return 0;
    }

    bool isPartial;
    unsigned needsLayoutObjects;
    unsigned totalObjects;
    contextFrame->view()->countObjectsNeedingLayout(needsLayoutObjects, totalObjects, isPartial);
    return needsLayoutObjects;
}

unsigned Internals::hitTestCount(Document* doc, ExceptionState& exceptionState) const
{
    if (!doc) {
        exceptionState.throwDOMException(InvalidAccessError, "Must supply document to check");
        return 0;
    }

    return doc->layoutView()->hitTestCount();
}

unsigned Internals::hitTestCacheHits(Document* doc, ExceptionState& exceptionState) const
{
    if (!doc) {
        exceptionState.throwDOMException(InvalidAccessError, "Must supply document to check");
        return 0;
    }

    return doc->layoutView()->hitTestCacheHits();
}

Element* Internals::elementFromPoint(Document* doc, double x, double y, bool ignoreClipping, bool allowChildFrameContent, ExceptionState& exceptionState) const
{
    if (!doc) {
        exceptionState.throwDOMException(InvalidAccessError, "Must supply document to check");
        return 0;
    }

    if (!doc->layoutView())
        return 0;

    HitTestRequest::HitTestRequestType hitType = HitTestRequest::ReadOnly | HitTestRequest::Active;
    if (ignoreClipping)
        hitType |= HitTestRequest::IgnoreClipping;
    if (allowChildFrameContent)
        hitType |= HitTestRequest::AllowChildFrameContent;

    HitTestRequest request(hitType);

    return doc->hitTestPoint(x, y, request);
}

void Internals::clearHitTestCache(Document* doc, ExceptionState& exceptionState) const
{
    if (!doc) {
        exceptionState.throwDOMException(InvalidAccessError, "Must supply document to check");
        return;
    }

    if (!doc->layoutView())
        return;

    doc->layoutView()->clearHitTestCache();
}

bool Internals::isPreloaded(const String& url)
{
    return isPreloadedBy(url, contextDocument());
}

bool Internals::isPreloadedBy(const String& url, Document* document)
{
    if (!document)
        return false;
    return document->fetcher()->isPreloaded(document->completeURL(url));
}

bool Internals::isLoadingFromMemoryCache(const String& url)
{
    if (!contextDocument())
        return false;
    const String cacheIdentifier = contextDocument()->fetcher()->getCacheIdentifier();
    Resource* resource = memoryCache()->resourceForURL(contextDocument()->completeURL(url), cacheIdentifier);
    return resource && resource->status() == Resource::Cached;
}

bool Internals::isSharingStyle(Element* element1, Element* element2) const
{
    ASSERT(element1 && element2);
    return element1->computedStyle() == element2->computedStyle();
}

bool Internals::isValidContentSelect(Element* insertionPoint, ExceptionState& exceptionState)
{
    ASSERT(insertionPoint);
    if (!insertionPoint->isInsertionPoint()) {
        exceptionState.throwDOMException(InvalidAccessError, "The element is not an insertion point.");
        return false;
    }

    return isHTMLContentElement(*insertionPoint) && toHTMLContentElement(*insertionPoint).isSelectValid();
}

Node* Internals::treeScopeRootNode(Node* node)
{
    ASSERT(node);
    return &node->treeScope().rootNode();
}

Node* Internals::parentTreeScope(Node* node)
{
    ASSERT(node);
    const TreeScope* parentTreeScope = node->treeScope().parentTreeScope();
    return parentTreeScope ? &parentTreeScope->rootNode() : 0;
}

bool Internals::hasSelectorForIdInShadow(Element* host, const AtomicString& idValue, ExceptionState& exceptionState)
{
    ASSERT(host);
    if (!host->shadow()) {
        exceptionState.throwDOMException(InvalidAccessError, "The host element does not have a shadow.");
        return false;
    }

    return host->shadow()->ensureSelectFeatureSet().hasSelectorForId(idValue);
}

bool Internals::hasSelectorForClassInShadow(Element* host, const AtomicString& className, ExceptionState& exceptionState)
{
    ASSERT(host);
    if (!host->shadow()) {
        exceptionState.throwDOMException(InvalidAccessError, "The host element does not have a shadow.");
        return false;
    }

    return host->shadow()->ensureSelectFeatureSet().hasSelectorForClass(className);
}

bool Internals::hasSelectorForAttributeInShadow(Element* host, const AtomicString& attributeName, ExceptionState& exceptionState)
{
    ASSERT(host);
    if (!host->shadow()) {
        exceptionState.throwDOMException(InvalidAccessError, "The host element does not have a shadow.");
        return false;
    }

    return host->shadow()->ensureSelectFeatureSet().hasSelectorForAttribute(attributeName);
}

unsigned short Internals::compareTreeScopePosition(const Node* node1, const Node* node2, ExceptionState& exceptionState) const
{
    ASSERT(node1 && node2);
    const TreeScope* treeScope1 = node1->isDocumentNode() ? static_cast<const TreeScope*>(toDocument(node1)) :
        node1->isShadowRoot() ? static_cast<const TreeScope*>(toShadowRoot(node1)) : 0;
    const TreeScope* treeScope2 = node2->isDocumentNode() ? static_cast<const TreeScope*>(toDocument(node2)) :
        node2->isShadowRoot() ? static_cast<const TreeScope*>(toShadowRoot(node2)) : 0;
    if (!treeScope1 || !treeScope2) {
        exceptionState.throwDOMException(InvalidAccessError, String::format("The %s node is neither a document node, nor a shadow root.", treeScope1 ? "second" : "first"));
        return 0;
    }
    return treeScope1->comparePosition(*treeScope2);
}

void Internals::pauseAnimations(double pauseTime, ExceptionState& exceptionState)
{
    if (pauseTime < 0) {
        exceptionState.throwDOMException(InvalidAccessError, ExceptionMessages::indexExceedsMinimumBound("pauseTime", pauseTime, 0.0));
        return;
    }

    frame()->view()->updateAllLifecyclePhases();
    frame()->document()->timeline().pauseAnimationsForTesting(pauseTime);
}

bool Internals::isCompositedAnimation(Animation* animation)
{
    return animation->hasActiveAnimationsOnCompositor();
}

void Internals::advanceTimeForImage(Element* image, double deltaTimeInSeconds, ExceptionState& exceptionState)
{
    ASSERT(image);
    if (deltaTimeInSeconds < 0) {
        exceptionState.throwDOMException(InvalidAccessError, ExceptionMessages::indexExceedsMinimumBound("deltaTimeInSeconds", deltaTimeInSeconds, 0.0));
        return;
    }

    ImageResource* resource = nullptr;
    if (isHTMLImageElement(*image)) {
        resource = toHTMLImageElement(*image).cachedImage();
    } else if (isSVGImageElement(*image)) {
        resource = toSVGImageElement(*image).cachedImage();
    } else {
        exceptionState.throwDOMException(InvalidAccessError, "The element provided is not a image element.");
        return;
    }

    if (!resource || !resource->hasImage()) {
        exceptionState.throwDOMException(InvalidAccessError, "The image resource is not available.");
        return;
    }

    Image* imageData = resource->image();
    if (!imageData->isBitmapImage()) {
        exceptionState.throwDOMException(InvalidAccessError, "The image resource is not a BitmapImage type.");
        return;
    }

    imageData->advanceTime(deltaTimeInSeconds);
}

bool Internals::hasShadowInsertionPoint(const Node* root, ExceptionState& exceptionState) const
{
    ASSERT(root);
    if (!root->isShadowRoot()) {
        exceptionState.throwDOMException(InvalidAccessError, "The node argument is not a shadow root.");
        return false;
    }
    return toShadowRoot(root)->containsShadowElements();
}

bool Internals::hasContentElement(const Node* root, ExceptionState& exceptionState) const
{
    ASSERT(root);
    if (!root->isShadowRoot()) {
        exceptionState.throwDOMException(InvalidAccessError, "The node argument is not a shadow root.");
        return false;
    }
    return toShadowRoot(root)->containsContentElements();
}

size_t Internals::countElementShadow(const Node* root, ExceptionState& exceptionState) const
{
    ASSERT(root);
    if (!root->isShadowRoot()) {
        exceptionState.throwDOMException(InvalidAccessError, "The node argument is not a shadow root.");
        return 0;
    }
    return toShadowRoot(root)->childShadowRootCount();
}

Node* Internals::nextSiblingInComposedTree(Node* node, ExceptionState& exceptionState)
{
    ASSERT(node);
    if (!node->canParticipateInComposedTree()) {
        exceptionState.throwDOMException(InvalidAccessError, "The node argument doesn't particite in the composed tree.");
        return 0;
    }
    return ComposedTreeTraversal::nextSibling(*node);
}

Node* Internals::firstChildInComposedTree(Node* node, ExceptionState& exceptionState)
{
    ASSERT(node);
    if (!node->canParticipateInComposedTree()) {
        exceptionState.throwDOMException(InvalidAccessError, "The node argument doesn't particite in the composed tree");
        return 0;
    }
    return ComposedTreeTraversal::firstChild(*node);
}

Node* Internals::lastChildInComposedTree(Node* node, ExceptionState& exceptionState)
{
    ASSERT(node);
    if (!node->canParticipateInComposedTree()) {
        exceptionState.throwDOMException(InvalidAccessError, "The node argument doesn't particite in the composed tree.");
        return 0;
    }
    return ComposedTreeTraversal::lastChild(*node);
}

Node* Internals::nextInComposedTree(Node* node, ExceptionState& exceptionState)
{
    ASSERT(node);
    if (!node->canParticipateInComposedTree()) {
        exceptionState.throwDOMException(InvalidAccessError, "The node argument doesn't particite in the composed tree.");
        return 0;
    }
    return ComposedTreeTraversal::next(*node);
}

Node* Internals::previousInComposedTree(Node* node, ExceptionState& exceptionState)
{
    ASSERT(node);
    if (!node->canParticipateInComposedTree()) {
        exceptionState.throwDOMException(InvalidAccessError, "The node argument doesn't particite in the composed tree.");
        return 0;
    }
    return ComposedTreeTraversal::previous(*node);
}

String Internals::elementLayoutTreeAsText(Element* element, ExceptionState& exceptionState)
{
    ASSERT(element);
    String representation = externalRepresentation(element);
    if (representation.isEmpty()) {
        exceptionState.throwDOMException(InvalidAccessError, "The element provided has no external representation.");
        return String();
    }

    return representation;
}

PassRefPtrWillBeRawPtr<CSSStyleDeclaration> Internals::computedStyleIncludingVisitedInfo(Node* node) const
{
    ASSERT(node);
    bool allowVisitedStyle = true;
    return CSSComputedStyleDeclaration::create(node, allowVisitedStyle);
}

PassRefPtrWillBeRawPtr<ShadowRoot> Internals::createUserAgentShadowRoot(Element* host)
{
    ASSERT(host);
    return PassRefPtrWillBeRawPtr<ShadowRoot>(host->ensureUserAgentShadowRoot());
}

ShadowRoot* Internals::shadowRoot(Element* host)
{
    // FIXME: Internals::shadowRoot() in tests should be converted to youngestShadowRoot() or oldestShadowRoot().
    // https://bugs.webkit.org/show_bug.cgi?id=78465
    return youngestShadowRoot(host);
}

ShadowRoot* Internals::youngestShadowRoot(Element* host)
{
    ASSERT(host);
    if (ElementShadow* shadow = host->shadow())
        return shadow->youngestShadowRoot();
    return 0;
}

ShadowRoot* Internals::oldestShadowRoot(Element* host)
{
    ASSERT(host);
    if (ElementShadow* shadow = host->shadow())
        return shadow->oldestShadowRoot();
    return 0;
}

ShadowRoot* Internals::youngerShadowRoot(Node* shadow, ExceptionState& exceptionState)
{
    ASSERT(shadow);
    if (!shadow->isShadowRoot()) {
        exceptionState.throwDOMException(InvalidAccessError, "The node provided is not a shadow root.");
        return 0;
    }

    return toShadowRoot(shadow)->youngerShadowRoot();
}

String Internals::shadowRootType(const Node* root, ExceptionState& exceptionState) const
{
    ASSERT(root);
    if (!root->isShadowRoot()) {
        exceptionState.throwDOMException(InvalidAccessError, "The node provided is not a shadow root.");
        return String();
    }

    switch (toShadowRoot(root)->type()) {
    case ShadowRootType::UserAgent:
        return String("UserAgentShadowRoot");
    case ShadowRootType::Open:
        return String("OpenShadowRoot");
    default:
        ASSERT_NOT_REACHED();
        return String("Unknown");
    }
}

const AtomicString& Internals::shadowPseudoId(Element* element)
{
    ASSERT(element);
    return element->shadowPseudoId();
}

String Internals::visiblePlaceholder(Element* element)
{
    if (element && isHTMLTextFormControlElement(*element)) {
        if (toHTMLTextFormControlElement(element)->placeholderShouldBeVisible())
            return toHTMLTextFormControlElement(element)->placeholderElement()->textContent();
    }

    return String();
}

void Internals::selectColorInColorChooser(Element* element, const String& colorValue)
{
    ASSERT(element);
    if (!isHTMLInputElement(*element))
        return;
    Color color;
    if (!color.setFromString(colorValue))
        return;
    toHTMLInputElement(*element).selectColorInColorChooser(color);
}

void Internals::endColorChooser(Element* element)
{
    ASSERT(element);
    if (!isHTMLInputElement(*element))
        return;
    toHTMLInputElement(*element).endColorChooser();
}

bool Internals::hasAutofocusRequest(Document* document)
{
    if (!document)
        document = contextDocument();
    return document->autofocusElement();
}

bool Internals::hasAutofocusRequest()
{
    return hasAutofocusRequest(0);
}

Vector<String> Internals::formControlStateOfHistoryItem(ExceptionState& exceptionState)
{
    HistoryItem* mainItem = frame()->loader().currentItem();
    if (!mainItem) {
        exceptionState.throwDOMException(InvalidAccessError, "No history item is available.");
        return Vector<String>();
    }
    return mainItem->documentState();
}

void Internals::setFormControlStateOfHistoryItem(const Vector<String>& state, ExceptionState& exceptionState)
{
    HistoryItem* mainItem = frame()->loader().currentItem();
    if (!mainItem) {
        exceptionState.throwDOMException(InvalidAccessError, "No history item is available.");
        return;
    }
    mainItem->clearDocumentState();
    mainItem->setDocumentState(state);
}

DOMWindow* Internals::pagePopupWindow() const
{
    Document* document = contextDocument();
    if (!document)
        return nullptr;
    if (Page* page = document->page())
        return page->chromeClient().pagePopupWindowForTesting();
    return nullptr;
}

ClientRect* Internals::absoluteCaretBounds(ExceptionState& exceptionState)
{
    Document* document = contextDocument();
    if (!document || !document->frame()) {
        exceptionState.throwDOMException(InvalidAccessError, document ? "The document's frame cannot be retrieved." : "No context document can be obtained.");
        return ClientRect::create();
    }

    return ClientRect::create(document->frame()->selection().absoluteCaretBounds());
}

ClientRect* Internals::boundingBox(Element* element)
{
    ASSERT(element);

    element->document().updateLayoutIgnorePendingStylesheets();
    LayoutObject* layoutObject = element->layoutObject();
    if (!layoutObject)
        return ClientRect::create();
    return ClientRect::create(layoutObject->absoluteBoundingBoxRectIgnoringTransforms());
}

unsigned Internals::markerCountForNode(Node* node, const String& markerType, ExceptionState& exceptionState)
{
    ASSERT(node);
    DocumentMarker::MarkerTypes markerTypes = 0;
    if (!markerTypesFrom(markerType, markerTypes)) {
        exceptionState.throwDOMException(SyntaxError, "The marker type provided ('" + markerType + "') is invalid.");
        return 0;
    }

    return node->document().markers().markersFor(node, markerTypes).size();
}

unsigned Internals::activeMarkerCountForNode(Node* node)
{
    ASSERT(node);

    // Only TextMatch markers can be active.
    DocumentMarker::MarkerType markerType = DocumentMarker::TextMatch;
    DocumentMarkerVector markers = node->document().markers().markersFor(node, markerType);

    unsigned activeMarkerCount = 0;
    for (const auto& marker : markers) {
        if (marker->activeMatch())
            activeMarkerCount++;
    }

    return activeMarkerCount;
}

DocumentMarker* Internals::markerAt(Node* node, const String& markerType, unsigned index, ExceptionState& exceptionState)
{
    ASSERT(node);
    DocumentMarker::MarkerTypes markerTypes = 0;
    if (!markerTypesFrom(markerType, markerTypes)) {
        exceptionState.throwDOMException(SyntaxError, "The marker type provided ('" + markerType + "') is invalid.");
        return 0;
    }

    DocumentMarkerVector markers = node->document().markers().markersFor(node, markerTypes);
    if (markers.size() <= index)
        return 0;
    return markers[index];
}

PassRefPtrWillBeRawPtr<Range> Internals::markerRangeForNode(Node* node, const String& markerType, unsigned index, ExceptionState& exceptionState)
{
    ASSERT(node);
    DocumentMarker* marker = markerAt(node, markerType, index, exceptionState);
    if (!marker)
        return nullptr;
    return Range::create(node->document(), node, marker->startOffset(), node, marker->endOffset());
}

String Internals::markerDescriptionForNode(Node* node, const String& markerType, unsigned index, ExceptionState& exceptionState)
{
    DocumentMarker* marker = markerAt(node, markerType, index, exceptionState);
    if (!marker)
        return String();
    return marker->description();
}

void Internals::addTextMatchMarker(const Range* range, bool isActive)
{
    ASSERT(range);
    range->ownerDocument().updateLayoutIgnorePendingStylesheets();
    range->ownerDocument().markers().addTextMatchMarker(range, isActive);
}

void Internals::setMarkersActive(Node* node, unsigned startOffset, unsigned endOffset, bool active)
{
    ASSERT(node);
    node->document().markers().setMarkersActive(node, startOffset, endOffset, active);
}

void Internals::setMarkedTextMatchesAreHighlighted(Document* document, bool highlight)
{
    if (!document || !document->frame())
        return;

    document->frame()->editor().setMarkedTextMatchesAreHighlighted(highlight);
}

void Internals::setFrameViewPosition(Document* document, long x, long y, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->view()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return;
    }

    FrameView* frameView = document->view();
    bool scrollbarsSuppressedOldValue = frameView->scrollbarsSuppressed();

    frameView->setScrollbarsSuppressed(false);
    frameView->setScrollOffsetFromInternals(IntPoint(x, y));
    frameView->setScrollbarsSuppressed(scrollbarsSuppressedOldValue);
}

String Internals::viewportAsText(Document* document, float, int availableWidth, int availableHeight, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->page()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return String();
    }

    document->updateLayoutIgnorePendingStylesheets();

    Page* page = document->page();

    // Update initial viewport size.
    IntSize initialViewportSize(availableWidth, availableHeight);
    document->page()->deprecatedLocalMainFrame()->view()->setFrameRect(IntRect(IntPoint::zero(), initialViewportSize));

    ViewportDescription description = page->viewportDescription();
    PageScaleConstraints constraints = description.resolve(initialViewportSize, Length());

    constraints.fitToContentsWidth(constraints.layoutSize.width(), availableWidth);
    constraints.resolveAutoInitialScale();

    StringBuilder builder;

    builder.appendLiteral("viewport size ");
    builder.append(String::number(constraints.layoutSize.width()));
    builder.append('x');
    builder.append(String::number(constraints.layoutSize.height()));

    builder.appendLiteral(" scale ");
    builder.append(String::number(constraints.initialScale));
    builder.appendLiteral(" with limits [");
    builder.append(String::number(constraints.minimumScale));
    builder.appendLiteral(", ");
    builder.append(String::number(constraints.maximumScale));

    builder.appendLiteral("] and userScalable ");
    builder.append(description.userZoom ? "true" : "false");

    return builder.toString();
}

bool Internals::wasLastChangeUserEdit(Element* textField, ExceptionState& exceptionState)
{
    ASSERT(textField);
    if (isHTMLInputElement(*textField))
        return toHTMLInputElement(*textField).lastChangeWasUserEdit();

    if (isHTMLTextAreaElement(*textField))
        return toHTMLTextAreaElement(*textField).lastChangeWasUserEdit();

    exceptionState.throwDOMException(InvalidNodeTypeError, "The element provided is not a TEXTAREA.");
    return false;
}

bool Internals::elementShouldAutoComplete(Element* element, ExceptionState& exceptionState)
{
    ASSERT(element);
    if (isHTMLInputElement(*element))
        return toHTMLInputElement(*element).shouldAutocomplete();

    exceptionState.throwDOMException(InvalidNodeTypeError, "The element provided is not an INPUT.");
    return false;
}

String Internals::suggestedValue(Element* element, ExceptionState& exceptionState)
{
    ASSERT(element);
    if (!element->isFormControlElement()) {
        exceptionState.throwDOMException(InvalidNodeTypeError, "The element provided is not a form control element.");
        return String();
    }

    String suggestedValue;
    if (isHTMLInputElement(*element))
        suggestedValue = toHTMLInputElement(*element).suggestedValue();

    if (isHTMLTextAreaElement(*element))
        suggestedValue = toHTMLTextAreaElement(*element).suggestedValue();

    if (isHTMLSelectElement(*element))
        suggestedValue = toHTMLSelectElement(*element).suggestedValue();

    return suggestedValue;
}

void Internals::setSuggestedValue(Element* element, const String& value, ExceptionState& exceptionState)
{
    ASSERT(element);
    if (!element->isFormControlElement()) {
        exceptionState.throwDOMException(InvalidNodeTypeError, "The element provided is not a form control element.");
        return;
    }

    if (isHTMLInputElement(*element))
        toHTMLInputElement(*element).setSuggestedValue(value);

    if (isHTMLTextAreaElement(*element))
        toHTMLTextAreaElement(*element).setSuggestedValue(value);

    if (isHTMLSelectElement(*element))
        toHTMLSelectElement(*element).setSuggestedValue(value);
}

void Internals::setEditingValue(Element* element, const String& value, ExceptionState& exceptionState)
{
    ASSERT(element);
    if (!isHTMLInputElement(*element)) {
        exceptionState.throwDOMException(InvalidNodeTypeError, "The element provided is not an INPUT.");
        return;
    }

    toHTMLInputElement(*element).setEditingValue(value);
}

void Internals::setAutofilled(Element* element, bool enabled, ExceptionState& exceptionState)
{
    ASSERT(element);
    if (!element->isFormControlElement()) {
        exceptionState.throwDOMException(InvalidNodeTypeError, "The element provided is not a form control element.");
        return;
    }
    toHTMLFormControlElement(element)->setAutofilled(enabled);
}

void Internals::scrollElementToRect(Element* element, long x, long y, long w, long h, ExceptionState& exceptionState)
{
    ASSERT(element);
    if (!element->document().view()) {
        exceptionState.throwDOMException(InvalidNodeTypeError, element ? "No view can be obtained from the provided element's document." : ExceptionMessages::argumentNullOrIncorrectType(1, "Element"));
        return;
    }

    FrameView* mainFrame = toLocalFrame(element->document().page()->mainFrame())->view();
    mainFrame->frame().document()->updateLayoutIgnorePendingStylesheets();

    FrameView* elementView = element->document().view();
    IntRect boundsInRootFrame = elementView->contentsToRootFrame(pixelSnappedIntRect(element->boundingBox()));
    IntRect boundsInRootContent = mainFrame->frameToContents(boundsInRootFrame);
    mainFrame->scrollableArea()->scrollIntoRect(LayoutRect(boundsInRootContent), FloatRect(x, y, w, h));
}

PassRefPtrWillBeRawPtr<Range> Internals::rangeFromLocationAndLength(Element* scope, int rangeLocation, int rangeLength)
{
    ASSERT(scope);

    // TextIterator depends on Layout information, make sure layout it up to date.
    scope->document().updateLayoutIgnorePendingStylesheets();

    return PlainTextRange(rangeLocation, rangeLocation + rangeLength).createRange(*scope);
}

unsigned Internals::locationFromRange(Element* scope, const Range* range)
{
    ASSERT(scope && range);
    // PlainTextRange depends on Layout information, make sure layout it up to date.
    scope->document().updateLayoutIgnorePendingStylesheets();

    return PlainTextRange::create(*scope, *range).start();
}

unsigned Internals::lengthFromRange(Element* scope, const Range* range)
{
    ASSERT(scope && range);
    // PlainTextRange depends on Layout information, make sure layout it up to date.
    scope->document().updateLayoutIgnorePendingStylesheets();

    return PlainTextRange::create(*scope, *range).length();
}

String Internals::rangeAsText(const Range* range)
{
    ASSERT(range);
    return range->text();
}

// FIXME: The next four functions are very similar - combine them once
// bestClickableNode/bestContextMenuNode have been combined..

DOMPoint* Internals::touchPositionAdjustedToBestClickableNode(long x, long y, long width, long height, Document* document, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->frame()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return 0;
    }

    document->updateLayout();

    IntSize radius(width / 2, height / 2);
    IntPoint point(x + radius.width(), y + radius.height());

    EventHandler& eventHandler = document->frame()->eventHandler();
    IntPoint hitTestPoint = document->frame()->view()->rootFrameToContents(point);
    HitTestResult result = eventHandler.hitTestResultAtPoint(hitTestPoint, HitTestRequest::ReadOnly | HitTestRequest::Active | HitTestRequest::ListBased, LayoutSize(radius));

    Node* targetNode = 0;
    IntPoint adjustedPoint;

    bool foundNode = eventHandler.bestClickableNodeForHitTestResult(result, adjustedPoint, targetNode);
    if (foundNode)
        return DOMPoint::create(adjustedPoint.x(), adjustedPoint.y());

    return 0;
}

Node* Internals::touchNodeAdjustedToBestClickableNode(long x, long y, long width, long height, Document* document, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->frame()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return 0;
    }

    document->updateLayout();

    IntSize radius(width / 2, height / 2);
    IntPoint point(x + radius.width(), y + radius.height());

    EventHandler& eventHandler = document->frame()->eventHandler();
    IntPoint hitTestPoint = document->frame()->view()->rootFrameToContents(point);
    HitTestResult result = eventHandler.hitTestResultAtPoint(hitTestPoint, HitTestRequest::ReadOnly | HitTestRequest::Active | HitTestRequest::ListBased, LayoutSize(radius));

    Node* targetNode = 0;
    IntPoint adjustedPoint;
    document->frame()->eventHandler().bestClickableNodeForHitTestResult(result, adjustedPoint, targetNode);
    return targetNode;
}

DOMPoint* Internals::touchPositionAdjustedToBestContextMenuNode(long x, long y, long width, long height, Document* document, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->frame()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return 0;
    }

    document->updateLayout();

    IntSize radius(width / 2, height / 2);
    IntPoint point(x + radius.width(), y + radius.height());

    EventHandler& eventHandler = document->frame()->eventHandler();
    IntPoint hitTestPoint = document->frame()->view()->rootFrameToContents(point);
    HitTestResult result = eventHandler.hitTestResultAtPoint(hitTestPoint, HitTestRequest::ReadOnly | HitTestRequest::Active | HitTestRequest::ListBased, LayoutSize(radius));

    Node* targetNode = 0;
    IntPoint adjustedPoint;

    bool foundNode = eventHandler.bestContextMenuNodeForHitTestResult(result, adjustedPoint, targetNode);
    if (foundNode)
        return DOMPoint::create(adjustedPoint.x(), adjustedPoint.y());

    return DOMPoint::create(x, y);
}

Node* Internals::touchNodeAdjustedToBestContextMenuNode(long x, long y, long width, long height, Document* document, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->frame()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return 0;
    }

    document->updateLayout();

    IntSize radius(width / 2, height / 2);
    IntPoint point(x + radius.width(), y + radius.height());

    EventHandler& eventHandler = document->frame()->eventHandler();
    IntPoint hitTestPoint = document->frame()->view()->rootFrameToContents(point);
    HitTestResult result = eventHandler.hitTestResultAtPoint(hitTestPoint, HitTestRequest::ReadOnly | HitTestRequest::Active | HitTestRequest::ListBased, LayoutSize(radius));

    Node* targetNode = 0;
    IntPoint adjustedPoint;
    eventHandler.bestContextMenuNodeForHitTestResult(result, adjustedPoint, targetNode);
    return targetNode;
}

ClientRect* Internals::bestZoomableAreaForTouchPoint(long x, long y, long width, long height, Document* document, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->frame()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return nullptr;
    }

    document->updateLayout();

    IntSize radius(width / 2, height / 2);
    IntPoint point(x + radius.width(), y + radius.height());

    Node* targetNode = 0;
    IntRect zoomableArea;
    bool foundNode = document->frame()->eventHandler().bestZoomableAreaForTouchPoint(point, radius, zoomableArea, targetNode);
    if (foundNode)
        return ClientRect::create(zoomableArea);

    return nullptr;
}


int Internals::lastSpellCheckRequestSequence(Document* document, ExceptionState& exceptionState)
{
    SpellCheckRequester* requester = spellCheckRequester(document);

    if (!requester) {
        exceptionState.throwDOMException(InvalidAccessError, "No spell check requestor can be obtained for the provided document.");
        return -1;
    }

    return requester->lastRequestSequence();
}

int Internals::lastSpellCheckProcessedSequence(Document* document, ExceptionState& exceptionState)
{
    SpellCheckRequester* requester = spellCheckRequester(document);

    if (!requester) {
        exceptionState.throwDOMException(InvalidAccessError, "No spell check requestor can be obtained for the provided document.");
        return -1;
    }

    return requester->lastProcessedSequence();
}

Vector<AtomicString> Internals::userPreferredLanguages() const
{
    return blink::userPreferredLanguages();
}

// Optimally, the bindings generator would pass a Vector<AtomicString> here but
// this is not supported yet.
void Internals::setUserPreferredLanguages(const Vector<String>& languages)
{
    Vector<AtomicString> atomicLanguages;
    for (size_t i = 0; i < languages.size(); ++i)
        atomicLanguages.append(AtomicString(languages[i]));
    overrideUserPreferredLanguages(atomicLanguages);
}

unsigned Internals::activeDOMObjectCount(Document* document)
{
    ASSERT(document);
    return document->activeDOMObjectCount();
}

static unsigned eventHandlerCount(Document& document, EventHandlerRegistry::EventHandlerClass handlerClass)
{
    if (!document.frameHost())
        return 0;
    EventHandlerRegistry* registry = &document.frameHost()->eventHandlerRegistry();
    unsigned count = 0;
    const EventTargetSet* targets = registry->eventHandlerTargets(handlerClass);
    if (targets) {
        for (const auto& target : *targets)
            count += target.value;
    }
    return count;
}

unsigned Internals::wheelEventHandlerCount(Document* document)
{
    ASSERT(document);
    return eventHandlerCount(*document, EventHandlerRegistry::WheelEvent);
}

unsigned Internals::scrollEventHandlerCount(Document* document)
{
    ASSERT(document);
    return eventHandlerCount(*document, EventHandlerRegistry::ScrollEvent);
}

unsigned Internals::touchEventHandlerCount(Document* document)
{
    ASSERT(document);
    return eventHandlerCount(*document, EventHandlerRegistry::TouchEvent);
}

static DeprecatedPaintLayer* findLayerForGraphicsLayer(DeprecatedPaintLayer* searchRoot, GraphicsLayer* graphicsLayer, IntSize* layerOffset, String* layerType)
{
    *layerOffset = IntSize();
    if (searchRoot->hasCompositedDeprecatedPaintLayerMapping() && graphicsLayer == searchRoot->compositedDeprecatedPaintLayerMapping()->mainGraphicsLayer()) {
        // If the |graphicsLayer| sets the scrollingContent layer as its
        // scroll parent, consider it belongs to the scrolling layer and
        // mark the layer type as "scrolling".
        if (!searchRoot->layoutObject()->hasTransformRelatedProperty() && searchRoot->scrollParent() && searchRoot->parent() == searchRoot->scrollParent()) {
            *layerType = "scrolling";
            // For hit-test rect visualization to work, the hit-test rect should
            // be relative to the scrolling layer and in this case the hit-test
            // rect is relative to the element's own GraphicsLayer. So we will have
            // to adjust the rect to be relative to the scrolling layer here.
            // Only when the element's offsetParent == scroller's offsetParent we
            // can compute the element's relative position to the scrolling content
            // in this way.
            if (searchRoot->layoutObject()->offsetParent() == searchRoot->parent()->layoutObject()->offsetParent()) {
                LayoutBoxModelObject* current = searchRoot->layoutObject();
                LayoutBoxModelObject* parent = searchRoot->parent()->layoutObject();
                layerOffset->setWidth((parent->offsetLeft() - current->offsetLeft()).toInt());
                layerOffset->setHeight((parent->offsetTop() - current->offsetTop()).toInt());
                return searchRoot->parent();
            }
        }

        LayoutRect rect;
        DeprecatedPaintLayer::mapRectToPaintBackingCoordinates(searchRoot->layoutObject(), rect);
        *layerOffset = IntSize(rect.x(), rect.y());
        return searchRoot;
    }

    // If the |graphicsLayer| is a scroller's scrollingContent layer,
    // consider this is a scrolling layer.
    GraphicsLayer* layerForScrolling = searchRoot->scrollableArea() ? searchRoot->scrollableArea()->layerForScrolling() : 0;
    if (graphicsLayer == layerForScrolling) {
        *layerType = "scrolling";
        return searchRoot;
    }

    if (searchRoot->compositingState() == PaintsIntoGroupedBacking) {
        GraphicsLayer* squashingLayer = searchRoot->groupedMapping()->squashingLayer();
        if (graphicsLayer == squashingLayer) {
            *layerType ="squashing";
            LayoutRect rect;
            DeprecatedPaintLayer::mapRectToPaintBackingCoordinates(searchRoot->layoutObject(), rect);
            *layerOffset = IntSize(rect.x(), rect.y());
            return searchRoot;
        }
    }

    GraphicsLayer* layerForHorizontalScrollbar = searchRoot->scrollableArea() ? searchRoot->scrollableArea()->layerForHorizontalScrollbar() : 0;
    if (graphicsLayer == layerForHorizontalScrollbar) {
        *layerType = "horizontalScrollbar";
        return searchRoot;
    }

    GraphicsLayer* layerForVerticalScrollbar = searchRoot->scrollableArea() ? searchRoot->scrollableArea()->layerForVerticalScrollbar() : 0;
    if (graphicsLayer == layerForVerticalScrollbar) {
        *layerType = "verticalScrollbar";
        return searchRoot;
    }

    GraphicsLayer* layerForScrollCorner = searchRoot->scrollableArea() ? searchRoot->scrollableArea()->layerForScrollCorner() : 0;
    if (graphicsLayer == layerForScrollCorner) {
        *layerType = "scrollCorner";
        return searchRoot;
    }

    // Search right to left to increase the chances that we'll choose the top-most layers in a
    // grouped mapping for squashing.
    for (DeprecatedPaintLayer* child = searchRoot->lastChild(); child; child = child->previousSibling()) {
        DeprecatedPaintLayer* foundLayer = findLayerForGraphicsLayer(child, graphicsLayer, layerOffset, layerType);
        if (foundLayer)
            return foundLayer;
    }

    return 0;
}

// Given a vector of rects, merge those that are adjacent, leaving empty rects
// in the place of no longer used slots. This is intended to simplify the list
// of rects returned by an SkRegion (which have been split apart for sorting
// purposes). No attempt is made to do this efficiently (eg. by relying on the
// sort criteria of SkRegion).
static void mergeRects(WebVector<blink::WebRect>& rects)
{
    for (size_t i = 0; i < rects.size(); ++i) {
        if (rects[i].isEmpty())
            continue;
        bool updated;
        do {
            updated = false;
            for (size_t j = i+1; j < rects.size(); ++j) {
                if (rects[j].isEmpty())
                    continue;
                // Try to merge rects[j] into rects[i] along the 4 possible edges.
                if (rects[i].y == rects[j].y && rects[i].height == rects[j].height) {
                    if (rects[i].x + rects[i].width == rects[j].x) {
                        rects[i].width += rects[j].width;
                        rects[j] = blink::WebRect();
                        updated = true;
                    } else if (rects[i].x == rects[j].x + rects[j].width) {
                        rects[i].x = rects[j].x;
                        rects[i].width += rects[j].width;
                        rects[j] = blink::WebRect();
                        updated = true;
                    }
                } else if (rects[i].x == rects[j].x && rects[i].width == rects[j].width) {
                    if (rects[i].y + rects[i].height == rects[j].y) {
                        rects[i].height += rects[j].height;
                        rects[j] = blink::WebRect();
                        updated = true;
                    } else if (rects[i].y == rects[j].y + rects[j].height) {
                        rects[i].y = rects[j].y;
                        rects[i].height += rects[j].height;
                        rects[j] = blink::WebRect();
                        updated = true;
                    }
                }
            }
        } while (updated);
    }
}

static void accumulateLayerRectList(DeprecatedPaintLayerCompositor* compositor, GraphicsLayer* graphicsLayer, LayerRectList* rects)
{
    WebVector<blink::WebRect> layerRects = graphicsLayer->platformLayer()->touchEventHandlerRegion();
    if (!layerRects.isEmpty()) {
        mergeRects(layerRects);
        String layerType;
        IntSize layerOffset;
        DeprecatedPaintLayer* paintLayer = findLayerForGraphicsLayer(compositor->rootLayer(), graphicsLayer, &layerOffset, &layerType);
        Node* node = paintLayer ? paintLayer->layoutObject()->node() : 0;
        for (size_t i = 0; i < layerRects.size(); ++i) {
            if (!layerRects[i].isEmpty()) {
                rects->append(node, layerType, layerOffset.width(), layerOffset.height(), ClientRect::create(layerRects[i]));
            }
        }
    }

    size_t numChildren = graphicsLayer->children().size();
    for (size_t i = 0; i < numChildren; ++i)
        accumulateLayerRectList(compositor, graphicsLayer->children()[i], rects);
}

LayerRectList* Internals::touchEventTargetLayerRects(Document* document, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->view() || !document->page() || document != contextDocument()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return nullptr;
    }

    // Do any pending layout and compositing update (which may call touchEventTargetRectsChange) to ensure this
    // really takes any previous changes into account.
    forceCompositingUpdate(document, exceptionState);
    if (exceptionState.hadException())
        return nullptr;

    if (LayoutView* view = document->layoutView()) {
        if (DeprecatedPaintLayerCompositor* compositor = view->compositor()) {
            if (GraphicsLayer* rootLayer = compositor->rootGraphicsLayer()) {
                LayerRectList* rects = LayerRectList::create();
                accumulateLayerRectList(compositor, rootLayer, rects);
                return rects;
            }
        }
    }

    return nullptr;
}

bool Internals::executeCommand(Document* document, const String& name, const String& value, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->frame()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return false;
    }

    LocalFrame* frame = document->frame();
    return frame->editor().executeCommand(name, value);
}

AtomicString Internals::htmlNamespace()
{
    return HTMLNames::xhtmlNamespaceURI;
}

Vector<AtomicString> Internals::htmlTags()
{
    Vector<AtomicString> tags(HTMLNames::HTMLTagsCount);
    OwnPtr<const HTMLQualifiedName*[]> qualifiedNames = HTMLNames::getHTMLTags();
    for (size_t i = 0; i < HTMLNames::HTMLTagsCount; ++i)
        tags[i] = qualifiedNames[i]->localName();
    return tags;
}

AtomicString Internals::svgNamespace()
{
    return SVGNames::svgNamespaceURI;
}

Vector<AtomicString> Internals::svgTags()
{
    Vector<AtomicString> tags(SVGNames::SVGTagsCount);
    OwnPtr<const SVGQualifiedName*[]> qualifiedNames = SVGNames::getSVGTags();
    for (size_t i = 0; i < SVGNames::SVGTagsCount; ++i)
        tags[i] = qualifiedNames[i]->localName();
    return tags;
}

PassRefPtrWillBeRawPtr<StaticNodeList> Internals::nodesFromRect(Document* document, int centerX, int centerY, unsigned topPadding, unsigned rightPadding,
    unsigned bottomPadding, unsigned leftPadding, bool ignoreClipping, bool allowChildFrameContent, ExceptionState& exceptionState) const
{
    ASSERT(document);
    if (!document->frame() || !document->frame()->view()) {
        exceptionState.throwDOMException(InvalidAccessError, "No view can be obtained from the provided document.");
        return nullptr;
    }

    LocalFrame* frame = document->frame();
    FrameView* frameView = document->view();
    LayoutView* layoutView = document->layoutView();

    if (!layoutView)
        return nullptr;

    float zoomFactor = frame->pageZoomFactor();
    LayoutPoint point = roundedLayoutPoint(FloatPoint(centerX * zoomFactor + frameView->scrollX(), centerY * zoomFactor + frameView->scrollY()));

    HitTestRequest::HitTestRequestType hitType = HitTestRequest::ReadOnly | HitTestRequest::Active | HitTestRequest::ListBased;
    if (ignoreClipping)
        hitType |= HitTestRequest::IgnoreClipping;
    if (allowChildFrameContent)
        hitType |= HitTestRequest::AllowChildFrameContent;

    HitTestRequest request(hitType);

    // When ignoreClipping is false, this method returns null for coordinates outside of the viewport.
    if (!request.ignoreClipping() && !frameView->visibleContentRect().intersects(HitTestLocation::rectForPoint(point, topPadding, rightPadding, bottomPadding, leftPadding)))
        return nullptr;

    WillBeHeapVector<RefPtrWillBeMember<Node>> matches;
    HitTestResult result(request, point, topPadding, rightPadding, bottomPadding, leftPadding);
    layoutView->hitTest(result);
    copyToVector(result.listBasedTestResult(), matches);

    return StaticNodeList::adopt(matches);
}

bool Internals::hasSpellingMarker(Document* document, int from, int length)
{
    ASSERT(document);
    if (!document->frame())
        return false;

    return document->frame()->spellChecker().selectionStartHasMarkerFor(DocumentMarker::Spelling, from, length);
}

void Internals::setContinuousSpellCheckingEnabled(bool enabled)
{
    if (!contextDocument() || !contextDocument()->frame())
        return;

    if (enabled != contextDocument()->frame()->spellChecker().isContinuousSpellCheckingEnabled())
        contextDocument()->frame()->spellChecker().toggleContinuousSpellChecking();
}

bool Internals::isOverwriteModeEnabled(Document* document)
{
    ASSERT(document);
    if (!document->frame())
        return false;

    return document->frame()->editor().isOverwriteModeEnabled();
}

void Internals::toggleOverwriteModeEnabled(Document* document)
{
    ASSERT(document);
    if (!document->frame())
        return;

    document->frame()->editor().toggleOverwriteModeEnabled();
}

unsigned Internals::numberOfLiveNodes() const
{
    return InstanceCounters::counterValue(InstanceCounters::NodeCounter);
}

unsigned Internals::numberOfLiveDocuments() const
{
    return InstanceCounters::counterValue(InstanceCounters::DocumentCounter);
}

String Internals::dumpRefCountedInstanceCounts() const
{
    return WTF::dumpRefCountedInstanceCounts();
}

Vector<String> Internals::consoleMessageArgumentCounts(Document* document) const
{
    FrameHost* host = document->frameHost();
    if (!host)
        return Vector<String>();

    Vector<unsigned> counts = host->consoleMessageStorage().argumentCounts();
    Vector<String> result(counts.size());
    for (size_t i = 0; i < counts.size(); i++)
        result[i] = String::number(counts[i]);
    return result;
}

Vector<unsigned long> Internals::setMemoryCacheCapacities(unsigned long minDeadBytes, unsigned long maxDeadBytes, unsigned long totalBytes)
{
    Vector<unsigned long> result;
    result.append(memoryCache()->minDeadCapacity());
    result.append(memoryCache()->maxDeadCapacity());
    result.append(memoryCache()->capacity());
    memoryCache()->setCapacities(minDeadBytes, maxDeadBytes, totalBytes);
    return result;
}

bool Internals::hasGrammarMarker(Document* document, int from, int length)
{
    ASSERT(document);
    if (!document->frame())
        return false;

    return document->frame()->spellChecker().selectionStartHasMarkerFor(DocumentMarker::Grammar, from, length);
}

unsigned Internals::numberOfScrollableAreas(Document* document)
{
    ASSERT(document);
    if (!document->frame())
        return 0;

    unsigned count = 0;
    LocalFrame* frame = document->frame();
    if (frame->view()->scrollableAreas())
        count += frame->view()->scrollableAreas()->size();

    for (Frame* child = frame->tree().firstChild(); child; child = child->tree().nextSibling()) {
        if (child->isLocalFrame() && toLocalFrame(child)->view() && toLocalFrame(child)->view()->scrollableAreas())
            count += toLocalFrame(child)->view()->scrollableAreas()->size();
    }

    return count;
}

bool Internals::isPageBoxVisible(Document* document, int pageNumber)
{
    ASSERT(document);
    return document->isPageBoxVisible(pageNumber);
}

String Internals::layerTreeAsText(Document* document, ExceptionState& exceptionState) const
{
    return layerTreeAsText(document, 0, exceptionState);
}

String Internals::elementLayerTreeAsText(Element* element, ExceptionState& exceptionState) const
{
    ASSERT(element);
    FrameView* frameView = element->document().view();
    frameView->updateAllLifecyclePhases();

    return elementLayerTreeAsText(element, 0, exceptionState);
}

bool Internals::scrollsWithRespectTo(Element* element1, Element* element2, ExceptionState& exceptionState)
{
    ASSERT(element1 && element2);
    element1->document().view()->updateAllLifecyclePhases();

    LayoutObject* layoutObject1 = element1->layoutObject();
    LayoutObject* layoutObject2 = element2->layoutObject();
    if (!layoutObject1 || !layoutObject1->isBox()) {
        exceptionState.throwDOMException(InvalidAccessError, layoutObject1 ? "The first provided element's layoutObject is not a box." : "The first provided element has no layoutObject.");
        return false;
    }
    if (!layoutObject2 || !layoutObject2->isBox()) {
        exceptionState.throwDOMException(InvalidAccessError, layoutObject2 ? "The second provided element's layoutObject is not a box." : "The second provided element has no layoutObject.");
        return false;
    }

    DeprecatedPaintLayer* layer1 = toLayoutBox(layoutObject1)->layer();
    DeprecatedPaintLayer* layer2 = toLayoutBox(layoutObject2)->layer();
    if (!layer1 || !layer2) {
        exceptionState.throwDOMException(InvalidAccessError, String::format("No DeprecatedPaintLayer can be obtained from the %s provided element.", layer1 ? "second" : "first"));
        return false;
    }

    return layer1->scrollsWithRespectTo(layer2);
}

String Internals::layerTreeAsText(Document* document, unsigned flags, ExceptionState& exceptionState) const
{
    ASSERT(document);
    if (!document->frame()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return String();
    }

    document->view()->updateAllLifecyclePhases();

    return document->frame()->layerTreeAsText(flags);
}

String Internals::elementLayerTreeAsText(Element* element, unsigned flags, ExceptionState& exceptionState) const
{
    ASSERT(element);
    element->document().updateLayout();

    LayoutObject* layoutObject = element->layoutObject();
    if (!layoutObject || !layoutObject->isBox()) {
        exceptionState.throwDOMException(InvalidAccessError, layoutObject ? "The provided element's layoutObject is not a box." : "The provided element has no layoutObject.");
        return String();
    }

    DeprecatedPaintLayer* layer = toLayoutBox(layoutObject)->layer();
    if (!layer
        || !layer->hasCompositedDeprecatedPaintLayerMapping()
        || !layer->compositedDeprecatedPaintLayerMapping()->mainGraphicsLayer()) {
        // Don't raise exception in these cases which may be normally used in tests.
        return String();
    }

    return layer->compositedDeprecatedPaintLayerMapping()->mainGraphicsLayer()->layerTreeAsText(flags);
}

String Internals::scrollingStateTreeAsText(Document*) const
{
    return String();
}

String Internals::mainThreadScrollingReasons(Document* document, ExceptionState& exceptionState) const
{
    ASSERT(document);
    if (!document->frame()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return String();
    }

    document->frame()->view()->updateAllLifecyclePhases();

    Page* page = document->page();
    if (!page)
        return String();

    return page->mainThreadScrollingReasonsAsText();
}

ClientRectList* Internals::nonFastScrollableRects(Document* document, ExceptionState& exceptionState) const
{
    ASSERT(document);
    if (!document->frame()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return nullptr;
    }

    Page* page = document->page();
    if (!page)
        return nullptr;

    return page->nonFastScrollableRects(document->frame());
}

void Internals::garbageCollectDocumentResources(Document* document) const
{
    ASSERT(document);
    ResourceFetcher* fetcher = document->fetcher();
    if (!fetcher)
        return;
    fetcher->garbageCollectDocumentResources();
}

void Internals::evictAllResources() const
{
    memoryCache()->evictResources();
}

String Internals::counterValue(Element* element)
{
    if (!element)
        return String();

    return counterValueForElement(element);
}

int Internals::pageNumber(Element* element, float pageWidth, float pageHeight)
{
    if (!element)
        return 0;

    return PrintContext::pageNumberForElement(element, FloatSize(pageWidth, pageHeight));
}

Vector<String> Internals::iconURLs(Document* document, int iconTypesMask) const
{
    Vector<IconURL> iconURLs = document->iconURLs(iconTypesMask);
    Vector<String> array;

    for (auto& iconURL : iconURLs)
        array.append(iconURL.m_iconURL.string());

    return array;
}

Vector<String> Internals::shortcutIconURLs(Document* document) const
{
    return iconURLs(document, Favicon);
}

Vector<String> Internals::allIconURLs(Document* document) const
{
    return iconURLs(document, Favicon | TouchIcon | TouchPrecomposedIcon);
}

int Internals::numberOfPages(float pageWidth, float pageHeight)
{
    if (!frame())
        return -1;

    return PrintContext::numberOfPages(frame(), FloatSize(pageWidth, pageHeight));
}

String Internals::pageProperty(String propertyName, int pageNumber, ExceptionState& exceptionState) const
{
    if (!frame()) {
        exceptionState.throwDOMException(InvalidAccessError, "No frame is available.");
        return String();
    }

    return PrintContext::pageProperty(frame(), propertyName.utf8().data(), pageNumber);
}

String Internals::pageSizeAndMarginsInPixels(int pageNumber, int width, int height, int marginTop, int marginRight, int marginBottom, int marginLeft, ExceptionState& exceptionState) const
{
    if (!frame()) {
        exceptionState.throwDOMException(InvalidAccessError, "No frame is available.");
        return String();
    }

    return PrintContext::pageSizeAndMarginsInPixels(frame(), pageNumber, width, height, marginTop, marginRight, marginBottom, marginLeft);
}

void Internals::setDeviceScaleFactor(float scaleFactor, ExceptionState& exceptionState)
{
    Document* document = contextDocument();
    if (!document || !document->page()) {
        exceptionState.throwDOMException(InvalidAccessError, document ? "The document's page cannot be retrieved." : "No context document can be obtained.");
        return;
    }
    Page* page = document->page();
    page->setDeviceScaleFactor(scaleFactor);
}

void Internals::setPageScaleFactor(float scaleFactor, ExceptionState& exceptionState)
{
    Document* document = contextDocument();
    if (!document || !document->page()) {
        exceptionState.throwDOMException(InvalidAccessError, document ? "The document's page cannot be retrieved." : "No context document can be obtained.");
        return;
    }
    Page* page = document->page();
    page->frameHost().pinchViewport().setScale(scaleFactor);
}

void Internals::setPageScaleFactorLimits(float minScaleFactor, float maxScaleFactor, ExceptionState& exceptionState)
{
    Document* document = contextDocument();
    if (!document || !document->page()) {
        exceptionState.throwDOMException(InvalidAccessError, document ? "The document's page cannot be retrieved." : "No context document can be obtained.");
        return;
    }

    Page* page = document->page();
    page->frameHost().setDefaultPageScaleLimits(minScaleFactor, maxScaleFactor);
}

void Internals::setIsCursorVisible(Document* document, bool isVisible, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->page()) {
        exceptionState.throwDOMException(InvalidAccessError, "No context document can be obtained.");
        return;
    }
    document->page()->setIsCursorVisible(isVisible);
}

double Internals::effectiveMediaVolume(HTMLMediaElement* mediaElement)
{
    ASSERT(mediaElement);
    return mediaElement->effectiveMediaVolume();
}

void Internals::mediaPlayerRemoteRouteAvailabilityChanged(HTMLMediaElement* mediaElement, bool available)
{
    ASSERT(mediaElement);
    mediaElement->remoteRouteAvailabilityChanged(available);
}

void Internals::mediaPlayerPlayingRemotelyChanged(HTMLMediaElement* mediaElement, bool remote)
{
    ASSERT(mediaElement);
    if (remote)
        mediaElement->connectedToRemoteDevice();
    else
        mediaElement->disconnectedFromRemoteDevice();
}

void Internals::registerURLSchemeAsBypassingContentSecurityPolicy(const String& scheme)
{
    SchemeRegistry::registerURLSchemeAsBypassingContentSecurityPolicy(scheme);
}

void Internals::registerURLSchemeAsBypassingContentSecurityPolicy(const String& scheme, const Vector<String>& policyAreas)
{
    uint32_t policyAreasEnum = SchemeRegistry::PolicyAreaNone;
    for (const auto& policyArea : policyAreas) {
        if (policyArea == "img")
            policyAreasEnum |= SchemeRegistry::PolicyAreaImage;
        else if (policyArea == "style")
            policyAreasEnum |= SchemeRegistry::PolicyAreaStyle;
    }
    SchemeRegistry::registerURLSchemeAsBypassingContentSecurityPolicy(
        scheme, static_cast<SchemeRegistry::PolicyAreas>(policyAreasEnum));
}

void Internals::removeURLSchemeRegisteredAsBypassingContentSecurityPolicy(const String& scheme)
{
    SchemeRegistry::removeURLSchemeRegisteredAsBypassingContentSecurityPolicy(scheme);
}

TypeConversions* Internals::typeConversions() const
{
    return TypeConversions::create();
}

PrivateScriptTest* Internals::privateScriptTest() const
{
    return PrivateScriptTest::create(frame()->document());
}

DictionaryTest* Internals::dictionaryTest() const
{
    return DictionaryTest::create();
}

UnionTypesTest* Internals::unionTypesTest() const
{
    return UnionTypesTest::create();
}

Vector<String> Internals::getReferencedFilePaths() const
{
    return frame()->loader().currentItem()->getReferencedFilePaths();
}

void Internals::startTrackingRepaints(Document* document, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->view()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return;
    }

    FrameView* frameView = document->view();
    frameView->updateAllLifecyclePhases();
    frameView->setTracksPaintInvalidations(true);
}

void Internals::stopTrackingRepaints(Document* document, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->view()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return;
    }

    FrameView* frameView = document->view();
    frameView->updateAllLifecyclePhases();
    frameView->setTracksPaintInvalidations(false);
}

void Internals::updateLayoutIgnorePendingStylesheetsAndRunPostLayoutTasks(Node* node, ExceptionState& exceptionState)
{
    Document* document;
    if (!node) {
        document = contextDocument();
    } else if (node->isDocumentNode()) {
        document = toDocument(node);
    } else if (isHTMLIFrameElement(*node)) {
        document = toHTMLIFrameElement(*node).contentDocument();
    } else {
        exceptionState.throwTypeError("The node provided is neither a document nor an IFrame.");
        return;
    }
    document->updateLayoutIgnorePendingStylesheets(Document::RunPostLayoutTasksSynchronously);
}

void Internals::forceFullRepaint(Document* document, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->view()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return;
    }

    if (LayoutView *layoutView = document->layoutView())
        layoutView->invalidatePaintForViewAndCompositedLayers();
}

ClientRectList* Internals::draggableRegions(Document* document, ExceptionState& exceptionState)
{
    return annotatedRegions(document, true, exceptionState);
}

ClientRectList* Internals::nonDraggableRegions(Document* document, ExceptionState& exceptionState)
{
    return annotatedRegions(document, false, exceptionState);
}

ClientRectList* Internals::annotatedRegions(Document* document, bool draggable, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->view()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return ClientRectList::create();
    }

    document->updateLayout();
    document->view()->updateAnnotatedRegions();
    Vector<AnnotatedRegionValue> regions = document->annotatedRegions();

    Vector<FloatQuad> quads;
    for (size_t i = 0; i < regions.size(); ++i) {
        if (regions[i].draggable == draggable)
            quads.append(FloatQuad(regions[i].bounds));
    }
    return ClientRectList::create(quads);
}

static const char* cursorTypeToString(Cursor::Type cursorType)
{
    switch (cursorType) {
    case Cursor::Pointer: return "Pointer";
    case Cursor::Cross: return "Cross";
    case Cursor::Hand: return "Hand";
    case Cursor::IBeam: return "IBeam";
    case Cursor::Wait: return "Wait";
    case Cursor::Help: return "Help";
    case Cursor::EastResize: return "EastResize";
    case Cursor::NorthResize: return "NorthResize";
    case Cursor::NorthEastResize: return "NorthEastResize";
    case Cursor::NorthWestResize: return "NorthWestResize";
    case Cursor::SouthResize: return "SouthResize";
    case Cursor::SouthEastResize: return "SouthEastResize";
    case Cursor::SouthWestResize: return "SouthWestResize";
    case Cursor::WestResize: return "WestResize";
    case Cursor::NorthSouthResize: return "NorthSouthResize";
    case Cursor::EastWestResize: return "EastWestResize";
    case Cursor::NorthEastSouthWestResize: return "NorthEastSouthWestResize";
    case Cursor::NorthWestSouthEastResize: return "NorthWestSouthEastResize";
    case Cursor::ColumnResize: return "ColumnResize";
    case Cursor::RowResize: return "RowResize";
    case Cursor::MiddlePanning: return "MiddlePanning";
    case Cursor::EastPanning: return "EastPanning";
    case Cursor::NorthPanning: return "NorthPanning";
    case Cursor::NorthEastPanning: return "NorthEastPanning";
    case Cursor::NorthWestPanning: return "NorthWestPanning";
    case Cursor::SouthPanning: return "SouthPanning";
    case Cursor::SouthEastPanning: return "SouthEastPanning";
    case Cursor::SouthWestPanning: return "SouthWestPanning";
    case Cursor::WestPanning: return "WestPanning";
    case Cursor::Move: return "Move";
    case Cursor::VerticalText: return "VerticalText";
    case Cursor::Cell: return "Cell";
    case Cursor::ContextMenu: return "ContextMenu";
    case Cursor::Alias: return "Alias";
    case Cursor::Progress: return "Progress";
    case Cursor::NoDrop: return "NoDrop";
    case Cursor::Copy: return "Copy";
    case Cursor::None: return "None";
    case Cursor::NotAllowed: return "NotAllowed";
    case Cursor::ZoomIn: return "ZoomIn";
    case Cursor::ZoomOut: return "ZoomOut";
    case Cursor::Grab: return "Grab";
    case Cursor::Grabbing: return "Grabbing";
    case Cursor::Custom: return "Custom";
    }

    ASSERT_NOT_REACHED();
    return "UNKNOWN";
}

String Internals::getCurrentCursorInfo()
{
    Cursor cursor = frame()->page()->chromeClient().lastSetCursorForTesting();

    StringBuilder result;
    result.appendLiteral("type=");
    result.append(cursorTypeToString(cursor.type()));
    result.appendLiteral(" hotSpot=");
    result.appendNumber(cursor.hotSpot().x());
    result.append(',');
    result.appendNumber(cursor.hotSpot().y());
    if (cursor.image()) {
        IntSize size = cursor.image()->size();
        result.appendLiteral(" image=");
        result.appendNumber(size.width());
        result.append('x');
        result.appendNumber(size.height());
    }
    if (cursor.imageScaleFactor() != 1) {
        result.appendLiteral(" scale=");
        NumberToStringBuffer buffer;
        result.append(numberToFixedPrecisionString(cursor.imageScaleFactor(), 8, buffer, true));
    }

    return result.toString();
}

bool Internals::cursorUpdatePending() const
{
    return frame()->eventHandler().cursorUpdatePending();
}

PassRefPtr<DOMArrayBuffer> Internals::serializeObject(PassRefPtr<SerializedScriptValue> value) const
{
    String stringValue = value->toWireString();
    RefPtr<DOMArrayBuffer> buffer = DOMArrayBuffer::createUninitialized(stringValue.length(), sizeof(UChar));
    stringValue.copyTo(static_cast<UChar*>(buffer->data()), 0, stringValue.length());
    return buffer.release();
}

PassRefPtr<SerializedScriptValue> Internals::deserializeBuffer(PassRefPtr<DOMArrayBuffer> buffer) const
{
    String value(static_cast<const UChar*>(buffer->data()), buffer->byteLength() / sizeof(UChar));
    return SerializedScriptValueFactory::instance().createFromWire(value);
}

void Internals::forceReload(bool endToEnd)
{
    frame()->reload(endToEnd ? FrameLoadTypeReloadFromOrigin : FrameLoadTypeReload, NotClientRedirect);
}

ClientRect* Internals::selectionBounds(ExceptionState& exceptionState)
{
    Document* document = contextDocument();
    if (!document || !document->frame()) {
        exceptionState.throwDOMException(InvalidAccessError, document ? "The document's frame cannot be retrieved." : "No context document can be obtained.");
        return nullptr;
    }

    return ClientRect::create(document->frame()->selection().bounds());
}

String Internals::markerTextForListItem(Element* element)
{
    ASSERT(element);
    return blink::markerTextForListItem(element);
}

String Internals::getImageSourceURL(Element* element)
{
    ASSERT(element);
    return element->imageSourceURL();
}

String Internals::selectMenuListText(HTMLSelectElement* select)
{
    ASSERT(select);
    LayoutObject* layoutObject = select->layoutObject();
    if (!layoutObject || !layoutObject->isMenuList())
        return String();

    LayoutMenuList* menuList = toLayoutMenuList(layoutObject);
    return menuList->text();
}

bool Internals::isSelectPopupVisible(Node* node)
{
    ASSERT(node);
    if (!isHTMLSelectElement(*node))
        return false;

    HTMLSelectElement& select = toHTMLSelectElement(*node);

    LayoutObject* layoutObject = select.layoutObject();
    if (!layoutObject || !layoutObject->isMenuList())
        return false;

    LayoutMenuList* menuList = toLayoutMenuList(layoutObject);
    return menuList->popupIsVisible();
}

bool Internals::selectPopupItemStyleIsRtl(Node* node, int itemIndex)
{
    if (!node || !isHTMLSelectElement(*node))
        return false;

    HTMLSelectElement& select = toHTMLSelectElement(*node);

    LayoutObject* layoutObject = select.layoutObject();
    if (!layoutObject || !layoutObject->isMenuList())
        return false;

    LayoutMenuList& menuList = toLayoutMenuList(*layoutObject);
    PopupMenuStyle itemStyle = menuList.itemStyle(itemIndex);
    return itemStyle.textDirection() == RTL;
}

int Internals::selectPopupItemStyleFontHeight(Node* node, int itemIndex)
{
    if (!node || !isHTMLSelectElement(*node))
        return false;

    HTMLSelectElement& select = toHTMLSelectElement(*node);

    LayoutObject* layoutObject = select.layoutObject();
    if (!layoutObject || !layoutObject->isMenuList())
        return false;

    LayoutMenuList& menuList = toLayoutMenuList(*layoutObject);
    PopupMenuStyle itemStyle = menuList.itemStyle(itemIndex);
    return itemStyle.font().fontMetrics().height();
}

bool Internals::loseSharedGraphicsContext3D()
{
    OwnPtr<WebGraphicsContext3DProvider> sharedProvider = adoptPtr(Platform::current()->createSharedOffscreenGraphicsContext3DProvider());
    if (!sharedProvider)
        return false;
    WebGraphicsContext3D* sharedContext = sharedProvider->context3d();
    sharedContext->loseContextCHROMIUM(GL_GUILTY_CONTEXT_RESET_EXT, GL_INNOCENT_CONTEXT_RESET_EXT);
    // To prevent tests that call loseSharedGraphicsContext3D from being
    // flaky, we call finish so that the context is guaranteed to be lost
    // synchronously (i.e. before returning).
    sharedContext->finish();
    return true;
}

void Internals::forceCompositingUpdate(Document* document, ExceptionState& exceptionState)
{
    ASSERT(document);
    if (!document->layoutView()) {
        exceptionState.throwDOMException(InvalidAccessError, "The document provided is invalid.");
        return;
    }

    document->frame()->view()->updateAllLifecyclePhases();
}

void Internals::setZoomFactor(float factor)
{
    frame()->setPageZoomFactor(factor);
}

void Internals::setShouldRevealPassword(Element* element, bool reveal, ExceptionState& exceptionState)
{
    ASSERT(element);
    if (!isHTMLInputElement(element)) {
        exceptionState.throwDOMException(InvalidNodeTypeError, "The element provided is not an INPUT.");
        return;
    }

    return toHTMLInputElement(*element).setShouldRevealPassword(reveal);
}

namespace {

class AddOneFunction : public ScriptFunction {
public:
    static v8::Local<v8::Function> createFunction(ScriptState* scriptState)
    {
        AddOneFunction* self = new AddOneFunction(scriptState);
        return self->bindToV8Function();
    }

private:
    explicit AddOneFunction(ScriptState* scriptState)
        : ScriptFunction(scriptState)
    {
    }

    virtual ScriptValue call(ScriptValue value) override
    {
        v8::Local<v8::Value> v8Value = value.v8Value();
        ASSERT(v8Value->IsNumber());
        int intValue = v8Value.As<v8::Integer>()->Value();
        return ScriptValue(scriptState(), v8::Integer::New(scriptState()->isolate(), intValue + 1));
    }
};

} // namespace

ScriptPromise Internals::createResolvedPromise(ScriptState* scriptState, ScriptValue value)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    resolver->resolve(value);
    return promise;
}

ScriptPromise Internals::createRejectedPromise(ScriptState* scriptState, ScriptValue value)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    resolver->reject(value);
    return promise;
}

ScriptPromise Internals::addOneToPromise(ScriptState* scriptState, ScriptPromise promise)
{
    return promise.then(AddOneFunction::createFunction(scriptState));
}

ScriptPromise Internals::promiseCheck(ScriptState* scriptState, long arg1, bool arg2, const Dictionary& arg3, const String& arg4, const Vector<String>& arg5, ExceptionState& exceptionState)
{
    if (arg2)
        return ScriptPromise::cast(scriptState, v8String(scriptState->isolate(), "done"));
    exceptionState.throwDOMException(InvalidStateError, "Thrown from the native implementation.");
    return ScriptPromise();
}

ScriptPromise Internals::promiseCheckWithoutExceptionState(ScriptState* scriptState, const Dictionary& arg1, const String& arg2, const Vector<String>& arg3)
{
    return ScriptPromise::cast(scriptState, v8String(scriptState->isolate(), "done"));
}

ScriptPromise Internals::promiseCheckRange(ScriptState* scriptState, long arg1)
{
    return ScriptPromise::cast(scriptState, v8String(scriptState->isolate(), "done"));
}

ScriptPromise Internals::promiseCheckOverload(ScriptState* scriptState, Location*)
{
    return ScriptPromise::cast(scriptState, v8String(scriptState->isolate(), "done"));
}

ScriptPromise Internals::promiseCheckOverload(ScriptState* scriptState, Document*)
{
    return ScriptPromise::cast(scriptState, v8String(scriptState->isolate(), "done"));
}

ScriptPromise Internals::promiseCheckOverload(ScriptState* scriptState, Location*, long, long)
{
    return ScriptPromise::cast(scriptState, v8String(scriptState->isolate(), "done"));
}

DEFINE_TRACE(Internals)
{
    visitor->trace(m_runtimeFlags);
    ContextLifecycleObserver::trace(visitor);
}

void Internals::setValueForUser(Element* element, const String& value)
{
    toHTMLInputElement(element)->setValueForUser(value);
}

String Internals::textSurroundingNode(Node* node, int x, int y, unsigned long maxLength)
{
    if (!node)
        return String();
    blink::WebPoint point(x, y);
    SurroundingText surroundingText(VisiblePosition(node->layoutObject()->positionForPoint(static_cast<IntPoint>(point))).deepEquivalent().parentAnchoredEquivalent(), maxLength);
    return surroundingText.content();
}

void Internals::setFocused(bool focused)
{
    frame()->page()->focusController().setFocused(focused);
}

void Internals::setInitialFocus(bool reverse)
{
    frame()->document()->setFocusedElement(nullptr);
    frame()->page()->focusController().setInitialFocus(reverse ? WebFocusTypeBackward : WebFocusTypeForward);
}

bool Internals::ignoreLayoutWithPendingStylesheets(Document* document)
{
    ASSERT(document);
    return document->ignoreLayoutWithPendingStylesheets();
}

void Internals::setNetworkStateNotifierTestOnly(bool testOnly)
{
    networkStateNotifier().setTestUpdatesOnly(testOnly);
}

void Internals::setNetworkConnectionInfo(const String& type, ExceptionState& exceptionState)
{
    WebConnectionType webtype;
    if (type == "cellular") {
        webtype = ConnectionTypeCellular;
    } else if (type == "bluetooth") {
        webtype = ConnectionTypeBluetooth;
    } else if (type == "ethernet") {
        webtype = ConnectionTypeEthernet;
    } else if (type == "wifi") {
        webtype = ConnectionTypeWifi;
    } else if (type == "other") {
        webtype = ConnectionTypeOther;
    } else if (type == "none") {
        webtype = ConnectionTypeNone;
    } else if (type == "unknown") {
        webtype = ConnectionTypeUnknown;
    } else {
        exceptionState.throwDOMException(NotFoundError, ExceptionMessages::failedToEnumerate("connection type", type));
        return;
    }
    networkStateNotifier().setWebConnectionTypeForTest(webtype);
}

unsigned Internals::countHitRegions(CanvasRenderingContext* context)
{
    return context->hitRegionsCount();
}

bool Internals::isInCanvasFontCache(Document* document, const String& fontString)
{
    return document->canvasFontCache()->isInCache(fontString);
}

unsigned Internals::canvasFontCacheMaxFonts()
{
    return CanvasFontCache::maxFonts();
}

ClientRect* Internals::boundsInViewportSpace(Element* element)
{
    ASSERT(element);
    return ClientRect::create(element->boundsInViewportSpace());
}

void Internals::forcePluginPlaceholder(HTMLElement* element, PassRefPtrWillBeRawPtr<DocumentFragment> fragment, ExceptionState& exceptionState)
{
    if (!element->isPluginElement()) {
        exceptionState.throwDOMException(InvalidNodeTypeError, "The element provided is not a plugin.");
        return;
    }
    toHTMLPlugInElement(element)->setPlaceholder(DocumentFragmentPluginPlaceholder::create(fragment));
}

void Internals::forcePluginPlaceholder(HTMLElement* element, const PluginPlaceholderOptions& options, ExceptionState& exceptionState)
{
    if (!element->isPluginElement()) {
        exceptionState.throwDOMException(InvalidNodeTypeError, "The element provided is not a plugin.");
        return;
    }
    toHTMLPlugInElement(element)->setPlaceholder(DictionaryPluginPlaceholder::create(element->document(), options));
}

void Internals::forceBlinkGCWithoutV8GC()
{
    ThreadState::current()->setGCState(ThreadState::FullGCScheduled);
}

String Internals::selectedHTMLForClipboard()
{
    return frame()->selection().selectedHTMLForClipboard();
}

String Internals::selectedTextForClipboard()
{
    return frame()->selection().selectedTextForClipboard();
}

void Internals::setVisualViewportOffset(int x, int y)
{
    frame()->host()->pinchViewport().setLocation(FloatPoint(x, y));
}

ValueIterable<int>::IterationSource* Internals::startIteration(ScriptState*, ExceptionState&)
{
    return new InternalsIterationSource();
}

bool Internals::isUseCounted(Document* document, int useCounterId)
{
    if (useCounterId < 0 || useCounterId >= UseCounter::NumberOfFeatures)
        return false;
    return UseCounter::isCounted(*document, static_cast<UseCounter::Feature>(useCounterId));
}

String Internals::unscopeableAttribute()
{
    return "unscopeableAttribute";
}

String Internals::unscopeableMethod()
{
    return "unscopeableMethod";
}

ClientRectList* Internals::focusRingRects(Element* element)
{
    Vector<LayoutRect> rects;
    if (element && element->layoutObject())
        element->layoutObject()->addFocusRingRects(rects, LayoutPoint());
    return ClientRectList::create(rects);
}

void Internals::setCapsLockState(bool enabled)
{
    PlatformKeyboardEvent::setCurrentCapsLockState(enabled ?
        PlatformKeyboardEvent::OverrideCapsLockState::On : PlatformKeyboardEvent::OverrideCapsLockState::Off);
}

} // namespace blink

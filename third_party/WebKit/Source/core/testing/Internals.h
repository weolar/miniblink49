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
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Internals_h
#define Internals_h

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/core/v8/Iterable.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "core/css/CSSComputedStyleDeclaration.h"
#include "core/dom/ContextLifecycleObserver.h"
#include "core/page/scrolling/ScrollingCoordinator.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class CanvasRenderingContext;
class ClientRect;
class ClientRectList;
class DOMArrayBuffer;
class DOMPoint;
class DictionaryTest;
class Document;
class DocumentFragment;
class DocumentMarker;
class Element;
class ExceptionState;
class GCObservation;
class HTMLElement;
class HTMLMediaElement;
class InternalRuntimeFlags;
class InternalSettings;
class Iterator;
class LayerRectList;
class LocalDOMWindow;
class LocalFrame;
class Node;
class Page;
class PluginPlaceholderOptions;
class PrivateScriptTest;
class Range;
class SerializedScriptValue;
class ShadowRoot;
class TypeConversions;
class UnionTypesTest;
template <typename NodeType> class StaticNodeTypeList;
typedef StaticNodeTypeList<Node> StaticNodeList;

class Internals final : public GarbageCollectedFinalized<Internals>, public ScriptWrappable, public ContextLifecycleObserver, public ValueIterable<int> {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(Internals);
public:
    static Internals* create(ScriptState*);
    virtual ~Internals();

    static void resetToConsistentState(Page*);

    String elementLayoutTreeAsText(Element*, ExceptionState&);

    String address(Node*);

    GCObservation* observeGC(ScriptValue);

    bool isPreloaded(const String& url);
    bool isPreloadedBy(const String& url, Document*);
    bool isLoadingFromMemoryCache(const String& url);

    bool isSharingStyle(Element*, Element*) const;

    PassRefPtrWillBeRawPtr<CSSStyleDeclaration> computedStyleIncludingVisitedInfo(Node*) const;

    PassRefPtrWillBeRawPtr<ShadowRoot> createUserAgentShadowRoot(Element* host);

    ShadowRoot* shadowRoot(Element* host);
    ShadowRoot* youngestShadowRoot(Element* host);
    ShadowRoot* oldestShadowRoot(Element* host);
    ShadowRoot* youngerShadowRoot(Node* shadow, ExceptionState&);
    String shadowRootType(const Node*, ExceptionState&) const;
    bool hasShadowInsertionPoint(const Node*, ExceptionState&) const;
    bool hasContentElement(const Node*, ExceptionState&) const;
    size_t countElementShadow(const Node*, ExceptionState&) const;
    const AtomicString& shadowPseudoId(Element*);

    // Animation testing.
    void pauseAnimations(double pauseTime, ExceptionState&);
    bool isCompositedAnimation(Animation*);

    // Modifies m_desiredFrameStartTime in BitmapImage to advance the next frame time
    // for testing whether animated images work properly.
    void advanceTimeForImage(Element* image, double deltaTimeInSeconds, ExceptionState&);

    bool isValidContentSelect(Element* insertionPoint, ExceptionState&);
    Node* treeScopeRootNode(Node*);
    Node* parentTreeScope(Node*);
    bool hasSelectorForIdInShadow(Element* host, const AtomicString& idValue, ExceptionState&);
    bool hasSelectorForClassInShadow(Element* host, const AtomicString& className, ExceptionState&);
    bool hasSelectorForAttributeInShadow(Element* host, const AtomicString& attributeName, ExceptionState&);
    unsigned short compareTreeScopePosition(const Node*, const Node*, ExceptionState&) const;

    Node* nextSiblingInComposedTree(Node*, ExceptionState&);
    Node* firstChildInComposedTree(Node*, ExceptionState&);
    Node* lastChildInComposedTree(Node*, ExceptionState&);
    Node* nextInComposedTree(Node*, ExceptionState&);
    Node* previousInComposedTree(Node*, ExceptionState&);

    unsigned updateStyleAndReturnAffectedElementCount(ExceptionState&) const;
    unsigned needsLayoutCount(ExceptionState&) const;
    unsigned hitTestCount(Document*, ExceptionState&) const;
    unsigned hitTestCacheHits(Document*, ExceptionState&) const;
    Element* elementFromPoint(Document*, double x, double y, bool ignoreClipping, bool allowChildFrameContent, ExceptionState&) const;
    void clearHitTestCache(Document*, ExceptionState&) const;

    String visiblePlaceholder(Element*);
    void selectColorInColorChooser(Element*, const String& colorValue);
    void endColorChooser(Element*);
    bool hasAutofocusRequest(Document*);
    bool hasAutofocusRequest();
    Vector<String> formControlStateOfHistoryItem(ExceptionState&);
    void setFormControlStateOfHistoryItem(const Vector<String>&, ExceptionState&);
    DOMWindow* pagePopupWindow() const;

    ClientRect* absoluteCaretBounds(ExceptionState&);

    ClientRect* boundingBox(Element*);

    unsigned markerCountForNode(Node*, const String&, ExceptionState&);
    unsigned activeMarkerCountForNode(Node*);
    PassRefPtrWillBeRawPtr<Range> markerRangeForNode(Node*, const String& markerType, unsigned index, ExceptionState&);
    String markerDescriptionForNode(Node*, const String& markerType, unsigned index, ExceptionState&);
    void addTextMatchMarker(const Range*, bool isActive);
    void setMarkersActive(Node*, unsigned startOffset, unsigned endOffset, bool);
    void setMarkedTextMatchesAreHighlighted(Document*, bool);

    void setFrameViewPosition(Document*, long x, long y, ExceptionState&);
    String viewportAsText(Document*, float devicePixelRatio, int availableWidth, int availableHeight, ExceptionState&);

    bool wasLastChangeUserEdit(Element* textField, ExceptionState&);
    bool elementShouldAutoComplete(Element* inputElement, ExceptionState&);
    String suggestedValue(Element*, ExceptionState&);
    void setSuggestedValue(Element*, const String&, ExceptionState&);
    void setEditingValue(Element* inputElement, const String&, ExceptionState&);
    void setAutofilled(Element*, bool enabled, ExceptionState&);
    void scrollElementToRect(Element*, long x, long y, long w, long h, ExceptionState&);

    PassRefPtrWillBeRawPtr<Range> rangeFromLocationAndLength(Element* scope, int rangeLocation, int rangeLength);
    unsigned locationFromRange(Element* scope, const Range*);
    unsigned lengthFromRange(Element* scope, const Range*);
    String rangeAsText(const Range*);

    DOMPoint* touchPositionAdjustedToBestClickableNode(long x, long y, long width, long height, Document*, ExceptionState&);
    Node* touchNodeAdjustedToBestClickableNode(long x, long y, long width, long height, Document*, ExceptionState&);
    DOMPoint* touchPositionAdjustedToBestContextMenuNode(long x, long y, long width, long height, Document*, ExceptionState&);
    Node* touchNodeAdjustedToBestContextMenuNode(long x, long y, long width, long height, Document*, ExceptionState&);
    ClientRect* bestZoomableAreaForTouchPoint(long x, long y, long width, long height, Document*, ExceptionState&);

    int lastSpellCheckRequestSequence(Document*, ExceptionState&);
    int lastSpellCheckProcessedSequence(Document*, ExceptionState&);

    Vector<AtomicString> userPreferredLanguages() const;
    void setUserPreferredLanguages(const Vector<String>&);

    unsigned activeDOMObjectCount(Document*);
    unsigned wheelEventHandlerCount(Document*);
    unsigned scrollEventHandlerCount(Document*);
    unsigned touchEventHandlerCount(Document*);
    LayerRectList* touchEventTargetLayerRects(Document*, ExceptionState&);

    bool executeCommand(Document*, const String& name, const String& value, ExceptionState&);

    AtomicString htmlNamespace();
    Vector<AtomicString> htmlTags();
    AtomicString svgNamespace();
    Vector<AtomicString> svgTags();

    // This is used to test rect based hit testing like what's done on touch screens.
    PassRefPtrWillBeRawPtr<StaticNodeList> nodesFromRect(Document*, int x, int y, unsigned topPadding, unsigned rightPadding,
        unsigned bottomPadding, unsigned leftPadding, bool ignoreClipping, bool allowChildFrameContent, ExceptionState&) const;

    bool hasSpellingMarker(Document*, int from, int length);
    bool hasGrammarMarker(Document*, int from, int length);
    void setContinuousSpellCheckingEnabled(bool);

    bool isOverwriteModeEnabled(Document*);
    void toggleOverwriteModeEnabled(Document*);

    unsigned numberOfScrollableAreas(Document*);

    bool isPageBoxVisible(Document*, int pageNumber);

    static const char* internalsId;

    InternalSettings* settings() const;
    InternalRuntimeFlags* runtimeFlags() const;
    unsigned workerThreadCount() const;

    void setDeviceProximity(Document*, const String& eventType, double value, double min, double max, ExceptionState&);

    String layerTreeAsText(Document*, unsigned flags, ExceptionState&) const;
    String layerTreeAsText(Document*, ExceptionState&) const;
    String elementLayerTreeAsText(Element*, unsigned flags, ExceptionState&) const;
    String elementLayerTreeAsText(Element*, ExceptionState&) const;

    bool scrollsWithRespectTo(Element*, Element*, ExceptionState&);

    String scrollingStateTreeAsText(Document*) const;
    String mainThreadScrollingReasons(Document*, ExceptionState&) const;
    ClientRectList* nonFastScrollableRects(Document*, ExceptionState&) const;

    void garbageCollectDocumentResources(Document*) const;
    void evictAllResources() const;

    unsigned numberOfLiveNodes() const;
    unsigned numberOfLiveDocuments() const;
    String dumpRefCountedInstanceCounts() const;
    Vector<String> consoleMessageArgumentCounts(Document*) const;
    PassRefPtrWillBeRawPtr<LocalDOMWindow> openDummyInspectorFrontend(const String& url);
    void closeDummyInspectorFrontend();
    Vector<unsigned long> setMemoryCacheCapacities(unsigned long minDeadBytes, unsigned long maxDeadBytes, unsigned long totalBytes);

    String counterValue(Element*);

    int pageNumber(Element*, float pageWidth = 800, float pageHeight = 600);
    Vector<String> shortcutIconURLs(Document*) const;
    Vector<String> allIconURLs(Document*) const;

    int numberOfPages(float pageWidthInPixels = 800, float pageHeightInPixels = 600);
    String pageProperty(String, int, ExceptionState& = ASSERT_NO_EXCEPTION) const;
    String pageSizeAndMarginsInPixels(int, int, int, int, int, int, int, ExceptionState& = ASSERT_NO_EXCEPTION) const;

    void setDeviceScaleFactor(float scaleFactor, ExceptionState&);
    void setPageScaleFactor(float scaleFactor, ExceptionState&);
    void setPageScaleFactorLimits(float minScaleFactor, float maxScaleFactor, ExceptionState&);

    void setIsCursorVisible(Document*, bool, ExceptionState&);

    double effectiveMediaVolume(HTMLMediaElement*);

    void mediaPlayerRemoteRouteAvailabilityChanged(HTMLMediaElement*, bool);
    void mediaPlayerPlayingRemotelyChanged(HTMLMediaElement*, bool);

    void registerURLSchemeAsBypassingContentSecurityPolicy(const String& scheme);
    void registerURLSchemeAsBypassingContentSecurityPolicy(const String& scheme, const Vector<String>& policyAreas);
    void removeURLSchemeRegisteredAsBypassingContentSecurityPolicy(const String& scheme);

    TypeConversions* typeConversions() const;
    PrivateScriptTest* privateScriptTest() const;
    DictionaryTest* dictionaryTest() const;
    UnionTypesTest* unionTypesTest() const;

    Vector<String> getReferencedFilePaths() const;

    void startTrackingRepaints(Document*, ExceptionState&);
    void stopTrackingRepaints(Document*, ExceptionState&);
    void updateLayoutIgnorePendingStylesheetsAndRunPostLayoutTasks(Node*, ExceptionState&);
    void forceFullRepaint(Document*, ExceptionState&);

    ClientRectList* draggableRegions(Document*, ExceptionState&);
    ClientRectList* nonDraggableRegions(Document*, ExceptionState&);

    PassRefPtr<DOMArrayBuffer> serializeObject(PassRefPtr<SerializedScriptValue>) const;
    PassRefPtr<SerializedScriptValue> deserializeBuffer(PassRefPtr<DOMArrayBuffer>) const;

    String getCurrentCursorInfo();

    bool cursorUpdatePending() const;

    String markerTextForListItem(Element*);

    void forceReload(bool endToEnd);

    String getImageSourceURL(Element*);

    String selectMenuListText(HTMLSelectElement*);
    bool isSelectPopupVisible(Node*);
    bool selectPopupItemStyleIsRtl(Node*, int);
    int selectPopupItemStyleFontHeight(Node*, int);

    ClientRect* selectionBounds(ExceptionState&);

    bool loseSharedGraphicsContext3D();

    void forceCompositingUpdate(Document*, ExceptionState&);

    void setZoomFactor(float);

    void setShouldRevealPassword(Element*, bool, ExceptionState&);

    ScriptPromise createResolvedPromise(ScriptState*, ScriptValue);
    ScriptPromise createRejectedPromise(ScriptState*, ScriptValue);
    ScriptPromise addOneToPromise(ScriptState*, ScriptPromise);
    ScriptPromise promiseCheck(ScriptState*, long, bool, const Dictionary&, const String&, const Vector<String>&, ExceptionState&);
    ScriptPromise promiseCheckWithoutExceptionState(ScriptState*, const Dictionary&, const String&, const Vector<String>&);
    ScriptPromise promiseCheckRange(ScriptState*, long);
    ScriptPromise promiseCheckOverload(ScriptState*, Location*);
    ScriptPromise promiseCheckOverload(ScriptState*, Document*);
    ScriptPromise promiseCheckOverload(ScriptState*, Location*, long, long);

    DECLARE_TRACE();

    void setValueForUser(Element*, const String&);

    String textSurroundingNode(Node*, int x, int y, unsigned long maxLength);

    void setFocused(bool);
    void setInitialFocus(bool);

    bool ignoreLayoutWithPendingStylesheets(Document*);

    void setNetworkStateNotifierTestOnly(bool);
    // Test must call setNetworkStateNotifierTestOnly(true) before calling setNetworkConnectionInfo.
    void setNetworkConnectionInfo(const String&, ExceptionState&);

    ClientRect* boundsInViewportSpace(Element*);

    unsigned countHitRegions(CanvasRenderingContext*);

    bool isInCanvasFontCache(Document*, const String&);
    unsigned canvasFontCacheMaxFonts();

    void forcePluginPlaceholder(HTMLElement* plugin, PassRefPtrWillBeRawPtr<DocumentFragment>, ExceptionState&);
    void forcePluginPlaceholder(HTMLElement* plugin, const PluginPlaceholderOptions&, ExceptionState&);

    // Scheudle a forced Blink GC run (Oilpan) at the end of event loop.
    // Note: This is designed to be only used from PerformanceTests/BlinkGC to explicitly measure only Blink GC time.
    //       Normal LayoutTests should use gc() instead as it would trigger both Blink GC and V8 GC.
    void forceBlinkGCWithoutV8GC();

    String selectedHTMLForClipboard();
    String selectedTextForClipboard();

    void setVisualViewportOffset(int x, int y);

    // Return true if the given use counter exists for the given document.
    // |useCounterId| must be one of the values from the UseCounter::Feature enum.
    bool isUseCounted(Document*, int useCounterId);

    String unscopeableAttribute();
    String unscopeableMethod();

    ClientRectList* focusRingRects(Element*);

    void setCapsLockState(bool enabled);

private:
    explicit Internals(ScriptState*);
    Document* contextDocument() const;
    LocalFrame* frame() const;
    Vector<String> iconURLs(Document*, int iconTypesMask) const;
    ClientRectList* annotatedRegions(Document*, bool draggable, ExceptionState&);

    DocumentMarker* markerAt(Node*, const String& markerType, unsigned index, ExceptionState&);
    Member<InternalRuntimeFlags> m_runtimeFlags;

    IterationSource* startIteration(ScriptState*, ExceptionState&) override;
};

} // namespace blink

#endif // Internals_h

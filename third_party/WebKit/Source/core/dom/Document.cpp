/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 *           (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2011, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2008, 2009, 2011, 2012 Google Inc. All rights reserved.
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) Research In Motion Limited 2010-2011. All rights reserved.
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
#include "core/dom/Document.h"

#include "bindings/core/v8/CustomElementConstructorBuilder.h"
#include "bindings/core/v8/DOMDataStore.h"
#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/UnionTypesCore.h"
#include "bindings/core/v8/V8DOMWrapper.h"
#include "bindings/core/v8/V8PerIsolateData.h"
#include "bindings/core/v8/WindowProxy.h"
#include "core/HTMLElementFactory.h"
#include "core/HTMLNames.h"
#include "core/SVGElementFactory.h"
#include "core/SVGNames.h"
#include "core/XMLNSNames.h"
#include "core/XMLNames.h"
#include "core/animation/AnimationTimeline.h"
#include "core/animation/DocumentAnimations.h"
#include "core/css/CSSFontSelector.h"
#include "core/css/CSSStyleDeclaration.h"
#include "core/css/CSSStyleSheet.h"
#include "core/css/MediaQueryMatcher.h"
#include "core/css/StylePropertySet.h"
#include "core/css/StyleSheetContents.h"
#include "core/css/StyleSheetList.h"
#include "core/css/invalidation/StyleInvalidator.h"
#include "core/css/parser/CSSParser.h"
#include "core/css/resolver/FontBuilder.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/css/resolver/StyleResolverStats.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/AddConsoleMessageTask.h"
#include "core/dom/Attr.h"
#include "core/dom/CDATASection.h"
#include "core/dom/ClientRect.h"
#include "core/dom/Comment.h"
#include "core/dom/ContextFeatures.h"
#include "core/dom/DOMImplementation.h"
#include "core/dom/DocumentFragment.h"
#include "core/dom/DocumentLifecycleObserver.h"
#include "core/dom/DocumentMarkerController.h"
#include "core/dom/DocumentType.h"
#include "core/dom/Element.h"
#include "core/dom/ElementDataCache.h"
#include "core/dom/ElementRegistrationOptions.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContextTask.h"
#include "core/dom/FrameRequestCallback.h"
#include "core/dom/LayoutTreeBuilderTraversal.h"
#include "core/dom/MainThreadTaskRunner.h"
#include "core/dom/Microtask.h"
#include "core/dom/MutationObserver.h"
#include "core/dom/NodeChildRemovalTracker.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/dom/NodeFilter.h"
#include "core/dom/NodeIterator.h"
#include "core/dom/NodeRareData.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/NodeWithIndex.h"
#include "core/dom/NthIndexCache.h"
#include "core/dom/ProcessingInstruction.h"
#include "core/dom/ScriptRunner.h"
#include "core/dom/ScriptedAnimationController.h"
#include "core/dom/SelectorQuery.h"
#include "core/dom/StaticNodeList.h"
#include "core/dom/StyleEngine.h"
#include "core/dom/TouchList.h"
#include "core/dom/TransformSource.h"
#include "core/dom/TreeWalker.h"
#include "core/dom/VisitedLinkState.h"
#include "core/dom/XMLDocument.h"
#include "core/dom/custom/CustomElementMicrotaskRunQueue.h"
#include "core/dom/custom/CustomElementRegistrationContext.h"
#include "core/dom/shadow/ComposedTreeTraversal.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/Editor.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/SpellChecker.h"
#include "core/editing/markup.h"
#include "core/events/BeforeUnloadEvent.h"
#include "core/events/Event.h"
#include "core/events/EventFactory.h"
#include "core/events/EventListener.h"
#include "core/events/HashChangeEvent.h"
#include "core/events/PageTransitionEvent.h"
#include "core/events/ScopedEventQueue.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/frame/DOMTimer.h"
#include "core/frame/EventHandlerRegistry.h"
#include "core/frame/FrameConsole.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/History.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/OriginsUsingFeatures.h"
#include "core/frame/PinchViewport.h"
#include "core/frame/Settings.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/html/DocumentNameCollection.h"
#include "core/html/HTMLAllCollection.h"
#include "core/html/HTMLAnchorElement.h"
#include "core/html/HTMLBaseElement.h"
#include "core/html/HTMLBodyElement.h"
#include "core/html/HTMLCanvasElement.h"
#include "core/html/HTMLCollection.h"
#include "core/html/HTMLDialogElement.h"
#include "core/html/HTMLDocument.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/html/HTMLHeadElement.h"
#include "core/html/HTMLHtmlElement.h"
#include "core/html/HTMLIFrameElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLLinkElement.h"
#include "core/html/HTMLMetaElement.h"
#include "core/html/HTMLScriptElement.h"
#include "core/html/HTMLStyleElement.h"
#include "core/html/HTMLTemplateElement.h"
#include "core/html/HTMLTitleElement.h"
#include "core/html/PluginDocument.h"
#include "core/html/WindowNameCollection.h"
#include "core/html/canvas/CanvasContextCreationAttributes.h"
#include "core/html/canvas/CanvasFontCache.h"
#include "core/html/canvas/CanvasRenderingContext.h"
#include "core/html/forms/FormController.h"
#include "core/html/imports/HTMLImportLoader.h"
#include "core/html/imports/HTMLImportsController.h"
#include "core/html/parser/HTMLDocumentParser.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/html/parser/NestingLevelIncrementer.h"
#include "core/html/parser/TextResourceDecoder.h"
#include "core/input/EventHandler.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/inspector/InstanceCounters.h"
#include "core/inspector/ScriptCallStack.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutPart.h"
#include "core/layout/LayoutView.h"
#include "core/layout/TextAutosizer.h"
#include "core/layout/compositing/DeprecatedPaintLayerCompositor.h"
#include "core/loader/CookieJar.h"
#include "core/loader/DocumentLoader.h"
#include "core/loader/FrameFetchContext.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/loader/ImageLoader.h"
#include "core/loader/appcache/ApplicationCacheHost.h"
#include "core/page/ChromeClient.h"
#include "core/page/EventWithHitTestResults.h"
#include "core/page/FocusController.h"
#include "core/page/FrameTree.h"
#include "core/page/Page.h"
#include "core/page/PointerLockController.h"
#include "core/page/scrolling/ScrollingCoordinator.h"
#include "core/svg/SVGDocumentExtensions.h"
#include "core/svg/SVGTitleElement.h"
#include "core/svg/SVGUseElement.h"
#include "core/workers/SharedWorkerRepositoryClient.h"
#include "core/xml/parser/XMLDocumentParser.h"
#include "platform/DateComponents.h"
#include "platform/EventDispatchForbiddenScope.h"
#include "platform/Language.h"
#include "platform/LengthFunctions.h"
#include "platform/Logging.h"
#include "platform/PluginScriptForbiddenScope.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/ScriptForbiddenScope.h"
#include "platform/TraceEvent.h"
#include "platform/network/ContentSecurityPolicyParsers.h"
#include "platform/network/HTTPParsers.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "platform/text/PlatformLocale.h"
#include "platform/text/SegmentedString.h"
#include "platform/weborigin/OriginAccessEntry.h"
#include "platform/weborigin/SchemeRegistry.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/Platform.h"
#include "wtf/CurrentTime.h"
#include "wtf/DateMath.h"
#include "wtf/HashFunctions.h"
#include "wtf/MainThread.h"
#include "wtf/StdLibExtras.h"
#include "wtf/TemporaryChange.h"
#include "wtf/text/StringBuffer.h"
#include "wtf/text/TextEncodingRegistry.h"
#include "wtf/RefCountedLeakCounter.h"

using namespace WTF;
using namespace Unicode;

namespace blink {

using namespace HTMLNames;

static const unsigned cMaxWriteRecursionDepth = 21;

// This amount of time must have elapsed before we will even consider scheduling a layout without a delay.
// FIXME: For faster machines this value can really be lowered to 200.  250 is adequate, but a little high
// for dual G5s. :)
static const int cLayoutScheduleThreshold = 250;

// DOM Level 2 says (letters added):
//
// a) Name start characters must have one of the categories Ll, Lu, Lo, Lt, Nl.
// b) Name characters other than Name-start characters must have one of the categories Mc, Me, Mn, Lm, or Nd.
// c) Characters in the compatibility area (i.e. with character code greater than #xF900 and less than #xFFFE) are not allowed in XML names.
// d) Characters which have a font or compatibility decomposition (i.e. those with a "compatibility formatting tag" in field 5 of the database -- marked by field 5 beginning with a "<") are not allowed.
// e) The following characters are treated as name-start characters rather than name characters, because the property file classifies them as Alphabetic: [#x02BB-#x02C1], #x0559, #x06E5, #x06E6.
// f) Characters #x20DD-#x20E0 are excluded (in accordance with Unicode, section 5.14).
// g) Character #x00B7 is classified as an extender, because the property list so identifies it.
// h) Character #x0387 is added as a name character, because #x00B7 is its canonical equivalent.
// i) Characters ':' and '_' are allowed as name-start characters.
// j) Characters '-' and '.' are allowed as name characters.
//
// It also contains complete tables. If we decide it's better, we could include those instead of the following code.

static inline bool isValidNameStart(UChar32 c)
{
    // rule (e) above
    if ((c >= 0x02BB && c <= 0x02C1) || c == 0x559 || c == 0x6E5 || c == 0x6E6)
        return true;

    // rule (i) above
    if (c == ':' || c == '_')
        return true;

    // rules (a) and (f) above
    const uint32_t nameStartMask = Letter_Lowercase | Letter_Uppercase | Letter_Other | Letter_Titlecase | Number_Letter;
    if (!(Unicode::category(c) & nameStartMask))
        return false;

    // rule (c) above
    if (c >= 0xF900 && c < 0xFFFE)
        return false;

    // rule (d) above
    DecompositionType decompType = decompositionType(c);
    if (decompType == DecompositionFont || decompType == DecompositionCompat)
        return false;

    return true;
}

static inline bool isValidNamePart(UChar32 c)
{
    // rules (a), (e), and (i) above
    if (isValidNameStart(c))
        return true;

    // rules (g) and (h) above
    if (c == 0x00B7 || c == 0x0387)
        return true;

    // rule (j) above
    if (c == '-' || c == '.')
        return true;

    // rules (b) and (f) above
    const uint32_t otherNamePartMask = Mark_NonSpacing | Mark_Enclosing | Mark_SpacingCombining | Letter_Modifier | Number_DecimalDigit;
    if (!(Unicode::category(c) & otherNamePartMask))
        return false;

    // rule (c) above
    if (c >= 0xF900 && c < 0xFFFE)
        return false;

    // rule (d) above
    DecompositionType decompType = decompositionType(c);
    if (decompType == DecompositionFont || decompType == DecompositionCompat)
        return false;

    return true;
}

static bool shouldInheritSecurityOriginFromOwner(const KURL& url)
{
    // http://www.whatwg.org/specs/web-apps/current-work/#origin-0
    //
    // If a Document has the address "about:blank"
    //     The origin of the Document is the origin it was assigned when its browsing context was created.
    //
    // Note: We generalize this to all "blank" URLs and invalid URLs because we
    // treat all of these URLs as about:blank.
    //
    return url.isEmpty() || url.protocolIsAbout();
}

static Widget* widgetForElement(const Element& focusedElement)
{
    LayoutObject* layoutObject = focusedElement.layoutObject();
    if (!layoutObject || !layoutObject->isLayoutPart())
        return 0;
    return toLayoutPart(layoutObject)->widget();
}

static bool acceptsEditingFocus(const Element& element)
{
    ASSERT(element.hasEditableStyle());

    return element.document().frame() && element.rootEditableElement();
}

uint64_t Document::s_globalTreeVersion = 0;

#ifndef NDEBUG
using WeakDocumentSet = WillBeHeapHashSet<RawPtrWillBeWeakMember<Document>>;
static WeakDocumentSet& liveDocumentSet()
{
    DEFINE_STATIC_LOCAL(OwnPtrWillBePersistent<WeakDocumentSet>, set, (adoptPtrWillBeNoop(new WeakDocumentSet())));
    return *set;
}
#endif

// This class doesn't work with non-Document ExecutionContext.
class AutofocusTask final : public ExecutionContextTask {
public:
    static PassOwnPtr<AutofocusTask> create()
    {
        return adoptPtr(new AutofocusTask());
    }
    ~AutofocusTask() override { }

private:
    AutofocusTask() { }
    void performTask(ExecutionContext* context) override
    {
        Document* document = toDocument(context);
        if (RefPtrWillBeRawPtr<Element> element = document->autofocusElement()) {
            document->setAutofocusElement(0);
            element->focus();
        }
    }
};

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, documentCounter, ("documentCounter"));
#endif

Document::Document(const DocumentInit& initializer, DocumentClassFlags documentClasses)
    : ContainerNode(0, CreateDocument)
    , TreeScope(*this)
    , m_hasNodesWithPlaceholderStyle(false)
    , m_evaluateMediaQueriesOnStyleRecalc(false)
    , m_pendingSheetLayout(NoLayoutWithPendingSheets)
    , m_frame(initializer.frame())
    , m_domWindow(m_frame ? m_frame->localDOMWindow() : 0)
    , m_importsController(initializer.importsController())
    , m_activeParserCount(0)
    , m_contextFeatures(ContextFeatures::defaultSwitch())
    , m_wellFormed(false)
    , m_printing(false)
    , m_paginatedForScreen(false)
    , m_compatibilityMode(NoQuirksMode)
    , m_compatibilityModeLocked(false)
    , m_executeScriptsWaitingForResourcesTimer(this, &Document::executeScriptsWaitingForResourcesTimerFired)
    , m_hasAutofocused(false)
    , m_clearFocusedElementTimer(this, &Document::clearFocusedElementTimerFired)
    , m_domTreeVersion(++s_globalTreeVersion)
    , m_styleVersion(0)
    , m_listenerTypes(0)
    , m_mutationObserverTypes(0)
    , m_visitedLinkState(VisitedLinkState::create(*this))
    , m_visuallyOrdered(false)
    , m_readyState(Complete)
    , m_parsingState(FinishedParsing)
    , m_gotoAnchorNeededAfterStylesheetsLoad(false)
    , m_containsValidityStyleRules(false)
    , m_updateFocusAppearanceRestoresSelection(false)
    , m_containsPlugins(false)
    , m_ignoreDestructiveWriteCount(0)
    , m_markers(adoptPtrWillBeNoop(new DocumentMarkerController))
    , m_updateFocusAppearanceTimer(this, &Document::updateFocusAppearanceTimerFired)
    , m_cssTarget(nullptr)
    , m_loadEventProgress(LoadEventNotRun)
    , m_startTime(currentTime())
    , m_scriptRunner(ScriptRunner::create(this))
    , m_xmlVersion("1.0")
    , m_xmlStandalone(StandaloneUnspecified)
    , m_hasXMLDeclaration(0)
    , m_designMode(false)
    , m_hasAnnotatedRegions(false)
    , m_annotatedRegionsDirty(false)
    , m_useSecureKeyboardEntryWhenActive(false)
    , m_documentClasses(documentClasses)
    , m_isViewSource(false)
    , m_sawElementsInKnownNamespaces(false)
    , m_isSrcdocDocument(false)
    , m_isMobileDocument(false)
    , m_layoutView(0)
#if !ENABLE(OILPAN)
    , m_weakFactory(this)
#endif
    , m_contextDocument(initializer.contextDocument())
    , m_hasFullscreenSupplement(false)
    , m_loadEventDelayCount(0)
    , m_loadEventDelayTimer(this, &Document::loadEventDelayTimerFired)
    , m_pluginLoadingTimer(this, &Document::pluginLoadingTimerFired)
    , m_writeRecursionIsTooDeep(false)
    , m_writeRecursionDepth(0)
    , m_taskRunner(MainThreadTaskRunner::create(this))
    , m_registrationContext(initializer.registrationContext(this))
    , m_elementDataCacheClearTimer(this, &Document::elementDataCacheClearTimerFired)
    , m_timeline(AnimationTimeline::create(this))
    , m_templateDocumentHost(nullptr)
    , m_didAssociateFormControlsTimer(this, &Document::didAssociateFormControlsTimerFired)
    , m_hasViewportUnits(false)
    , m_styleRecalcElementCounter(0)
    , m_parserSyncPolicy(AllowAsynchronousParsing)
{
    if (m_frame) {
        ASSERT(m_frame->page());
        provideContextFeaturesToDocumentFrom(*this, *m_frame->page());

        m_fetcher = m_frame->loader().documentLoader()->fetcher();
        FrameFetchContext::provideDocumentToContext(m_fetcher->context(), this);
    } else if (m_importsController) {
        m_fetcher = FrameFetchContext::createContextAndFetcher(nullptr);
        FrameFetchContext::provideDocumentToContext(m_fetcher->context(), this);
    } else {
        m_fetcher = ResourceFetcher::create(nullptr);
    }

    // We depend on the url getting immediately set in subframes, but we
    // also depend on the url NOT getting immediately set in opened windows.
    // See fast/dom/early-frame-url.html
    // and fast/dom/location-new-window-no-crash.html, respectively.
    // FIXME: Can/should we unify this behavior?
    if (initializer.shouldSetURL())
        setURL(initializer.url());

    initSecurityContext(initializer);
    initDNSPrefetch();

#if !ENABLE(OILPAN)
    for (unsigned i = 0; i < WTF_ARRAY_LENGTH(m_nodeListCounts); ++i)
        m_nodeListCounts[i] = 0;
#endif

    InstanceCounters::incrementCounter(InstanceCounters::DocumentCounter);

    m_lifecycle.advanceTo(DocumentLifecycle::Inactive);

    // Since CSSFontSelector requires Document::m_fetcher and StyleEngine owns
    // CSSFontSelector, need to initialize m_styleEngine after initializing
    // m_fetcher.
    m_styleEngine = StyleEngine::create(*this);

    // The parent's parser should be suspended together with all the other objects,
    // else this new Document would have a new ExecutionContext which suspended state
    // would not match the one from the parent, and could start loading resources
    // ignoring the defersLoading flag.
    ASSERT(!parentDocument() || !parentDocument()->activeDOMObjectsAreSuspended());

#ifndef NDEBUG
    liveDocumentSet().add(this);
    documentCounter.increment();
#endif
}

Document::~Document()
{
    ASSERT(!layoutView());
    ASSERT(!parentTreeScope());
    // If a top document with a cache, verify that it was comprehensively
    // cleared during detach.
    ASSERT(!m_axObjectCache);
#if !ENABLE(OILPAN)
    ASSERT(m_ranges.isEmpty());
    ASSERT(!hasGuardRefCount());
    ASSERT(!m_importsController);
    // With Oilpan, either the document outlives the visibility observers
    // or the visibility observers and the document die in the same GC round.
    // When they die in the same GC round, the list of visibility observers
    // will not be empty on Document destruction.
    ASSERT(m_visibilityObservers.isEmpty());

    if (m_templateDocument)
        m_templateDocument->m_templateDocumentHost = nullptr; // balanced in ensureTemplateDocument().

    m_scriptRunner.clear();

    // FIXME: Oilpan: Not removing event listeners here also means that we do
    // not notify the inspector instrumentation that the event listeners are
    // gone. The Document and all the nodes in the document are gone, so maybe
    // that is OK?
    removeAllEventListenersRecursively();

    // Currently we believe that Document can never outlive the parser.
    // Although the Document may be replaced synchronously, DocumentParsers
    // generally keep at least one reference to an Element which would in turn
    // has a reference to the Document.  If you hit this ASSERT, then that
    // assumption is wrong.  DocumentParser::detach() should ensure that even
    // if the DocumentParser outlives the Document it won't cause badness.
    ASSERT(!m_parser || m_parser->refCount() == 1);
    detachParser();

    if (m_styleSheetList)
        m_styleSheetList->detachFromDocument();

    m_timeline->detachFromDocument();

    // We need to destroy CSSFontSelector before destroying m_fetcher.
    m_styleEngine->detachFromDocument();

    if (m_elemSheet)
        m_elemSheet->clearOwnerNode();

    // We must call clearRareData() here since a Document class inherits TreeScope
    // as well as Node. See a comment on TreeScope.h for the reason.
    if (hasRareData())
        clearRareData();

    ASSERT(m_listsInvalidatedAtDocument.isEmpty());

    for (unsigned i = 0; i < WTF_ARRAY_LENGTH(m_nodeListCounts); ++i)
        ASSERT(!m_nodeListCounts[i]);

#ifndef NDEBUG
    liveDocumentSet().remove(this);
#endif
#endif // !ENABLE(OILPAN)

#ifndef NDEBUG
    documentCounter.decrement();
#endif
    InstanceCounters::decrementCounter(InstanceCounters::DocumentCounter);
}

#if !ENABLE(OILPAN)
void Document::dispose()
{
    ASSERT_WITH_SECURITY_IMPLICATION(!m_deletionHasBegun);

    // We must make sure not to be retaining any of our children through
    // these extra pointers or we will create a reference cycle.
    m_docType = nullptr;
    m_focusedElement = nullptr;
    m_hoverNode = nullptr;
    m_activeHoverElement = nullptr;
    m_titleElement = nullptr;
    m_documentElement = nullptr;
    m_contextFeatures = ContextFeatures::defaultSwitch();
    m_userActionElements.documentDidRemoveLastRef();
    m_associatedFormControls.clear();

    detachParser();

    m_registrationContext.clear();

    // removeDetachedChildren() doesn't always unregister IDs,
    // so tear down scope information upfront to avoid having stale references in the map.
    destroyTreeScopeData();

    removeDetachedChildren();

    // removeDetachedChildren() can access FormController.
    m_formController.clear();

    m_markers->clear();

    m_cssCanvasElements.clear();

    // FIXME: consider using ActiveDOMObject.
    if (m_scriptedAnimationController)
        m_scriptedAnimationController->clearDocumentPointer();
    m_scriptedAnimationController.clear();

    if (svgExtensions())
        accessSVGExtensions().pauseAnimations();

    m_lifecycle.advanceTo(DocumentLifecycle::Disposed);
    DocumentLifecycleNotifier::notifyDocumentWasDisposed();

    m_canvasFontCache.clear();
}
#endif

SelectorQueryCache& Document::selectorQueryCache()
{
    if (!m_selectorQueryCache)
        m_selectorQueryCache = adoptPtr(new SelectorQueryCache());
    return *m_selectorQueryCache;
}

MediaQueryMatcher& Document::mediaQueryMatcher()
{
    if (!m_mediaQueryMatcher)
        m_mediaQueryMatcher = MediaQueryMatcher::create(*this);
    return *m_mediaQueryMatcher;
}

void Document::mediaQueryAffectingValueChanged()
{
    m_evaluateMediaQueriesOnStyleRecalc = true;
    styleEngine().clearMediaQueryRuleSetStyleSheets();
    InspectorInstrumentation::mediaQueryResultChanged(this);
}

void Document::setCompatibilityMode(CompatibilityMode mode)
{
    if (m_compatibilityModeLocked || mode == m_compatibilityMode)
        return;
    m_compatibilityMode = mode;
    selectorQueryCache().invalidate();
}

String Document::compatMode() const
{
    return inQuirksMode() ? "BackCompat" : "CSS1Compat";
}

void Document::setDoctype(PassRefPtrWillBeRawPtr<DocumentType> docType)
{
    // This should never be called more than once.
    ASSERT(!m_docType || !docType);
    m_docType = docType;
    if (m_docType) {
        this->adoptIfNeeded(*m_docType);
        if (m_docType->publicId().startsWith("-//wapforum//dtd xhtml mobile 1.", TextCaseInsensitive))
            m_isMobileDocument = true;
    }
    // Doctype affects the interpretation of the stylesheets.
    styleEngine().clearResolver();
}

DOMImplementation& Document::implementation()
{
    if (!m_implementation)
        m_implementation = DOMImplementation::create(*this);
    return *m_implementation;
}

bool Document::hasAppCacheManifest() const
{
    return isHTMLHtmlElement(documentElement()) && documentElement()->hasAttribute(manifestAttr);
}

Location* Document::location() const
{
    if (!frame())
        return 0;

    return domWindow()->location();
}

void Document::childrenChanged(const ChildrenChange& change)
{
    ContainerNode::childrenChanged(change);
    m_documentElement = ElementTraversal::firstWithin(*this);
}

AtomicString Document::convertLocalName(const AtomicString& name)
{
    return isHTMLDocument() ? name.lower() : name;
}

PassRefPtrWillBeRawPtr<Element> Document::createElement(const AtomicString& name, ExceptionState& exceptionState)
{
    if (!isValidName(name)) {
        exceptionState.throwDOMException(InvalidCharacterError, "The tag name provided ('" + name + "') is not a valid name.");
        return nullptr;
    }

    if (isXHTMLDocument() || isHTMLDocument())
        return HTMLElementFactory::createHTMLElement(convertLocalName(name), *this, 0, false);

    return Element::create(QualifiedName(nullAtom, name, nullAtom), this);
}

PassRefPtrWillBeRawPtr<Element> Document::createElement(const AtomicString& localName, const AtomicString& typeExtension, ExceptionState& exceptionState)
{
    if (!isValidName(localName)) {
        exceptionState.throwDOMException(InvalidCharacterError, "The tag name provided ('" + localName + "') is not a valid name.");
        return nullptr;
    }

    RefPtrWillBeRawPtr<Element> element;

    if (CustomElement::isValidName(localName) && registrationContext()) {
        element = registrationContext()->createCustomTagElement(*this, QualifiedName(nullAtom, convertLocalName(localName), xhtmlNamespaceURI));
    } else {
        element = createElement(localName, exceptionState);
        if (exceptionState.hadException())
            return nullptr;
    }

    if (!typeExtension.isEmpty())
        CustomElementRegistrationContext::setIsAttributeAndTypeExtension(element.get(), typeExtension);

    return element.release();
}

static inline QualifiedName createQualifiedName(const AtomicString& namespaceURI, const AtomicString& qualifiedName, ExceptionState& exceptionState)
{
    AtomicString prefix, localName;
    if (!Document::parseQualifiedName(qualifiedName, prefix, localName, exceptionState))
        return QualifiedName::null();

    QualifiedName qName(prefix, localName, namespaceURI);
    if (!Document::hasValidNamespaceForElements(qName)) {
        exceptionState.throwDOMException(NamespaceError, "The namespace URI provided ('" + namespaceURI + "') is not valid for the qualified name provided ('" + qualifiedName + "').");
        return QualifiedName::null();
    }

    return qName;
}

PassRefPtrWillBeRawPtr<Element> Document::createElementNS(const AtomicString& namespaceURI, const AtomicString& qualifiedName, ExceptionState& exceptionState)
{
    QualifiedName qName(createQualifiedName(namespaceURI, qualifiedName, exceptionState));
    if (qName == QualifiedName::null())
        return nullptr;

    return createElement(qName, false);
}

PassRefPtrWillBeRawPtr<Element> Document::createElementNS(const AtomicString& namespaceURI, const AtomicString& qualifiedName, const AtomicString& typeExtension, ExceptionState& exceptionState)
{
    QualifiedName qName(createQualifiedName(namespaceURI, qualifiedName, exceptionState));
    if (qName == QualifiedName::null())
        return nullptr;

    RefPtrWillBeRawPtr<Element> element;
    if (CustomElement::isValidName(qName.localName()) && registrationContext())
        element = registrationContext()->createCustomTagElement(*this, qName);
    else
        element = createElement(qName, false);

    if (!typeExtension.isEmpty())
        CustomElementRegistrationContext::setIsAttributeAndTypeExtension(element.get(), typeExtension);

    return element.release();
}

ScriptValue Document::registerElement(ScriptState* scriptState, const AtomicString& name, const ElementRegistrationOptions& options, ExceptionState& exceptionState, CustomElement::NameSet validNames)
{
    OriginsUsingFeatures::count(scriptState, *this, OriginsUsingFeatures::Feature::DocumentRegisterElement);

    if (!registrationContext()) {
        exceptionState.throwDOMException(NotSupportedError, "No element registration context is available.");
        return ScriptValue();
    }

    CustomElementConstructorBuilder constructorBuilder(scriptState, options);
    registrationContext()->registerElement(this, &constructorBuilder, name, validNames, exceptionState);
    return constructorBuilder.bindingsReturnValue();
}

CustomElementMicrotaskRunQueue* Document::customElementMicrotaskRunQueue()
{
    if (!m_customElementMicrotaskRunQueue)
        m_customElementMicrotaskRunQueue = CustomElementMicrotaskRunQueue::create();
    return m_customElementMicrotaskRunQueue.get();
}

void Document::setImportsController(HTMLImportsController* controller)
{
    ASSERT(!m_importsController || !controller);
    m_importsController = controller;
    if (!m_importsController && !loader())
        m_fetcher->clearContext();
}

HTMLImportLoader* Document::importLoader() const
{
    if (!m_importsController)
        return 0;
    return m_importsController->loaderFor(*this);
}

bool Document::haveImportsLoaded() const
{
    if (!m_importsController)
        return true;
    return !m_importsController->shouldBlockScriptExecution(*this);
}

LocalDOMWindow* Document::executingWindow()
{
    if (LocalDOMWindow* owningWindow = domWindow())
        return owningWindow;
    if (HTMLImportsController* import = this->importsController())
        return import->master()->domWindow();
    return 0;
}

LocalFrame* Document::executingFrame()
{
    LocalDOMWindow* window = executingWindow();
    if (!window)
        return 0;
    return window->frame();
}

PassRefPtrWillBeRawPtr<DocumentFragment> Document::createDocumentFragment()
{
    return DocumentFragment::create(*this);
}

PassRefPtrWillBeRawPtr<Text> Document::createTextNode(const String& data)
{
    return Text::create(*this, data);
}

PassRefPtrWillBeRawPtr<Comment> Document::createComment(const String& data)
{
    return Comment::create(*this, data);
}

PassRefPtrWillBeRawPtr<CDATASection> Document::createCDATASection(const String& data, ExceptionState& exceptionState)
{
    if (isHTMLDocument()) {
        exceptionState.throwDOMException(NotSupportedError, "This operation is not supported for HTML documents.");
        return nullptr;
    }
    if (data.contains("]]>")) {
        exceptionState.throwDOMException(InvalidCharacterError, "String cannot contain ']]>' since that is the end delimiter of a CData section.");
        return nullptr;
    }
    return CDATASection::create(*this, data);
}

PassRefPtrWillBeRawPtr<ProcessingInstruction> Document::createProcessingInstruction(const String& target, const String& data, ExceptionState& exceptionState)
{
    if (!isValidName(target)) {
        exceptionState.throwDOMException(InvalidCharacterError, "The target provided ('" + target + "') is not a valid name.");
        return nullptr;
    }
    if (data.contains("?>")) {
        exceptionState.throwDOMException(InvalidCharacterError, "The data provided ('" + data + "') contains '?>'.");
        return nullptr;
    }
    return ProcessingInstruction::create(*this, target, data);
}

PassRefPtrWillBeRawPtr<Text> Document::createEditingTextNode(const String& text)
{
    return Text::createEditingText(*this, text);
}

bool Document::importContainerNodeChildren(ContainerNode* oldContainerNode, PassRefPtrWillBeRawPtr<ContainerNode> newContainerNode, ExceptionState& exceptionState)
{
    for (Node& oldChild : NodeTraversal::childrenOf(*oldContainerNode)) {
        RefPtrWillBeRawPtr<Node> newChild = importNode(&oldChild, true, exceptionState);
        if (exceptionState.hadException())
            return false;
        newContainerNode->appendChild(newChild.release(), exceptionState);
        if (exceptionState.hadException())
            return false;
    }

    return true;
}

PassRefPtrWillBeRawPtr<Node> Document::importNode(Node* importedNode, bool deep, ExceptionState& exceptionState)
{
    switch (importedNode->nodeType()) {
    case TEXT_NODE:
        return createTextNode(importedNode->nodeValue());
    case CDATA_SECTION_NODE:
        return CDATASection::create(*this, importedNode->nodeValue());
    case PROCESSING_INSTRUCTION_NODE:
        return createProcessingInstruction(importedNode->nodeName(), importedNode->nodeValue(), exceptionState);
    case COMMENT_NODE:
        return createComment(importedNode->nodeValue());
    case DOCUMENT_TYPE_NODE: {
        DocumentType* doctype = toDocumentType(importedNode);
        return DocumentType::create(this, doctype->name(), doctype->publicId(), doctype->systemId());
    }
    case ELEMENT_NODE: {
        Element* oldElement = toElement(importedNode);
        // FIXME: The following check might be unnecessary. Is it possible that
        // oldElement has mismatched prefix/namespace?
        if (!hasValidNamespaceForElements(oldElement->tagQName())) {
            exceptionState.throwDOMException(NamespaceError, "The imported node has an invalid namespace.");
            return nullptr;
        }
        RefPtrWillBeRawPtr<Element> newElement = createElement(oldElement->tagQName(), false);

        newElement->cloneDataFromElement(*oldElement);

        if (deep) {
            if (!importContainerNodeChildren(oldElement, newElement, exceptionState))
                return nullptr;
            if (isHTMLTemplateElement(*oldElement)
                && !ensureTemplateDocument().importContainerNodeChildren(
                    toHTMLTemplateElement(oldElement)->content(),
                    toHTMLTemplateElement(newElement)->content(), exceptionState))
                return nullptr;
        }

        return newElement.release();
    }
    case ATTRIBUTE_NODE:
        return Attr::create(*this, QualifiedName(nullAtom, AtomicString(toAttr(importedNode)->name()), nullAtom), toAttr(importedNode)->value());
    case DOCUMENT_FRAGMENT_NODE: {
        if (importedNode->isShadowRoot()) {
            // ShadowRoot nodes should not be explicitly importable.
            // Either they are imported along with their host node, or created implicitly.
            exceptionState.throwDOMException(NotSupportedError, "The node provided is a shadow root, which may not be imported.");
            return nullptr;
        }
        DocumentFragment* oldFragment = toDocumentFragment(importedNode);
        RefPtrWillBeRawPtr<DocumentFragment> newFragment = createDocumentFragment();
        if (deep && !importContainerNodeChildren(oldFragment, newFragment, exceptionState))
            return nullptr;

        return newFragment.release();
    }
    case DOCUMENT_NODE:
        exceptionState.throwDOMException(NotSupportedError, "The node provided is a document, which may not be imported.");
        return nullptr;
    }

    ASSERT_NOT_REACHED();
    return nullptr;
}

PassRefPtrWillBeRawPtr<Node> Document::adoptNode(PassRefPtrWillBeRawPtr<Node> source, ExceptionState& exceptionState)
{
    EventQueueScope scope;

    switch (source->nodeType()) {
    case DOCUMENT_NODE:
        exceptionState.throwDOMException(NotSupportedError, "The node provided is of type '" + source->nodeName() + "', which may not be adopted.");
        return nullptr;
    case ATTRIBUTE_NODE: {
        Attr* attr = toAttr(source.get());
        if (RefPtrWillBeRawPtr<Element> ownerElement = attr->ownerElement())
            ownerElement->removeAttributeNode(attr, exceptionState);
        break;
    }
    default:
        if (source->isShadowRoot()) {
            // ShadowRoot cannot disconnect itself from the host node.
            exceptionState.throwDOMException(HierarchyRequestError, "The node provided is a shadow root, which may not be adopted.");
            return nullptr;
        }

        if (source->isFrameOwnerElement()) {
            HTMLFrameOwnerElement* frameOwnerElement = toHTMLFrameOwnerElement(source.get());
            // FIXME(kenrb): the downcast can be removed when the FrameTree supports RemoteFrames.
            if (frame() && frame()->tree().isDescendantOf(toLocalFrameTemporary(frameOwnerElement->contentFrame()))) {
                exceptionState.throwDOMException(HierarchyRequestError, "The node provided is a frame which contains this document.");
                return nullptr;
            }
        }
        if (source->parentNode()) {
            source->parentNode()->removeChild(source.get(), exceptionState);
            if (exceptionState.hadException())
                return nullptr;
        }
    }

    this->adoptIfNeeded(*source);

    return source;
}

bool Document::hasValidNamespaceForElements(const QualifiedName& qName)
{
    // These checks are from DOM Core Level 2, createElementNS
    // http://www.w3.org/TR/DOM-Level-2-Core/core.html#ID-DocCrElNS
    if (!qName.prefix().isEmpty() && qName.namespaceURI().isNull()) // createElementNS(null, "html:div")
        return false;
    if (qName.prefix() == xmlAtom && qName.namespaceURI() != XMLNames::xmlNamespaceURI) // createElementNS("http://www.example.com", "xml:lang")
        return false;

    // Required by DOM Level 3 Core and unspecified by DOM Level 2 Core:
    // http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/core.html#ID-DocCrElNS
    // createElementNS("http://www.w3.org/2000/xmlns/", "foo:bar"), createElementNS(null, "xmlns:bar"), createElementNS(null, "xmlns")
    if (qName.prefix() == xmlnsAtom || (qName.prefix().isEmpty() && qName.localName() == xmlnsAtom))
        return qName.namespaceURI() == XMLNSNames::xmlnsNamespaceURI;
    return qName.namespaceURI() != XMLNSNames::xmlnsNamespaceURI;
}

bool Document::hasValidNamespaceForAttributes(const QualifiedName& qName)
{
    return hasValidNamespaceForElements(qName);
}

// FIXME: This should really be in a possible ElementFactory class
PassRefPtrWillBeRawPtr<Element> Document::createElement(const QualifiedName& qName, bool createdByParser)
{
    RefPtrWillBeRawPtr<Element> e = nullptr;

    // FIXME: Use registered namespaces and look up in a hash to find the right factory.
    if (qName.namespaceURI() == xhtmlNamespaceURI)
        e = HTMLElementFactory::createHTMLElement(qName.localName(), *this, 0, createdByParser);
    else if (qName.namespaceURI() == SVGNames::svgNamespaceURI)
        e = SVGElementFactory::createSVGElement(qName.localName(), *this, createdByParser);

    if (e)
        m_sawElementsInKnownNamespaces = true;
    else
        e = Element::create(qName, this);

    if (e->prefix() != qName.prefix())
        e->setTagNameForCreateElementNS(qName);

    ASSERT(qName == e->tagQName());

    return e.release();
}

String Document::readyState() const
{
    DEFINE_STATIC_LOCAL(const String, loading, ("loading"));
    DEFINE_STATIC_LOCAL(const String, interactive, ("interactive"));
    DEFINE_STATIC_LOCAL(const String, complete, ("complete"));

    switch (m_readyState) {
    case Loading:
        return loading;
    case Interactive:
        return interactive;
    case Complete:
        return complete;
    }

    ASSERT_NOT_REACHED();
    return String();
}

void Document::setReadyState(ReadyState readyState)
{
    if (readyState == m_readyState)
        return;

    switch (readyState) {
    case Loading:
        if (!m_documentTiming.domLoading()) {
            m_documentTiming.setDomLoading(monotonicallyIncreasingTime());
        }
        break;
    case Interactive:
        if (!m_documentTiming.domInteractive())
            m_documentTiming.setDomInteractive(monotonicallyIncreasingTime());
        break;
    case Complete:
        if (!m_documentTiming.domComplete())
            m_documentTiming.setDomComplete(monotonicallyIncreasingTime());
        break;
    }

    m_readyState = readyState;
    dispatchEvent(Event::create(EventTypeNames::readystatechange));
}

bool Document::isLoadCompleted()
{
    return m_readyState == Complete;
}

AtomicString Document::encodingName() const
{
    // TextEncoding::name() returns a char*, no need to allocate a new
    // String for it each time.
    // FIXME: We should fix TextEncoding to speak AtomicString anyway.
    return AtomicString(encoding().name());
}

String Document::defaultCharset() const
{
    if (Settings* settings = this->settings())
        return settings->defaultTextEncodingName();
    return String();
}

void Document::setContentLanguage(const AtomicString& language)
{
    if (m_contentLanguage == language)
        return;
    m_contentLanguage = language;

    // Document's style depends on the content language.
    setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::Language));
}

void Document::setXMLVersion(const String& version, ExceptionState& exceptionState)
{
    if (!XMLDocumentParser::supportsXMLVersion(version)) {
        exceptionState.throwDOMException(NotSupportedError, "This document does not support the XML version '" + version + "'.");
        return;
    }

    m_xmlVersion = version;
}

void Document::setXMLStandalone(bool standalone, ExceptionState& exceptionState)
{
    m_xmlStandalone = standalone ? Standalone : NotStandalone;
}

KURL Document::baseURI() const
{
    return m_baseURL;
}

void Document::setContent(const String& content)
{
    open();
    m_parser->append(content);
    close();
}

String Document::suggestedMIMEType() const
{
    if (isXMLDocument()) {
        if (isXHTMLDocument())
            return "application/xhtml+xml";
        if (isSVGDocument())
            return "image/svg+xml";
        return "application/xml";
    }
    if (xmlStandalone())
        return "text/xml";
    if (isHTMLDocument())
        return "text/html";

    if (DocumentLoader* documentLoader = loader())
        return documentLoader->responseMIMEType();
    return String();
}

void Document::setMimeType(const AtomicString& mimeType)
{
    m_mimeType = mimeType;
}

AtomicString Document::contentType() const
{
    if (!m_mimeType.isEmpty())
        return m_mimeType;

    if (DocumentLoader* documentLoader = loader())
        return documentLoader->mimeType();

    String mimeType = suggestedMIMEType();
    if (!mimeType.isEmpty())
        return AtomicString(mimeType);

    return AtomicString("application/xml");
}

Element* Document::elementFromPoint(int x, int y) const
{
    if (!layoutView())
        return 0;

    return TreeScope::elementFromPoint(x, y);
}

Vector<Element*> Document::elementsFromPoint(int x, int y) const
{
    if (!layoutView())
        return Vector<Element*>();
    return TreeScope::elementsFromPoint(x, y);
}

PassRefPtrWillBeRawPtr<Range> Document::caretRangeFromPoint(int x, int y)
{
    if (!layoutView())
        return nullptr;

    HitTestResult result = hitTestInDocument(this, x, y);
    PositionWithAffinity positionWithAffinity = result.position();
    if (positionWithAffinity.position().isNull())
        return nullptr;

    Position rangeCompliantPosition = positionWithAffinity.position().parentAnchoredEquivalent();
    return Range::createAdjustedToTreeScope(*this, rangeCompliantPosition);
}

Element* Document::scrollingElement()
{
    if (RuntimeEnabledFeatures::scrollTopLeftInteropEnabled()) {
        if (inQuirksMode()) {
            updateLayoutTreeIfNeeded();
            HTMLBodyElement* body = firstBodyElement();
            if (body && body->layoutObject() && body->layoutObject()->hasOverflowClip())
                return nullptr;

            return body;
        }

        return documentElement();
    }

    return body();
}

/*
 * Performs three operations:
 *  1. Convert control characters to spaces
 *  2. Trim leading and trailing spaces
 *  3. Collapse internal whitespace.
 */
template <typename CharacterType>
static inline String canonicalizedTitle(Document* document, const String& title)
{
    unsigned length = title.length();
    unsigned builderIndex = 0;
    const CharacterType* characters = title.getCharacters<CharacterType>();

    StringBuffer<CharacterType> buffer(length);

    // Replace control characters with spaces and collapse whitespace.
    bool pendingWhitespace = false;
    for (unsigned i = 0; i < length; ++i) {
        UChar32 c = characters[i];
        if (c <= 0x20 || c == 0x7F || (WTF::Unicode::category(c) & (WTF::Unicode::Separator_Line | WTF::Unicode::Separator_Paragraph))) {
            if (builderIndex != 0)
                pendingWhitespace = true;
        } else {
            if (pendingWhitespace) {
                buffer[builderIndex++] = ' ';
                pendingWhitespace = false;
            }
            buffer[builderIndex++] = c;
        }
    }
    buffer.shrink(builderIndex);

    return String::adopt(buffer);
}

void Document::updateTitle(const String& title)
{
    if (m_rawTitle == title)
        return;

    m_rawTitle = title;

    String oldTitle = m_title;
    if (m_rawTitle.isEmpty())
        m_title = String();
    else if (m_rawTitle.is8Bit())
        m_title = canonicalizedTitle<LChar>(this, m_rawTitle);
    else
        m_title = canonicalizedTitle<UChar>(this, m_rawTitle);

    if (!m_frame || oldTitle == m_title)
        return;
    m_frame->loader().client()->dispatchDidReceiveTitle(m_title);
}

void Document::setTitle(const String& title)
{
    // Title set by JavaScript -- overrides any title elements.
    if (!isHTMLDocument() && !isXHTMLDocument()) {
        m_titleElement = nullptr;
    } else if (!m_titleElement) {
        HTMLElement* headElement = head();
        if (!headElement)
            return;
        m_titleElement = HTMLTitleElement::create(*this);
        headElement->appendChild(m_titleElement.get());
    }

    if (isHTMLTitleElement(m_titleElement))
        toHTMLTitleElement(m_titleElement)->setText(title);
    else
        updateTitle(title);
}

void Document::setTitleElement(Element* titleElement)
{
    // Only allow the first title element to change the title -- others have no effect.
    if (m_titleElement && m_titleElement != titleElement) {
        if (isHTMLDocument() || isXHTMLDocument()) {
            m_titleElement = Traversal<HTMLTitleElement>::firstWithin(*this);
        } else if (isSVGDocument()) {
            m_titleElement = Traversal<SVGTitleElement>::firstWithin(*this);
        }
    } else {
        m_titleElement = titleElement;
    }

    if (isHTMLTitleElement(m_titleElement))
        updateTitle(toHTMLTitleElement(m_titleElement)->text());
    else if (isSVGTitleElement(m_titleElement))
        updateTitle(toSVGTitleElement(m_titleElement)->textContent());
}

void Document::removeTitle(Element* titleElement)
{
    if (m_titleElement != titleElement)
        return;

    m_titleElement = nullptr;

    // Update title based on first title element in the document, if one exists.
    if (isHTMLDocument() || isXHTMLDocument()) {
        if (HTMLTitleElement* title = Traversal<HTMLTitleElement>::firstWithin(*this))
            setTitleElement(title);
    } else if (isSVGDocument()) {
        if (SVGTitleElement* title = Traversal<SVGTitleElement>::firstWithin(*this))
            setTitleElement(title);
    }

    if (!m_titleElement)
        updateTitle(String());
}

const AtomicString& Document::dir()
{
    Element* rootElement = documentElement();
    if (isHTMLHtmlElement(rootElement))
        return toHTMLHtmlElement(rootElement)->dir();
    return nullAtom;
}

void Document::setDir(const AtomicString& value)
{
    Element* rootElement = documentElement();
    if (isHTMLHtmlElement(rootElement))
        toHTMLHtmlElement(rootElement)->setDir(value);
}

PageVisibilityState Document::pageVisibilityState() const
{
    // The visibility of the document is inherited from the visibility of the
    // page. If there is no page associated with the document, we will assume
    // that the page is hidden, as specified by the spec:
    // http://dvcs.w3.org/hg/webperf/raw-file/tip/specs/PageVisibility/Overview.html#dom-document-hidden
    if (!m_frame || !m_frame->page())
        return PageVisibilityStateHidden;
    return m_frame->page()->visibilityState();
}

String Document::visibilityState() const
{
    return pageVisibilityStateString(pageVisibilityState());
}

bool Document::hidden() const
{
    return pageVisibilityState() != PageVisibilityStateVisible;
}

void Document::didChangeVisibilityState()
{
    dispatchEvent(Event::create(EventTypeNames::visibilitychange));
    // Also send out the deprecated version until it can be removed.
    dispatchEvent(Event::create(EventTypeNames::webkitvisibilitychange));

    PageVisibilityState state = pageVisibilityState();
    for (DocumentVisibilityObserver* observer : m_visibilityObservers)
        observer->didChangeVisibilityState(state);

    if (hidden() && m_canvasFontCache)
        m_canvasFontCache->pruneAll();
}

void Document::registerVisibilityObserver(DocumentVisibilityObserver* observer)
{
    ASSERT(!m_visibilityObservers.contains(observer));
    m_visibilityObservers.add(observer);
}

void Document::unregisterVisibilityObserver(DocumentVisibilityObserver* observer)
{
    ASSERT(m_visibilityObservers.contains(observer));
    m_visibilityObservers.remove(observer);
}

String Document::nodeName() const
{
    return "#document";
}

Node::NodeType Document::nodeType() const
{
    return DOCUMENT_NODE;
}

FormController& Document::formController()
{
    if (!m_formController) {
        m_formController = FormController::create();
        if (m_frame && m_frame->loader().currentItem() && m_frame->loader().currentItem()->isCurrentDocument(this))
            m_frame->loader().currentItem()->setDocumentState(m_formController->formElementsState());
    }
    return *m_formController;
}

DocumentState* Document::formElementsState() const
{
    if (!m_formController)
        return 0;
    return m_formController->formElementsState();
}

void Document::setStateForNewFormElements(const Vector<String>& stateVector)
{
    if (!stateVector.size() && !m_formController)
        return;
    formController().setStateForNewFormElements(stateVector);
}

FrameView* Document::view() const
{
    return m_frame ? m_frame->view() : 0;
}

Page* Document::page() const
{
    return m_frame ? m_frame->page() : 0;
}

FrameHost* Document::frameHost() const
{
    return m_frame ? m_frame->host() : 0;
}

Settings* Document::settings() const
{
    return m_frame ? m_frame->settings() : 0;
}

PassRefPtrWillBeRawPtr<Range> Document::createRange()
{
    return Range::create(*this);
}

PassRefPtrWillBeRawPtr<NodeIterator> Document::createNodeIterator(Node* root, unsigned whatToShow, PassRefPtrWillBeRawPtr<NodeFilter> filter, ExceptionState& exceptionState)
{
    // FIXME: It might be a good idea to emit a warning if |whatToShow| contains a bit that is not defined in
    // NodeFilter.
    return NodeIterator::create(root, whatToShow, filter);
}

PassRefPtrWillBeRawPtr<TreeWalker> Document::createTreeWalker(Node* root, unsigned whatToShow, PassRefPtrWillBeRawPtr<NodeFilter> filter, ExceptionState& exceptionState)
{
    return TreeWalker::create(root, whatToShow, filter);
}

bool Document::needsLayoutTreeUpdate() const
{
    if (!isActive() || !view())
        return false;
    if (needsFullLayoutTreeUpdate())
        return true;
    if (childNeedsStyleRecalc())
        return true;
    if (childNeedsStyleInvalidation())
        return true;
    if (layoutView()->wasNotifiedOfSubtreeChange())
        return true;
    return false;
}

bool Document::needsFullLayoutTreeUpdate() const
{
    if (!isActive() || !view())
        return false;
    if (!m_useElementsNeedingUpdate.isEmpty())
        return true;
    if (!m_layerUpdateSVGFilterElements.isEmpty())
        return true;
    if (needsStyleRecalc())
        return true;
    if (needsStyleInvalidation())
        return true;
    // FIXME: The childNeedsDistributionRecalc bit means either self or children, we should fix that.
    if (childNeedsDistributionRecalc())
        return true;
    if (DocumentAnimations::needsOutdatedAnimationUpdate(*this))
        return true;
    return false;
}

bool Document::shouldScheduleLayoutTreeUpdate() const
{
    if (!isActive())
        return false;
    if (inStyleRecalc())
        return false;
    // InPreLayout will recalc style itself. There's no reason to schedule another recalc.
    if (m_lifecycle.state() == DocumentLifecycle::InPreLayout)
        return false;
    if (!shouldScheduleLayout())
        return false;
    return true;
}

void Document::scheduleLayoutTreeUpdate()
{
    ASSERT(!hasPendingStyleRecalc());
    ASSERT(shouldScheduleLayoutTreeUpdate());
    ASSERT(needsLayoutTreeUpdate());

    page()->animator().scheduleVisualUpdate();
    m_lifecycle.ensureStateAtMost(DocumentLifecycle::VisualUpdatePending);

    TRACE_EVENT_INSTANT1(TRACE_DISABLED_BY_DEFAULT("devtools.timeline"), "ScheduleStyleRecalculation", TRACE_EVENT_SCOPE_THREAD, "data", InspectorRecalculateStylesEvent::data(frame()));
    InspectorInstrumentation::didScheduleStyleRecalculation(this);
}

bool Document::hasPendingForcedStyleRecalc() const
{
    return hasPendingStyleRecalc() && !inStyleRecalc() && styleChangeType() >= SubtreeStyleChange;
}

void Document::updateStyleInvalidationIfNeeded()
{
    ScriptForbiddenScope forbidScript;

    if (!isActive())
        return;
    if (!childNeedsStyleInvalidation())
        return;
    TRACE_EVENT0("blink", "Document::updateStyleInvalidationIfNeeded");
    styleEngine().styleInvalidator().invalidate(*this);
}

void Document::setupFontBuilder(ComputedStyle& documentStyle)
{
    FontBuilder fontBuilder(*this);
    RefPtrWillBeRawPtr<CSSFontSelector> selector = styleEngine().fontSelector();
    fontBuilder.createFontForDocument(selector, documentStyle);
}

void Document::inheritHtmlAndBodyElementStyles(StyleRecalcChange change)
{
    ASSERT(inStyleRecalc());
    ASSERT(documentElement());

    bool didRecalcDocumentElement = false;
    RefPtr<ComputedStyle> documentElementStyle = documentElement()->mutableComputedStyle();
    if (change == Force)
        documentElement()->clearAnimationStyleChange();
    if (!documentElementStyle || documentElement()->needsStyleRecalc() || change == Force) {
        documentElementStyle = ensureStyleResolver().styleForElement(documentElement());
        didRecalcDocumentElement = true;
    }

    WritingMode rootWritingMode = documentElementStyle->writingMode();
    TextDirection rootDirection = documentElementStyle->direction();

    HTMLElement* body = this->body();
    RefPtr<ComputedStyle> bodyStyle;

    if (body) {
        bodyStyle = body->mutableComputedStyle();
        if (didRecalcDocumentElement)
            body->clearAnimationStyleChange();
        if (!bodyStyle || body->needsStyleRecalc() || didRecalcDocumentElement)
            bodyStyle = ensureStyleResolver().styleForElement(body, documentElementStyle.get());
        rootWritingMode = bodyStyle->writingMode();
        rootDirection = bodyStyle->direction();
    }

    const ComputedStyle* backgroundStyle = documentElementStyle.get();
    // http://www.w3.org/TR/css3-background/#body-background
    // <html> root element with no background steals background from its first <body> child.
    // Also see LayoutBoxModelObject::backgroundStolenForBeingBody()
    if (isHTMLHtmlElement(documentElement()) && isHTMLBodyElement(body) && !backgroundStyle->hasBackground())
        backgroundStyle = bodyStyle.get();
    Color backgroundColor = backgroundStyle->visitedDependentColor(CSSPropertyBackgroundColor);
    FillLayer backgroundLayers = backgroundStyle->backgroundLayers();
    for (auto currentLayer = &backgroundLayers; currentLayer; currentLayer = currentLayer->next()) {
        // http://www.w3.org/TR/css3-background/#root-background
        // The root element background always have painting area of the whole canvas.
        currentLayer->setClip(BorderFillBox);

        // The root element doesn't scroll. It always propagates its layout overflow
        // to the viewport. Positioning background against either box is equivalent to
        // positioning against the scrolled box of the viewport.
        if (currentLayer->attachment() == ScrollBackgroundAttachment)
            currentLayer->setAttachment(LocalBackgroundAttachment);
    }
    EImageRendering imageRendering = backgroundStyle->imageRendering();

    const ComputedStyle* overflowStyle = nullptr;
    if (Element* element = viewportDefiningElement(documentElementStyle.get())) {
        if (element == body) {
            overflowStyle = bodyStyle.get();
        } else {
            ASSERT(element == documentElement());
            overflowStyle = documentElementStyle.get();
        }
    }

    // Resolved rem units are stored in the matched properties cache so we need to make sure to
    // invalidate the cache if the documentElement needed to reattach or the font size changed
    // and then trigger a full document recalc. We also need to clear it here since the
    // call to styleForElement on the body above can cache bad values for rem units if the
    // documentElement's style was dirty. We could keep track of which elements depend on
    // rem units like we do for viewport styles, but we assume root font size changes are
    // rare and just invalidate the cache for now.
    if (styleEngine().usesRemUnits() && (documentElement()->needsAttach() || documentElement()->ensureComputedStyle()->fontSize() != documentElementStyle->fontSize())) {
        ensureStyleResolver().invalidateMatchedPropertiesCache();
        documentElement()->setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::FontSizeChange));
    }

    EOverflow overflowX = OAUTO;
    EOverflow overflowY = OAUTO;
    float columnGap = 0;
    if (overflowStyle) {
        overflowX = overflowStyle->overflowX();
        overflowY = overflowStyle->overflowY();
        // Visible overflow on the viewport is meaningless, and the spec says to treat it as 'auto':
        if (overflowX == OVISIBLE)
            overflowX = OAUTO;
        if (overflowY == OVISIBLE)
            overflowY = OAUTO;
        // Column-gap is (ab)used by the current paged overflow implementation (in lack of other
        // ways to specify gaps between pages), so we have to propagate it too.
        columnGap = overflowStyle->columnGap();
    }

    WebScrollBlocksOn scrollBlocksOn = documentElementStyle->scrollBlocksOn();

    RefPtr<ComputedStyle> documentStyle = layoutView()->mutableStyle();
    if (documentStyle->writingMode() != rootWritingMode
        || documentStyle->direction() != rootDirection
        || documentStyle->visitedDependentColor(CSSPropertyBackgroundColor) != backgroundColor
        || documentStyle->backgroundLayers() != backgroundLayers
        || documentStyle->imageRendering() != imageRendering
        || documentStyle->overflowX() != overflowX
        || documentStyle->overflowY() != overflowY
        || documentStyle->columnGap() != columnGap
        || documentStyle->scrollBlocksOn() != scrollBlocksOn) {
        RefPtr<ComputedStyle> newStyle = ComputedStyle::clone(*documentStyle);
        newStyle->setWritingMode(rootWritingMode);
        newStyle->setDirection(rootDirection);
        newStyle->setBackgroundColor(backgroundColor);
        newStyle->accessBackgroundLayers() = backgroundLayers;
        newStyle->setImageRendering(imageRendering);
        newStyle->setOverflowX(overflowX);
        newStyle->setOverflowY(overflowY);
        newStyle->setColumnGap(columnGap);
        newStyle->setScrollBlocksOn(scrollBlocksOn);
        layoutView()->setStyle(newStyle);
        setupFontBuilder(*newStyle);
    }

    if (body) {
        if (const ComputedStyle* style = body->computedStyle()) {
            if (style->direction() != rootDirection || style->writingMode() != rootWritingMode)
                body->setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::WritingModeChange));
        }
    }

    if (const ComputedStyle* style = documentElement()->computedStyle()) {
        if (style->direction() != rootDirection || style->writingMode() != rootWritingMode)
            documentElement()->setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::WritingModeChange));
    }
}

#if ENABLE(ASSERT)
static void assertLayoutTreeUpdated(Node& root)
{
    for (Node& node : NodeTraversal::inclusiveDescendantsOf(root)) {
        // We leave some nodes with dirty bits in the tree because they don't
        // matter like Comment and ProcessingInstruction nodes.
        // TODO(esprehn): Don't even mark those nodes as needing recalcs in the
        // first place.
        if (!node.isElementNode()
            && !node.isTextNode()
            && !node.isShadowRoot()
            && !node.isDocumentNode())
            continue;
        ASSERT(!node.needsStyleRecalc());
        ASSERT(!node.childNeedsStyleRecalc());
        ASSERT(!node.childNeedsDistributionRecalc());
        ASSERT(!node.needsStyleInvalidation());
        ASSERT(!node.childNeedsStyleInvalidation());
        for (ShadowRoot* shadowRoot = node.youngestShadowRoot(); shadowRoot; shadowRoot = shadowRoot->olderShadowRoot())
            assertLayoutTreeUpdated(*shadowRoot);
    }
}
#endif

void Document::updateLayoutTree(StyleRecalcChange change)
{
    ASSERT(isMainThread());

    ScriptForbiddenScope forbidScript;

    if (!view() || !isActive())
        return;

    if (change != Force && !needsLayoutTreeUpdate())
        return;

    if (inStyleRecalc())
        return;

    // Entering here from inside layout or paint would be catastrophic since recalcStyle can
    // tear down the layout tree or (unfortunately) run script. Kill the whole layoutObject if
    // someone managed to get into here from inside layout or paint.
    RELEASE_ASSERT(!view()->isInPerformLayout());
    RELEASE_ASSERT(!view()->isPainting());

    // Script can run below in WidgetUpdates, so protect the LocalFrame.
    // FIXME: Can this still happen? How does script run inside
    // UpdateSuspendScope::performDeferredWidgetTreeOperations() ?
    RefPtrWillBeRawPtr<LocalFrame> protect(m_frame.get());

    TRACE_EVENT_BEGIN1("blink,devtools.timeline", "UpdateLayoutTree", "beginData", InspectorRecalculateStylesEvent::data(frame()));
    TRACE_EVENT_SCOPED_SAMPLING_STATE("blink", "UpdateLayoutTree");

    // FIXME: Remove m_styleRecalcElementCounter, we should just use the accessCount() on the resolver.
    m_styleRecalcElementCounter = 0;
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::willRecalculateStyle(this);

    DocumentAnimations::updateAnimationTimingIfNeeded(*this);
    evaluateMediaQueryListIfNeeded();
    updateUseShadowTreesIfNeeded();
    updateDistribution();
    updateStyleInvalidationIfNeeded();

    // FIXME: We should update style on our ancestor chain before proceeding
    // however doing so currently causes several tests to crash, as LocalFrame::setDocument calls Document::attach
    // before setting the LocalDOMWindow on the LocalFrame, or the SecurityOrigin on the document. The attach, in turn
    // resolves style (here) and then when we resolve style on the parent chain, we may end up
    // re-attaching our containing iframe, which when asked HTMLFrameElementBase::isURLAllowed
    // hits a null-dereference due to security code always assuming the document has a SecurityOrigin.

    if (m_elemSheet && m_elemSheet->contents()->usesRemUnits())
        styleEngine().setUsesRemUnit(true);

    updateStyle(change);

    notifyLayoutTreeOfSubtreeChanges();

    // As a result of the style recalculation, the currently hovered element might have been
    // detached (for example, by setting display:none in the :hover style), schedule another mouseMove event
    // to check if any other elements ended up under the mouse pointer due to re-layout.
    if (hoverNode() && !hoverNode()->layoutObject() && frame())
        frame()->eventHandler().dispatchFakeMouseMoveEventSoon();

    if (m_focusedElement && !m_focusedElement->isFocusable())
        clearFocusedElementSoon();
    layoutView()->clearHitTestCache();

    ASSERT(!m_timeline->hasOutdatedAnimation());

    TRACE_EVENT_END1("blink,devtools.timeline", "UpdateLayoutTree", "elementCount", m_styleRecalcElementCounter);
    InspectorInstrumentation::didRecalculateStyle(cookie, m_styleRecalcElementCounter);

#if ENABLE(ASSERT)
    assertLayoutTreeUpdated(*this);
#endif
}

void Document::updateStyle(StyleRecalcChange change)
{
    TRACE_EVENT_BEGIN0("blink,blink_style", "Document::updateStyle");
    unsigned initialResolverAccessCount = styleEngine().resolverAccessCount();

    HTMLFrameOwnerElement::UpdateSuspendScope suspendWidgetHierarchyUpdates;
    m_lifecycle.advanceTo(DocumentLifecycle::InStyleRecalc);

    NthIndexCache nthIndexCache(*this);

    if (styleChangeType() >= SubtreeStyleChange)
        change = Force;

    // FIXME: Cannot access the ensureStyleResolver() before calling styleForDocument below because
    // apparently the StyleResolver's constructor has side effects. We should fix it.
    // See printing/setPrinting.html, printing/width-overflow.html though they only fail on
    // mac when accessing the resolver by what appears to be a viewport size difference.

    if (change == Force) {
        m_hasNodesWithPlaceholderStyle = false;
        RefPtr<ComputedStyle> documentStyle = StyleResolver::styleForDocument(*this);
        StyleRecalcChange localChange = ComputedStyle::stylePropagationDiff(documentStyle.get(), layoutView()->style());
        if (localChange != NoChange)
            layoutView()->setStyle(documentStyle.release());
    }

    clearNeedsStyleRecalc();

    StyleResolver& resolver = ensureStyleResolver();

    bool shouldRecordStats = false;
    TRACE_EVENT_CATEGORY_GROUP_ENABLED("blink,blink_style", &shouldRecordStats);
    resolver.setStatsEnabled(shouldRecordStats);

    if (Element* documentElement = this->documentElement()) {
        inheritHtmlAndBodyElementStyles(change);
        dirtyElementsForLayerUpdate();

        if (documentElement->shouldCallRecalcStyle(change))
            documentElement->recalcStyle(change);

        while (dirtyElementsForLayerUpdate())
            documentElement->recalcStyle(NoChange);
    }

    view()->recalcOverflowAfterStyleChange();
    
    clearChildNeedsStyleRecalc();

    // Pseudo element removal and similar may only work with these flags still set. Reset them after the style recalc.
    styleEngine().resetCSSFeatureFlags(resolver.ensureUpdatedRuleFeatureSet());
    resolver.clearStyleSharingList();

    ASSERT(!needsStyleRecalc());
    ASSERT(!childNeedsStyleRecalc());
    ASSERT(inStyleRecalc());
    ASSERT(styleResolver() == &resolver);
    m_lifecycle.advanceTo(DocumentLifecycle::StyleClean);
    if (shouldRecordStats) {
        TRACE_EVENT_END2("blink,blink_style", "Document::updateStyle",
            "resolverAccessCount", styleEngine().resolverAccessCount() - initialResolverAccessCount,
            "counters", resolver.stats()->toTracedValue());
    } else {
        TRACE_EVENT_END1("blink,blink_style", "Document::updateStyle",
            "resolverAccessCount", styleEngine().resolverAccessCount() - initialResolverAccessCount);
    }
}

void Document::notifyLayoutTreeOfSubtreeChanges()
{
    if (!layoutView()->wasNotifiedOfSubtreeChange())
        return;

    m_lifecycle.advanceTo(DocumentLifecycle::InLayoutSubtreeChange);

    layoutView()->handleSubtreeModifications();
    ASSERT(!layoutView()->wasNotifiedOfSubtreeChange());

    m_lifecycle.advanceTo(DocumentLifecycle::LayoutSubtreeChangeClean);
}

void Document::updateLayoutTreeForNodeIfNeeded(Node* node)
{
    ASSERT(node);
    if (!node->canParticipateInComposedTree())
        return;
    if (!needsLayoutTreeUpdate())
        return;

    // https://chromium.googlesource.com/chromium/src/+log/daf0427f8e0301729a905fad746c6e8afbeffd11/third_party/WebKit/Source/core/dom/Document.cpp?s=7a4cc8d341b50e4f51c73ec408f038940f38c6b0
    // https://chromium.googlesource.com/chromium/src/+/73a40db1b07fbd9b67286a6cbda1491418fec3d9%5E%21/#F0
    // https://chromium.googlesource.com/chromium/src/+/40fbf8f4d6a7af7041d45319d92312755aa28977%5E%21/#F4
    if (!node->inDocument())
        return;

    bool needsRecalc = needsFullLayoutTreeUpdate() || node->needsStyleRecalc() || node->needsStyleInvalidation();

    if (!needsRecalc) {
        for (const ContainerNode* ancestor = LayoutTreeBuilderTraversal::parent(*node); ancestor && !needsRecalc; ancestor = LayoutTreeBuilderTraversal::parent(*ancestor))
            needsRecalc = ancestor->needsStyleRecalc() || ancestor->needsStyleInvalidation() || ancestor->needsAdjacentStyleRecalc();
    }

    if (needsRecalc)
        updateLayoutTreeIfNeeded();
}

void Document::updateLayout()
{
    ASSERT(isMainThread());

    ScriptForbiddenScope forbidScript;

    RefPtrWillBeRawPtr<FrameView> frameView = view();
    if (frameView && frameView->isInPerformLayout()) {
        // View layout should not be re-entrant.
        ASSERT_NOT_REACHED();
        return;
    }

    if (HTMLFrameOwnerElement* owner = ownerElement())
        owner->document().updateLayout();

    updateLayoutTreeIfNeeded();

    if (!isActive())
        return;

    if (frameView->needsLayout())
        frameView->layout();

    if (lifecycle().state() < DocumentLifecycle::LayoutClean)
        lifecycle().advanceTo(DocumentLifecycle::LayoutClean);
}

void Document::layoutUpdated()
{
    markers().updateRenderedRectsForMarkers();

    // The layout system may perform layouts with pending stylesheets. When
    // recording first layout time, we ignore these layouts, since painting is
    // suppressed for them. We're interested in tracking the time of the
    // first real or 'paintable' layout.
    if (isRenderingReady() && body() && !styleEngine().hasPendingSheets()) {
        if (!m_documentTiming.firstLayout())
            m_documentTiming.setFirstLayout(monotonicallyIncreasingTime());
    }
}

void Document::setNeedsFocusedElementCheck()
{
    setNeedsStyleRecalc(LocalStyleChange, StyleChangeReasonForTracing::createWithExtraData(StyleChangeReason::PseudoClass, StyleChangeExtraData::Focus));
}

void Document::clearFocusedElementSoon()
{
    if (!m_clearFocusedElementTimer.isActive())
        m_clearFocusedElementTimer.startOneShot(0, FROM_HERE);
}

void Document::clearFocusedElementTimerFired(Timer<Document>*)
{
    updateLayoutTreeIfNeeded();
    m_clearFocusedElementTimer.stop();

    if (m_focusedElement && !m_focusedElement->isFocusable())
        m_focusedElement->blur();
}

// FIXME: This is a bad idea and needs to be removed eventually.
// Other browsers load stylesheets before they continue parsing the web page.
// Since we don't, we can run JavaScript code that needs answers before the
// stylesheets are loaded. Doing a layout ignoring the pending stylesheets
// lets us get reasonable answers. The long term solution to this problem is
// to instead suspend JavaScript execution.
void Document::updateLayoutIgnorePendingStylesheets(Document::RunPostLayoutTasks runPostLayoutTasks)
{
    StyleEngine::IgnoringPendingStylesheet ignoring(styleEngine());

    if (styleEngine().hasPendingSheets()) {
        // FIXME: We are willing to attempt to suppress painting with outdated style info only once.
        // Our assumption is that it would be dangerous to try to stop it a second time, after page
        // content has already been loaded and displayed with accurate style information. (Our
        // suppression involves blanking the whole page at the moment. If it were more refined, we
        // might be able to do something better.) It's worth noting though that this entire method
        // is a hack, since what we really want to do is suspend JS instead of doing a layout with
        // inaccurate information.
        HTMLElement* bodyElement = body();
        if (bodyElement && !bodyElement->layoutObject() && m_pendingSheetLayout == NoLayoutWithPendingSheets) {
            m_pendingSheetLayout = DidLayoutWithPendingSheets;
            styleResolverChanged();
        } else if (m_hasNodesWithPlaceholderStyle) {
            // If new nodes have been added or style recalc has been done with style sheets still
            // pending, some nodes may not have had their real style calculated yet. Normally this
            // gets cleaned when style sheets arrive but here we need up-to-date style immediately.
            updateLayoutTree(Force);
        }
    }

    updateLayout();

    if (runPostLayoutTasks == RunPostLayoutTasksSynchronously && view())
        view()->flushAnyPendingPostLayoutTasks();
}

PassRefPtr<ComputedStyle> Document::styleForElementIgnoringPendingStylesheets(Element* element)
{
    ASSERT_ARG(element, element->document() == this);
    StyleEngine::IgnoringPendingStylesheet ignoring(styleEngine());
    return ensureStyleResolver().styleForElement(element, element->parentNode() ? element->parentNode()->ensureComputedStyle() : 0);
}

PassRefPtr<ComputedStyle> Document::styleForPage(int pageIndex)
{
    updateDistribution();
    return ensureStyleResolver().styleForPage(pageIndex);
}

bool Document::isPageBoxVisible(int pageIndex)
{
    return styleForPage(pageIndex)->visibility() != HIDDEN; // display property doesn't apply to @page.
}

void Document::pageSizeAndMarginsInPixels(int pageIndex, IntSize& pageSize, int& marginTop, int& marginRight, int& marginBottom, int& marginLeft)
{
    RefPtr<ComputedStyle> style = styleForPage(pageIndex);

    int width = pageSize.width();
    int height = pageSize.height();
    switch (style->pageSizeType()) {
    case PAGE_SIZE_AUTO:
        break;
    case PAGE_SIZE_AUTO_LANDSCAPE:
        if (width < height)
            std::swap(width, height);
        break;
    case PAGE_SIZE_AUTO_PORTRAIT:
        if (width > height)
            std::swap(width, height);
        break;
    case PAGE_SIZE_RESOLVED: {
        FloatSize size = style->pageSize();
        width = size.width();
        height = size.height();
        break;
    }
    default:
        ASSERT_NOT_REACHED();
    }
    pageSize = IntSize(width, height);

    // The percentage is calculated with respect to the width even for margin top and bottom.
    // http://www.w3.org/TR/CSS2/box.html#margin-properties
    marginTop = style->marginTop().isAuto() ? marginTop : intValueForLength(style->marginTop(), width);
    marginRight = style->marginRight().isAuto() ? marginRight : intValueForLength(style->marginRight(), width);
    marginBottom = style->marginBottom().isAuto() ? marginBottom : intValueForLength(style->marginBottom(), width);
    marginLeft = style->marginLeft().isAuto() ? marginLeft : intValueForLength(style->marginLeft(), width);
}

void Document::setIsViewSource(bool isViewSource)
{
    m_isViewSource = isViewSource;
    if (!m_isViewSource)
        return;

    setSecurityOrigin(SecurityOrigin::createUnique());
    didUpdateSecurityOrigin();
}

bool Document::dirtyElementsForLayerUpdate()
{
    if (m_layerUpdateSVGFilterElements.isEmpty())
        return false;

    for (Element* element : m_layerUpdateSVGFilterElements)
        element->setNeedsStyleRecalc(LocalStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::SVGFilterLayerUpdate));
    m_layerUpdateSVGFilterElements.clear();
    return true;
}

void Document::scheduleSVGFilterLayerUpdateHack(Element& element)
{
    if (element.styleChangeType() == NeedsReattachStyleChange)
        return;
    element.setSVGFilterNeedsLayerUpdate();
    m_layerUpdateSVGFilterElements.add(&element);
    scheduleLayoutTreeUpdateIfNeeded();
}

void Document::unscheduleSVGFilterLayerUpdateHack(Element& element)
{
    element.clearSVGFilterNeedsLayerUpdate();
    m_layerUpdateSVGFilterElements.remove(&element);
}

void Document::scheduleUseShadowTreeUpdate(SVGUseElement& element)
{
    m_useElementsNeedingUpdate.add(&element);
    scheduleLayoutTreeUpdateIfNeeded();
}

void Document::unscheduleUseShadowTreeUpdate(SVGUseElement& element)
{
    m_useElementsNeedingUpdate.remove(&element);
}

void Document::updateUseShadowTreesIfNeeded()
{
    ScriptForbiddenScope forbidScript;

    if (m_useElementsNeedingUpdate.isEmpty())
        return;

    WillBeHeapVector<RawPtrWillBeMember<SVGUseElement>> elements;
    copyToVector(m_useElementsNeedingUpdate, elements);
    m_useElementsNeedingUpdate.clear();

    for (SVGUseElement* element : elements)
        element->buildPendingResource();
}

StyleResolver* Document::styleResolver() const
{
    return m_styleEngine->resolver();
}

StyleResolver& Document::ensureStyleResolver() const
{
    return m_styleEngine->ensureResolver();
}

void Document::attach(const AttachContext& context)
{
    ASSERT(m_lifecycle.state() == DocumentLifecycle::Inactive);
    ASSERT(!m_axObjectCache || this != &axObjectCacheOwner());

    m_layoutView = new LayoutView(this);
    setLayoutObject(m_layoutView);

    m_layoutView->setIsInWindow(true);
    m_layoutView->setStyle(StyleResolver::styleForDocument(*this));
    m_layoutView->compositor()->setNeedsCompositingUpdate(CompositingUpdateAfterCompositingInputChange);

    ContainerNode::attach(context);

    // The TextAutosizer can't update layout view info while the Document is detached, so update now in case anything changed.
    if (TextAutosizer* autosizer = textAutosizer())
        autosizer->updatePageInfo();

    m_lifecycle.advanceTo(DocumentLifecycle::StyleClean);
}

void Document::detach(const AttachContext& context)
{
    ASSERT(!m_frame || m_frame->tree().childCount() == 0);
    if (!isActive())
        return;

    ScriptForbiddenScope forbidScript;
    view()->dispose();
    m_markers->prepareForDestruction();
    if (LocalDOMWindow* window = this->domWindow())
        window->willDetachDocumentFromFrame();

    m_lifecycle.advanceTo(DocumentLifecycle::Stopping);

    if (page())
        page()->documentDetached(this);
    InspectorInstrumentation::documentDetached(this);

    if (m_frame->loader().client()->sharedWorkerRepositoryClient())
        m_frame->loader().client()->sharedWorkerRepositoryClient()->documentDetached(this);

    stopActiveDOMObjects();

    // FIXME: consider using ActiveDOMObject.
    if (m_scriptedAnimationController)
        m_scriptedAnimationController->clearDocumentPointer();
    m_scriptedAnimationController.clear();

    if (svgExtensions())
        accessSVGExtensions().pauseAnimations();

    // FIXME: This shouldn't be needed once LocalDOMWindow becomes ExecutionContext.
    if (m_domWindow)
        m_domWindow->clearEventQueue();

    if (m_layoutView)
        m_layoutView->setIsInWindow(false);

    if (registrationContext())
        registrationContext()->documentWasDetached();

    m_hoverNode = nullptr;
    m_activeHoverElement = nullptr;
    m_autofocusElement = nullptr;

    if (m_focusedElement.get()) {
        RefPtrWillBeRawPtr<Element> oldFocusedElement = m_focusedElement;
        m_focusedElement = nullptr;
        if (frameHost())
            frameHost()->chromeClient().focusedNodeChanged(oldFocusedElement.get(), nullptr);
    }

    if (this == &axObjectCacheOwner())
        clearAXObjectCache();

    m_layoutView = nullptr;
    ContainerNode::detach(context);

    styleEngine().didDetach();

    frameHost()->eventHandlerRegistry().documentDetached(*this);

    // If this Document is associated with a live DocumentLoader, the
    // DocumentLoader will take care of clearing the FetchContext. Deferring
    // to the DocumentLoader when possible also prevents prematurely clearing
    // the context in the case where multiple Documents end up associated with
    // a single DocumentLoader (e.g., navigating to a javascript: url).
    if (!loader())
        m_fetcher->clearContext();
    // If this document is the master for an HTMLImportsController, sever that
    // relationship. This ensures that we don't leave import loads in flight,
    // thinking they should have access to a valid frame when they don't.
    if (m_importsController)
        HTMLImportsController::removeFrom(*this);

    // This is required, as our LocalFrame might delete itself as soon as it detaches
    // us. However, this violates Node::detach() semantics, as it's never
    // possible to re-attach. Eventually Document::detach() should be renamed,
    // or this setting of the frame to 0 could be made explicit in each of the
    // callers of Document::detach().
    m_frame = nullptr;

    if (m_mediaQueryMatcher)
        m_mediaQueryMatcher->documentDetached();

    DocumentLifecycleNotifier::notifyDocumentWasDetached();
    m_lifecycle.advanceTo(DocumentLifecycle::Stopped);

    // FIXME: Currently we call notifyContextDestroyed() only in
    // Document::detach(), which means that we don't call
    // notifyContextDestroyed() for a document that doesn't get detached.
    // If such a document has any observer, the observer won't get
    // a contextDestroyed() notification. This can happen for a document
    // created by DOMImplementation::createDocument().
    DocumentLifecycleNotifier::notifyContextDestroyed();
    ExecutionContext::notifyContextDestroyed();
}

void Document::removeAllEventListeners()
{
    ContainerNode::removeAllEventListeners();

    if (LocalDOMWindow* domWindow = this->domWindow())
        domWindow->removeAllEventListeners();
}

Document& Document::axObjectCacheOwner() const
{
    Document& top = topDocument();
    if (top.frame() && top.frame()->pagePopupOwner()) {
        ASSERT(!top.m_axObjectCache);
        return top.frame()->pagePopupOwner()->document().axObjectCacheOwner();
    }
    return top;
}

void Document::clearAXObjectCache()
{
    ASSERT(&axObjectCacheOwner() == this);
    // Clear the cache member variable before calling delete because attempts
    // are made to access it during destruction.
    if (m_axObjectCache)
        m_axObjectCache->dispose();
    m_axObjectCache.clear();
}

AXObjectCache* Document::existingAXObjectCache() const
{
    // If the layoutObject is gone then we are in the process of destruction.
    // This method will be called before m_frame = nullptr.
    if (!axObjectCacheOwner().layoutView())
        return 0;

    return axObjectCacheOwner().m_axObjectCache.get();
}

AXObjectCache* Document::axObjectCache() const
{
    Settings* settings = this->settings();
    if (!settings || !settings->accessibilityEnabled())
        return 0;

    // The only document that actually has a AXObjectCache is the top-level
    // document.  This is because we need to be able to get from any WebCoreAXObject
    // to any other WebCoreAXObject on the same page.  Using a single cache allows
    // lookups across nested webareas (i.e. multiple documents).
    Document& cacheOwner = this->axObjectCacheOwner();

    // If the document has already been detached, do not make a new axObjectCache.
    if (!cacheOwner.layoutView())
        return 0;

    ASSERT(&cacheOwner == this || !m_axObjectCache);
    if (!cacheOwner.m_axObjectCache)
        cacheOwner.m_axObjectCache = AXObjectCache::create(cacheOwner);
    return cacheOwner.m_axObjectCache.get();
}

CanvasFontCache* Document::canvasFontCache()
{
    if (!m_canvasFontCache)
        m_canvasFontCache = CanvasFontCache::create(*this);

    return m_canvasFontCache.get();
}

PassRefPtrWillBeRawPtr<DocumentParser> Document::createParser()
{
    if (isHTMLDocument() /*|| isSVGDocument()*/) { // svg also go to HTMLDocumentParser, avoid libxml to load. by weolar
        bool reportErrors = InspectorInstrumentation::collectingHTMLParseErrors(this);
        return HTMLDocumentParser::create(toHTMLDocument(*this), reportErrors, m_parserSyncPolicy);
    }

    // FIXME: this should probably pass the frame instead
    return XMLDocumentParser::create(*this, view());
}

bool Document::isFrameSet() const
{
    if (!isHTMLDocument())
        return false;
    return isHTMLFrameSetElement(body());
}

ScriptableDocumentParser* Document::scriptableDocumentParser() const
{
    return parser() ? parser()->asScriptableDocumentParser() : 0;
}

void Document::open(Document* ownerDocument, ExceptionState& exceptionState)
{
    if (importLoader()) {
        exceptionState.throwDOMException(InvalidStateError, "Imported document doesn't support open().");
        return;
    }

    if (!isHTMLDocument()) {
        exceptionState.throwDOMException(InvalidStateError, "Only HTML documents support open().");
        return;
    }

    if (ownerDocument) {
        setURL(ownerDocument->url());
        m_cookieURL = ownerDocument->cookieURL();
        setSecurityOrigin(ownerDocument->securityOrigin());
    }

    open();
}

void Document::open()
{
    ASSERT(!importLoader());

    if (m_frame) {
        if (ScriptableDocumentParser* parser = scriptableDocumentParser()) {
            if (parser->isParsing()) {
                // FIXME: HTML5 doesn't tell us to check this, it might not be correct.
                if (parser->isExecutingScript())
                    return;

                if (!parser->wasCreatedByScript() && parser->hasInsertionPoint())
                    return;
            }
        }

        if (m_frame->loader().provisionalDocumentLoader())
            m_frame->loader().stopAllLoaders();
    }

    removeAllEventListenersRecursively();
    implicitOpen(ForceSynchronousParsing);
    if (ScriptableDocumentParser* parser = scriptableDocumentParser())
        parser->setWasCreatedByScript(true);

    if (m_frame)
        m_frame->loader().didExplicitOpen();
    if (m_loadEventProgress != LoadEventInProgress && m_loadEventProgress != UnloadEventInProgress)
        m_loadEventProgress = LoadEventNotRun;
}

void Document::detachParser()
{
    if (!m_parser)
        return;
    m_parser->detach();
    m_parser.clear();
}

void Document::cancelParsing()
{
    if (!m_parser)
        return;

    // We have to clear the parser to avoid possibly triggering
    // the onload handler when closing as a side effect of a cancel-style
    // change, such as opening a new document or closing the window while
    // still parsing.
    detachParser();
    explicitClose();
}

PassRefPtrWillBeRawPtr<DocumentParser> Document::implicitOpen(ParserSynchronizationPolicy parserSyncPolicy)
{
    cancelParsing();

    removeChildren();
    ASSERT(!m_focusedElement);

    setCompatibilityMode(NoQuirksMode);

    m_parserSyncPolicy = parserSyncPolicy;
    m_parser = createParser();
    setParsingState(Parsing);
    setReadyState(Loading);

    return m_parser;
}

HTMLElement* Document::body() const
{
    if (!documentElement())
        return 0;

    for (HTMLElement* child = Traversal<HTMLElement>::firstChild(*documentElement()); child; child = Traversal<HTMLElement>::nextSibling(*child)) {
        if (isHTMLFrameSetElement(*child) || isHTMLBodyElement(*child))
            return child;
    }

    return 0;
}

HTMLBodyElement* Document::firstBodyElement() const
{
    if (!documentElement())
        return 0;

    for (HTMLElement* child = Traversal<HTMLElement>::firstChild(*documentElement()); child; child = Traversal<HTMLElement>::nextSibling(*child)) {
        if (isHTMLBodyElement(*child))
            return toHTMLBodyElement(child);
    }

    return 0;
}

void Document::setBody(PassRefPtrWillBeRawPtr<HTMLElement> prpNewBody, ExceptionState& exceptionState)
{
    RefPtrWillBeRawPtr<HTMLElement> newBody = prpNewBody;

    if (!newBody) {
        exceptionState.throwDOMException(HierarchyRequestError, ExceptionMessages::argumentNullOrIncorrectType(1, "HTMLElement"));
        return;
    }
    if (!documentElement()) {
        exceptionState.throwDOMException(HierarchyRequestError, "No document element exists.");
        return;
    }

    if (!isHTMLBodyElement(*newBody) && !isHTMLFrameSetElement(*newBody)) {
        exceptionState.throwDOMException(HierarchyRequestError, "The new body element is of type '" + newBody->tagName() + "'. It must be either a 'BODY' or 'FRAMESET' element.");
        return;
    }

    HTMLElement* oldBody = body();
    if (oldBody == newBody)
        return;

    if (oldBody)
        documentElement()->replaceChild(newBody.release(), oldBody, exceptionState);
    else
        documentElement()->appendChild(newBody.release(), exceptionState);
}

HTMLHeadElement* Document::head() const
{
    Node* de = documentElement();
    if (!de)
        return 0;

    return Traversal<HTMLHeadElement>::firstChild(*de);
}

Element* Document::viewportDefiningElement(const ComputedStyle* rootStyle) const
{
    // If a BODY element sets non-visible overflow, it is to be propagated to the viewport, as long
    // as the following conditions are all met:
    // (1) The root element is HTML.
    // (2) It is the primary BODY element (we only assert for this, expecting callers to behave).
    // (3) The root element has visible overflow.
    // Otherwise it's the root element's properties that are to be propagated.
    Element* rootElement = documentElement();
    Element* bodyElement = body();
    if (!rootElement)
        return 0;
    if (!rootStyle) {
        rootStyle = rootElement->computedStyle();
        if (!rootStyle)
            return 0;
    }
    if (bodyElement && rootStyle->isOverflowVisible() && isHTMLHtmlElement(*rootElement))
        return bodyElement;
    return rootElement;
}

void Document::close(ExceptionState& exceptionState)
{
    // FIXME: We should follow the specification more closely:
    //        http://www.whatwg.org/specs/web-apps/current-work/#dom-document-close

    if (importLoader()) {
        exceptionState.throwDOMException(InvalidStateError, "Imported document doesn't support close().");
        return;
    }

    if (!isHTMLDocument()) {
        exceptionState.throwDOMException(InvalidStateError, "Only HTML documents support close().");
        return;
    }

    close();
}

void Document::close()
{
    if (!scriptableDocumentParser() || !scriptableDocumentParser()->wasCreatedByScript() || !scriptableDocumentParser()->isParsing())
        return;

    explicitClose();
}

void Document::explicitClose()
{
    if (RefPtrWillBeRawPtr<DocumentParser> parser = m_parser)
        parser->finish();

    if (!m_frame) {
        // Because we have no frame, we don't know if all loading has completed,
        // so we just call implicitClose() immediately. FIXME: This might fire
        // the load event prematurely <http://bugs.webkit.org/show_bug.cgi?id=14568>.
        implicitClose();
        return;
    }

    m_frame->loader().checkCompleted();
}

void Document::implicitClose()
{
    ASSERT(!inStyleRecalc());
    if (processingLoadEvent() || !m_parser)
        return;
    if (frame() && frame()->navigationScheduler().locationChangePending()) {
        suppressLoadEvent();
        return;
    }

    // The call to dispatchWindowLoadEvent can detach the LocalDOMWindow and cause it (and its
    // attached Document) to be destroyed.
    RefPtrWillBeRawPtr<LocalDOMWindow> protectedWindow(this->domWindow());

    m_loadEventProgress = LoadEventInProgress;

    ScriptableDocumentParser* parser = scriptableDocumentParser();
    m_wellFormed = parser && parser->wellFormed();

    // We have to clear the parser, in case someone document.write()s from the
    // onLoad event handler, as in Radar 3206524.
    detachParser();

    if (frame() && frame()->script().canExecuteScripts(NotAboutToExecuteScript)) {
        ImageLoader::dispatchPendingLoadEvents();
        ImageLoader::dispatchPendingErrorEvents();

        HTMLLinkElement::dispatchPendingLoadEvents();
        HTMLStyleElement::dispatchPendingLoadEvents();
    }

    // JS running below could remove the frame or destroy the LayoutView so we call
    // those two functions repeatedly and don't save them on the stack.

    // To align the HTML load event and the SVGLoad event for the outermost <svg> element, fire it from
    // here, instead of doing it from SVGElement::finishedParsingChildren.
    if (svgExtensions())
        accessSVGExtensions().dispatchSVGLoadEventToOutermostSVGElements();

    if (protectedWindow)
        protectedWindow->documentWasClosed();

    if (frame()) {
        frame()->loader().client()->dispatchDidHandleOnloadEvents();
        loader()->applicationCacheHost()->stopDeferringEvents();
    }

    if (!frame()) {
        m_loadEventProgress = LoadEventCompleted;
        return;
    }

    // Make sure both the initial layout and reflow happen after the onload
    // fires. This will improve onload scores, and other browsers do it.
    // If they wanna cheat, we can too. -dwh

    if (frame()->navigationScheduler().locationChangePending() && elapsedTime() < cLayoutScheduleThreshold) {
        // Just bail out. Before or during the onload we were shifted to another page.
        // The old i-Bench suite does this. When this happens don't bother painting or laying out.
        m_loadEventProgress = LoadEventCompleted;
        return;
    }

    // We used to force a synchronous display and flush here.  This really isn't
    // necessary and can in fact be actively harmful if pages are loading at a rate of > 60fps
    // (if your platform is syncing flushes and limiting them to 60fps).
    if (!ownerElement() || (ownerElement()->layoutObject() && !ownerElement()->layoutObject()->needsLayout())) {
        updateLayoutTreeIfNeeded();

        // Always do a layout after loading if needed.
        if (view() && layoutView() && (!layoutView()->firstChild() || layoutView()->needsLayout()))
            view()->layout();
    }

    m_loadEventProgress = LoadEventCompleted;

    if (frame() && layoutView() && settings()->accessibilityEnabled()) {
        if (AXObjectCache* cache = axObjectCache()) {
            if (this == &axObjectCacheOwner())
                cache->handleLoadComplete(this);
            else
                cache->handleLayoutComplete(this);
        }
    }

    if (svgExtensions())
        accessSVGExtensions().startAnimations();
}

bool Document::dispatchBeforeUnloadEvent(ChromeClient& chromeClient, bool& didAllowNavigation)
{
    if (!m_domWindow)
        return true;

    if (!body())
        return true;

    if (processingBeforeUnload())
        return false;

    RefPtrWillBeRawPtr<Document> protect(this);

    RefPtrWillBeRawPtr<BeforeUnloadEvent> beforeUnloadEvent = BeforeUnloadEvent::create();
    m_loadEventProgress = BeforeUnloadEventInProgress;
    m_domWindow->dispatchEvent(beforeUnloadEvent.get(), this);
    m_loadEventProgress = BeforeUnloadEventCompleted;
    if (!beforeUnloadEvent->defaultPrevented())
        defaultEventHandler(beforeUnloadEvent.get());
    if (!frame() || beforeUnloadEvent->returnValue().isNull())
        return true;

    if (didAllowNavigation) {
        addConsoleMessage(ConsoleMessage::create(JSMessageSource, ErrorMessageLevel, "Blocked attempt to show multiple 'beforeunload' confirmation panels for a single navigation."));
        return true;
    }

    String text = beforeUnloadEvent->returnValue();
    if (chromeClient.openBeforeUnloadConfirmPanel(text, m_frame)) {
        didAllowNavigation = true;
        return true;
    }
    return false;
}

void Document::dispatchUnloadEvents()
{
    PluginScriptForbiddenScope forbidPluginDestructorScripting;
    RefPtrWillBeRawPtr<Document> protect(this);
    if (m_parser)
        m_parser->stopParsing();

    if (m_loadEventProgress == LoadEventNotRun)
        return;

    if (m_loadEventProgress <= UnloadEventInProgress) {
        Element* currentFocusedElement = focusedElement();
        if (isHTMLInputElement(currentFocusedElement))
            toHTMLInputElement(*currentFocusedElement).endEditing();
        if (m_loadEventProgress < PageHideInProgress) {
            m_loadEventProgress = PageHideInProgress;
            if (LocalDOMWindow* window = domWindow())
                window->dispatchEvent(PageTransitionEvent::create(EventTypeNames::pagehide, false), this);
            if (!m_frame)
                return;

            // The DocumentLoader (and thus its DocumentLoadTiming) might get destroyed
            // while dispatching the event, so protect it to prevent writing the end
            // time into freed memory.
            RefPtrWillBeRawPtr<DocumentLoader> documentLoader = m_frame->loader().provisionalDocumentLoader();
            m_loadEventProgress = UnloadEventInProgress;
            RefPtrWillBeRawPtr<Event> unloadEvent(Event::create(EventTypeNames::unload));
            if (documentLoader && !documentLoader->timing().unloadEventStart() && !documentLoader->timing().unloadEventEnd()) {
                DocumentLoadTiming& timing = documentLoader->timing();
                ASSERT(timing.navigationStart());
                timing.markUnloadEventStart();
                m_frame->localDOMWindow()->dispatchEvent(unloadEvent, this); // weolar:不发送卸载消息，防止脚本再去取navigator.userAgent发生崩溃
                timing.markUnloadEventEnd();
            } else {
                m_frame->localDOMWindow()->dispatchEvent(unloadEvent, m_frame->document()); // weolar
            }
        }
        m_loadEventProgress = UnloadEventHandled;
    }

    if (!m_frame)
        return;

    // Don't remove event listeners from a transitional empty document (see https://bugs.webkit.org/show_bug.cgi?id=28716 for more information).
    bool keepEventListeners = m_frame->loader().stateMachine()->isDisplayingInitialEmptyDocument() && m_frame->loader().provisionalDocumentLoader()
        && isSecureTransitionTo(m_frame->loader().provisionalDocumentLoader()->url());
    if (!keepEventListeners)
        removeAllEventListenersRecursively();
}

Document::PageDismissalType Document::pageDismissalEventBeingDispatched() const
{
    if (m_loadEventProgress == BeforeUnloadEventInProgress)
        return BeforeUnloadDismissal;
    if (m_loadEventProgress == PageHideInProgress)
        return PageHideDismissal;
    if (m_loadEventProgress == UnloadEventInProgress)
        return UnloadDismissal;
    return NoDismissal;
}

void Document::setParsingState(ParsingState parsingState)
{
    m_parsingState = parsingState;

    if (parsing() && !m_elementDataCache)
        m_elementDataCache = ElementDataCache::create();
}

bool Document::shouldScheduleLayout() const
{
    // This function will only be called when FrameView thinks a layout is needed.
    // This enforces a couple extra rules.
    //
    //    (a) Only schedule a layout once the stylesheets are loaded.
    //    (b) Only schedule layout once we have a body element.
    if (!isActive())
        return false;

    if (isRenderingReady() && body())
        return true;

    if (documentElement() && !isHTMLHtmlElement(*documentElement()))
        return true;

    return false;
}

int Document::elapsedTime() const
{
    return static_cast<int>((currentTime() - m_startTime) * 1000);
}

void Document::write(const SegmentedString& text, Document* ownerDocument, ExceptionState& exceptionState)
{
    if (importLoader()) {
        exceptionState.throwDOMException(InvalidStateError, "Imported document doesn't support write().");
        return;
    }

    if (!isHTMLDocument()) {
        exceptionState.throwDOMException(InvalidStateError, "Only HTML documents support write().");
        return;
    }

    NestingLevelIncrementer nestingLevelIncrementer(m_writeRecursionDepth);

    m_writeRecursionIsTooDeep = (m_writeRecursionDepth > 1) && m_writeRecursionIsTooDeep;
    m_writeRecursionIsTooDeep = (m_writeRecursionDepth > cMaxWriteRecursionDepth) || m_writeRecursionIsTooDeep;

    if (m_writeRecursionIsTooDeep)
        return;

    bool hasInsertionPoint = m_parser && m_parser->hasInsertionPoint();

    if (!hasInsertionPoint && m_ignoreDestructiveWriteCount) {
        addConsoleMessage(ConsoleMessage::create(JSMessageSource, WarningMessageLevel, ExceptionMessages::failedToExecute("write", "Document", "It isn't possible to write into a document from an asynchronously-loaded external script unless it is explicitly opened.")));
        return;
    }

    if (!hasInsertionPoint)
        open(ownerDocument, ASSERT_NO_EXCEPTION);

    ASSERT(m_parser);
    m_parser->insert(text);
}

void Document::write(const String& text, Document* ownerDocument, ExceptionState& exceptionState)
{
    write(SegmentedString(text), ownerDocument, exceptionState);
}

void Document::writeln(const String& text, Document* ownerDocument, ExceptionState& exceptionState)
{
    write(text, ownerDocument, exceptionState);
    if (exceptionState.hadException())
        return;
    write("\n", ownerDocument);
}

void Document::write(LocalDOMWindow* callingWindow, const Vector<String>& text, ExceptionState& exceptionState)
{
    ASSERT(callingWindow);
    StringBuilder builder;
    for (const String& string : text)
        builder.append(string);
    write(builder.toString(), callingWindow->document(), exceptionState);
}

void Document::writeln(LocalDOMWindow* callingWindow, const Vector<String>& text, ExceptionState& exceptionState)
{
    ASSERT(callingWindow);
    StringBuilder builder;
    for (const String& string : text)
        builder.append(string);
    writeln(builder.toString(), callingWindow->document(), exceptionState);
}

const KURL& Document::virtualURL() const
{
    return m_url;
}

KURL Document::virtualCompleteURL(const String& url) const
{
    return completeURL(url);
}

double Document::timerAlignmentInterval() const
{
    Page* p = page();
    if (!p)
        return DOMTimer::visiblePageAlignmentInterval();
    return p->timerAlignmentInterval();
}

DOMTimerCoordinator* Document::timers()
{
    return &m_timers;
}

EventTarget* Document::errorEventTarget()
{
    return domWindow();
}

void Document::logExceptionToConsole(const String& errorMessage, int scriptId, const String& sourceURL, int lineNumber, int columnNumber, PassRefPtrWillBeRawPtr<ScriptCallStack> callStack)
{
    RefPtrWillBeRawPtr<ConsoleMessage> consoleMessage = ConsoleMessage::create(JSMessageSource, ErrorMessageLevel, errorMessage, sourceURL, lineNumber);
    consoleMessage->setScriptId(scriptId);
    consoleMessage->setCallStack(callStack);
    addConsoleMessage(consoleMessage.release());
}

void Document::setURL(const KURL& url)
{
    const KURL& newURL = url.isEmpty() ? blankURL() : url;
    if (newURL == m_url)
        return;

    m_url = newURL;
    updateBaseURL();
    contextFeatures().urlDidChange(this);
}

void Document::updateBaseURL()
{
    KURL oldBaseURL = m_baseURL;
    // DOM 3 Core: When the Document supports the feature "HTML" [DOM Level 2 HTML], the base URI is computed using
    // first the value of the href attribute of the HTML BASE element if any, and the value of the documentURI attribute
    // from the Document interface otherwise (which we store, preparsed, in m_url).
    if (!m_baseElementURL.isEmpty())
        m_baseURL = m_baseElementURL;
    else if (!m_baseURLOverride.isEmpty())
        m_baseURL = m_baseURLOverride;
    else
        m_baseURL = m_url;

    selectorQueryCache().invalidate();

    if (!m_baseURL.isValid())
        m_baseURL = KURL();

    if (m_elemSheet) {
        // Element sheet is silly. It never contains anything.
        ASSERT(!m_elemSheet->contents()->ruleCount());
        bool usesRemUnits = m_elemSheet->contents()->usesRemUnits();
        m_elemSheet = CSSStyleSheet::createInline(this, m_baseURL);
        // FIXME: So we are not really the parser. The right fix is to eliminate the element sheet completely.
        m_elemSheet->contents()->parserSetUsesRemUnits(usesRemUnits);
    }

    if (!equalIgnoringFragmentIdentifier(oldBaseURL, m_baseURL)) {
        // Base URL change changes any relative visited links.
        // FIXME: There are other URLs in the tree that would need to be re-evaluated on dynamic base URL change. Style should be invalidated too.
        for (HTMLAnchorElement& anchor : Traversal<HTMLAnchorElement>::startsAfter(*this))
            anchor.invalidateCachedVisitedLinkHash();
    }
}

void Document::setBaseURLOverride(const KURL& url)
{
    m_baseURLOverride = url;
    updateBaseURL();
}

void Document::processBaseElement()
{
    // Find the first href attribute in a base element and the first target attribute in a base element.
    const AtomicString* href = 0;
    const AtomicString* target = 0;
    for (HTMLBaseElement* base = Traversal<HTMLBaseElement>::firstWithin(*this); base && (!href || !target); base = Traversal<HTMLBaseElement>::next(*base)) {
        if (!href) {
            const AtomicString& value = base->fastGetAttribute(hrefAttr);
            if (!value.isNull())
                href = &value;
        }
        if (!target) {
            const AtomicString& value = base->fastGetAttribute(targetAttr);
            if (!value.isNull())
                target = &value;
        }
        if (contentSecurityPolicy()->isActive())
            UseCounter::count(*this, UseCounter::ContentSecurityPolicyWithBaseElement);
    }

    // FIXME: Since this doesn't share code with completeURL it may not handle encodings correctly.
    KURL baseElementURL;
    if (href) {
        String strippedHref = stripLeadingAndTrailingHTMLSpaces(*href);
        if (!strippedHref.isEmpty())
            baseElementURL = KURL(url(), strippedHref);
    }
    if (m_baseElementURL != baseElementURL && contentSecurityPolicy()->allowBaseURI(baseElementURL)) {
        m_baseElementURL = baseElementURL;
        updateBaseURL();
    }

    m_baseTarget = target ? *target : nullAtom;
}

String Document::userAgent(const KURL& url) const
{
    return frame() ? frame()->loader().userAgent(url) : String();
}

void Document::disableEval(const String& errorMessage)
{
    if (!frame())
        return;

    frame()->script().disableEval(errorMessage);
}


void Document::didLoadAllImports()
{
    if (!haveStylesheetsLoaded())
        return;
    if (!importLoader())
        styleResolverMayHaveChanged();
    didLoadAllScriptBlockingResources();
}

void Document::didRemoveAllPendingStylesheet()
{
    styleResolverMayHaveChanged();

    // Only imports on master documents can trigger rendering.
    if (HTMLImportLoader* import = importLoader())
        import->didRemoveAllPendingStylesheet();
    if (!haveImportsLoaded())
        return;
    didLoadAllScriptBlockingResources();
}

void Document::didLoadAllScriptBlockingResources()
{
    m_executeScriptsWaitingForResourcesTimer.startOneShot(0, FROM_HERE);

    if (frame())
        frame()->loader().client()->didRemoveAllPendingStylesheet();

    if (m_gotoAnchorNeededAfterStylesheetsLoad && view())
        view()->processUrlFragment(m_url);
}

void Document::executeScriptsWaitingForResourcesTimerFired(Timer<Document>*)
{
    if (!isRenderingReady())
        return;
    if (ScriptableDocumentParser* parser = scriptableDocumentParser())
        parser->executeScriptsWaitingForResources();
}

CSSStyleSheet& Document::elementSheet()
{
    if (!m_elemSheet)
        m_elemSheet = CSSStyleSheet::createInline(this, m_baseURL);
    return *m_elemSheet;
}

void Document::processHttpEquiv(const AtomicString& equiv, const AtomicString& content, bool inDocumentHeadElement)
{
    ASSERT(!equiv.isNull() && !content.isNull());

    if (equalIgnoringCase(equiv, "default-style")) {
        processHttpEquivDefaultStyle(content);
    } else if (equalIgnoringCase(equiv, "refresh")) {
        processHttpEquivRefresh(content);
    } else if (equalIgnoringCase(equiv, "set-cookie")) {
        processHttpEquivSetCookie(content);
    } else if (equalIgnoringCase(equiv, "content-language")) {
        setContentLanguage(content);
    } else if (equalIgnoringCase(equiv, "x-dns-prefetch-control")) {
        parseDNSPrefetchControlHeader(content);
    } else if (equalIgnoringCase(equiv, "x-frame-options")) {
        processHttpEquivXFrameOptions(content);
    } else if (equalIgnoringCase(equiv, "accept-ch")) {
        processHttpEquivAcceptCH(content);
    } else if (equalIgnoringCase(equiv, "content-security-policy") || equalIgnoringCase(equiv, "content-security-policy-report-only")) {
        if (inDocumentHeadElement)
            processHttpEquivContentSecurityPolicy(equiv, content);
        else
            contentSecurityPolicy()->reportMetaOutsideHead(content);
    }
}

void Document::processHttpEquivContentSecurityPolicy(const AtomicString& equiv, const AtomicString& content)
{
    if (importLoader())
        return;
    if (equalIgnoringCase(equiv, "content-security-policy"))
        contentSecurityPolicy()->didReceiveHeader(content, ContentSecurityPolicyHeaderTypeEnforce, ContentSecurityPolicyHeaderSourceMeta);
    else if (equalIgnoringCase(equiv, "content-security-policy-report-only"))
        contentSecurityPolicy()->didReceiveHeader(content, ContentSecurityPolicyHeaderTypeReport, ContentSecurityPolicyHeaderSourceMeta);
    else
        ASSERT_NOT_REACHED();
}

void Document::processHttpEquivAcceptCH(const AtomicString& content)
{
    if (frame())
        handleAcceptClientHintsHeader(content, m_clientHintsPreferences, fetcher());
}

void Document::processHttpEquivDefaultStyle(const AtomicString& content)
{
    // The preferred style set has been overridden as per section
    // 14.3.2 of the HTML4.0 specification. We need to update the
    // sheet used variable and then update our style selector.
    // For more info, see the test at:
    // http://www.hixie.ch/tests/evil/css/import/main/preferred.html
    // -dwh
    styleEngine().setSelectedStylesheetSetName(content);
    styleEngine().setPreferredStylesheetSetName(content);
    styleResolverChanged();
}

void Document::processHttpEquivRefresh(const AtomicString& content)
{
    maybeHandleHttpRefresh(content, HttpRefreshFromMetaTag);
}

void Document::maybeHandleHttpRefresh(const String& content, HttpRefreshType httpRefreshType)
{
    if (m_isViewSource || !m_frame)
        return;

    double delay;
    String refreshURL;
    if (!parseHTTPRefresh(content, httpRefreshType == HttpRefreshFromMetaTag, delay, refreshURL))
        return;
    if (refreshURL.isEmpty())
        refreshURL = url().string();
    else
        refreshURL = completeURL(refreshURL).string();

    if (protocolIsJavaScript(refreshURL)) {
        String message = "Refused to refresh " + m_url.elidedString() + " to a javascript: URL";
        addConsoleMessage(ConsoleMessage::create(SecurityMessageSource, ErrorMessageLevel, message));
        return;
    }

    if (httpRefreshType == HttpRefreshFromMetaTag && isSandboxed(SandboxAutomaticFeatures)) {
        String message = "Refused to execute the redirect specified via '<meta http-equiv='refresh' content='...'>'. The document is sandboxed, and the 'allow-scripts' keyword is not set.";
        addConsoleMessage(ConsoleMessage::create(SecurityMessageSource, ErrorMessageLevel, message));
        return;
    }
    m_frame->navigationScheduler().scheduleRedirect(delay, refreshURL);
}

void Document::processHttpEquivSetCookie(const AtomicString& content)
{
    // FIXME: make setCookie work on XML documents too; e.g. in case of <html:meta .....>
    if (!isHTMLDocument())
        return;

    // Exception (for sandboxed documents) ignored.
    toHTMLDocument(this)->setCookie(content, IGNORE_EXCEPTION);
}

void Document::processHttpEquivXFrameOptions(const AtomicString& content)
{
    if (!m_frame)
        return;

    unsigned long requestIdentifier = loader()->mainResourceIdentifier();
    if (m_frame->loader().shouldInterruptLoadForXFrameOptions(content, url(), requestIdentifier)) {
        String message = "Refused to display '" + url().elidedString() + "' in a frame because it set 'X-Frame-Options' to '" + content + "'.";
        m_frame->loader().stopAllLoaders();
        if (!m_frame)
            return;
        // Stopping the loader isn't enough, as we're already parsing the document; to honor the header's
        // intent, we must navigate away from the possibly partially-rendered document to a location that
        // doesn't inherit the parent's SecurityOrigin.
        m_frame->navigate(*this, SecurityOrigin::urlWithUniqueSecurityOrigin(), true, UserGestureStatus::None);
        RefPtrWillBeRawPtr<ConsoleMessage> consoleMessage = ConsoleMessage::create(SecurityMessageSource, ErrorMessageLevel, message);
        consoleMessage->setRequestIdentifier(requestIdentifier);
        addConsoleMessage(consoleMessage.release());
    }
}

bool Document::shouldMergeWithLegacyDescription(ViewportDescription::Type origin)
{
    return settings() && settings()->viewportMetaMergeContentQuirk() && m_legacyViewportDescription.isMetaViewportType() && m_legacyViewportDescription.type == origin;
}

void Document::setViewportDescription(const ViewportDescription& viewportDescription)
{
    // The UA-defined min-width is used by the processing of legacy meta tags.
    if (!viewportDescription.isSpecifiedByAuthor())
        m_viewportDefaultMinWidth = viewportDescription.minWidth;

    if (viewportDescription.isLegacyViewportType()) {
        if (settings() && !settings()->viewportMetaEnabled())
            return;

        m_legacyViewportDescription = viewportDescription;

        // When no author style for @viewport is present, and a meta tag for defining
        // the viewport is, apply the meta tag viewport instead of the UA styles.
        if (m_viewportDescription.type == ViewportDescription::AuthorStyleSheet)
            return;
        m_viewportDescription = viewportDescription;
    } else {
        // If the legacy viewport tag has higher priority than the cascaded @viewport
        // descriptors, use the values from the legacy tag.
        if (!shouldOverrideLegacyDescription(viewportDescription.type))
            m_viewportDescription = m_legacyViewportDescription;
        else
            m_viewportDescription = viewportDescription;
    }

    updateViewportDescription();
}

void Document::updateViewportDescription()
{
    if (frame() && frame()->isMainFrame()) {
        frameHost()->chromeClient().dispatchViewportPropertiesDidChange(m_viewportDescription);
    }
}

void Document::processReferrerPolicy(const String& policy)
{
    ASSERT(!policy.isNull());

    if (equalIgnoringCase(policy, "no-referrer") || equalIgnoringCase(policy, "never"))
        setReferrerPolicy(ReferrerPolicyNever);
    else if (equalIgnoringCase(policy, "unsafe-url") || equalIgnoringCase(policy, "always"))
        setReferrerPolicy(ReferrerPolicyAlways);
    else if (equalIgnoringCase(policy, "origin"))
        setReferrerPolicy(ReferrerPolicyOrigin);
    else if (equalIgnoringCase(policy, "origin-when-cross-origin") || equalIgnoringCase(policy, "origin-when-crossorigin"))
        setReferrerPolicy(ReferrerPolicyOriginWhenCrossOrigin);
    else if (equalIgnoringCase(policy, "no-referrer-when-downgrade") || equalIgnoringCase(policy, "default"))
        setReferrerPolicy(ReferrerPolicyNoReferrerWhenDowngrade);
    else
        addConsoleMessage(ConsoleMessage::create(RenderingMessageSource, ErrorMessageLevel, "Failed to set referrer policy: The value '" + policy + "' is not one of 'always', 'default', 'never', 'no-referrer', 'no-referrer-when-downgrade', 'origin', 'origin-when-crossorigin', or 'unsafe-url'. This document's referrer policy has been left unchanged."));
}

String Document::outgoingReferrer()
{
    // See http://www.whatwg.org/specs/web-apps/current-work/#fetching-resources
    // for why we walk the parent chain for srcdoc documents.
    Document* referrerDocument = this;
    if (LocalFrame* frame = m_frame) {
        while (frame->document()->isSrcdocDocument()) {
            // Srcdoc documents must be local within the containing frame.
            frame = toLocalFrame(frame->tree().parent());
            // Srcdoc documents cannot be top-level documents, by definition,
            // because they need to be contained in iframes with the srcdoc.
            ASSERT(frame);
        }
        referrerDocument = frame->document();
    }
    return referrerDocument->m_url.strippedForUseAsReferrer();
}

String Document::outgoingOrigin() const
{
    return securityOrigin()->toString();
}

MouseEventWithHitTestResults Document::prepareMouseEvent(const HitTestRequest& request, const LayoutPoint& documentPoint, const PlatformMouseEvent& event)
{
    ASSERT(!layoutView() || layoutView()->isLayoutView());

    // LayoutView::hitTest causes a layout, and we don't want to hit that until the first
    // layout because until then, there is nothing shown on the screen - the user can't
    // have intentionally clicked on something belonging to this page. Furthermore,
    // mousemove events before the first layout should not lead to a premature layout()
    // happening, which could show a flash of white.
    // See also the similar code in EventHandler::hitTestResultAtPoint.
    if (!layoutView() || !view() || !view()->didFirstLayout())
        return MouseEventWithHitTestResults(event, HitTestResult(request, LayoutPoint()));

    HitTestResult result(request, documentPoint);
    layoutView()->hitTest(result);

    if (!request.readOnly())
        updateHoverActiveState(request, result.innerElement());

    return MouseEventWithHitTestResults(event, result);
}

// DOM Section 1.1.1
bool Document::childTypeAllowed(NodeType type) const
{
    switch (type) {
    case ATTRIBUTE_NODE:
    case CDATA_SECTION_NODE:
    case DOCUMENT_FRAGMENT_NODE:
    case DOCUMENT_NODE:
    case TEXT_NODE:
        return false;
    case COMMENT_NODE:
    case PROCESSING_INSTRUCTION_NODE:
        return true;
    case DOCUMENT_TYPE_NODE:
    case ELEMENT_NODE:
        // Documents may contain no more than one of each of these.
        // (One Element and one DocumentType.)
        for (Node& c : NodeTraversal::childrenOf(*this)) {
            if (c.nodeType() == type)
                return false;
        }
        return true;
    }
    return false;
}

bool Document::canAcceptChild(const Node& newChild, const Node* oldChild, ExceptionState& exceptionState) const
{
    if (oldChild && oldChild->nodeType() == newChild.nodeType())
        return true;

    int numDoctypes = 0;
    int numElements = 0;

    // First, check how many doctypes and elements we have, not counting
    // the child we're about to remove.
    for (Node& child : NodeTraversal::childrenOf(*this)) {
        if (oldChild && *oldChild == child)
            continue;

        switch (child.nodeType()) {
        case DOCUMENT_TYPE_NODE:
            numDoctypes++;
            break;
        case ELEMENT_NODE:
            numElements++;
            break;
        default:
            break;
        }
    }

    // Then, see how many doctypes and elements might be added by the new child.
    if (newChild.isDocumentFragment()) {
        for (Node& child : NodeTraversal::childrenOf(toDocumentFragment(newChild))) {
            switch (child.nodeType()) {
            case ATTRIBUTE_NODE:
            case CDATA_SECTION_NODE:
            case DOCUMENT_FRAGMENT_NODE:
            case DOCUMENT_NODE:
            case TEXT_NODE:
                exceptionState.throwDOMException(HierarchyRequestError, "Nodes of type '" + newChild.nodeName() +
                    "' may not be inserted inside nodes of type '#document'.");
                return false;
            case COMMENT_NODE:
            case PROCESSING_INSTRUCTION_NODE:
                break;
            case DOCUMENT_TYPE_NODE:
                numDoctypes++;
                break;
            case ELEMENT_NODE:
                numElements++;
                break;
            }
        }
    } else {
        switch (newChild.nodeType()) {
        case ATTRIBUTE_NODE:
        case CDATA_SECTION_NODE:
        case DOCUMENT_FRAGMENT_NODE:
        case DOCUMENT_NODE:
        case TEXT_NODE:
            exceptionState.throwDOMException(HierarchyRequestError, "Nodes of type '" + newChild.nodeName() +
                "' may not be inserted inside nodes of type '#document'.");
            return false;
        case COMMENT_NODE:
        case PROCESSING_INSTRUCTION_NODE:
            return true;
        case DOCUMENT_TYPE_NODE:
            numDoctypes++;
            break;
        case ELEMENT_NODE:
            numElements++;
            break;
        }
    }

    if (numElements > 1 || numDoctypes > 1) {
        exceptionState.throwDOMException(HierarchyRequestError,
            String::format("Only one %s on document allowed.",
            numElements > 1 ? "element" : "doctype"));
        return false;
    }

    return true;
}

PassRefPtrWillBeRawPtr<Node> Document::cloneNode(bool deep)
{
    RefPtrWillBeRawPtr<Document> clone = cloneDocumentWithoutChildren();
    clone->cloneDataFromDocument(*this);
    if (deep)
        cloneChildNodes(clone.get());
    return clone.release();
}

PassRefPtrWillBeRawPtr<Document> Document::cloneDocumentWithoutChildren()
{
    DocumentInit init(url());
    if (isXMLDocument()) {
        if (isXHTMLDocument())
            return XMLDocument::createXHTML(init.withRegistrationContext(registrationContext()));
        return XMLDocument::create(init);
    }
    return create(init);
}

void Document::cloneDataFromDocument(const Document& other)
{
    setCompatibilityMode(other.compatibilityMode());
    setEncodingData(other.m_encodingData);
    setContextFeatures(other.contextFeatures());
    setSecurityOrigin(other.securityOrigin()->isolatedCopy());
    setMimeType(other.contentType());
}

StyleSheetList* Document::styleSheets()
{
    if (!m_styleSheetList)
        m_styleSheetList = StyleSheetList::create(this);
    return m_styleSheetList.get();
}

String Document::preferredStylesheetSet() const
{
    return m_styleEngine->preferredStylesheetSetName();
}

String Document::selectedStylesheetSet() const
{
    return m_styleEngine->selectedStylesheetSetName();
}

void Document::setSelectedStylesheetSet(const String& aString)
{
    styleEngine().setSelectedStylesheetSetName(aString);
    styleResolverChanged();
}

void Document::evaluateMediaQueryListIfNeeded()
{
    if (!m_evaluateMediaQueriesOnStyleRecalc)
        return;
    evaluateMediaQueryList();
    m_evaluateMediaQueriesOnStyleRecalc = false;
}

void Document::evaluateMediaQueryList()
{
    if (m_mediaQueryMatcher)
        m_mediaQueryMatcher->mediaFeaturesChanged();
}

void Document::notifyResizeForViewportUnits()
{
    if (m_mediaQueryMatcher)
        m_mediaQueryMatcher->viewportChanged();
    if (!hasViewportUnits())
        return;
    ensureStyleResolver().notifyResizeForViewportUnits();
    setNeedsStyleRecalcForViewportUnits();
}

void Document::styleResolverChanged(StyleResolverUpdateMode updateMode)
{
    styleEngine().resolverChanged(updateMode);

    if (didLayoutWithPendingStylesheets() && !styleEngine().hasPendingSheets()) {
        // We need to manually repaint because we avoid doing all repaints in layout or style
        // recalc while sheets are still loading to avoid FOUC.
        m_pendingSheetLayout = IgnoreLayoutWithPendingSheets;

        ASSERT(layoutView() || importsController());
        if (layoutView())
            layoutView()->invalidatePaintForViewAndCompositedLayers();
    }
}

void Document::styleResolverMayHaveChanged()
{
    styleResolverChanged(hasNodesWithPlaceholderStyle() ? FullStyleUpdate : AnalyzedStyleUpdate);
}

void Document::setHoverNode(PassRefPtrWillBeRawPtr<Node> newHoverNode)
{
    m_hoverNode = newHoverNode;
}

void Document::setActiveHoverElement(PassRefPtrWillBeRawPtr<Element> newActiveElement)
{
    if (!newActiveElement) {
        m_activeHoverElement.clear();
        return;
    }

    m_activeHoverElement = newActiveElement;
}

void Document::removeFocusedElementOfSubtree(Node* node, bool amongChildrenOnly)
{
    if (!m_focusedElement)
        return;

    // We can't be focused if we're not in the document.
    if (!node->inDocument())
        return;
    bool contains = node->containsIncludingShadowDOM(m_focusedElement.get());
    if (contains && (m_focusedElement != node || !amongChildrenOnly))
        setFocusedElement(nullptr);
}

void Document::hoveredNodeDetached(Element& element)
{
    if (!m_hoverNode)
        return;

    m_hoverNode->updateDistribution();
    if (element != m_hoverNode && (!m_hoverNode->isTextNode() || element != ComposedTreeTraversal::parent(*m_hoverNode)))
        return;

    m_hoverNode = ComposedTreeTraversal::parent(element);
    while (m_hoverNode && !m_hoverNode->layoutObject())
        m_hoverNode = ComposedTreeTraversal::parent(*m_hoverNode);

    // If the mouse cursor is not visible, do not clear existing
    // hover effects on the ancestors of |element| and do not invoke
    // new hover effects on any other element.
    if (!page()->isCursorVisible())
        return;

    if (frame())
        frame()->eventHandler().scheduleHoverStateUpdate();
}

void Document::activeChainNodeDetached(Element& element)
{
    if (!m_activeHoverElement)
        return;

    if (element != m_activeHoverElement)
        return;

    Node* activeNode = ComposedTreeTraversal::parent(element);
    while (activeNode && activeNode->isElementNode() && !activeNode->layoutObject())
        activeNode = ComposedTreeTraversal::parent(*activeNode);

    m_activeHoverElement = activeNode && activeNode->isElementNode() ? toElement(activeNode) : 0;
}

const Vector<AnnotatedRegionValue>& Document::annotatedRegions() const
{
    return m_annotatedRegions;
}

void Document::setAnnotatedRegions(const Vector<AnnotatedRegionValue>& regions)
{
    m_annotatedRegions = regions;
    setAnnotatedRegionsDirty(false);
}

bool Document::setFocusedElement(PassRefPtrWillBeRawPtr<Element> prpNewFocusedElement, WebFocusType type)
{
    ASSERT(!m_lifecycle.inDetach());

    m_clearFocusedElementTimer.stop();

    RefPtrWillBeRawPtr<Element> newFocusedElement = prpNewFocusedElement;

    // Make sure newFocusedNode is actually in this document
    if (newFocusedElement && (newFocusedElement->document() != this))
        return true;

    if (NodeChildRemovalTracker::isBeingRemoved(newFocusedElement.get()))
        return true;

    if (m_focusedElement == newFocusedElement)
        return true;

    bool focusChangeBlocked = false;
    RefPtrWillBeRawPtr<Element> oldFocusedElement = m_focusedElement;
    m_focusedElement = nullptr;

    // Remove focus from the existing focus node (if any)
    if (oldFocusedElement) {
        if (oldFocusedElement->active())
            oldFocusedElement->setActive(false);

        oldFocusedElement->setFocus(false);

        // Dispatch the blur event and let the node do any other blur related activities (important for text fields)
        // If page lost focus, blur event will have already been dispatched
        if (page() && (page()->focusController().isFocused())) {
            oldFocusedElement->dispatchBlurEvent(newFocusedElement.get(), type);

            if (m_focusedElement) {
                // handler shifted focus
                focusChangeBlocked = true;
                newFocusedElement = nullptr;
            }

            oldFocusedElement->dispatchFocusOutEvent(EventTypeNames::focusout, newFocusedElement.get()); // DOM level 3 name for the bubbling blur event.
            // FIXME: We should remove firing DOMFocusOutEvent event when we are sure no content depends
            // on it, probably when <rdar://problem/8503958> is resolved.
            oldFocusedElement->dispatchFocusOutEvent(EventTypeNames::DOMFocusOut, newFocusedElement.get()); // DOM level 2 name for compatibility.

            if (m_focusedElement) {
                // handler shifted focus
                focusChangeBlocked = true;
                newFocusedElement = nullptr;
            }
        }

        if (view()) {
            Widget* oldWidget = widgetForElement(*oldFocusedElement);
            if (oldWidget)
                oldWidget->setFocus(false, type);
            else
                view()->setFocus(false, type);
        }
    }

    if (newFocusedElement && newFocusedElement->isFocusable()) {
        if (newFocusedElement->isRootEditableElement() && !acceptsEditingFocus(*newFocusedElement)) {
            // delegate blocks focus change
            focusChangeBlocked = true;
            goto SetFocusedElementDone;
        }
        // Set focus on the new node
        m_focusedElement = newFocusedElement;

        // Dispatch the focus event and let the node do any other focus related activities (important for text fields)
        // If page lost focus, event will be dispatched on page focus, don't duplicate
        if (page() && (page()->focusController().isFocused())) {
            m_focusedElement->dispatchFocusEvent(oldFocusedElement.get(), type);


            if (m_focusedElement != newFocusedElement) {
                // handler shifted focus
                focusChangeBlocked = true;
                goto SetFocusedElementDone;
            }

            m_focusedElement->dispatchFocusInEvent(EventTypeNames::focusin, oldFocusedElement.get(), type); // DOM level 3 bubbling focus event.

            if (m_focusedElement != newFocusedElement) {
                // handler shifted focus
                focusChangeBlocked = true;
                goto SetFocusedElementDone;
            }

            // FIXME: We should remove firing DOMFocusInEvent event when we are sure no content depends
            // on it, probably when <rdar://problem/8503958> is m.
            m_focusedElement->dispatchFocusInEvent(EventTypeNames::DOMFocusIn, oldFocusedElement.get(), type); // DOM level 2 for compatibility.

            if (m_focusedElement != newFocusedElement) {
                // handler shifted focus
                focusChangeBlocked = true;
                goto SetFocusedElementDone;
            }
        }

        m_focusedElement->setFocus(true);

        if (m_focusedElement->isRootEditableElement())
            frame()->spellChecker().didBeginEditing(m_focusedElement.get());

        // eww, I suck. set the qt focus correctly
        // ### find a better place in the code for this
        if (view()) {
            Widget* focusWidget = widgetForElement(*m_focusedElement);
            if (focusWidget) {
                // Make sure a widget has the right size before giving it focus.
                // Otherwise, we are testing edge cases of the Widget code.
                // Specifically, in WebCore this does not work well for text fields.
                updateLayout();
                // Re-get the widget in case updating the layout changed things.
                focusWidget = widgetForElement(*m_focusedElement);
            }
            if (focusWidget)
                focusWidget->setFocus(true, type);
            else
                view()->setFocus(true, type);
        }
    }

    if (!focusChangeBlocked && m_focusedElement) {
        // Create the AXObject cache in a focus change because Chromium relies on it.
        if (AXObjectCache* cache = axObjectCache())
            cache->handleFocusedUIElementChanged(oldFocusedElement.get(), newFocusedElement.get());
    }

    if (!focusChangeBlocked && frameHost())
        frameHost()->chromeClient().focusedNodeChanged(oldFocusedElement.get(), m_focusedElement.get());

SetFocusedElementDone:
    updateLayoutTreeIfNeeded();
    if (LocalFrame* frame = this->frame())
        frame->selection().didChangeFocus();
    return !focusChangeBlocked;
}

void Document::setCSSTarget(Element* newTarget)
{
    if (m_cssTarget)
        m_cssTarget->pseudoStateChanged(CSSSelector::PseudoTarget);
    m_cssTarget = newTarget;
    if (m_cssTarget)
        m_cssTarget->pseudoStateChanged(CSSSelector::PseudoTarget);
}

void Document::registerNodeList(const LiveNodeListBase* list)
{
#if ENABLE(OILPAN)
    m_nodeLists[list->invalidationType()].add(list);
#else
    m_nodeListCounts[list->invalidationType()]++;
#endif
    if (list->isRootedAtDocument())
        m_listsInvalidatedAtDocument.add(list);
}

void Document::unregisterNodeList(const LiveNodeListBase* list)
{
#if ENABLE(OILPAN)
    ASSERT(m_nodeLists[list->invalidationType()].contains(list));
    m_nodeLists[list->invalidationType()].remove(list);
#else
    m_nodeListCounts[list->invalidationType()]--;
#endif
    if (list->isRootedAtDocument()) {
        ASSERT(m_listsInvalidatedAtDocument.contains(list));
        m_listsInvalidatedAtDocument.remove(list);
    }
}

void Document::registerNodeListWithIdNameCache(const LiveNodeListBase* list)
{
#if ENABLE(OILPAN)
    m_nodeLists[InvalidateOnIdNameAttrChange].add(list);
#else
    m_nodeListCounts[InvalidateOnIdNameAttrChange]++;
#endif
}

void Document::unregisterNodeListWithIdNameCache(const LiveNodeListBase* list)
{
#if ENABLE(OILPAN)
    ASSERT(m_nodeLists[InvalidateOnIdNameAttrChange].contains(list));
    m_nodeLists[InvalidateOnIdNameAttrChange].remove(list);
#else
    ASSERT(m_nodeListCounts[InvalidateOnIdNameAttrChange] > 0);
    m_nodeListCounts[InvalidateOnIdNameAttrChange]--;
#endif
}

void Document::attachNodeIterator(NodeIterator* ni)
{
    m_nodeIterators.add(ni);
}

void Document::detachNodeIterator(NodeIterator* ni)
{
    // The node iterator can be detached without having been attached if its root node didn't have a document
    // when the iterator was created, but has it now.
    m_nodeIterators.remove(ni);
}

void Document::moveNodeIteratorsToNewDocument(Node& node, Document& newDocument)
{
    WillBeHeapHashSet<RawPtrWillBeWeakMember<NodeIterator>> nodeIteratorsList = m_nodeIterators;
    for (NodeIterator* ni : nodeIteratorsList) {
        if (ni->root() == node) {
            detachNodeIterator(ni);
            newDocument.attachNodeIterator(ni);
        }
    }
}

void Document::updateRangesAfterChildrenChanged(ContainerNode* container)
{
    for (Range* range : m_ranges)
        range->nodeChildrenChanged(container);
}

void Document::updateRangesAfterNodeMovedToAnotherDocument(const Node& node)
{
    ASSERT(node.document() != this);
    if (m_ranges.isEmpty())
        return;

    AttachedRangeSet ranges = m_ranges;
    for (Range* range : ranges)
        range->updateOwnerDocumentIfNeeded();
}

void Document::nodeChildrenWillBeRemoved(ContainerNode& container)
{
    EventDispatchForbiddenScope assertNoEventDispatch;
    for (Range* range : m_ranges)
        range->nodeChildrenWillBeRemoved(container);

    for (NodeIterator* ni : m_nodeIterators) {
        for (Node& n : NodeTraversal::childrenOf(container))
            ni->nodeWillBeRemoved(n);
    }

    if (LocalFrame* frame = this->frame()) {
        for (Node& n : NodeTraversal::childrenOf(container)) {
            frame->eventHandler().nodeWillBeRemoved(n);
            frame->selection().nodeWillBeRemoved(n);
            frame->page()->dragCaretController().nodeWillBeRemoved(n);
        }
    }
}

void Document::nodeWillBeRemoved(Node& n)
{
    for (NodeIterator* ni : m_nodeIterators)
        ni->nodeWillBeRemoved(n);

    for (Range* range : m_ranges)
        range->nodeWillBeRemoved(n);

    if (LocalFrame* frame = this->frame()) {
        frame->eventHandler().nodeWillBeRemoved(n);
        frame->selection().nodeWillBeRemoved(n);
        frame->page()->dragCaretController().nodeWillBeRemoved(n);
    }
}

void Document::didInsertText(Node* text, unsigned offset, unsigned length)
{
    for (Range* range : m_ranges)
        range->didInsertText(text, offset, length);

    m_markers->shiftMarkers(text, offset, length);
}

void Document::didRemoveText(Node* text, unsigned offset, unsigned length)
{
    for (Range* range : m_ranges)
        range->didRemoveText(text, offset, length);

    m_markers->removeMarkers(text, offset, length);
    m_markers->shiftMarkers(text, offset + length, 0 - length);
}

void Document::didMergeTextNodes(Text& oldNode, unsigned offset)
{
    if (!m_ranges.isEmpty()) {
        NodeWithIndex oldNodeWithIndex(oldNode);
        for (Range* range : m_ranges)
            range->didMergeTextNodes(oldNodeWithIndex, offset);
    }

    if (m_frame)
        m_frame->selection().didMergeTextNodes(oldNode, offset);

    // FIXME: This should update markers for spelling and grammar checking.
}

void Document::didSplitTextNode(Text& oldNode)
{
    for (Range* range : m_ranges)
        range->didSplitTextNode(oldNode);

    if (m_frame)
        m_frame->selection().didSplitTextNode(oldNode);

    // FIXME: This should update markers for spelling and grammar checking.
}

void Document::setWindowAttributeEventListener(const AtomicString& eventType, PassRefPtr<EventListener> listener)
{
    LocalDOMWindow* domWindow = this->domWindow();
    if (!domWindow)
        return;
    domWindow->setAttributeEventListener(eventType, listener);
}

EventListener* Document::getWindowAttributeEventListener(const AtomicString& eventType)
{
    LocalDOMWindow* domWindow = this->domWindow();
    if (!domWindow)
        return 0;
    return domWindow->getAttributeEventListener(eventType);
}

EventQueue* Document::eventQueue() const
{
    if (!m_domWindow)
        return 0;
    return m_domWindow->eventQueue();
}

void Document::enqueueAnimationFrameEvent(PassRefPtrWillBeRawPtr<Event> event)
{
    ensureScriptedAnimationController().enqueueEvent(event);
}

void Document::enqueueUniqueAnimationFrameEvent(PassRefPtrWillBeRawPtr<Event> event)
{
    ensureScriptedAnimationController().enqueuePerFrameEvent(event);
}

void Document::enqueueScrollEventForNode(Node* target)
{
    // Per the W3C CSSOM View Module only scroll events fired at the document should bubble.
    RefPtrWillBeRawPtr<Event> scrollEvent = target->isDocumentNode() ? Event::createBubble(EventTypeNames::scroll) : Event::create(EventTypeNames::scroll);
    scrollEvent->setTarget(target);
    ensureScriptedAnimationController().enqueuePerFrameEvent(scrollEvent.release());
}

void Document::enqueueResizeEvent()
{
    RefPtrWillBeRawPtr<Event> event = Event::create(EventTypeNames::resize);
    event->setTarget(domWindow());
    ensureScriptedAnimationController().enqueuePerFrameEvent(event.release());
}

void Document::enqueueMediaQueryChangeListeners(WillBeHeapVector<RefPtrWillBeMember<MediaQueryListListener>>& listeners)
{
    ensureScriptedAnimationController().enqueueMediaQueryChangeListeners(listeners);
}

void Document::dispatchEventsForPrinting()
{
    if (!m_scriptedAnimationController)
        return;
    m_scriptedAnimationController->dispatchEventsAndCallbacksForPrinting();
}

Document::EventFactorySet& Document::eventFactories()
{
    DEFINE_STATIC_LOCAL(EventFactorySet, s_eventFactory, ());
    return s_eventFactory;
}

void Document::registerEventFactory(PassOwnPtr<EventFactoryBase> eventFactory)
{
    ASSERT(!eventFactories().contains(eventFactory.get()));
    eventFactories().add(eventFactory);
}

PassRefPtrWillBeRawPtr<Event> Document::createEvent(const String& eventType, ExceptionState& exceptionState)
{
    RefPtrWillBeRawPtr<Event> event = nullptr;
    for (const auto& factory : eventFactories()) {
        event = factory->create(eventType);
        if (event)
            return event.release();
    }
    exceptionState.throwDOMException(NotSupportedError, "The provided event type ('" + eventType + "') is invalid.");
    return nullptr;
}

void Document::addMutationEventListenerTypeIfEnabled(ListenerType listenerType)
{
    if (ContextFeatures::mutationEventsEnabled(this))
        addListenerType(listenerType);
}

void Document::addListenerTypeIfNeeded(const AtomicString& eventType)
{
    if (eventType == EventTypeNames::DOMSubtreeModified) {
        UseCounter::count(*this, UseCounter::DOMSubtreeModifiedEvent);
        addMutationEventListenerTypeIfEnabled(DOMSUBTREEMODIFIED_LISTENER);
    } else if (eventType == EventTypeNames::DOMNodeInserted) {
        UseCounter::count(*this, UseCounter::DOMNodeInsertedEvent);
        addMutationEventListenerTypeIfEnabled(DOMNODEINSERTED_LISTENER);
    } else if (eventType == EventTypeNames::DOMNodeRemoved) {
        UseCounter::count(*this, UseCounter::DOMNodeRemovedEvent);
        addMutationEventListenerTypeIfEnabled(DOMNODEREMOVED_LISTENER);
    } else if (eventType == EventTypeNames::DOMNodeRemovedFromDocument) {
        UseCounter::count(*this, UseCounter::DOMNodeRemovedFromDocumentEvent);
        addMutationEventListenerTypeIfEnabled(DOMNODEREMOVEDFROMDOCUMENT_LISTENER);
    } else if (eventType == EventTypeNames::DOMNodeInsertedIntoDocument) {
        UseCounter::count(*this, UseCounter::DOMNodeInsertedIntoDocumentEvent);
        addMutationEventListenerTypeIfEnabled(DOMNODEINSERTEDINTODOCUMENT_LISTENER);
    } else if (eventType == EventTypeNames::DOMCharacterDataModified) {
        UseCounter::count(*this, UseCounter::DOMCharacterDataModifiedEvent);
        addMutationEventListenerTypeIfEnabled(DOMCHARACTERDATAMODIFIED_LISTENER);
    } else if (eventType == EventTypeNames::webkitAnimationStart || eventType == EventTypeNames::animationstart) {
        addListenerType(ANIMATIONSTART_LISTENER);
    } else if (eventType == EventTypeNames::webkitAnimationEnd || eventType == EventTypeNames::animationend) {
        addListenerType(ANIMATIONEND_LISTENER);
    } else if (eventType == EventTypeNames::webkitAnimationIteration || eventType == EventTypeNames::animationiteration) {
        addListenerType(ANIMATIONITERATION_LISTENER);
        if (view()) {
            // Need to re-evaluate time-to-effect-change for any running animations.
            view()->scheduleAnimation();
        }
    } else if (eventType == EventTypeNames::webkitTransitionEnd || eventType == EventTypeNames::transitionend) {
        addListenerType(TRANSITIONEND_LISTENER);
    } else if (eventType == EventTypeNames::scroll) {
        addListenerType(SCROLL_LISTENER);
    }
}

HTMLFrameOwnerElement* Document::ownerElement() const
{
    if (!frame())
        return 0;
    // FIXME: This probably breaks the attempts to layout after a load is finished in implicitClose(), and probably tons of other things...
    return frame()->deprecatedLocalOwner();
}

bool Document::isInInvisibleSubframe() const
{
    if (!ownerElement())
        return false; // this is the root element

    ASSERT(frame());
    return !frame()->ownerLayoutObject();
}

String Document::cookie(ExceptionState& exceptionState) const
{
    if (settings() && !settings()->cookieEnabled())
        return String();

    // FIXME: The HTML5 DOM spec states that this attribute can raise an
    // InvalidStateError exception on getting if the Document has no
    // browsing context.

    if (!securityOrigin()->canAccessCookies()) {
        if (isSandboxed(SandboxOrigin))
            exceptionState.throwSecurityError("The document is sandboxed and lacks the 'allow-same-origin' flag.");
        else if (url().protocolIs("data"))
            exceptionState.throwSecurityError("Cookies are disabled inside 'data:' URLs.");
        else
            exceptionState.throwSecurityError("Access is denied for this document.");
        return String();
    }

    KURL cookieURL = this->cookieURL();
    if (cookieURL.isEmpty())
        return String();

    return cookies(this, cookieURL);
}

void Document::setCookie(const String& value, ExceptionState& exceptionState)
{
    if (settings() && !settings()->cookieEnabled())
        return;

    // FIXME: The HTML5 DOM spec states that this attribute can raise an
    // InvalidStateError exception on setting if the Document has no
    // browsing context.

    if (!securityOrigin()->canAccessCookies()) {
        if (isSandboxed(SandboxOrigin))
            exceptionState.throwSecurityError("The document is sandboxed and lacks the 'allow-same-origin' flag.");
        else if (url().protocolIs("data"))
            exceptionState.throwSecurityError("Cookies are disabled inside 'data:' URLs.");
        else
            exceptionState.throwSecurityError("Access is denied for this document.");
        return;
    }

    KURL cookieURL = this->cookieURL();
    if (cookieURL.isEmpty())
        return;

    setCookies(this, cookieURL, value);
}

const AtomicString& Document::referrer() const
{
    if (loader())
        return loader()->request().httpReferrer();
    return nullAtom;
}

String Document::domain() const
{
    return securityOrigin()->domain();
}

void Document::setDomain(const String& newDomain, ExceptionState& exceptionState)
{
    UseCounter::count(*this, UseCounter::DocumentSetDomain);

    if (isSandboxed(SandboxDocumentDomain)) {
        exceptionState.throwSecurityError("Assignment is forbidden for sandboxed iframes.");
        return;
    }

    if (SchemeRegistry::isDomainRelaxationForbiddenForURLScheme(securityOrigin()->protocol())) {
        exceptionState.throwSecurityError("Assignment is forbidden for the '" + securityOrigin()->protocol() + "' scheme.");
        return;
    }

    if (newDomain.isEmpty()) {
        exceptionState.throwSecurityError("'" + newDomain + "' is an empty domain.");
        return;
    }

    OriginAccessEntry accessEntry(securityOrigin()->protocol(), newDomain, OriginAccessEntry::AllowSubdomains);
    OriginAccessEntry::MatchResult result = accessEntry.matchesOrigin(*securityOrigin());
    if (result == OriginAccessEntry::DoesNotMatchOrigin) {
        exceptionState.throwSecurityError("'" + newDomain + "' is not a suffix of '" + domain() + "'.");
        return;
    }

    if (result == OriginAccessEntry::MatchesOriginButIsPublicSuffix) {
        exceptionState.throwSecurityError("'" + newDomain + "' is a top-level domain.");
        return;
    }

    securityOrigin()->setDomainFromDOM(newDomain);
    if (m_frame)
        m_frame->script().updateSecurityOrigin(securityOrigin());
}

// http://www.whatwg.org/specs/web-apps/current-work/#dom-document-lastmodified
String Document::lastModified() const
{
    DateComponents date;
    bool foundDate = false;
    if (m_frame) {
        if (DocumentLoader* documentLoader = loader()) {
            const AtomicString& httpLastModified = documentLoader->response().httpHeaderField("Last-Modified");
            if (!httpLastModified.isEmpty()) {
                date.setMillisecondsSinceEpochForDateTime(convertToLocalTime(parseDate(httpLastModified)));
                foundDate = true;
            }
        }
    }
    // FIXME: If this document came from the file system, the HTML5
    // specificiation tells us to read the last modification date from the file
    // system.
    if (!foundDate)
        date.setMillisecondsSinceEpochForDateTime(convertToLocalTime(currentTimeMS()));
    return String::format("%02d/%02d/%04d %02d:%02d:%02d", date.month() + 1, date.monthDay(), date.fullYear(), date.hour(), date.minute(), date.second());
}

const KURL& Document::firstPartyForCookies() const
{
    // We're intentionally using the URL of each document rather than the document's SecurityOrigin.
    // Sandboxing a document into a unique origin shouldn't effect first-/third-party status for
    // cookies and site data.
    OriginAccessEntry accessEntry(topDocument().url().protocol(), topDocument().url().host(), OriginAccessEntry::AllowRegisterableDomains);
    const Document* currentDocument = this;
    while (currentDocument) {
        // Skip over srcdoc documents, as they are always same-origin with their closest non-srcdoc parent.
        while (currentDocument->isSrcdocDocument())
            currentDocument = currentDocument->parentDocument();
        ASSERT(currentDocument);

        if (accessEntry.matchesOrigin(*currentDocument->securityOrigin()) == OriginAccessEntry::DoesNotMatchOrigin)
            return SecurityOrigin::urlWithUniqueSecurityOrigin();

        currentDocument = currentDocument->parentDocument();
    }

    return topDocument().url();
}

static bool isValidNameNonASCII(const LChar* characters, unsigned length)
{
    if (!isValidNameStart(characters[0]))
        return false;

    for (unsigned i = 1; i < length; ++i) {
        if (!isValidNamePart(characters[i]))
            return false;
    }

    return true;
}

static bool isValidNameNonASCII(const UChar* characters, unsigned length)
{
    for (unsigned i = 0; i < length;) {
        bool first = i == 0;
        UChar32 c;
        U16_NEXT(characters, i, length, c); // Increments i.
        if (first ? !isValidNameStart(c) : !isValidNamePart(c))
            return false;
    }

    return true;
}

template<typename CharType>
static inline bool isValidNameASCII(const CharType* characters, unsigned length)
{
    CharType c = characters[0];
    if (!(isASCIIAlpha(c) || c == ':' || c == '_'))
        return false;

    for (unsigned i = 1; i < length; ++i) {
        c = characters[i];
        if (!(isASCIIAlphanumeric(c) || c == ':' || c == '_' || c == '-' || c == '.'))
            return false;
    }

    return true;
}

bool Document::isValidName(const String& name)
{
    unsigned length = name.length();
    if (!length)
        return false;

    if (name.is8Bit()) {
        const LChar* characters = name.characters8();

        if (isValidNameASCII(characters, length))
            return true;

        return isValidNameNonASCII(characters, length);
    }

    const UChar* characters = name.characters16();

    if (isValidNameASCII(characters, length))
        return true;

    return isValidNameNonASCII(characters, length);
}

enum QualifiedNameStatus {
    QNValid,
    QNMultipleColons,
    QNInvalidStartChar,
    QNInvalidChar,
    QNEmptyPrefix,
    QNEmptyLocalName
};

struct ParseQualifiedNameResult {
    QualifiedNameStatus status;
    UChar32 character;
    ParseQualifiedNameResult() { }
    explicit ParseQualifiedNameResult(QualifiedNameStatus status) : status(status) { }
    ParseQualifiedNameResult(QualifiedNameStatus status, UChar32 character) : status(status), character(character) { }
};

template<typename CharType>
static ParseQualifiedNameResult parseQualifiedNameInternal(const AtomicString& qualifiedName, const CharType* characters, unsigned length, AtomicString& prefix, AtomicString& localName)
{
    bool nameStart = true;
    bool sawColon = false;
    int colonPos = 0;

    for (unsigned i = 0; i < length;) {
        UChar32 c;
        U16_NEXT(characters, i, length, c)
        if (c == ':') {
            if (sawColon)
                return ParseQualifiedNameResult(QNMultipleColons);
            nameStart = true;
            sawColon = true;
            colonPos = i - 1;
        } else if (nameStart) {
            if (!isValidNameStart(c))
                return ParseQualifiedNameResult(QNInvalidStartChar, c);
            nameStart = false;
        } else {
            if (!isValidNamePart(c))
                return ParseQualifiedNameResult(QNInvalidChar, c);
        }
    }

    if (!sawColon) {
        prefix = nullAtom;
        localName = qualifiedName;
    } else {
        prefix = AtomicString(characters, colonPos);
        if (prefix.isEmpty())
            return ParseQualifiedNameResult(QNEmptyPrefix);
        int prefixStart = colonPos + 1;
        localName = AtomicString(characters + prefixStart, length - prefixStart);
    }

    if (localName.isEmpty())
        return ParseQualifiedNameResult(QNEmptyLocalName);

    return ParseQualifiedNameResult(QNValid);
}

bool Document::parseQualifiedName(const AtomicString& qualifiedName, AtomicString& prefix, AtomicString& localName, ExceptionState& exceptionState)
{
    unsigned length = qualifiedName.length();

    if (!length) {
        exceptionState.throwDOMException(InvalidCharacterError, "The qualified name provided is empty.");
        return false;
    }

    ParseQualifiedNameResult returnValue;
    if (qualifiedName.is8Bit())
        returnValue = parseQualifiedNameInternal(qualifiedName, qualifiedName.characters8(), length, prefix, localName);
    else
        returnValue = parseQualifiedNameInternal(qualifiedName, qualifiedName.characters16(), length, prefix, localName);
    if (returnValue.status == QNValid)
        return true;

    StringBuilder message;
    message.appendLiteral("The qualified name provided ('");
    message.append(qualifiedName);
    message.appendLiteral("') ");

    if (returnValue.status == QNMultipleColons) {
        message.appendLiteral("contains multiple colons.");
    } else if (returnValue.status == QNInvalidStartChar) {
        message.appendLiteral("contains the invalid name-start character '");
        message.append(returnValue.character);
        message.appendLiteral("'.");
    } else if (returnValue.status == QNInvalidChar) {
        message.appendLiteral("contains the invalid character '");
        message.append(returnValue.character);
        message.appendLiteral("'.");
    } else if (returnValue.status == QNEmptyPrefix) {
        message.appendLiteral("has an empty namespace prefix.");
    } else {
        ASSERT(returnValue.status == QNEmptyLocalName);
        message.appendLiteral("has an empty local name.");
    }

    if (returnValue.status == QNInvalidStartChar || returnValue.status == QNInvalidChar)
        exceptionState.throwDOMException(InvalidCharacterError, message.toString());
    else
        exceptionState.throwDOMException(NamespaceError, message.toString());
    return false;
}

void Document::setEncodingData(const DocumentEncodingData& newData)
{
    // It's possible for the encoding of the document to change while we're decoding
    // data. That can only occur while we're processing the <head> portion of the
    // document. There isn't much user-visible content in the <head>, but there is
    // the <title> element. This function detects that situation and re-decodes the
    // document's title so that the user doesn't see an incorrectly decoded title
    // in the title bar.
    if (m_titleElement
        && encoding() != newData.encoding()
        && !ElementTraversal::firstWithin(*m_titleElement)
        && encoding() == Latin1Encoding()
        && m_titleElement->textContent().containsOnlyLatin1()) {

        CString originalBytes = m_titleElement->textContent().latin1();
        OwnPtr<TextCodec> codec = newTextCodec(newData.encoding());
        String correctlyDecodedTitle = codec->decode(originalBytes.data(), originalBytes.length(), DataEOF);
        m_titleElement->setTextContent(correctlyDecodedTitle);
    }

    ASSERT(newData.encoding().isValid());
    m_encodingData = newData;

    // FIXME: Should be removed as part of https://code.google.com/p/chromium/issues/detail?id=319643
    bool shouldUseVisualOrdering = m_encodingData.encoding().usesVisualOrdering();
    if (shouldUseVisualOrdering != m_visuallyOrdered) {
        m_visuallyOrdered = shouldUseVisualOrdering;
        // FIXME: How is possible to not have a layoutObject here?
        if (layoutView())
            layoutView()->mutableStyleRef().setRTLOrdering(m_visuallyOrdered ? VisualOrder : LogicalOrder);
        setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::VisuallyOrdered));
    }
}

KURL Document::completeURL(const String& url) const
{
    return completeURLWithOverride(url, m_baseURL);
}

KURL Document::completeURLWithOverride(const String& url, const KURL& baseURLOverride) const
{
    ASSERT(baseURLOverride.isEmpty() || baseURLOverride.isValid());

    // Always return a null URL when passed a null string.
    // FIXME: Should we change the KURL constructor to have this behavior?
    // See also [CSS]StyleSheet::completeURL(const String&)
    if (url.isNull())
        return KURL();
    // This logic is deliberately spread over many statements in an attempt to track down http://crbug.com/312410.
    const KURL* baseURLFromParent = 0;
    bool shouldUseParentBaseURL = baseURLOverride.isEmpty();
    if (!shouldUseParentBaseURL) {
        const KURL& aboutBlankURL = blankURL();
        shouldUseParentBaseURL = (baseURLOverride == aboutBlankURL);
    }
    if (shouldUseParentBaseURL) {
        if (Document* parent = parentDocument())
            baseURLFromParent = &parent->baseURL();
    }
    const KURL& baseURL = baseURLFromParent ? *baseURLFromParent : baseURLOverride;
    if (!encoding().isValid())
        return KURL(baseURL, url);
    return KURL(baseURL, url, encoding());
}

// Support for Javascript execCommand, and related methods

static Editor::Command command(Document* document, const String& commandName)
{
    LocalFrame* frame = document->frame();
    if (!frame || frame->document() != document)
        return Editor::Command();

    document->updateLayoutTreeIfNeeded();
    return frame->editor().command(commandName, CommandFromDOM);
}

bool Document::execCommand(const String& commandName, bool, const String& value, ExceptionState& exceptionState)
{
    if (!isHTMLDocument() && !isXHTMLDocument()) {
        exceptionState.throwDOMException(InvalidStateError, "execCommand is only supported on HTML documents.");
        return false;
    }

    // We don't allow recursive |execCommand()| to protect against attack code.
    // Recursive call of |execCommand()| could be happened by moving iframe
    // with script triggered by insertion, e.g. <iframe src="javascript:...">
    // <iframe onload="...">. This usage is valid as of the specification
    // although, it isn't common use case, rather it is used as attack code.
    static bool inExecCommand = false;
    if (inExecCommand) {
        String message = "We don't execute document.execCommand() this time, because it is called recursively.";
        addConsoleMessage(ConsoleMessage::create(JSMessageSource, WarningMessageLevel, message));
        return false;
    }
    TemporaryChange<bool> executeScope(inExecCommand, true);

    // Postpone DOM mutation events, which can execute scripts and change
    // DOM tree against implementation assumption.
    EventQueueScope eventQueueScope;
    Editor::Command editorCommand = command(this, commandName);
    Platform::current()->histogramSparse("WebCore.Document.execCommand", editorCommand.idForHistogram());
    return editorCommand.execute(value);
}

bool Document::queryCommandEnabled(const String& commandName, ExceptionState& exceptionState)
{
    if (!isHTMLDocument() && !isXHTMLDocument()) {
        exceptionState.throwDOMException(InvalidStateError, "queryCommandEnabled is only supported on HTML documents.");
        return false;
    }

    return command(this, commandName).isEnabled();
}

bool Document::queryCommandIndeterm(const String& commandName, ExceptionState& exceptionState)
{
    if (!isHTMLDocument() && !isXHTMLDocument()) {
        exceptionState.throwDOMException(InvalidStateError, "queryCommandIndeterm is only supported on HTML documents.");
        return false;
    }

    return command(this, commandName).state() == MixedTriState;
}

bool Document::queryCommandState(const String& commandName, ExceptionState& exceptionState)
{
    if (!isHTMLDocument() && !isXHTMLDocument()) {
        exceptionState.throwDOMException(InvalidStateError, "queryCommandState is only supported on HTML documents.");
        return false;
    }

    return command(this, commandName).state() == TrueTriState;
}

bool Document::queryCommandSupported(const String& commandName, ExceptionState& exceptionState)
{
    if (!isHTMLDocument() && !isXHTMLDocument()) {
        exceptionState.throwDOMException(InvalidStateError, "queryCommandSupported is only supported on HTML documents.");
        return false;
    }

    return command(this, commandName).isSupported();
}

String Document::queryCommandValue(const String& commandName, ExceptionState& exceptionState)
{
    if (!isHTMLDocument() && !isXHTMLDocument()) {
        exceptionState.throwDOMException(InvalidStateError, "queryCommandValue is only supported on HTML documents.");
        return "";
    }

    return command(this, commandName).value();
}

KURL Document::openSearchDescriptionURL()
{
    static const char openSearchMIMEType[] = "application/opensearchdescription+xml";
    static const char openSearchRelation[] = "search";

    // FIXME: Why do only top-level frames have openSearchDescriptionURLs?
    if (!frame() || frame()->tree().parent())
        return KURL();

    // FIXME: Why do we need to wait for load completion?
    if (!loadEventFinished())
        return KURL();

    if (!head())
        return KURL();

    for (HTMLLinkElement* linkElement = Traversal<HTMLLinkElement>::firstChild(*head()); linkElement; linkElement = Traversal<HTMLLinkElement>::nextSibling(*linkElement)) {
        if (!equalIgnoringCase(linkElement->type(), openSearchMIMEType) || !equalIgnoringCase(linkElement->rel(), openSearchRelation))
            continue;
        if (linkElement->href().isEmpty())
            continue;
        return linkElement->href();
    }

    return KURL();
}

void Document::pushCurrentScript(PassRefPtrWillBeRawPtr<HTMLScriptElement> newCurrentScript)
{
    ASSERT(newCurrentScript);
    m_currentScriptStack.append(newCurrentScript);
}

void Document::popCurrentScript()
{
    ASSERT(!m_currentScriptStack.isEmpty());
    m_currentScriptStack.removeLast();
}

void Document::setTransformSource(PassOwnPtr<TransformSource> source)
{
    m_transformSource = source;
}

String Document::designMode() const
{
    return inDesignMode() ? "on" : "off";
}

void Document::setDesignMode(const String& value)
{
    bool newValue = m_designMode;
    if (equalIgnoringCase(value, "on"))
        newValue = true;
    else if (equalIgnoringCase(value, "off"))
        newValue = false;
    if (newValue == m_designMode)
        return;
    m_designMode = newValue;
    setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::DesignMode));
}

Document* Document::parentDocument() const
{
    if (!m_frame)
        return 0;
    Frame* parent = m_frame->tree().parent();
    if (!parent || !parent->isLocalFrame())
        return 0;
    return toLocalFrame(parent)->document();
}

Document& Document::topDocument() const
{
    // FIXME: Not clear what topDocument() should do in the OOPI case--should it return the topmost
    // available Document, or something else?
    Document* doc = const_cast<Document*>(this);
    for (HTMLFrameOwnerElement* element = doc->ownerElement(); element; element = doc->ownerElement())
        doc = &element->document();

    ASSERT(doc);
    return *doc;
}

WeakPtrWillBeRawPtr<Document> Document::contextDocument()
{
    if (m_contextDocument)
        return m_contextDocument;
    if (m_frame) {
#if ENABLE(OILPAN)
        return this;
#else
        return m_weakFactory.createWeakPtr();
#endif
    }
    return WeakPtrWillBeRawPtr<Document>(nullptr);
}

PassRefPtrWillBeRawPtr<Attr> Document::createAttribute(const AtomicString& name, ExceptionState& exceptionState)
{
    if (isHTMLDocument() && name != name.lower())
        UseCounter::count(*this, UseCounter::HTMLDocumentCreateAttributeNameNotLowercase);
    return createAttributeNS(nullAtom, name, exceptionState, true);
}

PassRefPtrWillBeRawPtr<Attr> Document::createAttributeNS(const AtomicString& namespaceURI, const AtomicString& qualifiedName, ExceptionState& exceptionState, bool shouldIgnoreNamespaceChecks)
{
    AtomicString prefix, localName;
    if (!parseQualifiedName(qualifiedName, prefix, localName, exceptionState))
        return nullptr;

    QualifiedName qName(prefix, localName, namespaceURI);

    if (!shouldIgnoreNamespaceChecks && !hasValidNamespaceForAttributes(qName)) {
        exceptionState.throwDOMException(NamespaceError, "The namespace URI provided ('" + namespaceURI + "') is not valid for the qualified name provided ('" + qualifiedName + "').");
        return nullptr;
    }

    return Attr::create(*this, qName, emptyAtom);
}

const SVGDocumentExtensions* Document::svgExtensions()
{
    return m_svgExtensions.get();
}

SVGDocumentExtensions& Document::accessSVGExtensions()
{
    if (!m_svgExtensions)
        m_svgExtensions = adoptPtrWillBeNoop(new SVGDocumentExtensions(this));
    return *m_svgExtensions;
}

bool Document::hasSVGRootNode() const
{
    return isSVGSVGElement(documentElement());
}

PassRefPtrWillBeRawPtr<HTMLCollection> Document::images()
{
    return ensureCachedCollection<HTMLCollection>(DocImages);
}

PassRefPtrWillBeRawPtr<HTMLCollection> Document::applets()
{
    return ensureCachedCollection<HTMLCollection>(DocApplets);
}

PassRefPtrWillBeRawPtr<HTMLCollection> Document::embeds()
{
    return ensureCachedCollection<HTMLCollection>(DocEmbeds);
}

PassRefPtrWillBeRawPtr<HTMLCollection> Document::scripts()
{
    return ensureCachedCollection<HTMLCollection>(DocScripts);
}

PassRefPtrWillBeRawPtr<HTMLCollection> Document::links()
{
    return ensureCachedCollection<HTMLCollection>(DocLinks);
}

PassRefPtrWillBeRawPtr<HTMLCollection> Document::forms()
{
    return ensureCachedCollection<HTMLCollection>(DocForms);
}

PassRefPtrWillBeRawPtr<HTMLCollection> Document::anchors()
{
    return ensureCachedCollection<HTMLCollection>(DocAnchors);
}

PassRefPtrWillBeRawPtr<HTMLAllCollection> Document::allForBinding()
{
    UseCounter::count(*this, UseCounter::DocumentAll);
    return all();
}

PassRefPtrWillBeRawPtr<HTMLAllCollection> Document::all()
{
    return ensureCachedCollection<HTMLAllCollection>(DocAll);
}

PassRefPtrWillBeRawPtr<HTMLCollection> Document::windowNamedItems(const AtomicString& name)
{
    return ensureCachedCollection<WindowNameCollection>(WindowNamedItems, name);
}

PassRefPtrWillBeRawPtr<DocumentNameCollection> Document::documentNamedItems(const AtomicString& name)
{
    return ensureCachedCollection<DocumentNameCollection>(DocumentNamedItems, name);
}

void Document::finishedParsing()
{
    ASSERT(!scriptableDocumentParser() || !m_parser->isParsing());
    ASSERT(!scriptableDocumentParser() || m_readyState != Loading);
    setParsingState(InDOMContentLoaded);

    // FIXME: DOMContentLoaded is dispatched synchronously, but this should be dispatched in a queued task,
    // See https://crbug.com/425790
    if (!m_documentTiming.domContentLoadedEventStart())
        m_documentTiming.setDomContentLoadedEventStart(monotonicallyIncreasingTime());
    dispatchEvent(Event::createBubble(EventTypeNames::DOMContentLoaded));
    if (!m_documentTiming.domContentLoadedEventEnd())
        m_documentTiming.setDomContentLoadedEventEnd(monotonicallyIncreasingTime());
    setParsingState(FinishedParsing);

    // The microtask checkpoint or the loader's finishedParsing() method may invoke script that causes this object to
    // be dereferenced (when this document is in an iframe and the onload causes the iframe's src to change).
    // Keep it alive until we are done.
    RefPtrWillBeRawPtr<Document> protect(this);

    // Ensure Custom Element callbacks are drained before DOMContentLoaded.
    // FIXME: Remove this ad-hoc checkpoint when DOMContentLoaded is dispatched in a
    // queued task, which will do a checkpoint anyway. https://crbug.com/425790
    Microtask::performCheckpoint(V8PerIsolateData::mainThreadIsolate());

    if (RefPtrWillBeRawPtr<LocalFrame> frame = this->frame()) {
        // Don't update the layout tree if we haven't requested the main resource yet to avoid
        // adding extra latency. Note that the first layout tree update can be expensive since it
        // triggers the parsing of the default stylesheets which are compiled-in.
        const bool mainResourceWasAlreadyRequested = frame->loader().stateMachine()->committedFirstRealDocumentLoad();

        // FrameLoader::finishedParsing() might end up calling Document::implicitClose() if all
        // resource loads are complete. HTMLObjectElements can start loading their resources from
        // post attach callbacks triggered by recalcStyle().  This means if we parse out an <object>
        // tag and then reach the end of the document without updating styles, we might not have yet
        // started the resource load and might fire the window load event too early.  To avoid this
        // we force the styles to be up to date before calling FrameLoader::finishedParsing().
        // See https://bugs.webkit.org/show_bug.cgi?id=36864 starting around comment 35.
        if (mainResourceWasAlreadyRequested)
            updateLayoutTreeIfNeeded();

        frame->loader().finishedParsing();

        TRACE_EVENT_INSTANT1("devtools.timeline", "MarkDOMContent", TRACE_EVENT_SCOPE_THREAD, "data", InspectorMarkLoadEvent::data(frame.get()));
        InspectorInstrumentation::domContentLoadedEventFired(frame.get());
    }

    // Schedule dropping of the ElementDataCache. We keep it alive for a while after parsing finishes
    // so that dynamically inserted content can also benefit from sharing optimizations.
    // Note that we don't refresh the timer on cache access since that could lead to huge caches being kept
    // alive indefinitely by something innocuous like JS setting .innerHTML repeatedly on a timer.
    m_elementDataCacheClearTimer.startOneShot(10, FROM_HERE);

    // Parser should have picked up all preloads by now
    m_fetcher->clearPreloads();
}

void Document::elementDataCacheClearTimerFired(Timer<Document>*)
{
    m_elementDataCache.clear();
}

Vector<IconURL> Document::iconURLs(int iconTypesMask)
{
    IconURL firstFavicon;
    IconURL firstTouchIcon;
    IconURL firstTouchPrecomposedIcon;
    Vector<IconURL> secondaryIcons;

    // Start from the last child node so that icons seen later take precedence as required by the spec.
    for (HTMLLinkElement* linkElement = head() ? Traversal<HTMLLinkElement>::firstChild(*head()) : 0; linkElement; linkElement = Traversal<HTMLLinkElement>::nextSibling(*linkElement)) {
        if (!(linkElement->iconType() & iconTypesMask))
            continue;
        if (linkElement->href().isEmpty())
            continue;
        if (!RuntimeEnabledFeatures::touchIconLoadingEnabled() && linkElement->iconType() != Favicon)
            continue;

        IconURL newURL(linkElement->href(), linkElement->iconSizes(), linkElement->type(), linkElement->iconType());
        if (linkElement->iconType() == Favicon) {
            if (firstFavicon.m_iconType != InvalidIcon)
                secondaryIcons.append(firstFavicon);
            firstFavicon = newURL;
        } else if (linkElement->iconType() == TouchIcon) {
            if (firstTouchIcon.m_iconType != InvalidIcon)
                secondaryIcons.append(firstTouchIcon);
            firstTouchIcon = newURL;
        } else if (linkElement->iconType() == TouchPrecomposedIcon) {
            if (firstTouchPrecomposedIcon.m_iconType != InvalidIcon)
                secondaryIcons.append(firstTouchPrecomposedIcon);
            firstTouchPrecomposedIcon = newURL;
        } else {
            ASSERT_NOT_REACHED();
        }
    }

    Vector<IconURL> iconURLs;
    if (firstFavicon.m_iconType != InvalidIcon)
        iconURLs.append(firstFavicon);
    else if (m_url.protocolIsInHTTPFamily() && iconTypesMask & Favicon)
        iconURLs.append(IconURL::defaultFavicon(m_url));

    if (firstTouchIcon.m_iconType != InvalidIcon)
        iconURLs.append(firstTouchIcon);
    if (firstTouchPrecomposedIcon.m_iconType != InvalidIcon)
        iconURLs.append(firstTouchPrecomposedIcon);
    for (int i = secondaryIcons.size() - 1; i >= 0; --i)
        iconURLs.append(secondaryIcons[i]);
    return iconURLs;
}

Color Document::themeColor() const
{
    for (HTMLMetaElement* metaElement = head() ? Traversal<HTMLMetaElement>::firstChild(*head()) : 0; metaElement; metaElement = Traversal<HTMLMetaElement>::nextSibling(*metaElement)) {
        RGBA32 rgb = Color::transparent;
        if (equalIgnoringCase(metaElement->name(), "theme-color") && CSSParser::parseColor(rgb, metaElement->content().string().stripWhiteSpace(), true))
            return Color(rgb);
    }
    return Color();
}

HTMLLinkElement* Document::linkManifest() const
{
    HTMLHeadElement* head = this->head();
    if (!head)
        return 0;

    // The first link element with a manifest rel must be used. Others are ignored.
    for (HTMLLinkElement* linkElement = Traversal<HTMLLinkElement>::firstChild(*head); linkElement; linkElement = Traversal<HTMLLinkElement>::nextSibling(*linkElement)) {
        if (!linkElement->relAttribute().isManifest())
            continue;
        return linkElement;
    }

    return 0;
}

HTMLLinkElement* Document::linkDefaultPresentation() const
{
    HTMLHeadElement* head = this->head();
    if (!head)
        return nullptr;

    // The first link element with a presentation URL rel must be used. Others are ignored.
    for (HTMLLinkElement* linkElement = Traversal<HTMLLinkElement>::firstChild(*head); linkElement; linkElement = Traversal<HTMLLinkElement>::nextSibling(*linkElement)) {
        if (!linkElement->relAttribute().isDefaultPresentation())
            continue;
        return linkElement;
    }

    return nullptr;
}

void Document::setUseSecureKeyboardEntryWhenActive(bool usesSecureKeyboard)
{
    if (m_useSecureKeyboardEntryWhenActive == usesSecureKeyboard)
        return;

    m_useSecureKeyboardEntryWhenActive = usesSecureKeyboard;
    m_frame->selection().updateSecureKeyboardEntryIfActive();
}

bool Document::useSecureKeyboardEntryWhenActive() const
{
    return m_useSecureKeyboardEntryWhenActive;
}

void Document::initSecurityContext()
{
    initSecurityContext(DocumentInit(m_url, m_frame, contextDocument(), m_importsController));
}

void Document::initSecurityContext(const DocumentInit& initializer)
{
    if (haveInitializedSecurityOrigin()) {
        ASSERT(securityOrigin());
        return;
    }

    if (initializer.isHostedInReservedIPRange())
        setHostedInReservedIPRange();

    if (!initializer.hasSecurityContext()) {
        // No source for a security context.
        // This can occur via document.implementation.createDocument().
        m_cookieURL = KURL(ParsedURLString, emptyString());
        setSecurityOrigin(SecurityOrigin::createUnique());
        initContentSecurityPolicy();
        return;
    }

    // In the common case, create the security context from the currently
    // loading URL with a fresh content security policy.
    m_cookieURL = m_url;
    enforceSandboxFlags(initializer.sandboxFlags());
    if (initializer.shouldEnforceStrictMixedContentChecking())
        enforceStrictMixedContentChecking();
    setInsecureRequestsPolicy(initializer.insecureRequestsPolicy());
    if (initializer.insecureNavigationsToUpgrade()) {
        for (auto toUpgrade : *initializer.insecureNavigationsToUpgrade())
            addInsecureNavigationUpgrade(toUpgrade);
    }
    setSecurityOrigin(isSandboxed(SandboxOrigin) ? SecurityOrigin::createUnique() : SecurityOrigin::create(m_url));

    if (importsController()) {
        // If this document is an HTML import, grab a reference to it's master document's Content
        // Security Policy. We don't call 'initContentSecurityPolicy' in this case, as we can't
        // rebind the master document's policy object: its ExecutionContext needs to remain tied
        // to the master document.
        setContentSecurityPolicy(importsController()->master()->contentSecurityPolicy());
    } else {
        initContentSecurityPolicy();
    }

    if (Settings* settings = initializer.settings()) {
        if (!settings->webSecurityEnabled()) {
            // Web security is turned off. We should let this document access every other document. This is used primary by testing
            // harnesses for web sites.
            securityOrigin()->grantUniversalAccess();
        } else if (securityOrigin()->isLocal()) {
            if (settings->allowUniversalAccessFromFileURLs()) {
                // Some clients want local URLs to have universal access, but that setting is dangerous for other clients.
                securityOrigin()->grantUniversalAccess();
            } else if (!settings->allowFileAccessFromFileURLs()) {
                // Some clients want local URLs to have even tighter restrictions by default, and not be able to access other local files.
                // FIXME 81578: The naming of this is confusing. Files with restricted access to other local files
                // still can have other privileges that can be remembered, thereby not making them unique origins.
                securityOrigin()->enforceFilePathSeparation();
            }
        }
    }

    if (initializer.shouldTreatURLAsSrcdocDocument()) {
        m_isSrcdocDocument = true;
        setBaseURLOverride(initializer.parentBaseURL());
    }

    if (!shouldInheritSecurityOriginFromOwner(m_url))
        return;

    // If we do not obtain a meaningful origin from the URL, then we try to
    // find one via the frame hierarchy.

    if (!initializer.owner()) {
        didFailToInitializeSecurityOrigin();
        return;
    }

    if (isSandboxed(SandboxOrigin)) {
        // If we're supposed to inherit our security origin from our owner,
        // but we're also sandboxed, the only thing we inherit is the ability
        // to load local resources. This lets about:blank iframes in file://
        // URL documents load images and other resources from the file system.
        if (initializer.owner()->securityOrigin()->canLoadLocalResources())
            securityOrigin()->grantLoadLocalResources();
        return;
    }

    m_cookieURL = initializer.owner()->cookieURL();
    // We alias the SecurityOrigins to match Firefox, see Bug 15313
    // https://bugs.webkit.org/show_bug.cgi?id=15313
    setSecurityOrigin(initializer.owner()->securityOrigin());
}

void Document::initContentSecurityPolicy(PassRefPtr<ContentSecurityPolicy> csp)
{
    setContentSecurityPolicy(csp ? csp : ContentSecurityPolicy::create());
    if (m_frame && m_frame->tree().parent() && m_frame->tree().parent()->isLocalFrame()) {
        ContentSecurityPolicy* parentCSP = toLocalFrame(m_frame->tree().parent())->document()->contentSecurityPolicy();
        if (shouldInheritSecurityOriginFromOwner(m_url)) {
            contentSecurityPolicy()->copyStateFrom(parentCSP);
        } else if (isPluginDocument()) {
            // Per CSP2, plugin-types for plugin documents in nested browsing
            // contexts gets inherited from the parent.
            contentSecurityPolicy()->copyPluginTypesFrom(parentCSP);
        }
    }
    contentSecurityPolicy()->bindToExecutionContext(this);
}

bool Document::allowInlineEventHandlers(Node* node, EventListener* listener, const String& contextURL, const WTF::OrdinalNumber& contextLine)
{
    if (!contentSecurityPolicy()->allowInlineEventHandlers(contextURL, contextLine))
        return false;

    // HTML says that inline script needs browsing context to create its execution environment.
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/webappapis.html#event-handler-attributes
    // Also, if the listening node came from other document, which happens on context-less event dispatching,
    // we also need to ask the owner document of the node.
    LocalFrame* frame = executingFrame();
    if (!frame)
        return false;
    if (!frame->script().canExecuteScripts(NotAboutToExecuteScript))
        return false;
    if (node && node->document() != this && !node->document().allowInlineEventHandlers(node, listener, contextURL, contextLine))
        return false;

    return true;
}

bool Document::allowExecutingScripts(Node* node)
{
    // FIXME: Eventually we'd like to evaluate scripts which are inserted into a
    // viewless document but this'll do for now.
    // See http://bugs.webkit.org/show_bug.cgi?id=5727
    LocalFrame* frame = executingFrame();
    if (!frame)
        return false;
    if (!node->document().executingFrame())
        return false;
    if (!frame->script().canExecuteScripts(AboutToExecuteScript))
        return false;
    return true;
}

void Document::updateSecurityOrigin(PassRefPtr<SecurityOrigin> origin)
{
    setSecurityOrigin(origin);
    didUpdateSecurityOrigin();
}

void Document::didUpdateSecurityOrigin()
{
    if (!m_frame)
        return;
    m_frame->script().updateSecurityOrigin(securityOrigin());
}

bool Document::isContextThread() const
{
    return isMainThread();
}

void Document::updateFocusAppearanceSoon(bool restorePreviousSelection)
{
    m_updateFocusAppearanceRestoresSelection = restorePreviousSelection;
    if (!m_updateFocusAppearanceTimer.isActive())
        m_updateFocusAppearanceTimer.startOneShot(0, FROM_HERE);
}

void Document::cancelFocusAppearanceUpdate()
{
    m_updateFocusAppearanceTimer.stop();
}

void Document::updateFocusAppearanceTimerFired(Timer<Document>*)
{
    Element* element = focusedElement();
    if (!element)
        return;
    updateLayout();
    if (element->isFocusable())
        element->updateFocusAppearance(m_updateFocusAppearanceRestoresSelection);
}

void Document::attachRange(Range* range)
{
    ASSERT(!m_ranges.contains(range));
    m_ranges.add(range);
}

void Document::detachRange(Range* range)
{
    // We don't ASSERT m_ranges.contains(range) to allow us to call this
    // unconditionally to fix: https://bugs.webkit.org/show_bug.cgi?id=26044
    m_ranges.remove(range);
}

ScriptValue Document::getCSSCanvasContext(ScriptState* scriptState, const String& type, const String& name, int width, int height)
{
    HTMLCanvasElement& element = getCSSCanvasElement(name);
    element.setSize(IntSize(width, height));
    CanvasRenderingContext* context = element.getCanvasRenderingContext(type, CanvasContextCreationAttributes());
    if (!context) {
        return ScriptValue::createNull(scriptState);
    }

    return ScriptValue(scriptState, toV8(context, scriptState->context()->Global(), scriptState->isolate()));
}

HTMLCanvasElement& Document::getCSSCanvasElement(const String& name)
{
    RefPtrWillBeMember<HTMLCanvasElement>& element = m_cssCanvasElements.add(name, nullptr).storedValue->value;
    if (!element) {
        element = HTMLCanvasElement::create(*this);
        element->setAccelerationDisabled(true);
    }
    return *element;
}

void Document::initDNSPrefetch()
{
    Settings* settings = this->settings();

    m_haveExplicitlyDisabledDNSPrefetch = false;
    m_isDNSPrefetchEnabled = settings && settings->dnsPrefetchingEnabled() && securityOrigin()->protocol() == "http";

    // Inherit DNS prefetch opt-out from parent frame
    if (Document* parent = parentDocument()) {
        if (!parent->isDNSPrefetchEnabled())
            m_isDNSPrefetchEnabled = false;
    }
}

void Document::parseDNSPrefetchControlHeader(const String& dnsPrefetchControl)
{
    if (equalIgnoringCase(dnsPrefetchControl, "on") && !m_haveExplicitlyDisabledDNSPrefetch) {
        m_isDNSPrefetchEnabled = true;
        return;
    }

    m_isDNSPrefetchEnabled = false;
    m_haveExplicitlyDisabledDNSPrefetch = true;
}

void Document::reportBlockedScriptExecutionToInspector(const String& directiveText)
{
    InspectorInstrumentation::scriptExecutionBlockedByCSP(this, directiveText);
}

void Document::addConsoleMessage(PassRefPtrWillBeRawPtr<ConsoleMessage> consoleMessage)
{
    if (!isContextThread()) {
        m_taskRunner->postTask(FROM_HERE, AddConsoleMessageTask::create(consoleMessage->source(), consoleMessage->level(), consoleMessage->message()));
        return;
    }

    if (!m_frame)
        return;

    if (!consoleMessage->scriptState() && consoleMessage->url().isNull() && !consoleMessage->lineNumber()) {
        consoleMessage->setURL(url().string());
        if (!isInDocumentWrite() && scriptableDocumentParser()) {
            ScriptableDocumentParser* parser = scriptableDocumentParser();
            if (parser->isParsingAtLineNumber())
                consoleMessage->setLineNumber(parser->lineNumber().oneBasedInt());
        }
    }
    m_frame->console().addMessage(consoleMessage);
}

// FIXME(crbug.com/305497): This should be removed after ExecutionContext-LocalDOMWindow migration.
void Document::postTask(const WebTraceLocation& location, PassOwnPtr<ExecutionContextTask> task)
{
    m_taskRunner->postTask(location, task);
}

void Document::postInspectorTask(const WebTraceLocation& location, PassOwnPtr<ExecutionContextTask> task)
{
    m_taskRunner->postInspectorTask(location, task);
}

void Document::tasksWereSuspended()
{
    scriptRunner()->suspend();

    if (m_parser)
        m_parser->suspendScheduledTasks();
    if (m_scriptedAnimationController)
        m_scriptedAnimationController->suspend();
}

void Document::tasksWereResumed()
{
    scriptRunner()->resume();

    if (m_parser)
        m_parser->resumeScheduledTasks();
    if (m_scriptedAnimationController)
        m_scriptedAnimationController->resume();

    MutationObserver::resumeSuspendedObservers();
}

// FIXME: suspendScheduledTasks(), resumeScheduledTasks(), tasksNeedSuspension()
// should be moved to LocalDOMWindow once it inherits ExecutionContext
void Document::suspendScheduledTasks()
{
    ExecutionContext::suspendScheduledTasks();
    m_taskRunner->suspend();
}

void Document::resumeScheduledTasks()
{
    ExecutionContext::resumeScheduledTasks();
    m_taskRunner->resume();
}

bool Document::tasksNeedSuspension()
{
    Page* page = this->page();
    return page && page->defersLoading();
}

void Document::addToTopLayer(Element* element, const Element* before)
{
    if (element->isInTopLayer())
        return;

    ASSERT(!m_topLayerElements.contains(element));
    ASSERT(!before || m_topLayerElements.contains(before));
    if (before) {
        size_t beforePosition = m_topLayerElements.find(before);
        m_topLayerElements.insert(beforePosition, element);
    } else {
        m_topLayerElements.append(element);
    }
    element->setIsInTopLayer(true);
}

void Document::removeFromTopLayer(Element* element)
{
    if (!element->isInTopLayer())
        return;
    size_t position = m_topLayerElements.find(element);
    ASSERT(position != kNotFound);
    m_topLayerElements.remove(position);
    element->setIsInTopLayer(false);
}

HTMLDialogElement* Document::activeModalDialog() const
{
    if (m_topLayerElements.isEmpty())
        return 0;
    return toHTMLDialogElement(m_topLayerElements.last().get());
}

void Document::exitPointerLock()
{
    if (!page())
        return;
    if (Element* target = page()->pointerLockController().element()) {
        if (target->document() != this)
            return;
    }
    page()->pointerLockController().requestPointerUnlock();
}

Element* Document::pointerLockElement() const
{
    if (!page() || page()->pointerLockController().lockPending())
        return 0;
    if (Element* element = page()->pointerLockController().element()) {
        if (element->document() == this)
            return element;
    }
    return 0;
}

void Document::suppressLoadEvent()
{
    if (!loadEventFinished())
        m_loadEventProgress = LoadEventCompleted;
}

void Document::decrementLoadEventDelayCount()
{
    ASSERT(m_loadEventDelayCount);
    --m_loadEventDelayCount;

    if (!m_loadEventDelayCount)
        checkLoadEventSoon();
}

void Document::checkLoadEventSoon()
{
    if (frame() && !m_loadEventDelayTimer.isActive())
        m_loadEventDelayTimer.startOneShot(0, FROM_HERE);
}

bool Document::isDelayingLoadEvent()
{
#if ENABLE(OILPAN)
    // Always delay load events until after garbage collection.
    // This way we don't have to explicitly delay load events via
    // incrementLoadEventDelayCount and decrementLoadEventDelayCount in
    // Node destructors.
    if (ThreadState::current()->sweepForbidden()) {
        if (!m_loadEventDelayCount)
            checkLoadEventSoon();
        return true;
    }
#endif
    return m_loadEventDelayCount;
}


void Document::loadEventDelayTimerFired(Timer<Document>*)
{
    if (frame())
        frame()->loader().checkCompleted();
}

void Document::loadPluginsSoon()
{
    // FIXME: Remove this timer once we don't need to compute layout to load plugins.
    if (!m_pluginLoadingTimer.isActive())
        m_pluginLoadingTimer.startOneShot(0, FROM_HERE);
}

void Document::pluginLoadingTimerFired(Timer<Document>*)
{
    updateLayout();
}

ScriptedAnimationController& Document::ensureScriptedAnimationController()
{
    if (!m_scriptedAnimationController) {
        m_scriptedAnimationController = ScriptedAnimationController::create(this);
        // We need to make sure that we don't start up the animation controller on a background tab, for example.
        if (!page())
            m_scriptedAnimationController->suspend();
    }
    return *m_scriptedAnimationController;
}

int Document::requestAnimationFrame(FrameRequestCallback* callback)
{
    return ensureScriptedAnimationController().registerCallback(callback);
}

void Document::cancelAnimationFrame(int id)
{
    if (!m_scriptedAnimationController)
        return;
    m_scriptedAnimationController->cancelCallback(id);
}

void Document::serviceScriptedAnimations(double monotonicAnimationStartTime)
{
    if (!m_scriptedAnimationController)
        return;
    m_scriptedAnimationController->serviceScriptedAnimations(monotonicAnimationStartTime);
}

PassRefPtrWillBeRawPtr<Touch> Document::createTouch(DOMWindow* window, EventTarget* target, int identifier, double pageX, double pageY, double screenX, double screenY, double radiusX, double radiusY, float rotationAngle, float force) const
{
    // Match behavior from when these types were integers, and avoid surprises from someone explicitly
    // passing Infinity/NaN.
    if (!std::isfinite(pageX))
        pageX = 0;
    if (!std::isfinite(pageY))
        pageY = 0;
    if (!std::isfinite(screenX))
        screenX = 0;
    if (!std::isfinite(screenY))
        screenY = 0;
    if (!std::isfinite(radiusX))
        radiusX = 0;
    if (!std::isfinite(radiusY))
        radiusY = 0;
    if (!std::isfinite(rotationAngle))
        rotationAngle = 0;
    if (!std::isfinite(force))
        force = 0;

    // FIXME: It's not clear from the documentation at
    // http://developer.apple.com/library/safari/#documentation/UserExperience/Reference/DocumentAdditionsReference/DocumentAdditions/DocumentAdditions.html
    // when this method should throw and nor is it by inspection of iOS behavior. It would be nice to verify any cases where it throws under iOS
    // and implement them here. See https://bugs.webkit.org/show_bug.cgi?id=47819
    LocalFrame* frame = window && window->isLocalDOMWindow() ? toLocalDOMWindow(window)->frame() : this->frame();
    return Touch::create(frame, target, identifier, FloatPoint(screenX, screenY), FloatPoint(pageX, pageY), FloatSize(radiusX, radiusY), rotationAngle, force);
}

PassRefPtrWillBeRawPtr<TouchList> Document::createTouchList(WillBeHeapVector<RefPtrWillBeMember<Touch>>& touches) const
{
    return TouchList::adopt(touches);
}

DocumentLoader* Document::loader() const
{
    if (!m_frame)
        return 0;

    DocumentLoader* loader = m_frame->loader().documentLoader();
    if (!loader)
        return 0;

    if (m_frame->document() != this)
        return 0;

    return loader;
}

Node* eventTargetNodeForDocument(Document* doc)
{
    if (!doc)
        return 0;
    Node* node = doc->focusedElement();
    if (!node && doc->isPluginDocument()) {
        PluginDocument* pluginDocument = toPluginDocument(doc);
        node = pluginDocument->pluginNode();
    }
    if (!node && doc->isHTMLDocument())
        node = doc->body();
    if (!node)
        node = doc->documentElement();
    return node;
}

void Document::adjustFloatQuadsForScrollAndAbsoluteZoom(Vector<FloatQuad>& quads, LayoutObject& layoutObject)
{
    if (!view())
        return;

    LayoutRect visibleContentRect(view()->visibleContentRect());
    for (size_t i = 0; i < quads.size(); ++i) {
        quads[i].move(-FloatSize(visibleContentRect.x().toFloat(), visibleContentRect.y().toFloat()));
        adjustFloatQuadForAbsoluteZoom(quads[i], layoutObject);
    }
}

void Document::adjustFloatRectForScrollAndAbsoluteZoom(FloatRect& rect, LayoutObject& layoutObject)
{
    if (!view())
        return;

    LayoutRect visibleContentRect(view()->visibleContentRect());
    rect.move(-FloatSize(visibleContentRect.x().toFloat(), visibleContentRect.y().toFloat()));
    adjustFloatRectForAbsoluteZoom(rect, layoutObject);
}

bool Document::hasActiveParser()
{
    return m_activeParserCount || (m_parser && m_parser->processingData());
}

void Document::setContextFeatures(ContextFeatures& features)
{
    m_contextFeatures = PassRefPtrWillBeRawPtr<ContextFeatures>(features);
}

static LayoutObject* nearestCommonHoverAncestor(LayoutObject* obj1, LayoutObject* obj2)
{
    if (!obj1 || !obj2)
        return 0;

    for (LayoutObject* currObj1 = obj1; currObj1; currObj1 = currObj1->hoverAncestor()) {
        for (LayoutObject* currObj2 = obj2; currObj2; currObj2 = currObj2->hoverAncestor()) {
            if (currObj1 == currObj2)
                return currObj1;
        }
    }

    return 0;
}

void Document::updateHoverActiveState(const HitTestRequest& request, Element* innerElement)
{
    ASSERT(!request.readOnly());

    if (request.active() && m_frame)
        m_frame->eventHandler().notifyElementActivated();

    Element* innerElementInDocument = innerElement;
    while (innerElementInDocument && innerElementInDocument->document() != this) {
        innerElementInDocument->document().updateHoverActiveState(request, innerElementInDocument);
        innerElementInDocument = innerElementInDocument->document().ownerElement();
    }

    Element* oldActiveElement = activeHoverElement();
    if (oldActiveElement && !request.active()) {
        // The oldActiveElement layoutObject is null, dropped on :active by setting display: none,
        // for instance. We still need to clear the ActiveChain as the mouse is released.
        for (Node* node = oldActiveElement; node; node = ComposedTreeTraversal::parent(*node)) {
            ASSERT(!node->isTextNode());
            node->setActive(false);
            m_userActionElements.setInActiveChain(node, false);
        }
        setActiveHoverElement(nullptr);
    } else {
        Element* newActiveElement = innerElementInDocument;
        if (!oldActiveElement && newActiveElement && !newActiveElement->isDisabledFormControl() && request.active() && !request.touchMove()) {
            // We are setting the :active chain and freezing it. If future moves happen, they
            // will need to reference this chain.
            for (Node* node = newActiveElement; node; node = ComposedTreeTraversal::parent(*node)) {
                ASSERT(!node->isTextNode());
                m_userActionElements.setInActiveChain(node, true);
            }
            setActiveHoverElement(newActiveElement);
        }
    }
    // If the mouse has just been pressed, set :active on the chain. Those (and only those)
    // nodes should remain :active until the mouse is released.
    bool allowActiveChanges = !oldActiveElement && activeHoverElement();

    // If the mouse is down and if this is a mouse move event, we want to restrict changes in
    // :hover/:active to only apply to elements that are in the :active chain that we froze
    // at the time the mouse went down.
    bool mustBeInActiveChain = request.active() && request.move();

    RefPtrWillBeRawPtr<Node> oldHoverNode = hoverNode();

    // Check to see if the hovered node has changed.
    // If it hasn't, we do not need to do anything.
    Node* newHoverNode = innerElementInDocument;
    while (newHoverNode && !newHoverNode->layoutObject())
        newHoverNode = newHoverNode->parentOrShadowHostNode();

    // Update our current hover node.
    setHoverNode(newHoverNode);

    // We have two different objects. Fetch their layoutObjects.
    LayoutObject* oldHoverObj = oldHoverNode ? oldHoverNode->layoutObject() : 0;
    LayoutObject* newHoverObj = newHoverNode ? newHoverNode->layoutObject() : 0;

    // Locate the common ancestor layout object for the two layoutObjects.
    LayoutObject* ancestor = nearestCommonHoverAncestor(oldHoverObj, newHoverObj);
    RefPtrWillBeRawPtr<Node> ancestorNode(ancestor ? ancestor->node() : 0);

    WillBeHeapVector<RefPtrWillBeMember<Node>, 32> nodesToRemoveFromChain;
    WillBeHeapVector<RefPtrWillBeMember<Node>, 32> nodesToAddToChain;

    if (oldHoverObj != newHoverObj) {
        // If the old hovered node is not nil but it's layoutObject is, it was probably detached as part of the :hover style
        // (for instance by setting display:none in the :hover pseudo-class). In this case, the old hovered element (and its ancestors)
        // must be updated, to ensure it's normal style is re-applied.
        if (oldHoverNode && !oldHoverObj) {
            for (Node* node = oldHoverNode.get(); node; node = node->parentNode()) {
                if (!mustBeInActiveChain || (node->isElementNode() && toElement(node)->inActiveChain()))
                    nodesToRemoveFromChain.append(node);
            }

        }

        // The old hover path only needs to be cleared up to (and not including) the common ancestor;
        for (LayoutObject* curr = oldHoverObj; curr && curr != ancestor; curr = curr->hoverAncestor()) {
            if (curr->node() && !curr->isText() && (!mustBeInActiveChain || curr->node()->inActiveChain()))
                nodesToRemoveFromChain.append(curr->node());
        }

        // TODO(mustaq): The two loops above may push a single node twice into nodesToRemoveFromChain. There must be a better way.
    }

    // Now set the hover state for our new object up to the root.
    for (LayoutObject* curr = newHoverObj; curr; curr = curr->hoverAncestor()) {
        if (curr->node() && !curr->isText() && (!mustBeInActiveChain || curr->node()->inActiveChain()))
            nodesToAddToChain.append(curr->node());
    }

    size_t removeCount = nodesToRemoveFromChain.size();
    for (size_t i = 0; i < removeCount; ++i) {
        nodesToRemoveFromChain[i]->setHovered(false);
    }

    bool sawCommonAncestor = false;
    size_t addCount = nodesToAddToChain.size();
    for (size_t i = 0; i < addCount; ++i) {
        // Elements past the common ancestor do not change hover state, but might change active state.
        if (ancestorNode && nodesToAddToChain[i] == ancestorNode)
            sawCommonAncestor = true;
        if (allowActiveChanges)
            nodesToAddToChain[i]->setActive(true);
        if (!sawCommonAncestor || nodesToAddToChain[i] == m_hoverNode) {
            nodesToAddToChain[i]->setHovered(true);
        }
    }
}

bool Document::haveStylesheetsLoaded() const
{
    return m_styleEngine->haveStylesheetsLoaded();
}

Locale& Document::getCachedLocale(const AtomicString& locale)
{
    AtomicString localeKey = locale;
    if (locale.isEmpty() || !RuntimeEnabledFeatures::langAttributeAwareFormControlUIEnabled())
        return Locale::defaultLocale();
    LocaleIdentifierToLocaleMap::AddResult result = m_localeCache.add(localeKey, nullptr);
    if (result.isNewEntry)
        result.storedValue->value = Locale::create(localeKey);
    return *(result.storedValue->value);
}

Document& Document::ensureTemplateDocument()
{
    if (isTemplateDocument())
        return *this;

    if (m_templateDocument)
        return *m_templateDocument;

    if (isHTMLDocument()) {
        DocumentInit init = DocumentInit::fromContext(contextDocument(), blankURL()).withNewRegistrationContext();
        m_templateDocument = HTMLDocument::create(init);
    } else {
        m_templateDocument = Document::create(DocumentInit(blankURL()));
    }

    m_templateDocument->m_templateDocumentHost = this; // balanced in dtor.

    return *m_templateDocument.get();
}

void Document::didAssociateFormControl(Element* element)
{
    if (!frame() || !frame()->page())
        return;
    m_associatedFormControls.add(element);
    if (!m_didAssociateFormControlsTimer.isActive())
        m_didAssociateFormControlsTimer.startOneShot(0, FROM_HERE);
}

void Document::removeFormAssociation(Element* element)
{
    auto it = m_associatedFormControls.find(element);
    if (it == m_associatedFormControls.end())
        return;
    m_associatedFormControls.remove(it);
    if (m_associatedFormControls.isEmpty())
        m_didAssociateFormControlsTimer.stop();
}

void Document::didAssociateFormControlsTimerFired(Timer<Document>* timer)
{
    ASSERT_UNUSED(timer, timer == &m_didAssociateFormControlsTimer);
    if (!frame() || !frame()->page())
        return;

    WillBeHeapVector<RefPtrWillBeMember<Element>> associatedFormControls;
    copyToVector(m_associatedFormControls, associatedFormControls);

    frame()->page()->chromeClient().didAssociateFormControls(associatedFormControls, frame());
    m_associatedFormControls.clear();
}

float Document::devicePixelRatio() const
{
    return m_frame ? m_frame->devicePixelRatio() : 1.0;
}

void Document::removedStyleSheet(StyleSheet* sheet, StyleResolverUpdateMode updateMode)
{
    // If we're in document teardown, then we don't need this notification of our sheet's removal.
    // styleResolverChanged() is needed even when the document is inactive so that
    // imported docuements (which is inactive) notifies the change to the master document.
    if (isActive())
        styleEngine().modifiedStyleSheet(sheet);
    styleResolverChanged(updateMode);
}

void Document::modifiedStyleSheet(StyleSheet* sheet, StyleResolverUpdateMode updateMode)
{
    // If we're in document teardown, then we don't need this notification of our sheet's removal.
    // styleResolverChanged() is needed even when the document is inactive so that
    // imported docuements (which is inactive) notifies the change to the master document.
    if (isActive())
        styleEngine().modifiedStyleSheet(sheet);
    styleResolverChanged(updateMode);
}

TextAutosizer* Document::textAutosizer()
{
    if (!m_textAutosizer)
        m_textAutosizer = TextAutosizer::create(this);
    return m_textAutosizer.get();
}

void Document::setAutofocusElement(Element* element)
{
    if (!element) {
        m_autofocusElement = nullptr;
        return;
    }
    if (m_hasAutofocused)
        return;
    m_hasAutofocused = true;
    ASSERT(!m_autofocusElement);
    m_autofocusElement = element;
    m_taskRunner->postTask(FROM_HERE, AutofocusTask::create());
}

Element* Document::activeElement() const
{
    if (Element* element = adjustedFocusedElement())
        return element;
    return body();
}

bool Document::hasFocus() const
{
    Page* page = this->page();
    if (!page)
        return false;
    if (!page->focusController().isActive() || !page->focusController().isFocused())
        return false;
    Frame* focusedFrame = page->focusController().focusedFrame();
    if (focusedFrame && focusedFrame->isLocalFrame()) {
        if (toLocalFrame(focusedFrame)->tree().isDescendantOf(frame()))
            return true;
    }
    return false;
}

#if ENABLE(OILPAN)
template<unsigned type>
bool shouldInvalidateNodeListCachesForAttr(const HeapHashSet<WeakMember<const LiveNodeListBase>> nodeLists[], const QualifiedName& attrName)
{
    if (!nodeLists[type].isEmpty() && LiveNodeListBase::shouldInvalidateTypeOnAttributeChange(static_cast<NodeListInvalidationType>(type), attrName))
        return true;
    return shouldInvalidateNodeListCachesForAttr<type + 1>(nodeLists, attrName);
}

template<>
bool shouldInvalidateNodeListCachesForAttr<numNodeListInvalidationTypes>(const HeapHashSet<WeakMember<const LiveNodeListBase>>[], const QualifiedName&)
{
    return false;
}
#else
template<unsigned type>
bool shouldInvalidateNodeListCachesForAttr(const unsigned nodeListCounts[], const QualifiedName& attrName)
{
    if (nodeListCounts[type] && LiveNodeListBase::shouldInvalidateTypeOnAttributeChange(static_cast<NodeListInvalidationType>(type), attrName))
        return true;
    return shouldInvalidateNodeListCachesForAttr<type + 1>(nodeListCounts, attrName);
}

template<>
bool shouldInvalidateNodeListCachesForAttr<numNodeListInvalidationTypes>(const unsigned[], const QualifiedName&)
{
    return false;
}
#endif

bool Document::shouldInvalidateNodeListCaches(const QualifiedName* attrName) const
{
    if (attrName) {
#if ENABLE(OILPAN)
        return shouldInvalidateNodeListCachesForAttr<DoNotInvalidateOnAttributeChanges + 1>(m_nodeLists, *attrName);
#else
        return shouldInvalidateNodeListCachesForAttr<DoNotInvalidateOnAttributeChanges + 1>(m_nodeListCounts, *attrName);
#endif
    }

    for (int type = 0; type < numNodeListInvalidationTypes; ++type) {
#if ENABLE(OILPAN)
        if (!m_nodeLists[type].isEmpty())
#else
        if (m_nodeListCounts[type])
#endif
            return true;
    }

    return false;
}

void Document::invalidateNodeListCaches(const QualifiedName* attrName)
{
    for (const LiveNodeListBase* list : m_listsInvalidatedAtDocument)
        list->invalidateCacheForAttribute(attrName);
}

void Document::platformColorsChanged()
{
    if (!isActive())
        return;

    styleEngine().platformColorsChanged();
}

v8::Local<v8::Object> Document::wrap(v8::Isolate* isolate, v8::Local<v8::Object> creationContext)
{
    // It's possible that no one except for the new wrapper owns this object at
    // this moment, so we have to prevent GC to collect this object until the
    // object gets associated with the wrapper.
    RefPtrWillBeRawPtr<Document> protect(this);

    ASSERT(!DOMDataStore::containsWrapper(this, isolate));

    const WrapperTypeInfo* wrapperType = wrapperTypeInfo();

    if (frame() && frame()->script().initializeMainWorld()) {
        // initializeMainWorld may have created a wrapper for the object, retry from the start.
        v8::Local<v8::Object> wrapper = DOMDataStore::getWrapper(this, isolate);
        if (!wrapper.IsEmpty())
            return wrapper;
    }

    v8::Local<v8::Object> wrapper = V8DOMWrapper::createWrapper(isolate, creationContext, wrapperType, this);
    if (UNLIKELY(wrapper.IsEmpty()))
        return wrapper;

    wrapperType->installConditionallyEnabledProperties(wrapper, isolate);
    return associateWithWrapper(isolate, wrapperType, wrapper);
}

v8::Local<v8::Object> Document::associateWithWrapper(v8::Isolate* isolate, const WrapperTypeInfo* wrapperType, v8::Local<v8::Object> wrapper)
{
    wrapper = V8DOMWrapper::associateObjectWithWrapper(isolate, this, wrapperType, wrapper);
    DOMWrapperWorld& world = DOMWrapperWorld::current(isolate);
    if (world.isMainWorld() && frame())
        frame()->script().windowProxy(world)->updateDocumentWrapper(wrapper);
    return wrapper;
}

bool Document::isPrivilegedContext(String& errorMessage, const PrivilegeContextCheck privilegeContextCheck) const
{
    if (SecurityContext::isSandboxed(SandboxOrigin)) {
        if (!SecurityOrigin::create(url())->isPotentiallyTrustworthy(errorMessage))
            return false;
    } else {
        if (!securityOrigin()->isPotentiallyTrustworthy(errorMessage))
            return false;
    }

    if (privilegeContextCheck == StandardPrivilegeCheck) {
        Document* context = parentDocument();
        while (context) {
            // Skip to the next ancestor if it's a srcdoc.
            if (!context->isSrcdocDocument()) {
                if (context->securityContext().isSandboxed(SandboxOrigin)) {
                    // For a sandboxed origin, use the document's URL.
                    RefPtr<SecurityOrigin> origin = SecurityOrigin::create(context->url());
                    if (!origin->isPotentiallyTrustworthy(errorMessage))
                        return false;
                } else {
                    if (!context->securityOrigin()->isPotentiallyTrustworthy(errorMessage))
                        return false;
                }
            }
            context = context->parentDocument();
        }
    }
    return true;
}

DEFINE_TRACE(Document)
{
#if ENABLE(OILPAN)
    visitor->trace(m_importsController);
    visitor->trace(m_docType);
    visitor->trace(m_implementation);
    visitor->trace(m_autofocusElement);
    visitor->trace(m_focusedElement);
    visitor->trace(m_hoverNode);
    visitor->trace(m_activeHoverElement);
    visitor->trace(m_documentElement);
    visitor->trace(m_titleElement);
    visitor->trace(m_axObjectCache);
    visitor->trace(m_markers);
    visitor->trace(m_cssTarget);
    visitor->trace(m_currentScriptStack);
    visitor->trace(m_scriptRunner);
    visitor->trace(m_listsInvalidatedAtDocument);
    for (int i = 0; i < numNodeListInvalidationTypes; ++i)
        visitor->trace(m_nodeLists[i]);
    visitor->trace(m_cssCanvasElements);
    visitor->trace(m_topLayerElements);
    visitor->trace(m_elemSheet);
    visitor->trace(m_nodeIterators);
    visitor->trace(m_ranges);
    visitor->trace(m_styleEngine);
    visitor->trace(m_formController);
    visitor->trace(m_visitedLinkState);
    visitor->trace(m_frame);
    visitor->trace(m_domWindow);
    visitor->trace(m_fetcher);
    visitor->trace(m_parser);
    visitor->trace(m_contextFeatures);
    visitor->trace(m_styleSheetList);
    visitor->trace(m_mediaQueryMatcher);
    visitor->trace(m_scriptedAnimationController);
    visitor->trace(m_textAutosizer);
    visitor->trace(m_registrationContext);
    visitor->trace(m_customElementMicrotaskRunQueue);
    visitor->trace(m_elementDataCache);
    visitor->trace(m_associatedFormControls);
    visitor->trace(m_useElementsNeedingUpdate);
    visitor->trace(m_layerUpdateSVGFilterElements);
    visitor->trace(m_timers);
    visitor->trace(m_templateDocument);
    visitor->trace(m_templateDocumentHost);
    visitor->trace(m_visibilityObservers);
    visitor->trace(m_userActionElements);
    visitor->trace(m_svgExtensions);
    visitor->trace(m_timeline);
    visitor->trace(m_compositorPendingAnimations);
    visitor->trace(m_contextDocument);
    visitor->trace(m_canvasFontCache);
    WillBeHeapSupplementable<Document>::trace(visitor);
#endif
    TreeScope::trace(visitor);
    ContainerNode::trace(visitor);
    ExecutionContext::trace(visitor);
    DocumentLifecycleNotifier::trace(visitor);
}

template class CORE_TEMPLATE_EXPORT WillBeHeapSupplement<Document>;

} // namespace blink

#ifndef NDEBUG
using namespace blink;
void showLiveDocumentInstances()
{
    WeakDocumentSet& set = liveDocumentSet();
    fprintf(stderr, "There are %u documents currently alive:\n", set.size());
    for (Document* document : set)
        fprintf(stderr, "- Document %p URL: %s\n", document, document->url().string().utf8().data());
}
#endif

/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2008 Alp Toker <alp@atoker.com>
 * Copyright (C) Research In Motion Limited 2009. All rights reserved.
 * Copyright (C) 2011 Kris Jordan <krisjordan@gmail.com>
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
#include "core/loader/FrameLoader.h"

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/SerializedScriptValue.h"
#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/ViewportDescription.h"
#include "core/editing/Editor.h"
#include "core/editing/UndoStack.h"
#include "core/events/GestureEvent.h"
#include "core/events/KeyboardEvent.h"
#include "core/events/MouseEvent.h"
#include "core/events/PageTransitionEvent.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/fetch/ResourceLoader.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/PinchViewport.h"
#include "core/frame/Settings.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/html/HTMLFormElement.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/input/EventHandler.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/loader/DocumentLoadTiming.h"
#include "core/loader/DocumentLoader.h"
#include "core/loader/FormSubmission.h"
#include "core/loader/FrameLoadRequest.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/loader/MixedContentChecker.h"
#include "core/loader/ProgressTracker.h"
#include "core/loader/appcache/ApplicationCacheHost.h"
#include "core/page/ChromeClient.h"
#include "core/page/CreateWindow.h"
#include "core/page/FrameTree.h"
#include "core/page/Page.h"
#include "core/page/WindowFeatures.h"
#include "core/page/scrolling/ScrollingCoordinator.h"
#include "core/svg/graphics/SVGImage.h"
#include "core/xml/parser/XMLDocumentParser.h"
#include "platform/Logging.h"
#include "platform/PluginScriptForbiddenScope.h"
#include "platform/UserGestureIndicator.h"
#include "platform/network/HTTPParsers.h"
#include "platform/network/ResourceRequest.h"
#include "platform/scroll/ScrollAnimator.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "platform/weborigin/SecurityPolicy.h"
#include "public/platform/WebURLRequest.h"
#include "wtf/TemporaryChange.h"
#include "wtf/text/CString.h"
#include "wtf/text/WTFString.h"

using blink::WebURLRequest;

namespace blink {

using namespace HTMLNames;

bool isBackForwardLoadType(FrameLoadType type)
{
    return type == FrameLoadTypeBackForward || type == FrameLoadTypeInitialHistoryLoad;
}

static bool needsHistoryItemRestore(FrameLoadType type)
{
    return type == FrameLoadTypeBackForward || type == FrameLoadTypeReload
        || type == FrameLoadTypeReloadFromOrigin;
}

// static
ResourceRequest FrameLoader::resourceRequestFromHistoryItem(HistoryItem* item,
    ResourceRequestCachePolicy cachePolicy)
{
    RefPtr<FormData> formData = item->formData();
    ResourceRequest request(item->url());
    request.setHTTPReferrer(item->referrer());
    request.setCachePolicy(cachePolicy);
    if (formData) {
        request.setHTTPMethod("POST");
        request.setHTTPBody(formData);
        request.setHTTPContentType(item->formContentType());
        RefPtr<SecurityOrigin> securityOrigin = SecurityOrigin::createFromString(item->referrer().referrer);
        request.addHTTPOriginIfNeeded(securityOrigin->toAtomicString());
    }
    return request;
}

ResourceRequest FrameLoader::resourceRequestForReload(FrameLoadType frameLoadType,
    const KURL& overrideURL, ClientRedirectPolicy clientRedirectPolicy)
{
    ASSERT(frameLoadType == FrameLoadTypeReload || frameLoadType == FrameLoadTypeReloadFromOrigin);
    ResourceRequestCachePolicy cachePolicy = frameLoadType == FrameLoadTypeReloadFromOrigin ?
        ReloadBypassingCache : ReloadIgnoringCacheData;
    if (!m_currentItem)
        return ResourceRequest();
    ResourceRequest request = resourceRequestFromHistoryItem(m_currentItem.get(), cachePolicy);

    // ClientRedirectPolicy is an indication that this load was triggered by
    // some direct interaction with the page. If this reload is not a client
    // redirect, we should reuse the referrer from the original load of the
    // current document. If this reload is a client redirect (e.g., location.reload()),
    // it was initiated by something in the current document and should
    // therefore show the current document's url as the referrer.
    if (clientRedirectPolicy == ClientRedirect) {
        request.setHTTPReferrer(Referrer(m_frame->document()->outgoingReferrer(),
            m_frame->document()->referrerPolicy()));
    }

    if (!overrideURL.isEmpty()) {
        request.setURL(overrideURL);
        request.clearHTTPReferrer();
    }
    request.setSkipServiceWorker(frameLoadType == FrameLoadTypeReloadFromOrigin);
    return request;
}

FrameLoader::FrameLoader(LocalFrame* frame)
    : m_frame(frame)
    , m_progressTracker(ProgressTracker::create(frame))
    , m_loadType(FrameLoadTypeStandard)
    , m_inStopAllLoaders(false)
    , m_checkTimer(this, &FrameLoader::checkTimerFired)
    , m_didAccessInitialDocument(false)
    , m_didAccessInitialDocumentTimer(this, &FrameLoader::didAccessInitialDocumentTimerFired)
    , m_forcedSandboxFlags(SandboxNone)
    , m_dispatchingDidClearWindowObjectInMainWorld(false)
{
}

FrameLoader::~FrameLoader()
{
    // Verify that this FrameLoader has been detached.
    ASSERT(!m_progressTracker);
}

DEFINE_TRACE(FrameLoader)
{
    visitor->trace(m_frame);
    visitor->trace(m_progressTracker);
    visitor->trace(m_documentLoader);
    visitor->trace(m_provisionalDocumentLoader);
    visitor->trace(m_policyDocumentLoader);
    visitor->trace(m_currentItem);
    visitor->trace(m_provisionalItem);
    visitor->trace(m_deferredHistoryLoad);
}

void FrameLoader::init()
{
    ResourceRequest initialRequest(KURL(ParsedURLString, emptyString()));
    initialRequest.setRequestContext(WebURLRequest::RequestContextInternal);
    initialRequest.setFrameType(m_frame->isMainFrame() ? WebURLRequest::FrameTypeTopLevel : WebURLRequest::FrameTypeNested);
    m_provisionalDocumentLoader = client()->createDocumentLoader(m_frame, initialRequest, SubstituteData());
    m_provisionalDocumentLoader->startLoadingMainResource();
    m_frame->document()->cancelParsing();
    m_stateMachine.advanceTo(FrameLoaderStateMachine::DisplayingInitialEmptyDocument);
}

FrameLoaderClient* FrameLoader::client() const
{
    return static_cast<FrameLoaderClient*>(m_frame->client());
}

void FrameLoader::setDefersLoading(bool defers)
{
    if (m_documentLoader)
        m_documentLoader->setDefersLoading(defers);
    if (m_provisionalDocumentLoader)
        m_provisionalDocumentLoader->setDefersLoading(defers);
    if (m_policyDocumentLoader)
        m_policyDocumentLoader->setDefersLoading(defers);

    if (Document* document = m_frame->document()) {
        if (defers)
            document->suspendScheduledTasks();
        else
            document->resumeScheduledTasks();
    }

    if (!defers) {
        if (m_deferredHistoryLoad) {
            load(FrameLoadRequest(nullptr, m_deferredHistoryLoad->m_request),
                m_deferredHistoryLoad->m_loadType, m_deferredHistoryLoad->m_item.get(),
                m_deferredHistoryLoad->m_historyLoadType);
            m_deferredHistoryLoad.clear();
        }
        m_frame->navigationScheduler().startTimer();
        scheduleCheckCompleted();
    }
}

void FrameLoader::saveScrollState()
{
    if (!m_currentItem || !m_frame->view())
        return;

    // Shouldn't clobber anything if we might still restore later.
    if (needsHistoryItemRestore(m_loadType) && !m_frame->view()->wasScrolledByUser())
        return;

    if (ScrollableArea* layoutScrollableArea = m_frame->view()->layoutViewportScrollableArea())
        m_currentItem->setScrollPoint(layoutScrollableArea->scrollPosition());
    m_currentItem->setPinchViewportScrollPoint(m_frame->host()->pinchViewport().visibleRect().location());

    if (m_frame->isMainFrame())
        m_currentItem->setPageScaleFactor(m_frame->page()->pageScaleFactor());

    client()->didUpdateCurrentHistoryItem();
}

void FrameLoader::dispatchUnloadEvent()
{
    saveScrollState();

    if (m_frame->document() && !SVGImage::isInSVGImage(m_frame->document()))
        m_frame->document()->dispatchUnloadEvents();

    if (Page* page = m_frame->page())
        page->undoStack().didUnloadFrame(*m_frame);
}

void FrameLoader::didExplicitOpen()
{
    // Calling document.open counts as committing the first real document load.
    if (!m_stateMachine.committedFirstRealDocumentLoad())
        m_stateMachine.advanceTo(FrameLoaderStateMachine::CommittedFirstRealLoad);

    // Only model a document.open() as part of a navigation if its parent is not done
    // or in the process of completing.
    if (Frame* parent = m_frame->tree().parent()) {
        if ((parent->isLocalFrame() && toLocalFrame(parent)->document()->loadEventStillNeeded())
            || (parent->isRemoteFrame() && parent->isLoading())) {
            m_progressTracker->progressStarted();
        }
    }

    // Prevent window.open(url) -- eg window.open("about:blank") -- from blowing away results
    // from a subsequent window.document.open / window.document.write call.
    // Canceling redirection here works for all cases because document.open
    // implicitly precedes document.write.
    m_frame->navigationScheduler().cancel();
}

void FrameLoader::clear()
{
    // clear() is called during (Local)Frame detachment or when
    // reusing a FrameLoader by putting a new Document within it
    // (DocumentLoader::ensureWriter().)
    if (m_stateMachine.creatingInitialEmptyDocument())
        return;

    m_frame->editor().clear();
    m_frame->document()->cancelParsing();
    m_frame->document()->removeFocusedElementOfSubtree(m_frame->document());
    m_frame->selection().prepareForDestruction();
    m_frame->eventHandler().clear();
    if (m_frame->view())
        m_frame->view()->clear();

    m_frame->script().enableEval();

    m_frame->navigationScheduler().cancel();

    m_checkTimer.stop();

    if (m_stateMachine.isDisplayingInitialEmptyDocument())
        m_stateMachine.advanceTo(FrameLoaderStateMachine::CommittedFirstRealLoad);
}

// This is only called by ScriptController::executeScriptIfJavaScriptURL
// and always contains the result of evaluating a javascript: url.
// This is the <iframe src="javascript:'html'"> case.
void FrameLoader::replaceDocumentWhileExecutingJavaScriptURL(const String& source, Document* ownerDocument)
{
    if (!m_frame->document()->loader())
        return;

    // DocumentLoader::replaceDocumentWhileExecutingJavaScriptURL can cause the DocumentLoader to get deref'ed and possible destroyed,
    // so protect it with a RefPtr.
    RefPtrWillBeRawPtr<DocumentLoader> documentLoader(m_frame->document()->loader());

    UseCounter::count(*m_frame->document(), UseCounter::ReplaceDocumentViaJavaScriptURL);

    // Prepare a DocumentInit before clearing the frame, because it may need to
    // inherit an aliased security context.
    DocumentInit init(m_frame->document()->url(), m_frame);
    init.withNewRegistrationContext();

    stopAllLoaders();
    m_frame->detachChildren();
    m_frame->document()->detach();
    clear();

    // detachChildren() potentially detaches the frame from the document. The
    // loading cannot continue in that case.
    if (!m_frame->page())
        return;

    client()->transitionToCommittedForNewPage();
    documentLoader->replaceDocumentWhileExecutingJavaScriptURL(init, source, ownerDocument);
}

void FrameLoader::receivedMainResourceRedirect(const KURL& newURL)
{
    client()->dispatchDidReceiveServerRedirectForProvisionalLoad();
    // If a back/forward navigation redirects cross-origin, don't reuse any state from the HistoryItem.
    if (m_provisionalItem && !SecurityOrigin::create(m_provisionalItem->url())->isSameSchemeHostPort(SecurityOrigin::create(newURL).get()))
        m_provisionalItem.clear();
}

void FrameLoader::setHistoryItemStateForCommit(HistoryCommitType historyCommitType, HistoryNavigationType navigationType)
{
    RefPtrWillBeRawPtr<HistoryItem> oldItem = m_currentItem;
    if (historyCommitType == BackForwardCommit && m_provisionalItem)
        m_currentItem = m_provisionalItem.release();
    else
        m_currentItem = HistoryItem::create();
    m_currentItem->setURL(m_documentLoader->urlForHistory());
    m_currentItem->setDocumentState(m_frame->document()->formElementsState());
    m_currentItem->setTarget(m_frame->tree().uniqueName());
    m_currentItem->setReferrer(SecurityPolicy::generateReferrer(m_documentLoader->request().referrerPolicy(), m_currentItem->url(), m_documentLoader->request().httpReferrer()));
    m_currentItem->setFormInfoFromRequest(m_documentLoader->request());

    // Don't propagate state from the old item to the new item if there isn't an old item (obviously),
    // or if this is a back/forward navigation, since we explicitly want to restore the state we just
    // committed.
    if (!oldItem || historyCommitType == BackForwardCommit)
        return;
    // Don't propagate state from the old item if this is a different-document navigation, unless the before
    // and after pages are logically related. This means they have the same url (ignoring fragment) and
    // the new item was loaded via reload or client redirect.
    if (navigationType == HistoryNavigationType::DifferentDocument && (historyCommitType != HistoryInertCommit || !equalIgnoringFragmentIdentifier(oldItem->url(), m_currentItem->url())))
        return;
    m_currentItem->setDocumentSequenceNumber(oldItem->documentSequenceNumber());
    m_currentItem->setScrollPoint(oldItem->scrollPoint());
    m_currentItem->setPinchViewportScrollPoint(oldItem->pinchViewportScrollPoint());
    m_currentItem->setPageScaleFactor(oldItem->pageScaleFactor());
    // The item sequence number determines whether items are "the same", such back/forward navigation
    // between items with the same item sequence number is a no-op. Only treat this as identical if the
    // navigation did not create a back/forward entry and the url is identical or it was loaded via
    // history.replaceState().
    if (historyCommitType == HistoryInertCommit && (navigationType == HistoryNavigationType::HistoryApi || oldItem->url() == m_currentItem->url())) {
        m_currentItem->setStateObject(oldItem->stateObject());
        m_currentItem->setItemSequenceNumber(oldItem->itemSequenceNumber());
    }
}

// 加入isRedirect标志以细分FrameLoadTypeRedirectWithLockedBackForwardList类型的处理
static HistoryCommitType loadTypeToCommitType(FrameLoadType type,bool isRedirect)
{
    switch (type) {
    case FrameLoadTypeStandard:
        return StandardCommit;
    case FrameLoadTypeInitialInChildFrame:
    case FrameLoadTypeInitialHistoryLoad:
        return InitialCommitInChildFrame;
    case FrameLoadTypeBackForward:
        return BackForwardCommit;
    case FrameLoadTypeRedirectWithLockedBackForwardList:
        if (isRedirect)
            return HistoryInertCommit;
        else
            return InitialCommitInChildFrame;
    default:
        break;
    }
    return HistoryInertCommit;
}

void FrameLoader::receivedFirstData()
{
    if (m_stateMachine.creatingInitialEmptyDocument())
        return;

    HistoryCommitType historyCommitType = loadTypeToCommitType(m_loadType, documentLoader()->isClientRedirect());
    if (historyCommitType == StandardCommit && (m_documentLoader->urlForHistory().isEmpty() || (opener() && !m_currentItem && m_documentLoader->originalRequest().url().isEmpty())))
        historyCommitType = HistoryInertCommit;
    else if (historyCommitType == InitialCommitInChildFrame && MixedContentChecker::isMixedContent(m_frame->tree().top()->securityContext()->securityOrigin(), m_documentLoader->url()))
        historyCommitType = HistoryInertCommit;
    setHistoryItemStateForCommit(historyCommitType, HistoryNavigationType::DifferentDocument);

    if (!m_stateMachine.committedMultipleRealLoads() && m_loadType == FrameLoadTypeStandard)
        m_stateMachine.advanceTo(FrameLoaderStateMachine::CommittedMultipleRealLoads);

    client()->dispatchDidCommitLoad(m_currentItem.get(), historyCommitType);

    TRACE_EVENT1("devtools.timeline", "CommitLoad", "data", InspectorCommitLoadEvent::data(m_frame));
    InspectorInstrumentation::didCommitLoad(m_frame, m_documentLoader.get());
    m_frame->page()->didCommitLoad(m_frame);
    dispatchDidClearDocumentOfWindowObject();
}

void FrameLoader::didBeginDocument(bool dispatch)
{
    ASSERT(m_frame);
    ASSERT(m_frame->document());
    ASSERT(m_frame->document()->fetcher());
    m_frame->document()->setReadyState(Document::Loading);

    if (dispatch)
        dispatchDidClearDocumentOfWindowObject();

    m_frame->document()->initContentSecurityPolicy(m_documentLoader ? m_documentLoader->releaseContentSecurityPolicy() : ContentSecurityPolicy::create());
    if (m_documentLoader)
        m_frame->document()->setClientHintsPreferences(m_documentLoader->clientHintsPreferences());

    Settings* settings = m_frame->document()->settings();
    if (settings) {
        m_frame->document()->fetcher()->setImagesEnabled(settings->imagesEnabled());
        m_frame->document()->fetcher()->setAutoLoadImages(settings->loadsImagesAutomatically());
    }

    if (m_documentLoader) {
        const AtomicString& dnsPrefetchControl = m_documentLoader->response().httpHeaderField("X-DNS-Prefetch-Control");
        if (!dnsPrefetchControl.isEmpty())
            m_frame->document()->parseDNSPrefetchControlHeader(dnsPrefetchControl);

        String headerContentLanguage = m_documentLoader->response().httpHeaderField("Content-Language");
        if (!headerContentLanguage.isEmpty()) {
            size_t commaIndex = headerContentLanguage.find(',');
            headerContentLanguage.truncate(commaIndex); // kNotFound == -1 == don't truncate
            headerContentLanguage = headerContentLanguage.stripWhiteSpace(isHTMLSpace<UChar>);
            if (!headerContentLanguage.isEmpty())
                m_frame->document()->setContentLanguage(AtomicString(headerContentLanguage));
        }
    }

    if (m_provisionalItem && isBackForwardLoadType(m_loadType))
        m_frame->document()->setStateForNewFormElements(m_provisionalItem->documentState());

    client()->didCreateNewDocument();
}

void FrameLoader::finishedParsing()
{
    if (m_stateMachine.creatingInitialEmptyDocument())
        return;

    // This can be called from the LocalFrame's destructor, in which case we shouldn't protect ourselves
    // because doing so will cause us to re-enter the destructor when protector goes out of scope.
    // Null-checking the FrameView indicates whether or not we're in the destructor.
    RefPtrWillBeRawPtr<LocalFrame> protect(m_frame->view() ? m_frame.get() : nullptr);

    if (client())
        client()->dispatchDidFinishDocumentLoad();

    checkCompleted();

    if (!m_frame->view())
        return; // We are being destroyed by something checkCompleted called.

    // Check if the scrollbars are really needed for the content.
    // If not, remove them, relayout, and repaint.
    m_frame->view()->restoreScrollbar();
    processFragment(m_frame->document()->url(), NavigationToDifferentDocument);
}

static bool allDescendantsAreComplete(Frame* frame)
{
    for (Frame* child = frame->tree().firstChild(); child; child = child->tree().traverseNext(frame)) {
        if (child->isLoading())
            return false;
    }
    return true;
}

bool FrameLoader::allAncestorsAreComplete() const
{
    for (Frame* ancestor = m_frame; ancestor; ancestor = ancestor->tree().parent()) {
        if (ancestor->isLoading())
            return false;
    }
    return true;
}

static bool shouldComplete(Document* document)
{
    if (!document->frame())
        return false;
    if (document->parsing() || document->isInDOMContentLoaded())
        return false;
    if (!document->haveImportsLoaded())
        return false;
    if (document->fetcher()->requestCount())
        return false;
    if (document->isDelayingLoadEvent())
        return false;
    return allDescendantsAreComplete(document->frame());
}

static bool shouldSendCompleteNotifications(LocalFrame* frame)
{
    // Don't send stop notifications for inital empty documents, since they don't generate start notifications.
    if (!frame->loader().stateMachine()->committedFirstRealDocumentLoad())
        return false;

    // FIXME: We might have already sent stop notifications and be re-completing.
    if (!frame->isLoading())
        return false;

    // The readystatechanged or load event may have disconnected this frame.
    if (!frame->client())
        return false;

    // An event might have restarted a child frame.
    if (!allDescendantsAreComplete(frame))
        return false;

    // An event might have restarted this frame by scheduling a new navigation.
    if (frame->loader().provisionalDocumentLoader())
        return false;

    // We might have declined to run the load event due to an imminent content-initiated navigation.
    if (!frame->document()->loadEventFinished())
        return false;
    return true;
}

void FrameLoader::checkCompleted()
{
    RefPtrWillBeRawPtr<LocalFrame> protect(m_frame.get());
    if (!shouldComplete(m_frame->document()))
        return;

    // OK, completed.
    m_frame->document()->setReadyState(Document::Complete);
    if (m_frame->document()->loadEventStillNeeded())
        m_frame->document()->implicitClose();

    m_frame->navigationScheduler().startTimer();

    if (m_frame->view())
        m_frame->view()->handleLoadCompleted();

    if (shouldSendCompleteNotifications(m_frame)) {
        m_loadType = FrameLoadTypeStandard;
        m_progressTracker->progressCompleted();
        // Retry restoring scroll offset since finishing loading disables content
        // size clamping.
        restoreScrollPositionAndViewState();
        m_frame->localDOMWindow()->finishedLoading();

        // Report mobile vs. desktop page statistics. This will only report on Android.
        if (m_frame->isMainFrame())
            m_frame->document()->viewportDescription().reportMobilePageStats(m_frame);
        client()->dispatchDidFinishLoad();
    }

    Frame* parent = m_frame->tree().parent();
    if (parent && parent->isLocalFrame())
        toLocalFrame(parent)->loader().checkCompleted();
}

void FrameLoader::checkTimerFired(Timer<FrameLoader>*)
{
    if (Page* page = m_frame->page()) {
        if (page->defersLoading())
            return;
    }
    checkCompleted();
}

void FrameLoader::scheduleCheckCompleted()
{
    if (!m_checkTimer.isActive())
        m_checkTimer.startOneShot(0, FROM_HERE);
}

Frame* FrameLoader::opener()
{
    return client() ? client()->opener() : 0;
}

void FrameLoader::setOpener(LocalFrame* opener)
{
    // If the frame is already detached, the opener has already been cleared.
    if (client())
        client()->setOpener(opener);
}

bool FrameLoader::allowPlugins(ReasonForCallingAllowPlugins reason)
{
    // With Oilpan, a FrameLoader might be accessed after the
    // FrameHost has been detached. FrameClient will not be
    // accessible, so bail early.
    if (!client())
        return false;
    Settings* settings = m_frame->settings();
    bool allowed = client()->allowPlugins(settings && settings->pluginsEnabled());
    if (!allowed && reason == AboutToInstantiatePlugin)
        client()->didNotAllowPlugins();
    return allowed;
}

void FrameLoader::updateForSameDocumentNavigation(const KURL& newURL, SameDocumentNavigationSource sameDocumentNavigationSource, PassRefPtr<SerializedScriptValue> data, HistoryScrollRestorationType scrollRestorationType, FrameLoadType type)
{
    // Update the data source's request with the new URL to fake the URL change
    m_frame->document()->setURL(newURL);
    documentLoader()->setReplacesCurrentHistoryItem(type != FrameLoadTypeStandard);
    documentLoader()->updateForSameDocumentNavigation(newURL, sameDocumentNavigationSource);

    // Generate start and stop notifications only when loader is completed so that we
    // don't fire them for fragment redirection that happens in window.onload handler.
    // See https://bugs.webkit.org/show_bug.cgi?id=31838
    if (m_frame->document()->loadEventFinished())
        client()->didStartLoading(NavigationWithinSameDocument);

    HistoryCommitType historyCommitType = loadTypeToCommitType(type, documentLoader()->isClientRedirect());
    if (!m_currentItem)
        historyCommitType = HistoryInertCommit;

    setHistoryItemStateForCommit(historyCommitType, sameDocumentNavigationSource == SameDocumentNavigationHistoryApi ? HistoryNavigationType::HistoryApi : HistoryNavigationType::Fragment);
    if (sameDocumentNavigationSource == SameDocumentNavigationHistoryApi) {
        m_currentItem->setStateObject(data);
        m_currentItem->setScrollRestorationType(scrollRestorationType);
    }
    client()->dispatchDidNavigateWithinPage(m_currentItem.get(), historyCommitType);
    client()->dispatchDidReceiveTitle(m_frame->document()->title());
    if (m_frame->document()->loadEventFinished())
        client()->didStopLoading();
}

void FrameLoader::detachDocumentLoader(RefPtrWillBeMember<DocumentLoader>& loader)
{
    if (!loader)
        return;

    loader->detachFromFrame();
    loader = nullptr;
}

void FrameLoader::loadInSameDocument(const KURL& url, PassRefPtr<SerializedScriptValue> stateObject, FrameLoadType type, ClientRedirectPolicy clientRedirect)
{
    // If we have a state object, we cannot also be a new navigation.
    ASSERT(!stateObject || type == FrameLoadTypeBackForward);

    // If we have a provisional request for a different document, a fragment scroll should cancel it.
    if (m_provisionalDocumentLoader) {
        m_provisionalDocumentLoader->stopLoading();
        detachDocumentLoader(m_provisionalDocumentLoader);
        if (!m_frame->host())
            return;
    }
    m_loadType = type;
    saveScrollState();

    KURL oldURL = m_frame->document()->url();
    // If we were in the autoscroll/panScroll mode we want to stop it before following the link to the anchor
    bool hashChange = equalIgnoringFragmentIdentifier(url, oldURL) && url.fragmentIdentifier() != oldURL.fragmentIdentifier();
    if (hashChange) {
        m_frame->eventHandler().stopAutoscroll();
        m_frame->localDOMWindow()->enqueueHashchangeEvent(oldURL, url);
    }
    m_documentLoader->setIsClientRedirect(clientRedirect == ClientRedirect);
    updateForSameDocumentNavigation(url, SameDocumentNavigationDefault, nullptr, ScrollRestorationAuto, type);

    m_frame->view()->setWasScrolledByUser(false);

    // We need to scroll to the fragment whether or not a hash change occurred, since
    // the user might have scrolled since the previous navigation.
    processFragment(url, NavigationWithinSameDocument);
    checkCompleted();

    m_frame->localDOMWindow()->statePopped(stateObject ? stateObject : SerializedScriptValue::nullValue());
}

void FrameLoader::setReferrerForFrameRequest(ResourceRequest& request, ShouldSendReferrer shouldSendReferrer, Document* originDocument)
{
    if (!originDocument)
        return;
    // FIXME: This should be an assertion, but there's some plugin code in the chromium repo
    // that both determines its own referrer and expects to be associated with an originDocument.
    if (!request.httpReferrer().isEmpty())
        return;
    if (shouldSendReferrer == NeverSendReferrer)
        return;

    // Always use the initiating document to generate the referrer.
    // We need to generateReferrer(), because we haven't enforced ReferrerPolicy or https->http
    // referrer suppression yet.
    Referrer referrer = SecurityPolicy::generateReferrer(originDocument->referrerPolicy(), request.url(), originDocument->outgoingReferrer());

    request.setHTTPReferrer(referrer);
    RefPtr<SecurityOrigin> referrerOrigin = SecurityOrigin::createFromString(referrer.referrer);
    request.addHTTPOriginIfNeeded(referrerOrigin->toAtomicString());
}

FrameLoadType FrameLoader::determineFrameLoadType(const FrameLoadRequest& request)
{
    if (m_frame->tree().parent() && !m_stateMachine.committedFirstRealDocumentLoad())
        return FrameLoadTypeInitialInChildFrame;
    if (!m_frame->tree().parent() && !client()->backForwardLength())
        return FrameLoadTypeStandard;
    if (m_provisionalDocumentLoader && request.substituteData().failingURL() == m_provisionalDocumentLoader->url() && m_loadType == FrameLoadTypeBackForward)
        return FrameLoadTypeBackForward;
    if (request.resourceRequest().cachePolicy() == ReloadIgnoringCacheData)
        return FrameLoadTypeReload;
    if (request.resourceRequest().cachePolicy() == ReloadBypassingCache)
        return FrameLoadTypeReloadFromOrigin;
    // From the HTML5 spec for location.assign():
    //  "If the browsing context's session history contains only one Document,
    //   and that was the about:blank Document created when the browsing context
    //   was created, then the navigation must be done with replacement enabled."
    if (request.lockBackForwardList()
        || (!m_stateMachine.committedMultipleRealLoads()
            && equalIgnoringCase(m_frame->document()->url(), blankURL())))
        return FrameLoadTypeRedirectWithLockedBackForwardList;
    if (!request.originDocument() && request.resourceRequest().url() == m_documentLoader->urlForHistory())
        return FrameLoadTypeSame;
    if (request.substituteData().failingURL() == m_documentLoader->urlForHistory() && m_loadType == FrameLoadTypeReload)
        return FrameLoadTypeReload;
    return FrameLoadTypeStandard;
}

bool FrameLoader::prepareRequestForThisFrame(FrameLoadRequest& request)
{
    // If no origin Document* was specified, skip remaining security checks and assume the caller has fully initialized the FrameLoadRequest.
    if (!request.originDocument())
        return true;

    KURL url = request.resourceRequest().url();
    if (m_frame->script().executeScriptIfJavaScriptURL(url))
        return false;

    if (!request.originDocument()->securityOrigin()->canDisplay(url)) {
        reportLocalLoadFailed(m_frame, url.elidedString());
        return false;
    }

    if (!request.form() && request.frameName().isEmpty())
        request.setFrameName(m_frame->document()->baseTarget());
    return true;
}

static bool shouldOpenInNewWindow(Frame* targetFrame, const FrameLoadRequest& request, NavigationPolicy policy)
{
    if (!RuntimeEnabledFeatures::navigationToNewWindowEnabled())
        return false;

    if (!targetFrame && !request.frameName().isEmpty())
        return true;
    // FIXME: This case is a workaround for the fact that ctrl+clicking a form submission incorrectly
    // sends as a GET rather than a POST if it creates a new window in a different process.
    return request.form() && policy != NavigationPolicyCurrentTab;
}

static NavigationType determineNavigationType(FrameLoadType frameLoadType, bool isFormSubmission, bool haveEvent)
{
    bool isReload = frameLoadType == FrameLoadTypeReload || frameLoadType == FrameLoadTypeReloadFromOrigin;
    bool isBackForward = isBackForwardLoadType(frameLoadType);
    if (isFormSubmission)
        return (isReload || isBackForward) ? NavigationTypeFormResubmitted : NavigationTypeFormSubmitted;
    if (haveEvent)
        return NavigationTypeLinkClicked;
    if (isReload)
        return NavigationTypeReload;
    if (isBackForward)
        return NavigationTypeBackForward;
    return NavigationTypeOther;
}

static WebURLRequest::RequestContext determineRequestContextFromNavigationType(const NavigationType navigationType)
{
    switch (navigationType) {
    case NavigationTypeLinkClicked:
        return WebURLRequest::RequestContextHyperlink;

    case NavigationTypeOther:
        return WebURLRequest::RequestContextLocation;

    case NavigationTypeFormResubmitted:
    case NavigationTypeFormSubmitted:
        return WebURLRequest::RequestContextForm;

    case NavigationTypeBackForward:
    case NavigationTypeReload:
        return WebURLRequest::RequestContextInternal;
    }
    ASSERT_NOT_REACHED();
    return WebURLRequest::RequestContextHyperlink;
}

static NavigationPolicy navigationPolicyForRequest(const FrameLoadRequest& request)
{
    NavigationPolicy policy = NavigationPolicyCurrentTab;
    Event* event = request.triggeringEvent();
    if (!event)
        return policy;

    if (request.form() && event->underlyingEvent())
        event = event->underlyingEvent();

    if (event->isMouseEvent()) {
        MouseEvent* mouseEvent = toMouseEvent(event);
        navigationPolicyFromMouseEvent(mouseEvent->button(), mouseEvent->ctrlKey(), mouseEvent->shiftKey(), mouseEvent->altKey(), mouseEvent->metaKey(), &policy);
    } else if (event->isKeyboardEvent()) {
        // The click is simulated when triggering the keypress event.
        KeyboardEvent* keyEvent = toKeyboardEvent(event);
        navigationPolicyFromMouseEvent(0, keyEvent->ctrlKey(), keyEvent->shiftKey(), keyEvent->altKey(), keyEvent->metaKey(), &policy);
    } else if (event->isGestureEvent()) {
        // The click is simulated when triggering the gesture-tap event
        GestureEvent* gestureEvent = toGestureEvent(event);
        navigationPolicyFromMouseEvent(0, gestureEvent->ctrlKey(), gestureEvent->shiftKey(), gestureEvent->altKey(), gestureEvent->metaKey(), &policy);
    }
    return policy;
}

void FrameLoader::load(const FrameLoadRequest& passedRequest, FrameLoadType frameLoadType,
    HistoryItem* historyItem, HistoryLoadType historyLoadType)
{
    ASSERT(m_frame->document());

    RefPtrWillBeRawPtr<LocalFrame> protect(m_frame.get());

    if (m_inStopAllLoaders)
        return;

    if (m_frame->page()->defersLoading() && isBackForwardLoadType(frameLoadType)) {
        m_deferredHistoryLoad = DeferredHistoryLoad::create(passedRequest.resourceRequest(), historyItem, frameLoadType, historyLoadType);
        return;
    }

    FrameLoadRequest request(passedRequest);
    request.resourceRequest().setHasUserGesture(UserGestureIndicator::processingUserGesture());

    if (!prepareRequestForThisFrame(request))
        return;

    RefPtrWillBeRawPtr<Frame> targetFrame = request.form() ? nullptr : m_frame->findFrameForNavigation(AtomicString(request.frameName()), *m_frame);

    if (isBackForwardLoadType(frameLoadType)) {
        ASSERT(historyItem);
        m_provisionalItem = historyItem;
    }

    if (targetFrame && targetFrame.get() != m_frame) {
        bool wasInSamePage = targetFrame->page() == m_frame->page();

        request.setFrameName("_self");
        targetFrame->navigate(request);
        Page* page = targetFrame->page();
        if (!wasInSamePage && page)
            page->chromeClient().focus();
        return;
    }

    setReferrerForFrameRequest(request.resourceRequest(), request.shouldSendReferrer(), request.originDocument());

    FrameLoadType newLoadType = (frameLoadType == FrameLoadTypeStandard) ?
        determineFrameLoadType(request) : frameLoadType;
    NavigationPolicy policy = navigationPolicyForRequest(request);
    if (shouldOpenInNewWindow(targetFrame.get(), request, policy)) {
        if (policy == NavigationPolicyDownload) {
            client()->loadURLExternally(request.resourceRequest(), NavigationPolicyDownload);
        } else {
            request.resourceRequest().setFrameType(WebURLRequest::FrameTypeAuxiliary);
            createWindowForRequest(request, *m_frame, policy, request.shouldSendReferrer());
        }
        return;
    }

    const KURL& url = request.resourceRequest().url();
    bool sameDocumentHistoryNavigation =
        isBackForwardLoadType(newLoadType) && historyLoadType == HistorySameDocumentLoad;
    bool sameDocumentNavigation = policy == NavigationPolicyCurrentTab
        && shouldPerformFragmentNavigation(
            request.form(), request.resourceRequest().httpMethod(), newLoadType, url);

    // Perform same document navigation.
    if (sameDocumentHistoryNavigation || sameDocumentNavigation) {
        ASSERT(historyItem || !sameDocumentHistoryNavigation);
        RefPtr<SerializedScriptValue> stateObject = sameDocumentHistoryNavigation ?
            historyItem->stateObject() : nullptr;

        if (!sameDocumentHistoryNavigation) {
            m_documentLoader->setNavigationType(determineNavigationType(
                newLoadType, false, request.triggeringEvent()));
            if (shouldTreatURLAsSameAsCurrent(url))
                newLoadType = FrameLoadTypeRedirectWithLockedBackForwardList;
        }

        loadInSameDocument(url, stateObject, newLoadType, request.clientRedirect());

        if (sameDocumentHistoryNavigation)
            restoreScrollPositionAndViewState();
        return;
    }

    // Perform navigation to a different document.
    bool sameURL = url == m_documentLoader->urlForHistory();
    startLoad(request, newLoadType, policy);

    // Example of this case are sites that reload the same URL with a different cookie
    // driving the generated content, or a master frame with links that drive a target
    // frame, where the user has clicked on the same link repeatedly.
    if (sameURL
        && !isBackForwardLoadType(frameLoadType)
        && newLoadType != FrameLoadTypeReload
        && newLoadType != FrameLoadTypeReloadFromOrigin
        && request.resourceRequest().httpMethod() != "POST") {
        m_loadType = FrameLoadTypeSame;
    }
}

SubstituteData FrameLoader::defaultSubstituteDataForURL(const KURL& url)
{
    if (!shouldTreatURLAsSrcdocDocument(url))
        return SubstituteData();
    String srcdoc = m_frame->deprecatedLocalOwner()->fastGetAttribute(srcdocAttr);
    ASSERT(!srcdoc.isNull());
    CString encodedSrcdoc = srcdoc.utf8();
    return SubstituteData(SharedBuffer::create(encodedSrcdoc.data(), encodedSrcdoc.length()), "text/html", "UTF-8", KURL());
}

void FrameLoader::reportLocalLoadFailed(LocalFrame* frame, const String& url)
{
    ASSERT(!url.isEmpty());
    if (!frame)
        return;

    frame->document()->addConsoleMessage(ConsoleMessage::create(SecurityMessageSource, ErrorMessageLevel, "Not allowed to load local resource: " + url));
}

void FrameLoader::stopAllLoaders()
{
    if (m_frame->document()->pageDismissalEventBeingDispatched() != Document::NoDismissal)
        return;

    // If this method is called from within this method, infinite recursion can occur (3442218). Avoid this.
    if (m_inStopAllLoaders)
        return;

    // Calling stopLoading() on the provisional document loader can blow away
    // the frame from underneath.
    RefPtrWillBeRawPtr<LocalFrame> protect(m_frame.get());

    m_inStopAllLoaders = true;

    for (RefPtrWillBeRawPtr<Frame> child = m_frame->tree().firstChild(); child; child = child->tree().nextSibling()) {
        if (child->isLocalFrame())
            toLocalFrame(child.get())->loader().stopAllLoaders();
    }

    m_frame->document()->suppressLoadEvent();
    // FIXME: This is an odd set of steps to shut down parsing and it's unclear why it works.
    // It's also unclear why other steps don't work.
    if (m_frame->document()->parsing()) {
        finishedParsing();
        m_frame->document()->setParsingState(Document::FinishedParsing);
    }
    m_frame->document()->setReadyState(Document::Complete);
    if (m_provisionalDocumentLoader)
        m_provisionalDocumentLoader->stopLoading();
    if (m_documentLoader)
        m_documentLoader->stopLoading();

    detachDocumentLoader(m_provisionalDocumentLoader);
    detachDocumentLoader(m_policyDocumentLoader);

    m_checkTimer.stop();
    m_frame->navigationScheduler().cancel();

    m_inStopAllLoaders = false;

    // detachFromParent() can be called multiple times on same LocalFrame, which
    // means we may no longer have a FrameLoaderClient to talk to.
    if (client())
        client()->didStopAllLoaders();
}

void FrameLoader::didAccessInitialDocument()
{
    // We only need to notify the client once, and only for the main frame.
    if (isLoadingMainFrame() && !m_didAccessInitialDocument) {
        m_didAccessInitialDocument = true;
        // Notify asynchronously, since this is called within a JavaScript security check.
        m_didAccessInitialDocumentTimer.startOneShot(0, FROM_HERE);
    }
}

void FrameLoader::didAccessInitialDocumentTimerFired(Timer<FrameLoader>*)
{
    if (client())
        client()->didAccessInitialDocument();
}

void FrameLoader::notifyIfInitialDocumentAccessed()
{
    if (m_didAccessInitialDocumentTimer.isActive()) {
        m_didAccessInitialDocumentTimer.stop();
        didAccessInitialDocumentTimerFired(0);
    }
}

bool FrameLoader::prepareForCommit()
{
    PluginScriptForbiddenScope forbidPluginDestructorScripting;
    RefPtrWillBeRawPtr<DocumentLoader> pdl = m_provisionalDocumentLoader;

    if (m_documentLoader) {
        client()->dispatchWillClose();
        dispatchUnloadEvent();
    }
    m_frame->detachChildren();
    // The previous calls to dispatchUnloadEvent() and detachChildren() can
    // execute arbitrary script via things like unload events. If the executed
    // script intiates a new load or causes the current frame to be detached,
    // we need to abandon the current load.
    if (pdl != m_provisionalDocumentLoader)
        return false;
    if (m_documentLoader) {
        FrameNavigationDisabler navigationDisabler(m_frame);
        detachDocumentLoader(m_documentLoader);
    }
    // detachFromFrame() will abort XHRs that haven't completed, which can
    // trigger event listeners for 'abort'. These event listeners might detach
    // the frame.
    // TODO(dcheng): Investigate if this can be moved above the check that
    // m_provisionalDocumentLoader hasn't changed.
    if (!m_frame->client())
        return false;
    // No more events will be dispatched so detach the Document.
    if (m_frame->document())
        m_frame->document()->detach();
    m_documentLoader = m_provisionalDocumentLoader.release();

    return true;
}

void FrameLoader::commitProvisionalLoad()
{
    ASSERT(client()->hasWebView());
    RefPtrWillBeRawPtr<LocalFrame> protect(m_frame.get());

    // Check if the destination page is allowed to access the previous page's timing information.
    if (m_frame->document()) {
        RefPtr<SecurityOrigin> securityOrigin = SecurityOrigin::create(m_provisionalDocumentLoader->request().url());
        m_provisionalDocumentLoader->timing().setHasSameOriginAsPreviousDocument(securityOrigin->canRequest(m_frame->document()->url()));
    }

    if (!prepareForCommit())
        return;

    if (isLoadingMainFrame())
        m_frame->page()->chromeClient().needTouchEvents(false);

    client()->transitionToCommittedForNewPage();
    m_frame->navigationScheduler().cancel();
    m_frame->editor().clearLastEditCommand();

    // If we are still in the process of initializing an empty document then
    // its frame is not in a consistent state for rendering, so avoid setJSStatusBarText
    // since it may cause clients to attempt to render the frame.
    if (!m_stateMachine.creatingInitialEmptyDocument()) {
        DOMWindow* window = m_frame->domWindow();
        window->setStatus(String());
        window->setDefaultStatus(String());
    }
}

bool FrameLoader::isLoadingMainFrame() const
{
    return m_frame->isMainFrame();
}

FrameLoadType FrameLoader::loadType() const
{
    return m_loadType;
}

void FrameLoader::restoreScrollPositionAndViewState()
{
    FrameView* view = m_frame->view();
    if (!m_frame->page() || !view || !m_currentItem || !m_stateMachine.committedFirstRealDocumentLoad())
        return;

    if (!needsHistoryItemRestore(m_loadType))
        return;

    if (m_currentItem->scrollRestorationType() == ScrollRestorationManual)
        return;

    // This tries to balance 1. restoring as soon as possible, 2. detecting
    // clamping to avoid repeatedly popping the scroll position down as the
    // page height increases, 3. ignore clamp detection after load completes
    // because that may be because the page will never reach its previous
    // height.
    bool canRestoreWithoutClamping = view->clampOffsetAtScale(m_currentItem->scrollPoint(), 1) == m_currentItem->scrollPoint();
    bool canRestoreWithoutAnnoyingUser = !view->wasScrolledByUser() && (canRestoreWithoutClamping || m_frame->isLoading());
    if (!canRestoreWithoutAnnoyingUser)
        return;

    if (m_frame->isMainFrame() && m_currentItem->pageScaleFactor()) {
        FloatPoint pinchViewportOffset(m_currentItem->pinchViewportScrollPoint());
        IntPoint frameScrollOffset(m_currentItem->scrollPoint());

        m_frame->page()->setPageScaleFactor(m_currentItem->pageScaleFactor(), frameScrollOffset);

        // If the pinch viewport's offset is (-1, -1) it means the history item
        // is an old version of HistoryItem so distribute the scroll between
        // the main frame and the pinch viewport as best as we can.
        if (pinchViewportOffset.x() == -1 && pinchViewportOffset.y() == -1)
            pinchViewportOffset = FloatPoint(frameScrollOffset - view->scrollPosition());

        m_frame->host()->pinchViewport().setLocation(pinchViewportOffset);
    } else {
        IntPoint adjustedScrollPosition = view->clampScrollPosition(m_currentItem->scrollPoint());
        if (adjustedScrollPosition != view->scrollPosition())
            view->setScrollPosition(adjustedScrollPosition, ProgrammaticScroll);
    }

    if (m_frame->isMainFrame()) {
        if (ScrollingCoordinator* scrollingCoordinator = m_frame->page()->scrollingCoordinator())
            scrollingCoordinator->frameViewRootLayerDidChange(view);
    }
    documentLoader()->initialScrollState().didRestoreFromHistory = true;
}

String FrameLoader::userAgent(const KURL& url) const
{
    String userAgent = client()->userAgent(url);
    InspectorInstrumentation::applyUserAgentOverride(m_frame, &userAgent);
    return userAgent;
}

void FrameLoader::detach()
{
#if !ENABLE(OILPAN)
    // The caller must protect a reference to m_frame.
    ASSERT(m_frame->refCount() > 1);
#endif
    detachDocumentLoader(m_documentLoader);
    detachDocumentLoader(m_provisionalDocumentLoader);
    detachDocumentLoader(m_policyDocumentLoader);

    Frame* parent = m_frame->tree().parent();
    if (parent && parent->isLocalFrame())
        toLocalFrame(parent)->loader().scheduleCheckCompleted();
    if (m_progressTracker) {
        m_progressTracker->dispose();
        m_progressTracker.clear();
    }
}

void FrameLoader::receivedMainResourceError(DocumentLoader* loader, const ResourceError& error)
{
    // Retain because the stop may release the last reference to it.
    RefPtrWillBeRawPtr<LocalFrame> protect(m_frame.get());
    RefPtrWillBeRawPtr<DocumentLoader> protectDocumentLoader(loader);

    // FIXME: We really ought to be able to just check for isCancellation() here, but there are some
    // ResourceErrors that setIsCancellation() but aren't created by ResourceError::cancelledError().
    ResourceError c(ResourceError::cancelledError(KURL()));
    if ((error.errorCode() != c.errorCode() || error.domain() != c.domain()) && m_frame->owner()) {
        // FIXME: For now, fallback content doesn't work cross process.
        ASSERT(m_frame->owner()->isLocal());
        m_frame->deprecatedLocalOwner()->renderFallbackContent();
    }

    HistoryCommitType historyCommitType = loadTypeToCommitType(m_loadType, documentLoader()->isClientRedirect());
    if (loader == m_provisionalDocumentLoader) {
        client()->dispatchDidFailProvisionalLoad(error, historyCommitType);
        if (loader != m_provisionalDocumentLoader)
            return;
        detachDocumentLoader(m_provisionalDocumentLoader);
        m_progressTracker->progressCompleted();
    } else {
        ASSERT(loader == m_documentLoader);
        if (m_frame->document()->parser())
            m_frame->document()->parser()->stopParsing();
        if (!m_provisionalDocumentLoader && m_frame->isLoading()) {
            client()->dispatchDidFailLoad(error, historyCommitType);
            m_progressTracker->progressCompleted();
        }
    }
    checkCompleted();
}

bool FrameLoader::shouldPerformFragmentNavigation(bool isFormSubmission, const String& httpMethod, FrameLoadType loadType, const KURL& url)
{
    // We don't do this if we are submitting a form with method other than "GET", explicitly reloading,
    // currently displaying a frameset, or if the URL does not have a fragment.
    return (!isFormSubmission || equalIgnoringCase(httpMethod, "GET"))
        && loadType != FrameLoadTypeReload
        && loadType != FrameLoadTypeReloadFromOrigin
        && loadType != FrameLoadTypeSame
        && loadType != FrameLoadTypeBackForward
        && url.hasFragmentIdentifier()
        && equalIgnoringFragmentIdentifier(m_frame->document()->url(), url)
        // We don't want to just scroll if a link from within a
        // frameset is trying to reload the frameset into _top.
        && !m_frame->document()->isFrameSet();
}

void FrameLoader::processFragment(const KURL& url, LoadStartType loadStartType)
{
    FrameView* view = m_frame->view();
    if (!view)
        return;

    // Leaking scroll position to a cross-origin ancestor would permit the so-called "framesniffing" attack.
    RefPtrWillBeRawPtr<Frame> boundaryFrame = url.hasFragmentIdentifier() ? m_frame->findUnsafeParentScrollPropagationBoundary() : 0;

    // FIXME: Handle RemoteFrames
    if (boundaryFrame && boundaryFrame->isLocalFrame())
        toLocalFrame(boundaryFrame.get())->view()->setSafeToPropagateScrollToParent(false);

    view->processUrlFragment(url,
        loadStartType == NavigationToDifferentDocument && documentLoader()->initialScrollState().didRestoreFromHistory ?
            FrameView::UrlFragmentDontScroll : FrameView::UrlFragmentScroll);

    if (boundaryFrame && boundaryFrame->isLocalFrame())
        toLocalFrame(boundaryFrame.get())->view()->setSafeToPropagateScrollToParent(true);
}

bool FrameLoader::shouldClose()
{
    Page* page = m_frame->page();
    if (!page || !page->chromeClient().canOpenBeforeUnloadConfirmPanel())
        return true;

    // Store all references to each subframe in advance since beforeunload's event handler may modify frame
    WillBeHeapVector<RefPtrWillBeMember<LocalFrame>> targetFrames;
    targetFrames.append(m_frame);
    for (Frame* child = m_frame->tree().firstChild(); child; child = child->tree().traverseNext(m_frame)) {
        // FIXME: There is not yet any way to dispatch events to out-of-process frames.
        if (child->isLocalFrame())
            targetFrames.append(toLocalFrame(child));
    }

    bool shouldClose = false;
    {
        NavigationDisablerForBeforeUnload navigationDisabler;
        size_t i;

        bool didAllowNavigation = false;
        for (i = 0; i < targetFrames.size(); i++) {
            if (!targetFrames[i]->tree().isDescendantOf(m_frame))
                continue;
            if (!targetFrames[i]->document()->dispatchBeforeUnloadEvent(page->chromeClient(), didAllowNavigation))
                break;
        }

        if (i == targetFrames.size())
            shouldClose = true;
    }
    return shouldClose;
}


void FrameLoader::startLoad(FrameLoadRequest& frameLoadRequest, FrameLoadType type, NavigationPolicy navigationPolicy)
{
    ASSERT(client()->hasWebView());
    if (m_frame->document()->pageDismissalEventBeingDispatched() != Document::NoDismissal)
        return;

    NavigationType navigationType = determineNavigationType(type, frameLoadRequest.resourceRequest().httpBody() || frameLoadRequest.form(), frameLoadRequest.triggeringEvent());
    frameLoadRequest.resourceRequest().setRequestContext(determineRequestContextFromNavigationType(navigationType));
    frameLoadRequest.resourceRequest().setFrameType(m_frame->isMainFrame() ? WebURLRequest::FrameTypeTopLevel : WebURLRequest::FrameTypeNested);
    ResourceRequest& request = frameLoadRequest.resourceRequest();

    m_policyDocumentLoader = client()->createDocumentLoader(m_frame, request, frameLoadRequest.substituteData().isValid() ? frameLoadRequest.substituteData() : defaultSubstituteDataForURL(request.url()));
    m_policyDocumentLoader->setNavigationType(navigationType);
    m_policyDocumentLoader->setReplacesCurrentHistoryItem(type == FrameLoadTypeRedirectWithLockedBackForwardList);
    m_policyDocumentLoader->setIsClientRedirect(frameLoadRequest.clientRedirect() == ClientRedirect);

    // stopAllLoaders can detach the LocalFrame, so protect it.
    RefPtrWillBeRawPtr<LocalFrame> protect(m_frame.get());
    if ((!m_policyDocumentLoader->shouldContinueForNavigationPolicy(request, frameLoadRequest.shouldCheckMainWorldContentSecurityPolicy(), navigationPolicy) || !shouldClose()) && m_policyDocumentLoader) {
        detachDocumentLoader(m_policyDocumentLoader);
        return;
    }

    // FIXME: This is an odd set of steps to shut down parsing and it's unclear why it works.
    // It's also unclear why other steps don't work.
    if (m_frame->document()->parsing()) {
        finishedParsing();
        m_frame->document()->setParsingState(Document::FinishedParsing);
    }
    m_frame->document()->setReadyState(Document::Complete);

    if (m_provisionalDocumentLoader) {
        m_provisionalDocumentLoader->stopLoading();
        detachDocumentLoader(m_provisionalDocumentLoader);
    }
    m_checkTimer.stop();

    // <rdar://problem/6250856> - In certain circumstances on pages with multiple frames, stopAllLoaders()
    // might detach the current FrameLoader, in which case we should bail on this newly defunct load.
    if (!m_frame->page() || !m_policyDocumentLoader)
        return;

    InspectorInstrumentation::didStartProvisionalLoad(m_frame);

    m_frame->navigationScheduler().cancel();

    m_provisionalDocumentLoader = m_policyDocumentLoader.release();
    m_loadType = type;

    if (frameLoadRequest.form())
        client()->dispatchWillSubmitForm(frameLoadRequest.form());

    m_progressTracker->progressStarted();
    if (m_provisionalDocumentLoader->isClientRedirect())
        m_provisionalDocumentLoader->appendRedirect(m_frame->document()->url());
    m_provisionalDocumentLoader->appendRedirect(m_provisionalDocumentLoader->request().url());
    double triggeringEventTime = frameLoadRequest.triggeringEvent() ? convertDOMTimeStampToSeconds(frameLoadRequest.triggeringEvent()->timeStamp()) : 0;
    client()->dispatchDidStartProvisionalLoad(triggeringEventTime);
    ASSERT(m_provisionalDocumentLoader);
    m_provisionalDocumentLoader->startLoadingMainResource();
}

void FrameLoader::applyUserAgent(ResourceRequest& request)
{
    String userAgent = this->userAgent(request.url());
    ASSERT(!userAgent.isNull());
    request.setHTTPUserAgent(AtomicString(userAgent));
}

bool FrameLoader::shouldInterruptLoadForXFrameOptions(const String& content, const KURL& url, unsigned long requestIdentifier)
{
    UseCounter::count(m_frame->domWindow()->document(), UseCounter::XFrameOptions);

    if (!RuntimeEnabledFeatures::cspCheckEnabled())
        return false;

    Frame* topFrame = m_frame->tree().top();
    if (m_frame == topFrame)
        return false;

    XFrameOptionsDisposition disposition = parseXFrameOptionsHeader(content);

    switch (disposition) {
    case XFrameOptionsSameOrigin: {
        UseCounter::count(m_frame->domWindow()->document(), UseCounter::XFrameOptionsSameOrigin);
        RefPtr<SecurityOrigin> origin = SecurityOrigin::create(url);
        // Out-of-process ancestors are always a different origin.
        if (!topFrame->isLocalFrame() || !origin->isSameSchemeHostPort(toLocalFrame(topFrame)->document()->securityOrigin()))
            return true;
        for (Frame* frame = m_frame->tree().parent(); frame; frame = frame->tree().parent()) {
            if (!frame->isLocalFrame() || !origin->isSameSchemeHostPort(toLocalFrame(frame)->document()->securityOrigin())) {
                UseCounter::count(m_frame->domWindow()->document(), UseCounter::XFrameOptionsSameOriginWithBadAncestorChain);
                break;
            }
        }
        return false;
    }
    case XFrameOptionsDeny:
        return true;
    case XFrameOptionsAllowAll:
        return false;
    case XFrameOptionsConflict: {
        RefPtrWillBeRawPtr<ConsoleMessage> consoleMessage = ConsoleMessage::create(JSMessageSource, ErrorMessageLevel, "Multiple 'X-Frame-Options' headers with conflicting values ('" + content + "') encountered when loading '" + url.elidedString() + "'. Falling back to 'DENY'.");
        consoleMessage->setRequestIdentifier(requestIdentifier);
        m_frame->document()->addConsoleMessage(consoleMessage.release());
        return true;
    }
    case XFrameOptionsInvalid: {
        RefPtrWillBeRawPtr<ConsoleMessage> consoleMessage = ConsoleMessage::create(JSMessageSource, ErrorMessageLevel, "Invalid 'X-Frame-Options' header encountered when loading '" + url.elidedString() + "': '" + content + "' is not a recognized directive. The header will be ignored.");
        consoleMessage->setRequestIdentifier(requestIdentifier);
        m_frame->document()->addConsoleMessage(consoleMessage.release());
        return false;
    }
    default:
        ASSERT_NOT_REACHED();
        return false;
    }
}

bool FrameLoader::shouldTreatURLAsSameAsCurrent(const KURL& url) const
{
    return m_currentItem && url == m_currentItem->url();
}

bool FrameLoader::shouldTreatURLAsSrcdocDocument(const KURL& url) const
{
    if (!equalIgnoringCase(url.string(), "about:srcdoc"))
        return false;
    HTMLFrameOwnerElement* ownerElement = m_frame->deprecatedLocalOwner();
    if (!isHTMLIFrameElement(ownerElement))
        return false;
    return ownerElement->fastHasAttribute(srcdocAttr);
}

void FrameLoader::dispatchDocumentElementAvailable()
{
    client()->documentElementAvailable();
}

void FrameLoader::dispatchDidClearDocumentOfWindowObject()
{
    if (!m_frame->script().canExecuteScripts(NotAboutToExecuteScript))
        return;

    InspectorInstrumentation::didClearDocumentOfWindowObject(m_frame);

    if (m_dispatchingDidClearWindowObjectInMainWorld)
        return;
    TemporaryChange<bool>
        inDidClearWindowObject(m_dispatchingDidClearWindowObjectInMainWorld, true);
    // We just cleared the document, not the entire window object, but for the
    // embedder that's close enough.
    client()->dispatchDidClearWindowObjectInMainWorld();
}

void FrameLoader::dispatchDidClearWindowObjectInMainWorld()
{
    if (!m_frame->script().canExecuteScripts(NotAboutToExecuteScript))
        return;

    if (m_dispatchingDidClearWindowObjectInMainWorld)
        return;
    TemporaryChange<bool>
        inDidClearWindowObject(m_dispatchingDidClearWindowObjectInMainWorld, true);
    client()->dispatchDidClearWindowObjectInMainWorld();
}

SandboxFlags FrameLoader::effectiveSandboxFlags() const
{
    SandboxFlags flags = m_forcedSandboxFlags;
    if (FrameOwner* frameOwner = m_frame->owner())
        flags |= frameOwner->sandboxFlags();
    // Frames need to inherit the sandbox flags of their parent frame.
    if (Frame* parentFrame = m_frame->tree().parent())
        flags |= parentFrame->securityContext()->sandboxFlags();
    return flags;
}

bool FrameLoader::shouldEnforceStrictMixedContentChecking() const
{
    Frame* parentFrame = m_frame->tree().parent();
    if (!parentFrame)
        return false;

    // FIXME: We need a way to propagate strict mixed content checking flags to
    // out-of-process frames. For now, we'll always enforce.
    if (!parentFrame->isLocalFrame())
        return true;

    ASSERT(toLocalFrame(parentFrame)->document());
    return toLocalFrame(parentFrame)->document()->shouldEnforceStrictMixedContentChecking();
}

SecurityContext::InsecureRequestsPolicy FrameLoader::insecureRequestsPolicy() const
{
    Frame* parentFrame = m_frame->tree().parent();
    if (!parentFrame)
        return SecurityContext::InsecureRequestsDoNotUpgrade;

    // FIXME: We need a way to propagate insecure requests policy flags to
    // out-of-process frames. For now, we'll always use default behavior.
    if (!parentFrame->isLocalFrame())
        return SecurityContext::InsecureRequestsDoNotUpgrade;

    ASSERT(toLocalFrame(parentFrame)->document());
    return toLocalFrame(parentFrame)->document()->insecureRequestsPolicy();
}

SecurityContext::InsecureNavigationsSet* FrameLoader::insecureNavigationsToUpgrade() const
{
    ASSERT(m_frame);
    Frame* parentFrame = m_frame->tree().parent();
    if (!parentFrame)
        return nullptr;

    // FIXME: We need a way to propagate insecure requests policy flags to
    // out-of-process frames. For now, we'll always use default behavior.
    if (!parentFrame->isLocalFrame())
        return nullptr;

    ASSERT(toLocalFrame(parentFrame)->document());
    return toLocalFrame(parentFrame)->document()->insecureNavigationsToUpgrade();
}

} // namespace blink

/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013 Apple Inc. All Rights Reserved.
 * Copyright (C) 2008 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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
#include "core/page/Page.h"

#include "core/css/resolver/ViewportStyleResolver.h"
#include "core/dom/ClientRectList.h"
#include "core/dom/DocumentMarkerController.h"
#include "core/dom/VisitedLinkState.h"
#include "core/editing/Caret.h"
#include "core/editing/UndoStack.h"
#include "core/events/Event.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/frame/DOMTimer.h"
#include "core/frame/FrameConsole.h"
#include "core/frame/FrameHost.h"
#include "core/frame/RemoteFrame.h"
#include "core/frame/RemoteFrameView.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLMediaElement.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/layout/LayoutView.h"
#include "core/layout/TextAutosizer.h"
#include "core/page/AutoscrollController.h"
#include "core/page/ChromeClient.h"
#include "core/page/ContextMenuController.h"
#include "core/page/DragController.h"
#include "core/page/FocusController.h"
#include "core/page/PointerLockController.h"
#include "core/page/ValidationMessageClient.h"
#include "core/page/scrolling/ScrollingCoordinator.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/plugins/PluginData.h"
#include "wtf/RefCountedLeakCounter.h"

namespace blink {

DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, pageCounter, ("Page"));

// static
HashSet<Page*>& Page::allPages()
{
    DEFINE_STATIC_LOCAL(HashSet<Page*>, allPages, ());
    return allPages;
}

// static
HashSet<Page*>& Page::ordinaryPages()
{
    DEFINE_STATIC_LOCAL(HashSet<Page*>, ordinaryPages, ());
    return ordinaryPages;
}


void Page::networkStateChanged(bool online)
{
    WillBeHeapVector<RefPtrWillBeMember<LocalFrame>> frames;

    // Get all the frames of all the pages in all the page groups
    for (Page* page : allPages()) {
        for (Frame* frame = page->mainFrame(); frame; frame = frame->tree().traverseNext()) {
            // FIXME: There is currently no way to dispatch events to out-of-process frames.
            if (frame->isLocalFrame())
                frames.append(toLocalFrame(frame));
        }
    }

    AtomicString eventName = online ? EventTypeNames::online : EventTypeNames::offline;
    for (unsigned i = 0; i < frames.size(); i++) {
        frames[i]->domWindow()->dispatchEvent(Event::create(eventName));
        InspectorInstrumentation::networkStateChanged(frames[i].get(), online);
    }
}

float deviceScaleFactor(LocalFrame* frame)
{
    if (!frame)
        return 1;
    Page* page = frame->page();
    if (!page)
        return 1;
    return page->deviceScaleFactor();
}

Page::Page(PageClients& pageClients)
    : SettingsDelegate(Settings::create())
    , m_animator(PageAnimator::create(*this))
    , m_autoscrollController(AutoscrollController::create(*this))
    , m_chromeClient(pageClients.chromeClient)
    , m_dragCaretController(DragCaretController::create())
    , m_dragController(DragController::create(this, pageClients.dragClient))
    , m_focusController(FocusController::create(this))
    , m_contextMenuController(ContextMenuController::create(this, pageClients.contextMenuClient))
    , m_pointerLockController(PointerLockController::create(this))
    , m_undoStack(UndoStack::create())
    , m_mainFrame(nullptr)
    , m_editorClient(pageClients.editorClient)
    , m_spellCheckerClient(pageClients.spellCheckerClient)
    , m_openedByDOM(false)
    , m_tabKeyCyclesThroughElements(true)
    , m_defersLoading(false)
    , m_deviceScaleFactor(1)
    , m_timerAlignmentInterval(DOMTimer::visiblePageAlignmentInterval())
    , m_visibilityState(PageVisibilityStateVisible)
    , m_isCursorVisible(true)
#if ENABLE(ASSERT)
    , m_isPainting(false)
#endif
    , m_frameHost(FrameHost::create(*this))
{
    ASSERT(m_editorClient);

    ASSERT(!allPages().contains(this));
    allPages().add(this);

#ifndef NDEBUG
    pageCounter.increment();
#endif
}

Page::~Page()
{
    // willBeDestroyed() must be called before Page destruction.
    ASSERT(!m_mainFrame);
}

void Page::makeOrdinary()
{
    ASSERT(!ordinaryPages().contains(this));
    ordinaryPages().add(this);
}

ViewportDescription Page::viewportDescription() const
{
    return mainFrame() && mainFrame()->isLocalFrame() && deprecatedLocalMainFrame()->document() ? deprecatedLocalMainFrame()->document()->viewportDescription() : ViewportDescription();
}

ScrollingCoordinator* Page::scrollingCoordinator()
{
    if (!m_scrollingCoordinator && m_settings->acceleratedCompositingEnabled())
        m_scrollingCoordinator = ScrollingCoordinator::create(this);

    return m_scrollingCoordinator.get();
}

String Page::mainThreadScrollingReasonsAsText()
{
    if (ScrollingCoordinator* scrollingCoordinator = this->scrollingCoordinator())
        return scrollingCoordinator->mainThreadScrollingReasonsAsText();

    return String();
}

ClientRectList* Page::nonFastScrollableRects(const LocalFrame* frame)
{
    if (ScrollingCoordinator* scrollingCoordinator = this->scrollingCoordinator()) {
        // Hits in compositing/iframes/iframe-composited-scrolling.html
        DisableCompositingQueryAsserts disabler;
        scrollingCoordinator->updateAfterCompositingChangeIfNeeded();
    }

    // Now retain non-fast scrollable regions
    return ClientRectList::create(frame->view()->layerForScrolling()->platformLayer()->nonFastScrollableRegion());
}

void Page::setMainFrame(Frame* mainFrame)
{
    // Should only be called during initialization or swaps between local and
    // remote frames.
    // FIXME: Unfortunately we can't assert on this at the moment, because this
    // is called in the base constructor for both LocalFrame and RemoteFrame,
    // when the vtables for the derived classes have not yet been setup.
    m_mainFrame = mainFrame;
}

void Page::documentDetached(Document* document)
{
    m_multisamplingChangedObservers.clear();
    m_pointerLockController->documentDetached(document);
    m_contextMenuController->documentDetached(document);
    if (m_validationMessageClient)
        m_validationMessageClient->documentDetached(*document);
    m_originsUsingFeatures.documentDetached(*document);
}

bool Page::openedByDOM() const
{
    return m_openedByDOM;
}

void Page::setOpenedByDOM()
{
    m_openedByDOM = true;
}

void Page::platformColorsChanged()
{
    for (const Page* page : allPages())
        for (Frame* frame = page->mainFrame(); frame; frame = frame->tree().traverseNext()) {
            if (frame->isLocalFrame())
                toLocalFrame(frame)->document()->platformColorsChanged();
        }
}

void Page::setNeedsRecalcStyleInAllFrames()
{
    for (Frame* frame = mainFrame(); frame; frame = frame->tree().traverseNext()) {
        if (frame->isLocalFrame())
            toLocalFrame(frame)->document()->styleResolverChanged();
    }
}

void Page::setNeedsLayoutInAllFrames()
{
    for (Frame* frame = mainFrame(); frame; frame = frame->tree().traverseNext()) {
        if (!frame->isLocalFrame())
            continue;
        if (FrameView* view = toLocalFrame(frame)->view()) {
            view->setNeedsLayout();
            view->scheduleRelayout();
        }
    }
}

void Page::refreshPlugins()
{
    if (allPages().isEmpty())
        return;

    PluginData::refresh();

    for (const Page* page : allPages()) {
        // Clear out the page's plugin data.
        if (page->m_pluginData)
            page->m_pluginData = nullptr;
    }
}

PluginData* Page::pluginData() const
{
    if (!mainFrame()->isLocalFrame()
        || !deprecatedLocalMainFrame()->loader().allowPlugins(NotAboutToInstantiatePlugin))
        return nullptr;
    if (!m_pluginData)
        m_pluginData = PluginData::create(this);
    return m_pluginData.get();
}

static Frame* incrementFrame(Frame* curr, bool forward, bool wrapFlag)
{
    return forward
        ? curr->tree().traverseNextWithWrap(wrapFlag)
        : curr->tree().traversePreviousWithWrap(wrapFlag);
}

void Page::unmarkAllTextMatches()
{
    if (!mainFrame())
        return;

    Frame* frame = mainFrame();
    do {
        if (frame->isLocalFrame())
            toLocalFrame(frame)->document()->markers().removeMarkers(DocumentMarker::TextMatch);
        frame = incrementFrame(frame, true, false);
    } while (frame);
}

void Page::setValidationMessageClient(PassOwnPtrWillBeRawPtr<ValidationMessageClient> client)
{
    m_validationMessageClient = client;
}

void Page::setDefersLoading(bool defers)
{
    if (defers == m_defersLoading)
        return;

    m_defersLoading = defers;
    for (Frame* frame = mainFrame(); frame; frame = frame->tree().traverseNext()) {
        if (frame->isLocalFrame())
            toLocalFrame(frame)->loader().setDefersLoading(defers);
    }
}

void Page::setPageScaleFactor(float scale, const IntPoint& origin)
{
    if (!mainFrame()->isLocalFrame())
        return;

    FrameView* view = deprecatedLocalMainFrame()->view();
    PinchViewport& viewport = frameHost().pinchViewport();

    if (scale != viewport.scale()) {
        viewport.setScale(scale);

        chromeClient().pageScaleFactorChanged();

        deprecatedLocalMainFrame()->loader().saveScrollState();
    }

    if (view && view->scrollPosition() != origin)
        view->setScrollPosition(origin, ProgrammaticScroll);
}

float Page::pageScaleFactor() const
{
    return frameHost().pinchViewport().scale();
}

void Page::setDeviceScaleFactor(float scaleFactor)
{
    if (m_deviceScaleFactor == scaleFactor)
        return;

    m_deviceScaleFactor = scaleFactor;
    setNeedsRecalcStyleInAllFrames();

    if (mainFrame() && mainFrame()->isLocalFrame())
        deprecatedLocalMainFrame()->deviceScaleFactorChanged();
}

void Page::setDeviceColorProfile(const Vector<char>& profile)
{
    // FIXME: implement.
}

void Page::resetDeviceColorProfile()
{
    // FIXME: implement.
}

void Page::allVisitedStateChanged()
{
    for (const Page* page : ordinaryPages()) {
        for (Frame* frame = page->m_mainFrame; frame; frame = frame->tree().traverseNext()) {
            if (frame->isLocalFrame())
                toLocalFrame(frame)->document()->visitedLinkState().invalidateStyleForAllLinks();
        }
    }
}

void Page::visitedStateChanged(LinkHash linkHash)
{
    for (const Page* page : ordinaryPages()) {
        for (Frame* frame = page->m_mainFrame; frame; frame = frame->tree().traverseNext()) {
            if (frame->isLocalFrame())
                toLocalFrame(frame)->document()->visitedLinkState().invalidateStyleForLink(linkHash);
        }
    }
}

void Page::setTimerAlignmentInterval(double interval)
{
    if (interval == m_timerAlignmentInterval)
        return;

    m_timerAlignmentInterval = interval;
    for (Frame* frame = mainFrame(); frame; frame = frame->tree().traverseNextWithWrap(false)) {
        if (!frame->isLocalFrame())
            continue;

        if (Document* document = toLocalFrame(frame)->document()) {
            if (DOMTimerCoordinator* timers = document->timers()) {
                timers->didChangeTimerAlignmentInterval();
            }
        }
    }
}

double Page::timerAlignmentInterval() const
{
    return m_timerAlignmentInterval;
}

void Page::setVisibilityState(PageVisibilityState visibilityState, bool isInitialState)
{
    if (m_visibilityState == visibilityState)
        return;
    m_visibilityState = visibilityState;

    if (visibilityState == PageVisibilityStateVisible)
        setTimerAlignmentInterval(DOMTimer::visiblePageAlignmentInterval());
    else
        setTimerAlignmentInterval(DOMTimer::hiddenPageAlignmentInterval());

    if (!isInitialState)
        notifyPageVisibilityChanged();

    if (!isInitialState && m_mainFrame && m_mainFrame->isLocalFrame())
        deprecatedLocalMainFrame()->didChangeVisibilityState();
}

PageVisibilityState Page::visibilityState() const
{
    return m_visibilityState;
}

bool Page::isCursorVisible() const
{
    return m_isCursorVisible && settings().deviceSupportsMouse();
}

void Page::addMultisamplingChangedObserver(MultisamplingChangedObserver* observer)
{
    m_multisamplingChangedObservers.add(observer);
}

void Page::removeMultisamplingChangedObserver(MultisamplingChangedObserver* observer)
{
    m_multisamplingChangedObservers.remove(observer);
}

void Page::settingsChanged(SettingsDelegate::ChangeType changeType)
{
    switch (changeType) {
    case SettingsDelegate::StyleChange:
        setNeedsRecalcStyleInAllFrames();
        break;
    case SettingsDelegate::ViewportDescriptionChange:
        if (mainFrame() && mainFrame()->isLocalFrame())
            deprecatedLocalMainFrame()->document()->updateViewportDescription();
        break;
    case SettingsDelegate::DNSPrefetchingChange:
        for (Frame* frame = mainFrame(); frame; frame = frame->tree().traverseNext()) {
            if (frame->isLocalFrame())
                toLocalFrame(frame)->document()->initDNSPrefetch();
        }
        break;
    case SettingsDelegate::MultisamplingChange: {
        for (MultisamplingChangedObserver* observer : m_multisamplingChangedObservers)
            observer->multisamplingChanged(m_settings->openGLMultisamplingEnabled());
        break;
    }
    case SettingsDelegate::ImageLoadingChange:
        for (Frame* frame = mainFrame(); frame; frame = frame->tree().traverseNext()) {
            if (frame->isLocalFrame()) {
                toLocalFrame(frame)->document()->fetcher()->setImagesEnabled(settings().imagesEnabled());
                toLocalFrame(frame)->document()->fetcher()->setAutoLoadImages(settings().loadsImagesAutomatically());
            }
        }
        break;
    case SettingsDelegate::TextAutosizingChange:
        if (!mainFrame() || !mainFrame()->isLocalFrame())
            break;
        if (TextAutosizer* textAutosizer = deprecatedLocalMainFrame()->document()->textAutosizer())
            textAutosizer->updatePageInfoInAllFrames();
        break;
    case SettingsDelegate::FontFamilyChange:
        for (Frame* frame = mainFrame(); frame; frame = frame->tree().traverseNext()) {
            if (frame->isLocalFrame())
                toLocalFrame(frame)->document()->styleEngine().updateGenericFontFamilySettings();
        }
        setNeedsRecalcStyleInAllFrames();
        break;
    case SettingsDelegate::AcceleratedCompositingChange:
        updateAcceleratedCompositingSettings();
        break;
    case SettingsDelegate::MediaQueryChange:
        for (Frame* frame = mainFrame(); frame; frame = frame->tree().traverseNext()) {
            if (frame->isLocalFrame())
                toLocalFrame(frame)->document()->mediaQueryAffectingValueChanged();
        }
        setNeedsRecalcStyleInAllFrames();
        break;
    case SettingsDelegate::AccessibilityStateChange:
        if (!mainFrame() || !mainFrame()->isLocalFrame())
            break;
        deprecatedLocalMainFrame()->document()->axObjectCacheOwner().clearAXObjectCache();
        break;
    case SettingsDelegate::ViewportRuleChange:
        {
            if (!mainFrame() || !mainFrame()->isLocalFrame())
                break;
            Document* doc = toLocalFrame(mainFrame())->document();
            if (!doc || !doc->styleResolver())
                break;
            doc->styleResolver()->viewportStyleResolver()->collectViewportRules();
        }
        break;
    case SettingsDelegate::TextTrackKindUserPreferenceChange:
        for (Frame* frame = mainFrame(); frame; frame = frame->tree().traverseNext()) {
            if (frame->isLocalFrame()) {
                Document* doc = toLocalFrame(frame)->document();
                if (doc)
                    HTMLMediaElement::setTextTrackKindUserPreferenceForAllMediaElements(doc);
            }
        }
        break;
    }
}

void Page::updateAcceleratedCompositingSettings()
{
    for (Frame* frame = mainFrame(); frame; frame = frame->tree().traverseNext()) {
        if (!frame->isLocalFrame())
            continue;
        if (FrameView* view = toLocalFrame(frame)->view())
            view->updateAcceleratedCompositingSettings();
    }
}

void Page::didCommitLoad(LocalFrame* frame)
{
    notifyDidCommitLoad(frame);
    if (m_mainFrame == frame) {
        frame->console().clearMessages();
        useCounter().didCommitLoad();
        m_originsUsingFeatures.updateMeasurementsAndClear();
        UserGestureIndicator::clearProcessedUserGestureSinceLoad();
    }
}

void Page::acceptLanguagesChanged()
{
    WillBeHeapVector<RefPtrWillBeMember<LocalFrame>> frames;

    // Even though we don't fire an event from here, the LocalDOMWindow's will fire
    // an event so we keep the frames alive until we are done.
    for (Frame* frame = mainFrame(); frame; frame = frame->tree().traverseNext()) {
        if (frame->isLocalFrame())
            frames.append(toLocalFrame(frame));
    }

    for (unsigned i = 0; i < frames.size(); ++i)
        frames[i]->localDOMWindow()->acceptLanguagesChanged();
}

DEFINE_TRACE(Page)
{
#if ENABLE(OILPAN)
    visitor->trace(m_animator);
    visitor->trace(m_dragCaretController);
    visitor->trace(m_dragController);
    visitor->trace(m_focusController);
    visitor->trace(m_contextMenuController);
    visitor->trace(m_pointerLockController);
    visitor->trace(m_scrollingCoordinator);
    visitor->trace(m_undoStack);
    visitor->trace(m_mainFrame);
    visitor->trace(m_validationMessageClient);
    visitor->trace(m_multisamplingChangedObservers);
    visitor->trace(m_frameHost);
    HeapSupplementable<Page>::trace(visitor);
#endif
    PageLifecycleNotifier::trace(visitor);
}

void Page::willBeDestroyed()
{
    RefPtrWillBeRawPtr<Frame> mainFrame = m_mainFrame;

    mainFrame->detach(FrameDetachType::Remove);

    if (mainFrame->isLocalFrame()) {
        toLocalFrame(mainFrame.get())->setView(nullptr);
    } else {
        ASSERT(m_mainFrame->isRemoteFrame());
        toRemoteFrame(mainFrame.get())->setView(nullptr);
    }

    allPages().remove(this);
    if (ordinaryPages().contains(this))
        ordinaryPages().remove(this);

    if (m_scrollingCoordinator)
        m_scrollingCoordinator->willBeDestroyed();

#ifndef NDEBUG
    pageCounter.decrement();
#endif

    chromeClient().chromeDestroyed();
    if (m_validationMessageClient)
        m_validationMessageClient->willBeDestroyed();
    m_mainFrame = nullptr;

    Page::notifyContextDestroyed();
}

Page::PageClients::PageClients()
    : chromeClient(nullptr)
    , contextMenuClient(nullptr)
    , editorClient(nullptr)
    , dragClient(nullptr)
    , spellCheckerClient(nullptr)
{
}

Page::PageClients::~PageClients()
{
}

template class CORE_TEMPLATE_EXPORT WillBeHeapSupplement<Page>;

} // namespace blink

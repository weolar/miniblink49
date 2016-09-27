// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/web/WebFrame.h"

#include "bindings/core/v8/WindowProxyManager.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/RemoteFrame.h"
#include "core/html/HTMLFrameElementBase.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/page/Page.h"
#include "platform/UserGestureIndicator.h"
#include "platform/heap/Handle.h"
#include "public/web/WebElement.h"
#include "public/web/WebSandboxFlags.h"
#include "web/OpenedFrameTracker.h"
#include "web/RemoteBridgeFrameOwner.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebRemoteFrameImpl.h"
#include <algorithm>

namespace blink {

Frame* toCoreFrame(const WebFrame* frame)
{
    if (!frame)
        return 0;

    return frame->isWebLocalFrame()
        ? static_cast<Frame*>(toWebLocalFrameImpl(frame)->frame())
        : toWebRemoteFrameImpl(frame)->frame();
}

bool WebFrame::swap(WebFrame* frame)
{
    using std::swap;
    RefPtrWillBeRawPtr<Frame> oldFrame = toCoreFrame(this);
#if !ENABLE(OILPAN)
    RefPtrWillBeRawPtr<WebLocalFrameImpl> protectWebLocalFrame = isWebLocalFrame() ? toWebLocalFrameImpl(this) : nullptr;
    RefPtrWillBeRawPtr<WebRemoteFrameImpl> protectWebRemoteFrame = isWebRemoteFrame() ? toWebRemoteFrameImpl(this) : nullptr;
#endif

    // Unload the current Document in this frame: this calls unload handlers,
    // detaches child frames, etc. Since this runs script, make sure this frame
    // wasn't detached before continuing with the swap.
    // FIXME: There is no unit test for this condition, so one needs to be
    // written.
    if (!oldFrame->prepareForCommit())
        return false;

    if (m_parent) {
        if (m_parent->m_firstChild == this)
            m_parent->m_firstChild = frame;
        if (m_parent->m_lastChild == this)
            m_parent->m_lastChild = frame;
        // FIXME: This is due to the fact that the |frame| may be a provisional
        // local frame, because we don't know if the navigation will result in
        // an actual page or something else, like a download. The PlzNavigate
        // project will remove the need for provisional local frames.
        frame->m_parent = m_parent;
    }

    if (m_previousSibling) {
        m_previousSibling->m_nextSibling = frame;
        swap(m_previousSibling, frame->m_previousSibling);
    }
    if (m_nextSibling) {
        m_nextSibling->m_previousSibling = frame;
        swap(m_nextSibling, frame->m_nextSibling);
    }

    if (m_opener) {
        m_opener->m_openedFrameTracker->remove(this);
        m_opener->m_openedFrameTracker->add(frame);
        swap(m_opener, frame->m_opener);
    }
    if (!m_openedFrameTracker->isEmpty()) {
        m_openedFrameTracker->updateOpener(frame);
        frame->m_openedFrameTracker.reset(m_openedFrameTracker.release());
    }

    // Finally, clone the state of the current Frame into one matching
    // the type of the passed in WebFrame.
    // FIXME: This is a bit clunky; this results in pointless decrements and
    // increments of connected subframes.
    FrameOwner* owner = oldFrame->owner();
    oldFrame->disconnectOwnerElement();
    if (frame->isWebLocalFrame()) {
        LocalFrame& localFrame = *toWebLocalFrameImpl(frame)->frame();
        ASSERT(owner == localFrame.owner());
        if (owner) {
            if (owner->isLocal()) {
                HTMLFrameOwnerElement* ownerElement = toHTMLFrameOwnerElement(owner);
                ownerElement->setContentFrame(localFrame);
                ownerElement->setWidget(localFrame.view());
            } else {
                toRemoteBridgeFrameOwner(owner)->setContentFrame(toWebLocalFrameImpl(frame));
            }
        } else {
            localFrame.page()->setMainFrame(&localFrame);
        }
    } else {
        toWebRemoteFrameImpl(frame)->initializeCoreFrame(oldFrame->host(), owner, oldFrame->tree().name());
    }
    toCoreFrame(frame)->finishSwapFrom(oldFrame.get());

    // Although the Document in this frame is now unloaded, many resources
    // associated with the frame itself have not yet been freed yet.
    oldFrame->detach(FrameDetachType::Swap);
    m_parent = nullptr;

    return true;
}

void WebFrame::detach()
{
    toCoreFrame(this)->detach(FrameDetachType::Remove);
}

WebSecurityOrigin WebFrame::securityOrigin() const
{
    return WebSecurityOrigin(toCoreFrame(this)->securityContext()->securityOrigin());
}


void WebFrame::setFrameOwnerSandboxFlags(WebSandboxFlags flags)
{
    // At the moment, this is only used to replicate sandbox flags
    // for frames with a remote owner.
    FrameOwner* owner = toCoreFrame(this)->owner();
    ASSERT(owner);
    toRemoteBridgeFrameOwner(owner)->setSandboxFlags(static_cast<SandboxFlags>(flags));
}

WebFrame* WebFrame::opener() const
{
    return m_opener;
}

void WebFrame::setOpener(WebFrame* opener)
{
    if (m_opener)
        m_opener->m_openedFrameTracker->remove(this);
    if (opener)
        opener->m_openedFrameTracker->add(this);
    m_opener = opener;
}

void WebFrame::insertAfter(WebFrame* newChild, WebFrame* previousSibling)
{
    newChild->m_parent = this;

    WebFrame* next;
    if (!previousSibling) {
        // Insert at the beginning if no previous sibling is specified.
        next = m_firstChild;
        m_firstChild = newChild;
    } else {
        ASSERT(previousSibling->m_parent == this);
        next = previousSibling->m_nextSibling;
        previousSibling->m_nextSibling = newChild;
        newChild->m_previousSibling = previousSibling;
    }

    if (next) {
        newChild->m_nextSibling = next;
        next->m_previousSibling = newChild;
    } else {
        m_lastChild = newChild;
    }

    toCoreFrame(this)->tree().invalidateScopedChildCount();
    toCoreFrame(this)->host()->incrementSubframeCount();
}

void WebFrame::appendChild(WebFrame* child)
{
    // TODO(dcheng): Original code asserts that the frames have the same Page.
    // We should add an equivalent check... figure out what.
    insertAfter(child, m_lastChild);
}

void WebFrame::removeChild(WebFrame* child)
{
    child->m_parent = 0;

    if (m_firstChild == child)
        m_firstChild = child->m_nextSibling;
    else
        child->m_previousSibling->m_nextSibling = child->m_nextSibling;

    if (m_lastChild == child)
        m_lastChild = child->m_previousSibling;
    else
        child->m_nextSibling->m_previousSibling = child->m_previousSibling;

    child->m_previousSibling = child->m_nextSibling = 0;

    toCoreFrame(this)->tree().invalidateScopedChildCount();
    toCoreFrame(this)->host()->decrementSubframeCount();
}

void WebFrame::setParent(WebFrame* parent)
{
    m_parent = parent;
}

WebFrame* WebFrame::parent() const
{
    return m_parent;
}

WebFrame* WebFrame::top() const
{
    WebFrame* frame = const_cast<WebFrame*>(this);
    for (WebFrame* parent = frame; parent; parent = parent->m_parent)
        frame = parent;
    return frame;
}

WebFrame* WebFrame::firstChild() const
{
    return m_firstChild;
}

WebFrame* WebFrame::lastChild() const
{
    return m_lastChild;
}

WebFrame* WebFrame::previousSibling() const
{
    return m_previousSibling;
}

WebFrame* WebFrame::nextSibling() const
{
    return m_nextSibling;
}

WebFrame* WebFrame::traversePrevious(bool wrap) const
{
    if (Frame* frame = toCoreFrame(this))
        return fromFrame(frame->tree().traversePreviousWithWrap(wrap));
    return 0;
}

WebFrame* WebFrame::traverseNext(bool wrap) const
{
    if (Frame* frame = toCoreFrame(this))
        return fromFrame(frame->tree().traverseNextWithWrap(wrap));
    return 0;
}

WebFrame* WebFrame::findChildByName(const WebString& name) const
{
    Frame* frame = toCoreFrame(this);
    if (!frame)
        return 0;
    // FIXME: It's not clear this should ever be called to find a remote frame.
    // Perhaps just disallow that completely?
    return fromFrame(frame->tree().child(name));
}

WebFrame* WebFrame::fromFrameOwnerElement(const WebElement& webElement)
{
    Element* element = PassRefPtrWillBeRawPtr<Element>(webElement).get();

    if (!isHTMLFrameElementBase(element))
        return nullptr;
    return fromFrame(toHTMLFrameElementBase(element)->contentFrame());
}

WebFrame* WebFrame::fromFrame(Frame* frame)
{
    if (!frame)
        return 0;

    if (frame->isLocalFrame())
        return WebLocalFrameImpl::fromFrame(toLocalFrame(*frame));
    return WebRemoteFrameImpl::fromFrame(toRemoteFrame(*frame));
}

WebFrame::WebFrame(WebTreeScopeType scope)
    : m_scope(scope)
    , m_parent(0)
    , m_previousSibling(0)
    , m_nextSibling(0)
    , m_firstChild(0)
    , m_lastChild(0)
    , m_opener(0)
    , m_openedFrameTracker(new OpenedFrameTracker)
{
}

WebFrame::~WebFrame()
{
    m_openedFrameTracker.reset(0);
}

#if ENABLE(OILPAN)
ALWAYS_INLINE bool WebFrame::isFrameAlive(const WebFrame* frame)
{
    if (!frame)
        return true;

    if (frame->isWebLocalFrame())
        return Heap::isHeapObjectAlive(toWebLocalFrameImpl(frame));

    return Heap::isHeapObjectAlive(toWebRemoteFrameImpl(frame));
}

template <typename VisitorDispatcher>
ALWAYS_INLINE void WebFrame::traceFrameImpl(VisitorDispatcher visitor, WebFrame* frame)
{
    if (!frame)
        return;

    if (frame->isWebLocalFrame())
        visitor->trace(toWebLocalFrameImpl(frame));
    else
        visitor->trace(toWebRemoteFrameImpl(frame));
}

template <typename VisitorDispatcher>
ALWAYS_INLINE void WebFrame::traceFramesImpl(VisitorDispatcher visitor, WebFrame* frame)
{
    ASSERT(frame);
    traceFrame(visitor, frame->m_parent);
    for (WebFrame* child = frame->firstChild(); child; child = child->nextSibling())
        traceFrame(visitor, child);
    // m_opener is a weak reference.
    frame->m_openedFrameTracker->traceFrames(visitor);
}

template <typename VisitorDispatcher>
ALWAYS_INLINE void WebFrame::clearWeakFramesImpl(VisitorDispatcher visitor)
{
    if (!isFrameAlive(m_opener))
        m_opener = nullptr;
}

#define DEFINE_VISITOR_METHOD(VisitorDispatcher)                                                                               \
    void WebFrame::traceFrame(VisitorDispatcher visitor, WebFrame* frame) { traceFrameImpl(visitor, frame); }                  \
    void WebFrame::traceFrames(VisitorDispatcher visitor, WebFrame* frame) { traceFramesImpl(visitor, frame); }                \
    void WebFrame::clearWeakFrames(VisitorDispatcher visitor) { clearWeakFramesImpl(visitor); }

DEFINE_VISITOR_METHOD(Visitor*)
DEFINE_VISITOR_METHOD(InlinedGlobalMarkingVisitor)

#undef DEFINE_VISITOR_METHOD
#endif

} // namespace blink

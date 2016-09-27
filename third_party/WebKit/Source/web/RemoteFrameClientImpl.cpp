// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "web/RemoteFrameClientImpl.h"

#include "core/events/KeyboardEvent.h"
#include "core/events/MouseEvent.h"
#include "core/events/WheelEvent.h"
#include "core/frame/RemoteFrame.h"
#include "core/frame/RemoteFrameView.h"
#include "core/layout/LayoutPart.h"
#include "platform/exported/WrappedResourceRequest.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "platform/weborigin/SecurityPolicy.h"
#include "public/web/WebRemoteFrameClient.h"
#include "web/WebInputEventConversion.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebRemoteFrameImpl.h"

namespace blink {

RemoteFrameClientImpl::RemoteFrameClientImpl(WebRemoteFrameImpl* webFrame)
    : m_webFrame(webFrame)
{
}

bool RemoteFrameClientImpl::inShadowTree() const
{
    return m_webFrame->inShadowTree();
}

void RemoteFrameClientImpl::willBeDetached()
{
}

void RemoteFrameClientImpl::detached(FrameDetachType type)
{
    // Alert the client that the frame is being detached.
    RefPtrWillBeRawPtr<WebRemoteFrameImpl> protector(m_webFrame);

    WebRemoteFrameClient* client = m_webFrame->client();
    if (!client)
        return;

    client->frameDetached(static_cast<WebRemoteFrameClient::DetachType>(type));
    // Clear our reference to RemoteFrame at the very end, in case the client
    // refers to it.
    m_webFrame->setCoreFrame(nullptr);
}

Frame* RemoteFrameClientImpl::opener() const
{
    return toCoreFrame(m_webFrame->opener());
}

void RemoteFrameClientImpl::setOpener(Frame* opener)
{
    m_webFrame->setOpener(WebFrame::fromFrame(opener));
}

Frame* RemoteFrameClientImpl::parent() const
{
    return toCoreFrame(m_webFrame->parent());
}

Frame* RemoteFrameClientImpl::top() const
{
    return toCoreFrame(m_webFrame->top());
}

Frame* RemoteFrameClientImpl::previousSibling() const
{
    return toCoreFrame(m_webFrame->previousSibling());
}

Frame* RemoteFrameClientImpl::nextSibling() const
{
    return toCoreFrame(m_webFrame->nextSibling());
}

Frame* RemoteFrameClientImpl::firstChild() const
{
    return toCoreFrame(m_webFrame->firstChild());
}

Frame* RemoteFrameClientImpl::lastChild() const
{
    return toCoreFrame(m_webFrame->lastChild());
}

bool RemoteFrameClientImpl::willCheckAndDispatchMessageEvent(
    SecurityOrigin* target, MessageEvent* event, LocalFrame* sourceFrame) const
{
    if (m_webFrame->client())
        m_webFrame->client()->postMessageEvent(WebLocalFrameImpl::fromFrame(sourceFrame), m_webFrame, WebSecurityOrigin(target), WebDOMMessageEvent(event));
    return true;
}

void RemoteFrameClientImpl::navigate(const ResourceRequest& request, bool shouldReplaceCurrentEntry)
{
    if (m_webFrame->client())
        m_webFrame->client()->navigate(WrappedResourceRequest(request), shouldReplaceCurrentEntry);
}

void RemoteFrameClientImpl::reload(FrameLoadType loadType, ClientRedirectPolicy clientRedirectPolicy)
{
    if (m_webFrame->client())
        m_webFrame->client()->reload(loadType == FrameLoadTypeReloadFromOrigin, clientRedirectPolicy == ClientRedirect);
}

unsigned RemoteFrameClientImpl::backForwardLength()
{
    // TODO(creis,japhet): This method should return the real value for the
    // session history length. For now, return static value for the initial
    // navigation and the subsequent one moving the frame out-of-process.
    // See https://crbug.com/501116.
    return 2;
}

// FIXME: Remove this code once we have input routing in the browser
// process. See http://crbug.com/339659.
void RemoteFrameClientImpl::forwardInputEvent(Event* event)
{
    // This is only called when we have out-of-process iframes, which
    // need to forward input events across processes.
    // FIXME: Add a check for out-of-process iframes enabled.
    OwnPtr<WebInputEvent> webEvent;
    if (event->isKeyboardEvent())
        webEvent = adoptPtr(new WebKeyboardEventBuilder(*static_cast<KeyboardEvent*>(event)));
    else if (event->isMouseEvent())
        webEvent = adoptPtr(new WebMouseEventBuilder(m_webFrame->frame()->view(), toCoreFrame(m_webFrame)->ownerLayoutObject(), *static_cast<MouseEvent*>(event)));
    else if (event->isWheelEvent())
        webEvent = adoptPtr(new WebMouseWheelEventBuilder(m_webFrame->frame()->view(), toCoreFrame(m_webFrame)->ownerLayoutObject(), *static_cast<WheelEvent*>(event)));

    // Other or internal Blink events should not be forwarded.
    if (!webEvent || webEvent->type == WebInputEvent::Undefined)
        return;

    m_webFrame->client()->forwardInputEvent(webEvent.get());
}

} // namespace blink

// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RemoteFrameClientImpl_h
#define RemoteFrameClientImpl_h

#include "core/frame/RemoteFrameClient.h"

namespace blink {
class WebRemoteFrameImpl;

class RemoteFrameClientImpl final : public RemoteFrameClient {
public:
    explicit RemoteFrameClientImpl(WebRemoteFrameImpl*);

    // FrameClient overrides:
    bool inShadowTree() const override;
    void willBeDetached() override;
    void detached(FrameDetachType) override;
    Frame* opener() const override;
    void setOpener(Frame*) override;
    Frame* parent() const override;
    Frame* top() const override;
    Frame* previousSibling() const override;
    Frame* nextSibling() const override;
    Frame* firstChild() const override;
    Frame* lastChild() const override;
    bool willCheckAndDispatchMessageEvent(SecurityOrigin*, MessageEvent*, LocalFrame*) const override;

    // RemoteFrameClient overrides:
    void navigate(const ResourceRequest&, bool shouldReplaceCurrentEntry) override;
    void reload(FrameLoadType, ClientRedirectPolicy) override;
    unsigned backForwardLength() override;
    void forwardInputEvent(Event*) override;

    WebRemoteFrameImpl* webFrame() const { return m_webFrame; }

private:
    WebRemoteFrameImpl* m_webFrame;
};

} // namespace blink

#endif // RemoteFrameClientImpl_h

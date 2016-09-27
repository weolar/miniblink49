// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebSocketChannelClientProxy_h
#define WebSocketChannelClientProxy_h

#include "modules/websockets/WebSocketChannelClient.h"
#include "platform/heap/Handle.h"
#include "web/WebSocketImpl.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"
#include <stdint.h>

namespace blink {

// Ideally we want to simply make WebSocketImpl inherit from
// WebSocketChannelClient, but we cannot do that because WebSocketChannelClient
// needs to be on Oilpan's heap whereas WebSocketImpl cannot be on Oilpan's
// heap. Thus we need to introduce a proxy class to decouple WebSocketImpl
// from WebSocketChannelClient.
class WebSocketChannelClientProxy final : public GarbageCollectedFinalized<WebSocketChannelClientProxy>, public WebSocketChannelClient {
    USING_GARBAGE_COLLECTED_MIXIN(WebSocketChannelClientProxy)
public:
    static WebSocketChannelClientProxy* create(WebSocketImpl* impl)
    {
        return new WebSocketChannelClientProxy(impl);
    }

    void didConnect(const String& subprotocol, const String& extensions) override
    {
        m_impl->didConnect(subprotocol, extensions);
    }
    void didReceiveTextMessage(const String& payload) override
    {
        m_impl->didReceiveTextMessage(payload);
    }
    void didReceiveBinaryMessage(PassOwnPtr<Vector<char>> payload) override
    {
        m_impl->didReceiveBinaryMessage(payload);
    }
    void didError() override
    {
        m_impl->didError();
    }
    void didConsumeBufferedAmount(uint64_t consumed) override
    {
        m_impl->didConsumeBufferedAmount(consumed);
    }
    void didStartClosingHandshake() override
    {
        m_impl->didStartClosingHandshake();
    }
    void didClose(ClosingHandshakeCompletionStatus status, unsigned short code, const String& reason) override
    {
        WebSocketImpl* impl = m_impl;
        m_impl = nullptr;
        impl->didClose(status, code, reason);
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        WebSocketChannelClient::trace(visitor);
    }

private:
    explicit WebSocketChannelClientProxy(WebSocketImpl* impl)
        : m_impl(impl)
    {
    }

    WebSocketImpl* m_impl;
};

} // namespace blink

#endif // WebSocketChannelClientProxy_h

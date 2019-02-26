// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef content_web_impl_win_WebMessagePortChannelImpl_h
#define content_web_impl_win_WebMessagePortChannelImpl_h

#include "third_party/WebKit/public/platform/WebMessagePortChannel.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "content/browser/PlatformMessagePortChannel.h"

namespace content {

class PlatformMessagePortChannel;

// This is thread safe.
class WebMessagePortChannelImpl : public NoBaseWillBeGarbageCollectedFinalized<WebMessagePortChannelImpl>, public blink::WebMessagePortChannel {
public:
    WebMessagePortChannelImpl(PlatformMessagePortChannel::MessagePortQueue* incoming, PlatformMessagePortChannel::MessagePortQueue* outgoing);

    PlatformMessagePortChannel* getChannel() const
    {
        return m_channel;
    }
private:
    DECLARE_TRACE();

    ~WebMessagePortChannelImpl();

    // WebMessagePortChannel implementation.
    void setClient(blink::WebMessagePortChannelClient* client) override;
    void destroy() override;
    void postMessage(const blink::WebString& message, blink::WebMessagePortChannelArray* channels_ptr) override;
    bool tryGetMessage(blink::WebString* message, blink::WebMessagePortChannelArray& channels) override;

    blink::Member<PlatformMessagePortChannel> m_channel;
    blink::Persistent<WebMessagePortChannelImpl> m_keepAlive;

    WTF::Mutex m_mutex;
};

}  // namespace content

#endif  // content_web_impl_win_WebMessagePortChannelImpl_h
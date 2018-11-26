// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_DEVTOOLS_DEVTOOLS_AGENT_H_
#define CONTENT_RENDERER_DEVTOOLS_DEVTOOLS_AGENT_H_

#include "third_party/WebKit/public/web/WebDevToolsAgentClient.h"
#include <string>

namespace blink {
class WebDevToolsFrontend;
class WebString;
class WebLocalFrame;
class WebDevToolsAgent;
class LocalFrame;
}

namespace base {
class Value;
class DictionaryValue;
}

namespace content {

class DevToolsClient;
class WebPage;

// 被调试网页使用
// Developer tools UI end of communication channel between the render process of
// the page being inspected and tools UI renderer process. All messages will
// go through browser process. On the side of the inspected page there's
// corresponding DevToolsAgent object.
// TODO(yurys): now the client is almost empty later it will delegate calls to
// code in glue
class DevToolsAgent : public blink::WebDevToolsAgentClient {
public:
    DevToolsAgent(WebPage* page, blink::WebLocalFrame* frame);
    ~DevToolsAgent();

    void onMessageReceivedFromFronEnd(const std::string* message);

    void setDevToolsClient(DevToolsClient* devToolsClient);
    int getId() const { return m_id; }
    void onAttach(const std::string& hostId);
    void onDetach();

    void inspectElementAt(int x, int y);

    blink::WebDevToolsAgent* getWebAgent();

    bool isDevToolsClientConnet() const;

private:
    // WebDevToolsFrontendClient implementation.
    // WebDevToolsAgentClient implementation.
    void sendProtocolMessage(int callId,
        const blink::WebString& response,
        const blink::WebString& state) override;
    blink::WebDevToolsAgentClient::WebKitClientMessageLoop* createClientMessageLoop() override;
    void willEnterDebugLoop() override;
    void didExitDebugLoop() override;
    void willEnterDebugLoopInRun();
    void didExitDebugLoopInRun();

    void enableTracing(const blink::WebString& category_filter) override;
    void disableTracing() override;

    DISALLOW_COPY_AND_ASSIGN(DevToolsAgent);

private:
    void closeDefersLoading();
    void openDefersLoading();

    friend class WebKitClientMessageLoopImpl;
    bool m_isAttached;
    DevToolsClient* m_devToolsClient;
    int m_id;
    WebPage* m_page;
    blink::WebLocalFrame* m_frame;
};

}  // namespace content

#endif  // CONTENT_RENDERER_DEVTOOLS_DEVTOOLS_AGENT_H_

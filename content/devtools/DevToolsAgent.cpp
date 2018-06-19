// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/devtools/DevToolsAgent.h"
#include "content/devtools/DevToolsClient.h"
#include "content/devtools/DevToolsMgr.h"
#include "content/browser/CheckReEnter.h"
#include "content/browser/WebPageImpl.h"
#include "content/browser/WebPage.h"
#include "third_party/WebKit/public/web/WebDevToolsAgent.h"
#include "third_party/WebKit/public/web/WebScriptSource.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/platform/WebThread.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/Source/bindings/core/v8/V8RecursionScope.h"
#include "gen/blink/platform/RuntimeEnabledFeatures.h"
#include <v8.h>

namespace content {

DevToolsAgent::DevToolsAgent(WebPage* page, blink::WebLocalFrame* frame)
{
    m_isAttached = false;
    m_devToolsClient = nullptr;
    m_id = DevToolsMgr::getInst()->getNewestId();
    DevToolsMgr::getInst()->addLivedId(m_id);

    m_page = page;
    m_frame = frame;
    m_frame->setDevToolsAgentClient(this);
}

DevToolsAgent::~DevToolsAgent()
{
    if (m_devToolsClient)
        m_devToolsClient->setDevToolsAgent(nullptr);
    DevToolsMgr::getInst()->removeLivedId(m_id);
}

void DevToolsAgent::onAttach(const std::string& hostId)
{
    blink::WebDevToolsAgent* webAgent = getWebAgent();
    if (webAgent) {
        webAgent->attach(blink::WebString::fromUTF8(hostId.c_str()));
        m_isAttached = true;
    }
}

void DevToolsAgent::onDetach()
{
    blink::WebDevToolsAgent* webAgent = getWebAgent();
    if (webAgent) {
        webAgent->detach();
        m_isAttached = false;
    }
}

void DevToolsAgent::setDevToolsClient(DevToolsClient* devToolsClient)
{
    m_devToolsClient = devToolsClient;
}

void DevToolsAgent::inspectElementAt(int x, int y)
{
    blink::WebDevToolsAgent* webAgent = getWebAgent();
    if (webAgent)
        webAgent->inspectElementAt(blink::WebPoint(x, y));
}

blink::WebDevToolsAgent* DevToolsAgent::getWebAgent()
{
    return m_frame ? m_frame->devToolsAgent() : nullptr;
}

bool DevToolsAgent::isDevToolsClientConnet() const
{
    return !!m_devToolsClient;
}

class InterruptMessageImpl : public blink::WebDevToolsAgent::MessageDescriptor{
public:
    InterruptMessageImpl(const std::string& message, DevToolsAgent* agent)
        : m_msg(message)
        , m_agent(agent)
        , m_devToolsAgentId(m_agent->getId())
    {
    }
    ~InterruptMessageImpl() override {}

    blink::WebDevToolsAgent* agent() override
    {
        if (!DevToolsMgr::getInst()->isLivedId(m_devToolsAgentId))
            return nullptr;
        return m_agent->getWebAgent();
    }

    blink::WebString message() override { return blink::WebString::fromUTF8(m_msg); }

private:
    std::string m_msg;
    DevToolsAgent* m_agent;
    int m_devToolsAgentId;
};

void DevToolsAgent::onMessageReceivedFromFronEnd(const std::string* message)
{
    //TRACE_EVENT0("devtools", "DevToolsAgent::OnDispatchOnInspectorBackend");
    blink::WebDevToolsAgent* webAgent = getWebAgent();
    if (!webAgent)
        return;

    if (blink::WebDevToolsAgent::shouldInterruptForMessage(blink::WebString::fromUTF8(message->c_str()))) {
        blink::WebDevToolsAgent::interruptAndDispatch(new InterruptMessageImpl(*message, this));
        return;
    }
    
    webAgent->dispatchOnInspectorBackend(blink::WebString::fromUTF8(message->c_str()));
}

class MessageToClientTask : public blink::WebThread::Task {
public:
    MessageToClientTask(DevToolsClient* devToolsClient, int callId, const blink::WebString& response, const blink::WebString& state)
    {
        m_devToolsClient = devToolsClient;
        m_devToolsClientId = devToolsClient->getId();

        m_callId = callId;
        m_response = new std::string(response.utf8());
        m_state = new std::string(state.utf8());
    }
    virtual ~MessageToClientTask() override
    {
        delete m_response;
        delete m_state;
    }
    virtual void run() override
    {
        if (!DevToolsMgr::getInst()->isLivedId(m_devToolsClientId))
            return;
        m_devToolsClient->onMessageReceivedFromEmbedder(m_callId, m_response, m_state);
    }

private:
    int m_callId;
    std::string* m_response;
    std::string* m_state;
    DevToolsClient* m_devToolsClient;
    int m_devToolsClientId;
};

void DevToolsAgent::sendProtocolMessage(int callId, const blink::WebString& response, const blink::WebString& state)
{
    if (!m_devToolsClient)
        return;

    blink::Platform::current()->currentThread()->postTask(FROM_HERE,
        new MessageToClientTask(m_devToolsClient, callId, response, state));
}

class WebKitClientMessageLoopImpl : public blink::WebDevToolsAgentClient::WebKitClientMessageLoop{
public:
    WebKitClientMessageLoopImpl(DevToolsAgent* agent)
    {
        m_quit = false;
        m_agent = agent;
    }

    ~WebKitClientMessageLoopImpl() override 
    {
    }

    void runImpl()
    {
        MSG msg;
        while (::GetMessageW(&msg, NULL, 0, 0)) {

            // 为了确保js的Promise能执行，强行执行Microtasks。本来因为isolateData->recursionLevel()的存在，执行不了。
            // 见third_party\WebKit\Source\core\dom\Microtask.cpp
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            blink::V8RecursionScope recursionScope(isolate);
            isolate->RunMicrotasks();

            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
            if (m_quit)
                return;
        }
    }

    void run() override
    {
        m_agent->willEnterDebugLoopInRun();
        runImpl();
        m_quit = false;
        m_agent->didExitDebugLoopInRun();
    }

    void quitNow() override
    {
        m_quit = true;
    }

private:
    bool m_quit;
    DevToolsAgent* m_agent;
};

blink::WebDevToolsAgentClient::WebKitClientMessageLoop* DevToolsAgent::createClientMessageLoop()
{
    return new WebKitClientMessageLoopImpl(this);
}

void DevToolsAgent::willEnterDebugLoopInRun()
{
    if (blink::RuntimeEnabledFeatures::updataInOtherThreadEnabled()) {
        RELEASE_ASSERT(0 == CheckReEnter::s_kEnterContent);
    } else
        CheckReEnter::s_kEnterContent--;
    blink::ThreadState* threadState = blink::ThreadState::current();
    threadState->enterGCForbiddenScope();

    m_page->willEnterDebugLoop();
    m_devToolsClient->willEnterDebugLoop();
}

void DevToolsAgent::didExitDebugLoopInRun()
{
    blink::ThreadState* threadState = blink::ThreadState::current();
    threadState->leaveGCForbiddenScope();
    
    // 放到这，是因为ClientMessageLoopAdapter的顺序是先执行setIgnoreInputEvents(false)，会引发断言
    m_page->didExitDebugLoop();
    m_devToolsClient->didExitDebugLoop();

    if (blink::RuntimeEnabledFeatures::updataInOtherThreadEnabled()) {
        RELEASE_ASSERT(0 == CheckReEnter::s_kEnterContent);
    } else {
        CheckReEnter::s_kEnterContent++;
    }
}

void DevToolsAgent::willEnterDebugLoop()
{

}

void DevToolsAgent::didExitDebugLoop()
{

}

void DevToolsAgent::enableTracing(const blink::WebString& category_filter)
{

}

void DevToolsAgent::disableTracing()
{

}

}
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/devtools/DevtoolsClient.h"

#include "content/devtools/DevToolsAgent.h"
#include "content/devtools/DevToolsMgr.h"
#include "content/devtools/DevToolsProtocolDispatcher.h"
#include "content/browser/WebPage.h"
#include "third_party/WebKit/public/web/WebDevToolsFrontend.h"
#include "third_party/WebKit/public/web/WebScriptSource.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/platform/WebThread.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/Source/core/frame/Frame.h"
#include "third_party/WebKit/Source/core/frame/LocalFrame.h"
#include "base/values.h"
#include "base/json/json_writer.h"
#include "base/json/json_reader.h"

namespace content {

DevToolsClient::DevToolsClient(WebPage* page, blink::WebLocalFrame* frame)
{
    m_id = DevToolsMgr::getInst()->getNewestId();
    DevToolsMgr::getInst()->addLivedId(m_id);

    m_page = page;
    m_frame = frame;
    m_webToolsFrontend = blink::WebDevToolsFrontend::create(frame, this, blink::WebString::fromLatin1("zh-cn"));
    m_preferences = new base::DictionaryValue();
    m_devToolsProtocolDispatcher = new DevToolsProtocolDispatcher(this);
}

DevToolsClient::~DevToolsClient()
{
    if (m_devToolsAgent)
        m_devToolsAgent->setDevToolsClient(nullptr);
    delete m_webToolsFrontend;
    delete m_preferences;
    delete m_devToolsProtocolDispatcher;
    DevToolsMgr::getInst()->removeLivedId(m_id);
}

void DevToolsClient::setDevToolsAgent(DevToolsAgent* devToolsAgent)
{
    m_devToolsAgent = devToolsAgent;
}

void DevToolsClient::onMessageReceivedFromEmbedder(int callId, const std::string* response, const std::string* state)
{
    std::string javascript = "DevToolsAPI.dispatchMessage(" + *response + ");";
    blink::WebScriptSource source(blink::WebString::fromUTF8(javascript.c_str()));
    m_frame->executeScript(source);
}

class MessageToAgentTask : public blink::WebThread::Task {
public:
    MessageToAgentTask(DevToolsAgent* devToolsAgent, const std::string& protocolMessage)
    {
        m_devToolsAgent = devToolsAgent;
        m_devToolsAgentId = devToolsAgent->getId();
        m_protocolMessage = new std::string(protocolMessage);
    }
    virtual ~MessageToAgentTask() override
    {
        delete m_protocolMessage;
    }
    virtual void run() override
    {
        if (!DevToolsMgr::getInst()->isLivedId(m_devToolsAgentId))
            return;
        m_devToolsAgent->onMessageReceivedFromFronEnd(m_protocolMessage);
    }

private:
    std::string* m_protocolMessage;
    DevToolsAgent* m_devToolsAgent;
    int m_devToolsAgentId;
};

class DocumentReadyToAgentTask : public blink::WebThread::Task {
public:
    DocumentReadyToAgentTask(int devToolsClientId, DevToolsAgent* devToolsAgent)
    {
        m_devToolsClientId = devToolsClientId;
        m_devToolsAgent = devToolsAgent;
        m_devToolsAgentId = devToolsAgent->getId();
    }
    virtual ~DocumentReadyToAgentTask() override
    {

    }
    virtual void run() override
    {
        if (!DevToolsMgr::getInst()->isLivedId(m_devToolsAgentId))
            return;
        String devToolsAgentId = String::format("%d", m_devToolsClientId);
        m_devToolsAgent->onAttach(devToolsAgentId.utf8().data());
    }

private:
    int m_devToolsClientId;
    DevToolsAgent* m_devToolsAgent;
    int m_devToolsAgentId;
};

void DevToolsClient::onDocumentReady()
{
    blink::Platform::current()->currentThread()->postTask(FROM_HERE, 
        new DocumentReadyToAgentTask(getId(), m_devToolsAgent));
}

void DevToolsClient::sendMessageToBackend(const blink::WebString& message)
{
    String messageStr = message;
    String output = messageStr;

    if (!m_devToolsProtocolDispatcher->dispatcher(messageStr.utf8().data()))
        blink::Platform::current()->currentThread()->postTask(FROM_HERE, new MessageToAgentTask(m_devToolsAgent, messageStr.utf8().data()));

//     output = "sendMessageToBackend:";
//     output.append(messageStr);
//     output.append("\n");
//     OutputDebugStringA(output.utf8().data());
}

void DevToolsClient::sendMessageToEmbedder(const blink::WebString& message)
{
    String messageStr = message;

    std::string method;
    base::ListValue* params = NULL;
    base::DictionaryValue* dict = NULL;
    std::unique_ptr<base::Value> parsed_message = base::JSONReader::Read(messageStr.utf8().data());
    if (!parsed_message || !parsed_message->GetAsDictionary(&dict) || !dict->GetString("method", &method))
        return;

    String output = messageStr;
    
    int requestId = 0;
    dict->GetInteger("id", &requestId);
    dict->GetList("params", &params);

    if (method == "dispatchProtocolMessage" && params && params->GetSize() == 1) {
        std::string protocolMessage;
        if (!params->GetString(0, &protocolMessage))
            return;

        if (!m_devToolsProtocolDispatcher->dispatcher(protocolMessage))
            blink::Platform::current()->currentThread()->postTask(FROM_HERE, new MessageToAgentTask(m_devToolsAgent, protocolMessage));
        
        output = "dispatchProtocolMessage:";
        output.append(protocolMessage.c_str());
        output.append("\n");
        OutputDebugStringA(output.utf8().data());
    } else if (method == "loadCompleted") {
        blink::WebScriptSource source(blink::WebString::fromUTF8("DevToolsAPI.setUseSoftMenu(true);"));
        m_frame->executeScript(source);
    } else if (method == "loadNetworkResource" && params->GetSize() == 3) {
        std::string url;
        std::string headers;
        int streamId;
        if (!params->GetString(0, &url) || !params->GetString(1, &headers) || !params->GetInteger(2, &streamId)) {
            return;
        }
        OutputDebugStringA("DevToolsClient.loadNetworkResource:");
        OutputDebugStringA(url.c_str());
        OutputDebugStringA("\n");

        OutputDebugStringA(headers.c_str());
        OutputDebugStringA("\n");

        return;
    } else if (method == "getPreferences") {
        sendMessageAck(requestId, m_preferences);
        return;
    } else if (method == "setPreference") {
        std::string name;
        std::string value;
        if (!params->GetString(0, &name) || !params->GetString(1, &value))
            return;
        m_preferences->SetStringWithoutPathExpansion(name, value);
    } else if (method == "removePreference") {
        std::string name;
        if (!params->GetString(0, &name))
            return;
        m_preferences->RemoveWithoutPathExpansion(name, nullptr);
    } else if (method == "requestFileSystems") {
        blink::WebScriptSource source(blink::WebString::fromUTF8("DevToolsAPI.fileSystemsLoaded([]);"));
        m_frame->executeScript(source);
    } else {
        return;
    }

    if (requestId)
        sendMessageAck(requestId, nullptr);

    messageStr.insert("DevToolsClient::sendMessageToEmbedder:", 0);
    messageStr.append("\n");
    OutputDebugStringA(messageStr.utf8().data());
}

bool DevToolsClient::isUnderTest()
{
    return false;
}

void DevToolsClient::callClientFunction(
    const std::string& functionName,
    const base::Value* arg1,
    const base::Value* arg2,
    const base::Value* arg3)
{
    std::string javascript = functionName + "(";
    if (arg1) {
        std::string json;
        base::JSONWriter::Write(*arg1, &json);
        javascript.append(json);
        if (arg2) {
            base::JSONWriter::Write(*arg2, &json);
            javascript.append(", ").append(json);
            if (arg3) {
                base::JSONWriter::Write(*arg3, &json);
                javascript.append(", ").append(json);
            }
        }
    }
    javascript.append(");");

    blink::WebScriptSource source(blink::WebString::fromUTF8(javascript.c_str()));
    m_frame->executeScript(source);
}


void DevToolsClient::sendMessageAck(int requestId, const base::Value* arg)
{
    base::FundamentalValue idValue(requestId);
    callClientFunction("DevToolsAPI.embedderMessageAck", &idValue, arg, nullptr);
}

void DevToolsClient::closeDefersLoading()
{
    for (blink::Frame* frame = blink::toCoreFrame(m_page->mainFrame()); frame; frame = frame->tree().traverseNext()) {
        if (!frame->isLocalFrame())
            continue;

        blink::LocalFrame* localFrame = blink::toLocalFrame(frame);
        m_defersLoadingframes.append(localFrame);
        localFrame->loader().setDefersLoading(false);
    }
}

void DevToolsClient::openDefersLoading()
{
    for (size_t i = 0; i < m_defersLoadingframes.size(); ++i) {
        blink::LocalFrame* localFrame = m_defersLoadingframes[i];
        localFrame->loader().setDefersLoading(true);
    }
}

void DevToolsClient::willEnterDebugLoop()
{
    closeDefersLoading();
    m_page->willEnterDebugLoop();
}

void DevToolsClient::didExitDebugLoop()
{
    m_page->didExitDebugLoop();
    openDefersLoading();
}

}
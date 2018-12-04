#include "content/devtools/DevToolsProtocolDispatcher.h"

#include "content/devtools/DevtoolsClient.h"
#include "content/devtools/DevToolsMgr.h"
#include "base/values.h"
#include "base/json/json_writer.h"
#include "base/json/json_reader.h"
#include "third_party/WebKit/public/platform/WebThread.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/platform/Platform.h"

namespace content {

DevToolsProtocolDispatcher::DevToolsProtocolDispatcher(DevToolsClient* devToolsClient)
{
    m_devToolsClient = devToolsClient;
}

DevToolsProtocolDispatcher::~DevToolsProtocolDispatcher()
{

}

class ProtocolMessageToClientTask : public blink::WebThread::Task {
public:
    ProtocolMessageToClientTask(DevToolsClient* devToolsClient, int commandId, std::string* protocolMessage)
    {
        m_devToolsClient = devToolsClient;
        m_devToolsClientId = m_devToolsClient->getId();
        m_protocolMessage = protocolMessage;
    }
    virtual ~ProtocolMessageToClientTask() override
    {
        delete m_protocolMessage;
    }
    virtual void run() override
    {
        if (!DevToolsMgr::getInst()->isLivedId(m_devToolsClientId))
            return;
        m_devToolsClient->onMessageReceivedFromEmbedder(m_commandId, m_protocolMessage, nullptr);
    }

private:
    std::string* m_protocolMessage;
    DevToolsClient* m_devToolsClient;
    int m_devToolsClientId;
    int m_commandId;
};

void DevToolsProtocolDispatcher::sendMessage(int commandId, const base::DictionaryValue& message)
{
    std::string* jsonMessage = new std::string();
    base::JSONWriter::Write(message, jsonMessage);
    blink::Platform::current()->currentThread()->postTask(FROM_HERE,
        new ProtocolMessageToClientTask(m_devToolsClient, commandId, jsonMessage));
}

void DevToolsProtocolDispatcher::sendSuccess(int commandId, std::unique_ptr<base::DictionaryValue> params)
{
    base::DictionaryValue response;
    response.SetInteger("id", commandId);
    response.Set("result", params.get() ? params.release() : new base::DictionaryValue());
    sendMessage(commandId, response);
}

bool DevToolsProtocolDispatcher::dispatcher(const std::string& message)
{
    std::unique_ptr<base::Value> value = base::JSONReader::Read(message);
    if (!value || !value->IsType(base::Value::TYPE_DICTIONARY))
        return false;

    base::DictionaryValue* command = static_cast<base::DictionaryValue*>(value.get());

    int id = -1;
    bool ok = command->GetInteger("id", &id) && id >= 0;
    if (!ok)
        return false;
    
    std::string method;
    ok = command->GetString("method", &method);
    if (!ok)
        return false;

    std::unique_ptr<base::DictionaryValue> result(new base::DictionaryValue());
    // gen\content\browser\devtools\protocol\devtools_protocol_dispatcher.cc
    if ("Page.enable" == method) {
        return false;
    } else if ("Page.disable" == method) {
        return false;
    } else if ("Page.reload" == method) {
        return false;
    } else if ("Page.navigate" == method) {
        return false;
    } else if ("Page.getNavigationHistory" == method) {
        return false;
    } else if ("Page.navigateToHistoryEntry" == method) {
        return false;
    } else if ("Page.captureScreenshot" == method) {
        sendSuccess(id, std::move(result));
        return true;
    } else if ("Page.canScreencast" == method) {
        result->SetBoolean("result", false);
        sendSuccess(id, std::move(result));
        return true;
    } else if ("Page.startScreencast" == method) {
        sendSuccess(id, std::move(result));
        return true;
    } else if ("Page.stopScreencast" == method) {
        sendSuccess(id, std::move(result));
        return true;
    } else if ("Page.screencastFrameAck" == method) {
        sendSuccess(id, std::move(result));
        return true;
    } else if ("Page.handleJavaScriptDialog" == method) {
        sendSuccess(id, std::move(result));
        return true;
    } else if ("Page.setColorPickerEnabled" == method) {
        sendSuccess(id, std::move(result));
        return true;
    } else if ("Page.getResourceTree" == method) {
        OutputDebugStringA("Page.getResourceTree\n");
        return false;
    }
        
    return false;
}

}
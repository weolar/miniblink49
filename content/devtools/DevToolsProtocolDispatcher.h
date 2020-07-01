
#ifndef content_devtools_DevToolsProtocolDispatcher_h
#define content_devtools_DevToolsProtocolDispatcher_h

#include <string>
#include <memory>

namespace base {
class DictionaryValue;
}

namespace content {

class DevToolsClient;
    
class DevToolsProtocolDispatcher {
public:
    DevToolsProtocolDispatcher(DevToolsClient* devToolsClient);
    ~DevToolsProtocolDispatcher();

    bool dispatcher(const std::string& message);

private:
    void sendMessage(int commandId, const base::DictionaryValue& message);
    void sendSuccess(int command_id, std::unique_ptr<base::DictionaryValue> params);
    DevToolsClient* m_devToolsClient;
};

}

#endif // content_devtools_DevToolsProtocolDispatcher_h
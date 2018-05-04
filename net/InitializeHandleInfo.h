
#ifndef net_InitializeHandleInfo_h
#define net_InitializeHandleInfo_h

#include "net/ProxyType.h"
#include "curl/curl.h"
#include <string>

namespace net {

struct SetupHttpMethodInfo;

struct InitializeHandleInfo {
    std::string url;
    std::string method;
    curl_slist* headers;
    std::string proxy;
    std::string wkeNetInterface;
    ProxyType proxyType;
    SetupHttpMethodInfo* methodInfo;

    InitializeHandleInfo() {
        methodInfo = nullptr;
    }

    ~InitializeHandleInfo();
};

}

#endif // net_InitializeHandleInfo_h
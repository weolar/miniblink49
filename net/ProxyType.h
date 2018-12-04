
#ifndef net_ProxyType_h
#define net_ProxyType_h

#include "curl/curl.h"

namespace net  {

enum ProxyType {
    HTTP = CURLPROXY_HTTP,
    Socks4 = CURLPROXY_SOCKS4,
    Socks4A = CURLPROXY_SOCKS4A,
    Socks5 = CURLPROXY_SOCKS5,
    Socks5Hostname = CURLPROXY_SOCKS5_HOSTNAME
};

}

#endif // net_ProxyType_h

#ifndef net_WebURLLoaderManagerSetupInfo_h
#define net_WebURLLoaderManagerSetupInfo_h

#include "third_party/libcurl/include/curl/curl.h"

namespace net {

struct FlattenHTTPBodyElement;

struct SetupDataInfo {
    CURLoption sizeOption;
    curl_off_t size;
    bool islongLong;
    WTF::Vector<FlattenHTTPBodyElement*> flattenElements;

    ~SetupDataInfo()
    {

    }
};

struct SetupInfoBase {
    SetupDataInfo* data;

    SetupInfoBase() { data = nullptr; }

    ~SetupInfoBase()
    {
        if (data)
            delete data;
    }
};

struct SetupPutInfo : public SetupInfoBase {
};

struct SetupPostInfo : public SetupInfoBase {
};

struct SetupHttpMethodInfo {
    SetupHttpMethodInfo()
    {
        put = nullptr;
        post = nullptr;
    }

    ~SetupHttpMethodInfo()
    {
        if (put)
            delete put;
        if (post)
            delete post;
    }
    SetupPutInfo* put;
    SetupPostInfo* post;
    //SetupDeleteInfo* del;
};

}

#endif // net_WebURLLoaderManagerSetupInfo_h
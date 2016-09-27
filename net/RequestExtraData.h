#ifndef RequestExtraData_h
#define RequestExtraData_h

#include "third_party/WebKit/public/platform/WebURLRequest.h"

namespace blink {
class WebLocalFrame;
}

class CefBrowserHostImpl;

namespace net {

class RequestExtraData : public blink::WebURLRequest::ExtraData {
public:
    virtual ~RequestExtraData() override
    {

    }

    blink::WebLocalFrame* frame;
    CefBrowserHostImpl* browser;
};

} // net

#endif // RequestExtraData_h
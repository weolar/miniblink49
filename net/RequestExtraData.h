#ifndef RequestExtraData_h
#define RequestExtraData_h

#include "third_party/WebKit/public/platform/WebURLRequest.h"

namespace blink {
class WebLocalFrame;
}
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
class CefBrowserHostImpl;
#endif
namespace net {

class RequestExtraData : public blink::WebURLRequest::ExtraData {
public:
    virtual ~RequestExtraData() override
    {

    }

    blink::WebLocalFrame* frame;
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefBrowserHostImpl* browser;
#endif
};

} // net

#endif // RequestExtraData_h
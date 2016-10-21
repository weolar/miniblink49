#ifndef RequestExtraData_h
#define RequestExtraData_h

#include "third_party/WebKit/public/platform/WebURLRequest.h"

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
namespace content {
	class WebPage;
}
#endif


#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
namespace blink {
class WebLocalFrame;
}
class CefBrowserHostImpl;
#endif
namespace net {

class RequestExtraData : public blink::WebURLRequest::ExtraData {
public:
    virtual ~RequestExtraData() override
    {

    }

    
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
	content::WebPage* page;
#endif
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
	blink::WebLocalFrame* frame;
    CefBrowserHostImpl* browser;
#endif
};

} // net

#endif // RequestExtraData_h
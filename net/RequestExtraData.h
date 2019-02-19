#ifndef RequestExtraData_h
#define RequestExtraData_h

#include "content/browser/WebPage.h"
#include "third_party/WebKit/public/platform/WebURLRequest.h"

namespace blink {
class WebLocalFrame;
}

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
namespace content {
class WebPage;
}
#endif

#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
class CefBrowserHostImpl;
#endif

namespace net {

class RequestExtraData : public blink::WebURLRequest::ExtraData {
public:
    virtual ~RequestExtraData() override
    {

    }

    blink::WebLocalFrame* getFrame()
    {
        if (!page)
            return nullptr;

        blink::WebFrame* frame = page->getWebFrameFromFrameId(m_frameId);
        if (!frame)
            return nullptr;

        if (!frame->isWebLocalFrame())
            return nullptr;

        return frame->toWebLocalFrame();
    }

    void setFrame(blink::WebLocalFrame* frame)
    {
        m_frameId = page->getFrameIdByBlinkFrame(frame);
    }

private:
    //blink::WebLocalFrame* frame;
    int64_t m_frameId;

public:
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    content::WebPage* page;
#endif

#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefBrowserHostImpl* browser;
#endif
};

} // net

#endif // RequestExtraData_h
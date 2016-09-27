
#ifndef WebFrameClientImpl_h
#define WebFrameClientImpl_h

#include "third_party/WebKit/public/web/WebConsoleMessage.h"

namespace blink {

    class WebPage;

    class WebFrameClientImpl : public WebFrameClient {
    public:
        WebFrameClientImpl(WebPage* webPage)
            : m_webPage(webPage)
        {
            ;
        }

        virtual void didAddMessageToConsole(const WebConsoleMessage& message, const WebString& sourceName, unsigned sourceLine, const WebString& stackTrace) OVERRIDE
        {
            WTF::String outstr(L"console:");

            outstr.append((WTF::String)(message.text));
            outstr.append(L" ;sourceName:");
            outstr.append(sourceName);

            outstr.append(L" ;sourceLine:");
            outstr.append(String::number(sourceLine));
            outstr.append(L" \n");
            OutputDebugStringW(outstr.charactersWithNullTermination().data());
        }

    protected:
        WebPage* m_webPage;
};

} // namespace blink

#endif // WebFrameClientImpl_h

#ifndef net_HeaderVisitor_h
#define net_HeaderVisitor_h

#include "third_party/WebKit/public/platform/WebHTTPHeaderVisitor.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "curl/curl.h"

namespace net {

class HeaderVisitor : public blink::WebHTTPHeaderVisitor {
public:
    explicit HeaderVisitor(curl_slist** headers) : m_headers(headers) {}

    virtual void visitHeader(const blink::WebString& webName, const blink::WebString& webValue) override
    {
        String value = webValue;
        String headerString(webName);
        if (value.isNull() || value.isEmpty())
            // Insert the ; to tell curl that this header has an empty value.
            headerString.append(";");
        else {
            headerString.append(": ");
            headerString.append(value);
        }
        CString headerLatin1 = headerString.latin1();
        *m_headers = curl_slist_append(*m_headers, headerLatin1.data());
    }

    curl_slist* headers() { return*m_headers; }
private:
    curl_slist** m_headers;
};
}

#endif
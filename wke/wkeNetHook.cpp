#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

#define BUILDING_wke 1

#include <config.h>
#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"
#include "third_party/WebKit/Source/platform/network/HTTPParsers.h"
#include "net/WebURLLoaderInternal.h"

#include "wke/wke.h"

void wkeNetSetHTTPHeaderField(void *job, wchar_t *key, wchar_t *value, bool response)
{
    if (response)
        ((net::WebURLLoaderInternal*)job)->m_response.setHTTPHeaderField(String(key), String(value));
    else
        ((net::WebURLLoaderInternal*)job)->firstRequest()->setHTTPHeaderField(String(key), String(value));
}
void wkeNetSetMIMEType(void *job, char *type)
{
    ((net::WebURLLoaderInternal*)job)->m_response.setMIMEType(WebString::fromUTF8(type));
}
void wkeNetSetURL(void *job, const char *url)
{
    ((net::WebURLLoaderInternal*)job)->m_response.setURL(KURL(ParsedURLString, url));
}
void wkeNetSetData(void *job, void *buf, int len)
{
    ((net::WebURLLoaderInternal*)job)->client()->didReceiveResponse(((net::WebURLLoaderInternal*)job)->loader(), ((net::WebURLLoaderInternal*)job)->m_response);
    ((net::WebURLLoaderInternal*)job)->client()->didReceiveData(((net::WebURLLoaderInternal*)job)->loader(), static_cast<char *>(buf), len, 0);
}
void wkeNetHookRequest(void *job)
{
    ((net::WebURLLoaderInternal*)job)->m_isHookRequest = true;
}
//WebURLResponse req = job->m_response;
//req.setHTTPStatusText(String("OK"));
//req.setHTTPHeaderField("Content-Leng", "4");
//req.setHTTPHeaderField("Content-Type", "text/html");
//req.setExpectedContentLength(static_cast<long long int>(4));
//req.setURL(KURL(ParsedURLString, "http://127.0.0.1/a.html"));
//req.setHTTPStatusCode(200);
//req.setMIMEType(extractMIMETypeFromMediaType(req.httpHeaderField(WebString::fromUTF8("Content-Type"))).lower());

//req.setTextEncodingName(extractCharsetFromMediaType(req.httpHeaderField(WebString::fromUTF8("Content-Type"))));
//job->client()->didReceiveResponse(job->loader(), req);
//job->setResponseFired(true);

//job->client()->didReceiveData(job->loader(), "aaaa", 4, 0);
//job->client()->didFinishLoading(job->loader(), WTF::currentTime(), 0);



#endif

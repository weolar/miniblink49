#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

#define BUILDING_wke 1

#include <config.h>
#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"
#include "third_party/WebKit/Source/platform/network/HTTPParsers.h"
#include "net/WebURLLoaderInternal.h"

#include "wke/wke.h"
#include "wke/wkeString.h"

void wkeNetSetHTTPHeaderField(void* jobPtr, wchar_t *key, wchar_t *value, bool response)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;

    if (response)
        job->m_response.setHTTPHeaderField(String(key), String(value));
    else {
        String keyString(key);
        if (equalIgnoringCase(keyString, "referer")) {
            job->firstRequest()->setHTTPReferrer(String(value), WebReferrerPolicyDefault);
        } else
            job->firstRequest()->setHTTPHeaderField(keyString, String(value));
    }
}

void wkeNetSetMIMEType(void* jobPtr, char* type)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    job->m_response.setMIMEType(WebString::fromUTF8(type));
}

void wkeNetGetMIMEType(void* jobPtr, wkeString mime)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    AtomicString contentType = job->m_response.httpHeaderField(WebString::fromUTF8("Content-Type"));
    WTF::CString contentTypeUtf8 = contentType.utf8();
    mime->setString(contentTypeUtf8.data(), contentTypeUtf8.length());
}

void wkeNetSetURL(void* jobPtr, const char *url)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    job->m_response.setURL(KURL(ParsedURLString, url));
}

void wkeNetSetData(void *jobPtr, void *buf, int len)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    WebURLLoaderClient* client = job->client();
    WebURLLoaderImplCurl* loader = job->loader();

    //client->didReceiveResponse(loader, job->m_response);
    //client->didReceiveData(loader, static_cast<char *>(buf), len, 0);
    if (job->m_asynWkeNetSetData)
        free(job->m_asynWkeNetSetData);

    if (0 != len) {
        job->m_asynWkeNetSetData = malloc(len);
        job->m_asynWkeNetSetDataLength = len;
        memcpy(job->m_asynWkeNetSetData, buf, len);
    }
    job->m_isWkeNetSetDataBeSetted = true;
}

void wkeNetHookRequest(void *jobPtr)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    job->m_isHookRequest = true;
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

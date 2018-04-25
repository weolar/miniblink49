#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

#define BUILDING_wke 1

#include <config.h>

#include "wke/wkeNetHook.h"
#include "wke/wke.h"
#include "wke/wkeString.h"

#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"
#include "third_party/WebKit/Source/platform/network/HTTPParsers.h"
#include "net/WebURLLoaderInternal.h"
#include "net/FlattenHTTPBodyElement.h"

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

void wkeNetSetData(void* jobPtr, void* buf, int len)
{
    if (0 == len)
        return;

    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    WebURLLoaderClient* client = job->client();
    WebURLLoaderImplCurl* loader = job->loader();

    if (job->m_hookBuf) {
        free(job->m_hookBuf);

        job->m_hookBuf = malloc(len);
        job->m_hookLength = len;
        memcpy(job->m_hookBuf, buf, len);

        return;
    }

    if (job->m_asynWkeNetSetData)
        free(job->m_asynWkeNetSetData);


    job->m_asynWkeNetSetData = malloc(len);
    job->m_asynWkeNetSetDataLength = len;
    memcpy(job->m_asynWkeNetSetData, buf, len);
    
    job->m_isWkeNetSetDataBeSetted = true;
}

void wkeNetHookRequest(void *jobPtr)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    job->m_isHookRequest = true;
}

wkePostFlattenBodyElements* wkeCreatePostFlattenBodyElements(wkeWebView webView, size_t length)
{
    if (0 == length)
        return nullptr;

    wkePostFlattenBodyElements* result = new wkePostFlattenBodyElements();
    result->size = sizeof(wkePostFlattenBodyElements);
    result->isDirty = false;

    size_t allocLength = sizeof(wkePostFlattenBodyElement*) * length;
    result->element = (wkePostFlattenBodyElement**)malloc(allocLength);
    memset(result->element, 0, allocLength);

    result->elementSize = length;

    return result;
}

void wkeFreePostFlattenBodyElements(wkePostFlattenBodyElements* elements)
{
    for (size_t i = 0; i < elements->elementSize; ++i) {
        wkeFreePostFlattenBodyElement(elements->element[i]);
    }
    free(elements->element);
    delete elements;
}

wkePostFlattenBodyElement* wkeCreatePostFlattenBodyElement(wkeWebView webView)
{
    wkePostFlattenBodyElement* wkeElement = new wkePostFlattenBodyElement();
    wkeElement->size = sizeof(wkePostFlattenBodyElement);
    return wkeElement;
}

void wkeFreePostFlattenBodyElement(wkePostFlattenBodyElement* element)
{
    wkeFreeMemBuf(element->data);
    wkeDeleteString(element->filePath);
    delete element;
}

wkeMemBuf* wkeCreateMemBuf(wkeWebView webView, void* buf, size_t length)
{
    if (0 == length)
        return nullptr;
    wkeMemBuf* result = (wkeMemBuf*)malloc(sizeof(wkeMemBuf));
    result->size = sizeof(wkeMemBuf);
    result->length = length;
    result->data = malloc(length);
    memcpy(result->data, buf, length);
    return result;
}

void wkeFreeMemBuf(wkeMemBuf* buf)
{
    if (!buf)
        return;
    if (buf->data)
        free(buf->data);
    free(buf);
}

namespace wke {

wkePostFlattenBodyElements* flattenHTTPBodyElementToWke(const WTF::Vector<net::FlattenHTTPBodyElement*>& body)
{
    if (0 == body.size())
        return nullptr;

    wkePostFlattenBodyElements* result = wkeCreatePostFlattenBodyElements(nullptr, body.size());
    for (size_t i = 0; i < result->elementSize; ++i) {
        wkePostFlattenBodyElement*wkeElement = wkeCreatePostFlattenBodyElement(nullptr);
        result->element[i] = wkeElement;
        const net::FlattenHTTPBodyElement* element = body[i];

        if (blink::WebHTTPBody::Element::Type::TypeFile == element->type ||
            blink::WebHTTPBody::Element::Type::TypeFileSystemURL == element->type) {

            wkeElement->type = wkeHttBodyElementTypeFile;
            wkeElement->filePath = wkeCreateStringW(element->filePath.c_str(), element->filePath.size());
            wkeElement->fileLength = element->fileLength;
            wkeElement->fileStart = element->fileStart;
            wkeElement->data = nullptr;
        } else {
            wkeElement->type = wkeHttBodyElementTypeData;
            wkeElement->filePath = nullptr;
            wkeElement->fileLength = 0;
            wkeElement->fileStart = 0;
            wkeElement->data = wkeCreateMemBuf(nullptr, (void*)element->data.data(), element->data.size());
        }
    }
    return result;
}

void wkeflattenElementToBlink(const wkePostFlattenBodyElements& body, WTF::Vector<net::FlattenHTTPBodyElement*>* out)
{
    out->clear();

    if (0 == body.elementSize)
        return;

    for (size_t i = 0; i < body.elementSize; ++i) {
        const wkePostFlattenBodyElement* wkeElement = body.element[i];
        net::FlattenHTTPBodyElement* blinkElement = new net::FlattenHTTPBodyElement();

        blinkElement->type = (wkeElement->type == wkeHttBodyElementTypeFile ? 
            blink::WebHTTPBody::Element::TypeFile : blink::WebHTTPBody::Element::TypeData);

        if (blink::WebHTTPBody::Element::Type::TypeFile == blinkElement->type) {
            const wchar_t* filePath = wkeGetStringW(wkeElement->filePath);
            blinkElement->filePath = filePath;
            blinkElement->fileLength = wkeElement->fileLength;
            blinkElement->fileStart = wkeElement->fileStart;
        } else {
            if (wkeElement->data && wkeElement->data->length) {
                blinkElement->data.resize(wkeElement->data->length);
                memcpy(blinkElement->data.data(), wkeElement->data->data, wkeElement->data->length);
            }
        }
        out->append(blinkElement);
    }
}
    
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

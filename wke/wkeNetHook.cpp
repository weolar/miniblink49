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
#include "net/InitializeHandleInfo.h"
#include "net/WebURLLoaderManagerSetupInfo.h"
#include "net/WebURLLoaderManager.h"
#include "net/HeaderVisitor.h"

void wkeNetSetHTTPHeaderField(void* jobPtr, wchar_t* key, wchar_t* value, bool response)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;

    if (response) {
        job->m_response.setHTTPHeaderField(String(key), String(value));
    } else {
        String keyString(key);
        if (equalIgnoringCase(keyString, "referer"))
            job->firstRequest()->setHTTPReferrer(String(value), WebReferrerPolicyDefault);
        else
            job->firstRequest()->setHTTPHeaderField(keyString, String(value));

        if (job->m_initializeHandleInfo) { // setHttpResponseDataToJobWhenDidReceiveResponseOnMainThread里m_initializeHandleInfo为空
            curl_slist* headers = job->m_initializeHandleInfo->headers;
            curl_slist_free_all(headers);
            headers = nullptr;
            net::HeaderVisitor visitor(&headers);
            job->firstRequest()->visitHTTPHeaderFields(&visitor);
            job->m_initializeHandleInfo->headers = headers;
        }
    }
}

const char* wkeNetGetHTTPHeaderField(void* jobPtr, const char* key)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    String value = job->firstRequest()->httpHeaderField(String(key));
    Vector<char> valueBuffer = WTF::ensureStringToUTF8(value, false);

    return wke::createTempCharString(valueBuffer.data(), valueBuffer.size());
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

// void wkeNetSetURL(void* jobPtr, const char* url)
// {
//     net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
//     KURL kurl(ParsedURLString, url);
//     job->m_response.setURL(kurl);
//     job->firstRequest()->setURL(kurl);
//     job->m_initializeHandleInfo->url = url;
//     ASSERT(!job->m_url);
// }

void wkeNetSetData(void* jobPtr, void* buf, int len)
{
    if (0 == len)
        return;

    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    WebURLLoaderClient* client = job->client();
    WebURLLoaderImplCurl* loader = job->loader();

    if (job->m_hookBufForEndHook) {
        free(job->m_hookBufForEndHook);

        job->m_hookBufForEndHook = malloc(len);
        job->m_hookLength = len;
        memcpy(job->m_hookBufForEndHook, buf, len);

        return;
    }

    if (job->m_asynWkeNetSetData)
        free(job->m_asynWkeNetSetData);

    job->m_asynWkeNetSetData = malloc(len);
    job->m_asynWkeNetSetDataLength = len;
    memcpy(job->m_asynWkeNetSetData, buf, len);
    
    job->m_isWkeNetSetDataBeSetted = true;
}

void wkeNetHookRequest(void* jobPtr)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    job->m_isWkeNetSetDataBeSetted = false;
    if (job->m_asynWkeNetSetData)
        free(job->m_asynWkeNetSetData);
    job->m_asynWkeNetSetData = nullptr;
    job->m_isHoldJobToAsynCommit = false;

    job->m_isHookRequest = true;
}

void wkeNetCancelRequest(void* jobPtr)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    job->m_cancelledReason = net::kNormalCancelled;
}

const char* wkeNetGetUrlByJob(void* jobPtr)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    blink::KURL kurl = job->firstRequest()->url();

    CString urlString = kurl.getUTF8String().utf8();
    return wke::createTempCharString(urlString.data(), urlString.length());
}

void wkeNetContinueJob(void* jobPtr)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    net::WebURLLoaderManager::sharedInstance()->continueJob(job);
}

void wkeNetChangeRequestUrl(void* jobPtr, const char* url)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    blink::KURL newUrl(blink::ParsedURLString, url);
    job->firstRequest()->setURL(newUrl);
    job->m_initializeHandleInfo->url = url;
}

void wkeNetHoldJobToAsynCommit(void* jobPtr)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;

    job->m_isWkeNetSetDataBeSetted = false;
    if (job->m_asynWkeNetSetData)
        free(job->m_asynWkeNetSetData);
    job->m_asynWkeNetSetData = nullptr;

    if (job->m_hookBufForEndHook)
        free(job->m_hookBufForEndHook);
    job->m_hookBufForEndHook = nullptr;

    job->m_isHookRequest = false;

    job->m_isHoldJobToAsynCommit = true;
}

wkeRequestType wkeNetGetRequestMethod(void *jobPtr)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    net::InitializeHandleInfo* info = job->m_initializeHandleInfo;
    if (!info)
        return kWkeRequestTypeInvalidation;

    WTF::Vector<net::FlattenHTTPBodyElement*>* flattenElements = nullptr;
    if ("POST" == info->method) {
        return kWkeRequestTypePost;
    } else if ("PUT" == info->method) {
        return kWkeRequestTypePut;
    } else if ("GET" == info->method) {
        return kWkeRequestTypeGet;
    }
    return kWkeRequestTypeInvalidation;
}

// wkePostBodyElements* wkeNetGetPostBody(void *jobPtr)
// {
//     net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
//     net::InitializeHandleInfo* info = job->m_initializeHandleInfo;
//     if (!info)
//         return nullptr;
// 
//     WTF::Vector<net::FlattenHTTPBodyElement*>* flattenElements = nullptr;
//     if ("POST" == info->method) {
//         flattenElements = &info->methodInfo->post->data->flattenElements;
//     } else if ("PUT" == info->method) {
//         flattenElements = &info->methodInfo->put->data->flattenElements;
//     }
//     if (!flattenElements)
//         return nullptr;
// 
//     wkePostBodyElements* postBody = wke::flattenHTTPBodyElementToWke(*flattenElements);
//     return postBody;
// }
//
// wkePostBodyElements* wkeNetCreatePostBodyElements(wkeWebView webView, size_t length)
// {
//     if (0 == length)
//         return nullptr;
// 
//     wkePostBodyElements* result = new wkePostBodyElements();
//     result->size = sizeof(wkePostBodyElements);
//     result->isDirty = true;
// 
//     size_t allocLength = sizeof(wkePostBodyElement*) * length;
//     result->element = (wkePostBodyElement**)malloc(allocLength);
//     memset(result->element, 0, allocLength);
// 
//     result->elementSize = length;
// 
//     return result;
// }
// 
// void wkeNetFreePostBodyElements(wkePostBodyElements* elements)
// {
//     for (size_t i = 0; i < elements->elementSize; ++i) {
//         wkeNetFreePostBodyElement(elements->element[i]);
//     }
//     free(elements->element);
//     delete elements;
// }
// 
// wkePostBodyElement* wkeNetCreatePostBodyElement(wkeWebView webView)
// {
//     wkePostBodyElement* wkeElement = new wkePostBodyElement();
//     wkeElement->size = sizeof(wkePostBodyElement);
//     return wkeElement;
// }
// 
// void wkeNetFreePostBodyElement(wkePostBodyElement* element)
// {
//     wkeFreeMemBuf(element->data);
//     wkeDeleteString(element->filePath);
//     delete element;
// }

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

// wkePostBodyElements* flattenHTTPBodyElementToWke(const WTF::Vector<net::FlattenHTTPBodyElement*>& body)
// {
//     if (0 == body.size())
//         return nullptr;
// 
//     wkePostBodyElements* result = wkeNetCreatePostBodyElements(nullptr, body.size());
//     result->isDirty = false;
//     for (size_t i = 0; i < result->elementSize; ++i) {
//         wkePostBodyElement*wkeElement = wkeNetCreatePostBodyElement(nullptr);
//         result->element[i] = wkeElement;
//         const net::FlattenHTTPBodyElement* element = body[i];
// 
//         if (blink::WebHTTPBody::Element::Type::TypeFile == element->type ||
//             blink::WebHTTPBody::Element::Type::TypeFileSystemURL == element->type) {
// 
//             wkeElement->type = wkeHttBodyElementTypeFile;
//             wkeElement->filePath = wkeCreateStringW(element->filePath.c_str(), element->filePath.size());
//             wkeElement->fileLength = element->fileLength;
//             wkeElement->fileStart = element->fileStart;
//             wkeElement->data = nullptr;
//         } else {
//             wkeElement->type = wkeHttBodyElementTypeData;
//             wkeElement->filePath = nullptr;
//             wkeElement->fileLength = 0;
//             wkeElement->fileStart = 0;
//             wkeElement->data = wkeCreateMemBuf(nullptr, (void*)element->data.data(), element->data.size());
//         }
//     }
//     return result;
// }
// 
// void wkeflattenElementToBlink(const wkePostBodyElements& body, WTF::Vector<net::FlattenHTTPBodyElement*>* out)
// {
//     out->clear();
// 
//     if (0 == body.elementSize)
//         return;
// 
//     for (size_t i = 0; i < body.elementSize; ++i) {
//         const wkePostBodyElement* wkeElement = body.element[i];
//         net::FlattenHTTPBodyElement* blinkElement = new net::FlattenHTTPBodyElement();
// 
//         blinkElement->type = (wkeElement->type == wkeHttBodyElementTypeFile ? 
//             blink::WebHTTPBody::Element::TypeFile : blink::WebHTTPBody::Element::TypeData);
// 
//         if (blink::WebHTTPBody::Element::Type::TypeFile == blinkElement->type) {
//             const wchar_t* filePath = wkeGetStringW(wkeElement->filePath);
//             blinkElement->filePath = filePath;
//             blinkElement->fileLength = wkeElement->fileLength;
//             blinkElement->fileStart = wkeElement->fileStart;
//         } else {
//             if (wkeElement->data && wkeElement->data->length) {
//                 blinkElement->data.resize(wkeElement->data->length);
//                 memcpy(blinkElement->data.data(), wkeElement->data->data, wkeElement->data->length);
//             }
//         }
//         out->append(blinkElement);
//     }
// }
    
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

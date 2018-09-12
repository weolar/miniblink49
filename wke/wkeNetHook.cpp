#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

#define BUILDING_wke 1

#include <config.h>

#include "wke/wkeNetHook.h"
#include "wke/wke.h"
#include "wke/wkeString.h"
#include "wke/wkeUtil.h"
#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/Source/platform/network/HTTPParsers.h"
#include "net/WebURLLoaderInternal.h"
#include "net/WebURLLoaderManagerUtil.h"
#include "net/FlattenHTTPBodyElement.h"
#include "net/InitializeHandleInfo.h"
#include "net/WebURLLoaderManagerSetupInfo.h"
#include "net/WebURLLoaderManager.h"
#include "net/HeaderVisitor.h"

void wkeNetSetHTTPHeaderField(wkeNetJob jobPtr, wchar_t* key, wchar_t* value, bool response)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

const char* wkeNetGetHTTPHeaderField(wkeNetJob jobPtr, const char* key)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    String value = job->firstRequest()->httpHeaderField(String(key));
    Vector<char> valueBuffer = WTF::ensureStringToUTF8(value, false);

    return wke::createTempCharString(valueBuffer.data(), valueBuffer.size());
}

void wkeNetSetMIMEType(wkeNetJob jobPtr, char* type)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    job->m_response.setMIMEType(WebString::fromUTF8(type));
}

const char* wkeNetGetMIMEType(wkeNetJob jobPtr, wkeString mime)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    AtomicString contentType = job->m_response.httpHeaderField(WebString::fromUTF8("Content-Type"));
    WTF::CString contentTypeUtf8 = contentType.utf8();

    if (mime)
        mime->setString(contentTypeUtf8.data(), contentTypeUtf8.length());

    return wke::createTempCharString(contentTypeUtf8.data(), contentTypeUtf8.length());
}

void wkeNetSetData(wkeNetJob jobPtr, void* buf, int len)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (0 == len)
        return;

    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    WebURLLoaderClient* client = job->client();
    WebURLLoaderImplCurl* loader = job->loader();

    if (job->m_hookBufForEndHook) {
        job->m_hookBufForEndHook->resize(len);
        memcpy(job->m_hookBufForEndHook->data(), buf, len);
        return;
    }

    if (!job->m_asynWkeNetSetData)
        job->m_asynWkeNetSetData = new Vector<char>();
    job->m_asynWkeNetSetData->resize(len);
    memcpy(job->m_asynWkeNetSetData->data(), buf, len);
    
    job->m_isWkeNetSetDataBeSetted = true;
}

void wkeNetHookRequest(wkeNetJob jobPtr)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    job->m_isWkeNetSetDataBeSetted = false;
    if (job->m_asynWkeNetSetData)
        delete (job->m_asynWkeNetSetData);
    job->m_asynWkeNetSetData = nullptr;
    job->m_isHoldJobToAsynCommit = false;

    job->m_isHookRequest = true;
}

void wkeNetCancelRequest(wkeNetJob jobPtr)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    job->m_cancelledReason = net::kNormalCancelled;
}

const char* wkeNetGetUrlByJob(wkeNetJob jobPtr)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    blink::KURL kurl = job->firstRequest()->url();

    CString urlString = kurl.getUTF8String().utf8();
    return wke::createTempCharString(urlString.data(), urlString.length());
}

void wkeNetContinueJob(wkeNetJob jobPtr)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    net::WebURLLoaderManager::sharedInstance()->continueJob(job);
}

// void wkeNetSetURL(wkeNetJob jobPtr, const char* url)
// {
//     net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
//     KURL kurl(ParsedURLString, url);
//     job->m_response.setURL(kurl);
//     job->firstRequest()->setURL(kurl);
//     job->m_initializeHandleInfo->url = url;
//     ASSERT(!job->m_url);
// }

void wkeNetChangeRequestUrl(wkeNetJob jobPtr, const char* url)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    blink::KURL newUrl(blink::ParsedURLString, url);
    job->m_response.setURL(newUrl);
    job->firstRequest()->setURL(newUrl);
    job->m_initializeHandleInfo->url = url;
    ASSERT(!job->m_url);
}

void wkeNetHoldJobToAsynCommit(wkeNetJob jobPtr)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;

    job->m_isWkeNetSetDataBeSetted = false;
    if (job->m_asynWkeNetSetData)
        delete job->m_asynWkeNetSetData;
    job->m_asynWkeNetSetData = nullptr;

    if (job->m_hookBufForEndHook)
        delete job->m_hookBufForEndHook;
    job->m_hookBufForEndHook = nullptr;

    job->m_isHookRequest &= (~((unsigned int)1));

    job->m_isHoldJobToAsynCommit = true;
}

wkeRequestType wkeNetGetRequestMethod(void *jobPtr)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

wkePostBodyElements* wkeNetGetPostBody(void *jobPtr)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    net::InitializeHandleInfo* info = job->m_initializeHandleInfo;
    if (!info)
        return nullptr;

    WTF::Vector<net::FlattenHTTPBodyElement*>* flattenElements = nullptr;
    if ("POST" == info->method) {
        if (!info->methodInfo || !info->methodInfo->post || !info->methodInfo->post->data)
            return nullptr;
        flattenElements = &info->methodInfo->post->data->flattenElements;
    } else if ("PUT" == info->method) {
        if (!info->methodInfo || !info->methodInfo->put || !info->methodInfo->put->data)
            return nullptr;
        flattenElements = &info->methodInfo->put->data->flattenElements;
    }
    if (!flattenElements)
        return nullptr;

    wkePostBodyElements* postBody = wke::flattenHTTPBodyElementToWke(*flattenElements);
    return postBody;
}

wkePostBodyElements* wkeNetCreatePostBodyElements(wkeWebView webView, size_t length)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (0 == length)
        return nullptr;

    wkePostBodyElements* result = new wkePostBodyElements();
    result->size = sizeof(wkePostBodyElements);
    result->isDirty = true;

    size_t allocLength = sizeof(wkePostBodyElement*) * length;
    result->element = (wkePostBodyElement**)malloc(allocLength);
    memset(result->element, 0, allocLength);

    result->elementSize = length;

    return result;
}

void wkeNetFreePostBodyElements(wkePostBodyElements* elements)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    for (size_t i = 0; i < elements->elementSize; ++i) {
        wkeNetFreePostBodyElement(elements->element[i]);
    }
    free(elements->element);
    delete elements;
}

wkePostBodyElement* wkeNetCreatePostBodyElement(wkeWebView webView)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    wkePostBodyElement* wkeElement = new wkePostBodyElement();
    wkeElement->size = sizeof(wkePostBodyElement);
    return wkeElement;
}

void wkeNetFreePostBodyElement(wkePostBodyElement* element)
{
    wkeFreeMemBuf(element->data);
    wkeDeleteString(element->filePath);
    delete element;
}

wkeMemBuf* wkeCreateMemBuf(wkeWebView webView, void* buf, size_t length)
{
    if (!buf || 0 == length)
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

int wkeNetGetFavicon(wkeWebView webView, wkeOnNetGetFaviconCallback callback, void* param)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    return net::getFavicon(webView, callback, param);
}

struct wkeWebUrlRequest {
    blink::WebURLRequest m_resourceRequest;

    wkeWebUrlRequest(const blink::WebURLRequest& resourceRequest)
    {
        m_resourceRequest = resourceRequest;
    }

    wkeWebUrlRequest(const utf8* url, const utf8* method, const utf8* mime)
    {
        m_resourceRequest.initialize();

        blink::KURL kurl(blink::ParsedURLString, url);
        m_resourceRequest.setURL(kurl);
        m_resourceRequest.setHTTPMethod(blink::WebString::fromUTF8(method));
    }

    void addHTTPHeaderField(const utf8* name, const utf8* value)
    {
        m_resourceRequest.addHTTPHeaderField(blink::WebString::fromUTF8(name), blink::WebString::fromUTF8(value));
    }
};

struct wkeWebUrlResponse {
    wkeWebUrlResponse(const blink::WebURLResponse& response)
    {
        m_response = response;
    }
    blink::WebURLResponse m_response;
};

struct NetUrlRequest : public net::JobHead, public blink::WebURLLoaderClient {
public:
    NetUrlRequest(wkeWebView webView, wkeWebUrlRequestPtr request, void* param, wkeUrlRequestCallbacks callbacks)
    {
        m_ref = 0;
        m_id = 0;
        m_type = net::JobHead::kWkeCustomNetRequest;
        m_webView = webView;
        if (m_webView)
            m_webviewId = wkeGetWebviewId(webView);
        m_resourceRequest = request->m_resourceRequest;
        
        m_callbackParam = nullptr;
        m_didReceiveDataCallback = nullptr;
        m_didFailCallback = nullptr;
        m_didFinishLoadingCallback = nullptr;
        m_loader = nullptr;

        setCallback(param, callbacks);
    }

    ~NetUrlRequest()
    {
        if (m_loader) {
            m_loader->cancel();
            delete m_loader;
        }
    }

    void start()
    {
        m_loader = blink::Platform::current()->createURLLoader();
        m_loader->loadAsynchronously(m_resourceRequest, this);
    }

    void setCallback(void* param, wkeUrlRequestCallbacks callbacks)
    {
        m_callbackParam = param;
        m_willRedirectCallback = callbacks.willRedirectCallback;
        m_didReceiveResponseCallback = callbacks.didReceiveResponseCallback;
        m_didReceiveDataCallback = callbacks.didReceiveDataCallback;
        m_didFailCallback = callbacks.didFailCallback;
        m_didFinishLoadingCallback = callbacks.didFinishLoadingCallback;
    }

    // WebURLLoaderClient
    virtual void willSendRequest(blink::WebURLLoader*, blink::WebURLRequest& newRequest, const blink::WebURLResponse& redirectResponse) override
    {
        wkeWebUrlRequest oldWebRequest(m_resourceRequest);
        wkeWebUrlRequest newWebRequest(newRequest);
        wkeWebUrlResponse webRedirectResponse(redirectResponse);
        if (m_willRedirectCallback) {
            m_willRedirectCallback(m_webView, m_callbackParam, &oldWebRequest, &newWebRequest, &webRedirectResponse);
        }
    }

    virtual void didReceiveResponse(blink::WebURLLoader*, const blink::WebURLResponse& response) override
    {
        wkeWebUrlRequest webRequest(m_resourceRequest);
        wkeWebUrlResponse webResponse(response);
        if (m_didReceiveResponseCallback)
            m_didReceiveResponseCallback(m_webView, m_callbackParam, &webRequest, &webResponse);
    }

    virtual void didReceiveData(blink::WebURLLoader*, const char* data, int dataLength, int encodedDataLength) override
    {
        wkeWebUrlRequest webRequest(m_resourceRequest);
        if (m_didReceiveDataCallback)
            m_didReceiveDataCallback(m_webView, m_callbackParam, &webRequest, data, dataLength);
    }

    virtual void didFail(blink::WebURLLoader* loader, const blink::WebURLError& error) override
    {
        wkeWebUrlRequest webRequest(m_resourceRequest);
        if (m_didFailCallback)
            m_didFailCallback(m_webView, m_callbackParam, &webRequest, error.localizedDescription.utf8().c_str());

        delete this;
    }

    virtual void didFinishLoading(blink::WebURLLoader* loader, double finishTime, int64_t totalEncodedDataLength) override
    {
        wkeWebUrlRequest webRequest(m_resourceRequest);
        if (m_didFinishLoadingCallback)
            m_didFinishLoadingCallback(m_webView, m_callbackParam, &webRequest, finishTime);
        delete this;
    }

private:
    int m_webviewId;
    wkeWebView m_webView;
    blink::WebURLLoader* m_loader;
    blink::WebURLRequest m_resourceRequest;

    void* m_callbackParam;
    wkeOnUrlRequestWillRedirectCallback m_willRedirectCallback;
    wkeOnUrlRequestDidReceiveResponseCallback m_didReceiveResponseCallback;
    wkeOnUrlRequestDidReceiveDataCallback m_didReceiveDataCallback;
    wkeOnUrlRequestDidFailCallback m_didFailCallback;
    wkeOnUrlRequestDidFinishLoadingCallback m_didFinishLoadingCallback;
};

wkeWebUrlRequestPtr wkeNetCreateWebUrlRequest(const utf8* url, const utf8* method, const utf8* mime)
{
    return new wkeWebUrlRequest(url, method, mime);
}

void wkeNetAddHTTPHeaderFieldToUrlRequest(wkeWebUrlRequestPtr request, const utf8* name, const utf8* value)
{
    request->addHTTPHeaderField(name, value);
}

int wkeNetGetHttpStatusCode(wkeWebUrlResponsePtr response)
{
    return response->m_response.httpStatusCode();
}

long long wkeNetGetExpectedContentLength(wkeWebUrlResponsePtr response)
{
    return response->m_response.expectedContentLength();
}

const utf8* wkeNetGetResponseUrl(wkeWebUrlResponsePtr response)
{
    blink::KURL kurl = response->m_response.url();
    String url = kurl.getUTF8String();
    return wke::createTempCharString((const char*)url.characters8(), url.length());
}

void wkeNetStartUrlRequest(wkeWebView webView, wkeWebUrlRequestPtr request, void* param, wkeUrlRequestCallbacks callbacks)
{
    NetUrlRequest* netRequest = new NetUrlRequest(webView, request, param, callbacks);
    netRequest->start();
}

namespace wke {

wkePostBodyElements* flattenHTTPBodyElementToWke(const WTF::Vector<net::FlattenHTTPBodyElement*>& body)
{
    if (0 == body.size())
        return nullptr;

    wkePostBodyElements* result = wkeNetCreatePostBodyElements(nullptr, body.size());
    result->isDirty = false;
    for (size_t i = 0; i < result->elementSize; ++i) {
        wkePostBodyElement*wkeElement = wkeNetCreatePostBodyElement(nullptr);
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

void wkeflattenElementToBlink(const wkePostBodyElements& body, WTF::Vector<net::FlattenHTTPBodyElement*>* out)
{
    out->clear();

    if (0 == body.elementSize)
        return;

    for (size_t i = 0; i < body.elementSize; ++i) {
        const wkePostBodyElement* wkeElement = body.element[i];
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

#endif

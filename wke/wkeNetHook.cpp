#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

#define BUILDING_wke 1

#include <config.h>

#include "wke/wkeNetHook.h"
#include "wke/wke.h"
#include "wke/wkeString.h"
#include "wke/wkeUtil.h"
#include "wke/wkeGlobalVar.h"
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
#include "net/DiskCache.h"

void WKE_CALL_TYPE wkeNetSetHTTPHeaderField(wkeNetJob jobPtr, const wchar_t* key, const wchar_t* value, bool response)
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

const char* WKE_CALL_TYPE wkeNetGetHTTPHeaderField(wkeNetJob jobPtr, const char* key)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    String value = job->firstRequest()->httpHeaderField(String(key));
    Vector<char> valueBuffer = WTF::ensureStringToUTF8(value, false);

    return wke::createTempCharString(valueBuffer.data(), valueBuffer.size());
}

const char* WKE_CALL_TYPE wkeNetGetHTTPHeaderFieldFromResponse(wkeNetJob jobPtr, const char* key)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    String value = job->m_response.httpHeaderField(String(key));
    Vector<char> valueBuffer = WTF::ensureStringToUTF8(value, false);

    return wke::createTempCharString(valueBuffer.data(), valueBuffer.size());
}

void WKE_CALL_TYPE wkeNetSetMIMEType(wkeNetJob jobPtr, const char* type)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    job->m_response.setMIMEType(WebString::fromUTF8(type));
}

// void wkeNetSetMIMETypeToRequest(wkeNetJob jobPtr, const char* type)
// {
// 
// }

const char* WKE_CALL_TYPE wkeNetGetMIMEType(wkeNetJob jobPtr, wkeString mime)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    AtomicString contentType = job->m_response.httpHeaderField(WebString::fromUTF8("Content-Type"));
    WTF::CString contentTypeUtf8 = contentType.utf8();

    if (mime)
        mime->setString(contentTypeUtf8.data(), contentTypeUtf8.length());

    return wke::createTempCharString(contentTypeUtf8.data(), contentTypeUtf8.length());
}

// const char* wkeNetGetMIMETypeFromRequest(wkeNetJob jobPtr)
// {
//     wke::checkThreadCallIsValid(__FUNCTION__);
//     net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
//     AtomicString contentType = job->firstRequest()->httpHeaderField(WebString::fromUTF8("Content-Type"));
//     WTF::CString contentTypeUtf8 = contentType.utf8();
// 
//     return wke::createTempCharString(contentTypeUtf8.data(), contentTypeUtf8.length());
// }

void WKE_CALL_TYPE wkeNetSetData(wkeNetJob jobPtr, void* buf, int len)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (0 == len) {
        len = 1;
        buf = " ";
    }

    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    WebURLLoaderClient* client = job->client();
    WebURLLoaderImplCurl* loader = job->loader();

    if (job->m_hookBufForEndHook) {
        job->m_hookBufForEndHook->resize(len);
        memcpy(job->m_hookBufForEndHook->data(), buf, len);
        return;
    }

    if (job->m_diskCacheItem) // 如果外部设置了数据，则不走disk cache了
        delete job->m_diskCacheItem;

    if (!job->m_asynWkeNetSetData)
        job->m_asynWkeNetSetData = new Vector<char>();
    job->m_asynWkeNetSetData->resize(len);
    memcpy(job->m_asynWkeNetSetData->data(), buf, len);
    
    job->m_isHoldJobToAsynCommit = false;
    job->m_isWkeNetSetDataBeSetted = true;
}

void WKE_CALL_TYPE wkeNetHookRequest(wkeNetJob jobPtr)
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

void WKE_CALL_TYPE wkeNetCancelRequest(wkeNetJob jobPtr)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    job->m_cancelledReason = net::kNormalCancelled;
}

const char* WKE_CALL_TYPE wkeNetGetUrlByJob(wkeNetJob jobPtr)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    blink::KURL kurl = job->firstRequest()->url();

    CString urlString = kurl.getUTF8String().utf8();
    return wke::createTempCharString(urlString.data(), urlString.length());
}

void WKE_CALL_TYPE wkeNetContinueJob(wkeNetJob jobPtr)
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

void WKE_CALL_TYPE wkeNetChangeRequestUrl(wkeNetJob jobPtr, const char* url)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    blink::KURL newUrl(blink::ParsedURLString, url);
    job->m_response.setURL(newUrl);
    job->firstRequest()->setURL(newUrl);
    job->m_initializeHandleInfo->url = url;
    job->m_url = fastStrDup(url);
    //ASSERT(!job->m_url);
}

BOOL WKE_CALL_TYPE wkeNetHoldJobToAsynCommit(wkeNetJob jobPtr)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    if (job->m_isRedirection || job->m_isSynchronous)
        return FALSE;

    job->m_isWkeNetSetDataBeSetted = false;
    if (job->m_asynWkeNetSetData)
        delete job->m_asynWkeNetSetData;
    job->m_asynWkeNetSetData = nullptr;

    if (job->m_hookBufForEndHook)
        delete job->m_hookBufForEndHook;
    job->m_hookBufForEndHook = nullptr;

    job->m_isHookRequest &= (~((unsigned int)1));

    job->m_isHoldJobToAsynCommit = true;

    return TRUE;
}

wkeRequestType WKE_CALL_TYPE wkeNetGetRequestMethod(void *jobPtr)
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

wkePostBodyElements* WKE_CALL_TYPE wkeNetGetPostBody(void* jobPtr)
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

wkePostBodyElements* WKE_CALL_TYPE wkeNetCreatePostBodyElements(wkeWebView webView, size_t length)
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

void WKE_CALL_TYPE wkeNetFreePostBodyElements(wkePostBodyElements* elements)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    for (size_t i = 0; i < elements->elementSize; ++i) {
        wkeNetFreePostBodyElement(elements->element[i]);
    }
    free(elements->element);
    delete elements;
}

wkePostBodyElement* WKE_CALL_TYPE wkeNetCreatePostBodyElement(wkeWebView webView)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    wkePostBodyElement* wkeElement = new wkePostBodyElement();
    wkeElement->size = sizeof(wkePostBodyElement);
    return wkeElement;
}

void WKE_CALL_TYPE wkeNetFreePostBodyElement(wkePostBodyElement* element)
{
    wkeFreeMemBuf(element->data);
    wkeDeleteString(element->filePath);
    delete element;
}

wkeMemBuf* WKE_CALL_TYPE wkeCreateMemBuf(wkeWebView webView, void* buf, size_t length)
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

void WKE_CALL_TYPE wkeFreeMemBuf(wkeMemBuf* buf)
{
    if (!buf)
        return;
    if (buf->data)
        free(buf->data);
    free(buf);
}

int WKE_CALL_TYPE wkeNetGetFavicon(wkeWebView webView, wkeOnNetGetFaviconCallback callback, void* param)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    return net::getFavicon(webView, callback, param);
}

struct wkeWebUrlRequest {
    blink::WebURLRequest m_resourceRequest;

    wkeWebUrlRequest(wkeWebView webView, const blink::WebURLRequest& resourceRequest)
    {
        m_webView = webView;
        if (m_webView)
            m_webviewId = wkeGetWebviewId(webView);
        m_resourceRequest = resourceRequest;
    }

    wkeWebUrlRequest(const utf8* url, const utf8* method, const utf8* mime)
    {
        m_webView = nullptr;
        m_webviewId = 0;

        m_resourceRequest.initialize();

        blink::KURL kurl(blink::ParsedURLString, url);
        m_resourceRequest.setURL(kurl);
        m_resourceRequest.setHTTPMethod(blink::WebString::fromUTF8(method));
    }

    void addHTTPHeaderField(const utf8* name, const utf8* value)
    {
        std::string* nameStr = new std::string(name);
        std::string* valueStr = new std::string(value);
        if (nameStr->size() == 0 || valueStr->size() == 0) {
            delete nameStr;
            delete valueStr;
            return;
        }
        if (WTF::isMainThread()) {
            if (m_webView && 0 == m_webviewId && wke::g_liveWebViews.end() != wke::g_liveWebViews.find(m_webView))
                m_webviewId = wkeGetWebviewId(m_webView);

            if (wkeIsWebviewAlive(m_webviewId))
                m_resourceRequest.addHTTPHeaderField(blink::WebString::fromUTF8(name), blink::WebString::fromUTF8(value));

            delete nameStr;
            delete valueStr;
        } else {
            wkeWebUrlRequest* self = this;
            WTF::internal::callOnMainThreadClosure([self, nameStr, valueStr] {
                self->addHTTPHeaderField(nameStr->c_str(), valueStr->c_str());
                delete nameStr;
                delete valueStr;
            });
        }
    }

    wkeWebView getWebView() const
    {
        return m_webView;
    }

    void setWebView(wkeWebView webView)
    {
        m_webView = webView;
    }

private:
    wkeWebView m_webView;
    int m_webviewId;
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
    NetUrlRequest(wkeWebView webView, wkeWebUrlRequestPtr request, void* param, const wkeUrlRequestCallbacks* callbacks)
    {
        m_ref = 0;
        m_id = 0;
        m_type = net::JobHead::kWkeCustomNetRequest;
        m_webView = webView;
        m_webviewId = 0;
        if (m_webView && WTF::isMainThread())
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
        cancel();
    }

    void createLoader()
    {
        if (wke::g_liveWebViews.end() != wke::g_liveWebViews.find(m_webView))
            m_webviewId = wkeGetWebviewId(m_webView);

        m_loader = blink::Platform::current()->createURLLoader();
        m_loader->loadAsynchronously(m_resourceRequest, this);
    }

    int start()
    {
        if (WTF::isMainThread()) {
            createLoader();
        } else {
            NetUrlRequest* self = this;
            WTF::internal::callOnMainThreadClosure([self] {
                self->createLoader();
            });
        }
        m_id = net::WebURLLoaderManager::sharedInstance()->addLiveJobs(this);
        return m_id;
    }

    void cancelLoader()
    {
        if (!m_loader)
            return;

        m_loader->cancel();
        delete m_loader;
        m_loader = nullptr;
    }

    void cancel()
    {
        if (WTF::isMainThread()) {
            cancelLoader();
        } else {
            NetUrlRequest* self = this;
            WTF::internal::callOnMainThreadClosure([self] {
                self->cancelLoader();
            });
        }

        net::WebURLLoaderManager::sharedInstance()->removeLiveJobs(m_id);
    }

    void setCallback(void* param, const wkeUrlRequestCallbacks* callbacks)
    {
        m_callbackParam = param;
        m_willRedirectCallback = callbacks->willRedirectCallback;
        m_didReceiveResponseCallback = callbacks->didReceiveResponseCallback;
        m_didReceiveDataCallback = callbacks->didReceiveDataCallback;
        m_didFailCallback = callbacks->didFailCallback;
        m_didFinishLoadingCallback = callbacks->didFinishLoadingCallback;
    }

    // WebURLLoaderClient
    virtual void willSendRequest(blink::WebURLLoader*, blink::WebURLRequest& newRequest, const blink::WebURLResponse& redirectResponse) override
    {
        wkeWebUrlRequest oldWebRequest(m_webView, m_resourceRequest);
        wkeWebUrlRequest newWebRequest(m_webView, newRequest);
        wkeWebUrlResponse webRedirectResponse(redirectResponse);
        if (m_willRedirectCallback) {
            m_willRedirectCallback(m_webView, m_callbackParam, &oldWebRequest, &newWebRequest, &webRedirectResponse);
        }
    }

    virtual void didReceiveResponse(blink::WebURLLoader*, const blink::WebURLResponse& response) override
    {
        wkeWebUrlRequest webRequest(m_webView, m_resourceRequest);
        wkeWebUrlResponse webResponse(response);
        if (m_didReceiveResponseCallback)
            m_didReceiveResponseCallback(m_webView, m_callbackParam, &webRequest, &webResponse);
    }

    virtual void didReceiveData(blink::WebURLLoader*, const char* data, int dataLength, int encodedDataLength) override
    {
        wkeWebUrlRequest webRequest(m_webView, m_resourceRequest);
        if (m_didReceiveDataCallback)
            m_didReceiveDataCallback(m_webView, m_callbackParam, &webRequest, data, dataLength);
    }

    virtual void didFail(blink::WebURLLoader* loader, const blink::WebURLError& error) override
    {
        wkeWebUrlRequest webRequest(m_webView, m_resourceRequest);
        if (m_didFailCallback)
            m_didFailCallback(m_webView, m_callbackParam, &webRequest, error.localizedDescription.utf8().c_str());

        delete this;
    }

    virtual void didFinishLoading(blink::WebURLLoader* loader, double finishTime, int64_t totalEncodedDataLength) override
    {
        wkeWebUrlRequest webRequest(m_webView, m_resourceRequest);
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

blinkWebURLRequestPtr WKE_CALL_TYPE wkeNetCopyWebUrlRequest(wkeNetJob jobPtr, bool needExtraData)
{
    net::WebURLLoaderInternal* job = (net::WebURLLoaderInternal*)jobPtr;
    blink::WebURLRequest* request = job->firstRequest();

    blink::WebURLRequest* result = new blink::WebURLRequest();
    result->assign(*request);

    if (!needExtraData)
        result->setExtraData(nullptr);

    return result;
}

void WKE_CALL_TYPE wkeNetDeleteBlinkWebURLRequestPtr(blinkWebURLRequestPtr ptr)
{
    delete ptr;
}

wkeWebUrlRequestPtr WKE_CALL_TYPE wkeNetCreateWebUrlRequest2(const blinkWebURLRequestPtr request)
{
    return new wkeWebUrlRequest(nullptr, *request);
}

wkeWebUrlRequestPtr WKE_CALL_TYPE wkeNetCreateWebUrlRequest(const utf8* url, const utf8* method, const utf8* mime)
{
    return new wkeWebUrlRequest(url, method, mime);
}

void WKE_CALL_TYPE wkeNetAddHTTPHeaderFieldToUrlRequest(wkeWebUrlRequestPtr request, const utf8* name, const utf8* value)
{
    if (WTF::isMainThread())
        return request->addHTTPHeaderField(name, value);

    std::string* nameStr = new std::string(name);
    std::string* valueStr = new std::string(value);
    WTF::internal::callOnMainThreadClosure([request, nameStr, valueStr] {
        request->addHTTPHeaderField(nameStr->c_str(), valueStr->c_str());
        delete valueStr;
        delete nameStr;
    });
}

int WKE_CALL_TYPE wkeNetGetHttpStatusCode(wkeWebUrlResponsePtr response)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    return response->m_response.httpStatusCode();
}

long long WKE_CALL_TYPE wkeNetGetExpectedContentLength(wkeWebUrlResponsePtr response)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    return response->m_response.expectedContentLength();
}

const utf8* WKE_CALL_TYPE wkeNetGetResponseUrl(wkeWebUrlResponsePtr response)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    blink::KURL kurl = response->m_response.url();
    String url = kurl.getUTF8String();
    return wke::createTempCharString((const char*)url.characters8(), url.length());
}

int WKE_CALL_TYPE wkeNetStartUrlRequest(wkeWebView webView, wkeWebUrlRequestPtr request, void* param, const wkeUrlRequestCallbacks* callbacks)
{
    request->setWebView(webView);
    NetUrlRequest* netRequest = new NetUrlRequest(webView, request, param, callbacks);
    return netRequest->start();
}

void WKE_CALL_TYPE wkeNetCancelWebUrlRequest(int requestId)
{
    net::JobHead* jobHead = net::WebURLLoaderManager::sharedInstance()->checkJob(requestId);
    if (!jobHead || net::JobHead::kWkeCustomNetRequest != jobHead->getType())
        return;
    NetUrlRequest* netRequest = (NetUrlRequest*)jobHead;
    netRequest->cancel();
}

namespace wke {

wkePostBodyElements* flattenHTTPBodyElementToWke(const WTF::Vector<net::FlattenHTTPBodyElement*>& body)
{
    if (0 == body.size())
        return nullptr;

    wkePostBodyElements* result = wkeNetCreatePostBodyElements(nullptr, body.size());
    result->isDirty = false;
    for (size_t i = 0; i < result->elementSize; ++i) {
        wkePostBodyElement* wkeElement = wkeNetCreatePostBodyElement(nullptr);
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


#include "config.h"

#include "include/cef_response.h"
#include "libcef/browser/CefSchemeLoaderImpl.h"
#include "libcef/browser/ThreadUtil.h"
#include "libcef/browser/CefRequestContextImpl.h"
#include "libcef/browser/CefBrowserHostImpl.h"
#include "libcef/browser/CefFrameHostImpl.h"
#include "libcef/browser/CefContext.h"
#include "libcef/common/CefResponseImpl.h"
#include "libcef/common/CefRequestImpl.h"
#include "libcef/common/CefTaskImpl.h"
#include "libcef/common/StringUtil.h"
#include "net/WebURLLoaderWinINet.h"
#include "net/RequestExtraData.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/platform/WebThread.h"
#include "third_party/WebKit/public/platform/WebURLLoaderClient.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"
#include "third_party/WebKit/Source/web/WebLocalFrameImpl.h"
#include "third_party/WebKit/Source/wtf/Functional.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "content/browser/WebPage.h"
#include "wke/wke.h"

// Client callback for asynchronous response continuation.
class CefRequestHeadCallback : public CefCallback {
public:
    explicit CefRequestHeadCallback(CefSchemeLoaderImpl* loaderImpl)
        : m_loaderImpl(loaderImpl) {}

    void Continue() override {
        // Continue asynchronously.
        CEF_POST_BLINK_TASK(CEF_UIT, WTF::bind(&CefRequestHeadCallback::ContinueAsynchronously, this));
    }

    void Cancel() override {
        Detach();
    }

    void Detach() {
        CEF_REQUIRE_UIT();
        m_loaderImpl = nullptr;
    }

private:
    void ContinueAsynchronously() {
        CEF_REQUIRE_UIT();
        if (!m_loaderImpl)
            return;

        m_loaderImpl->OnHeadAvailable();
    }

private:
    CefSchemeLoaderImpl* m_loaderImpl;
    IMPLEMENT_REFCOUNTING(CefRequestHeadCallback);
};

class CefRequestDataCallback : public CefCallback {
public:
    explicit CefRequestDataCallback(CefSchemeLoaderImpl* loaderImpl)
        : m_loaderImpl(loaderImpl) {}

    void Continue() override {
        // Continue asynchronously.
        CEF_POST_BLINK_TASK(CEF_UIT, WTF::bind(&CefRequestDataCallback::ContinueAsynchronously, this));
    }

    void Cancel() override {
        Detach();
    }

    void Detach() {
        CEF_REQUIRE_UIT();
        m_loaderImpl = nullptr;
    }

private:
    void ContinueAsynchronously() {
        CEF_REQUIRE_UIT();
        if (!m_loaderImpl)
            return;

        m_loaderImpl->OnDataAvailable();
    }

private:
    CefSchemeLoaderImpl* m_loaderImpl;

    IMPLEMENT_REFCOUNTING(CefRequestDataCallback);
};

//////////////////////////////////////////////////////////////////////////

CefSchemeLoaderImpl::CefSchemeLoaderImpl(blink::WebURLLoader* loader, blink::WebURLLoaderClient* client) {
    m_responseLength = 0;
    m_totalEncodedDataLength = 0;
    m_headAvailable = false;
    m_loader = loader;
    m_client = client;
}

CefSchemeLoaderImpl::~CefSchemeLoaderImpl() {

}

void CefSchemeLoaderImpl::OnLoaderWillBeDelete() {
    m_loader = nullptr;
    m_client = nullptr;
}

void CefSchemeLoaderImpl::OnHeadAvailable() {
    m_headCallback->Detach();

    CefRefPtr<CefResponseImpl> response = new CefResponseImpl();
    m_responseLength = 0;
    m_totalEncodedDataLength = 0;
    CefString redirectUrl;
    m_resourceHandler->GetResponseHeaders(response, m_responseLength, redirectUrl);
    m_client->didReceiveResponse(m_loader, response->GetWebResponseHeaders(), nullptr);

    OnDataAvailable();
}

void CefSchemeLoaderImpl::OnDataAvailable() {
    ASSERT(!m_dataCallback);
    m_dataCallback = new CefRequestDataCallback(this);

    Vector<char> dataToRead;
    dataToRead.resize(m_responseLength);
    int bytesRead = 0;
    bool b = m_resourceHandler->ReadResponse(dataToRead.data(), m_responseLength, bytesRead, m_dataCallback);
    m_totalEncodedDataLength += bytesRead;

    m_client->didReceiveData(m_loader, dataToRead.data(), bytesRead, m_totalEncodedDataLength);
    if (!b) {
        m_dataCallback->Detach();
        m_client->didFinishLoading(m_loader, WTF::currentTime(), m_totalEncodedDataLength);
    }
}

CefSchemeLoaderImpl* CefSchemeLoaderImpl::LoadAsynchronously(
    const blink::WebURLRequest& request,
    blink::WebURLLoaderClient* client,
    blink::WebURLLoader* loader,
    bool& canLoad) {
    canLoad = false;

    if (!CefContext::Get()) {
        ASSERT(wkeIsInitialize());
        return nullptr;
    }

    CefRefPtr<CefRequestContext> requestContext = CefRequestContext::GetGlobalContext();
    if (!requestContext)
        return nullptr;

    blink::KURL url = (blink::KURL)request.url();

    CefRefPtr<CefSchemeHandlerFactory> factory = ((CefRequestContextImpl*)(requestContext.get()))->GetHandlerFactory(request, url.protocol());
    if (!factory)
        return nullptr;

    canLoad = true;

    net::RequestExtraData* extraData = (net::RequestExtraData*)request.extraData();

    CefRefPtr<CefBrowser> browser(extraData->browser);
    CefStringUTF16 frameName;
    cef::WebStringToCefString(extraData->frame->uniqueName(), frameName);

    // TODO delete
    //CefRefPtr<CefFrame> frame = new CefFrameHostImpl(extraData->browser, extraData->frame, frameName);
    CefRefPtr<CefFrame> frame = extraData->browser->GetOrCreateFrame(
        extraData->frame, content::WebPage::kUnspecifiedFrameId, blink::KURL());

    CefString schemeName;
    cef::WTFStringToCefString(url.protocol(), schemeName);

    CefRefPtr<CefRequestImpl> cefRequest(new CefRequestImpl());
    cefRequest->Set(request);

    CefRefPtr<CefResourceHandler> resourceHandler = factory->Create(browser, frame, schemeName, cefRequest);
    if (!resourceHandler) {
        blink::WebURLError error;
        error.domain = blink::WebString(url.string());
        error.localizedDescription = blink::WebString::fromUTF8("CefSchemeHandlerFactory::Create fail\n");
        client->didFail(loader, error);
        return nullptr;
    }

    CefSchemeLoaderImpl* loaderImpl = new CefSchemeLoaderImpl(loader, client);
    loaderImpl->m_resourceHandler = resourceHandler;

    CefRefPtr<CefRequestHeadCallback> headCallback = new CefRequestHeadCallback(loaderImpl);
    loaderImpl->m_headCallback = headCallback;

    bool b = resourceHandler->ProcessRequest(cefRequest, headCallback);
    if (!b) {
        delete loaderImpl;
        headCallback->Detach();
        blink::WebURLError error;
        error.domain = blink::WebString(url.string());
        error.localizedDescription = blink::WebString::fromUTF8("CefSchemeHandlerFactory::Create fail\n");
        client->didFail(loader, error);
        return nullptr;
    }

    return loaderImpl;
}

void CefSchemeLoaderImpl::Cancel() {
    if (m_headCallback)
        m_headCallback->Detach();
    m_headCallback = nullptr;

    if (m_dataCallback)
        m_dataCallback->Detach();
    m_dataCallback = nullptr;
}

void CefSchemeLoaderImpl::SetDefersLoading(bool value) {
    
}

void CefSchemeLoaderImpl::DidChangePriority(blink::WebURLRequest::Priority newPriority, int intraPriorityValue) {

}

bool CefSchemeLoaderImpl::AttachThreadedDataReceiver(blink::WebThreadedDataReceiver* threadedDataReceiver) {
    DebugBreak();
    return false;
}
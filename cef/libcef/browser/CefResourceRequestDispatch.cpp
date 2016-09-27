

// Client callback for asynchronous response continuation.
class CefRequestHeadCallback : public CefCallback {
public:
    explicit CefRequestHeadCallback(CefResourceRequestDispatch* dispatch)
        : m_dispatch(dispatch) {}

    void Continue() override {
        // Continue asynchronously.
        CEF_POST_TASK(CEF_UIT, WTF::Bind(&CefRequestHeadCallback::ContinueAsynchronously, this));
    }

    void Cancel() override {
        Detach();

        // Cancel asynchronously.
        //CEF_POST_TASK(CEF_UIT, WTF::Bind(&CefRequestHeadCallback::CancelAsynchronously, this));
    }

    void Detach() {
        CEF_REQUIRE_UIT();
        m_dispatch = NULL;
    }

private:
    void ContinueAsynchronously() {
        CEF_REQUIRE_UIT();
        if (!m_dispatch)
            return;

        ;
    }

    void CancelAsynchronously() {
        CEF_REQUIRE_UIT();
        if (!m_dispatch)
            return;

//         if (m_dispatch)
//             m_dispatch->Kill();
    }
private:
    CefResourceRequestDispatch* m_dispatch;

    IMPLEMENT_REFCOUNTING(CefRequestHeadCallback);
};

class CefRequestDataCallback : public CefCallback {
public:

    explicit CefRequestDataCallback(CefResourceRequestDispatch* dispatch)
        : m_dispatch(dispatch) {}

    void Continue() override {
        // Continue asynchronously.
        CEF_POST_TASK(CEF_UIT, WTF::Bind(&CefRequestDataCallback::ContinueAsynchronously, this));
    }

    void Cancel() override {
        Detach();
    }

    void Detach() {
        CEF_REQUIRE_UIT();
        m_dispatch = NULL;
    }

private:
    CefResourceRequestDispatch* m_dispatch;

    IMPLEMENT_REFCOUNTING(CefRequestDataCallback);
};

bool CefResourceRequestDispatch::CreateResourceHandlerByRequest(const blink::WebURLRequest& request) {
    blink::KURL url = (blink::KURL)request.url();

    CefRefPtr<CefSchemeHandlerFactory> factory =
        ((CefRequestContextImpl*)CefRequestContext::GetGlobalContext())->GetHandlerFactory(request, url.protocol());
    if (!factory)
        return false;

    RequestExtraData* extraData = (RequestExtraData*)request.extraData();

    CefRefPtr<CefBrowser> browser(extraData->browser);
    ;
    CefStringUTF16 frameName;
    cef::SetWebStringToCefString(extraData->frame->uniqueName(), frameName);

    // TODO delete
    CefRefPtr<CefFrame> frame = new CefFrameHostImpl(extraData->browser, extraData->frame, frameName);

    CefString schemeName;
    cef::SetWTFStringToCefString(url.protocol(), schemeName);

    CefRefPtr<CefRequest> cefRequest(new CefRequestImpl());
    cefRequest->Set(request);

    m_resourceHandler = factory->Create(browser, frame, schemeName, cefRequest);
    if (!m_resourceHandler)
        return false;

    if (!m_headCallback)
        m_headCallback = new CefRequestHeadCallback(this);
    bool b = m_resourceHandler->ProcessRequest(cefRequest, m_headCallback);
    if (!b) {
        m_headCallback->Detach();
    }

    return true;
}

void CefResourceRequestDispatch::OnHeadAvailable() {
    CefRefPtr<CefResponse> response = new CefResponseImpl();
    m_responseLength = 0;
    CefString redirectUrl;
    m_resourceHandler->GetResponseHeaders(response, m_responseLength, redirectUrl);
}

bool CefResourceRequestDispatch::OnDataAvailable() {
    if (!m_dataCallback)
        m_dataCallback = new CefRequestHeadCallback(this);

    Vector<char> dataToRead;
    int bytesRead = 0;
    bool b = m_resourceHandler->ReadResponse(dataToRead.data(), m_responseLength, bytesRead, m_dataCallback);

    ;
}
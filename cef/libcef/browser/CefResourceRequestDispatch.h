#ifndef CefResourceRequestDispatch_h
#define CefResourceRequestDispatch_h

class CefRequestHeadCallback;
class CefRequestDataCallback;

class CefResourceRequestDispatch {
public:
    bool CreateResourceHandlerByRequest(const blink::WebURLRequest& request);

private:
    CefRefPtr<CefResourceHandler> m_resourceHandler;
    CefRefPtr<CefRequestHeadCallback> m_headCallback;
    CefRefPtr<CefRequestDataCallback> m_dataCallback;
    CefRefPtr<CefResponse> m_response;

    int64 m_responseLength;

    bool m_headAvailable;
};

#endif // CefResourceRequest_h
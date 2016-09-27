#ifndef CEF_LIBCEF_COMMON_REQUEST_IMPL_H_
#define CEF_LIBCEF_COMMON_REQUEST_IMPL_H_

#include "include/cef_request.h"

// Implementation of CefPostData
class CefPostDataImpl : public CefPostData {
public:
    CefPostDataImpl();

    bool IsReadOnly() override;
    size_t GetElementCount() override;
    void GetElements(ElementVector& elements) override;
    bool RemoveElement(CefRefPtr<CefPostDataElement> element) override;
    bool AddElement(CefRefPtr<CefPostDataElement> element) override;
    void RemoveElements() override;

    void Set(const blink::WebHTTPBody& data);
    void Get(blink::WebHTTPBody& data);

    void SetReadOnly(bool read_only);

private:
    ElementVector elements_;

    // True if this object is read-only.
    bool m_readOnly;

    WTF::Mutex lock_;

    IMPLEMENT_REFCOUNTING(CefPostDataImpl);
};


// Implementation of CefPostDataElement
class CefPostDataElementImpl : public CefPostDataElement {
public:
    CefPostDataElementImpl();
    ~CefPostDataElementImpl() override;

    bool IsReadOnly() override;
    void SetToEmpty() override;
    void SetToFile(const CefString& fileName) override;
    void SetToBytes(size_t size, const void* bytes) override;
    Type GetType() override;
    CefString GetFile() override;
    size_t GetBytesCount() override;
    size_t GetBytes(size_t size, void* bytes) override;

    void* GetBytes() { return data_.bytes.bytes; }

    void Set(const blink::WebHTTPBody::Element& element);
    void Get(blink::WebHTTPBody::Element& element);

    void SetReadOnly(bool read_only);

private:
    void Cleanup();

    Type type_;
    union {
        struct {
            void* bytes;
            size_t size;
        } bytes;
        cef_string_t filename;
    } data_;

    // True if this object is read-only.
    bool m_readOnly;

    WTF::Mutex lock_;

    IMPLEMENT_REFCOUNTING(CefPostDataElementImpl);
};

// Implementation of CefRequest
class CefRequestImpl : public CefRequest {
public:
    enum Changes {
        kChangedNone = 0,
        kChangedUrl = 1 << 0,
        kChangedMethod = 1 << 1,
        kChangedReferrer = 1 << 2,
        kChangedPostData = 1 << 3,
        kChangedHeaderMap = 1 << 4,
        kChangedFlags = 1 << 5,
        kChangedFirstPartyForCookies = 1 << 6,
    };

    CefRequestImpl();

    bool IsReadOnly() override;
    CefString GetURL() override;
    void SetURL(const CefString& url) override;
    CefString GetMethod() override;
    void SetMethod(const CefString& method) override;
    void SetReferrer(const CefString& referrer_url,
        ReferrerPolicy policy) override;
    CefString GetReferrerURL() override;
    ReferrerPolicy GetReferrerPolicy() override;
    CefRefPtr<CefPostData> GetPostData() override;
    void SetPostData(CefRefPtr<CefPostData> postData) override;
    void GetHeaderMap(HeaderMap& headerMap) override;
    void SetHeaderMap(const HeaderMap& headerMap) override;
    void Set(const CefString& url,
        const CefString& method,
        CefRefPtr<CefPostData> postData,
        const HeaderMap& headerMap) override;
    int GetFlags() override;
    void SetFlags(int flags) override;
    CefString GetFirstPartyForCookies() override;
    void SetFirstPartyForCookies(const CefString& url) override;
    ResourceType GetResourceType() override;
    TransitionType GetTransitionType() override;
    uint64 GetIdentifier() override;

    // Populate this object from the URLRequest object.
    //void Set(net::URLRequest* request);

    // Populate the URLRequest object from this object.
    // If |changed_only| is true then only the changed fields will be updated.
    //void Get(net::URLRequest* request, bool changed_only) const;

    // Populate this object from the NavigationParams object.
    // TODO(cef): Remove the |is_main_frame| argument once NavigationParams is
    // reliable in reporting that value.
    // Called from content_browser_client.cc NavigationOnUIThread().
    //void Set(const navigation_interception::NavigationParams& params, bool is_main_frame);

    // Populate this object from a WebURLRequest object.
    // Called from CefContentRendererClient::HandleNavigation().
    void Set(const blink::WebURLRequest& request);

    // Populate the WebURLRequest object from this object.
    // Called from CefRenderURLRequest::Context::Start().
    void Get(blink::WebURLRequest& request, int64& upload_data_size) const;

    // Populate the WebURLRequest object based on the contents of |params|.
    // Called from CefBrowserImpl::LoadRequest().
    //static void Get(const CefMsg_LoadRequest_Params& params, blink::WebURLRequest& request);

    // Populate the CefNavigateParams object from this object.
    // Called from CefBrowserHostImpl::LoadRequest().
    //void Get(CefNavigateParams& params) const;

    // Populate the URLFetcher object from this object.
    // Called from CefBrowserURLRequest::Context::ContinueOnOriginatingThread().
    //void Get(net::URLFetcher& fetcher, int64& upload_data_size) const;

    void SetReadOnly(bool read_only);

    void SetTrackChanges(bool track_changes);
    uint8_t GetChanges() const;

private:
    void Changed(uint8_t changes);
    bool ShouldSet(uint8_t changes, bool changed_only) const;

    void Reset();

    CefString m_url;
    CefString m_method;
    CefString m_referrerUrl;
    ReferrerPolicy m_referrerPolicy;
    CefRefPtr<CefPostData> m_postdata;
    HeaderMap m_headermap;
    ResourceType m_resourceType;
    TransitionType m_transitionType;
    uint64 m_identifier;

    // The below members are used by CefURLRequest.
    int m_flags;
    CefString m_firstPartyForCookies;

    // True if this object is read-only.
    bool m_readOnly;

    // True if this object should track changes.
    bool m_trackChanges;

    // Bitmask of |Changes| values which indicate which fields have changed.
    uint8_t m_changes;

    mutable WTF::Mutex m_lock;

    IMPLEMENT_REFCOUNTING(CefRequestImpl);
};

#endif // CEF_LIBCEF_COMMON_REQUEST_IMPL_H_
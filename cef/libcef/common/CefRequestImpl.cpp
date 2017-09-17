#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include "third_party/WebKit/public/platform/WebHTTPHeaderVisitor.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "include/base/cef_logging.h"
#include "include/cef_request.h"
#include "libcef/common/CefRequestImpl.h"
#include "libcef/common/StringUtil.h"

#define CHECK_READONLY_RETURN(val) \
  if (m_readOnly) { \
    NOTREACHED() << "object is read only"; \
    return val; \
  }

#define CHECK_READONLY_RETURN_VOID() \
  if (m_readOnly) { \
    NOTREACHED() << "object is read only"; \
    return; \
  }

#define SETBOOLFLAG(obj, flags, method, FLAG) \
    obj.method((flags & (FLAG)) == (FLAG))

const char kReferrerLowerCase[] = "referer";
const char kContentTypeLowerCase[] = "content-type";
const char kApplicationFormURLEncoded[] = "application/x-www-form-urlencoded";

// CefRequest -----------------------------------------------------------------

// static
CefRefPtr<CefRequest> CefRequest::Create() {
    CefRefPtr<CefRequest> request(new CefRequestImpl());
    return request;
}

// Read |request| into |map|. If a Referer value is specified populate
// |referrer|.
class HeaderVisitor : public blink::WebHTTPHeaderVisitor {
public:
    HeaderVisitor(CefRequest::HeaderMap* map, CefString* referrer)
        : map_(map),
        referrer_(referrer) {
    }

    void visitHeader(const blink::WebString& name, const blink::WebString& value) override {
        String nameStr = WTF::ensureUTF16String((String)name);
        String valueStr = WTF::ensureUTF16String((String)value);
        if (!equalIgnoringCase(nameStr, kReferrerLowerCase))
            referrer_->FromString(valueStr.characters16(), valueStr.length(), true);
        else
            map_->insert(std::make_pair(CefString(nameStr.charactersWithNullTermination().data()),
                CefString(valueStr.charactersWithNullTermination().data())));
    }

private:
    CefRequest::HeaderMap* map_;
    CefString* referrer_;
};

void GetHeaderMap(const blink::WebURLRequest& request, CefRequest::HeaderMap& map, CefString& referrer) {
    HeaderVisitor visitor(&map, &referrer);
    request.visitHTTPHeaderFields(&visitor);
}

// Read |source| into |map|.
void GetHeaderMap(const CefRequest::HeaderMap& source, CefRequest::HeaderMap& map) {
    CefRequest::HeaderMap::const_iterator it = source.begin();
    for (; it != source.end(); ++it) {
        const CefString& name = it->first;

        // Do not include Referer in the header map.
        if (!equalIgnoringCase(String(name.ToString().c_str()), kReferrerLowerCase))
            map.insert(std::make_pair(name, it->second));
    }
}

// Read |map| into |request|.
void SetHeaderMap(const CefRequest::HeaderMap& map, blink::WebURLRequest& request) {
    CefRequest::HeaderMap::const_iterator it = map.begin();
    for (; it != map.end(); ++it) {
        blink::WebString key(it->first.c_str(), it->first.size());
        blink::WebString value(it->second.c_str(), it->first.size());
        request.setHTTPHeaderField(key, value);
    }
}


// CefRequestImpl -------------------------------------------------------------

CefRequestImpl::CefRequestImpl()
    : m_readOnly(false),
    m_trackChanges(false) {
    MutexLocker locker(m_lock);
    Reset();
}

bool CefRequestImpl::IsReadOnly() {
    MutexLocker locker(m_lock);
    return m_readOnly;
}

CefString CefRequestImpl::GetURL() {
    MutexLocker locker(m_lock);
    return m_url;
}

void CefRequestImpl::SetURL(const CefString& url) {
    MutexLocker locker(m_lock);
    CHECK_READONLY_RETURN_VOID();
    if (m_url != url) {
        m_url = url;
        Changed(kChangedUrl);
    }
}

CefString CefRequestImpl::GetMethod() {
    MutexLocker locker(m_lock);
    return m_method;
}

void CefRequestImpl::SetMethod(const CefString& method) {
    MutexLocker locker(m_lock);
    CHECK_READONLY_RETURN_VOID();
    if (m_method != method) {
        m_method = method;
        Changed(kChangedMethod);
    }
}

void CefRequestImpl::SetReferrer(const CefString& referrerUrl, ReferrerPolicy policy) {
    MutexLocker locker(m_lock);
    CHECK_READONLY_RETURN_VOID();

    // Call GetAsReferrer here for consistency since the same logic will later be
    // applied by URLRequest::SetReferrer().
    //const GURL& gurl = GURL(referrerUrl.ToString()).GetAsReferrer();
    blink::KURL kurl(blink::ParsedURLString, referrerUrl.ToString().c_str());
    String url = kurl.strippedForUseAsReferrer();
    ASSERT(url.is8Bit());
    if (url != m_referrerUrl.ToString().c_str() || m_referrerPolicy != policy) {
        m_referrerUrl = WTF::ensureUTF16UChar(url, true).data();
        m_referrerPolicy = policy;
        Changed(kChangedReferrer);
    }
}

CefString CefRequestImpl::GetReferrerURL() {
    MutexLocker locker(m_lock);
    return m_referrerUrl;
}

CefRequestImpl::ReferrerPolicy CefRequestImpl::GetReferrerPolicy() {
    MutexLocker locker(m_lock);
    return m_referrerPolicy;
}

CefRefPtr<CefPostData> CefRequestImpl::GetPostData() {
    MutexLocker locker(m_lock);
    return m_postdata;
}

void CefRequestImpl::SetPostData(CefRefPtr<CefPostData> postData) {
    MutexLocker locker(m_lock);
    CHECK_READONLY_RETURN_VOID();
    //m_postdata = postData;
    Changed(kChangedPostData);
    DebugBreak();
}

void CefRequestImpl::GetHeaderMap(HeaderMap& headerMap) {
    MutexLocker locker(m_lock);
    headerMap = m_headermap;
}

void CefRequestImpl::SetHeaderMap(const HeaderMap& headerMap) {
    MutexLocker locker(m_lock);
    CHECK_READONLY_RETURN_VOID();
    ::GetHeaderMap(headerMap, m_headermap);
    Changed(kChangedHeaderMap);
}

void CefRequestImpl::Set(const CefString& url,
    const CefString& method,
    CefRefPtr<CefPostData> postData,
    const HeaderMap& headerMap) {
    MutexLocker locker(m_lock);
    CHECK_READONLY_RETURN_VOID();
    if (m_url != url) {
        m_url = url;
        Changed(kChangedUrl);
    }
    if (m_method != method) {
        m_method = method;
        Changed(kChangedMethod);
    }
    m_postdata = postData;
    ::GetHeaderMap(headerMap, m_headermap);
    Changed(kChangedPostData | kChangedHeaderMap);
}

int CefRequestImpl::GetFlags() {
    MutexLocker locker(m_lock);
    return m_flags;
}

void CefRequestImpl::SetFlags(int flags) {
    MutexLocker locker(m_lock);
    CHECK_READONLY_RETURN_VOID();
    if (m_flags != flags) {
        m_flags = flags;
        Changed(kChangedFlags);
    }
}

CefString CefRequestImpl::GetFirstPartyForCookies() {
    MutexLocker locker(m_lock);
    return m_firstPartyForCookies;
}

void CefRequestImpl::SetFirstPartyForCookies(const CefString& url) {
    MutexLocker locker(m_lock);
    CHECK_READONLY_RETURN_VOID();
    if (m_firstPartyForCookies != url) {
        m_firstPartyForCookies = url;
        Changed(kChangedFirstPartyForCookies);
    }
}

CefRequestImpl::ResourceType CefRequestImpl::GetResourceType() {
    MutexLocker locker(m_lock);
    return m_resourceType;
}

CefRequestImpl::TransitionType CefRequestImpl::GetTransitionType() {
    MutexLocker locker(m_lock);
    return m_transitionType;
}

uint64 CefRequestImpl::GetIdentifier() {
    MutexLocker locker(m_lock);
    return m_identifier;
}

CefString WebStringToCefString(const blink::WebString& in) {
    CefString out;
    Vector<UChar> temp = WTF::ensureUTF16UChar(in, false);
    out.FromString(temp.data(), temp.size(), true);
    return out;
}

void CefRequestImpl::Set(const blink::WebURLRequest& request) {
    DCHECK(!request.isNull());

    MutexLocker locker(m_lock);
    CHECK_READONLY_RETURN_VOID();

    Reset();

    m_url = WebStringToCefString(request.url().string());
    m_method = WebStringToCefString(request.httpMethod());

    ::GetHeaderMap(request, m_headermap, m_referrerUrl);
    m_referrerPolicy = static_cast<cef_referrer_policy_t>(request.referrerPolicy());

    const blink::WebHTTPBody& body = request.httpBody();
    if (!body.isNull()) {
//         m_postdata = new CefPostDataImpl();
//         static_cast<CefPostDataImpl*>(m_postdata.get())->Set(body);
    }

    m_firstPartyForCookies = WebStringToCefString(request.firstPartyForCookies().string());

    if (request.cachePolicy() == blink::WebURLRequest::ReloadIgnoringCacheData)
        m_flags |= UR_FLAG_SKIP_CACHE;
    if (request.allowStoredCredentials())
        m_flags |= UR_FLAG_ALLOW_CACHED_CREDENTIALS;
    if (request.reportUploadProgress())
        m_flags |= UR_FLAG_REPORT_UPLOAD_PROGRESS;
}

void CefRequestImpl::Get(blink::WebURLRequest& request,
    int64& upload_data_size) const {
    request.initialize();
    MutexLocker locker(m_lock);

    request.setURL(blink::KURL(blink::ParsedURLString, m_url.ToString().c_str()));
    request.setHTTPMethod(blink::WebString::fromUTF8(m_method.ToString()));

    if (!m_referrerUrl.empty()) {
        DebugBreak();
//         const blink::WebString& referrer =
//             blink::WebSecurityPolicy::generateReferrerHeader(
//             static_cast<blink::WebReferrerPolicy>(m_referrerPolicy),
//             KURL(blink::ParsedURLString, m_url.ToString().c_str()),
//             blink::WebString::fromUTF8(m_referrerUrl));
//         if (!referrer.isEmpty()) {
//             request.setHTTPReferrer(referrer, static_cast<blink::WebReferrerPolicy>(m_referrerPolicy));
//         }
    }

    if (m_postdata.get()) {
        DebugBreak();
//         blink::WebHTTPBody body;
//         body.initialize();
//         static_cast<CefPostDataImpl*>(m_postdata.get())->Get(body);
//         request.setHTTPBody(body);
// 
//         if (m_flags & UR_FLAG_REPORT_UPLOAD_PROGRESS) {
//             // Attempt to determine the upload data size.
//             CefPostData::ElementVector elements;
//             m_postdata->GetElements(elements);
//             if (elements.size() == 1 && elements[0]->GetType() == PDE_TYPE_BYTES) {
//                 CefPostDataElementImpl* impl =
//                     static_cast<CefPostDataElementImpl*>(elements[0].get());
//                 upload_data_size = impl->GetBytesCount();
//             }
//         }
    }

    ::SetHeaderMap(m_headermap, request);

    if (!m_firstPartyForCookies.empty()) {
        blink::WebURL url = blink::KURL(blink::ParsedURLString, m_firstPartyForCookies.ToString().c_str());
        request.setFirstPartyForCookies(url);
    }

    request.setCachePolicy((m_flags & UR_FLAG_SKIP_CACHE) ?
        blink::WebURLRequest::ReloadIgnoringCacheData :
        blink::WebURLRequest::UseProtocolCachePolicy);

    SETBOOLFLAG(request, m_flags, setAllowStoredCredentials, UR_FLAG_ALLOW_CACHED_CREDENTIALS);
    SETBOOLFLAG(request, m_flags, setReportUploadProgress, UR_FLAG_REPORT_UPLOAD_PROGRESS);
}

void CefRequestImpl::SetReadOnly(bool read_only) {
    MutexLocker locker(m_lock);
    if (m_readOnly == read_only)
        return;

    m_readOnly = read_only;
    if (m_postdata.get())
        static_cast<CefPostDataImpl*>(m_postdata.get())->SetReadOnly(read_only);
}

void CefRequestImpl::SetTrackChanges(bool track_changes) {
    MutexLocker locker(m_lock);
    if (m_trackChanges == track_changes)
        return;

    m_trackChanges = track_changes;
    m_changes = kChangedNone;
    if (m_postdata.get()) {
        //static_cast<CefPostDataImpl*>(m_postdata.get())->SetTrackChanges(track_changes);
        DebugBreak();
    }
}

uint8_t CefRequestImpl::GetChanges() const {
    MutexLocker locker(m_lock);
    uint8_t changes = m_changes;
    DebugBreak();
//     if (m_postdata.get() && static_cast<CefPostDataImpl*>(m_postdata.get())->HasChanges()) {
//         changes |= kChangedPostData;
//     }
    return changes;
}

void CefRequestImpl::Changed(uint8_t changes) {
    //m_lock.AssertAcquired();
#if ENABLE(ASSERT)
    DCHECK(m_lock.locked());
#endif
    if (m_trackChanges)
        m_changes |= changes;
}

bool CefRequestImpl::ShouldSet(uint8_t changes, bool changed_only) const {
//     m_lock.AssertAcquired();
// 
//     // Always change if changes are not being tracked.
//     if (!m_trackChanges)
//         return true;
// 
//     // Always change if changed-only was not requested.
//     if (!changed_only)
//         return true;
// 
//     // Change if the |changes| bit flag has been set.
//     if ((m_changes & changes) == changes)
//         return true;
// 
//     if ((changes & kChangedPostData) == kChangedPostData) {
//         // Change if the post data object was modified directly.
//         if (m_postdata.get() &&
//             static_cast<CefPostDataImpl*>(m_postdata.get())->HasChanges()) {
//             return true;
//         }
//     }
    DebugBreak();
    return false;
}

void CefRequestImpl::Reset() {
    //m_lock.AssertAcquired();
    DCHECK(!m_readOnly);

    m_url.clear();
    m_method = "GET";
    m_referrerUrl.clear();
    m_referrerPolicy = REFERRER_POLICY_DEFAULT;
    m_postdata = NULL;
    m_headermap.clear();
    m_resourceType = RT_SUB_RESOURCE;
    m_transitionType = TT_EXPLICIT;
    m_identifier = 0U;
    m_flags = UR_FLAG_NONE;
    m_firstPartyForCookies.clear();

    m_changes = kChangedNone;
}

// CefPostDataImpl ------------------------------------------------------------

CefPostDataImpl::CefPostDataImpl()
    : m_readOnly(false) {
}

bool CefPostDataImpl::IsReadOnly() {
    MutexLocker lock_scope(lock_);
    return m_readOnly;
}

size_t CefPostDataImpl::GetElementCount() {
    MutexLocker lock_scope(lock_);
    return elements_.size();
}

void CefPostDataImpl::GetElements(ElementVector& elements) {
    MutexLocker lock_scope(lock_);
    elements = elements_;
}

bool CefPostDataImpl::RemoveElement(CefRefPtr<CefPostDataElement> element) {
    MutexLocker lock_scope(lock_);
    CHECK_READONLY_RETURN(false);

    ElementVector::iterator it = elements_.begin();
    for (; it != elements_.end(); ++it) {
        if (it->get() == element.get()) {
            elements_.erase(it);
            return true;
        }
    }

    return false;
}

bool CefPostDataImpl::AddElement(CefRefPtr<CefPostDataElement> element) {
    bool found = false;

    MutexLocker lock_scope(lock_);
    CHECK_READONLY_RETURN(false);

    // check that the element isn't already in the list before adding
    ElementVector::const_iterator it = elements_.begin();
    for (; it != elements_.end(); ++it) {
        if (it->get() == element.get()) {
            found = true;
            break;
        }
    }

    if (!found)
        elements_.push_back(element);

    return !found;
}

void CefPostDataImpl::RemoveElements() {
    MutexLocker lock_scope(lock_);
    CHECK_READONLY_RETURN_VOID();
    elements_.clear();
}

void CefPostDataImpl::Set(const blink::WebHTTPBody& data) {
    {
        MutexLocker lock_scope(lock_);
        CHECK_READONLY_RETURN_VOID();
    }

    CefRefPtr<CefPostDataElement> postelem;
    blink::WebHTTPBody::Element element;
    size_t size = data.elementCount();
    for (size_t i = 0; i < size; ++i) {
        if (data.elementAt(i, element)) {
            postelem = CefPostDataElement::Create();
            static_cast<CefPostDataElementImpl*>(postelem.get())->Set(element);
            AddElement(postelem);
        }
    }
}

void CefPostDataImpl::Get(blink::WebHTTPBody& data) {
    MutexLocker lock_scope(lock_);

    blink::WebHTTPBody::Element element;
    ElementVector::iterator it = elements_.begin();
    for (; it != elements_.end(); ++it) {
        static_cast<CefPostDataElementImpl*>(it->get())->Get(element);
        if (element.type == blink::WebHTTPBody::Element::TypeData) {
            data.appendData(element.data);
        }
        else if (element.type == blink::WebHTTPBody::Element::TypeFile) {
            data.appendFile(element.filePath);
        }
        else {
            NOTREACHED();
        }
    }
}

void CefPostDataImpl::SetReadOnly(bool read_only) {
    MutexLocker lock_scope(lock_);
    if (m_readOnly == read_only)
        return;

    m_readOnly = read_only;

    ElementVector::const_iterator it = elements_.begin();
    for (; it != elements_.end(); ++it) {
        static_cast<CefPostDataElementImpl*>(it->get())->SetReadOnly(read_only);
    }
}

// CefPostDataElementImpl -----------------------------------------------------

CefRefPtr<CefPostDataElement> CefPostDataElement::Create() {
	CefRefPtr<CefPostDataElement> element(new CefPostDataElementImpl());
	return element;
}

CefPostDataElementImpl::CefPostDataElementImpl()
    : type_(PDE_TYPE_EMPTY),
    m_readOnly(false) {
    memset(&data_, 0, sizeof(data_));
}

CefPostDataElementImpl::~CefPostDataElementImpl() {
    Cleanup();
}

bool CefPostDataElementImpl::IsReadOnly() {
    MutexLocker lock_scope(lock_);
    return m_readOnly;
}

void CefPostDataElementImpl::SetToEmpty() {
    MutexLocker lock_scope(lock_);
    CHECK_READONLY_RETURN_VOID();

    Cleanup();
}

void CefPostDataElementImpl::SetToFile(const CefString& fileName) {
    MutexLocker lock_scope(lock_);
    CHECK_READONLY_RETURN_VOID();

    // Clear any data currently in the element
    Cleanup();

    // Assign the new data
    type_ = PDE_TYPE_FILE;
    cef_string_copy(fileName.c_str(), fileName.length(), &data_.filename);
}

void CefPostDataElementImpl::SetToBytes(size_t size, const void* bytes) {
    MutexLocker lock_scope(lock_);
    CHECK_READONLY_RETURN_VOID();

    // Clear any data currently in the element
    Cleanup();

    // Assign the new data
    void* data = malloc(size);
    DCHECK(data != NULL);
    if (data == NULL)
        return;

    memcpy(data, bytes, size);

    type_ = PDE_TYPE_BYTES;
    data_.bytes.bytes = data;
    data_.bytes.size = size;
}

CefPostDataElement::Type CefPostDataElementImpl::GetType() {
    MutexLocker lock_scope(lock_);
    return type_;
}

CefString CefPostDataElementImpl::GetFile() {
    MutexLocker lock_scope(lock_);
    DCHECK(type_ == PDE_TYPE_FILE);
    CefString filename;
    if (type_ == PDE_TYPE_FILE)
        filename.FromString(data_.filename.str, data_.filename.length, false);
    return filename;
}

size_t CefPostDataElementImpl::GetBytesCount() {
    MutexLocker lock_scope(lock_);
    DCHECK(type_ == PDE_TYPE_BYTES);
    size_t size = 0;
    if (type_ == PDE_TYPE_BYTES)
        size = data_.bytes.size;
    return size;
}

size_t CefPostDataElementImpl::GetBytes(size_t size, void* bytes) {
    MutexLocker lock_scope(lock_);
    DCHECK(type_ == PDE_TYPE_BYTES);
    size_t rv = 0;
    if (type_ == PDE_TYPE_BYTES) {
        rv = (size < data_.bytes.size ? size : data_.bytes.size);
        memcpy(bytes, data_.bytes.bytes, rv);
    }
    return rv;
}

void CefPostDataElementImpl::Set(const blink::WebHTTPBody::Element& element) {
    {
        MutexLocker lock_scope(lock_);
        CHECK_READONLY_RETURN_VOID();
    }

    if (element.type == blink::WebHTTPBody::Element::TypeData) {
        SetToBytes(element.data.size(),
            static_cast<const void*>(element.data.data()));
    } else if (element.type == blink::WebHTTPBody::Element::TypeFile) {
        CefString filePath;
        cef::WebStringToCefString(element.filePath, filePath);
        SetToFile(filePath);
    } else {
        NOTREACHED();
    }
}

void CefPostDataElementImpl::Get(blink::WebHTTPBody::Element& element) {
    MutexLocker lock_scope(lock_);

    if (type_ == PDE_TYPE_BYTES) {
        element.type = blink::WebHTTPBody::Element::TypeData;
        element.data.assign(
            static_cast<char*>(data_.bytes.bytes), data_.bytes.size);
    }
    else if (type_ == PDE_TYPE_FILE) {
        element.type = blink::WebHTTPBody::Element::TypeFile;
        CefString filename(&data_.filename);
        element.filePath.assign(filename.c_str(), filename.length());
    }
    else {
        NOTREACHED();
    }
}

void CefPostDataElementImpl::SetReadOnly(bool read_only) {
    MutexLocker lock_scope(lock_);
    if (m_readOnly == read_only)
        return;

    m_readOnly = read_only;
}

void CefPostDataElementImpl::Cleanup() {
    if (type_ == PDE_TYPE_EMPTY)
        return;

    if (type_ == PDE_TYPE_BYTES)
        free(data_.bytes.bytes);
    else if (type_ == PDE_TYPE_FILE)
        cef_string_clear(&data_.filename);
    type_ = PDE_TYPE_EMPTY;
    memset(&data_, 0, sizeof(data_));
}
#endif

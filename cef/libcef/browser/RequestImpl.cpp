
#include "include/capi/cef_request_capi.h"
#include "libcef/common/CommonBase.h"
#include "cef/libcef/browser/RequestImpl.h"
#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include "third_party/WebKit/public/platform/WebURL.h"


namespace cef {

RequestImpl::RequestImpl()
{
    memset(&m_baseClass, 0, sizeof(m_baseClass));

    m_baseClass.is_read_only = is_read_only;
    m_baseClass.get_url = get_url;
    m_baseClass.set_url = set_url;
    m_baseClass.get_method = get_method;
    m_baseClass.set_method = set_method;
    m_baseClass.get_post_data = get_post_data;
    m_baseClass.set_post_data = set_post_data;
    m_baseClass.get_header_map = get_header_map;
    m_baseClass.set_header_map = set_header_map;
    m_baseClass.set = set;
    m_baseClass.get_flags = get_flags;
    m_baseClass.set_flags = set_flags;
    m_baseClass.get_first_party_for_cookies = get_first_party_for_cookies;
    m_baseClass.set_first_party_for_cookies = set_first_party_for_cookies;
    m_baseClass.get_resource_type = get_resource_type;
    m_baseClass.get_transition_type = get_transition_type;
    m_baseClass.get_identifier = get_identifier;

    CefCppBase<cef_request_t, RequestImpl>::init(this);

    m_request = new blink::WebURLRequest();
}

void RequestImpl::willDeleted()
{
    delete m_request;
}

cef_string_userfree_t RequestImpl::get_url(cef_request_t* s)
{
    CEF_CHECK_ARGS_CAST(RequestImpl, nullptr);

    cef_string_utf16_t* cefurl = cef_string_userfree_utf16_alloc();

    Vector<UChar> url = ((String)(self->m_request->url().string())).charactersWithNullTermination();
    cef_string_utf16_set(url.data(), url.size(), cefurl, true);

    return cefurl;
}

void RequestImpl::set_url(cef_request_t* self, const cef_string_t* url) { DebugBreak(); return; }

} // cef

#include "include/capi/cef_browser_capi.h"

#include "libcef/common/CommonBase.h"

namespace blink {
class WebURLRequest;
}

namespace cef {

class RequestImpl : public CefCppBase < cef_request_t, RequestImpl > {
public:
    RequestImpl();
    void willDeleted();

    static int CEF_CALLBACK is_read_only(cef_request_t* self) { DebugBreak(); return 0; }

    static cef_string_userfree_t CEF_CALLBACK get_url(cef_request_t* self);

    static void CEF_CALLBACK set_url(cef_request_t* self, const cef_string_t* url);

    static cef_string_userfree_t CEF_CALLBACK get_method(cef_request_t* self) { DebugBreak(); return 0; }

    static void CEF_CALLBACK set_method(cef_request_t* self, const cef_string_t* method) { DebugBreak(); return; }

    static cef_post_data_t* CEF_CALLBACK get_post_data(cef_request_t* self) { DebugBreak(); return 0; }

    static void CEF_CALLBACK set_post_data(cef_request_t* self, cef_post_data_t* postData) { DebugBreak(); return; }

    static void CEF_CALLBACK get_header_map(cef_request_t* self, cef_string_multimap_t headerMap) { DebugBreak(); return; }

    static void CEF_CALLBACK set_header_map(cef_request_t* self, cef_string_multimap_t headerMap) { DebugBreak(); return; }

    static void CEF_CALLBACK set(cef_request_t* self, const cef_string_t* url,
        const cef_string_t* method, cef_post_data_t* postData,
        cef_string_multimap_t headerMap) {
        DebugBreak(); return;
    }

    static int CEF_CALLBACK get_flags(cef_request_t* self) { DebugBreak(); return 0; }

    static void CEF_CALLBACK set_flags(cef_request_t* self, int flags) { DebugBreak(); return; }

    static cef_string_userfree_t CEF_CALLBACK get_first_party_for_cookies(cef_request_t* self) { DebugBreak(); return 0; }

    static void CEF_CALLBACK set_first_party_for_cookies(cef_request_t* self, const cef_string_t* url) { DebugBreak(); return; }

    static cef_resource_type_t CEF_CALLBACK get_resource_type(cef_request_t* self) { DebugBreak(); return RT_MAIN_FRAME; }

    static cef_transition_type_t CEF_CALLBACK get_transition_type(cef_request_t* self) { DebugBreak(); return TT_RELOAD; }

    static uint64 CEF_CALLBACK get_identifier(cef_request_t* self) {
        DebugBreak(); return 0;
    }

    blink::WebURLRequest* m_request;
};

} // cef
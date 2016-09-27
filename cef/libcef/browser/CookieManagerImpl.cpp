
#include "include/capi/cef_cookie_capi.h"
#include "cef/libcef/common/CommonBase.h"

namespace cef {
     
class CookieManagerImpl : public CefCppBase<cef_cookie_manager_t, CookieManagerImpl> {
public:
    CookieManagerImpl();
    void willDeleted() { DebugBreak(); }

    static void CEF_CALLBACK setSupportedSchemes(cef_cookie_manager_t* self,
        cef_string_list_t schemes
#ifdef _CEF_2454_VERSION_
        , cef_completion_callback_t* callback
#endif
        )
    {
        return;
    }

    static int CEF_CALLBACK visitAllCookies(cef_cookie_manager_t* self, cef_cookie_visitor_t* visitor)
    {
        DebugBreak(); return 0;
    }

    static int CEF_CALLBACK visitUrlCookies(cef_cookie_manager_t* self,
        const cef_string_t* url, int includeHttpOnly,
        struct _cef_cookie_visitor_t* visitor)
    {
        DebugBreak(); return 0;
    }

    static int CEF_CALLBACK setCookie(cef_cookie_manager_t* self,
        const cef_string_t* url, const struct _cef_cookie_t* cookie
#ifdef _CEF_2454_VERSION_
        , cef_set_cookie_callback_t* callback
#endif
        )
    {
        DebugBreak(); return 0;
    }

    static int CEF_CALLBACK deleteCookies(cef_cookie_manager_t* self,
        const cef_string_t* url, const cef_string_t* cookie_name
#ifdef _CEF_2454_VERSION_
        , cef_delete_cookies_callback_t* callback
#endif
        )
    {
        DebugBreak(); return 0;
    }

    static int CEF_CALLBACK setStoragePath(cef_cookie_manager_t* self,
        const cef_string_t* path, int persist_session_cookies
#ifdef _CEF_2454_VERSION_
        , cef_completion_callback_t* callback
#endif
            )
    {
        DebugBreak(); return 0;
    }

    static int CEF_CALLBACK flushStore(cef_cookie_manager_t* self,
        struct _cef_completion_callback_t* callback)
    {
        DebugBreak(); return 0;
    }
};

CookieManagerImpl::CookieManagerImpl()
{
    memset(&m_baseClass, 0, sizeof(m_baseClass));
    m_baseClass.set_supported_schemes = setSupportedSchemes;
    m_baseClass.visit_all_cookies = visitAllCookies;
    m_baseClass.visit_url_cookies = visitUrlCookies;
    m_baseClass.set_cookie = setCookie;
    m_baseClass.delete_cookies = deleteCookies;
    m_baseClass.set_storage_path = setStoragePath;
    m_baseClass.flush_store = flushStore;
    CefCppBase<cef_cookie_manager_t, CookieManagerImpl>::init(this);
}

} // cef

CEF_EXPORT cef_cookie_manager_t* cef_cookie_manager_get_global_manager(
#ifdef _CEF_2454_VERSION_
    cef_completion_callback_t* callback
#endif
    )
{
    static cef::CookieManagerImpl* self = nullptr;
    if (!self) {
        self = new cef::CookieManagerImpl();
        self->ref();
    }

    return (cef_cookie_manager_t*)(self);
}

CEF_EXPORT cef_cookie_manager_t* cef_cookie_manager_create_manager(
    const cef_string_t* path, int persist_session_cookies
#ifdef _CEF_2454_VERSION_
    , cef_completion_callback_t* callback
#endif
    )
{
    return (cef_cookie_manager_t*)(new cef::CookieManagerImpl());
}

#include "include/capi/cef_request_context_capi.h"
#include "cef/libcef/common/CommonBase.h"
#include "include/capi/cef_scheme_capi.h"

namespace cef {

class RequestContextImpl : public CefCppBase < cef_request_context_t, RequestContextImpl > {
public:
    RequestContextImpl();
    void willDeleted() { DebugBreak(); }

    static int CEF_CALLBACK isSame(cef_request_context_t* self, cef_request_context_t* other)
    {
        return 0;
    }

    static int CEF_CALLBACK isSharingWith(cef_request_context_t* self, cef_request_context_t* other)
    {
        DebugBreak(); return 0;
    }

    static int CEF_CALLBACK isGlobal(cef_request_context_t* self)
    {
        DebugBreak(); return 0;
    }

    static cef_request_context_handler_t* CEF_CALLBACK getHandler(cef_request_context_t* self) { DebugBreak(); return nullptr; }

    static cef_string_userfree_t CEF_CALLBACK getCachePath(cef_request_context_t* self) { DebugBreak(); return nullptr; }

    static cef_cookie_manager_t* CEF_CALLBACK getCefaultCookieManager(cef_request_context_t* self,
        cef_completion_callback_t* callback) {
        DebugBreak(); return nullptr;
    }

    static int CEF_CALLBACK registerSchemeHandlerFactory(
        cef_request_context_t* self, const cef_string_t* scheme_name,
        const cef_string_t* domain_name,
        cef_scheme_handler_factory_t* factory) { DebugBreak(); return 0; }

    static int CEF_CALLBACK clearSchemeHandlerFactories(cef_request_context_t* self) { DebugBreak(); return 0; }

    static void CEF_CALLBACK purgePluginListCache(cef_request_context_t* self, int reload_pages) { DebugBreak(); return; }

    static int CEF_CALLBACK hasPreference(cef_request_context_t* self,
        const cef_string_t* name) {
        DebugBreak(); return 0;
    }
#ifdef _CEF_2454_VERSION_
    static cef_value_t* CEF_CALLBACK getPreference(cef_request_context_t* self, const cef_string_t* name) { DebugBreak(); return nullptr; }

    static cef_dictionary_value_t* CEF_CALLBACK getAllPreferences(cef_request_context_t* self, int include_defaults) { DebugBreak(); return nullptr; }

    static int CEF_CALLBACK canSetPreference(cef_request_context_t* self, const cef_string_t* name) { DebugBreak(); return 0; }

    static int CEF_CALLBACK setPreference(cef_request_context_t* self,
        const cef_string_t* name, cef_value_t* value,
        cef_string_t* error) {
        DebugBreak(); return 0;
    }
#endif
};

RequestContextImpl::RequestContextImpl()
{
    memset(&m_baseClass, 0, sizeof(m_baseClass));
    m_baseClass.is_same = isSame;
#ifdef _CEF_2454_VERSION_
    m_baseClass.is_sharing_with = isSharingWith;
#endif
    m_baseClass.is_global = isGlobal;

    m_baseClass.get_handler = getHandler;
#ifdef _CEF_2454_VERSION_
    m_baseClass.get_cache_path = getCachePath;
    m_baseClass.get_default_cookie_manager = getCefaultCookieManager;
    m_baseClass.register_scheme_handler_factory = registerSchemeHandlerFactory;
    m_baseClass.clear_scheme_handler_factories = clearSchemeHandlerFactories;
    m_baseClass.purge_plugin_list_cache = purgePluginListCache;
    m_baseClass.has_preference = hasPreference;
    m_baseClass.get_preference = getPreference;
    m_baseClass.get_all_preferences = getAllPreferences;

    m_baseClass.can_set_preference = canSetPreference;
    m_baseClass.set_preference = setPreference;
#endif
    CefCppBase<cef_request_context_t, RequestContextImpl>::init(this);
}

} // cef

CEF_EXPORT cef_request_context_t* cef_request_context_get_global_context()
{
    static cef::RequestContextImpl* self = nullptr;
    if (!self) {
        self = new cef::RequestContextImpl();
        ((cef_base_t*)self)->add_ref((cef_base_t*)self);
    }

    return (cef_request_context_t*)(self);
}

CEF_EXPORT cef_request_context_t* cef_request_context_create_context(
#ifdef _CEF_2454_VERSION_
    const cef_request_context_settings_t* settings, 
#endif
    cef_request_context_handler_t* handler)
{
    cef::RequestContextImpl* self = new cef::RequestContextImpl();
    self->ref();

    return (cef_request_context_t*)(self);
}

///
// Creates a new context object that shares storage with |other| and uses an
// optional |handler|.
///
CEF_EXPORT cef_request_context_t* create_context_shared(cef_request_context_t* other, struct _cef_request_context_handler_t* handler)
{
    return (cef_request_context_t*)(new cef::RequestContextImpl());
}
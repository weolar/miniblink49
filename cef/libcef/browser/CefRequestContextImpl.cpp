// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "libcef/browser/CefRequestContextImpl.h"
#include "libcef/browser/CefContext.h"
#include "libcef/browser/CefCookieManagerImpl.h"
#include "libcef/browser/ThreadUtil.h"
#include "libcef/common/CefTaskImpl.h"
#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/FastMalloc.h" // Test
#include "third_party/WebKit/public/platform/WebTraceLocation.h"

namespace {

bool IsStandardScheme(const std::string& scheme) {
    if (0 == scheme.length())
        return false;
    return true;
}

std::string ToLower(const std::string& str) {
    std::string str_lower = str;
    std::transform(str_lower.begin(), str_lower.end(), str_lower.begin(), towlower);
    return str;
}

}

// CefBrowserContext

// static
CefRefPtr<CefRequestContext> CefRequestContext::GetGlobalContext() {
    // Verify that the context is in a valid state.
    if (!CONTEXT_STATE_VALID()) {
        NOTREACHED() << "context not valid";
        return NULL;
    }
    static CefRequestContext* s_requestContext = nullptr;
    if (!s_requestContext) {
        CefRequestContextSettings settings;
        s_requestContext = new CefRequestContextImpl(settings, nullptr);
        s_requestContext->AddRef();
    }
    return s_requestContext;
}

// static
CefRefPtr<CefRequestContext> CefRequestContext::CreateContext(
    const CefRequestContextSettings& settings,
    CefRefPtr<CefRequestContextHandler> handler) {
    // Verify that the context is in a valid state.
    if (!CONTEXT_STATE_VALID()) {
        NOTREACHED() << "context not valid";
        return NULL;
    }

    return new CefRequestContextImpl(settings, handler);
}

// static
CefRefPtr<CefRequestContext> CefRequestContext::CreateContext(
    CefRefPtr<CefRequestContext> other,
    CefRefPtr<CefRequestContextHandler> handler) {
    // Verify that the context is in a valid state.
    if (!CONTEXT_STATE_VALID()) {
        NOTREACHED() << "context not valid";
        return NULL;
    }

    if (!other.get())
        return NULL;

    return new CefRequestContextImpl(static_cast<CefRequestContextImpl*>(other.get()), handler);
}

// CefBrowserContextImpl

CefRequestContextImpl::CefRequestContextImpl(
    const CefRequestContextSettings& settings,
    CefRefPtr<CefRequestContextHandler> handler)
    : settings_(settings)
//     handler_(handler),
//     unique_id_(g_next_id.GetNext()),
//     request_context_impl_(NULL) 
{
}

CefRequestContextImpl::CefRequestContextImpl(
    CefRefPtr<CefRequestContextImpl> other,
    CefRefPtr<CefRequestContextHandler> handler)
//     : other_(other),
//     handler_(handler),
//     unique_id_(g_next_id.GetNext()),
//     request_context_impl_(NULL) 
{
}


CefRequestContextImpl::~CefRequestContextImpl() {

}

bool CefRequestContextImpl::IsSame(CefRefPtr<CefRequestContext> other) {
    CefRequestContextImpl* other_impl =
        static_cast<CefRequestContextImpl*>(other.get());
    if (!other_impl)
        return false;

    DebugBreak();
    return false;
    // Compare CefBrowserContext pointers if one has been associated.
//     if (browser_context_ && other_impl->browser_context_)
//         return (browser_context_ == other_impl->browser_context_);
//     else if (browser_context_ || other_impl->browser_context_)
//         return false;
// 
//     // Otherwise compare unique IDs.
//     return (unique_id_ == other_impl->unique_id_);
}

bool CefRequestContextImpl::IsSharingWith(CefRefPtr<CefRequestContext> other) {
    CefRequestContextImpl* other_impl =
        static_cast<CefRequestContextImpl*>(other.get());
    if (!other_impl)
        return false;

    if (IsSame(other))
        return true;

//     CefRefPtr<CefRequestContext> pending_other = other_;
//     if (pending_other.get()) {
//         // This object is not initialized but we know what context this object will
//         // share with. Compare to that other context instead.
//         return pending_other->IsSharingWith(other);
//     }
// 
//     pending_other = other_impl->other_;
//     if (pending_other.get()) {
//         // The other object is not initialized but we know what context that object
//         // will share with. Compare to that other context instead.
//         return pending_other->IsSharingWith(this);
//     }
// 
//     if (request_context_impl_ && other_impl->request_context_impl_) {
//         // Both objects are initialized. Compare the request context objects.
//         return (request_context_impl_ == other_impl->request_context_impl_);
//     }
// 
//     // This or the other object is not initialized. Compare the cache path values.
//     // If both are non-empty and the same then they'll share the same storage.
//     if (settings_.cache_path.length > 0 &&
//         other_impl->settings_.cache_path.length > 0) {
//         return (base::FilePath(CefString(&settings_.cache_path)) ==
//             base::FilePath(CefString(&other_impl->settings_.cache_path)));
//     }
    DebugBreak();
    return false;
}

bool CefRequestContextImpl::IsGlobal() {
    //return (browser_context_ == CefContentBrowserClient::Get()->browser_context());
    DebugBreak();
    return false;
}

CefRefPtr<CefRequestContextHandler> CefRequestContextImpl::GetHandler() {
    //return handler_;
    DebugBreak();
    return nullptr;
}

CefString CefRequestContextImpl::GetCachePath() {
    return CefString(&settings_.cache_path);
}

CefRefPtr<CefCookieManager> CefRequestContextImpl::GetDefaultCookieManager(CefRefPtr<CefCompletionCallback> callback) {
     CefRefPtr<CefCookieManagerImpl> cookie_manager = new CefCookieManagerImpl();
    cookie_manager->Initialize(this, CefString(), false, callback);
    return cookie_manager.get();
}

void CefRequestContextImpl::RemoveFactory(const std::string& scheme, const std::string& domain) {
    if (!CEF_CURRENTLY_ON_UIT()) {
        CEF_POST_BLINK_TASK(CEF_UIT, WTF::bind(&CefRequestContextImpl::RemoveFactory, this, scheme, domain));
        return;
    }

    std::string scheme_lower = ToLower(scheme);
    std::string domain_lower = ToLower(domain);

    // Hostname is only supported for standard schemes.
    if (!IsStandardScheme(scheme_lower))
        domain_lower = "";

    HandlerMap::iterator iter = m_handlerMap.find(make_pair(scheme_lower, domain_lower));
    if (iter != m_handlerMap.end()) {
        m_handlerMap.erase(iter);
        //SetProtocolHandlerIfNecessary(scheme_lower, false);
    }
}

void CefRequestContextImpl::RegisterSchemeHandlerFactoryImpl(
    const CefString& scheme,
    const CefString& domain,
    CefRefPtr<CefSchemeHandlerFactory> factory) {
    std::string scheme_lower = ToLower(scheme);
    std::string domain_lower = ToLower(domain);

    // Hostname is only supported for standard schemes.
    if (!IsStandardScheme(scheme_lower))
        domain_lower.resize(0);

    //SetProtocolHandlerIfNecessary(scheme_lower, true);

    m_handlerMap[make_pair(scheme_lower, domain_lower)] = factory;
}

bool CefRequestContextImpl::RegisterSchemeHandlerFactory(
    const CefString& scheme,
    const CefString& domain,
    CefRefPtr<CefSchemeHandlerFactory> factory) {
    if (!factory.get()) {
        RemoveFactory(scheme, domain);
        return true;
    }

    if (!CEF_CURRENTLY_ON_UIT()) {
        CEF_POST_BLINK_TASK(CEF_UIT, WTF::bind(&CefRequestContextImpl::RegisterSchemeHandlerFactoryImpl, this, scheme, domain, factory));
        return true;
    }

    RegisterSchemeHandlerFactoryImpl(scheme, domain, factory);

    return true;
}

bool CefRequestContextImpl::ClearSchemeHandlerFactories() {
    m_handlerMap.clear();
    return true;
}

CefRefPtr<CefSchemeHandlerFactory> CefRequestContextImpl::GetHandlerFactory(const blink::WebURLRequest& request, const WTF::String& schemeValue) {
    CEF_REQUIRE_UIT();

    CefRefPtr<CefSchemeHandlerFactory> factory;
    const std::string scheme = schemeValue.utf8().data();
    if (request.url().isValid() && IsStandardScheme(scheme)) {
        // Check for a match with a domain first.
        const std::string& domain = ((blink::KURL)(request.url())).host().utf8().data();

        HandlerMap::iterator i = m_handlerMap.find(make_pair(scheme, domain));
        if (i != m_handlerMap.end())
            factory = i->second;
    }

    if (!factory.get()) {
        // Check for a match with no specified domain.
        HandlerMap::iterator i =
            m_handlerMap.find(make_pair(scheme, std::string()));
        if (i != m_handlerMap.end())
            factory = i->second;
    }

    return factory;
}

void CefRequestContextImpl::PurgePluginListCache(bool reload_pages) {
//     GetBrowserContext(
//         BrowserThread::GetMessageLoopProxyForThread(BrowserThread::UI),
//         base::Bind(&CefRequestContextImpl::PurgePluginListCacheInternal,
//         this, reload_pages));
}

bool CefRequestContextImpl::HasPreference(const CefString& name) {
    // Verify that this method is being called on the UI thread.
    if (!CEF_CURRENTLY_ON_UIT()) {
        NOTREACHED() << "called on invalid thread";
        return false;
    }

    // Make sure the browser context exists.
//     EnsureBrowserContext();
// 
//     PrefService* pref_service = browser_context_->GetPrefs();
//     return (pref_service->FindPreference(name) != NULL);
    return false;
}

CefRefPtr<CefValue> CefRequestContextImpl::GetPreference(const CefString& name) {
    // Verify that this method is being called on the UI thread.
    if (!CEF_CURRENTLY_ON_UIT()) {
        NOTREACHED() << "called on invalid thread";
        return NULL;
    }

    // Make sure the browser context exists.
//     EnsureBrowserContext();
// 
//     PrefService* pref_service = browser_context_->GetPrefs();
//     const PrefService::Preference* pref = pref_service->FindPreference(name);
//     if (!pref)
//         return NULL;
//     return new CefValueImpl(pref->GetValue()->DeepCopy());
    return nullptr;
}


CefRefPtr<CefDictionaryValue> CefRequestContextImpl::GetAllPreferences(bool include_defaults) {
    // Verify that this method is being called on the UI thread.
    if (!CEF_CURRENTLY_ON_UIT()) {
        NOTREACHED() << "called on invalid thread";
        return NULL;
    }

    // Make sure the browser context exists.
//     EnsureBrowserContext();
// 
//     PrefService* pref_service = browser_context_->GetPrefs();
// 
//     scoped_ptr<base::DictionaryValue> values;
//     if (include_defaults)
//         values = pref_service->GetPreferenceValues();
//     else
//         values = pref_service->GetPreferenceValuesOmitDefaults();
// 
//     // CefDictionaryValueImpl takes ownership of |values|.
//     return new CefDictionaryValueImpl(values.release(), true, false);
    return nullptr;
}

bool CefRequestContextImpl::CanSetPreference(const CefString& name) {
    // Verify that this method is being called on the UI thread.
    if (!CEF_CURRENTLY_ON_UIT()) {
        NOTREACHED() << "called on invalid thread";
        return false;
    }

    // Make sure the browser context exists.
//     EnsureBrowserContext();
// 
//     PrefService* pref_service = browser_context_->GetPrefs();
//     const PrefService::Preference* pref = pref_service->FindPreference(name);
//     return (pref && pref->IsUserModifiable());
    return false;
}

bool CefRequestContextImpl::SetPreference(const CefString& name, CefRefPtr<CefValue> value, CefString& error) {
    // Verify that this method is being called on the UI thread.
    if (!CEF_CURRENTLY_ON_UIT()) {
        NOTREACHED() << "called on invalid thread";
        return false;
    }

    // Make sure the browser context exists.
//     EnsureBrowserContext();
// 
//     PrefService* pref_service = browser_context_->GetPrefs();
// 
//     // The below validation logic should match PrefService::SetUserPrefValue.
// 
//     const PrefService::Preference* pref = pref_service->FindPreference(name);
//     if (!pref) {
//         error = "Trying to modify an unregistered preference";
//         return false;
//     }
// 
//     if (!pref->IsUserModifiable()) {
//         error = "Trying to modify a preference that is not user modifiable";
//         return false;
//     }
// 
//     if (!value.get()) {
//         // Reset the preference to its default value.
//         pref_service->ClearPref(name);
//         return true;
//     }
// 
//     if (!value->IsValid()) {
//         error = "A valid value is required";
//         return false;
//     }
// 
//     CefValueImpl* impl = static_cast<CefValueImpl*>(value.get());
// 
//     CefValueImpl::ScopedLockedValue scoped_locked_value(impl);
//     base::Value* impl_value = impl->GetValueUnsafe();
// 
//     if (pref->GetType() != impl_value->GetType()) {
//         error = base::StringPrintf(
//             "Trying to set a preference of type %s to value of type %s",
//             GetTypeString(pref->GetType()), GetTypeString(impl_value->GetType()));
//         return false;
//     }
// 
//     // PrefService will make a DeepCopy of |impl_value|.
//     pref_service->Set(name, *impl_value);
    return true;
}

void CefRequestContextImpl::ClearCertificateExceptions(CefRefPtr<CefCompletionCallback> callback) {
    ;
}

void CefRequestContextImpl::CloseAllConnections(CefRefPtr<CefCompletionCallback> callback) {
    ;
}

void CefRequestContextImpl::ResolveHost(const CefString& origin, CefRefPtr<CefResolveCallback> callback) {
    ;
}

cef_errorcode_t CefRequestContextImpl::ResolveHostCached(const CefString& origin, std::vector<CefString>& resolved_ips) {
    return ERR_NONE;
}
// 
// CefRequestContextImpl::CefRequestContextImpl(
//     scoped_refptr<CefBrowserContext> browser_context)
//     : browser_context_(browser_context),
//     settings_(browser_context->GetSettings()),
//     handler_(browser_context->GetHandler()),
//     unique_id_(0),
//     request_context_impl_(NULL) {
// }
// 
// CefRequestContextImpl::CefRequestContextImpl(
//     const CefRequestContextSettings& settings,
//     CefRefPtr<CefRequestContextHandler> handler)
//     : settings_(settings),
//     handler_(handler),
//     unique_id_(g_next_id.GetNext()),
//     request_context_impl_(NULL) {
// }
// 
// CefRequestContextImpl::CefRequestContextImpl(
//     CefRefPtr<CefRequestContextImpl> other,
//     CefRefPtr<CefRequestContextHandler> handler)
//     : other_(other),
//     handler_(handler),
//     unique_id_(g_next_id.GetNext()),
//     request_context_impl_(NULL) {
// }
// 
// void CefRequestContextImpl::EnsureBrowserContext() {
//     GetBrowserContext();
//     DCHECK(browser_context_.get());
//     DCHECK(request_context_impl_);
// }
// 
// void CefRequestContextImpl::GetBrowserContextOnUIThread(
//     scoped_refptr<base::SingleThreadTaskRunner> task_runner,
//     const BrowserContextCallback& callback) {
//     if (!CEF_CURRENTLY_ON_UIT()) {
//         CEF_POST_TASK(CEF_UIT,
//             base::Bind(&CefRequestContextImpl::GetBrowserContextOnUIThread,
//             this, task_runner, callback));
//         return;
//     }
// 
//     // Make sure the browser context exists.
//     EnsureBrowserContext();
// 
//     if (task_runner->BelongsToCurrentThread()) {
//         // Execute the callback immediately.
//         callback.Run(browser_context_);
//     }
//     else {
//         // Execute the callback on the target thread.
//         task_runner->PostTask(FROM_HERE, base::Bind(callback, browser_context_));
//     }
// }
// 
// void CefRequestContextImpl::GetRequestContextImplOnIOThread(
//     scoped_refptr<base::SingleThreadTaskRunner> task_runner,
//     const RequestContextCallback& callback,
//     scoped_refptr<CefBrowserContext> browser_context) {
//     if (!CEF_CURRENTLY_ON_IOT()) {
//         CEF_POST_TASK(CEF_IOT,
//             base::Bind(&CefRequestContextImpl::GetRequestContextImplOnIOThread,
//             this, task_runner, callback, browser_context));
//         return;
//     }
// 
//     DCHECK(request_context_impl_);
// 
//     // Make sure the request context exists.
//     request_context_impl_->GetURLRequestContext();
// 
//     if (task_runner->BelongsToCurrentThread()) {
//         // Execute the callback immediately.
//         callback.Run(request_context_impl_);
//     }
//     else {
//         // Execute the callback on the target thread.
//         task_runner->PostTask(FROM_HERE,
//             base::Bind(callback, make_scoped_refptr(request_context_impl_)));
//     }
// }
// 
// void CefRequestContextImpl::RegisterSchemeHandlerFactoryInternal(
//     const CefString& scheme_name,
//     const CefString& domain_name,
//     CefRefPtr<CefSchemeHandlerFactory> factory,
//     scoped_refptr<CefURLRequestContextGetterImpl> request_context) {
//     CEF_REQUIRE_IOT();
//     request_context->request_manager()->AddFactory(scheme_name, domain_name,
//         factory);
// }
// 
// void CefRequestContextImpl::ClearSchemeHandlerFactoriesInternal(
//     scoped_refptr<CefURLRequestContextGetterImpl> request_context) {
//     CEF_REQUIRE_IOT();
//     request_context->request_manager()->ClearFactories();
// }
// 
// void CefRequestContextImpl::PurgePluginListCacheInternal(
//     bool reload_pages,
//     scoped_refptr<CefBrowserContext> browser_context) {
//     CEF_REQUIRE_UIT();
//     content::PluginService::GetInstance()->PurgePluginListCache(
//         browser_context.get(), false);
// }
#endif

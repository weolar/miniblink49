// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_REQUEST_CONTEXT_IMPL_H_
#define CEF_LIBCEF_REQUEST_CONTEXT_IMPL_H_
#pragma once

#include "include/cef_request_context.h"
#include <map>

namespace blink {
class WebURLRequest;
}

namespace WTF {
class String;
}

// Implementation of the CefRequestContext interface. All methods are thread-
// safe unless otherwise indicated.
class CefRequestContextImpl : public CefRequestContext {
public:
    ~CefRequestContextImpl() override;

    bool IsSame(CefRefPtr<CefRequestContext> other) override;
    bool IsSharingWith(CefRefPtr<CefRequestContext> other) override;
    bool IsGlobal() override;
    CefRefPtr<CefRequestContextHandler> GetHandler() override;
    CefString GetCachePath() override;
    CefRefPtr<CefCookieManager> GetDefaultCookieManager(
        CefRefPtr<CefCompletionCallback> callback) override;
    bool RegisterSchemeHandlerFactory(
        const CefString& scheme_name,
        const CefString& domain_name,
        CefRefPtr<CefSchemeHandlerFactory> factory) override;    
    bool ClearSchemeHandlerFactories() override;
    void PurgePluginListCache(bool reload_pages) override;
    bool HasPreference(const CefString& name) override;
    CefRefPtr<CefValue> GetPreference(const CefString& name) override;
    CefRefPtr<CefDictionaryValue> GetAllPreferences(
        bool include_defaults) override;
    bool CanSetPreference(const CefString& name) override;
    bool SetPreference(const CefString& name,
        CefRefPtr<CefValue> value,
        CefString& error) override;

    void ClearCertificateExceptions(CefRefPtr<CefCompletionCallback> callback) override;
    void CloseAllConnections(CefRefPtr<CefCompletionCallback> callback) override;
    void ResolveHost(const CefString& origin, CefRefPtr<CefResolveCallback> callback) override;
    cef_errorcode_t ResolveHostCached(const CefString& origin, std::vector<CefString>& resolved_ips);

    const CefRequestContextSettings& settings() const { return settings_; }

    CefRequestContextImpl(const CefRequestContextSettings& settings, CefRefPtr<CefRequestContextHandler> handler);
    CefRequestContextImpl(CefRefPtr<CefRequestContextImpl> other, CefRefPtr<CefRequestContextHandler> handler);

    CefRefPtr<CefSchemeHandlerFactory> GetHandlerFactory(const blink::WebURLRequest& request, const WTF::String& schemeValue);
    void RemoveFactory(const std::string& scheme, const std::string& domain);

private:
    void RegisterSchemeHandlerFactoryImpl(
        const CefString& scheme_name,
        const CefString& domain_name,
        CefRefPtr<CefSchemeHandlerFactory> factory);

    CefRequestContextSettings settings_;
    IMPLEMENT_REFCOUNTING(CefRequestContextImpl);
    DISALLOW_COPY_AND_ASSIGN(CefRequestContextImpl);

    // Map (scheme, domain) to factories.
    typedef std::map<std::pair<std::string, std::string>, CefRefPtr<CefSchemeHandlerFactory> > HandlerMap;
    HandlerMap m_handlerMap;
};

#endif  // CEF_LIBCEF_REQUEST_CONTEXT_IMPL_H_

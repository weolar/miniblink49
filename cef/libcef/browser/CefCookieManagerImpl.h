// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_COOKIE_MANAGER_IMPL_H_
#define CEF_LIBCEF_BROWSER_COOKIE_MANAGER_IMPL_H_

#include <set>

#include "include/cef_cookie.h"
#include "libcef/browser/CefRequestContextImpl.h"

// Implementation of the CefCookieManager interface.
class CefCookieManagerImpl : public CefCookieManager {
public:
    CefCookieManagerImpl();
    ~CefCookieManagerImpl() override;

    // Must be called immediately after this object is created.
    void Initialize(CefRefPtr<CefRequestContextImpl> request_context,
        const CefString& path,
        bool persist_session_cookies,
        CefRefPtr<CefCompletionCallback> callback);

    // Executes |callback| either synchronously or asynchronously with the cookie
    // monster object when it's available. If |task_runner| is NULL the callback
    // will be executed on the originating thread. The resulting cookie monster
    // object can only be accessed on the IO thread.
    //   typedef base::Callback<void(scoped_refptr<net::CookieMonster>)>
    //       CookieMonsterCallback;
    //   void GetCookieMonster(
    //       scoped_refptr<base::SingleThreadTaskRunner> task_runner,
    //       const CookieMonsterCallback& callback);
    // 
    //   // Returns the existing cookie monster object. Logs an error if the cookie
    //   // monster does not yet exist. Must be called on the IO thread.
    //   scoped_refptr<net::CookieMonster> GetExistingCookieMonster();

    // CefCookieManager methods.
    void SetSupportedSchemes(const std::vector<CefString>& schemes,
        CefRefPtr<CefCompletionCallback> callback) override;
    bool VisitAllCookies(CefRefPtr<CefCookieVisitor> visitor) override;
    bool VisitUrlCookies(const CefString& url, bool includeHttpOnly,
        CefRefPtr<CefCookieVisitor> visitor) override;
    bool SetCookie(const CefString& url,
        const CefCookie& cookie,
        CefRefPtr<CefSetCookieCallback> callback) override;
    bool DeleteCookies(const CefString& url,
        const CefString& cookie_name,
        CefRefPtr<CefDeleteCookiesCallback> callback) override;
    bool SetStoragePath(const CefString& path,
        bool persist_session_cookies,
        CefRefPtr<CefCompletionCallback> callback) override;
    bool FlushStore(CefRefPtr<CefCompletionCallback> callback) override;

    //   static bool GetCefCookie(const net::CanonicalCookie& cc, CefCookie& cookie);
    //   static bool GetCefCookie(const GURL& url, const std::string& cookie_line,
    //                            CefCookie& cookie);
    // 
    //   // Set the schemes supported by |cookie_monster|. Default schemes will always
    //   // be supported.
    //   static void SetCookieMonsterSchemes(net::CookieMonster* cookie_monster,
    //                                       const std::set<std::string>& schemes);

private:
    //   // Returns true if a context is or will be available.
    //   bool HasContext();
    // 
    //   // Execute |method| on the IO thread once the request context is available.
    //   void RunMethodWithContext(
    //       const CefRequestContextImpl::RequestContextCallback& method);
    // 
    //   void InitWithContext(
    //       CefRefPtr<CefCompletionCallback> callback,
    //       scoped_refptr<CefURLRequestContextGetterImpl> request_context);
    //   void SetStoragePathWithContext(
    //       const CefString& path,
    //       bool persist_session_cookies,
    //       CefRefPtr<CefCompletionCallback> callback,
    //       scoped_refptr<CefURLRequestContextGetterImpl> request_context);
    //   void SetSupportedSchemesWithContext(
    //       const std::set<std::string>& schemes,
    //       CefRefPtr<CefCompletionCallback> callback,
    //       scoped_refptr<CefURLRequestContextGetterImpl> request_context);
    //   void GetCookieMonsterWithContext(
    //       scoped_refptr<base::SingleThreadTaskRunner> task_runner,
    //       const CookieMonsterCallback& callback,
    //       scoped_refptr<CefURLRequestContextGetterImpl> request_context);
    // 
    //   void SetSupportedSchemesInternal(
    //       const std::set<std::string>& schemes,
    //       CefRefPtr<CefCompletionCallback> callback);
    //   void VisitAllCookiesInternal(
    //       CefRefPtr<CefCookieVisitor> visitor,
    //       scoped_refptr<net::CookieMonster> cookie_monster);
    //   void VisitUrlCookiesInternal(
    //       const CefString& url,
    //       bool includeHttpOnly,
    //       CefRefPtr<CefCookieVisitor> visitor,
    //       scoped_refptr<net::CookieMonster> cookie_monster);
    //   void SetCookieInternal(
    //       const GURL& url,
    //       const CefCookie& cookie,
    //       CefRefPtr<CefSetCookieCallback> callback,
    //       scoped_refptr<net::CookieMonster> cookie_monster);
    //   void DeleteCookiesInternal(
    //       const GURL& url,
    //       const CefString& cookie_name,
    //       CefRefPtr<CefDeleteCookiesCallback> callback,
    //       scoped_refptr<net::CookieMonster> cookie_monster);
    //   void FlushStoreInternal(
    //       CefRefPtr<CefCompletionCallback> callback,
    //       scoped_refptr<net::CookieMonster> cookie_monster);
    // 
    //   // Used for cookie monsters owned by the context.
    //   CefRefPtr<CefRequestContextImpl> request_context_;
    //   scoped_refptr<CefURLRequestContextGetterImpl> request_context_impl_;
    // 
    //   // Used for cookie monsters owned by this object.
    //   base::FilePath storage_path_;
    //   std::set<std::string> supported_schemes_;
    //   scoped_refptr<net::CookieMonster> cookie_monster_;

    //IMPLEMENT_REFCOUNTING(CefCookieManagerImpl);
public:
    void AddRef() const OVERRIDE;
    bool Release() const OVERRIDE;
    bool HasOneRef() const OVERRIDE;
private:
    CefRefCount ref_count_;
};

#endif  // CEF_LIBCEF_BROWSER_COOKIE_MANAGER_IMPL_H_

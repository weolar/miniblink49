// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "libcef/browser/CefCookieManagerImpl.h"

#include <set>
#include <string>
#include <vector>

#include "libcef/browser/CefContext.h"
#include "libcef/browser/ThreadUtil.h"

#include "content/web_impl_win/WebCookieJarCurlImpl.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/public/platform/WebURL.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/Source/wtf/Functional.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/FastMalloc.h"
#include "third_party/libcurl/include/curl/curl.h"

namespace {

// Callback class for visiting cookies.
// class VisitCookiesCallback : public base::RefCounted<VisitCookiesCallback> {
//  public:
//   explicit VisitCookiesCallback(
//       scoped_refptr<net::CookieMonster> cookie_monster,
//       CefRefPtr<CefCookieVisitor> visitor)
//     : cookie_monster_(cookie_monster),
//       visitor_(visitor) {
//   }
// 
//   void Run(const net::CookieList& list) {
//     CEF_REQUIRE_IOT();
// 
//     int total = list.size(), count = 0;
// 
//     net::CookieList::const_iterator it = list.begin();
//     for (; it != list.end(); ++it, ++count) {
//       CefCookie cookie;
//       const net::CanonicalCookie& cc = *(it);
//       CefCookieManagerImpl::GetCefCookie(cc, cookie);
// 
//       bool deleteCookie = false;
//       bool keepLooping = visitor_->Visit(cookie, count, total, deleteCookie);
//       if (deleteCookie) {
//         cookie_monster_->DeleteCanonicalCookieAsync(cc,
//             net::CookieMonster::DeleteCookieCallback());
//       }
//       if (!keepLooping)
//         break;
//     }
//   }
// 
//  private:
//   friend class base::RefCounted<VisitCookiesCallback>;
// 
//   ~VisitCookiesCallback() {}
// 
//   scoped_refptr<net::CookieMonster> cookie_monster_;
//   CefRefPtr<CefCookieVisitor> visitor_;
// };
// 
// 
// // Methods extracted from net/cookies/cookie_monster.cc
// 
// // Determine the cookie domain to use for setting the specified cookie.
// bool GetCookieDomain(const GURL& url,
//                      const net::ParsedCookie& pc,
//                      std::string* result) {
//   std::string domain_string;
//   if (pc.HasDomain())
//     domain_string = pc.Domain();
//   return net::cookie_util::GetCookieDomainWithString(url, domain_string,
//       result);
// }
// 
// // Always execute the callback asynchronously.
// void RunAsyncCompletionOnIOThread(CefRefPtr<CefCompletionCallback> callback) {
//   if (!callback.get())
//     return;
//   CEF_POST_TASK(CEF_IOT,
//       base::Bind(&CefCompletionCallback::OnComplete, callback.get()));
// }
// 
// // Always execute the callback asynchronously.
// void DeleteCookiesCallbackImpl(CefRefPtr<CefDeleteCookiesCallback> callback,
//                                int num_deleted) {
//   if (!callback.get())
//     return;
//   CEF_POST_TASK(CEF_IOT,
//       base::Bind(&CefDeleteCookiesCallback::OnComplete, callback.get(),
//                  num_deleted));
// }
// 
// // Always execute the callback asynchronously.
// void SetCookieCallbackImpl(CefRefPtr<CefSetCookieCallback> callback,
//                            bool success) {
//   if (!callback.get())
//     return;
//   CEF_POST_TASK(CEF_IOT,
//       base::Bind(&CefSetCookieCallback::OnComplete, callback.get(), success));
// }

}  // namespace


CefCookieManagerImpl::CefCookieManagerImpl() {
    WTF::String out = WTF::String::format("CefCookieManagerImpl: %p\n", this);
    OutputDebugStringW(out.charactersWithNullTermination().data());
}

CefCookieManagerImpl::~CefCookieManagerImpl() {
}

void CefCookieManagerImpl::Initialize(
    CefRefPtr<CefRequestContextImpl> request_context,
    const CefString& path,
    bool persist_session_cookies,
    CefRefPtr<CefCompletionCallback> callback) {
//   if (request_context.get()) {
//     request_context_ = request_context;
//     request_context_->GetRequestContextImpl(
//         BrowserThread::GetMessageLoopProxyForThread(BrowserThread::IO),
//         base::Bind(&CefCookieManagerImpl::InitWithContext, this, callback));
//   } else {
//     SetStoragePath(path, persist_session_cookies, callback);
//   }
}

// void CefCookieManagerImpl::GetCookieMonster(
//     scoped_refptr<base::SingleThreadTaskRunner> task_runner,
//     const CookieMonsterCallback& callback) {
//   if (!task_runner.get())
//     task_runner = base::MessageLoop::current()->task_runner();
// 
//   if (!CEF_CURRENTLY_ON_IOT()) {
//     CEF_POST_TASK(CEF_IOT,
//         base::Bind(&CefCookieManagerImpl::GetCookieMonster, this, task_runner,
//                    callback));
//     return;
//   }
// 
//   if (HasContext()) {
//     RunMethodWithContext(
//         base::Bind(&CefCookieManagerImpl::GetCookieMonsterWithContext, this,
//                    task_runner, callback));
//     return;
//   }
// 
//   DCHECK(cookie_monster_.get());
//   if (cookie_monster_.get()) {
//     if (task_runner->BelongsToCurrentThread()) {
//       // Execute the callback immediately.
//       callback.Run(cookie_monster_);
//     } else {
//       // Execute the callback on the target thread.
//       task_runner->PostTask(FROM_HERE, base::Bind(callback, cookie_monster_));
//     }
//     return;
//   }
// }
// 
// scoped_refptr<net::CookieMonster>
// CefCookieManagerImpl::GetExistingCookieMonster() {
//   CEF_REQUIRE_IOT();
//   if (cookie_monster_.get()) {
//     return cookie_monster_;
//   } else if (request_context_impl_.get()) {
//     scoped_refptr<net::CookieMonster> cookie_monster =
//         request_context_impl_->GetCookieMonster();
//     DCHECK(cookie_monster.get());
//     return cookie_monster;
//   }
// 
//   LOG(ERROR) << "Cookie manager backing store does not exist yet";
//   return NULL;
// }

void CefCookieManagerImpl::SetSupportedSchemes(
    const std::vector<CefString>& schemes,
    CefRefPtr<CefCompletionCallback> callback) {
//   if (!CEF_CURRENTLY_ON_IOT()) {
//     CEF_POST_TASK(CEF_IOT,
//         base::Bind(&CefCookieManagerImpl::SetSupportedSchemes, this, schemes,
//                    callback));
//     return;
//   }
// 
//   std::set<std::string> scheme_set;
//   std::vector<CefString>::const_iterator it = schemes.begin();
//   for (; it != schemes.end(); ++it)
//     scheme_set.insert(*it);
// 
//   SetSupportedSchemesInternal(scheme_set, callback);
}

class CefCookieVisitorImpl {
public:
    CefCookieVisitorImpl(CefCookieVisitor* cefVisitor, const CefString* url, bool includeHttpOnly) {
        if (url)
            m_url = *url;
        m_visitor = cefVisitor;
        m_includeHttpOnly = includeHttpOnly;
    }

    struct Item {
        std::string name;
        std::string value;
        std::string domain;
        std::string path;
        int secure;
        int httponly;
        int* expires;

        ~Item() {
            if (expires)
                delete expires;
        }
    };

    static bool Visitor(void* params, const char* name, const char* value, const char* domain, const char* path, int secure, int httponly, int* expires) {
        CefCookieVisitorImpl* self = (CefCookieVisitorImpl*)params;

        if (!self->m_url.empty() && -1 == std::string(domain).find(self->m_url))
            return false;
            
        if (!self->m_includeHttpOnly && httponly)           
            return false;

        Item* item = new Item();
        item->name = name;
        item->value = value;
        item->domain = domain;
        item->path = path;
        item->secure = secure;
        item->httponly = httponly;
        item->expires = nullptr;
        if (expires) {
            item->expires = new int();
            *item->expires = *expires;
        }
        self->m_items.push_back(item);

        return false;
    }

    void PushToVisitor() {
        for (size_t i = 0; i < m_items.size(); ++i) {
            Item* it = m_items[i];
            bool deleteCookie = false;
            CefCookie cookie;

            CefString(&cookie.name).FromString(it->name);
            CefString(&cookie.value).FromString(it->value);
            CefString(&cookie.domain).FromString(it->domain);
            CefString(&cookie.path).FromString(it->path);
            cookie.secure = it->secure;
            cookie.httponly = it->httponly;
            cookie.has_expires = 0;

            m_visitor->Visit(cookie, i, m_items.size(), deleteCookie);

            delete it;
        }
    }

private:
    CefCookieVisitor* m_visitor;
    std::vector<Item*> m_items;
    std::string m_url;
    bool m_includeHttpOnly;
};

bool CefCookieManagerImpl::VisitAllCookies(CefRefPtr<CefCookieVisitor> visitor) {
//   GetCookieMonster(
//       BrowserThread::GetMessageLoopProxyForThread(BrowserThread::IO),
//       base::Bind(&CefCookieManagerImpl::VisitAllCookiesInternal, this,
//                  visitor));
    CefCookieVisitorImpl visitorImpl(visitor.get(), nullptr, true);
    content::WebCookieJarImpl::visitAllCookie(&visitorImpl, &CefCookieVisitorImpl::Visitor);
    visitorImpl.PushToVisitor();

    return true;
}

bool CefCookieManagerImpl::VisitUrlCookies(const CefString& url, bool includeHttpOnly, CefRefPtr<CefCookieVisitor> visitor) {
//   GetCookieMonster(
//       BrowserThread::GetMessageLoopProxyForThread(BrowserThread::IO),
//       base::Bind(&CefCookieManagerImpl::VisitUrlCookiesInternal, this, url,
//                  includeHttpOnly, visitor));

    CefCookieVisitorImpl visitorImpl(visitor.get(), &url, includeHttpOnly);
    content::WebCookieJarImpl::visitAllCookie(&visitorImpl, &CefCookieVisitorImpl::Visitor);
    visitorImpl.PushToVisitor();

    return true;
}

std::string CefCookieToString(const CefCookie& cookie)
{
    std::string name = CefString(&cookie.name).ToString();
    std::string value = CefString(&cookie.value).ToString();
    std::string domain = CefString(&cookie.domain).ToString();
    std::string path = CefString(&cookie.path).ToString();

//     base::Time expiration_time;
//     if (cookie.has_expires)
//         cef_time_to_basetime(cookie.expires, expiration_time);

    //Set-cookie: DisPend=none;expires=Monday, 13-Jun-1988 03:04:55 GMT; domain=.fidelity.com; path=/; secure
    std::string result;
    result += name;
    result += "=";
    result += value;
    result += ";";

    result += " domain=";
    result += domain;
    result += ";";

    result += " path=";
    result += path;
    result += ";";

    if (cookie.secure)
        result += " secure";
    return result;
}

static void CefOnSetCookieCallback(CefSetCookieCallback* callback)
{
    callback->OnComplete(true);
    callback->Release();
}

static void CefOnDelCookieCallback(CefDeleteCookiesCallback* callback)
{
    callback->OnComplete(1);
    callback->Release();
}

bool CefCookieManagerImpl::SetCookie(const CefString& url, const CefCookie& cookie, CefRefPtr<CefSetCookieCallback> callback) {
    if (!CEF_CURRENTLY_ON_UIT())
        return false;

    std::string urlString = url.ToString();
    if (urlString.empty())
        return false;

    std::string cookieString = CefCookieToString(cookie);
    if (cookieString.empty())
        return false;

    blink::KURL kurl(blink::ParsedURLString, url.ToString().c_str());
    if (!kurl.isValid())
        return false;

    content::WebCookieJarImpl::inst()->setCookie(blink::WebURL(), kurl, blink::WebString::fromUTF8(cookieString.c_str()));

    if (callback.get()) {
        callback->AddRef();
        blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(CefOnSetCookieCallback, callback.get()));
    }

    return true;
}

bool CefCookieManagerImpl::DeleteCookies(const CefString& url, const CefString& cookie_name, CefRefPtr<CefDeleteCookiesCallback> callback) {
    // Empty URLs are allowed but not invalid URLs.
    blink::KURL gurl(blink::ParsedURLString, url.ToString().c_str());
    if (!gurl.isEmpty() && !gurl.isValid())
        return false;

    if (!CEF_CURRENTLY_ON_UIT())
        return false;

    std::string urlString = url.ToString();
    if (urlString.empty())
        return false;

    std::string cookieName = cookie_name.ToString();
    if (cookieName.empty())
        return false;

    blink::KURL kurl(blink::ParsedURLString, url.ToString().c_str());
    if (!kurl.isValid())
        return false;

    content::WebCookieJarImpl::inst()->deleteCookies(kurl, blink::WebString::fromUTF8(cookieName.c_str()));

    if (callback.get()) {
        callback->AddRef();
        blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(CefOnDelCookieCallback, callback.get()));
    }
    return true;
}

bool CefCookieManagerImpl::SetStoragePath(
    const CefString& path,
    bool persist_session_cookies,
    CefRefPtr<CefCompletionCallback> callback) {
//   if (!CEF_CURRENTLY_ON_IOT()) {
//       CEF_POST_TASK(CEF_IOT,
//           base::Bind(base::IgnoreResult(&CefCookieManagerImpl::SetStoragePath),
//           this, path, persist_session_cookies, callback));
//     return true;
//   }
// 
//   if (HasContext()) {
//     RunMethodWithContext(
//         base::Bind(&CefCookieManagerImpl::SetStoragePathWithContext, this, path,
//                    persist_session_cookies, callback));
//     return true;
//   }
// 
//   base::FilePath new_path;
//   if (!path.empty())
//     new_path = base::FilePath(path);
// 
//   if (cookie_monster_.get() && ((storage_path_.empty() && path.empty()) ||
//                                 storage_path_ == new_path)) {
//     // The path has not changed so don't do anything.
//     RunAsyncCompletionOnIOThread(callback);
//     return true;
//   }
// 
//   scoped_refptr<net::SQLitePersistentCookieStore> persistent_store;
//   if (!new_path.empty()) {
//     // TODO(cef): Move directory creation to the blocking pool instead of
//     // allowing file IO on this thread.
//     base::ThreadRestrictions::ScopedAllowIO allow_io;
//     if (base::DirectoryExists(new_path) ||
//         base::CreateDirectory(new_path)) {
//       const base::FilePath& cookie_path = new_path.AppendASCII("Cookies");
//       persistent_store =
//           new net::SQLitePersistentCookieStore(
//               cookie_path,
//               BrowserThread::GetMessageLoopProxyForThread(BrowserThread::IO),
//               BrowserThread::GetMessageLoopProxyForThread(BrowserThread::DB),
//               persist_session_cookies,
//               NULL);
//     } else {
//       NOTREACHED() << "The cookie storage directory could not be created";
//       storage_path_.clear();
//     }
//   }
// 
//   // Set the new cookie store that will be used for all new requests. The old
//   // cookie store, if any, will be automatically flushed and closed when no
//   // longer referenced.
//   cookie_monster_ = new net::CookieMonster(persistent_store.get(), NULL);
//   if (persistent_store.get() && persist_session_cookies)
//     cookie_monster_->SetPersistSessionCookies(true);
//   storage_path_ = new_path;
// 
//   // Restore the previously supported schemes.
//   SetSupportedSchemesInternal(supported_schemes_, callback);

  return true;
}

bool CefCookieManagerImpl::FlushStore(
    CefRefPtr<CefCompletionCallback> callback) {
//   GetCookieMonster(
//       BrowserThread::GetMessageLoopProxyForThread(BrowserThread::IO),
//       base::Bind(&CefCookieManagerImpl::FlushStoreInternal, this, callback));
  return true;
}

// static
// bool CefCookieManagerImpl::GetCefCookie(const net::CanonicalCookie& cc,
//                                         CefCookie& cookie) {
//   CefString(&cookie.name).FromString(cc.Name());
//   CefString(&cookie.value).FromString(cc.Value());
//   CefString(&cookie.domain).FromString(cc.Domain());
//   CefString(&cookie.path).FromString(cc.Path());
//   cookie.secure = cc.IsSecure();
//   cookie.httponly = cc.IsHttpOnly();
//   cef_time_from_basetime(cc.CreationDate(), cookie.creation);
//   cef_time_from_basetime(cc.LastAccessDate(), cookie.last_access);
//   cookie.has_expires = cc.IsPersistent();
//   if (cookie.has_expires)
//     cef_time_from_basetime(cc.ExpiryDate(), cookie.expires);
// 
//   return true;
// }
// 
// // static
// bool CefCookieManagerImpl::GetCefCookie(const GURL& url,
//                                         const std::string& cookie_line,
//                                         CefCookie& cookie) {
//   // Parse the cookie.
//   net::ParsedCookie pc(cookie_line);
//   if (!pc.IsValid())
//     return false;
// 
//   std::string cookie_domain;
//   if (!GetCookieDomain(url, pc, &cookie_domain))
//     return false;
// 
//   std::string cookie_path = net::CanonicalCookie::CanonPath(url, pc);
//   base::Time creation_time = base::Time::Now();
//   base::Time cookie_expires =
//       net::CanonicalCookie::CanonExpiration(pc, creation_time, creation_time);
// 
//   CefString(&cookie.name).FromString(pc.Name());
//   CefString(&cookie.value).FromString(pc.Value());
//   CefString(&cookie.domain).FromString(cookie_domain);
//   CefString(&cookie.path).FromString(cookie_path);
//   cookie.secure = pc.IsSecure();
//   cookie.httponly = pc.IsHttpOnly();
//   cef_time_from_basetime(creation_time, cookie.creation);
//   cef_time_from_basetime(creation_time, cookie.last_access);
//   cookie.has_expires = !cookie_expires.is_null();
//   if (cookie.has_expires)
//     cef_time_from_basetime(cookie_expires, cookie.expires);
// 
//   return true;
// }
// 
// // static
// void CefCookieManagerImpl::SetCookieMonsterSchemes(
//     net::CookieMonster* cookie_monster,
//     const std::set<std::string>& schemes) {
//   CEF_REQUIRE_IOT();
// 
//   std::set<std::string> all_schemes = schemes;
// 
//   // Add default schemes that should always support cookies.
//   all_schemes.insert("http");
//   all_schemes.insert("https");
//   all_schemes.insert("ws");
//   all_schemes.insert("wss");
// 
//   const char** arr = new const char*[all_schemes.size()];
//   std::set<std::string>::const_iterator it2 = all_schemes.begin();
//   for (int i = 0; it2 != all_schemes.end(); ++it2, ++i)
//     arr[i] = it2->c_str();
// 
//   cookie_monster->SetCookieableSchemes(arr, all_schemes.size());
// 
//   delete [] arr;
// }
// 
// bool CefCookieManagerImpl::HasContext() {
//   CEF_REQUIRE_IOT();
//   return (request_context_impl_.get() || request_context_.get());
// }
// 
// void CefCookieManagerImpl::RunMethodWithContext(
//     const CefRequestContextImpl::RequestContextCallback& method) {
//   CEF_REQUIRE_IOT();
//   if (request_context_impl_.get()) {
//     method.Run(request_context_impl_);
//   } else if (request_context_.get()) {
//     // Try again after the request context is initialized.
//     request_context_->GetRequestContextImpl(
//         BrowserThread::GetMessageLoopProxyForThread(BrowserThread::IO),
//         method);
//   } else {
//     NOTREACHED();
//   }
// }
// 
// void CefCookieManagerImpl::InitWithContext(
//     CefRefPtr<CefCompletionCallback> callback,
//     scoped_refptr<CefURLRequestContextGetterImpl> request_context) {
//   CEF_REQUIRE_IOT();
// 
//   DCHECK(!request_context_impl_.get());
//   request_context_impl_ = request_context;
// 
//   // Clear the CefRequestContextImpl reference here to avoid a potential
//   // reference loop between CefRequestContextImpl (which has a reference to
//   // CefRequestContextHandler), CefRequestContextHandler (which may keep a
//   // reference to this object) and this object.
//   request_context_ = NULL;
// 
//   RunAsyncCompletionOnIOThread(callback);
// }
// 
// void CefCookieManagerImpl::SetStoragePathWithContext(
//     const CefString& path,
//     bool persist_session_cookies,
//     CefRefPtr<CefCompletionCallback> callback,
//     scoped_refptr<CefURLRequestContextGetterImpl> request_context) {
//   CEF_REQUIRE_IOT();
// 
//   base::FilePath new_path;
//   if (!path.empty())
//     new_path = base::FilePath(path);
// 
//   request_context->SetCookieStoragePath(new_path, persist_session_cookies);
// 
//   RunAsyncCompletionOnIOThread(callback);
// }
// 
// void CefCookieManagerImpl::SetSupportedSchemesWithContext(
//     const std::set<std::string>& schemes,
//     CefRefPtr<CefCompletionCallback> callback,
//     scoped_refptr<CefURLRequestContextGetterImpl> request_context) {
//   CEF_REQUIRE_IOT();
// 
//   request_context->SetCookieSupportedSchemes(schemes);
// 
//   RunAsyncCompletionOnIOThread(callback);
// }
// 
// void CefCookieManagerImpl::GetCookieMonsterWithContext(
//     scoped_refptr<base::SingleThreadTaskRunner> task_runner,
//     const CookieMonsterCallback& callback,
//     scoped_refptr<CefURLRequestContextGetterImpl> request_context) {
//   CEF_REQUIRE_IOT();
// 
//   scoped_refptr<net::CookieMonster> cookie_monster =
//       request_context->GetCookieMonster();
// 
//   if (task_runner->BelongsToCurrentThread()) {
//     // Execute the callback immediately.
//     callback.Run(cookie_monster);
//   } else {
//     // Execute the callback on the target thread.
//     task_runner->PostTask(FROM_HERE, base::Bind(callback, cookie_monster));
//   }
// }
// 
// void CefCookieManagerImpl::SetSupportedSchemesInternal(
//     const std::set<std::string>& schemes,
//     CefRefPtr<CefCompletionCallback> callback){
//   CEF_REQUIRE_IOT();
// 
//   if (HasContext()) {
//     RunMethodWithContext(
//         base::Bind(&CefCookieManagerImpl::SetSupportedSchemesWithContext, this,
//                    schemes, callback));
//     return;
//   }
// 
//   DCHECK(cookie_monster_.get());
//   if (!cookie_monster_.get())
//     return;
// 
//   supported_schemes_ = schemes;
//   SetCookieMonsterSchemes(cookie_monster_.get(), supported_schemes_);
// 
//   RunAsyncCompletionOnIOThread(callback);
// }
// 
// void CefCookieManagerImpl::VisitAllCookiesInternal(
//     CefRefPtr<CefCookieVisitor> visitor,
//     scoped_refptr<net::CookieMonster> cookie_monster) {
//   CEF_REQUIRE_IOT();
// 
//   scoped_refptr<VisitCookiesCallback> callback(
//       new VisitCookiesCallback(cookie_monster, visitor));
// 
//   cookie_monster->GetAllCookiesAsync(
//       base::Bind(&VisitCookiesCallback::Run, callback.get()));
// }
// 
// void CefCookieManagerImpl::VisitUrlCookiesInternal(
//     const CefString& url,
//     bool includeHttpOnly,
//     CefRefPtr<CefCookieVisitor> visitor,
//     scoped_refptr<net::CookieMonster> cookie_monster) {
//   CEF_REQUIRE_IOT();
// 
//   net::CookieOptions options;
//   if (includeHttpOnly)
//     options.set_include_httponly();
// 
//   scoped_refptr<VisitCookiesCallback> callback(
//       new VisitCookiesCallback(cookie_monster, visitor));
// 
//   GURL gurl = GURL(url.ToString());
//   cookie_monster->GetAllCookiesForURLWithOptionsAsync(gurl, options,
//       base::Bind(&VisitCookiesCallback::Run, callback.get()));
// }
// 
// void CefCookieManagerImpl::SetCookieInternal(
//     const GURL& url,
//     const CefCookie& cookie,
//     CefRefPtr<CefSetCookieCallback> callback,
//     scoped_refptr<net::CookieMonster> cookie_monster) {
//   CEF_REQUIRE_IOT();
// 
//   std::string name = CefString(&cookie.name).ToString();
//   std::string value = CefString(&cookie.value).ToString();
//   std::string domain = CefString(&cookie.domain).ToString();
//   std::string path = CefString(&cookie.path).ToString();
// 
//   base::Time expiration_time;
//   if (cookie.has_expires)
//     cef_time_to_basetime(cookie.expires, expiration_time);
// 
//   cookie_monster->SetCookieWithDetailsAsync(
//       url, name, value, domain, path,
//       expiration_time,
//       cookie.secure ? true : false,
//       cookie.httponly ? true : false,
//       false,  // First-party only.
//       net::COOKIE_PRIORITY_DEFAULT,
//       base::Bind(SetCookieCallbackImpl, callback));
// }
// 
// void CefCookieManagerImpl::DeleteCookiesInternal(
//     const GURL& url,
//     const CefString& cookie_name,
//     CefRefPtr<CefDeleteCookiesCallback> callback,
//     scoped_refptr<net::CookieMonster> cookie_monster) {
//   CEF_REQUIRE_IOT();
// 
//   if (url.is_empty()) {
//     // Delete all cookies.
//     cookie_monster->DeleteAllAsync(
//         base::Bind(DeleteCookiesCallbackImpl, callback));
//   } else if (cookie_name.empty()) {
//     // Delete all matching host cookies.
//     cookie_monster->DeleteAllForHostAsync(url,
//         base::Bind(DeleteCookiesCallbackImpl, callback));
//   } else {
//     // Delete all matching host and domain cookies.
//     cookie_monster->DeleteCookieAsync(url, cookie_name,
//         base::Bind(DeleteCookiesCallbackImpl, callback, -1));
//   }
// }
// 
// void CefCookieManagerImpl::FlushStoreInternal(
//     CefRefPtr<CefCompletionCallback> callback,
//     scoped_refptr<net::CookieMonster> cookie_monster) {
//   CEF_REQUIRE_IOT();
// 
//   cookie_monster->FlushStore(
//       base::Bind(RunAsyncCompletionOnIOThread, callback));
// }

// CefCookieManager methods ----------------------------------------------------

// static
CefRefPtr<CefCookieManager> CefCookieManager::GetGlobalManager(
    CefRefPtr<CefCompletionCallback> callback) {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    NOTREACHED() << "context not valid";
    return NULL;
  }

  return CefRequestContext::GetGlobalContext()->GetDefaultCookieManager(callback);
}

// static
CefRefPtr<CefCookieManager> CefCookieManager::CreateManager(
    const CefString& path,
    bool persist_session_cookies,
    CefRefPtr<CefCompletionCallback> callback) {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    NOTREACHED() << "context not valid";
    return NULL;
  }

  CefRefPtr<CefCookieManagerImpl> cookie_manager = new CefCookieManagerImpl();
  cookie_manager->Initialize(NULL, path, persist_session_cookies, callback);
  return cookie_manager.get();
}

//////////////////////////////////////////////////////////////////////////
// test
void CefCookieManagerImpl::AddRef() const {
    ref_count_.AddRef();
}

bool CefCookieManagerImpl::Release() const {
    if (ref_count_.Release()) {
        delete static_cast<const CefCookieManagerImpl*>(this);
        return true;
    }
    return false;
}

bool CefCookieManagerImpl::HasOneRef() const {
    return ref_count_.HasOneRef();
}
#endif

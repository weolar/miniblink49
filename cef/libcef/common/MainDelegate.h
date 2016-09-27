#ifndef CEF_LIBCEF_COMMON_MAIN_DELEGATE_H_
#define CEF_LIBCEF_COMMON_MAIN_DELEGATE_H_

#include "include/cef_app.h"
#include "libcef/common/CefContentClient.h"

class CefMainDelegate {
public:
    explicit CefMainDelegate(CefRefPtr<CefApp> application);
    ~CefMainDelegate();

private:
    void InitializeResourceBundle();

//     scoped_ptr<CefContentBrowserClient> browser_client_;
//     scoped_ptr<CefContentRendererClient> renderer_client_;
//     scoped_ptr<CefContentUtilityClient> utility_client_;
    CefContentClient m_contentClient;

};

#endif // CEF_LIBCEF_COMMON_MAIN_DELEGATE_H_
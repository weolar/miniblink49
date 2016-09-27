
#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"

#include "include/base/cef_logging.h"
#include "libcef/common/CefContentClient.h"

CefContentClient* g_contentClient = nullptr;

CefContentClient::CefContentClient(CefRefPtr<CefApp> application)
    : m_application(application)
    , m_rendererApplication(application)
    , m_bSchemeInfoListLocked(false) {
    DCHECK(!g_contentClient);
    g_contentClient = this;
}

CefContentClient::~CefContentClient() {
    g_contentClient = NULL;
}

// static
CefContentClient* CefContentClient::Get() {
    return g_contentClient;
}
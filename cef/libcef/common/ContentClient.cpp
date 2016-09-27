
#include "libcef/common/ContentClient.h"

CefContentClient* g_contentClient = nullptr;

CefContentClient::CefContentClient(CefRefPtr<CefApp> application)
    : m_application(application),
    m_bSchemeInfoListLocked(false) {
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
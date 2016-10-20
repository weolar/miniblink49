#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "libcef/common/MainDelegate.h"

CefMainDelegate::CefMainDelegate(CefRefPtr<CefApp> application)
    : m_contentClient(application) {
    // Necessary so that exported functions from base_impl.cc will be included
    // in the binary.
}

CefMainDelegate::~CefMainDelegate() {
}
#endif

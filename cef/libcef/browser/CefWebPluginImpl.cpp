#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "cef/libcef/browser/CefWebPluginImpl.h"

void CefVisitWebPluginInfo(CefRefPtr<CefWebPluginInfoVisitor> visitor) {
    DebugBreak();
}

void CefRefreshWebPlugins() {
    DebugBreak();
}

void CefAddWebPluginPath(const CefString& path) {
    DebugBreak();
}

void CefAddWebPluginDirectory(const CefString& dir) {
    DebugBreak();
}

void CefRemoveWebPluginPath(const CefString& path) {
    DebugBreak();
}

void CefUnregisterInternalWebPlugin(const CefString& path) {
    DebugBreak();
}

void CefForceWebPluginShutdown(const CefString& path) {
    DebugBreak();
}

void CefRegisterWebPluginCrash(const CefString& path) {
    DebugBreak();
}
#endif

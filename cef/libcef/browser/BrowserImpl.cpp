
#include "include/capi/cef_browser_capi.h"
#include "include/capi/cef_client_capi.h"

#include "libcef/common/CommonBase.h"
#include "content/browser/WebPage.h"
#include "libcef/browser/BrowserHostImpl.h"
#include "libcef/browser/FrameImpl.h"

#include "third_party/WebKit/Source/web/WebViewImpl.h"

#undef CEF_EXPORT
#define CEF_EXPORT extern "C" __declspec(dllexport) 

namespace cef {

class BrowserImpl : public CefCppBase < cef_browser_t, BrowserImpl > {
public:
    BrowserImpl();
    void willDeleted() { DebugBreak(); return; }

    static cef_browser_host_t* CEF_CALLBACK getHost(cef_browser_t* self);

    static int CEF_CALLBACK canGoBack(cef_browser_t* self) { DebugBreak(); return 0; }

    static void CEF_CALLBACK goBack(cef_browser_t* self) { DebugBreak(); return; }

    static int CEF_CALLBACK canGoForward(cef_browser_t* self) { DebugBreak(); return 0; }

    static void CEF_CALLBACK goForward(cef_browser_t* self) { DebugBreak(); return; }

    static int CEF_CALLBACK isLoading(cef_browser_t* self) { DebugBreak(); return 0; }

    static void CEF_CALLBACK reload(cef_browser_t* s)
    {
        CEF_CHECK_ARGS_CAST_NORET(BrowserImpl);
        blink::WebViewImpl* webViewImpl = self->m_browserHost->m_webPage->webViewImpl();
        if (!webViewImpl)
            return;
        blink::WebFrame* mainFrame = webViewImpl->mainFrame();
        mainFrame->reload();
    }

    static void CEF_CALLBACK reloadIgnoreCache(cef_browser_t* self) { DebugBreak(); return; }

    static void CEF_CALLBACK stopLoad(cef_browser_t* s)
    {
        CEF_CHECK_ARGS_CAST_NORET(BrowserImpl);
        blink::WebViewImpl* webViewImpl = self->m_browserHost->m_webPage->webViewImpl();
        if (!webViewImpl)
            return;
        blink::WebFrame* mainFrame = webViewImpl->mainFrame();
        mainFrame->stopLoading();
    }

    static int CEF_CALLBACK getIdentifier(cef_browser_t* self) { return (int)self; }

    static int CEF_CALLBACK isSame(cef_browser_t* self, cef_browser_t* that) { DebugBreak(); return 0; }

    static int CEF_CALLBACK isPopup(cef_browser_t* self) { DebugBreak(); return 0; }

    static int CEF_CALLBACK hasDocument(cef_browser_t* self) { DebugBreak(); return 0; }

    static cef_frame_t* CEF_CALLBACK getMainFrame(cef_browser_t* s)
    {
        CEF_CHECK_ARGS_CAST(BrowserImpl, nullptr);
        blink::WebViewImpl* webViewImpl = self->m_browserHost->m_webPage->webViewImpl();
        if (!webViewImpl)
            return nullptr;
        blink::WebFrame* mainFrame = webViewImpl->mainFrame();
        FrameImpl* frame =  new FrameImpl(self, self->m_browserHost->m_webPage, mainFrame);
        return frame->cast();
    }

    static cef_frame_t* CEF_CALLBACK getFocusedFrame(cef_browser_t* s) 
    {
        CEF_CHECK_ARGS_CAST(BrowserImpl, nullptr);
        blink::WebViewImpl* webViewImpl = self->m_browserHost->m_webPage->webViewImpl();
        if (!webViewImpl)
            return nullptr;
        blink::WebFrame* mainFrame = webViewImpl->focusedFrame();
        FrameImpl* frame = new FrameImpl(self, self->m_browserHost->m_webPage, mainFrame);
        return frame->cast();
    }

    static cef_frame_t* CEF_CALLBACK getFrameByident(cef_browser_t* self, int64 identifier) { DebugBreak(); return nullptr; }
    static cef_frame_t* CEF_CALLBACK getFrame(cef_browser_t* self, const cef_string_t* name) { DebugBreak(); return nullptr; }
    static size_t CEF_CALLBACK getFrameCount(cef_browser_t* self) { DebugBreak(); return 0; }
    static void CEF_CALLBACK getFrameIdentifiers(cef_browser_t* self, size_t* identifiersCount, int64* identifiers) { DebugBreak(); return; }

    static void CEF_CALLBACK getFrameNames(cef_browser_t* self, cef_string_list_t names) { DebugBreak(); return; }

    static int CEF_CALLBACK sendProcessMessage(cef_browser_t* self, cef_process_id_t target_process, cef_process_message_t* message) { DebugBreak(); return 0; }

    cef_context_menu_handler_t* m_contextMenuHandler;
    cef_dialog_handler_t* m_dialogHandler;
    cef_display_handler_t* m_displayHandler;
    cef_download_handler_t* m_downloadHandler;
    cef_drag_handler_t* m_dragHandler;
#ifdef _CEF_2454_VERSION_
    cef_find_handler_t* m_findHandler;
#endif
    cef_focus_handler_t* m_focusHandler;
    cef_geolocation_handler_t* m_geolocationHandler;
    cef_jsdialog_handler_t* m_jsdialogHandler;
    cef_keyboard_handler_t* m_keyboardHandler;
    cef_life_span_handler_t* m_lifespanHandler;
    cef_load_handler_t* m_loadHandler;
    cef_render_handler_t* m_renderHandler;

    BrowserHostImpl* m_browserHost;
};

BrowserImpl::BrowserImpl()
{
    memset(&m_baseClass, 0, sizeof(m_baseClass));
    m_baseClass.get_host = getHost;
    m_baseClass.can_go_back = canGoBack;
    m_baseClass.go_back = goBack;
    m_baseClass.go_forward = goForward;
    m_baseClass.can_go_forward = canGoForward;
    m_baseClass.is_loading = isLoading;
    m_baseClass.reload = reload;
    m_baseClass.reload_ignore_cache = reloadIgnoreCache;
    m_baseClass.stop_load = stopLoad;
    m_baseClass.get_identifier = getIdentifier;
    m_baseClass.is_same = isSame;
    m_baseClass.is_popup = isPopup;
    m_baseClass.has_document = hasDocument;
    m_baseClass.get_main_frame = getMainFrame;
    m_baseClass.get_focused_frame = getFocusedFrame;
    m_baseClass.get_frame_byident = getFrameByident;
    m_baseClass.get_frame = getFrame;
    m_baseClass.get_frame_count = getFrameCount;
    m_baseClass.get_frame_identifiers = getFrameIdentifiers;
    m_baseClass.get_frame_names = getFrameNames;
    m_baseClass.send_process_message = sendProcessMessage;
    CefCppBase<cef_browser_t, BrowserImpl>::init(this);

    m_contextMenuHandler = nullptr;
    m_dialogHandler = nullptr;
    m_displayHandler = nullptr;
    m_downloadHandler = nullptr;
    m_dragHandler = nullptr;
#ifdef _CEF_2454_VERSION_
    m_findHandler = nullptr;
#endif
    m_focusHandler = nullptr;
    m_geolocationHandler = nullptr;
    m_jsdialogHandler = nullptr;
    m_keyboardHandler = nullptr;
    m_lifespanHandler = nullptr;
    m_loadHandler = nullptr;
    m_renderHandler = nullptr;

    m_browserHost = nullptr;
}

cef_browser_host_t* CEF_CALLBACK BrowserImpl::getHost(cef_browser_t* s)
{
    CEF_CHECK_ARGS_CAST(BrowserImpl, nullptr);
    self->m_browserHost->ref();
    return (cef_browser_host_t*)self->m_browserHost;
}

} // cef

#include "include/capi/cef_browser_capi.h"

#include "libcef/common/CommonBase.h"
#include "libcef/browser/CefGlobal.h"
#include "include/internal/cef_types_wrappers.h"

typedef struct _cef_context_menu_handler_t cef_context_menu_handler_t;
typedef struct _cef_dialog_handler_t cef_dialog_handler_t;
typedef struct _cef_display_handler_t cef_display_handler_t;
typedef struct _cef_download_handler_t cef_download_handler_t;
typedef struct _cef_drag_handler_t cef_drag_handler_t;
#ifdef _CEF_2454_VERSION_
typedef struct _cef_find_handler_t cef_find_handler_t;
#endif
typedef struct _cef_focus_handler_t cef_focus_handler_t;
typedef struct _cef_geolocation_handler_t cef_geolocation_handler_t;
typedef struct _cef_jsdialog_handler_t cef_jsdialog_handler_t;
typedef struct _cef_keyboard_handler_t cef_keyboard_handler_t;
typedef struct _cef_life_span_handler_t cef_life_span_handler_t;
typedef struct _cef_load_handler_t cef_load_handler_t;
typedef struct _cef_render_handler_t cef_render_handler_t;

namespace cef {

class BrowserHostImpl;
class CefRequestContext;

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

    static void CEF_CALLBACK reload(cef_browser_t* self) { DebugBreak(); return; }

    static void CEF_CALLBACK reloadIgnoreCache(cef_browser_t* self) { DebugBreak(); return; }

    static void CEF_CALLBACK stopLoad(cef_browser_t* self) { DebugBreak(); return; }

    static int CEF_CALLBACK getIdentifier(cef_browser_t* self) { DebugBreak(); return 0; }

    static int CEF_CALLBACK isSame(cef_browser_t* self, cef_browser_t* that) { DebugBreak(); return 0; }

    static int CEF_CALLBACK isPopup(cef_browser_t* self) { DebugBreak(); return 0; }

    static int CEF_CALLBACK hasDocument(cef_browser_t* self) { DebugBreak(); return 0; }

    static cef_frame_t* CEF_CALLBACK getMainFrame(cef_browser_t* self) { DebugBreak(); return nullptr; }
    static cef_frame_t* CEF_CALLBACK getFocusedFrame(cef_browser_t* self) { DebugBreak(); return nullptr; }
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

    cef_client_t* m_client;

    BrowserHostImpl* m_browserHost;
    const CefBrowserSettings& settings() const { return m_settings; }

    CefBrowserSettings m_settings;
    CefRequestContext* m_requestContext;
};

} // cef
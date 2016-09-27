
#include "include/internal/cef_export.h"
#include "include/capi/cef_base_capi.h"
#include "include/capi/cef_browser_capi.h"
#include "include/capi/cef_client_capi.h"

#include "include/capi/cef_context_menu_handler_capi.h"
#include "include/capi/cef_dialog_handler_capi.h"
#include "include/capi/cef_display_handler_capi.h"
#include "include/capi/cef_download_handler_capi.h"
#include "include/capi/cef_drag_handler_capi.h"
#ifdef _CEF_2454_VERSION_
#include "include/capi/cef_find_handler_capi.h"
#endif
#include "include/capi/cef_focus_handler_capi.h"
#include "include/capi/cef_geolocation_handler_capi.h"
#include "include/capi/cef_jsdialog_handler_capi.h"
#include "include/capi/cef_keyboard_handler_capi.h"
#include "include/capi/cef_life_span_handler_capi.h"
#include "include/capi/cef_load_handler_capi.h"
#include "include/capi/cef_render_handler_capi.h"

#include "include/capi/cef_resource_bundle_handler_capi.h"
#include "include/capi/cef_browser_process_handler_capi.h"
#include "include/capi/cef_render_process_handler_capi.h"

#include "libcef/common/CommonBase.h"
#include "libcef/browser/BrowserHostImpl.h"
#include "libcef/browser/BrowserImpl.h"
#include "libcef/browser/RequestImpl.h"

#include "content/browser/WebPage.h"

#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "libcef/browser/FrameImpl.h"

#undef CEF_EXPORT
#define CEF_EXPORT extern "C" __declspec(dllexport) 

extern const WCHAR* szWindowClass;
static ATOM registerClass(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

namespace cef {

cef_browser_t* BrowserHostImpl::get_browser(cef_browser_host_t* s)
{
    CEF_CHECK_ARGS_CAST(BrowserHostImpl, nullptr);
    self->m_browserImpl->ref();
    return (cef_browser_t*)self->m_browserImpl;
}

cef_window_handle_t BrowserHostImpl::get_window_handle(cef_browser_host_t* s)
{
    CEF_CHECK_ARGS_CAST(BrowserHostImpl, 0);
    return self->m_webPage->getHWND();
}

int BrowserHostImpl::get_windowless_frame_rate(cef_browser_host_t* s)
{ 
    CEF_CHECK_ARGS_CAST(BrowserHostImpl, 10);
    return self->m_windowlessFrameRate;
}

void BrowserHostImpl::set_windowless_frame_rate(cef_browser_host_t* s, int frame_rate) 
{ 
    CEF_CHECK_ARGS_CAST_NORET(BrowserHostImpl);
    self->m_windowlessFrameRate = frame_rate;
}

BrowserHostImpl::BrowserHostImpl(BrowserImpl* browser, blink::WebPage* webPage)
{
    m_windowlessFrameRate = 10;

    memset(&m_baseClass, 0, sizeof(m_baseClass));
    m_baseClass.get_browser = get_browser;
    m_baseClass.close_browser = close_browser;
    m_baseClass.set_focus = set_focus;
    m_baseClass.set_window_visibility = set_window_visibility;
    m_baseClass.get_window_handle = get_window_handle;
    m_baseClass.get_opener_window_handle = get_opener_window_handle;
    m_baseClass.get_client = get_client;
    m_baseClass.get_request_context = get_request_context;
    m_baseClass.get_zoom_level = get_zoom_level;
    m_baseClass.set_zoom_level = set_zoom_level;
    m_baseClass.run_file_dialog = run_file_dialog;
    m_baseClass.start_download = start_download;
    m_baseClass.print = print;
#ifdef _CEF_2454_VERSION_
    m_baseClass.print_to_pdf = print_to_pdf;
#endif
    m_baseClass.find = find;
    m_baseClass.stop_finding = stop_finding;

    m_baseClass.show_dev_tools = show_dev_tools;
    m_baseClass.close_dev_tools = close_dev_tools;

#ifdef _CEF_2454_VERSION_
    m_baseClass.get_navigation_entries = get_navigation_entries;
#endif
    m_baseClass.set_mouse_cursor_change_disabled = set_mouse_cursor_change_disabled;
    m_baseClass.is_mouse_cursor_change_disabled = is_mouse_cursor_change_disabled;
#ifdef _CEF_2454_VERSION_
    m_baseClass.replace_misspelling = replace_misspelling;
    m_baseClass.add_word_to_dictionary = add_word_to_dictionary;
#endif
    m_baseClass.is_window_rendering_disabled = is_window_rendering_disabled;
    m_baseClass.was_resized = was_resized;
    m_baseClass.was_hidden = was_hidden;
    m_baseClass.notify_screen_info_changed = notify_screen_info_changed;
    m_baseClass.invalidate = invalidate;
    m_baseClass.send_key_event = send_key_event;
    m_baseClass.send_mouse_click_event = send_mouse_click_event;
    m_baseClass.send_mouse_move_event = send_mouse_move_event;
    m_baseClass.send_mouse_wheel_event = send_mouse_wheel_event;
    m_baseClass.send_focus_event = send_focus_event;
    m_baseClass.send_capture_lost_event = send_capture_lost_event;
#ifdef _CEF_2454_VERSION_
    m_baseClass.notify_move_or_resize_started = notify_move_or_resize_started;
    m_baseClass.get_windowless_frame_rate = get_windowless_frame_rate;
    m_baseClass.set_windowless_frame_rate = set_windowless_frame_rate;
#endif
    m_baseClass.get_nstext_input_context = get_nstext_input_context;
    m_baseClass.handle_key_event_before_text_input_client = handle_key_event_before_text_input_client;
    m_baseClass.handle_key_event_after_text_input_client = handle_key_event_after_text_input_client;
    m_baseClass.drag_target_drag_enter = drag_target_drag_enter;
    m_baseClass.drag_target_drag_over = drag_target_drag_over;
    m_baseClass.drag_target_drag_leave = drag_target_drag_leave;
    m_baseClass.drag_target_drop = drag_target_drop;
    m_baseClass.drag_source_ended_at = drag_source_ended_at;
    m_baseClass.drag_source_system_drag_ended = drag_source_system_drag_ended;
    CefCppBase<cef_browser_host_t, BrowserHostImpl>::init(this);

    m_browserImpl = browser;
    m_webPage = webPage;
}

void BrowserHostImpl::willDeleted()
{
    DebugBreak();
}

} // cef

static void getHandle(cef_client_t* client, cef::BrowserImpl* browserImpl)
{
    browserImpl->m_contextMenuHandler = client->get_context_menu_handler(client);
    browserImpl->m_dialogHandler = client->get_dialog_handler(client);
    browserImpl->m_displayHandler = client->get_display_handler(client);
    browserImpl->m_downloadHandler = client->get_download_handler(client);
    browserImpl->m_dragHandler = client->get_drag_handler(client);
#ifdef _CEF_2454_VERSION_
    browserImpl->m_findHandler = client->get_find_handler(client);
#endif
    browserImpl->m_focusHandler = client->get_focus_handler(client);
    browserImpl->m_geolocationHandler = client->get_geolocation_handler(client);
    browserImpl->m_jsdialogHandler = client->get_jsdialog_handler(client);
    browserImpl->m_keyboardHandler = client->get_keyboard_handler(client);
    browserImpl->m_lifespanHandler = client->get_life_span_handler(client);
    browserImpl->m_loadHandler = client->get_load_handler(client);
    browserImpl->m_renderHandler = client->get_render_handler(client);
    browserImpl->m_client = client;
}

static void TranslatePopupFeatures(const blink::WebWindowFeatures& webKitFeatures, CefPopupFeatures& features) {
    features.x = static_cast<int>(webKitFeatures.x);
    features.xSet = webKitFeatures.xSet;
    features.y = static_cast<int>(webKitFeatures.y);
    features.ySet = webKitFeatures.ySet;
    features.width = static_cast<int>(webKitFeatures.width);
    features.widthSet = webKitFeatures.widthSet;
    features.height = static_cast<int>(webKitFeatures.height);
    features.heightSet = webKitFeatures.heightSet;

    features.menuBarVisible = webKitFeatures.menuBarVisible;
    features.statusBarVisible = webKitFeatures.statusBarVisible;
    features.toolBarVisible = webKitFeatures.toolBarVisible;
    features.locationBarVisible = webKitFeatures.locationBarVisible;
    features.scrollbarsVisible = webKitFeatures.scrollbarsVisible;
    features.resizable = webKitFeatures.resizable;

    features.fullscreen = webKitFeatures.fullscreen;
    features.dialog = webKitFeatures.dialog;
    features.additionalFeatures = NULL;
    if (webKitFeatures.additionalFeatures.size() > 0)
        features.additionalFeatures = cef_string_list_alloc();

    CefString str;
    for (unsigned int i = 0; i < webKitFeatures.additionalFeatures.size(); ++i) {
        str = base::string16(webKitFeatures.additionalFeatures[i]);
        cef_string_list_append(features.additionalFeatures, str.GetStruct());
    }
}

// cef原版实现中，BrowserImpl和BrowserHost的实现类居然是同一个
CEF_EXPORT cef_browser_t* cef_browser_host_create_browser_sync(
    const cef_window_info_t* windowInfo, cef_client_t* client,
    const cef_string_t* url, const cef_browser_settings_t* settings,
    cef_request_context_t* request_context)
{
    blink::WebPage* webPage = new blink::WebPage(nullptr, nullptr);

    cef::BrowserImpl* browserImpl = new cef::BrowserImpl();
    cef_browser_t* browser = browserImpl->cast();

    browserImpl->m_browserHost = new cef::BrowserHostImpl(browserImpl, webPage);

    if (g_cefGlobal->m_renderProcessHandler) {
        g_cefGlobal->m_renderProcessHandler->on_browser_created(g_cefGlobal->m_renderProcessHandler, browser);
    }

    //g_cefGlobal->m_client = client;
    getHandle(client, browserImpl);

    registerClass(g_cefGlobal->m_mainArgs->instance);
    ((cef_window_info_t*)windowInfo)->window = ::CreateWindowW(szWindowClass, L"mini cef", windowInfo->style,
        windowInfo->x, windowInfo->y, windowInfo->width, windowInfo->height,
        windowInfo->parent_window, NULL, g_cefGlobal->m_mainArgs->instance, webPage);

    if (!windowInfo->window)
        return 0;

    webPage->setCefBrowserImpl(browserImpl);

    browserImpl->m_lifespanHandler->on_after_created(browserImpl->m_lifespanHandler, browser);

    cef_frame_t frame = { 0 };
    cef_v8context_t context = { 0 };
    g_cefGlobal->m_renderProcessHandler->on_context_created(g_cefGlobal->m_renderProcessHandler, browser, &frame, &context);

    //LPCWSTR lpUrl = L"http://www.baidu.com";
    //webPage->loadFormUrl(lpUrl, -1);
    webPage->loadFormUrl(url->str, url->length);

    ::ShowWindow(windowInfo->window, TRUE);
    ::UpdateWindow(windowInfo->window);
    ::SetTimer(windowInfo->window, 10010, 50, NULL);

    browserImpl->ref();
    return browser;
}

CEF_EXPORT int cef_browser_host_create_browser(
    const cef_window_info_t* windowInfo, cef_client_t* client,
    const cef_string_t* url, const cef_browser_settings_t* settings,
    cef_request_context_t* request_context) {
    
    cef_browser_t* browser = cef_browser_host_create_browser_sync(windowInfo, client, url, settings, request_context);
    browser->base.release(&browser->base);
    return 1;
}

bool BrowserHostImpl::createView(
    blink::WebPageImpl* pageImpl,
    blink::WebLocalFrame* creator,
    const blink::WebURLRequest& request,
    const blink::WebWindowFeatures& features,
    const blink::WebString& name,
    blink::WebNavigationPolicy policy,
    bool suppressOpener)
{
    cef::BrowserImpl* browserImpl = pageImpl->cefBrowserImpl();
    CefWindowInfo windowInfo;
    CefPopupFeatures cefFeatures;
    TranslatePopupFeatures(features, cefFeatures);

    if (cefFeatures.xSet)
        windowInfo.x = cefFeatures.x;
    if (cefFeatures.ySet)
        windowInfo.y = cefFeatures.y;
    if (cefFeatures.widthSet)
        windowInfo.width = cefFeatures.width;
    if (cefFeatures.heightSet)
        windowInfo.height = cefFeatures.height;

    RefPtr<FrameImpl> frame(new FrameImpl(browserImpl, pageImpl->m_pagePtr, creator));

    CefStringUTF16 urlSpec;
    cef::setWebStringToCefString16(request.url().spec().utf16(), urlSpec);

    CefStringUTF16 frameName;
    cef::setWebStringToCefString16(name, frameName);

    cef_window_open_disposition_t disposition;
    if (suppressOpener)
        disposition = WOD_SUPPRESS_OPEN;
    else if (blink::WebNavigationPolicyIgnore == policy)
        disposition = WOD_IGNORE_ACTION;
    else if (blink::WebNavigationPolicyDownload == policy)
        disposition = WOD_SAVE_TO_DISK;
    else if (blink::WebNavigationPolicyCurrentTab == policy)
        disposition = WOD_CURRENT_TAB;
    else if (blink::WebNavigationPolicyNewBackgroundTab == policy)
        disposition = WOD_NEW_BACKGROUND_TAB;
    else if (blink::WebNavigationPolicyNewForegroundTab == policy)
        disposition = WOD_NEW_FOREGROUND_TAB;
    else if (blink::WebNavigationPolicyNewWindow == policy)
        disposition = WOD_SINGLETON_TAB;
    else if (blink::WebNavigationPolicyNewPopup == policy)
        disposition = WOD_NEW_POPUP;
    
    bool* no_javascript_access = nullptr;
    bool allow = !browserImpl->m_lifespanHandler->on_before_popup(browserImpl->m_lifespanHandler,
        browserImpl.cast(), frame.get(), urlSpec.GetStruct(), frameName.GetStruct(),
        disposition, true, cefFeatures, windowInfo,
        browserImpl->m_client, browserImpl->m_settings, no_javascript_access);
    if (!allow)
        return false;

    cef_browser_t* cef_browser_host_create_browser_sync(
        windowInfo, cef_client_t* client,
        const cef_string_t* url, browserImpl->m_settings,
        cef_request_context_t* request_context);
    return allow;
}

static ATOM registerClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex = { 0 };

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    //wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINIWEBKIT_BLINK));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    //wcex.lpszMenuName = MAKEINTRESOURCE(IDC_MINIWEBKIT_BLINK);
    wcex.lpszClassName = szWindowClass;
    //wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}
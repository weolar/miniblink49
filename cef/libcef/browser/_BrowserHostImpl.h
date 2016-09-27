#ifndef BrowserHostImpl_h
#define BrowserHostImpl_h

#include "third_party/WebKit/public/web/WebNavigationPolicy.h"

namespace blink {
class WebPage;
class WebPageImpl;
class WebLocalFrame;
class WebURLRequest;
class WebWindowFeatures;
class WebString;
}

namespace cef {

class BrowserImpl;

class BrowserHostImpl : public CefCppBase < cef_browser_host_t, BrowserHostImpl > {
public:
    BrowserHostImpl(BrowserImpl* browser, blink::WebPage* webPage);
    void willDeleted();

    static cef_browser_t* CEF_CALLBACK get_browser(cef_browser_host_t* self);

    static void CEF_CALLBACK close_browser(cef_browser_host_t* self, int force_close) { DebugBreak(); return; }

    static void CEF_CALLBACK set_focus(cef_browser_host_t* self, int focus) { return; }

    static void CEF_CALLBACK set_window_visibility(cef_browser_host_t* self, int visible) { return; }

    static cef_window_handle_t CEF_CALLBACK get_window_handle(cef_browser_host_t* self);

    static cef_window_handle_t CEF_CALLBACK get_opener_window_handle(cef_browser_host_t* self) { DebugBreak(); return nullptr; }

    static cef_client_t* CEF_CALLBACK get_client(cef_browser_host_t* self) { DebugBreak(); return nullptr; }

    static cef_request_context_t* CEF_CALLBACK get_request_context(cef_browser_host_t* self) { DebugBreak(); return nullptr; }

    static double CEF_CALLBACK get_zoom_level(cef_browser_host_t* self) { DebugBreak(); return 0; }

    static void CEF_CALLBACK set_zoom_level(cef_browser_host_t* self,
        double zoomLevel) { 
        DebugBreak(); 
        return ;
    }

    static void CEF_CALLBACK run_file_dialog(cef_browser_host_t* self,
        cef_file_dialog_mode_t mode, const cef_string_t* title,
        const cef_string_t* default_file_path, cef_string_list_t accept_filters,
#ifdef _CEF_2454_VERSION_
        int selected_accept_filter,
#endif
        cef_run_file_dialog_callback_t* callback) {
        DebugBreak(); return ;
    }

    static void CEF_CALLBACK start_download(cef_browser_host_t* self, const cef_string_t* url) { DebugBreak(); return; }

    static void CEF_CALLBACK print(cef_browser_host_t* self) { DebugBreak(); return; }

#ifdef _CEF_2454_VERSION_
    static void CEF_CALLBACK print_to_pdf(cef_browser_host_t* self, const cef_string_t* path,
        const cef_pdf_print_settings_t* settings, cef_pdf_print_callback_t* callback) {
        DebugBreak(); return ;
    }
#endif

    static void CEF_CALLBACK find(cef_browser_host_t* self, int identifier,
        const cef_string_t* searchText, int forward, int matchCase,
        int findNext) { DebugBreak(); return ; }

    static void CEF_CALLBACK stop_finding(cef_browser_host_t* self, int clearSelection) { DebugBreak(); return; }


    static void CEF_CALLBACK show_dev_tools(cef_browser_host_t* self, const cef_window_info_t* windowInfo,
        cef_client_t* client,
        const cef_browser_settings_t* settings
#ifdef _CEF_2454_VERSION_
        , const cef_point_t* inspect_element_at
#endif
        ) { DebugBreak(); return ; }


    static void CEF_CALLBACK close_dev_tools(cef_browser_host_t* self) { DebugBreak(); return; }

#ifdef _CEF_2454_VERSION_
    static void CEF_CALLBACK get_navigation_entries(cef_browser_host_t* self,
        cef_navigation_entry_visitor_t* visitor, int current_only) { DebugBreak(); return ; }
#endif

    static void CEF_CALLBACK set_mouse_cursor_change_disabled(cef_browser_host_t* self, int disabled) { DebugBreak(); return; }

    static int CEF_CALLBACK is_mouse_cursor_change_disabled(cef_browser_host_t* self) { DebugBreak(); return 0; }

    static void CEF_CALLBACK replace_misspelling(cef_browser_host_t* self, const cef_string_t* word) { DebugBreak(); return; }

    static void CEF_CALLBACK add_word_to_dictionary(cef_browser_host_t* self, const cef_string_t* word) { DebugBreak(); return; }

    static int CEF_CALLBACK is_window_rendering_disabled(cef_browser_host_t* self) { DebugBreak(); return 1; }

    static void CEF_CALLBACK was_resized(cef_browser_host_t* self) { return; }

    static void CEF_CALLBACK was_hidden(cef_browser_host_t* self, int hidden) { return; }

    static void CEF_CALLBACK notify_screen_info_changed(cef_browser_host_t* self) { return; }

    static void CEF_CALLBACK invalidate(cef_browser_host_t* self, 
#ifndef _CEF_2454_VERSION_
        const cef_rect_t* dirtyRect,
#endif
        cef_paint_element_type_t type) { return; }

    static void CEF_CALLBACK send_key_event(cef_browser_host_t* self, const cef_key_event_t* event) { return; }

    static void CEF_CALLBACK send_mouse_click_event(cef_browser_host_t* self, const cef_mouse_event_t* event, cef_mouse_button_type_t type,
        int mouseUp, int clickCount) {

        return;
    }

    static void CEF_CALLBACK send_mouse_move_event(cef_browser_host_t* self, const cef_mouse_event_t* event, int mouseLeave) { return; }

    static void CEF_CALLBACK send_mouse_wheel_event(cef_browser_host_t* self, const cef_mouse_event_t* event, int deltaX, int deltaY) { return; }

    static void CEF_CALLBACK send_focus_event(cef_browser_host_t* self, int setFocus) { return; }

    static void CEF_CALLBACK send_capture_lost_event(cef_browser_host_t* self) { return; }

    static void CEF_CALLBACK notify_move_or_resize_started(cef_browser_host_t* self) { return; }

    static int CEF_CALLBACK get_windowless_frame_rate(cef_browser_host_t* self);

    static void CEF_CALLBACK set_windowless_frame_rate(cef_browser_host_t* self, int frame_rate);

    static cef_text_input_context_t CEF_CALLBACK get_nstext_input_context(cef_browser_host_t* self) { DebugBreak(); return nullptr; }

    static void CEF_CALLBACK handle_key_event_before_text_input_client(cef_browser_host_t* self, cef_event_handle_t keyEvent) { return; }

    static void CEF_CALLBACK handle_key_event_after_text_input_client(cef_browser_host_t* self, cef_event_handle_t keyEvent) { return; }

    static void CEF_CALLBACK drag_target_drag_enter(cef_browser_host_t* self,
        cef_drag_data_t* drag_data,
        const cef_mouse_event_t* event,
        cef_drag_operations_mask_t allowed_ops) {
         
        return;
    }

    static void CEF_CALLBACK drag_target_drag_over(cef_browser_host_t* self,
        const cef_mouse_event_t* event,
        cef_drag_operations_mask_t allowed_ops) { return ; }

    static void CEF_CALLBACK drag_target_drag_leave(cef_browser_host_t* self) { return; }

    static void CEF_CALLBACK drag_target_drop(cef_browser_host_t* self, const cef_mouse_event_t* event) { return; }

    static void CEF_CALLBACK drag_source_ended_at(cef_browser_host_t* self, int x, int y, cef_drag_operations_mask_t op) { return; }

    static void CEF_CALLBACK drag_source_system_drag_ended(cef_browser_host_t* self) { return; }

    bool createView(
        blink::WebPageImpl* pageImpl,
        blink::WebLocalFrame* creator,
        const blink::WebURLRequest& request,
        const blink::WebWindowFeatures& features,
        const blink::WebString& name,
        blink::WebNavigationPolicy policy,
        bool suppressOpener);

    BrowserImpl* m_browserImpl;
    blink::WebPage* m_webPage;

    int m_windowlessFrameRate;
};

} // cef

#endif // BrowserHostImpl_h
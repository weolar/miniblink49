#ifndef FrameImpl_h
#define FrameImpl_h

namespace blink {
class WebPage;
class WebLocalFrameImpl;
class WebFrame;
}

typedef _cef_browser_t cef_browser_t;
typedef _cef_v8context_t cef_v8context_t;

namespace cef {

class BrowserImpl;

class FrameImpl : public CefCppBase < cef_frame_t, FrameImpl > {
public:
    FrameImpl(BrowserImpl* browser, content::WebPage* webPage, blink::WebFrame* webLocalFrameImpl);
    void willDeleted();

    static int CEF_CALLBACK is_valid(cef_frame_t* self);
    static void CEF_CALLBACK undo(cef_frame_t* self);
    static void CEF_CALLBACK redo(cef_frame_t* self);
    static void CEF_CALLBACK cut(cef_frame_t* self);
    static void CEF_CALLBACK copy(cef_frame_t* self);
    static void CEF_CALLBACK paste(cef_frame_t* self);
    static void CEF_CALLBACK del(cef_frame_t* self);
    static void CEF_CALLBACK select_all(cef_frame_t* self);
    static void CEF_CALLBACK view_source(cef_frame_t* self);
    static void CEF_CALLBACK get_source(cef_frame_t* self, cef_string_visitor_t* visitor);
    static void CEF_CALLBACK get_text(cef_frame_t* self, cef_string_visitor_t* visitor);
    static void CEF_CALLBACK load_request(cef_frame_t* self, cef_request_t* request);
    static void CEF_CALLBACK load_url(cef_frame_t* self, const cef_string_t* url);
    static void CEF_CALLBACK load_string(cef_frame_t* self, const cef_string_t* string_val, const cef_string_t* url);
    static void CEF_CALLBACK execute_java_script(cef_frame_t* self, const cef_string_t* code, const cef_string_t* script_url, int start_line);
    static int CEF_CALLBACK is_main(cef_frame_t* self);
    static int CEF_CALLBACK is_focused(cef_frame_t* self);
    static cef_string_userfree_t CEF_CALLBACK get_name(cef_frame_t* self);
    static int64 CEF_CALLBACK get_identifier(cef_frame_t* self);
    static cef_frame_t* CEF_CALLBACK get_parent(cef_frame_t* self);
    static cef_string_userfree_t CEF_CALLBACK get_url(cef_frame_t* self);
    static cef_browser_t* CEF_CALLBACK get_browser(cef_frame_t* self);
    static cef_v8context_t* CEF_CALLBACK get_v8context(cef_frame_t* self);
    static void CEF_CALLBACK visit_dom(cef_frame_t* self, cef_domvisitor_t* visitor);

    blink::WebFrame* m_webLocalFrameImpl;
    BrowserImpl* m_browserImpl;
    content::WebPage* m_webPage;
};

} // cef

#endif // BrowserHostImpl_h
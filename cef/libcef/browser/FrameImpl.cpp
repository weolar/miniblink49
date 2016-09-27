#include "include/internal/cef_export.h"
#include "include/capi/cef_base_capi.h"
#include "include/capi/cef_browser_capi.h"
#include "include/capi/cef_client_capi.h"
#include "include/capi/cef_v8_capi.h"

#include "libcef/common/CommonBase.h"
#include "libcef/browser/FrameImpl.h"
#include "libcef/browser/BrowserImpl.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include "third_party/WebKit/Source/core/frame/Frame.h"
#include "content/browser/WebPage.h"

#undef CEF_EXPORT
#define CEF_EXPORT extern "C" __declspec(dllexport) 

extern const WCHAR* szWindowClass;
static ATOM registerClass(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

namespace cef {

int FrameImpl::is_valid(cef_frame_t* self) { DebugBreak(); return 0; }
void FrameImpl::undo(cef_frame_t* self) { DebugBreak(); }
void FrameImpl::redo(cef_frame_t* self) { DebugBreak(); }
void FrameImpl::cut(cef_frame_t* self) { DebugBreak(); }
void FrameImpl::copy(cef_frame_t* self) { DebugBreak(); }
void FrameImpl::paste(cef_frame_t* self) { DebugBreak(); }
void FrameImpl::del(cef_frame_t* self) { DebugBreak(); }
void FrameImpl::select_all(cef_frame_t* self) { DebugBreak(); }
void FrameImpl::view_source(cef_frame_t* self) { DebugBreak(); }
void FrameImpl::get_source(cef_frame_t* self, cef_string_visitor_t* visitor) { DebugBreak(); }
void FrameImpl::get_text(cef_frame_t* self, cef_string_visitor_t* visitor) { DebugBreak(); }
void FrameImpl::load_request(cef_frame_t* self, cef_request_t* request) { DebugBreak(); }

void FrameImpl::load_url(cef_frame_t* s, const cef_string_t* url)
{
    CEF_CHECK_ARGS_CAST_NORET(FrameImpl);

    blink::WebURL webURL(blink::KURL(blink::ParsedURLString, WTF::String(url->str, url->length).utf8().data()));
    blink::WebURLRequest request(webURL);
    self->m_webLocalFrameImpl->loadRequest(request);
}

void FrameImpl::load_string(cef_frame_t* self, const cef_string_t* string_val, const cef_string_t* url) { DebugBreak(); }
void FrameImpl::execute_java_script(cef_frame_t* self, const cef_string_t* code, const cef_string_t* script_url, int start_line) { DebugBreak(); }

int FrameImpl::is_main(cef_frame_t* s)
{
    CEF_CHECK_ARGS_CAST(FrameImpl, false);

    blink::Frame* frame = blink::toCoreFrame(self->m_webLocalFrameImpl);
    if (frame)
        return frame->isMainFrame();
    return false;
}


int FrameImpl::is_focused(cef_frame_t* self) { DebugBreak(); return 0; }

cef_string_userfree_t FrameImpl::get_name(cef_frame_t* s)
{
    CEF_CHECK_ARGS_CAST(FrameImpl, nullptr);

    String uniqueName = (String)self->m_webLocalFrameImpl->uniqueName();
    Vector<UChar> uniqueNameStr = uniqueName.charactersWithNullTermination();
    if (0 == uniqueNameStr.size())
        return nullptr;

    cef_string_userfree_utf16_t name = cef_string_userfree_utf16_alloc();
    cef_string_utf16_set(uniqueNameStr.data(), uniqueNameStr.size(), name, true);

    return name;
}

int64 FrameImpl::get_identifier(cef_frame_t* s)
{
    CEF_CHECK_ARGS_CAST(FrameImpl, -1);

    blink::Frame* frame = blink::toCoreFrame(self->m_webLocalFrameImpl);
    if (frame)
        return frame->frameID();
    return -1;
}

cef_frame_t* FrameImpl::get_parent(cef_frame_t* self) { DebugBreak(); return nullptr; }
cef_string_userfree_t FrameImpl::get_url(cef_frame_t* self) { DebugBreak(); return nullptr; }
cef_browser_t* FrameImpl::get_browser(cef_frame_t* self) { DebugBreak(); return nullptr; }
cef_v8context_t* FrameImpl::get_v8context(cef_frame_t* self) { DebugBreak(); return nullptr; }
void FrameImpl::visit_dom(cef_frame_t* self, cef_domvisitor_t* visitor) { DebugBreak(); return; }

FrameImpl::FrameImpl(BrowserImpl* browser, blink::WebPage* webPage, blink::WebFrame* webLocalFrameImpl)
{
    memset(&m_baseClass, 0, sizeof(m_baseClass));
    m_baseClass.is_valid = is_valid;
    m_baseClass.undo = undo;
    m_baseClass.redo = redo;
    m_baseClass.cut = cut;
    m_baseClass.copy = copy;
    m_baseClass.paste = paste;
    m_baseClass.del = del;
    m_baseClass.select_all = select_all;
    m_baseClass.view_source = view_source;
    m_baseClass.get_source = get_source;
    m_baseClass.get_text = get_text;
    m_baseClass.load_request = load_request;
    m_baseClass.load_url = load_url;
    m_baseClass.load_string = load_string;
    m_baseClass.execute_java_script = execute_java_script;
    m_baseClass.is_main = is_main;
    m_baseClass.is_focused = is_focused;
    m_baseClass.get_name = get_name;
    m_baseClass.get_identifier = get_identifier;
    m_baseClass.get_parent = get_parent;
    m_baseClass.get_url = get_url;
    m_baseClass.get_browser = get_browser;
    m_baseClass.get_v8context = get_v8context;
    m_baseClass.visit_dom = visit_dom;
    CefCppBase<cef_frame_t, FrameImpl>::init(this);

    m_browserImpl = browser;
    m_webPage = webPage;
    m_webLocalFrameImpl = webLocalFrameImpl;
}

void FrameImpl::willDeleted()
{

}

} // cef
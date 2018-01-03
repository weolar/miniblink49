#ifndef CEF_LIBCEF_BROWSER_FRAME_HOST_IMPL_H_
#define CEF_LIBCEF_BROWSER_FRAME_HOST_IMPL_H_

#include "include/cef_frame.h"

#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"

class CefBrowserHostImpl;
struct Cef_Response_Params;

namespace blink {
class WebLocalFrame;
class WebFrame;
}

// Implementation of CefFrame. CefFrameHostImpl objects are owned by the
// CefBrowerHostImpl and will be detached when the browser is notified that the
// associated renderer WebFrame will close.
class CefFrameHostImpl : public CefFrame {
public:
//     CefFrameHostImpl(CefBrowserHostImpl* browser, blink::WebLocalFrame* frame);
//     CefFrameHostImpl(CefBrowserHostImpl* browser, blink::WebLocalFrame* frame, /*const CefString& url,*/ const CefString& name);
    CefFrameHostImpl(CefBrowserHostImpl* browser,
        int64 frameId,
        bool isMainFrame,
        const CefString& url,
        const CefString& name,
        int64 parentFrameId);
    ~CefFrameHostImpl() override {}

    // CefFrame methods
    bool IsValid() override;
    void Undo() override;
    void Redo() override;
    void Cut() override;
    void Copy() override;
    void Paste() override;
    void Delete() override;
    void SelectAll() override;
    void ViewSource() override;
    void GetSource(CefRefPtr<CefStringVisitor> visitor) override;
    void GetText(CefRefPtr<CefStringVisitor> visitor) override;
    void LoadRequest(CefRefPtr<CefRequest> request) override;
    void LoadURL(const CefString& url) override;
    void LoadString(const CefString& string,
        const CefString& url) override;
    void ExecuteJavaScript(const CefString& jsCode,
        const CefString& scriptUrl,
        int startLine) override;
    bool IsMain() override;
    bool IsFocused() override;
    CefString GetName() override;
    int64 GetIdentifier() override;
    CefRefPtr<CefFrame> GetParent() override;
    CefString GetURL() override;
    CefRefPtr<CefBrowser> GetBrowser() override;
    CefRefPtr<CefV8Context> GetV8Context() override;
    void VisitDOM(CefRefPtr<CefDOMVisitor> visitor) override;

    void SetFocused(bool focused);
    void SetAttributes(const CefString& url,
        const CefString& name,
        int64 parent_frame_id);

    // Avoids unnecessary string type conversions.
    void SendJavaScript(const std::string& jsCode,
        const std::string& scriptUrl,
        int startLine);

    // Detach the frame from the browser.
    void Detach() {}

    static int64 GetFrameIdByBlinkFrame(const blink::WebLocalFrame* frame);
    static int64 GetFrameIdByBlinkFrame(const blink::WebFrame* frame);
    static WTF::String GetFrameNameByBlinkFrame(const blink::WebLocalFrame* frame);
    
protected:
    void Init(CefBrowserHostImpl* browser, blink::WebLocalFrame* frame, /*const CefString& url,*/ const CefString& name);
    void SendCommand(const WTF::String& commandName, Cef_Response_Params* response);

    int64 m_frameId;
    bool m_isMainFrame;
    //blink::WebLocalFrame* m_frame;

    // Volatile state information. All access must be protected by the state lock.
    //base::Lock m_stateLock;
    WTF::Mutex m_stateLock;
    CefBrowserHostImpl* m_browser;
    bool m_isFocused;
    CefString m_url;
    CefString m_names;
    int64 m_parentFrameId;

    IMPLEMENT_REFCOUNTING(CefFrameHostImpl);
    DISALLOW_EVIL_CONSTRUCTORS(CefFrameHostImpl);
};

#endif // CEF_LIBCEF_BROWSER_FRAME_HOST_IMPL_H_
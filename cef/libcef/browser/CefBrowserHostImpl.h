
#ifndef BrowserHostImpl_h
#define BrowserHostImpl_h

#include "include/cef_browser.h"
#include "include/cef_app.h"
#include "include/cef_client.h"
#include "libcef/browser/CefBrowserInfo.h"
#include "third_party/WebKit/Source/platform/weborigin/Referrer.h"
#include "third_party/WebKit/public/web/WebHistoryCommitType.h"
#include "libcef/browser/CefFrameHostImpl.h"

namespace blink {
class WebFrame;
class WebHistoryItem;
class WebLocalFrame;
struct WebURLError;
class KURL;
}

namespace content {
class WebPage;
}

struct Cef_Request_Params;
struct Cef_Response_Params;
struct CreateBrowserHostWindowArgs;

class CefBrowserHostImpl 
    : public CefBrowserHost
    , public CefBrowser {
public:
    CefBrowserHostImpl(
        const CefBrowserSettings& settings,
        CefRefPtr<CefClient> client,
        scoped_refptr<CefBrowserInfo> browser_info,
        CefRefPtr<CefBrowserHostImpl> opener,
        CefRefPtr<CefRequestContext> request_context);
    ~CefBrowserHostImpl() override;

    // Create a new CefBrowserHostImpl instance.
    static CefRefPtr<CefBrowserHostImpl> Create(
        const CefWindowInfo& windowInfo,
        CefRefPtr<CefClient> client,
        const CefString& url,
        const CefBrowserSettings& settings,
        CefRefPtr<CefBrowserHostImpl> opener,
        bool is_popup,
        CefRefPtr<CefRequestContext> request_context);

    bool CreateHostWindow(const CefWindowInfo& windowInfo);

    // CefBrowserHost methods.
    virtual CefRefPtr<CefBrowser> GetBrowser() override;
    virtual void CloseBrowser(bool forceClose) override;
    virtual void SetFocus(bool focus) override;
    virtual void SetWindowVisibility(bool visible) override {}
    virtual CefWindowHandle GetWindowHandle() override;
    virtual CefWindowHandle GetOpenerWindowHandle() override { return nullptr; }
    virtual CefRefPtr<CefClient> GetClient() override { return m_client; }
    virtual CefRefPtr<CefRequestContext> GetRequestContext() override { return m_requestContext; }
    virtual double GetZoomLevel() override { return 0; }
    virtual void SetZoomLevel(double zoomLevel) override {}
    virtual void RunFileDialog(
        FileDialogMode mode,
        const CefString& title,
        const CefString& default_file_path,
        const std::vector<CefString>& accept_filters,
        int selected_accept_filter,
        CefRefPtr<CefRunFileDialogCallback> callback) override {}
    virtual void StartDownload(const CefString& url) override {}
    virtual void Print() override {}
    virtual void PrintToPDF(const CefString& path,
        const CefPdfPrintSettings& settings,
        CefRefPtr<CefPdfPrintCallback> callback) override {}
    virtual void Find(int identifier, const CefString& searchText,
        bool forward, bool matchCase, bool findNext) override {}
    virtual void StopFinding(bool clearSelection) override {}
    virtual void ShowDevTools(const CefWindowInfo& windowInfo,
        CefRefPtr<CefClient> client,
        const CefBrowserSettings& settings,
        const CefPoint& inspect_element_at) override {}
    virtual void CloseDevTools() override {}
    virtual void GetNavigationEntries(
        CefRefPtr<CefNavigationEntryVisitor> visitor,
        bool current_only) override {}
    virtual void SetMouseCursorChangeDisabled(bool disabled) override;
    virtual bool IsMouseCursorChangeDisabled() override;
    virtual bool IsWindowRenderingDisabled() override { return false; }
    virtual void ReplaceMisspelling(const CefString& word) override {}
    virtual void AddWordToDictionary(const CefString& word) override {}
    virtual void WasResized() override;
    virtual void WasHidden(bool hidden) override;
    virtual void NotifyScreenInfoChanged() override {}
    virtual void Invalidate(PaintElementType type) override {}
    virtual void SendKeyEvent(const CefKeyEvent& event) override;
    virtual void SendMouseClickEvent(const CefMouseEvent& event,
            MouseButtonType type,
            bool mouseUp, int clickCount) override;
    virtual void SendMouseMoveEvent(const CefMouseEvent& event,
            bool mouseLeave) override;
    virtual void SendMouseWheelEvent(const CefMouseEvent& event,
            int deltaX, int deltaY) override;
    virtual void SendFocusEvent(bool setFocus) override;
    virtual void SendCaptureLostEvent() override;
    virtual void NotifyMoveOrResizeStarted() override {}
    virtual int GetWindowlessFrameRate() override { return 10; }
    virtual void SetWindowlessFrameRate(int frame_rate) override {}
    virtual CefTextInputContext GetNSTextInputContext() override { return nullptr; }
    virtual void HandleKeyEventBeforeTextInputClient(CefEventHandle keyEvent) override {}
    virtual void HandleKeyEventAfterTextInputClient(CefEventHandle keyEvent) override {}
    virtual void DragTargetDragEnter(CefRefPtr<CefDragData> drag_data,
        const CefMouseEvent& event,
        DragOperationsMask allowed_ops) override {}
    virtual void DragTargetDragOver(const CefMouseEvent& event,
        DragOperationsMask allowed_ops) override {}
    virtual void DragTargetDragLeave() override {}
    virtual void DragTargetDrop(const CefMouseEvent& event) override {}
    virtual void DragSourceSystemDragEnded() override {}
    virtual void DragSourceEndedAt(int x, int y, DragOperationsMask op) override {}

    // CefBrowser methods.
    virtual CefRefPtr<CefBrowserHost> GetHost() override;
    virtual bool CanGoBack() override;
    virtual void GoBack() override;
    virtual bool CanGoForward() override;
    virtual void GoForward() override;
    virtual bool IsLoading() override;
    virtual void Reload() override;
    virtual void ReloadIgnoreCache() override;
    virtual void StopLoad() override;
    virtual int GetIdentifier() override;
    virtual bool IsSame(CefRefPtr<CefBrowser> that) override;
    virtual bool IsPopup() override;
    virtual bool HasDocument() override;
    virtual CefRefPtr<CefFrame> GetMainFrame() override;
    virtual CefRefPtr<CefFrame> GetFocusedFrame() override;
    virtual CefRefPtr<CefFrame> GetFrame(int64 identifier) override;
    CefRefPtr<CefFrame> GetFrame(const CefString& name) override;
    virtual size_t GetFrameCount() override;
    virtual void GetFrameIdentifiers(std::vector<int64>& identifiers) override;
    virtual void GetFrameNames(std::vector<CefString>& names) override;
    virtual bool SendProcessMessage(CefProcessId target_process, CefRefPtr<CefProcessMessage> message) override;

    void OnPaintUpdated(const uint32_t* buffer, const CefRect& paintRect, int width, int height);

    void OnLoadingStateChange(bool isLoading, bool toDifferentDocument);

    void OnSetFocus(cef_focus_source_t source);

    void CancelContextMenu();

    // Returns true if windowless rendering is enabled.
    bool IsWindowless() const { return m_isWindowless; }

    // Called when the OS window hosting the browser is destroyed.
    void WindowDestroyed();

    // Destroy the browser members. This method should only be called after the
    // native browser window is not longer processing messages.
    void DestroyBrowser();

    void DetachAllFrames();

    // Load the specified URL.
    void LoadURL(int64 frameId, const CefString& url, const blink::Referrer& referrer, const CefString& extraHeaders);

    // Load the specified request.
    void LoadRequest(int64 frameId, CefRefPtr<CefRequest> request);

    // Load the specified string.
    void LoadString(int64 frameId, const CefString& string, const CefString& url);

    // Thread safe accessors.
    const CefBrowserSettings& settings() const { return m_settings; }
    CefRefPtr<CefClient> client() const { return m_client; }
    scoped_refptr<CefBrowserInfo> browserInfo() const { return m_browserInfo; }
    int browserId() const;

    bool FireHeartbeat();
    void SetNeedHeartbeat();
    void ClearNeedHeartbeat();

    enum DestructionState {
        DESTRUCTION_STATE_NONE = 0,
        DESTRUCTION_STATE_PENDING,
        DESTRUCTION_STATE_ACCEPTED,
        DESTRUCTION_STATE_COMPLETED
    };
    DestructionState GetDestructionState() const { return m_destructionState; }

    content::WebPage* webPage() const { return m_webPage; }

    void SendCommand(const Cef_Request_Params* request, Cef_Response_Params* response);

    static CefBrowserHostImpl* GetBrowserForMainFrame(blink::WebFrame* webFrame);

    void DidFinishLoad(blink::WebLocalFrame* frame);
    void DidFailLoad(blink::WebLocalFrame* frame, const blink::WebURLError& error, blink::WebHistoryCommitType type);
    void DidCommitProvisionalLoadForFrame(blink::WebLocalFrame* frame, const blink::WebHistoryItem&);
    void DidStartProvisionalLoad(blink::WebLocalFrame* localFrame, double triggeringEventTime);
    void DidFailProvisionalLoad(blink::WebLocalFrame* frame, const blink::WebURLError& error, blink::WebHistoryCommitType);

    void OnLoadStart(CefRefPtr<CefFrame> fram);
    void OnLoadError(CefRefPtr<CefFrame> frame, const blink::KURL& url, int errorCode, const WTF::String& errorDescription);
    void OnAddressChange(CefRefPtr<CefFrame> frame, const CefString& url);
    void OnTitleChange(blink::WebLocalFrame* frame, const String& title);
    void OnFrameIdentified(blink::WebLocalFrame* frame, blink::WebLocalFrame* parent);

    CefRefPtr<CefFrame> GetOrCreateFrame(const blink::WebLocalFrame* webFrame, int64 parentFrameId, const blink::KURL& frameUrl);

private:
    static CefRefPtr<CefBrowserHostImpl> CreateInternal(
        const CefWindowInfo& windowInfo,
        const CefBrowserSettings& settings,
        CefRefPtr<CefClient> client,
        scoped_refptr<CefBrowserInfo> browser_info,
        CefRefPtr<CefBrowserHostImpl> opener,
        CefRefPtr<CefRequestContext> requestContext);

    static void CreateAndLoadOnWebkitThread(CreateBrowserHostWindowArgs* args);

    static LONG __stdcall SubClassFunc(HWND hWnd, UINT Message, WPARAM wParam, LONG lParam);

    static void RegisterWindowClass();
    static LPCTSTR GetWndClass();
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void PlatformSetFocus(bool focus);
    void CloseHostWindow();

    content::WebPage* m_webPage;

    bool m_hasLMouseUp;
    bool m_hasRMouseUp;
    bool m_isWindowless;

    WNDPROC m_lpfnOldWndProc;

    CefScreenInfo m_screenInfo;

    CefBrowserSettings m_settings;
    CefRefPtr<CefClient> m_client;
    scoped_refptr<CefBrowserInfo> m_browserInfo;
    CefWindowHandle m_opener;
    CefRefPtr<CefRequestContext> m_requestContext;

    DestructionState m_destructionState;

    bool m_isInOnsetfocus;

    WTF::Mutex m_stateLock;
    bool m_mouseCursorChangeDisabled;

    bool m_windowDestroyed;

    bool m_isLoading;

    typedef std::map<int64, CefRefPtr<CefFrameHostImpl> > FrameMap;
    FrameMap m_frames;
    int64 m_mainFrameId;
    int64 m_focusedFrameId;

    bool m_frameDestructionPending;

    int64 m_identifier;
	
    IMPLEMENT_REFCOUNTING(CefBrowserHostImpl);
    DISALLOW_COPY_AND_ASSIGN(CefBrowserHostImpl);
};



#endif // BrowserHostImpl_h
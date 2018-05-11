
#ifndef COMMONHANDLER_H
#define COMMONHANDLER_H

#include "include/cef_client.h"
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_runnable.h"

// MyHandler implements CefClient and a number of other interfaces.

class RenderHandler : public CefRenderHandler {
public:
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) = 0;
};
class CommonHandler : public CefClient,
    public RenderHandler,
    public CefContextMenuHandler,
    public CefDisplayHandler,
    public CefDownloadHandler,
    public CefDragHandler,
    public CefGeolocationHandler,
    public CefKeyboardHandler,
    public CefLifeSpanHandler,
    public CefLoadHandler,
    public CefRequestHandler {
public:
    CommonHandler() {}
public:
    HWND m_hwnd;

    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    }

    virtual void  OnBeforeClose(CefRefPtr<CefBrowser> browser)OVERRIDE {

    }

    // CefClient methods. Important to return |this| for the handler callbacks.
    virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE {
        return this;
    }

    virtual CefRefPtr<CefRenderHandler>  GetRenderHandler() OVERRIDE {
        return this;
    }

    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
        return this;
    }
    virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() OVERRIDE {
        return this;
    }
    virtual CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE {
        return this;
    }
    virtual CefRefPtr<CefGeolocationHandler> GetGeolocationHandler() OVERRIDE {
        return this;
    }
    virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE {
        return this;
    }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
        return this;
    }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
        return this;
    }
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE {
        return this;
    }
    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message)
        OVERRIDE {
        return true;
        // Handle IPC messages from the render process...
    }

    // CefRenderHandler methods
    virtual bool GetRootScreenRect(CefRefPtr<CefBrowser> browser,
        CefRect& rect) OVERRIDE {
        RECT window_rect = { 0 };
        HWND root_window = GetAncestor(m_hwnd, GA_ROOT);
        if (::GetWindowRect(root_window, &window_rect)) {
            rect = CefRect(window_rect.left,
                window_rect.top,
                window_rect.right - window_rect.left,
                window_rect.bottom - window_rect.top);
            return true;
        }
        return false;
    }
    virtual bool GetViewRect(CefRefPtr<CefBrowser> browser,
        CefRect& rect) OVERRIDE {
        RECT clientRect;
        if (!::GetClientRect(m_hwnd, &clientRect))
            return false;
        rect.x = rect.y = 0;
        rect.width = clientRect.right;
        rect.height = clientRect.bottom;
        return true;
    }
    virtual bool GetScreenPoint(CefRefPtr<CefBrowser> browser,
        int viewX,
        int viewY,
        int& screenX,
        int& screenY) OVERRIDE {
        if (!::IsWindow(m_hwnd))
            return false;

        // Convert the point from view coordinates to actual screen coordinates.
        POINT screen_pt = { viewX, viewY };
        ClientToScreen(m_hwnd, &screen_pt);
        screenX = screen_pt.x;
        screenY = screen_pt.y;
        return true;
    }
    virtual void OnPopupShow(CefRefPtr<CefBrowser> browser,
        bool show) OVERRIDE {

    }
    virtual void OnPopupSize(CefRefPtr<CefBrowser> browser,
        const CefRect& rect) OVERRIDE {

    }
    virtual void OnPaint(CefRefPtr<CefBrowser> browser,
        PaintElementType type,
        const RectList& dirtyRects,
        const void* buffer,
        int width,
        int height) OVERRIDE {
        HBITMAP bitmap = CreateBitmap(width, height, 1, 32, buffer);
        HDC dc = CreateCompatibleDC(NULL);

    }
    virtual void OnCursorChange(CefRefPtr<CefBrowser> browser,
        CefCursorHandle cursor,
        CefRenderHandler::CursorType type,
        const CefCursorInfo& custom_cursor_info) OVERRIDE {
        SetClassLongPtr(m_hwnd, GCLP_HCURSOR,
            static_cast<LONG>(reinterpret_cast<LONG_PTR>(cursor)));
        SetCursor(cursor);
    }

    // CefContextMenuHandler methods
    virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefContextMenuParams> params,
        CefRefPtr<CefMenuModel> model) OVERRIDE {

        // Customize the context menu...
    }
    virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefContextMenuParams> params,
        int command_id,
        EventFlags event_flags) OVERRIDE {
        return true;
        // Handle a context menu command...
    }

    // CefDisplayHandler methods
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
        bool isLoading,
        bool canGoBack,
        bool canGoForward) OVERRIDE {
        // Update UI for browser state...
    }
    virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& url) OVERRIDE {
        // Update the URL in the address bar...
    }
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
        const CefString& title) OVERRIDE {
        // Update the browser window title...
    }
    virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
        const CefString& message,
        const CefString& source,
        int line) OVERRIDE {
        return true;
        // Log a console message...
    }

    // CefDownloadHandler methods
    virtual void OnBeforeDownload(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefDownloadItem> download_item,
        const CefString& suggested_name,
        CefRefPtr<CefBeforeDownloadCallback> callback) OVERRIDE {
        // Specify a file path or cancel the download...
    }
    virtual void OnDownloadUpdated(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefDownloadItem> download_item,
        CefRefPtr<CefDownloadItemCallback> callback) OVERRIDE {
        // Update the download status...
    }

    // CefDragHandler methods
    virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefDragData> dragData,
        CefBrowserHost::DragOperationsMask mask) OVERRIDE {
        // Allow or deny drag events...
        return true;
    }

    // CefGeolocationHandler methods
//     virtual void OnRequestGeolocationPermission(
//         CefRefPtr<CefBrowser> browser,
//         const CefString& requesting_url,
//         int request_id,
//         CefRefPtr<CefGeolocationCallback> callback) OVERRIDE {
//         // Allow or deny geolocation API access...
//     }

    // CefKeyboardHandler methods
    virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
        const CefKeyEvent& event,
        CefEventHandle os_event,
        bool* is_keyboard_shortcut) OVERRIDE {
        // Perform custom handling of key events...
        return false;
    }

    // CefLifeSpanHandler methods
//     virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
//         CefRefPtr<CefFrame> frame,
//         const CefString& target_url,
//         const CefString& target_frame_name,
//         const CefPopupFeatures& popupFeatures,
//         CefWindowInfo& windowInfo,
//         CefRefPtr<CefClient>& client,
//         CefBrowserSettings& settings,
//         bool* no_javascript_access) OVERRIDE {
//         // Allow or block popup windows, customize popup window creation...
//         return false;
//     }

    virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE {
        return false;
        // Allow or block browser window close...
    }
    //virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE {
    //	// Browser window is closed, perform cleanup...
    //}

    // CefLoadHandler methods
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame) OVERRIDE {
        // A frame has started loading content...
    }
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        int httpStatusCode) OVERRIDE {
        // A frame has finished loading content...
    }
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        ErrorCode errorCode,
        const CefString& errorText,
        const CefString& failedUrl) OVERRIDE {
        // A frame has failed to load content...
    }
    virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
        TerminationStatus status) OVERRIDE {
        // A render process has crashed...
    }

    // CefRequestHandler methods
    virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request) OVERRIDE {
        return 0;
        // Optionally intercept resource requests...
    }
//     virtual bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
//         const CefString& origin_url,
//         int64 new_size,
//         CefRefPtr<CefQuotaCallback> callback) OVERRIDE {
//         return true;
//         // Allow or block quota requests...
//     }
    virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
        const CefString& url,
        bool& allow_os_execution) OVERRIDE {
        // Handle execution of external protocols...
    }

    IMPLEMENT_REFCOUNTING(CommonHandler);
};

#endif

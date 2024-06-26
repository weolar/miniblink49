
#ifndef PdfViewerPlugin_h
#define PdfViewerPlugin_h

#include "wke/wkedefine.h"
#include "common/BindJsQuery.h"
#include <string>
#include <vector>

#include "third_party/npapi/bindings/npapi.h"
#include "third_party/npapi/bindings/npfunctions.h"
#include "third_party/npapi/bindings/npruntime.h"

typedef struct NPObject NPObject;
typedef struct _NPP NPP_t;
typedef NPP_t* NPP;
typedef struct _NPWindow NPWindow;
typedef unsigned __int32 uint32_t;

namespace printing {

class PdfViewerPlugin {
public:
    PdfViewerPlugin(wkeWebView parentWebview, const char* url);
    ~PdfViewerPlugin();

    int getGuestId() const { return m_guestId; }
    void setGuestId(int id) { m_guestId = id; }

    void setNpObj(NPObject* npObj) { m_npObj = npObj; }
    NPObject* getNpObj() const { return m_npObj; }

    wkeWebView getWebview() const { return m_webview; }

    void setInstance(NPP instance) { m_instance = instance; }
    NPP getInstance() const { return m_instance; }

    void onSize(const WINDOWPOS& windowpos);
    void onPaint(HDC hdc);
    void onSetWinow(const NPWindow& npWindow);

    void onMouseEvt(uint32_t message, uint32_t wParam, uint32_t lParam);
    void onKey(uint32_t message, uint32_t wParam, uint32_t lParam);

    std::string getURL();

    void setPreloadURL(const std::string& preloadURL);
    
    void resetPdf(std::vector<char>* pdfData);

    int getPageCount() const
    {
        return m_pageCount;
    }

    std::vector<char>* getPdfData() const
    {
        return m_pdfData;
    }

private:
    void onJsQuery(mbJsExecState es, int64_t queryId, int customMsg, const utf8* request);

    static void staticOnPaintUpdated(wkeWebView webView, int id, const HDC hdc, int x, int y, int cx, int cy);
    static void staticDidCreateScriptContextCallback(wkeWebView webView, int id, wkeWebFrameHandle frame, void* context, int extensionGroup, int worldId);
    static void staticOnWillReleaseScriptContextCallback(wkeWebView webView, int id, wkeWebFrameHandle frame, void* context, int worldId);

    static void onDocumentReady(wkeWebView webView, int id);
    static void onLoadingFinishCallback(wkeWebView webView, int id, const wkeString url, wkeLoadingResult result, const wkeString failedReason);
    static void onOtherLoadCallback(wkeWebView webView, int id, wkeOtherLoadType type, wkeTempCallbackInfo* info);
    static bool onLoadUrlBegin(wkeWebView webView, int id, const char* url, wkeNetJob job);

    void onPaintUpdatedInCompositeThread(HWND hWnd, const HDC hdc, int x, int y, int cx, int cy);
    void onPrePaintUpdatedInCompositeThread(int id, HWND hWnd, const HDC hdc, int x, int y, int cx, int cy);

    void onSavePdf();

    bool onLoadUrlBeginImpl(wkeWebView webView, const char* url, wkeNetJob job);
    void resetPdfImpl();
    void clearPdfDatas();

    void loadPreloadURL();

    bool onCreatePreviewBitmap(wkeWebView webView, const char* url, wkeNetJob job);

    wkeWebView m_parentWebview;
    int m_parentId;
    NPObject* m_npObj;
    NPWindow m_npWindow;
    NPP m_instance;

    CRITICAL_SECTION m_memoryCanvasLock;
    HBITMAP m_memoryBMP;
    HDC m_memoryDC;
    HWND m_parentHWND;
    bool m_clientSizeDirty;

    wkeWebView m_webview;
    int64_t m_id;
    int m_guestId;

    std::string* m_preloadcode;

    std::vector<char>* m_pdfData;

    struct PdfPageInfo {
        int width;
        int height;
    };
    std::vector<PdfPageInfo> m_pdfPageInfos;

    common::BindJsQuery::QueryFn* m_closure;

    WINDOWPOS m_windowPos;
    bool m_docReady;
    bool m_needRender;
    int m_pageCount;

    std::string m_url;
};

}

#endif // PdfViewerPlugin_h

#include "printing/PdfViewerPlugin.h"
#include "printing/PdfiumLoad.h"
#include "printing/PrintingUtil.h"
#include "printing/PdfViewerHtml.h"
#include "common/LiveIdDetect.h"
#include "common/ThreadCall.h"
#include "common/StringUtil.h"
#include "common/UrlUtil.h"
#include "content/browser/PostTaskHelper.h"
#include "third_party/WebKit/Source/wtf/MainThread.h"
#include <process.h>

BOOL wkeUtilPrint(wkeWebView webviewHandle, wkeWebFrameHandle frameId, const wkePrintSettings* settings);

namespace printing {

extern NPNetscapeFuncs* g_npBrowserFunctions;

void PdfViewerPlugin::staticDidCreateScriptContextCallback(wkeWebView webView, int id, wkeWebFrameHandle frame, void* context, int extensionGroup, int worldId)
{
    PdfViewerPlugin* self = (PdfViewerPlugin*)common::LiveIdDetect::get()->getPtrLocked((int64_t)id);
    if (!self)
        return;
    common::BindJsQuery::bindFun(self->m_id, self->m_closure, webView, frame);
    self->loadPreloadURL();
    common::LiveIdDetect::get()->unlock(id, self);
}

void PdfViewerPlugin::onJsQuery(mbJsExecState es, int64_t queryId, int customMsg, const utf8* request)
{
    if (1 == customMsg) {
        onSavePdf();
    }
}

void PdfViewerPlugin::staticOnWillReleaseScriptContextCallback(wkeWebView webView, int id, wkeWebFrameHandle frame, void* context, int worldId)
{

}

void PdfViewerPlugin::onLoadingFinishCallback(wkeWebView webView, int id, const wkeString url, wkeLoadingResult result, const wkeString failedReason)
{
    
}

struct SavePdfThreadData {
    SavePdfThreadData(const std::vector<char>* pdfData, const std::wstring& url)
    {
        this->pdfData = *pdfData;
        this->url = url;
    }

    std::vector<char> pdfData;
    std::wstring url;
};

static int g_savePdfCount = 0;

static unsigned int __stdcall savePdfThread(void* param)
{
    SavePdfThreadData* data = (SavePdfThreadData*)param;
    
    OPENFILENAMEW ofn = { 0 };
    std::vector<wchar_t> fileResult;
    fileResult.resize(4 * MAX_PATH + 1);

    std::wstring defaultSaveName = data->url;
    if (defaultSaveName.size() > 150)
        defaultSaveName = defaultSaveName.substr(0, 150);
    if (0 == defaultSaveName.size())
        defaultSaveName = L"unknow.pdf";
    wcscpy(&fileResult.at(0), defaultSaveName.c_str());

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = &fileResult.at(0);
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"All\0*.*\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;

    if (!::GetSaveFileNameW(&ofn)) {
        InterlockedDecrement((long *)&g_savePdfCount);
        delete data;
        return 0;
    }

    HANDLE hFile = ::CreateFileW(&fileResult.at(0), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        ::MessageBoxW(nullptr, L"打开文件失败", L"失败", 0);
        InterlockedDecrement((long *)&g_savePdfCount);
        delete data;
        return 0;
    }

    DWORD numberOfBytesWrite = 0;
    BOOL b = ::WriteFile(hFile, data->pdfData.data(), (DWORD)data->pdfData.size(), &numberOfBytesWrite, nullptr);
    ::CloseHandle(hFile);

    std::wstring title = L"文件成功保存在：";
    title += &fileResult.at(0);
    ::MessageBoxW(nullptr, title.c_str(), L"成功", 0);

    InterlockedDecrement((long *)&g_savePdfCount);
    delete data;
    return 0;
}

void PdfViewerPlugin::onSavePdf()
{
    if (0 < g_savePdfCount)
        return;
    InterlockedIncrement((long *)&g_savePdfCount);

    std::string url = m_url;
    std::wstring urlW = UrlUtil::getSaveNameFromUrl(url);
    SavePdfThreadData* data = new SavePdfThreadData(m_pdfData, urlW);

    unsigned int threadId = 0;
    HANDLE threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, savePdfThread, data, 0, &threadId));
    ::CloseHandle(threadHandle);
}

void PdfViewerPlugin::onDocumentReady(wkeWebView webView, int id)
{
    if (!PdfiumLoad::get())
        return;

    PdfViewerPlugin* self = (PdfViewerPlugin*)common::LiveIdDetect::get()->getPtr((int64_t)id);
    if (!self)
        return;
    self->m_docReady = true;
    self->resetPdfImpl();
}

void PdfViewerPlugin::onOtherLoadCallback(wkeWebView webView, int id, wkeOtherLoadType type, wkeTempCallbackInfo* info)
{

}

bool PdfViewerPlugin::onLoadUrlBegin(wkeWebView webView, int id, const char* url, wkeNetJob job)
{
    PdfViewerPlugin* self = (PdfViewerPlugin*)common::LiveIdDetect::get()->getPtrLocked((int64_t)id);
    if (!self)
        return false;

    bool b = self->onLoadUrlBeginImpl(webView, url, job);
    common::LiveIdDetect::get()->unlock(id, self);
    return b;
}

static bool parseWidthHeightIndexFromUrl(const char* url, int* index, int* width, int* height)
{
    const char token[] = "pdfviewer://img_src_";
    std::string urlStr(url);
    size_t pos = urlStr.find(token);
    if (-1 == pos)
        return false;

    std::string temp = urlStr.substr(sizeof(token) - 1);
    size_t pos2 = temp.find('_', pos);
    if (-1 == pos2)
        return false;
    std::string indexStr = temp.substr(pos, pos2 - pos);
    pos2++;

    size_t pos3 = temp.find('_', pos2);
    if (-1 == pos3)
        return false;
    std::string widthStr = temp.substr(pos2, pos3 - pos2);
    pos3++;

    size_t pos4 = temp.find('?', pos3);
    if (-1 == pos4)
        return false;
    std::string heightStr = temp.substr(pos3, pos4 - pos3);

    *index = atoi(indexStr.c_str());
    *width = atoi(widthStr.c_str());
    *height = atoi(heightStr.c_str());

    return true;
}

bool PdfViewerPlugin::onCreatePreviewBitmap(wkeWebView webView, const char* url, wkeNetJob job)
{
    int index = 0;
    int imgWidth = 0;
    int imgHeight = 0;

    if (!m_pdfData)
        return false;

    if (!parseWidthHeightIndexFromUrl(url, &index, &imgWidth, &imgHeight))
        return false;

    if (index >= (int)m_pdfPageInfos.size())
        return false;
    
    const void* pdfData = &m_pdfData->at(0);
    size_t pdfDataSize = m_pdfData->size();

    HWND hWnd = wkeGetHostHWND(webView);
    HDC refDeviceContext = ::GetDC(hWnd);
    int screenDpi = ::GetDeviceCaps(refDeviceContext, LOGPIXELSX);
    ::ReleaseDC(hWnd, refDeviceContext);

    PdfPageInfo info = m_pdfPageInfos[index];

    int pdfWidthInPx = (int)convertUnitDouble(info.width, kPointsPerInch, screenDpi);
    int pdfHeightInPx = (int)convertUnitDouble(info.height, kPointsPerInch, screenDpi);

    PrintSettings settings = { screenDpi, { imgWidth, imgHeight }, { 0, 0, imgWidth, imgHeight },{ imgWidth, imgHeight } };
    
    RenderSettings renderSettings = { screenDpi, false, false, false, false, false };
    renderSettings.fitToBounds = true;
    renderSettings.centerInBounds = true;
    renderSettings.stretchToBounds = true;
    renderSettings.keepAspectRatio = true;

    std::vector<char>* bitmap = PrintingUtil::renderPdfPageToBitmap(NULL_WEBVIEW, hWnd, index, settings, renderSettings, pdfData, pdfDataSize);
    if (bitmap) {
        //mbNetSetData(job, bitmap->data(), (int)bitmap->size());
        RELEASE_ASSERT(WTF::isMainThread());
        wkeNetSetData(job, bitmap->data(), (int)bitmap->size());
        delete bitmap;
    }

    return true;
}

bool PdfViewerPlugin::onLoadUrlBeginImpl(wkeWebView webView, const char* url, wkeNetJob job)
{
    wkeSetResourceGc(webView, 3);
    return onCreatePreviewBitmap(webView, url, job);
}

static void WKE_CALL_TYPE onPrintPdfCallback(wkeWebView webView, void* param, wkeWebFrameHandle frameId, void* printParams)
{
    int64_t id = (int64_t)param;
    if (!PdfiumLoad::get())
        return;

    int64_t mbViewId = (int64_t)wkeGetUserKeyValue(webView, "MbWebView");
    mb::MbWebView* mbView = (mb::MbWebView*)common::LiveIdDetect::get()->getPtrLocked((int64_t)mbViewId);
    if (!mbView) {
        wkeUtilPrint(webView, frameId, nullptr);
        return;
    }

    content::postTaskToUiThread(FROM_HERE, nullptr, [mbViewId, mbView, frameId] {
        if (!common::LiveIdDetect::get()->isLive(mbViewId))
            return;
        mbUtilPrint((mbWebView)mbViewId, (mbWebFrameHandle)frameId, nullptr);
    });
    common::LiveIdDetect::get()->unlock(id, mbView);
}

PdfViewerPlugin::PdfViewerPlugin(wkeWebView parentWebview, const char* url)
{
    m_id = common::LiveIdDetect::get()->constructed(this);
    m_preloadcode = nullptr;
    m_npObj = nullptr;
    m_instance = nullptr;
    m_closure = nullptr;
    m_pdfData = nullptr;
    m_docReady = false;
    m_needRender = true;
    m_pageCount = 0;
    m_parentHWND = nullptr;
    m_memoryBMP = nullptr;
    m_memoryDC = nullptr;
    m_clientSizeDirty = true;
    ::InitializeCriticalSection(&m_memoryCanvasLock);
    if (url)
        m_url = url;

    m_parentWebview = parentWebview;
    wkeSetUserKeyValue(m_parentWebview, "ChildPdfViewerPlugin", this);
    m_parentId = wkeGetWebviewId(m_parentWebview);

    m_webview = wkeCreateWebView();
    m_parentHWND = wkeGetHostHWND(m_parentWebview);
    wkeSetDragDropEnable(m_webview, false);
    wkeSetHandle(m_webview, m_parentHWND);

    wkeSetContextMenuEnabled(parentWebview, false);
    wkeSetContextMenuEnabled(m_webview, false);

    int64_t mbParentViewId = (int64_t)wkeGetUserKeyValue(parentWebview, "MbWebView");
    wkeSetUserKeyValue(m_webview, "MbWebView", (void*)mbParentViewId); // 和关联的webview共用一个，使得打印的时候能直接给关联的webview发打印消息

    wkeOnPaintUpdated(m_webview, (wkePaintUpdatedCallback)staticOnPaintUpdated, (void*)(intptr_t)m_id);
    wkeOnDidCreateScriptContext(m_webview, (wkeDidCreateScriptContextCallback)&staticDidCreateScriptContextCallback, (void*)(intptr_t)m_id);
    wkeOnWillReleaseScriptContext(m_webview, (wkeWillReleaseScriptContextCallback)&staticOnWillReleaseScriptContextCallback, (void*)(intptr_t)m_id);
    
    wkeOnLoadingFinish(m_webview, (wkeLoadingFinishCallback)onLoadingFinishCallback, (void*)(intptr_t)m_id);
    wkeOnDocumentReady(m_webview, (wkeDocumentReadyCallback)onDocumentReady, (void*)(intptr_t)m_id);
    wkeOnOtherLoad(m_webview, (wkeOnOtherLoadCallback)onOtherLoadCallback, (void*)(intptr_t)m_id);
    wkeOnLoadUrlBegin(m_webview, (wkeLoadUrlBeginCallback)onLoadUrlBegin, (void*)(intptr_t)m_id);

    wkeSetCspCheckEnable(m_webview, false);
    wkeSetUserKeyValue(m_webview, "PdfViewerPlugin", this);
    wkeOnPrint(m_webview, onPrintPdfCallback, (void*)(intptr_t)m_id);

    PdfViewerPlugin* self = this;
    m_closure = new std::function<void(mbJsExecState es, int64_t queryId, int customMsg, const utf8* request)>(
        std::move([self](mbJsExecState es, int64_t queryId, int customMsg, const utf8* request) {
        return self->onJsQuery(es, queryId, customMsg, request);
    }));

    wkeLoadHtmlWithBaseUrl(m_webview, (const utf8 *)kPdfViewerHtml, "PdfViewer.html");
    //wkeLoadURL(m_webview, "file:///G:/mycode/mb/mbvip/printing/PdfViewer_zh.htm");
}

PdfViewerPlugin::~PdfViewerPlugin()
{
    common::LiveIdDetect::get()->deconstructed(m_id);

    if (wkeIsWebviewAlive(m_parentId)) {
        PdfViewerPlugin* plugin = (PdfViewerPlugin*)wkeGetUserKeyValue(m_parentWebview, "ChildPdfViewerPlugin");
        if (plugin == this)
            wkeSetUserKeyValue(m_parentWebview, "ChildPdfViewerPlugin", nullptr);
    }

    ::DeleteCriticalSection(&m_memoryCanvasLock);

    delete m_closure;
    if (m_pdfData)
        delete m_pdfData;

    if (m_memoryBMP)
        ::DeleteObject((HGDIOBJ)m_memoryBMP);
    if (m_memoryDC)
        ::DeleteObject((HGDIOBJ)m_memoryDC);

    if (m_preloadcode)
        delete m_preloadcode;

    wkeDestroyWebView(m_webview);
}

void PdfViewerPlugin::loadPreloadURL()
{
    if (!m_preloadcode)
        return;

    std::string* preloadcode = m_preloadcode;
    m_preloadcode = nullptr;
    wkeRunJS(m_webview, preloadcode->c_str());
    delete preloadcode;
}

void PdfViewerPlugin::staticOnPaintUpdated(wkeWebView webView, int id, const HDC hdc, int x, int y, int cx, int cy)
{
    PdfViewerPlugin* self = (PdfViewerPlugin*)common::LiveIdDetect::get()->getPtrLocked((int64_t)id);
    if (!self)
        return;

    content::postTaskToMainThread(FROM_HERE, [self, id, x, y, cx, cy] {
        if (!common::LiveIdDetect::get()->isLive(id))
            return;
        NPRect rect = { (uint16_t)y, (uint16_t)x, (uint16_t)(y + cy), (uint16_t)(x + cx) };
        g_npBrowserFunctions->invalidaterect(self->m_instance, &rect);
    });

    self->onPrePaintUpdatedInCompositeThread(id, self->m_parentHWND, hdc, x, y, cx, cy);

    common::LiveIdDetect::get()->unlock(id, self);
}

void PdfViewerPlugin::onPrePaintUpdatedInCompositeThread(int id, HWND hWnd, const HDC hdc, int x, int y, int cx, int cy)
{
    ::EnterCriticalSection(&m_memoryCanvasLock);
    onPaintUpdatedInCompositeThread(hWnd, hdc, x, y, cx, cy);
    ::LeaveCriticalSection(&m_memoryCanvasLock);
}

void PdfViewerPlugin::onPaintUpdatedInCompositeThread(HWND hWnd, const HDC hdc, int x, int y, int cx, int cy)
{
    SIZE clientSize = { m_windowPos.cx, m_windowPos.cy };
    if (0 == clientSize.cx * clientSize.cy)
        return;

    if (!m_memoryDC)
        m_memoryDC = ::CreateCompatibleDC(nullptr);

    if ((!m_memoryBMP) || m_clientSizeDirty) {
        if (m_memoryBMP)
            ::DeleteObject((HGDIOBJ)m_memoryBMP);
        m_memoryBMP = ::CreateCompatibleBitmap(hdc, clientSize.cx, clientSize.cy);
        ::SelectObject(m_memoryDC, m_memoryBMP);
        m_clientSizeDirty = false;
    }

    if (m_memoryDC)
        ::BitBlt(m_memoryDC, x, y, cx, cy, hdc, x, y, SRCCOPY);
}

void PdfViewerPlugin::onSize(const WINDOWPOS& windowpos)
{
    ::EnterCriticalSection(&m_memoryCanvasLock);
    if (m_windowPos.cx != windowpos.cx || m_windowPos.cy != windowpos.cy)
        m_clientSizeDirty = true;
    m_windowPos = windowpos;
    ::LeaveCriticalSection(&m_memoryCanvasLock);

    wkeResize(m_webview, windowpos.cx, windowpos.cy);
}

void PdfViewerPlugin::onPaint(HDC hdcScreen)
{
    ::EnterCriticalSection(&m_memoryCanvasLock);
    ::BitBlt(hdcScreen, m_npWindow.x, m_npWindow.y, m_npWindow.width, m_npWindow.height, m_memoryDC, 0, 0, SRCCOPY);
    ::LeaveCriticalSection(&m_memoryCanvasLock);
}

void PdfViewerPlugin::onSetWinow(const NPWindow& npWindow)
{
    m_npWindow = npWindow;
}

void PdfViewerPlugin::onMouseEvt(uint32_t message, uint32_t wParam, uint32_t lParam)
{
    int x = LOWORD(lParam) + ( - m_windowPos.x);
    int y = HIWORD(lParam) + ( - m_windowPos.y);

    unsigned int flags = 0;

    if (wParam & MK_CONTROL)
        flags |= WKE_CONTROL;
    if (wParam & MK_SHIFT)
        flags |= WKE_SHIFT;

    if (wParam & MK_LBUTTON)
        flags |= WKE_LBUTTON;
    if (wParam & MK_MBUTTON)
        flags |= WKE_MBUTTON;
    if (wParam & MK_RBUTTON)
        flags |= WKE_RBUTTON;

    int delta = GET_WHEEL_DELTA_WPARAM(wParam);
    int64_t id = m_id;
    wkeWebView webview = m_webview;

    content::postTaskToMainThread(FROM_HERE, [webview, id, message, x, y, delta, flags] {
        if (!common::LiveIdDetect::get()->isLive(id))
            return;

        if (WM_MOUSEWHEEL == message)
            wkeFireMouseWheelEvent(webview, x, y, delta, flags);
        else
            wkeFireMouseEvent(webview, message, x, y, flags);
    });
}

void PdfViewerPlugin::onKey(uint32_t message, uint32_t wParam, uint32_t lParam)
{
    unsigned int virtualKeyCode = wParam;
    unsigned int flags = 0;
    if (HIWORD(lParam) & KF_REPEAT)
        flags |= WKE_REPEAT;
    if (HIWORD(lParam) & KF_EXTENDED)
        flags |= WKE_EXTENDED;

    int64_t id = m_id;
    wkeWebView webview = m_webview;
    content::postTaskToMainThread(FROM_HERE, [webview, id, message, virtualKeyCode, flags] {
        if (!common::LiveIdDetect::get()->isLive(id))
            return;
        switch (message) {
        case WM_KEYDOWN:
            wkeFireKeyDownEvent(webview, virtualKeyCode, flags, false);
            break;
        case WM_KEYUP:
            wkeFireKeyUpEvent(webview, virtualKeyCode, flags, false);
            break;
        case WM_CHAR:
            wkeFireKeyPressEvent(webview, virtualKeyCode, flags, false);
            break;
        }
    });
}

void PdfViewerPlugin::setPreloadURL(const std::string& preload)
{
    std::string preloadURL = preload;
    if (preloadURL.size() > 9 && "file:///" == preloadURL.substr(0, 8))
        preloadURL = preloadURL.substr(8);

    if (m_preloadcode)
        delete m_preloadcode;
    m_preloadcode = new std::string("require('");
    m_preloadcode->append(preloadURL);
    m_preloadcode->append("');");
}

std::string PdfViewerPlugin::getURL()
{
    return wkeGetURL(m_webview);
}

void PdfViewerPlugin::clearPdfDatas()
{
    if (m_pdfData)
        delete m_pdfData;
    m_pdfData = nullptr;
    m_pdfPageInfos.clear();
}

void PdfViewerPlugin::resetPdfImpl()
{
    if (!m_docReady || !m_pdfData || !PdfiumLoad::get())
        return;
    m_needRender = false;

    double width = 0;
    double height = 0;
    int pageCount = 0;
    double maxPageWidth = 0;

    m_pdfPageInfos.clear();

    wchar_t* bookmarkJson = nullptr;
    BOOL b = PdfiumLoad::get()->getPDFDocInfo(&m_pdfData->at(0), (int)m_pdfData->size(), &pageCount, &maxPageWidth, &bookmarkJson);
    if (!b || 0 == pageCount) {
        clearPdfDatas();
        wkeRunJS(m_webview, "onNativeReflush(0, 0, 0, 'getPDFDocInfo fail')");
        return;
    }
    m_pageCount = pageCount;

    std::string bookmarkJsonUtf8;

    if (bookmarkJson) {
        bookmarkJsonUtf8 = common::utf16ToUtf8(bookmarkJson);
        PdfiumLoad::get()->freeMem(bookmarkJson);
    }

    for (int i = 0; i < pageCount; ++i) {
        b = PdfiumLoad::get()->getPDFPageSizeByIndex(&m_pdfData->at(0), (int)m_pdfData->size(), 0, &width, &height);
        if (!b || 0 == width || 0 == height) {
            clearPdfDatas();
            wkeRunJS(m_webview, "onNativeReflush(0, 0, 0, 'getPDFPageSizeByIndex fail')");
            return;
        }

        PdfPageInfo info = { (int)width , (int)height };
        m_pdfPageInfos.push_back(info);
    }

    char* script = (char*)malloc(0x200);
    sprintf(script, "window.onNativeReflush(%d, %f, %f, '');", pageCount, (float)width, (float)height);

    wkeRunJS(m_webview, script);
    free(script);

    //////////////////////////////////////////////////////////////////////////
//     HANDLE hFile = CreateFileW(L"E:\\test\\bt177\\json.js", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//     DWORD fileSizeHigh;
//     const DWORD bufferSize = ::GetFileSize(hFile, &fileSizeHigh);
// 
//     std::vector<char> buffer;
//     DWORD numberOfBytesRead = 0;
//     buffer.resize(bufferSize);
//     b = ::ReadFile(hFile, &buffer.at(0), bufferSize, &numberOfBytesRead, nullptr);
//     ::CloseHandle(hFile);

    if (false && !bookmarkJsonUtf8.empty()) { // 暂时关闭默认开启书签功能
        std::string scriptBookmark = "window.onNativeBookmark(";
        //scriptBookmark += std::string(&buffer.at(0), bufferSize);
        scriptBookmark += bookmarkJsonUtf8;
        scriptBookmark += ")";
        wkeRunJS(m_webview, scriptBookmark.c_str());
    } else {
        wkeRunJS(m_webview, "window.onNativeBookmark(null)");
    }
}

void PdfViewerPlugin::resetPdf(std::vector<char>* pdfData)
{
    if (m_pdfData)
        delete m_pdfData;
    m_pdfData = pdfData;

    resetPdfImpl();
}

}

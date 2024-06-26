
#include "download/DownloadMgr.h"
#include "download/DownloadPageHtml.h"
#include "download/DownloadUtil.h"
#include "download/SimpleDownload.h"
#include "core/MbWebView.h"
#include "common/ThreadCall.h"
#include "common/StringUtil.h"
#include "common/LiveIdDetect.h"
#include <process.h>

struct RequestWrap {
    DownloadMgr* self;
    int id;
};

static void notifDownloadFail(mbWebView mbWebview, int id)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(mbWebview);
    if (!webview)
        return;

    std::vector<char> scriptTemp;
    scriptTemp.resize(100);
    sprintf(&scriptTemp[0], "onNativeDownloadFail(%d);", id);
    mbRunJs(mbWebview, mbWebFrameGetMainFrame(mbWebview), &scriptTemp[0], false, nullptr, nullptr, nullptr);
}

static void notifDownloadProgress(mbWebView mbWebview, int id, float progress)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(mbWebview);
    if (!webview)
        return;

    std::vector<char> scriptTemp;
    scriptTemp.resize(100);
    sprintf(&scriptTemp[0], "onNativeDownloadProgress(%d, %f);", id, progress);
    mbRunJs(mbWebview, mbWebFrameGetMainFrame(mbWebview), &scriptTemp[0], false, nullptr, nullptr, nullptr);
}

static void notifDownloadFinish(mbWebView mbWebview, int id)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(mbWebview);
    if (!webview)
        return;

    std::vector<char> scriptTemp;
    scriptTemp.resize(100);
    sprintf(&scriptTemp[0], "onNativeDownloadFinish(%d);", id);
    mbRunJs(mbWebview, mbWebFrameGetMainFrame(mbWebview), &scriptTemp[0], false, nullptr, nullptr, nullptr);
}

static void WKE_CALL_TYPE onDownloadUiWillRedirectCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr oldRequest, wkeWebUrlRequestPtr request, wkeWebUrlResponsePtr redirectResponse)
{

}

static void WKE_CALL_TYPE onDownloadUiDidReceiveResponseCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, wkeWebUrlResponsePtr response)
{
    RequestWrap* wrap = (RequestWrap*)param;
    __int64 expectedContentLength = wkeNetGetExpectedContentLength(response);

    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [wrap, expectedContentLength] {
        DownloadMgr::DownloadItem* item = wrap->self->findById(wrap->id);
        if (!item)
            return;

        if (expectedContentLength > 0x00ffffff) {
            item->m_handleOfSave = nullptr;
            notifDownloadFail(item->m_parent->getView(), item->m_id);
            return;
        }

        item->m_totalSize = (size_t)expectedContentLength; // may be -1
        item->m_downloadedSize = 0;
    });
}

static void WKE_CALL_TYPE onDownloadUiDidReceiveDataCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, const char* data, int dataLength)
{
    RequestWrap* wrap = (RequestWrap*)param;
    std::vector<char>* buffer = new std::vector<char>();
    buffer->resize(dataLength);
    memcpy(&buffer->at(0), data, dataLength);

    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [wrap, buffer] {
        DownloadMgr::DownloadItem* item = wrap->self->findById(wrap->id);
        if (!item) {
            delete buffer;
            return;
        }

        HANDLE hFile = item->m_handleOfSave;
        if (!hFile || INVALID_HANDLE_VALUE == hFile) {
            notifDownloadFail(item->m_parent->getView(), item->m_id);
            delete buffer;
            return;
        }

        DWORD numberOfBytesWrite = 0;
        BOOL b = ::WriteFile(hFile, &buffer->at(0), buffer->size(), &numberOfBytesWrite, nullptr);
        if (!b) {
            ::CloseHandle(hFile);
            item->m_handleOfSave = nullptr;
            notifDownloadFail(item->m_parent->getView(), item->m_id);
            delete buffer;
            return;
        }

        item->m_downloadedSize += numberOfBytesWrite;

        if (-1 == item->m_totalSize) {
            item->m_progress += 0.01f;
        } else
            item->m_progress = item->m_downloadedSize / (item->m_totalSize + 0.0f);

        if (item->m_progress > 1)
            item->m_progress = 1.0f;
        notifDownloadProgress(item->m_parent->getView(), item->m_id, item->m_progress);
        delete buffer;
    });
}

static void WKE_CALL_TYPE onDownloadUiDidFailCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, const utf8* error)
{
    RequestWrap* wrap = (RequestWrap*)param;
    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [wrap] {
        DownloadMgr::DownloadItem* item = wrap->self->findById(wrap->id);
        if (!item)
            return;

        HANDLE hFile = item->m_handleOfSave;
        if (hFile && INVALID_HANDLE_VALUE != hFile)
            ::CloseHandle(hFile);
        item->m_handleOfSave = nullptr;
        notifDownloadFail(item->m_parent->getView(), item->m_id);
    });
}

static void WKE_CALL_TYPE onDownloadUiDidFinishLoadingCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, double finishTime)
{
    RequestWrap* wrap = (RequestWrap*)param;
    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [wrap] {
        DownloadMgr::DownloadItem* item = wrap->self->findById(wrap->id);
        if (!item)
            return;

        HANDLE hFile = item->m_handleOfSave;
        if (hFile && INVALID_HANDLE_VALUE != hFile)
            ::CloseHandle(hFile);
        item->m_handleOfSave = nullptr;
        item->m_progress = 1.0f;
        item->m_totalSize = item->m_downloadedSize;
        notifDownloadFinish(item->m_parent->getView(), item->m_id);
    });
}

DownloadMgr::DownloadMgr()
{
    m_mbView = NULL_WEBVIEW;
    m_idGen = 1;
    m_selectDialogThreadHandle = nullptr;
    m_scriptContextReady = false;
}

void DownloadMgr::startDownload()
{
    int id = m_curSeleteItem->m_id;
    const std::string& url = m_curSeleteItem->m_url;
    const std::string& mime = m_curSeleteItem->m_mime;

    HANDLE hFile = CreateFileW(m_curSeleteItem->m_savePath.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        notifDownloadFail(getView(), id);
        return;
    }
    m_curSeleteItem->m_handleOfSave = hFile;

    wkeUrlRequestCallbacks callbacksWrap = {
        onDownloadUiWillRedirectCallback ,
        onDownloadUiDidReceiveResponseCallback,
        onDownloadUiDidReceiveDataCallback,
        onDownloadUiDidFailCallback,
        onDownloadUiDidFinishLoadingCallback,
    };

    RequestWrap* wrap = new RequestWrap();
    wrap->self = this;
    wrap->id = id;

    wkeWebUrlRequestPtr request = wkeNetCreateWebUrlRequest(url.c_str(), "GET", mime.c_str());

    m_items.push_back(m_curSeleteItem);
    m_curSeleteItem = nullptr;

    wkeNetStartUrlRequest(nullptr, (wkeWebUrlRequestPtr)request, wrap, &callbacksWrap);
}

DownloadMgr::DownloadItem* DownloadMgr::findById(int id)
{
    for (size_t i = 0; i < m_items.size(); ++i) {
        DownloadItem* item = m_items[i];
        if (item->m_id == id)
            return item;
    }

    return nullptr;
}

struct SeleteDialogThreadParam {
    DownloadMgr* self;
    std::wstring defaultSaveName;
};

unsigned DownloadMgr::seleteDialogThread(void* param)
{
    SeleteDialogThreadParam* threadParam = (SeleteDialogThreadParam*)param;
    DownloadMgr* self = threadParam->self;

    OPENFILENAMEW ofn = { 0 };
    std::vector<wchar_t>* fileResult = new std::vector<wchar_t>();
    fileResult->resize(4 * MAX_PATH + 1);

    wcscpy(&fileResult->at(0), threadParam->defaultSaveName.c_str());

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = &fileResult->at(0);
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"All\0*.*\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;

    if (!GetSaveFileNameW(&ofn)) {
        delete fileResult;
        fileResult = nullptr;
    }

    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [self, fileResult] {
        self->onSeletePathFinish(fileResult);
    });

    return 0;
}

static std::string createSavePath(const std::wstring& savePath)
{
    std::wstring savePathW = savePath;
    if (savePathW.size() > 40) {
        savePathW = savePathW.substr(0, 40);
        savePathW += L"...";
    }

    std::string savePathA = common::utf16ToMulByte(savePathW.c_str(), CP_UTF8);
    savePathA = mbUtilEncodeURLEscape(savePathA.c_str());

    for (size_t i = 0; i < savePathA.size(); ++i) {
        char c = savePathA[i];
        if ('\\' == c) {
            savePathA[i] = '%';
            savePathA.insert(i + 1, "5C");
        }
    }

    return savePathA;
}

void DownloadMgr::onSeletePathFinish(std::vector<wchar_t>* path)
{
    ::CloseHandle(m_selectDialogThreadHandle);
    m_selectDialogThreadHandle = nullptr;

    if (!path || !m_curSeleteItem || 0 == path->size()) {
        delete m_curSeleteItem;
        m_curSeleteItem = nullptr;

        mbRunJs(m_mbView, mbWebFrameGetMainFrame(m_mbView), "onNativeSeletePathFinish(0);", false, nullptr, nullptr, nullptr);
        return;
    } else
        m_curSeleteItem->m_savePath = &path->at(0);

    delete path;

    std::vector<char> scriptTemp;
    scriptTemp.resize(0x200 + m_curSeleteItem->m_savePath.size());

    std::string savePath = createSavePath(m_curSeleteItem->m_savePath);

    sprintf(&scriptTemp[0], "onNativeSeletePathFinish(%d, '%s');", m_curSeleteItem->m_id, savePath.c_str());
    mbRunJs(m_mbView, mbWebFrameGetMainFrame(m_mbView), &scriptTemp[0], false, nullptr, nullptr, nullptr);

    startDownload();
}

void DownloadMgr::onNewDownloadItemImpl(DownloadItem* item)
{
    if (!item)
        return;

    std::vector<char> scriptTemp;
    scriptTemp.resize(0x200);
    sprintf(&scriptTemp[0], "onNativeNewDownloadItem(%d, \'", item->m_id);
    
    std::string script = &scriptTemp[0];

    script += common::base64Encode((const unsigned char *)item->m_url.c_str(), item->m_url.size());
    script += "\');";
    mbRunJs(m_mbView, mbWebFrameGetMainFrame(m_mbView), script.c_str(), false, nullptr, nullptr, nullptr);
}

void DownloadMgr::onDocumentReadyCallback(mbWebView webView, void* param, mbWebFrameHandle frameId)
{
    DownloadMgr* self = (DownloadMgr*)param;
    mbMoveToCenter(webView);
    mbShowWindow(webView, TRUE);
    self->m_scriptContextReady = true;
    self->onNewDownloadItemImpl(self->m_curSeleteItem);
}

void DownloadMgr::onNewDownloadItem(const char* url, const char* mime, const char* contentDisposition)
{
    createWnd();
    if (m_curSeleteItem)
        return;

    DownloadItem* item = new DownloadItem(this, url, mime, contentDisposition, m_idGen++);
    m_curSeleteItem = item;

    if (!m_scriptContextReady)
        return;
   
    onNewDownloadItemImpl(item);
}

bool DownloadMgr::createWnd()
{
    HWND hWnd = nullptr;
    if (m_mbView) {
        hWnd = mbGetHostHWND(m_mbView);
        SetForegroundWindow(hWnd);
        ::ShowWindow(hWnd, SW_SHOW);
        return false;
    }

    int width = 730;
    int height = 550;

    m_mbView = mbCreateWebWindow(MB_WINDOW_TYPE_POPUP, NULL, 0, 0, 1, 1);

    hWnd = mbGetHostHWND(m_mbView);
    DWORD dwExStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
    dwExStyle = dwExStyle | WS_EX_TOOLWINDOW;
    ::SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle);

    DWORD dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);
    dwStyle = dwStyle & (~WS_CAPTION) & (~WS_SYSMENU) & (~WS_SIZEBOX);
    ::SetWindowLong(hWnd, GWL_STYLE, dwStyle);

    mbResize(m_mbView, width, height);
    
//     ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    mbOnJsQuery(m_mbView, onJsQueryCallback, this);
    mbOnDocumentReady(m_mbView, onDocumentReadyCallback, this);

    //mbLoadURL(m_mbView, "file:///E:/mycode/mtmb/mtmb/download/DownloadPage.htm");
    mbLoadHtmlWithBaseUrl(m_mbView, (const utf8 *)kDownloadPageHtml, "DownloadPage.html");

    ::UpdateWindow(hWnd);

    return true;
}

void MB_CALL_TYPE DownloadMgr::onJsQueryCallback(mbWebView webView, void* param, mbJsExecState es, int64_t queryId, int customMsg, const utf8* request)
{
    DownloadMgr* self = (DownloadMgr*)param;
    HWND hWnd = mbGetHostHWND(webView);
    if (0 == strcmp(request, "min")) {
        ::PostMessage(hWnd, WM_CLOSE, 0, 0);
    } else if (0 == strcmp(request, "selectFilePath")) {
        self->runSelectFilePathDialog();
    } else if (0 == strcmp(request, "cancelSelectFilePath")) {
        self->cancelSelectFilePath();
    } else if (0 == strcmp(request, "close")) {
        self->close();
    } else if (0 == strcmp(request, "openFile")) {
        self->openFile(customMsg);
    } 
}

void DownloadMgr::close()
{
    HWND hWnd = mbGetHostHWND(m_mbView);
    ShowWindow(hWnd, SW_HIDE);
}

static unsigned int WINAPI openFileThread(void* param)
{
    std::wstring* savePath = (std::wstring*)param;
    std::wstring str = L"/select, ";
    str += *savePath;
    delete savePath;
    ::ShellExecuteW(NULL, L"open", L"explorer.exe", str.c_str(), NULL, SW_SHOWNORMAL);
    return 0;
}

void DownloadMgr::openFile(int id)
{
    DownloadItem* item = findById(id);
    if (!item)
        return;

    std::wstring* savePath = new std::wstring(item->m_savePath);
    unsigned int threadIdentifier = 0;
    HANDLE hHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, openFileThread, savePath, 0, &threadIdentifier));
    ::CloseHandle(hHandle);
}

void DownloadMgr::cancelSelectFilePath()
{
    if (m_curSeleteItem)
        delete m_curSeleteItem;
    m_curSeleteItem = nullptr;
}

void DownloadMgr::runSelectFilePathDialog()
{
    unsigned int threadIdentifier = 0;
    if (m_selectDialogThreadHandle || !m_curSeleteItem)
        return;

    SeleteDialogThreadParam* param = new SeleteDialogThreadParam();
    param->self = this;
    param->defaultSaveName = getSaveName(m_curSeleteItem->m_contentDisposition, m_curSeleteItem->m_url);
    if (0 == param->defaultSaveName.size()) {
        delete param;
        delete m_curSeleteItem;
        m_curSeleteItem = nullptr;

        mbRunJs(m_mbView, mbWebFrameGetMainFrame(m_mbView), "onNativeSeletePathFinish(0);", false, nullptr, nullptr, nullptr);
        return;
    }
        
    m_selectDialogThreadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, seleteDialogThread, param, 0, &threadIdentifier));
}

mbDownloadOpt DownloadMgr::simpleDownload(mbWebView mbWebview,
    const WCHAR* path,
    size_t expectedContentLength,
    const char* url,
    const char* mime,
    const char* disposition,
    mbNetJob job,
    mbNetJobDataBind* dataBind,
    mbDownloadBind* callbackBind)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(mbWebview);
    if (!webview)
        return kMbDownloadOptCancel;

    webview->setIsMouseKeyMessageEnable(false);
    SimpleDownload* downloader = SimpleDownload::create(mbWebview, path,
        expectedContentLength,
        url,
        mime,
        disposition,
        job,
        dataBind,
        callbackBind);
    if (downloader)
        return kMbDownloadOptCacheData;

    webview->setIsMouseKeyMessageEnable(true);
    return kMbDownloadOptCancel;
}

//////////////////////////////////////////////////////////////////////////

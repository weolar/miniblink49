
#ifndef download_SimpleDownload_h
#define download_SimpleDownload_h

#include "wke/wkedefine.h"
#include "wke/DownloadUtil.h"
#include "content/browser/PostTaskHelper.h"
#include "content/browser/WebPage.h"
#include "base/strings/string_util.h"
#include <process.h>
#include <shlwapi.h>

class SimpleDownload {
public:
    static SimpleDownload* create(wkeWebView webView,
        const wkeDialogOptions* dialogOpt,
        const WCHAR* savePath,
        size_t expectedContentLength,
        const char* url,
        const char* mime,
        const char* disposition,
        wkeNetJob job,
        wkeNetJobDataBind* dataBind,
        wkeDownloadBind* callbackBind);

    ~SimpleDownload();

    static unsigned int WINAPI dialogThread(void* param);

    static int getDialogCount()
    {
        return m_dialogCount;
    }
private:
    SimpleDownload(wkeWebView wkeView,
        size_t expectedContentLength,
        const char* url,
        const char* mime,
        const char* disposition,
        wkeNetJob job,
        wkeNetJobDataBind* dataBind,
        wkeDownloadBind* callbackBind);

    void startSave(std::vector<wchar_t>* path);
    void startSaveImpl();
    bool canSave();

    static void WKE_CALL_TYPE onSimpleDownloadWillRedirectCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr oldRequest, wkeWebUrlRequestPtr request, wkeWebUrlResponsePtr redirectResponse)
    {
    }

    static void WKE_CALL_TYPE onSimpleDownloadDidReceiveResponseCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, wkeWebUrlResponsePtr response)
    {
        SimpleDownload* self = (SimpleDownload*)param;
        __int64 expectedContentLength = wkeNetGetExpectedContentLength(response);
        if (expectedContentLength > 0x00ffffff) {
            self->m_handleOfSave = nullptr;
            return;
        }

        self->m_totalSize = (size_t)expectedContentLength; // may be -1
        self->m_downloadedSize = 0;
    }

    static void WKE_CALL_TYPE onSimpleDownloadDidReceiveDataCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, const char* data, int dataLength)
    {
        SimpleDownload* self = (SimpleDownload*)param;

        HANDLE hFile = self->m_handleOfSave;
        if (!hFile || INVALID_HANDLE_VALUE == hFile)
            return;

        DWORD nuwkeerOfBytesWrite = 0;
        BOOL b = ::WriteFile(hFile, data, dataLength, &nuwkeerOfBytesWrite, nullptr);
        if (!b) {
            ::CloseHandle(hFile);
            self->m_handleOfSave = nullptr;
            return;
        }

        self->m_downloadedSize += nuwkeerOfBytesWrite;
    }

    static void WKE_CALL_TYPE onSimpleDownloadDidFailCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, const utf8* error)
    {
        SimpleDownload* self = (SimpleDownload*)param;
        HANDLE hFile = self->m_handleOfSave;
        if (hFile && INVALID_HANDLE_VALUE != hFile)
            ::CloseHandle(hFile);
        self->m_handleOfSave = nullptr;

        delete self;
    }

    static void WKE_CALL_TYPE onSimpleDownloadDidFinishLoadingCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, double finishTime)
    {
        SimpleDownload* self = (SimpleDownload*)param;
        HANDLE hFile = self->m_handleOfSave;
        if (hFile && INVALID_HANDLE_VALUE != hFile)
            ::CloseHandle(hFile);
        self->m_handleOfSave = nullptr;
        self->m_totalSize = self->m_downloadedSize;

        delete self;
    }

    static void WKE_CALL_TYPE onDataRecv(void* param, wkeNetJob job, const char* data, int length)
    {
        SimpleDownload* self = (SimpleDownload*)param;
        self->onDataRecvImpl(job, data, length);
    }

    static void WKE_CALL_TYPE onDataFinish(void* param, wkeNetJob job, wkeLoadingResult result)
    {
        SimpleDownload* self = (SimpleDownload*)param;
        self->onDataFinishImpl(job, result);
    }

    void onDataRecvImpl(wkeNetJob job, const char* data, int length);
    void onDataFinishImpl(wkeNetJob job, wkeLoadingResult result);

    static int m_dialogCount;

    std::string m_url;
    std::string m_mime;
    std::string m_contentDisposition;
    std::wstring m_savePath;

    HANDLE m_handleOfSave;
    size_t m_totalSize;
    size_t m_downloadedSize;

    wkeWebView m_wkeView;

    std::vector<char> m_cacheData;
    bool m_hasFinish;
    wkeLoadingResult m_loadingResult;

    wkeDownloadBind m_callbackBind;

    struct DialogOptions {
        std::string title;
        std::string defaultPath;
        std::string buttonLabel;
    };
    DialogOptions dialogOpt;
};

int SimpleDownload::m_dialogCount = 0;

SimpleDownload::SimpleDownload(wkeWebView wkeView,
    size_t expectedContentLength,
    const char* url,
    const char* mime,
    const char* disposition,
    wkeNetJob job,
    wkeNetJobDataBind* dataBind,
    wkeDownloadBind* callbackBind)
{
    m_totalSize = expectedContentLength;
    m_url = url;
    m_mime = mime;
    m_contentDisposition = disposition;
    m_handleOfSave = nullptr;
    m_wkeView = wkeView;
    m_hasFinish = false;
    m_loadingResult = (wkeLoadingResult)-1;

    dataBind->param = this;
    dataBind->recvCallback = onDataRecv;
    dataBind->finishCallback = onDataFinish;

    m_callbackBind.param = nullptr;
    m_callbackBind.recvCallback = nullptr;
    m_callbackBind.finishCallback = nullptr;
    m_callbackBind.saveNameCallback = nullptr;
    
    if (callbackBind)
        m_callbackBind = *callbackBind;
}

SimpleDownload::~SimpleDownload()
{

}

SimpleDownload* SimpleDownload::create(wkeWebView webView,
    const wkeDialogOptions* dialogOpt,
    const WCHAR* savePath,
    size_t expectedContentLength,
    const char* url,
    const char* mime,
    const char* disposition,
    wkeNetJob job,
    wkeNetJobDataBind* dataBind,
    wkeDownloadBind* callbackBind)
{
    if (m_dialogCount > 0)
        return nullptr;

    SimpleDownload* self = new SimpleDownload(webView, expectedContentLength, url, mime, disposition, job, dataBind, callbackBind);

    if (dialogOpt && dialogOpt->magic == 'mbdo' && dialogOpt->defaultPath)
        self->dialogOpt.defaultPath = dialogOpt->defaultPath;

    if (savePath) {
        content::WebPage* page = webView->webPage();
        if (page)
            page->setIsMouseKeyMessageEnable(true);

        std::vector<wchar_t>* fileResult = nullptr;
        size_t pathLen = wcslen(savePath);
        if (pathLen > 0) {
            fileResult = new std::vector<wchar_t>();
            fileResult->resize(MAX_PATH * 2);

            memcpy(&fileResult->at(0), savePath, pathLen * sizeof(WCHAR));

            std::wstring defaultSaveName = getSaveName(self->m_contentDisposition, self->m_url);
            PathAppendW(&fileResult->at(0), defaultSaveName.c_str());
        }

        content::postTaskToUiThread(FROM_HERE, NULL, [self, fileResult] {
            self->startSave(fileResult);
        });

    } else {
        InterlockedIncrement((long *)&m_dialogCount);

        unsigned int threadIdentifier = 0;
        HANDLE hHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, SimpleDownload::dialogThread, self, 0, &threadIdentifier));
        ::CloseHandle(hHandle);
    }

    return self;
}

bool SimpleDownload::canSave()
{
    if (m_hasFinish && (WKE_LOADING_FAILED == m_loadingResult || WKE_LOADING_CANCELED == m_loadingResult))
        return false;
    return true;
}

void SimpleDownload::startSaveImpl()
{
    if (!m_handleOfSave && !m_savePath.empty()) {
        HANDLE hFile = CreateFileW(m_savePath.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (!hFile || INVALID_HANDLE_VALUE == hFile) {
            OutputDebugStringW(L"SimpleDownload::startSaveImpl fail:");
            OutputDebugStringW(m_savePath.c_str());
            OutputDebugStringW(L"\n");

            m_savePath = L"";

            m_loadingResult = WKE_LOADING_FAILED;

            if (m_callbackBind.finishCallback)
                m_callbackBind.finishCallback(m_callbackBind.param, nullptr, m_loadingResult);
            return;
        }
        m_handleOfSave = hFile;

        if (m_callbackBind.saveNameCallback)
            m_callbackBind.saveNameCallback(m_callbackBind.param, m_savePath.c_str());
    }

    if (!m_handleOfSave) {
        OutputDebugStringA("SimpleDownload::startSaveImpl, m_handleOfSave fail~\n");
        return;
    }

    if (m_callbackBind.recvCallback && canSave() && 0 != m_cacheData.size())
        m_callbackBind.recvCallback(m_callbackBind.param, nullptr, &m_cacheData[0], (int)m_cacheData.size());

    BOOL b = FALSE;
    DWORD nuwkeerOfBytesWrite = 0;
    if (0 != m_cacheData.size())
        b = ::WriteFile(m_handleOfSave, &m_cacheData[0], (DWORD)m_cacheData.size(), &nuwkeerOfBytesWrite, nullptr);
    m_cacheData.clear();

    if (!m_hasFinish)
        return;
    
    ::CloseHandle(m_handleOfSave);
    m_handleOfSave = nullptr;

    if (m_callbackBind.finishCallback)
        m_callbackBind.finishCallback(m_callbackBind.param, nullptr, m_loadingResult);

    SimpleDownload* self = this;
    content::postTaskToUiThread(FROM_HERE, NULL, [self] {
        delete self;
    });
}

void SimpleDownload::startSave(std::vector<wchar_t>* path)
{
    if (!path || 0 == path->size()) {
        m_loadingResult = (wkeLoadingResult)-1;
        if (m_callbackBind.finishCallback)
            m_callbackBind.finishCallback(m_callbackBind.param, nullptr, m_loadingResult);
        return;
    }
    m_savePath = &path->at(0);
    delete path;

    startSaveImpl();
}

void SimpleDownload::onDataRecvImpl(wkeNetJob job, const char* data, int length)
{
    if (0 == length)
        return;

    size_t oldSize = m_cacheData.size();
    m_cacheData.resize(oldSize + length);
    memcpy(&m_cacheData[oldSize], data, length);

    m_downloadedSize += length;

    startSaveImpl();
}

void SimpleDownload::onDataFinishImpl(wkeNetJob job, wkeLoadingResult result)
{
    m_hasFinish = true;
    m_loadingResult = result;
    startSaveImpl();
}

unsigned int SimpleDownload::dialogThread(void* param)
{
    SimpleDownload* self = (SimpleDownload*)param;

    OPENFILENAMEW ofn = { 0 };
    std::vector<wchar_t>* fileResult = new std::vector<wchar_t>();
    fileResult->resize(4 * MAX_PATH + 1);

    std::wstring defaultSaveName = getSaveName(self->m_contentDisposition, self->m_url);
    if (defaultSaveName.size() > 150)
        defaultSaveName = defaultSaveName.substr(0, 150);
    wcscpy(&fileResult->at(0), defaultSaveName.c_str());

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = &fileResult->at(0);
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"All\0*.*\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = base::UTF8ToWide(self->dialogOpt.defaultPath).c_str();
    ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;

    if (!GetSaveFileNameW(&ofn)) {
        delete fileResult;
        fileResult = nullptr;
    }

    wkeWebView wkeWebview = self->m_wkeView;

    content::postTaskToUiThread(FROM_HERE, NULL, [wkeWebview] {
        if (!wkeIsWebviewValid(wkeWebview))
            return;
        content::WebPage* page = wkeWebview->webPage();
        if (page)
            page->setIsMouseKeyMessageEnable(true);
    });

    content::postTaskToUiThread(FROM_HERE, NULL, [self, fileResult] {
        self->startSave(fileResult);
    });

    InterlockedDecrement((long *)&m_dialogCount);

    return 0;
}

#endif // download_SimpleDownload_h
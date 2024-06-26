
#include "download/SimpleDownload.h"

#include "common/ThreadCall.h"
#include "common/LiveIdDetect.h"
#include "download/DownloadUtil.h"
#include "wke/wkedefine.h"
#include "base/strings/string_util.h"
#include <process.h>
#include <shlwapi.h>

namespace download {

int SimpleDownload::getDialogCount()
{
    return m_dialogCount;
}

void SimpleDownload::onSimpleDownloadWillRedirectCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr oldRequest, wkeWebUrlRequestPtr request, wkeWebUrlResponsePtr redirectResponse)
{
}

void SimpleDownload::onSimpleDownloadDidReceiveResponseCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, wkeWebUrlResponsePtr response)
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

void SimpleDownload::onSimpleDownloadDidReceiveDataCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, const char* data, int dataLength)
{
    SimpleDownload* self = (SimpleDownload*)param;

    HANDLE hFile = self->m_handleOfSave;
    if (!hFile || INVALID_HANDLE_VALUE == hFile)
        return;

    DWORD numberOfBytesWrite = 0;
    BOOL b = ::WriteFile(hFile, data, dataLength, &numberOfBytesWrite, nullptr);
    if (!b) {
        ::CloseHandle(hFile);
        self->m_handleOfSave = nullptr;
        return;
    }

    self->m_downloadedSize += numberOfBytesWrite;
}

void SimpleDownload::onSimpleDownloadDidFailCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, const utf8* error)
{
    SimpleDownload* self = (SimpleDownload*)param;
    HANDLE hFile = self->m_handleOfSave;
    if (hFile && INVALID_HANDLE_VALUE != hFile)
        ::CloseHandle(hFile);
    self->m_handleOfSave = nullptr;

    delete self;
}

void SimpleDownload::onSimpleDownloadDidFinishLoadingCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, double finishTime)
{
    SimpleDownload* self = (SimpleDownload*)param;
    HANDLE hFile = self->m_handleOfSave;
    if (hFile && INVALID_HANDLE_VALUE != hFile)
        ::CloseHandle(hFile);
    self->m_handleOfSave = nullptr;
    self->m_totalSize = self->m_downloadedSize;

    delete self;
}

void SimpleDownload::onDataRecv(void* param, mbNetJob job, const char* data, int length)
{
    SimpleDownload* self = (SimpleDownload*)param;
    self->onDataRecvImpl(job, data, length);
}

void SimpleDownload::onDataFinish(void* param, mbNetJob job, mbLoadingResult result)
{
    SimpleDownload* self = (SimpleDownload*)param;
    self->onDataFinishImpl(job, result);
}

int SimpleDownload::m_dialogCount = 0;

SimpleDownload::SimpleDownload(mbWebView mbView,
    size_t expectedContentLength,
    const char* url,
    const char* mime,
    const char* disposition,
    mbNetJob job,
    mbNetJobDataBind* dataBind,
    mbDownloadBind* callbackBind)
{
    m_totalSize = expectedContentLength;
    m_url = url;
    m_mime = mime;
    m_contentDisposition = disposition;
    m_handleOfSave = nullptr;
    m_mbView = mbView;
    m_hasFinish = false;
    m_loadingResult = (mbLoadingResult)-1;

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

SimpleDownload* SimpleDownload::create(mbWebView webView,
    const WCHAR* savePath,
    const mbDialogOptions* dialogOpt,
    const mbDownloadOptions* downloadOpt,
    size_t expectedContentLength,
    const char* url,
    const char* mime,
    const char* disposition,
    mbNetJob job,
    mbNetJobDataBind* dataBind,
    mbDownloadBind* callbackBind)
{
    if (m_dialogCount > 0)
        return nullptr;

    SimpleDownload* self = new SimpleDownload(webView, expectedContentLength, url, mime, disposition, job, dataBind, callbackBind);

    if (dialogOpt && dialogOpt->magic == 'mbdo' && dialogOpt->defaultPath)
        self->dialogOpt.defaultPath = dialogOpt->defaultPath;

    if (savePath) {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webView);
        webview->setIsMouseKeyMessageEnable(true);

        std::vector<wchar_t>* fileResult = nullptr;
        size_t pathLen = wcslen(savePath);
        if (pathLen > 0) {
            fileResult = new std::vector<wchar_t>();
            fileResult->resize(MAX_PATH * 2);

            memcpy(&fileResult->at(0), savePath, pathLen * sizeof(WCHAR));

            if (!(downloadOpt && downloadOpt->magic == 'mbdo' && downloadOpt->saveAsPathAndName)) {
                std::wstring defaultSaveName = getSaveName(self->m_contentDisposition, self->m_url);
                PathAppendW(&fileResult->at(0), defaultSaveName.c_str());
            }
        }

        common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [self, fileResult] {
            self->startSave(fileResult);
        });

    } else {
        InterlockedIncrement((long*)&m_dialogCount);

        unsigned int threadIdentifier = 0;
        HANDLE hHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, SimpleDownload::dialogThread, self, 0, &threadIdentifier));
        ::CloseHandle(hHandle);
    }

    return self;
}

bool SimpleDownload::canSave()
{
    if (m_hasFinish && (MB_LOADING_FAILED == m_loadingResult || MB_LOADING_CANCELED == m_loadingResult))
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

            m_loadingResult = MB_LOADING_FAILED;

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

    char* output = (char*)malloc(0x1000);
    sprintf(output, "SimpleDownload::startSaveImpl: %p %d, %d\n", m_callbackBind.recvCallback, canSave(), m_cacheData.size());
    OutputDebugStringA(output);
    free(output);

    if (m_callbackBind.recvCallback && canSave() && 0 != m_cacheData.size())
        m_callbackBind.recvCallback(m_callbackBind.param, nullptr, &m_cacheData[0], (int)m_cacheData.size());

    BOOL b = FALSE;
    DWORD numberOfBytesWrite = 0;
    if (0 != m_cacheData.size())
        b = ::WriteFile(m_handleOfSave, &m_cacheData[0], (DWORD)m_cacheData.size(), &numberOfBytesWrite, nullptr);
    m_cacheData.clear();

    if (!m_hasFinish)
        return;

    ::CloseHandle(m_handleOfSave);
    m_handleOfSave = nullptr;

    if (m_callbackBind.finishCallback)
        m_callbackBind.finishCallback(m_callbackBind.param, nullptr, m_loadingResult);

    SimpleDownload* self = this;
    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [self] {
        delete self;
    });
}

void SimpleDownload::startSave(std::vector<wchar_t>* path)
{
    if (!path || 0 == path->size()) {
        m_loadingResult = (mbLoadingResult)-1;
        if (m_callbackBind.finishCallback)
            m_callbackBind.finishCallback(m_callbackBind.param, nullptr, m_loadingResult);
        return;
    }
    m_savePath = &path->at(0);
    delete path;

    startSaveImpl();
}

void SimpleDownload::onDataRecvImpl(mbNetJob job, const char* data, int length)
{
    if (0 == length)
        return;

    size_t oldSize = m_cacheData.size();
    m_cacheData.resize(oldSize + length);
    memcpy(&m_cacheData[oldSize], data, length);

    m_downloadedSize += length;

    startSaveImpl();

    if (MB_LOADING_FAILED == m_loadingResult)
        mbNetCancelRequest(job);
}

void SimpleDownload::onDataFinishImpl(mbNetJob job, mbLoadingResult result)
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

    mbWebView mbWebview = self->m_mbView;

    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [mbWebview] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)mbWebview);
        if (!webview)
            return;
        webview->setIsMouseKeyMessageEnable(true);
    });

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [self, fileResult] {
        self->startSave(fileResult);
    });

    InterlockedDecrement((long*)&m_dialogCount);

    return 0;
}

}
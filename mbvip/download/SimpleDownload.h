
#ifndef download_SimpleDownload_h
#define download_SimpleDownload_h

#include "common/ThreadCall.h"
#include "common/LiveIdDetect.h"
#include "wke/wkedefine.h"
#include <process.h>
#include <shlwapi.h>

namespace download {

class SimpleDownload {
public:
    static SimpleDownload* create(mbWebView webView,
        const WCHAR* savePath,
        const mbDialogOptions* dialogOpt,
        const mbDownloadOptions* downloadOpt,
        size_t expectedContentLength,
        const char* url,
        const char* mime,
        const char* disposition,
        mbNetJob job,
        mbNetJobDataBind* dataBind,
        mbDownloadBind* callbackBind);

    ~SimpleDownload();

    static unsigned int WINAPI dialogThread(void* param);

    static int getDialogCount();
private:
    SimpleDownload(mbWebView mbView,
        size_t expectedContentLength,
        const char* url,
        const char* mime,
        const char* disposition,
        mbNetJob job,
        mbNetJobDataBind* dataBind,
        mbDownloadBind* callbackBind);

    void startSave(std::vector<wchar_t>* path);
    void startSaveImpl();
    bool canSave();

    static void WKE_CALL_TYPE onSimpleDownloadWillRedirectCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr oldRequest, wkeWebUrlRequestPtr request, wkeWebUrlResponsePtr redirectResponse);
    static void WKE_CALL_TYPE onSimpleDownloadDidReceiveResponseCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, wkeWebUrlResponsePtr response);
    static void WKE_CALL_TYPE onSimpleDownloadDidReceiveDataCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, const char* data, int dataLength);
    static void WKE_CALL_TYPE onSimpleDownloadDidFailCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, const utf8* error);
    static void WKE_CALL_TYPE onSimpleDownloadDidFinishLoadingCallback(wkeWebView webView, void* param, wkeWebUrlRequestPtr request, double finishTime);
    static void MB_CALL_TYPE onDataRecv(void* param, mbNetJob job, const char* data, int length);
    static void MB_CALL_TYPE onDataFinish(void* param, mbNetJob job, mbLoadingResult result);

    void onDataRecvImpl(mbNetJob job, const char* data, int length);
    void onDataFinishImpl(mbNetJob job, mbLoadingResult result);

    static int m_dialogCount;

    std::string m_url;
    std::string m_mime;
    std::string m_contentDisposition;
    std::wstring m_savePath;

    HANDLE m_handleOfSave;
    size_t m_totalSize;
    size_t m_downloadedSize;

    mbWebView m_mbView;

    std::vector<char> m_cacheData;
    bool m_hasFinish;
    mbLoadingResult m_loadingResult;

    struct DialogOptions {
        std::string title;
        std::string defaultPath;
        std::string buttonLabel;
    };
    DialogOptions dialogOpt;

    mbDownloadBind m_callbackBind;
};

}

#endif // download_SimpleDownload_h
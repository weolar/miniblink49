
#ifndef Download_DownloadMgr_h
#define Download_DownloadMgr_h

#include "core/mb.h"
#include <string>
#include <vector>
#include "wke/wkedefine.h"

class DownloadMgr {
public:
    DownloadMgr();
    void onNewDownloadItem(const char* url, const char* mime, const char* contentDisposition);

    static mbDownloadOpt simpleDownload(mbWebView webView,
        const WCHAR* path,
        size_t expectedContentLength,
        const char* url,
        const char* mime,
        const char* disposition,
        mbNetJob job,
        mbNetJobDataBind* dataBind,
        mbDownloadBind* callbackBind);

    class DownloadItem {
    public:
        DownloadItem(DownloadMgr* parent, const char* url, const char* mime, const char* contentDisposition, int id)
        {
            m_url = url;
            m_mime = mime;
            m_contentDisposition = contentDisposition;
            m_id = id;
            m_handleOfSave = nullptr;
            m_parent = parent;
            m_totalSize = 0;
            m_downloadedSize = 0;
            m_progress = 0.0;
        }
        std::string m_url;
        std::string m_mime;
        std::string m_contentDisposition;
        std::wstring m_savePath;
        int m_id;
        HANDLE m_handleOfSave;
        size_t m_totalSize;
        size_t m_downloadedSize;
        DownloadMgr* m_parent;

        float m_progress;
    };

    mbWebView getView() const
    {
        return m_mbView;
    }

    DownloadItem* findById(int id);

    bool createWnd();

private:
    void startDownload();
    
    void onNewDownloadItemImpl(DownloadItem* item);
    void onSeletePathFinish(std::vector<wchar_t>* path);
    void runSelectFilePathDialog();
    void cancelSelectFilePath();
    void close();
    void openFile(int id);
    static unsigned int MB_CALL_TYPE seleteDialogThread(void* param);
    static void MB_CALL_TYPE onJsQueryCallback(mbWebView webView, void* param, mbJsExecState es, int64_t queryId, int customMsg, const utf8* request);
    static void MB_CALL_TYPE onDocumentReadyCallback(mbWebView webView, void* param, mbWebFrameHandle frameId);
    
    mbWebView m_mbView;
    int m_idGen;
    std::vector<DownloadItem*> m_items;
    HANDLE m_selectDialogThreadHandle;
    DownloadItem* m_curSeleteItem;
    bool m_scriptContextReady;
};

#endif

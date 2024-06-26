
#ifndef CallbackClosure_h
#define CallbackClosure_h

#include <functional>
#include "wke/wkedefine.h"

namespace mb {

class CallbackClosure {
public:

    ~CallbackClosure()
    {

    }

    std::function<bool(mbNavigationType, const char*)>* m_navigationClosure{ nullptr };
    void setNavigationClosure(std::function<bool(mbNavigationType, const char*)>* closure)
    {
        if (m_navigationClosure)
            delete m_navigationClosure;
        m_navigationClosure = closure;
    }

    std::function<void(mbWebFrameHandle)>* m_documentReadyClosure{ nullptr };
    void setDocumentReadyClosure(std::function<void(mbWebFrameHandle)>* closure)
    {
        if (m_documentReadyClosure)
            delete m_documentReadyClosure;
        m_documentReadyClosure = closure;
    }

    std::function<mbWebView(mbNavigationType, const utf8*, const mbWindowFeatures*)>* m_createViewClosure{ nullptr };
    void setCreateViewClosure(std::function<mbWebView(mbNavigationType, const utf8*, const mbWindowFeatures*)>* closure)
    {
        if (m_createViewClosure)
            delete m_createViewClosure;
        m_createViewClosure = closure;
    }

    std::function<bool(const char *url, void *job)>* m_loadUrlBeginClosure{ nullptr };
    void setLoadUrlBeginClosure(std::function<bool(const char *url, void *job)>* closure)
    {
        if (m_loadUrlBeginClosure)
            delete m_loadUrlBeginClosure;
        m_loadUrlBeginClosure = closure;
    }

    std::function<void(const char *url, void*, void*, int)>* m_loadUrlEndClosure{ nullptr };
    void setLoadUrlEndClosure(std::function<void(const char *url, void*, void*, int)>* closure)
    {
        if (m_loadUrlEndClosure)
            delete m_loadUrlEndClosure;
        m_loadUrlEndClosure = closure;
    }

    std::function<void(const char*)>* m_titleChangedClosure{ nullptr };
    void setTitleChangedClosure(std::function<void(const char*)>* closure)
    {
        if (m_titleChangedClosure)
            delete m_titleChangedClosure;
        m_titleChangedClosure = closure;
    }

// #define GEN_MB_CALLBACK(name) \
//     mb##name##Callback m_##name##Callback { nullptr }; \
//     void* m_##name##Param { nullptr }; \
//     void set##name##Callback(mb##name##Callback callback, void* param) { \
//         m_##name##Callback = callback; \
//         m_##name##Param = param; \
//     }
// 
//     GEN_MB_CALLBACK(Navigation);
//     GEN_MB_CALLBACK(DocumentReady);
//     GEN_MB_CALLBACK(LoadUrlBegin);
//     GEN_MB_CALLBACK(LoadUrlEnd);
//     GEN_MB_CALLBACK(TitleChanged);
//     GEN_MB_CALLBACK(URLChanged);
//     GEN_MB_CALLBACK(LoadingFinish);
//     GEN_MB_CALLBACK(PaintUpdated);
//     GEN_MB_CALLBACK(CreateView);
//     GEN_MB_CALLBACK(Download);
//     GEN_MB_CALLBACK(DownloadInBlinkThread);
//     GEN_MB_CALLBACK(AlertBox);
//     GEN_MB_CALLBACK(ConfirmBox);
//     GEN_MB_CALLBACK(PromptBox);
//     GEN_MB_CALLBACK(Console);
//     GEN_MB_CALLBACK(NetGetFavicon);
// 	GEN_MB_CALLBACK(Close);
// 
// #undef GEN_MB_CALLBACK

//     mbLoadingFinishCallback m_LoadingFinishCallback;
//     void* m_LoadingFinishParam;
//     void setLoadingFinish(mbLoadingFinishCallback callback, void* param)
//     {
//         m_LoadingFinishCallback = callback;
//         m_LoadingFinishParam = param;
//     }

    mbNavigationCallback m_NavigationCallback{ nullptr }; void* m_NavigationParam{ nullptr }; void setNavigationCallback(mbNavigationCallback callback, void* param) {
        m_NavigationCallback = callback; m_NavigationParam = param;
    };
    mbNavigationCallback m_NavigationSyncCallback{ nullptr }; void* m_NavigationSyncParam{ nullptr }; void setNavigationSyncCallback(mbNavigationCallback callback, void* param) {
        m_NavigationSyncCallback = callback; m_NavigationSyncParam = param;
    };
	mbDocumentReadyCallback m_DocumentReadyCallback{ nullptr }; void* m_DocumentReadyParam{ nullptr }; void setDocumentReadyCallback(mbDocumentReadyCallback callback, void* param) { m_DocumentReadyCallback = callback; m_DocumentReadyParam = param; };
    mbDocumentReadyCallback m_DocumentReadyInBlinkCallback{ nullptr }; void* m_DocumentReadyInBlinkParam{ nullptr }; void setDocumentReadyInBlinkCallback(mbDocumentReadyCallback callback, void* param) { m_DocumentReadyInBlinkCallback = callback; m_DocumentReadyInBlinkParam = param; };
	mbLoadUrlBeginCallback m_LoadUrlBeginCallback{ nullptr }; void* m_LoadUrlBeginParam{ nullptr }; void setLoadUrlBeginCallback(mbLoadUrlBeginCallback callback, void* param) { m_LoadUrlBeginCallback = callback; m_LoadUrlBeginParam = param; };
	mbLoadUrlEndCallback m_LoadUrlEndCallback{ nullptr }; void* m_LoadUrlEndParam{ nullptr }; void setLoadUrlEndCallback(mbLoadUrlEndCallback callback, void* param) { m_LoadUrlEndCallback = callback; m_LoadUrlEndParam = param; };
    mbLoadUrlFailCallback m_LoadUrlFailCallback{ nullptr }; void* m_LoadUrlFailParam{ nullptr }; void setLoadUrlFailCallback(mbLoadUrlFailCallback callback, void* param) { m_LoadUrlFailCallback = callback; m_LoadUrlFailParam = param; };
    mbLoadUrlHeadersReceivedCallback m_LoadUrlHeadersReceivedCallback{ nullptr }; void* m_LoadUrlHeadersReceivedParam{ nullptr }; void setLoadUrlHeadersReceivedCallback(mbLoadUrlHeadersReceivedCallback callback, void* param) { m_LoadUrlFailCallback = callback; m_LoadUrlHeadersReceivedParam = param; };    
    mbLoadUrlFinishCallback m_LoadUrlFinishCallback{ nullptr }; void* m_LoadUrlFinishParam{ nullptr }; void setLoadUrlFinishCallback(mbLoadUrlFinishCallback callback, void* param) { m_LoadUrlFinishCallback = callback; m_LoadUrlFinishParam = param; };

	mbTitleChangedCallback m_TitleChangedCallback{ nullptr }; void* m_TitleChangedParam{ nullptr }; void setTitleChangedCallback(mbTitleChangedCallback callback, void* param) { m_TitleChangedCallback = callback; m_TitleChangedParam = param; };
    mbMouseOverUrlChangedCallback m_MouseOverUrlChangedCallback{ nullptr }; void* m_MouseOverUrlChangedParam{ nullptr }; void setMouseOverUrlChangedCallback(mbMouseOverUrlChangedCallback callback, void* param) { m_MouseOverUrlChangedCallback = callback; m_MouseOverUrlChangedParam = param; };
	mbURLChangedCallback m_URLChangedCallback{ nullptr }; void* m_URLChangedParam{ nullptr }; void setURLChangedCallback(mbURLChangedCallback callback, void* param) { m_URLChangedCallback = callback; m_URLChangedParam = param; };
	mbLoadingFinishCallback m_LoadingFinishCallback{ nullptr }; void* m_LoadingFinishParam{ nullptr }; void setLoadingFinishCallback(mbLoadingFinishCallback callback, void* param) { m_LoadingFinishCallback = callback; m_LoadingFinishParam = param; };
	mbPaintUpdatedCallback m_PaintUpdatedCallback{ nullptr }; void* m_PaintUpdatedParam{ nullptr }; void setPaintUpdatedCallback(mbPaintUpdatedCallback callback, void* param) { m_PaintUpdatedCallback = callback; m_PaintUpdatedParam = param; };
    mbPaintBitUpdatedCallback m_PaintBitUpdatedCallback{ nullptr }; void* m_PaintBitUpdatedParam{ nullptr }; void setPaintBitUpdatedCallback(mbPaintBitUpdatedCallback callback, void* param) {
        m_PaintBitUpdatedCallback = callback; m_PaintBitUpdatedParam = param;
    };
	mbCreateViewCallback m_CreateViewCallback{ nullptr }; void* m_CreateViewParam{ nullptr }; void setCreateViewCallback(mbCreateViewCallback callback, void* param) { m_CreateViewCallback = callback; m_CreateViewParam = param; };
	mbDownloadCallback m_DownloadCallback{ nullptr }; void* m_DownloadParam{ nullptr }; void setDownloadCallback(mbDownloadCallback callback, void* param) { m_DownloadCallback = callback; m_DownloadParam = param; };
	mbDownloadInBlinkThreadCallback m_DownloadInBlinkThreadCallback{ nullptr }; void* m_DownloadInBlinkThreadParam{ nullptr }; void setDownloadInBlinkThreadCallback(mbDownloadInBlinkThreadCallback callback, void* param) { m_DownloadInBlinkThreadCallback = callback; m_DownloadInBlinkThreadParam = param; };
	mbAlertBoxCallback m_AlertBoxCallback{ nullptr }; void* m_AlertBoxParam{ nullptr }; void setAlertBoxCallback(mbAlertBoxCallback callback, void* param) { m_AlertBoxCallback = callback; m_AlertBoxParam = param; };
	mbConfirmBoxCallback m_ConfirmBoxCallback{ nullptr }; void* m_ConfirmBoxParam{ nullptr }; void setConfirmBoxCallback(mbConfirmBoxCallback callback, void* param) { m_ConfirmBoxCallback = callback; m_ConfirmBoxParam = param; };
	mbPromptBoxCallback m_PromptBoxCallback{ nullptr }; void* m_PromptBoxParam{ nullptr }; void setPromptBoxCallback(mbPromptBoxCallback callback, void* param) { m_PromptBoxCallback = callback; m_PromptBoxParam = param; };
	mbConsoleCallback m_ConsoleCallback{ nullptr }; void* m_ConsoleParam{ nullptr }; void setConsoleCallback(mbConsoleCallback callback, void* param) { m_ConsoleCallback = callback; m_ConsoleParam = param; };
	mbNetGetFaviconCallback m_NetGetFaviconCallback{ nullptr }; void* m_NetGetFaviconParam{ nullptr }; void setNetGetFaviconCallback(mbNetGetFaviconCallback callback, void* param) { m_NetGetFaviconCallback = callback; m_NetGetFaviconParam = param; };
	mbCloseCallback m_ClosingCallback{ nullptr }; void* m_ClosingParam{ nullptr }; void setCloseCallback(mbCloseCallback callback, void* param) { m_ClosingCallback = callback; m_ClosingParam = param; };
    mbDidCreateScriptContextCallback m_DidCreateScriptContextCallback{ nullptr }; void* m_DidCreateScriptContextParam{ nullptr }; void setDidCreateScriptContextCallback(mbDidCreateScriptContextCallback callback, void* param) { m_DidCreateScriptContextCallback = callback; m_DidCreateScriptContextParam = param; };
    mbWillReleaseScriptContextCallback m_WillReleaseScriptContextCallback{ nullptr }; void* m_WillReleaseScriptContextParam{ nullptr }; void setWillReleaseScriptContextCallback(mbWillReleaseScriptContextCallback callback, void* param) { m_WillReleaseScriptContextCallback = callback; m_WillReleaseScriptContextParam = param; };
    mbImageBufferToDataURLCallback m_ImageBufferToDataURLCallback{ nullptr }; void* m_ImageBufferToDataURLParam{ nullptr }; void setImageBufferToDataURLCallback(mbImageBufferToDataURLCallback callback, void* param) { m_ImageBufferToDataURLCallback = callback; m_ImageBufferToDataURLParam = param; };
    mbNetResponseCallback m_NetResponseCallback{ nullptr }; void* m_NetResponseParam{ nullptr }; void setNetResponseCallback(mbNetResponseCallback callback, void* param) { m_NetResponseCallback = callback; m_NetResponseParam = param; };
    mbNetViewLoadInfoCallback m_NetViewLoadInfoCallback{ nullptr }; void* m_NetViewLoadInfoParam{ nullptr }; void setNetViewLoadInfoCallback(mbNetViewLoadInfoCallback callback, void* param) { m_NetViewLoadInfoCallback = callback; m_NetViewLoadInfoParam = param; };

    std::function<void(mbJsExecState es, int64_t queryId, int customMsg, const utf8* request)>* m_jsQueryClosure{ nullptr };
    void setJsQueryClosure(std::function<void(mbJsExecState es, int64_t queryId, int customMsg, const utf8* request)>* closure)
    {
        if (m_jsQueryClosure)
            delete m_jsQueryClosure;
        m_jsQueryClosure = closure;
    }
};


}

#endif // CallbackClosure_h
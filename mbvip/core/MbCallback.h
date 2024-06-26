
mbImageBufferToDataURLCallback g_imageBufferToDataURLCallback = nullptr;
void* g_imageBufferToDataURLParam = nullptr;

void WKE_CALL_TYPE onMouseOverUrlChanged(wkeWebView wkeWebview, void* param, const wkeString title)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;

    std::string* titleString = new std::string(wkeGetString(title));
    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [webviewHandle, titleString] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (!webview) {
            delete titleString;
            return;
        }

        webview->setTitle(*titleString);
        if (webview->getClosure().m_MouseOverUrlChangedCallback)
            webview->getClosure().m_MouseOverUrlChangedCallback(webviewHandle, webview->getClosure().m_MouseOverUrlChangedParam, titleString->c_str());

        delete titleString;
    });
}

static bool WKE_CALL_TYPE onLoadUrlBegin(wkeWebView wkeWebview, void* param, const char* url, void* job)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return false;

    if (mb::MbWebView::kPageInited != webview->getState())
        return false;

    if (webview->handleResPacket(url, job))
        return true;

    BOOL b = webview->getClosure().m_LoadUrlBeginCallback(webviewHandle, webview->getClosure().m_LoadUrlBeginParam, url, job);
    return !!b;
}

static void WKE_CALL_TYPE onLoadUrlEnd(wkeWebView wkeWebview, void* param, const char* url, void* job, void* buf, int len)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;
    if (mb::MbWebView::kPageInited != webview->getState())
        return;

    webview->getClosure().m_LoadUrlEndCallback(webviewHandle, webview->getClosure().m_LoadUrlEndParam, url, job, buf, len);
}

static void WKE_CALL_TYPE onLoadUrlFail(wkeWebView wkeWebview, void* param, const char* url, void* job)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;
    if (mb::MbWebView::kPageInited != webview->getState())
        return;

    webview->getClosure().m_LoadUrlFailCallback(webviewHandle, webview->getClosure().m_LoadUrlFailParam, url, job);
}

static void WKE_CALL_TYPE onLoadUrlHeadersReceived(wkeWebView wkeWebview, void* param, const char* url, void* job)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;
    if (mb::MbWebView::kPageInited != webview->getState())
        return;

    webview->getClosure().m_LoadUrlHeadersReceivedCallback(webviewHandle, webview->getClosure().m_LoadUrlHeadersReceivedParam, url, job);
}

static void WKE_CALL_TYPE onLoadUrlFinish(wkeWebView wkeWebview, void* param, const char* url, void* job, int len)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;
    if (mb::MbWebView::kPageInited != webview->getState())
        return;

    webview->getClosure().m_LoadUrlFinishCallback(webviewHandle, webview->getClosure().m_LoadUrlFinishParam, url, job, len);
}

static void WKE_CALL_TYPE onLoadingFinish(wkeWebView wkeWebview, void* param, const wkeString url, wkeLoadingResult result, const wkeString failedReason)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;

    std::string* urlString = new std::string(wkeGetString(url));
    std::string* failedReasonString = new std::string(wkeGetString(failedReason));

    wkeTempCallbackInfo* temInfo = wkeGetTempCallbackInfo(wkeWebview);
    mbWebFrameHandle mbFrameId = mb::MbWebView::toMbFrameHandle(wkeWebview, temInfo->frame);

    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [webviewHandle, mbFrameId, urlString, result, failedReasonString] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview) {
        delete urlString;
        delete failedReasonString;
        return;
    }
    webview->getClosure().m_LoadingFinishCallback(webviewHandle, webview->getClosure().m_LoadingFinishParam, mbFrameId, urlString->c_str(), (mbLoadingResult)result, 
        failedReasonString->c_str());
        delete urlString;
        delete failedReasonString;
    });
}

void WKE_CALL_TYPE onDidCreateScriptContextCallback(wkeWebView wkeWebview, void* param, wkeWebFrameHandle frameId, void* context, int extensionGroup, int worldId)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;

    void* origParam = webview->getClosure().m_DidCreateScriptContextParam;
    webview->getClosure().m_DidCreateScriptContextCallback(webviewHandle, origParam, (mbWebFrameHandle)frameId, context, extensionGroup, worldId);
}

wkeString WKE_CALL_TYPE onImageBufferToDataURLCallback(wkeWebView wkeWebview, void* param, const char* data, size_t size)
{
    //     mbWebView webviewHandle = (mbWebView)param;
    //     mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    //     if (!webview)
    //         return nullptr;
    // 
    //     void* origParam = webview->getClosure().m_ImageBufferToDataURLParam;
    //     return (wkeString)webview->getClosure().m_ImageBufferToDataURLCallback(webviewHandle, origParam, data, size);

    return (wkeString)g_imageBufferToDataURLCallback(NULL_WEBVIEW, g_imageBufferToDataURLParam, data, size);
}

void MB_CALL_TYPE mbOnNavigation(mbWebView webviewHandle, mbNavigationCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setNavigationCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnNavigation(webview->getWkeWebView(), onNavigation, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnNavigationSync(mbWebView webviewHandle, mbNavigationCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setNavigationSyncCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnNavigation(webview->getWkeWebView(), onNavigationSync, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnDocumentReady(mbWebView webviewHandle, mbDocumentReadyCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setDocumentReadyCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnDocumentReady2(webview->getWkeWebView(), onDocumentReady, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnDocumentReadyInBlinkThread(mbWebView webviewHandle, mbDocumentReadyCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setDocumentReadyInBlinkCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnDocumentReady2(webview->getWkeWebView(), onDocumentReady, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnLoadUrlBegin(mbWebView webviewHandle, mbLoadUrlBeginCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setLoadUrlBeginCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnLoadUrlBegin(webview->getWkeWebView(), onLoadUrlBegin, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnLoadUrlEnd(mbWebView webviewHandle, mbLoadUrlEndCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setLoadUrlEndCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnLoadUrlEnd(webview->getWkeWebView(), onLoadUrlEnd, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnLoadUrlFail(mbWebView webviewHandle, mbLoadUrlFailCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setLoadUrlFailCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnLoadUrlFail(webview->getWkeWebView(), onLoadUrlFail, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnLoadUrlHeadersReceived(mbWebView webviewHandle, mbLoadUrlHeadersReceivedCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setLoadUrlHeadersReceivedCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnLoadUrlHeadersReceived(webview->getWkeWebView(), onLoadUrlHeadersReceived, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnLoadUrlFinish(mbWebView webviewHandle, mbLoadUrlFinishCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setLoadUrlFinishCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnLoadUrlFinish(webview->getWkeWebView(), onLoadUrlFinish, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnURLChanged(mbWebView webviewHandle, mbURLChangedCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setURLChangedCallback(callback, param);
    //common::ThreadCall::callBlinkThreadAsyncWithValid(webview, [webview] { wkeOnURLChanged2(webview->getWkeWebView(), onURLChanged, webview); });
}

void MB_CALL_TYPE mbOnTitleChanged(mbWebView webviewHandle, mbTitleChangedCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setTitleChangedCallback(callback, param);
    //common::ThreadCall::callBlinkThreadAsyncWithValid(webview, [webview] { wkeOnTitleChanged(webview->getWkeWebView(), onTitleChanged, webview); }); 
}

void MB_CALL_TYPE mbOnMouseOverUrlChanged(mbWebView webviewHandle, mbMouseOverUrlChangedCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setTitleChangedCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnMouseOverUrlChanged(webview->getWkeWebView(), onMouseOverUrlChanged, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnLoadingFinish(mbWebView webviewHandle, mbLoadingFinishCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setLoadingFinishCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnLoadingFinish(webview->getWkeWebView(), onLoadingFinish, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnDownload(mbWebView webviewHandle, mbDownloadCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setDownloadCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnDownload(webview->getWkeWebView(), onDownload, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnDownloadInBlinkThread(mbWebView webviewHandle, mbDownloadInBlinkThreadCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setDownloadInBlinkThreadCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnDownload2(webview->getWkeWebView(), onDownloadInBlinkThread, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnAlertBox(mbWebView webviewHandle, mbAlertBoxCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setAlertBoxCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnAlertBox(webview->getWkeWebView(), onAlertBox, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnConfirmBox(mbWebView webviewHandle, mbConfirmBoxCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setConfirmBoxCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnConfirmBox(webview->getWkeWebView(), onConfirmBox, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnPromptBox(mbWebView webviewHandle, mbPromptBoxCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setPromptBoxCallback(callback, param);
    //common::ThreadCall::callBlinkThreadAsyncWithValid(webview, [webview] { wkeOnPromptBox(webview->getWkeWebView(), onPromptBox, webview); }); 
}

void MB_CALL_TYPE mbOnNetGetFavicon(mbWebView webviewHandle, mbNetGetFaviconCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setNetGetFaviconCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeNetGetFavicon(webview->getWkeWebView(), onNetGetFavicon, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnConsole(mbWebView webviewHandle, mbConsoleCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setConsoleCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnConsole(webview->getWkeWebView(), onConsole, (void*)webviewHandle);
    });
}

BOOL MB_CALL_TYPE mbOnClose(mbWebView webviewHandle, mbCloseCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return TRUE;
    webview->getClosure().setCloseCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnWindowClosing(webview->getWkeWebView(), onWindowClosingCallback, (void*)webviewHandle);
    });
    return TRUE;
}

static bool WKE_CALL_TYPE onNetResponseCallback(wkeWebView webView, void* param, const utf8* url, wkeNetJob job)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return false;

    return !!(webview->getClosure().m_NetResponseCallback(webviewHandle, webview->getClosure().m_NetResponseParam, url, (mbNetJob)job));
}

void MB_CALL_TYPE mbNetOnResponse(mbWebView webviewHandle, mbNetResponseCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setNetResponseCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeNetOnResponse(webview->getWkeWebView(), onNetResponseCallback, (void*)webviewHandle);
    });
}

void MB_CALL_TYPE mbOnDidCreateScriptContext(mbWebView webviewHandle, mbDidCreateScriptContextCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setDidCreateScriptContextCallback(callback, param);
}

void MB_CALL_TYPE mbOnWillReleaseScriptContext(mbWebView webviewHandle, mbWillReleaseScriptContextCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setWillReleaseScriptContextCallback(callback, param);
}

void MB_CALL_TYPE mbOnImageBufferToDataURL(mbWebView webviewHandle, mbImageBufferToDataURLCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setImageBufferToDataURLCallback(callback, param);

    g_imageBufferToDataURLCallback = callback;
    g_imageBufferToDataURLParam = param;

    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeSetDebugConfig(webview->getWkeWebView(), "setImageBufferToDataUrlCallback", (const char*)onImageBufferToDataURLCallback);
    });
}
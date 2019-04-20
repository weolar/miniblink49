// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/web_impl_win/WebMediaplayerImpl.h"

#include "content/browser/WebPageImpl.h"
#include "content/web_impl_win/npapi/PluginPackage.h"
#include "content/web_impl_win/npapi/PluginDatabase.h"
#include "third_party/WebKit/Source/wtf/Functional.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/platform/WebContentDecryptionModule.h"
#include "third_party/WebKit/public/platform/WebMediaSource.h"
#include "third_party/WebKit/public/platform/WebRect.h"
#include "third_party/WebKit/public/platform/WebSize.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/platform/WebURL.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebRuntimeFeatures.h"
#include "third_party/WebKit/public/web/WebSecurityOrigin.h"
#include "third_party/WebKit/public/web/WebView.h"
#include "v8.h"
#include "wke/wke.h"
#include "wke/wkeGlobalVar.h"
#include "wke/wkeMediaPlayer.h"
#include "third_party/npapi/bindings/npfunctions.h"
#include "third_party/WebKit/Source/bindings/core/v8/npruntime_impl.h"

using blink::WebCanvas;
using blink::WebMediaPlayer;
using blink::WebRect;
using blink::WebSize;
using blink::WebString;

extern NPNetscapeFuncs s_wkeBrowserFuncs;

namespace content {

class MediaPlayerClientWkeWrap : public wke::WkeMediaPlayerClient {
public:
    MediaPlayerClientWkeWrap(blink::WebViewImpl* view, blink::WebMediaPlayerClient* client)
    {
        m_view = view;
        m_client = client;
    }

    virtual void keyAdded(const char* keySystem, const char* sessionId) override
    {
        m_client->keyAdded(blink::WebString::fromUTF8(keySystem), blink::WebString::fromUTF8(sessionId));
    }

    virtual void keyError(const char* keySystem, const char* sessionId, wke::WkeMediaPlayerClient::MediaKeyErrorCode code, unsigned short systemCode) override
    {
        m_client->keyError(blink::WebString::fromUTF8(keySystem), blink::WebString::fromUTF8(sessionId), (blink::WebMediaPlayerClient::MediaKeyErrorCode)code, systemCode);
    }

    virtual void keyMessage(const char* keySystem, const char* sessionId, const unsigned char* message, unsigned messageLength, const char* defaultURL) override
    {
        m_client->keyMessage(blink::WebString::fromUTF8(keySystem), blink::WebString::fromUTF8(sessionId), 
            message, messageLength, blink::KURL(blink::ParsedURLString, defaultURL));
    }

    virtual void encrypted(wke::WkeMediaPlayerClient::EncryptedMediaInitDataType type, const unsigned char* initData, unsigned initDataLength) override
    {
        m_client->encrypted((blink::WebEncryptedMediaInitDataType)type, initData, initDataLength);
    }

    virtual void didBlockPlaybackWaitingForKey() override
    {
        m_client->didBlockPlaybackWaitingForKey();
    }

    virtual void didResumePlaybackBlockedForKey()override
    {
        m_client->didResumePlaybackBlockedForKey();
    }

    virtual void networkStateChanged() override
    {
        m_client->networkStateChanged();
    }

    virtual void readyStateChanged() override
    {
        m_client->readyStateChanged();
    }

    virtual void timeChanged() override
    {
        m_client->timeChanged();
    }

    virtual void repaint() override
    {
        m_client->repaint();
    }

    virtual void durationChanged() override
    {
        m_client->durationChanged();
    }

    virtual void sizeChanged() override
    {
        m_client->sizeChanged();
    }

    virtual void playbackStateChanged() override
    {
        m_client->playbackStateChanged();
    }

    virtual void setWebLayer(void* layer) override
    {
        m_client->setWebLayer((blink::WebLayer*)layer);
    }
    virtual wke::WkeMediaPlayer::TrackId addAudioTrack(const char* id, wke::WkeMediaPlayerClient::AudioTrackKind, const char* label, const char* language, bool enabled)
    {
        return 0;
    }
    virtual void removeAudioTrack(wke::WkeMediaPlayer::TrackId) {};
    virtual wke::WkeMediaPlayer::TrackId addVideoTrack(const char* id, wke::WkeMediaPlayerClient::VideoTrackKind, const char* label, const char* language, bool selected)
    {
        return 0;
    }
    virtual void removeVideoTrack(wke::WkeMediaPlayer::TrackId) {};
    virtual void addTextTrack(void*) {};
    virtual void removeTextTrack(void*) {};
    virtual void mediaSourceOpened(void*) {};
    virtual void requestSeek(double) {};
    virtual void remoteRouteAvailabilityChanged(bool) {};
    virtual void connectedToRemoteDevice() {};
    virtual void disconnectedFromRemoteDevice() {};

    virtual void didExitFullScreen() override
    {
        m_view->didExitFullScreen();
    }

private:
    blink::WebMediaPlayerClient* m_client;
    blink::WebViewImpl* m_view;
};

// static uint16_t getNPVersion()
// {
//     return NP_VERSION_MINOR;
// }
// 
// static NPNetscapeFuncs* getBrowserFuncs()
// {
//     static NPNetscapeFuncs* browserFuncs = nullptr;
//     if (browserFuncs)
//         return browserFuncs;
//     browserFuncs = new NPNetscapeFuncs();
// 
//     browserFuncs->size = sizeof(NPNetscapeFuncs);
//     browserFuncs->version = getNPVersion();
// 
//     browserFuncs->geturl = NPN_GetURL;
//     browserFuncs->posturl = NPN_PostURL;
//     browserFuncs->requestread = NPN_RequestRead;
//     browserFuncs->newstream = NPN_NewStream;
//     browserFuncs->write = NPN_Write;
//     browserFuncs->destroystream = NPN_DestroyStream;
//     browserFuncs->status = NPN_Status;
//     browserFuncs->uagent = NPN_UserAgent;
//     browserFuncs->memalloc = NPN_MemAlloc;
//     browserFuncs->memfree = NPN_MemFree;
//     browserFuncs->memflush = NPN_MemFlush;
//     browserFuncs->reloadplugins = NPN_ReloadPlugins;
//     browserFuncs->geturlnotify = NPN_GetURLNotify;
//     browserFuncs->posturlnotify = NPN_PostURLNotify;
//     browserFuncs->getvalue = NPN_GetValue;
//     browserFuncs->setvalue = NPN_SetValue;
//     browserFuncs->invalidaterect = NPN_InvalidateRect;
//     browserFuncs->invalidateregion = NPN_InvalidateRegion;
//     browserFuncs->forceredraw = NPN_ForceRedraw;
//     browserFuncs->getJavaEnv = NPN_GetJavaEnv;
//     browserFuncs->getJavaPeer = NPN_GetJavaPeer;
//     browserFuncs->pushpopupsenabledstate = NPN_PushPopupsEnabledState;
//     browserFuncs->poppopupsenabledstate = NPN_PopPopupsEnabledState;
//     browserFuncs->pluginthreadasynccall = NPN_PluginThreadAsyncCall;
//     browserFuncs->releasevariantvalue = _NPN_ReleaseVariantValue;
//     browserFuncs->getstringidentifier = _NPN_GetStringIdentifier;
//     browserFuncs->getstringidentifiers = _NPN_GetStringIdentifiers;
//     browserFuncs->getintidentifier = _NPN_GetIntIdentifier;
//     browserFuncs->identifierisstring = _NPN_IdentifierIsString;
//     browserFuncs->utf8fromidentifier = _NPN_UTF8FromIdentifier;
//     browserFuncs->intfromidentifier = _NPN_IntFromIdentifier;
//     browserFuncs->createobject = _NPN_CreateObject;
//     browserFuncs->retainobject = _NPN_RetainObject;
//     browserFuncs->releaseobject = _NPN_ReleaseObject;
//     browserFuncs->invoke = _NPN_Invoke;
//     browserFuncs->invokeDefault = _NPN_InvokeDefault;
//     browserFuncs->evaluate = _NPN_Evaluate;
//     browserFuncs->getproperty = _NPN_GetProperty;
//     browserFuncs->setproperty = _NPN_SetProperty;
//     browserFuncs->removeproperty = _NPN_RemoveProperty;
//     browserFuncs->hasproperty = _NPN_HasProperty;
//     browserFuncs->hasmethod = _NPN_HasMethod;
//     browserFuncs->setexception = _NPN_SetException;
//     browserFuncs->enumerate = _NPN_Enumerate;
//     browserFuncs->construct = _NPN_Construct;
//     browserFuncs->getvalueforurl = NPN_GetValueForURL;
//     browserFuncs->setvalueforurl = NPN_SetValueForURL;
//     browserFuncs->getauthenticationinfo = NPN_GetAuthenticationInfo;
//     browserFuncs->popupcontextmenu = NPN_PopUpContextMenu;
// 
//     return browserFuncs;
// }

WebMediaPlayerImpl::WebMediaPlayerImpl(blink::WebLocalFrame* frame, const blink::WebURL& url, blink::WebMediaPlayerClient* client)
{
    m_width = 50;
    m_height = 50;
    m_duration = 1.0;

    m_paused = false;
    m_seeking = false;
    m_hasVideo = true;
    m_hasAudio = false;
    m_readyState = blink::WebMediaPlayer::ReadyStateHaveNothing;
    m_client = client;
    m_wkeClientWrap = nullptr;
    m_wkePlayer = nullptr;
    m_memoryCanvas = nullptr;

    blink::WebViewImpl* view = (blink::WebViewImpl*)frame->view();
    WebPageImpl* page = (WebPageImpl*)view->client();
    if (!wke::g_wkeMediaPlayerFactory)
        return;

    String mime("application/x-shockwave-flash");
    PluginPackage* plugin = PluginDatabase::installedPlugins()->findPlugin(blink::KURL(), mime);

    // No plugin was found, try refreshing the database and searching again
    if (!plugin && PluginDatabase::installedPlugins()->refresh())
        plugin = PluginDatabase::installedPlugins()->findPlugin(blink::KURL(), mime);

    if (!plugin)
        return;

    plugin->load();

    m_wkeClientWrap = new MediaPlayerClientWkeWrap(view, m_client);

    if (1) {
        NPNetscapeFuncs browserFuncs = { 0 };
        memcpy(&browserFuncs, plugin->browserFuncs(), sizeof(NPNetscapeFuncs));
        m_wkePlayer = wke::g_wkeMediaPlayerFactory(page->wkeWebView(), m_wkeClientWrap, (void*)&browserFuncs, (void*)plugin->pluginFuncs());

        if (s_wkeBrowserFuncs.size != sizeof(NPNetscapeFuncs))
            memcpy(&s_wkeBrowserFuncs, &browserFuncs, sizeof(NPNetscapeFuncs));
    } else {
        m_wkePlayer = wke::g_wkeMediaPlayerFactory(page->wkeWebView(), m_wkeClientWrap, nullptr, nullptr);
    }
}

WebMediaPlayerImpl::~WebMediaPlayerImpl()
{
    for (size_t i = 0; i < m_asynLoadCancelNotifers.size(); ++i)
        *(m_asynLoadCancelNotifers[i]) = true;
    
    if (m_wkeClientWrap)
        delete m_wkeClientWrap;

    if (m_wkePlayer)
        m_wkePlayer->destroy();
}

void WebMediaPlayerImpl::load(blink::WebMediaPlayer::LoadType type, const blink::WebURL& url, blink::WebMediaPlayer::CORSMode mode)
{
    ASSERT(isMainThread());
    blink::KURL kurl(url);
    CString urlString = kurl.getUTF8String().utf8();

    if (wke::g_wkeWillMediaLoadCallback) {
        wkeMediaLoadInfo info = { 0 };
        
        wke::g_wkeWillMediaLoadCallback(nullptr, wke::g_wkeWillMediaLoadCallbackCallbackParam, urlString.data(), &info);
        m_width = info.width;
        m_height = info.height;
        m_duration = info.duration;
    }

    if (m_wkePlayer) {
        m_wkePlayer->load((wke::WkeMediaPlayer::LoadType)type, urlString.data(), (wke::WkeMediaPlayer::CORSMode)mode);
        return;
    }

    for (int i = 0; i <= blink::WebMediaPlayer::ReadyStateHaveEnoughData; ++i) {
        bool* cancelNotifer = new bool();
        *cancelNotifer = false;
        m_asynLoadCancelNotifers.append(cancelNotifer);

        blink::Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(&WebMediaPlayerImpl::onLoad, this, (blink::WebMediaPlayer::ReadyState)i, cancelNotifer));
    }
}

void WebMediaPlayerImpl::onLoad(blink::WebMediaPlayer::ReadyState readyState, bool* cancelNotifer)
{
    if (*cancelNotifer) {
        delete cancelNotifer;
        return;
    }

    size_t notiferPos = m_asynLoadCancelNotifers.find(cancelNotifer);
    delete cancelNotifer;
    m_asynLoadCancelNotifers.remove(notiferPos);

    m_readyState = readyState;
    m_client->readyStateChanged();
}

// Playback controls.
void WebMediaPlayerImpl::play()
{
    m_paused = false;
    if (m_wkePlayer) {
        m_wkePlayer->play();
        return;
    }
}

void WebMediaPlayerImpl::pause()
{
    m_paused = true;
    if (m_wkePlayer) {
        m_wkePlayer->pause();
        return;
    }
}

bool WebMediaPlayerImpl::supportsSave() const
{
    if (m_wkePlayer)
        return m_wkePlayer->supportsSave();
    return false;
}

void WebMediaPlayerImpl::seek(double seconds)
{
    if (m_wkePlayer)
        return m_wkePlayer->seek(seconds);
}

void WebMediaPlayerImpl::setRate(double rate)
{
    if (m_wkePlayer)
        return m_wkePlayer->setRate(rate);
}

void WebMediaPlayerImpl::setVolume(double volume)
{
    if (m_wkePlayer)
        return m_wkePlayer->setVolume(volume);
}

void WebMediaPlayerImpl::requestRemotePlayback()
{
    if (m_wkePlayer)
        return m_wkePlayer->requestRemotePlayback();
}

void WebMediaPlayerImpl::requestRemotePlaybackControl()
{
    if (m_wkePlayer)
        return m_wkePlayer->requestRemotePlaybackControl();
}

void WebMediaPlayerImpl::setPreload(Preload preload)
{
    if (m_wkePlayer)
        return m_wkePlayer->setPreload((wke::WkeMediaPlayer::Preload)preload);
}

static blink::WebTimeRanges getWebTimeRanges(wkeMemBuf* buffer)
{
    WTF::Vector<blink::WebTimeRange> ranges;
    if (!buffer)
        return ranges;
    size_t size = buffer->length / sizeof(wke::WkeMediaPlayer::MediaTimeRange);

    for (size_t i = 0; i < size; ++i) {
        wke::WkeMediaPlayer::MediaTimeRange* range = ((wke::WkeMediaPlayer::MediaTimeRange*)buffer->data) + i;
        ranges.append(blink::WebTimeRange(range->start, range->end));
    }
    wkeFreeMemBuf(buffer);
    return ranges;
}

blink::WebTimeRanges WebMediaPlayerImpl::buffered() const
{
    if (m_wkePlayer) {
        wkeMemBuf* buffer = m_wkePlayer->buffered();
        return getWebTimeRanges(buffer);
    }
    return WTF::Vector<blink::WebTimeRange>();
}

blink::WebTimeRanges WebMediaPlayerImpl::seekable() const
{
    if (m_wkePlayer) {
        wkeMemBuf* buffer = m_wkePlayer->seekable();
        return getWebTimeRanges(buffer);
    }
    return WTF::Vector<blink::WebTimeRange>();
}

void WebMediaPlayerImpl::setSinkId(const WebString& deviceId, blink::WebCallbacks<void, blink::WebSetSinkIdError>*)
{
    ;
}

bool WebMediaPlayerImpl::hasVideo() const
{
    if (m_wkePlayer)
        return m_wkePlayer->hasVideo();
    return m_hasVideo;
}

bool WebMediaPlayerImpl::hasAudio() const
{
    if (m_wkePlayer)
        return m_wkePlayer->hasAudio();
    return m_hasAudio;
}

bool WebMediaPlayerImpl::isRemote() const { return false; }

WebSize WebMediaPlayerImpl::naturalSize() const
{
    if (m_wkePlayer) {
        wkePoint pt = m_wkePlayer->naturalSize();
        return WebSize(pt.x, pt.y);
    }

    if (m_hasVideo)
        return WebSize(m_width, m_height);
    return WebSize(m_width, m_height);
}

bool WebMediaPlayerImpl::paused() const
{
    if (m_wkePlayer)
        return m_wkePlayer->paused();
    return m_paused;
}

bool WebMediaPlayerImpl::seeking() const
{
    if (m_wkePlayer)
        return m_wkePlayer->seeking();
    return m_seeking;
}

double WebMediaPlayerImpl::duration() const
{
    if (m_wkePlayer)
        return m_wkePlayer->duration();
    return m_duration;
}

double WebMediaPlayerImpl::currentTime() const 
{
    if (m_wkePlayer)
        return m_wkePlayer->currentTime();
    return 0.0;
}

blink::WebMediaPlayer::NetworkState WebMediaPlayerImpl::networkState() const
{
    if (m_wkePlayer)
        return (blink::WebMediaPlayer::NetworkState)m_wkePlayer->networkState();
    return blink::WebMediaPlayer::NetworkStateIdle;
}

blink::WebMediaPlayer::ReadyState WebMediaPlayerImpl::readyState() const
{
    if (m_wkePlayer)
        return (blink::WebMediaPlayer::ReadyState)m_wkePlayer->readyState();
    return m_readyState;
}

bool WebMediaPlayerImpl::didLoadingProgress()
{
    if (m_wkePlayer)
        return m_wkePlayer->didLoadingProgress();
    return false;
}

bool WebMediaPlayerImpl::hasSingleSecurityOrigin() const
{
    if (m_wkePlayer)
        return m_wkePlayer->hasSingleSecurityOrigin();
    return false;
}

bool WebMediaPlayerImpl::didPassCORSAccessCheck() const
{
    if (m_wkePlayer)
        return m_wkePlayer->didPassCORSAccessCheck();
    return false;
}

double WebMediaPlayerImpl::mediaTimeForTimeValue(double timeValue) const
{
    if (m_wkePlayer)
        return m_wkePlayer->mediaTimeForTimeValue(timeValue);
    return timeValue;
}

unsigned WebMediaPlayerImpl::decodedFrameCount() const
{
    if (m_wkePlayer)
        return m_wkePlayer->decodedFrameCount();
    return 1;
}

unsigned WebMediaPlayerImpl::droppedFrameCount() const
{
    if (m_wkePlayer)
        return m_wkePlayer->droppedFrameCount();
    return 1;
}

unsigned WebMediaPlayerImpl::corruptedFrameCount() const
{
    if (m_wkePlayer)
        return m_wkePlayer->corruptedFrameCount();
    return 0;
}

unsigned WebMediaPlayerImpl::audioDecodedByteCount() const
{
    if (m_wkePlayer)
        return m_wkePlayer->audioDecodedByteCount();
    return 1;
}

unsigned WebMediaPlayerImpl::videoDecodedByteCount() const
{
    if (m_wkePlayer)
        return m_wkePlayer->videoDecodedByteCount();
    return 1;
}

void WebMediaPlayerImpl::paint(WebCanvas* canvas, const WebRect& rect, unsigned char alpha, SkXfermode::Mode mode)
{
    if (!m_wkePlayer)
        return;

    blink::WebSize size(rect.width, rect.height);
    if (!m_memoryCanvas || m_size != size) {
        if (m_memoryCanvas)
            delete m_memoryCanvas;
        m_size = size; // naturalSize();
        m_memoryCanvas = skia::CreatePlatformCanvas(m_size.width, m_size.height, true);
    }

    SkPaint clearPaint;
    clearPaint.setARGB(0xFF, 0x4F, 0x4F, 0x4F);
    clearPaint.setXfermodeMode(SkXfermode::kSrc_Mode);

    SkRect skrc;
    skrc.set(0, 0, m_size.width, m_size.height);
    m_memoryCanvas->drawRect(skrc, clearPaint);

    HDC hMemoryDC = skia::BeginPlatformPaint(nullptr, m_memoryCanvas);

    wkeRect r = { rect.x, rect.y, rect.width, rect.height };
    m_wkePlayer->paint(hMemoryDC, r, alpha, mode);

    SkBaseDevice* bitmapDevice = skia::GetTopDevice(*m_memoryCanvas);
    const SkBitmap& bitmap = bitmapDevice->accessBitmap(false);

    if (canvas != m_memoryCanvas)
        canvas->drawBitmap(bitmap, rect.x, rect.y);

    skia::EndPlatformPaint(m_memoryCanvas);
}

void WebMediaPlayerImpl::setContentsToNativeWindowOffset(const blink::WebPoint& p)
{
    if (m_wkePlayer)
        m_wkePlayer->setContentsToNativeWindowOffset(p.x, p.y);
}

bool WebMediaPlayerImpl::handleMouseEvent(const blink::WebMouseEvent& evt)
{
    bool isDefaultHandled = false;
    uint32_t wParam = 0;
    uint32_t lParam = 0;
    uint32_t msg = 0;

    blink::IntPoint documentScrollOffsetRelativeToViewOrigin;// = contentsToNativeWindow(m_pluginContainer, blink::IntPoint());
    blink::IntPoint p(evt.windowX - documentScrollOffsetRelativeToViewOrigin.x(), evt.windowY - documentScrollOffsetRelativeToViewOrigin.y());

    lParam = MAKELPARAM(p.x(), p.y());
    wParam = 0;

    if (evt.modifiers & blink::WebInputEvent::ControlKey)
        wParam |= MK_CONTROL;
    if (evt.modifiers & blink::WebInputEvent::ShiftKey)
        wParam |= MK_SHIFT;

    if (evt.type == blink::WebInputEvent::Type::MouseMove
        || evt.type == blink::WebInputEvent::Type::MouseLeave
        || evt.type == blink::WebInputEvent::Type::MouseEnter) {
        msg = WM_MOUSEMOVE;
        if (evt.button != blink::WebMouseEvent::Button::ButtonNone) {
            switch (evt.button) {
            case blink::WebMouseEvent::Button::ButtonLeft:
                wParam |= MK_LBUTTON;
                break;
            case blink::WebMouseEvent::Button::ButtonMiddle:
                wParam |= MK_MBUTTON;
                break;
            case blink::WebMouseEvent::Button::ButtonRight:
                wParam |= MK_RBUTTON;
                break;
            }
        }
    } else if (evt.type == blink::WebInputEvent::Type::MouseDown) {
        switch (evt.button) {
        case blink::WebMouseEvent::Button::ButtonLeft:
            msg = WM_LBUTTONDOWN;
            break;
        case blink::WebMouseEvent::Button::ButtonMiddle:
            msg = WM_MBUTTONDOWN;
            break;
        case blink::WebMouseEvent::Button::ButtonRight:
            msg = WM_RBUTTONDOWN;
            break;
        }
    } else if (evt.type == blink::WebInputEvent::Type::MouseUp) {
        switch (evt.button) {
        case blink::WebMouseEvent::Button::ButtonLeft:
            msg = WM_LBUTTONUP;
            break;
        case blink::WebMouseEvent::Button::ButtonMiddle:
            msg = WM_MBUTTONUP;
            break;
        case blink::WebMouseEvent::Button::ButtonRight:
            msg = WM_RBUTTONUP;
            break;
        }
    } else if (evt.type == blink::WebInputEvent::Type::MouseWheel) {
        const blink::WebMouseWheelEvent& wheelEvt = static_cast<const blink::WebMouseWheelEvent&>(evt);
        msg = WM_MOUSEWHEEL;
        wParam = MAKEWPARAM(wheelEvt.deltaX, wheelEvt.deltaY);
    } else
        return isDefaultHandled;

    if (m_wkePlayer) {
        if (m_wkePlayer->handleMouseEvent(msg, wParam, lParam))
            isDefaultHandled = true;
    }

    return isDefaultHandled;
}

bool WebMediaPlayerImpl::handleKeyboardEvent(const blink::WebKeyboardEvent& evt)
{
    return false;
}

void WebMediaPlayerImpl::showMediaControls()
{
    if (m_wkePlayer)
        m_wkePlayer->showMediaControls();
}

void WebMediaPlayerImpl::hideMediaControls()
{
    if (m_wkePlayer)
        m_wkePlayer->hideMediaControls();
}

bool WebMediaPlayerImpl::copyVideoTextureToPlatformTexture(blink::WebGraphicsContext3D* cotext, unsigned texture, unsigned internalFormat, unsigned type, bool premultiplyAlpha, bool flipY)
{
    if (m_wkePlayer)
        return m_wkePlayer->copyVideoTextureToPlatformTexture(cotext, texture, internalFormat, type, premultiplyAlpha, flipY);
    return false;
}

blink::WebAudioSourceProvider* WebMediaPlayerImpl::audioSourceProvider()
{
    if (m_wkePlayer)
        return (blink::WebAudioSourceProvider*)m_wkePlayer->audioSourceProvider();
    return nullptr;
}

blink::WebMediaPlayer::MediaKeyException WebMediaPlayerImpl::generateKeyRequest(const WebString& keySystem, const unsigned char* initData, unsigned initDataLength) 
{
    if (m_wkePlayer) {
        std::string keySystemString = keySystem.utf8();
        return (blink::WebMediaPlayer::MediaKeyException)m_wkePlayer->generateKeyRequest(keySystemString.c_str(), initData, initDataLength);
    }
    return blink::WebMediaPlayer::MediaKeyExceptionKeySystemNotSupported;
}

blink::WebMediaPlayer::MediaKeyException WebMediaPlayerImpl::addKey(
    const WebString& keySystem, const unsigned char* key, 
    unsigned keyLength, const unsigned char* initData, unsigned initDataLength, const WebString& sessionId)
{
    if (m_wkePlayer) {
        std::string keySystemString = keySystem.utf8();
        std::string sessionIdString = sessionId.utf8();
        return (blink::WebMediaPlayer::MediaKeyException)m_wkePlayer->addKey(keySystemString.c_str(), key, keyLength, initData, initDataLength, sessionIdString.c_str());
    }
    return blink::WebMediaPlayer::MediaKeyExceptionKeySystemNotSupported;
}

blink::WebMediaPlayer::MediaKeyException WebMediaPlayerImpl::cancelKeyRequest(const WebString& keySystem, const WebString& sessionId) 
{
    if (m_wkePlayer) {
        std::string keySystemString = keySystem.utf8();
        std::string sessionIdString = sessionId.utf8();
        return (blink::WebMediaPlayer::MediaKeyException)m_wkePlayer->cancelKeyRequest(keySystemString.c_str(), sessionIdString.c_str());
    }
    return blink::WebMediaPlayer::MediaKeyExceptionKeySystemNotSupported;
}

void WebMediaPlayerImpl::setContentDecryptionModule(blink::WebContentDecryptionModule* cdm, blink::WebContentDecryptionModuleResult result) 
{
    result.completeWithError(blink::WebContentDecryptionModuleExceptionNotSupportedError, 0, "ERROR");
}

void WebMediaPlayerImpl::setPoster(const blink::WebURL& poster) 
{
    if (m_wkePlayer) {
        blink::KURL posterUrl = poster;
        CString posterString = posterUrl.getUTF8String().utf8();
        return m_wkePlayer->setPoster(posterString.data());
    }
}

void WebMediaPlayerImpl::enterFullscreen()
{
    if (m_wkePlayer)
        m_wkePlayer->enterFullscreen();
}

void WebMediaPlayerImpl::enabledAudioTracksChanged(const blink::WebVector<blink::WebMediaPlayer::TrackId>& enabledTrackIds) { }

void WebMediaPlayerImpl::selectedVideoTrackChanged(blink::WebMediaPlayer::TrackId* selectedTrackId) { }

}  // namespace content

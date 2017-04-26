// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/web_impl_win/WebMediaplayerImpl.h"

#include "third_party/WebKit/Source/wtf/Functional.h"
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

using blink::WebCanvas;
using blink::WebMediaPlayer;
using blink::WebRect;
using blink::WebSize;
using blink::WebString;

namespace content {

WebMediaPlayerImpl::WebMediaPlayerImpl(
    blink::WebLocalFrame* frame,
    const blink::WebURL& url,
    blink::WebMediaPlayerClient* client)
{

    m_paused = false;
    m_seeking = false;
    m_hasVideo = true;
    m_hasAudio = false;
    m_readyState = blink::WebMediaPlayer::ReadyStateHaveNothing;
    m_client = client;
}

WebMediaPlayerImpl::~WebMediaPlayerImpl()
{
  
}

void WebMediaPlayerImpl::load(blink::WebMediaPlayer::LoadType, const blink::WebURL&, blink::WebMediaPlayer::CORSMode)
{
    ASSERT(isMainThread());
    for (int i = 0; i <= blink::WebMediaPlayer::ReadyStateHaveEnoughData; ++i) {
        blink::Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(&WebMediaPlayerImpl::onLoad, this, (blink::WebMediaPlayer::ReadyState)i));
    }
}

void WebMediaPlayerImpl::onLoad(blink::WebMediaPlayer::ReadyState readyState)
{
    m_readyState = readyState;
    m_client->readyStateChanged();
}

// Playback controls.
void WebMediaPlayerImpl::play()
{
    m_paused = false;
}

void WebMediaPlayerImpl::pause()
{
    m_paused = true;
}

bool WebMediaPlayerImpl::supportsSave() const
{
    return false;
}

void WebMediaPlayerImpl::seek(double seconds)
{

}

void WebMediaPlayerImpl::setRate(double)
{

}

void WebMediaPlayerImpl::setVolume(double)
{

}
void WebMediaPlayerImpl::requestRemotePlayback() {}
void WebMediaPlayerImpl::requestRemotePlaybackControl() { }
void WebMediaPlayerImpl::setPreload(Preload) { }
blink::WebTimeRanges WebMediaPlayerImpl::buffered() const
{
    return blink::WebVector<blink::WebTimeRange>();
}

blink::WebTimeRanges WebMediaPlayerImpl::seekable() const
{
    return blink::WebVector<blink::WebTimeRange>();
}

void WebMediaPlayerImpl::setSinkId(const WebString& deviceId, blink::WebCallbacks<void, blink::WebSetSinkIdError>*)
{
    ;
}

bool WebMediaPlayerImpl::hasVideo() const
{
    return m_hasVideo;
}

bool WebMediaPlayerImpl::hasAudio() const
{
    return m_hasAudio;
}

bool WebMediaPlayerImpl::isRemote() const { return false; }

WebSize WebMediaPlayerImpl::naturalSize() const
{
    if (m_hasVideo)
        return WebSize(50, 50);
    return WebSize(0, 0);
}

bool WebMediaPlayerImpl::paused() const
{
    return m_paused;
}
bool WebMediaPlayerImpl::seeking() const
{
    return m_seeking;
}
double WebMediaPlayerImpl::duration() const
{
    return 1.0;
}
double WebMediaPlayerImpl::currentTime() const 
{
    return 1.0;
}

blink::WebMediaPlayer::NetworkState WebMediaPlayerImpl::networkState() const
{
    return blink::WebMediaPlayer::NetworkStateIdle;
}

blink::WebMediaPlayer::ReadyState WebMediaPlayerImpl::readyState() const
{
    return m_readyState;
}

bool WebMediaPlayerImpl::didLoadingProgress()
{
    return false;
}

bool WebMediaPlayerImpl::hasSingleSecurityOrigin() const
{
    return false;
}

bool WebMediaPlayerImpl::didPassCORSAccessCheck() const
{
    return false;
}

double WebMediaPlayerImpl::mediaTimeForTimeValue(double timeValue) const
{
    return timeValue;
}

unsigned WebMediaPlayerImpl::decodedFrameCount() const
{
    return 1;
}
unsigned WebMediaPlayerImpl::droppedFrameCount() const
{
    return 1;
}
unsigned WebMediaPlayerImpl::corruptedFrameCount() const { return 0; }
unsigned WebMediaPlayerImpl::audioDecodedByteCount() const
{
    return 1;
}
unsigned WebMediaPlayerImpl::videoDecodedByteCount() const
{
    return 1;
}

void WebMediaPlayerImpl::paint(WebCanvas*, const WebRect&, unsigned char alpha, SkXfermode::Mode)
{
}

bool WebMediaPlayerImpl::copyVideoTextureToPlatformTexture(blink::WebGraphicsContext3D*, 
    unsigned texture, unsigned internalFormat, unsigned type, bool premultiplyAlpha, bool flipY) { return false; }

blink::WebAudioSourceProvider* WebMediaPlayerImpl::audioSourceProvider() { return nullptr; }

blink::WebMediaPlayer::MediaKeyException WebMediaPlayerImpl::generateKeyRequest(const WebString& keySystem, const unsigned char* initData, unsigned initDataLength) 
{
    return blink::WebMediaPlayer::MediaKeyExceptionKeySystemNotSupported;
}

blink::WebMediaPlayer::MediaKeyException WebMediaPlayerImpl::addKey(
    const WebString& keySystem, const unsigned char* key, 
    unsigned keyLength, const unsigned char* initData, unsigned initDataLength, const WebString& sessionId)
{
    return blink::WebMediaPlayer::MediaKeyExceptionKeySystemNotSupported;
}

blink::WebMediaPlayer::MediaKeyException WebMediaPlayerImpl::cancelKeyRequest(const WebString& keySystem, const WebString& sessionId) 
{
    return blink::WebMediaPlayer::MediaKeyExceptionKeySystemNotSupported;
}

void WebMediaPlayerImpl::setContentDecryptionModule(blink::WebContentDecryptionModule* cdm, blink::WebContentDecryptionModuleResult result) 
{
    result.completeWithError(blink::WebContentDecryptionModuleExceptionNotSupportedError, 0, "ERROR");
}

void WebMediaPlayerImpl::setPoster(const blink::WebURL& poster) { }

void WebMediaPlayerImpl::enterFullscreen() { }

void WebMediaPlayerImpl::enabledAudioTracksChanged(const blink::WebVector<blink::WebMediaPlayer::TrackId>& enabledTrackIds) { }

void WebMediaPlayerImpl::selectedVideoTrackChanged(blink::WebMediaPlayer::TrackId* selectedTrackId) { }

}  // namespace content

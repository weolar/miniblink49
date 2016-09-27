// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "web/WebMediaPlayerClientImpl.h"

#include "core/frame/LocalFrame.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/TimeRanges.h"
#include "core/layout/LayoutView.h"
#include "modules/encryptedmedia/HTMLMediaElementEncryptedMedia.h"
#include "modules/mediastream/MediaStreamRegistry.h"
#include "platform/audio/AudioBus.h"
#include "platform/audio/AudioSourceProviderClient.h"
#include "platform/geometry/IntSize.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/graphics/gpu/Extensions3DUtil.h"
#include "public/platform/Platform.h"
#include "public/platform/WebAudioSourceProvider.h"
#include "public/platform/WebCString.h"
#include "public/platform/WebCanvas.h"
#include "public/platform/WebCompositorSupport.h"
#include "public/platform/WebContentDecryptionModule.h"
#include "public/platform/WebGraphicsContext3DProvider.h"
#include "public/platform/WebInbandTextTrack.h"
#include "public/platform/WebMediaPlayer.h"
#include "public/platform/WebRect.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"
#include "public/web/WebDocument.h"
#include "public/web/WebFrameClient.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebViewImpl.h"

#include "wtf/Assertions.h"
#include "wtf/text/CString.h"

namespace blink {

static PassOwnPtr<WebMediaPlayer> createWebMediaPlayer(WebMediaPlayerClient* client, const WebURL& url, LocalFrame* frame, WebContentDecryptionModule* initialCdm)
{
    WebLocalFrameImpl* webFrame = WebLocalFrameImpl::fromFrame(frame);

    if (!webFrame || !webFrame->client())
        return nullptr;
    return adoptPtr(webFrame->client()->createMediaPlayer(webFrame, url, client, initialCdm));
}

WebMediaPlayer* WebMediaPlayerClientImpl::webMediaPlayer() const
{
    return m_webMediaPlayer.get();
}

// WebMediaPlayerClient --------------------------------------------------------

WebMediaPlayerClientImpl::~WebMediaPlayerClientImpl()
{
    // Explicitly destroy the WebMediaPlayer to allow verification of tear down.
    m_webMediaPlayer.clear();
}

void WebMediaPlayerClientImpl::networkStateChanged()
{
    m_client->mediaPlayerNetworkStateChanged();
}

void WebMediaPlayerClientImpl::readyStateChanged()
{
    m_client->mediaPlayerReadyStateChanged();
}

void WebMediaPlayerClientImpl::timeChanged()
{
    m_client->mediaPlayerTimeChanged();
}

void WebMediaPlayerClientImpl::repaint()
{
    m_client->mediaPlayerRepaint();
}

void WebMediaPlayerClientImpl::durationChanged()
{
    m_client->mediaPlayerDurationChanged();
}

void WebMediaPlayerClientImpl::sizeChanged()
{
    m_client->mediaPlayerSizeChanged();
}

void WebMediaPlayerClientImpl::playbackStateChanged()
{
    m_client->mediaPlayerPlaybackStateChanged();
}

void WebMediaPlayerClientImpl::keyAdded(const WebString& keySystem, const WebString& sessionId)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    HTMLMediaElementEncryptedMedia::keyAdded(mediaElement(), keySystem, sessionId);
#endif
}

void WebMediaPlayerClientImpl::keyError(const WebString& keySystem, const WebString& sessionId, MediaKeyErrorCode errorCode, unsigned short systemCode)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    HTMLMediaElementEncryptedMedia::keyError(mediaElement(), keySystem, sessionId, errorCode, systemCode);
#endif
}

void WebMediaPlayerClientImpl::keyMessage(const WebString& keySystem, const WebString& sessionId, const unsigned char* message, unsigned messageLength, const WebURL& defaultURL)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    HTMLMediaElementEncryptedMedia::keyMessage(mediaElement(), keySystem, sessionId, message, messageLength, defaultURL);
#endif
}

void WebMediaPlayerClientImpl::encrypted(WebEncryptedMediaInitDataType initDataType, const unsigned char* initData, unsigned initDataLength)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    HTMLMediaElementEncryptedMedia::encrypted(mediaElement(), initDataType, initData, initDataLength);
#endif
}

void WebMediaPlayerClientImpl::didBlockPlaybackWaitingForKey()
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    HTMLMediaElementEncryptedMedia::didBlockPlaybackWaitingForKey(mediaElement());
#endif
}

void WebMediaPlayerClientImpl::didResumePlaybackBlockedForKey()
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    HTMLMediaElementEncryptedMedia::didResumePlaybackBlockedForKey(mediaElement());
#endif
}

void WebMediaPlayerClientImpl::setWebLayer(WebLayer* layer)
{
    m_client->mediaPlayerSetWebLayer(layer);
}

WebMediaPlayer::TrackId WebMediaPlayerClientImpl::addAudioTrack(const WebString& id, AudioTrackKind kind, const WebString& label, const WebString& language, bool enabled)
{
    return mediaElement().addAudioTrack(id, kind, label, language, enabled);
}

void WebMediaPlayerClientImpl::removeAudioTrack(WebMediaPlayer::TrackId id)
{
    mediaElement().removeAudioTrack(id);
}

WebMediaPlayer::TrackId WebMediaPlayerClientImpl::addVideoTrack(const WebString& id, VideoTrackKind kind, const WebString& label, const WebString& language, bool selected)
{
    return mediaElement().addVideoTrack(id, kind, label, language, selected);
}

void WebMediaPlayerClientImpl::removeVideoTrack(WebMediaPlayer::TrackId id)
{
    mediaElement().removeVideoTrack(id);
}

void WebMediaPlayerClientImpl::addTextTrack(WebInbandTextTrack* textTrack)
{
    m_client->mediaPlayerDidAddTextTrack(textTrack);
}

void WebMediaPlayerClientImpl::removeTextTrack(WebInbandTextTrack* textTrack)
{
    m_client->mediaPlayerDidRemoveTextTrack(textTrack);
}

void WebMediaPlayerClientImpl::mediaSourceOpened(WebMediaSource* webMediaSource)
{
    ASSERT(webMediaSource);
    m_client->mediaPlayerMediaSourceOpened(webMediaSource);
}

void WebMediaPlayerClientImpl::requestSeek(double time)
{
    m_client->mediaPlayerRequestSeek(time);
}

void WebMediaPlayerClientImpl::remoteRouteAvailabilityChanged(bool routesAvailable)
{
    mediaElement().remoteRouteAvailabilityChanged(routesAvailable);
}

void WebMediaPlayerClientImpl::connectedToRemoteDevice()
{
    mediaElement().connectedToRemoteDevice();
}

void WebMediaPlayerClientImpl::disconnectedFromRemoteDevice()
{
    mediaElement().disconnectedFromRemoteDevice();
}

// MediaPlayer -------------------------------------------------
void WebMediaPlayerClientImpl::load(WebMediaPlayer::LoadType loadType, const WTF::String& url, WebMediaPlayer::CORSMode corsMode)
{
    ASSERT(!m_webMediaPlayer);

    // FIXME: Remove this cast
    LocalFrame* frame = mediaElement().document().frame();

    WebURL poster = m_client->mediaPlayerPosterURL();

    KURL kurl(ParsedURLString, url);
    m_webMediaPlayer = createWebMediaPlayer(this, kurl, frame, 
#ifdef MINIBLINK_NOT_IMPLEMENTED
        HTMLMediaElementEncryptedMedia::contentDecryptionModule(mediaElement())
#else
        nullptr
        );
#endif
    if (!m_webMediaPlayer)
        return;

    if (mediaElement().layoutObject())
        mediaElement().layoutObject()->setShouldDoFullPaintInvalidation();

#if ENABLE(WEB_AUDIO)
    // Make sure if we create/re-create the WebMediaPlayer that we update our wrapper.
    m_audioSourceProvider.wrap(m_webMediaPlayer->audioSourceProvider());
#endif

    m_webMediaPlayer->setVolume(mediaElement().effectiveMediaVolume());

    m_webMediaPlayer->setPoster(poster);

    setPreload(mediaElement().effectivePreloadType());

    m_webMediaPlayer->load(loadType, kurl, corsMode);

    if (mediaElement().isFullscreen())
        m_webMediaPlayer->enterFullscreen();
}

void WebMediaPlayerClientImpl::setPreload(MediaPlayer::Preload preload)
{
    if (m_webMediaPlayer)
        m_webMediaPlayer->setPreload(static_cast<WebMediaPlayer::Preload>(preload));
}

#if ENABLE(WEB_AUDIO)
AudioSourceProvider* WebMediaPlayerClientImpl::audioSourceProvider()
{
    return &m_audioSourceProvider;
}
#endif

PassOwnPtr<MediaPlayer> WebMediaPlayerClientImpl::create(MediaPlayerClient* client)
{
    return adoptPtr(new WebMediaPlayerClientImpl(client));
}

WebMediaPlayerClientImpl::WebMediaPlayerClientImpl(MediaPlayerClient* client)
    : m_client(client)
{
    ASSERT(m_client);
}

HTMLMediaElement& WebMediaPlayerClientImpl::mediaElement() const
{
    return *static_cast<HTMLMediaElement*>(m_client);
}

#if ENABLE(WEB_AUDIO)
void WebMediaPlayerClientImpl::AudioSourceProviderImpl::wrap(WebAudioSourceProvider* provider)
{
    MutexLocker locker(provideInputLock);

    if (m_webAudioSourceProvider && provider != m_webAudioSourceProvider)
        m_webAudioSourceProvider->setClient(0);

    m_webAudioSourceProvider = provider;
    if (m_webAudioSourceProvider)
        m_webAudioSourceProvider->setClient(m_client.get());
}

void WebMediaPlayerClientImpl::AudioSourceProviderImpl::setClient(AudioSourceProviderClient* client)
{
    MutexLocker locker(provideInputLock);

    if (client)
        m_client = new WebMediaPlayerClientImpl::AudioClientImpl(client);
    else
        m_client.clear();

    if (m_webAudioSourceProvider)
        m_webAudioSourceProvider->setClient(m_client.get());
}

void WebMediaPlayerClientImpl::AudioSourceProviderImpl::provideInput(AudioBus* bus, size_t framesToProcess)
{
    ASSERT(bus);
    if (!bus)
        return;

    MutexTryLocker tryLocker(provideInputLock);
    if (!tryLocker.locked() || !m_webAudioSourceProvider || !m_client.get()) {
        bus->zero();
        return;
    }

    // Wrap the AudioBus channel data using WebVector.
    size_t n = bus->numberOfChannels();
    WebVector<float*> webAudioData(n);
    for (size_t i = 0; i < n; ++i)
        webAudioData[i] = bus->channel(i)->mutableData();

    m_webAudioSourceProvider->provideInput(webAudioData, framesToProcess);
}

void WebMediaPlayerClientImpl::AudioClientImpl::setFormat(size_t numberOfChannels, float sampleRate)
{
    if (m_client)
        m_client->setFormat(numberOfChannels, sampleRate);
}

DEFINE_TRACE(WebMediaPlayerClientImpl::AudioClientImpl)
{
    visitor->trace(m_client);
}

#endif

} // namespace blink

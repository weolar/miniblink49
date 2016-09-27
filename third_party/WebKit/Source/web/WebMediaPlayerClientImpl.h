/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebMediaPlayerClientImpl_h
#define WebMediaPlayerClientImpl_h

#include "platform/audio/AudioSourceProvider.h"
#include "platform/graphics/media/MediaPlayer.h"
#include "public/platform/WebAudioSourceProviderClient.h"
#include "public/platform/WebEncryptedMediaTypes.h"
#include "public/platform/WebMediaPlayerClient.h"
#include "platform/weborigin/KURL.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/ThreadingPrimitives.h"

namespace blink {

class AudioSourceProviderClient;
class HTMLMediaElement;
class WebAudioSourceProvider;
class WebMediaPlayer;

// This class serves as a bridge between MediaPlayer and
// WebMediaPlayer.
class WebMediaPlayerClientImpl final : public MediaPlayer, public WebMediaPlayerClient {

public:
    static PassOwnPtr<MediaPlayer> create(MediaPlayerClient*);

    ~WebMediaPlayerClientImpl() override;

    // WebMediaPlayerClient methods:
    void networkStateChanged() override;
    void readyStateChanged() override;
    void timeChanged() override;
    void repaint() override;
    void durationChanged() override;
    void sizeChanged() override;
    void playbackStateChanged() override;

    // WebEncryptedMediaPlayerClient methods:
    void keyAdded(const WebString& keySystem, const WebString& sessionId) override;
    void keyError(const WebString& keySystem, const WebString& sessionId, MediaKeyErrorCode, unsigned short systemCode) override;
    void keyMessage(const WebString& keySystem, const WebString& sessionId, const unsigned char* message, unsigned messageLength, const WebURL& defaultURL) override;
    void encrypted(WebEncryptedMediaInitDataType, const unsigned char* initData, unsigned initDataLength) override;
    void didBlockPlaybackWaitingForKey() override;
    void didResumePlaybackBlockedForKey() override;

    void setWebLayer(WebLayer*) override;
    WebMediaPlayer::TrackId addAudioTrack(const WebString& id, AudioTrackKind, const WebString& label, const WebString& language, bool enabled) override;
    void removeAudioTrack(WebMediaPlayer::TrackId) override;
    WebMediaPlayer::TrackId addVideoTrack(const WebString& id, VideoTrackKind, const WebString& label, const WebString& language, bool selected) override;
    void removeVideoTrack(WebMediaPlayer::TrackId) override;
    void addTextTrack(WebInbandTextTrack*) override;
    void removeTextTrack(WebInbandTextTrack*) override;
    void mediaSourceOpened(WebMediaSource*) override;
    void requestSeek(double) override;
    void remoteRouteAvailabilityChanged(bool) override;
    void connectedToRemoteDevice() override;
    void disconnectedFromRemoteDevice() override;

    // MediaPlayer methods:
    WebMediaPlayer* webMediaPlayer() const override;
    void load(WebMediaPlayer::LoadType, const WTF::String& url, WebMediaPlayer::CORSMode) override;
    void setPreload(MediaPlayer::Preload) override;

#if ENABLE(WEB_AUDIO)
    AudioSourceProvider* audioSourceProvider() override;
#endif

private:
    explicit WebMediaPlayerClientImpl(MediaPlayerClient*);

    HTMLMediaElement& mediaElement() const;

    MediaPlayerClient* m_client;
    OwnPtr<WebMediaPlayer> m_webMediaPlayer;

#if ENABLE(WEB_AUDIO)
    // AudioClientImpl wraps an AudioSourceProviderClient.
    // When the audio format is known, Chromium calls setFormat() which then dispatches into WebCore.

    class AudioClientImpl final : public GarbageCollectedFinalized<AudioClientImpl>, public WebAudioSourceProviderClient {
    public:
        explicit AudioClientImpl(AudioSourceProviderClient* client)
            : m_client(client)
        {
        }

        ~AudioClientImpl() override { }

        // WebAudioSourceProviderClient
        void setFormat(size_t numberOfChannels, float sampleRate) override;

        DECLARE_TRACE();

    private:
        Member<AudioSourceProviderClient> m_client;
    };

    // AudioSourceProviderImpl wraps a WebAudioSourceProvider.
    // provideInput() calls into Chromium to get a rendered audio stream.

    class AudioSourceProviderImpl final : public AudioSourceProvider {
    public:
        AudioSourceProviderImpl()
            : m_webAudioSourceProvider(0)
        {
        }

        ~AudioSourceProviderImpl() override { }

        // Wraps the given WebAudioSourceProvider.
        void wrap(WebAudioSourceProvider*);

        // AudioSourceProvider
        void setClient(AudioSourceProviderClient*) override;
        void provideInput(AudioBus*, size_t framesToProcess) override;

    private:
        WebAudioSourceProvider* m_webAudioSourceProvider;
        Persistent<AudioClientImpl> m_client;
        Mutex provideInputLock;
    };

    AudioSourceProviderImpl m_audioSourceProvider;
#endif
};

} // namespace blink

#endif

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_MEDIA_WEBMEDIAPLAYER_IMPL_H_
#define CONTENT_RENDERER_MEDIA_WEBMEDIAPLAYER_IMPL_H_

#include "skia/ext/platform_canvas.h"

#include "third_party/WebKit/public/platform/WebMediaPlayer.h"
#include "third_party/WebKit/public/platform/WebMediaPlayerClient.h"

namespace blink {
class WebContentDecryptionModule;
class WebLocalFrame;
}

namespace content {
class BufferedDataSource;
class VideoFrameCompositor;
class WebAudioSourceProviderImpl;
class WebContentDecryptionModuleImpl;
class WebLayerImpl;
class WebMediaPlayerDelegate;
class WebMediaPlayerParams;
class WebTextTrackImpl;

// The canonical implementation of blink::WebMediaPlayer that's backed by
// media::Pipeline. Handles normal resource loading, Media Source, and
// Encrypted Media.
class WebMediaPlayerImpl : public blink::WebMediaPlayer {
public:
    // Constructs a WebMediaPlayer implementation using Chromium's media stack.
    // |delegate| may be null.
    WebMediaPlayerImpl(blink::WebLocalFrame* frame,
        const blink::WebURL& url,
        blink::WebMediaPlayerClient* client);
    virtual ~WebMediaPlayerImpl();

    virtual void load(blink::WebMediaPlayer::LoadType, const blink::WebURL&, blink::WebMediaPlayer::CORSMode) override;

    // Playback controls.
    virtual void play() override;
    virtual void pause() override;
    virtual bool supportsSave() const override;
    virtual void seek(double seconds) override;
    virtual void setRate(double) override;
    virtual void setVolume(double) override;

    virtual void requestRemotePlayback() override;
    virtual void requestRemotePlaybackControl() override;
    virtual void setPreload(Preload) override;
    virtual blink::WebTimeRanges buffered() const override;
    virtual blink::WebTimeRanges seekable() const override;

    // Attempts to switch the audio output device.
    // Implementations of setSinkId take ownership of the WebCallbacks
    // object, and the WebCallbacks object takes ownership of the returned
    // error value, if any.
    // Note also that setSinkId implementations must make sure that all
    // methods of the WebCallbacks object, including constructors and
    // destructors, run in the same thread where the object is created
    // (i.e., the blink thread).
    virtual void setSinkId(const blink::WebString& deviceId, blink::WebCallbacks<void, blink::WebSetSinkIdError>*) override;

    // True if the loaded media has a playable video/audio track.
    virtual bool hasVideo() const override;
    virtual bool hasAudio() const override;

    // True if the media is being played on a remote device.
    virtual bool isRemote() const override;

    // Dimension of the video.
    virtual blink::WebSize naturalSize() const override;

    // Getters of playback state.
    virtual bool paused() const override;
    virtual bool seeking() const override;
    virtual double duration() const override;
    virtual double currentTime() const override;

    // Internal states of loading and network.
    virtual blink::WebMediaPlayer::NetworkState networkState() const override;
    virtual blink::WebMediaPlayer::ReadyState readyState() const override;

    virtual bool didLoadingProgress() override;

    virtual bool hasSingleSecurityOrigin() const override;
    virtual bool didPassCORSAccessCheck() const override;

    virtual double mediaTimeForTimeValue(double timeValue) const override;

    virtual unsigned decodedFrameCount() const override;
    virtual unsigned droppedFrameCount() const override;
    virtual unsigned corruptedFrameCount() const override;
    virtual unsigned audioDecodedByteCount() const override;
    virtual unsigned videoDecodedByteCount() const override;

    virtual void paint(blink::WebCanvas*, const blink::WebRect&, unsigned char alpha, SkXfermode::Mode) override;
    // Do a GPU-GPU textures copy if possible.
    virtual bool copyVideoTextureToPlatformTexture(blink::WebGraphicsContext3D*, unsigned texture, unsigned internalFormat, unsigned type, bool premultiplyAlpha, bool flipY) override;

    virtual blink::WebAudioSourceProvider* audioSourceProvider() override;

    // Returns whether keySystem is supported. If true, the result will be
    // reported by an event.
    virtual blink::WebMediaPlayer::MediaKeyException generateKeyRequest(const blink::WebString& keySystem, const unsigned char* initData, unsigned initDataLength) override;
    virtual blink::WebMediaPlayer::MediaKeyException addKey(const blink::WebString& keySystem, const unsigned char* key, unsigned keyLength, const unsigned char* initData, unsigned initDataLength, const blink::WebString& sessionId) override;
    virtual blink::WebMediaPlayer::MediaKeyException cancelKeyRequest(const blink::WebString& keySystem, const blink::WebString& sessionId) override;
    virtual void setContentDecryptionModule(blink::WebContentDecryptionModule* cdm, blink::WebContentDecryptionModuleResult result) override;

    // Sets the poster image URL.
    virtual void setPoster(const blink::WebURL& poster) override;

    // Instruct WebMediaPlayer to enter/exit fullscreen.
    virtual void enterFullscreen() override;

    virtual void enabledAudioTracksChanged(const blink::WebVector<blink::WebMediaPlayer::TrackId>& enabledTrackIds) override;
    // |selectedTrackId| is null if no track is selected.
    virtual void selectedVideoTrackChanged(blink::WebMediaPlayer::TrackId* selectedTrackId) override;

    void onLoad(blink::WebMediaPlayer::ReadyState readyState, bool* cancelNotifer);

private:
    bool m_paused;
    bool m_seeking;
    bool m_hasVideo;
    bool m_hasAudio;
    blink::WebMediaPlayerClient* m_client;

    int m_width;
    int m_height;
    double m_duration;

    blink::WebMediaPlayer::ReadyState m_readyState;

    Vector<bool*> m_asynLoadCancelNotifers;
};

}  // namespace content

#endif  // CONTENT_RENDERER_MEDIA_WEBMEDIAPLAYER_IMPL_H_

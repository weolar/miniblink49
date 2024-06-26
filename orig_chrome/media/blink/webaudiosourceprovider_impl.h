// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_WEBAUDIOSOURCEPROVIDER_IMPL_H_
#define MEDIA_BLINK_WEBAUDIOSOURCEPROVIDER_IMPL_H_

#include <string>

#include "base/callback.h"
#include "base/memory/weak_ptr.h"
#include "base/synchronization/lock.h"
#include "media/base/audio_renderer_sink.h"
#include "media/blink/media_blink_export.h"
#include "third_party/WebKit/public/platform/WebAudioSourceProvider.h"
#include "third_party/WebKit/public/platform/WebVector.h"

namespace blink {
class WebAudioSourceProviderClient;
}

namespace media {

// WebAudioSourceProviderImpl provides a bridge between classes:
//     blink::WebAudioSourceProvider <---> AudioRendererSink
//
// WebAudioSourceProviderImpl wraps an existing audio sink that is used unless
// WebKit has set a client via setClient(). While a client is set WebKit will
// periodically call provideInput() to render a certain number of audio
// sample-frames using the sink's RenderCallback to get the data.
//
// All calls are protected by a lock.
class MEDIA_BLINK_EXPORT WebAudioSourceProviderImpl
    : NON_EXPORTED_BASE(public blink::WebAudioSourceProvider),
      NON_EXPORTED_BASE(public RestartableAudioRendererSink) {
public:
    explicit WebAudioSourceProviderImpl(
        const scoped_refptr<RestartableAudioRendererSink>& sink);

    // blink::WebAudioSourceProvider implementation.
    void setClient(blink::WebAudioSourceProviderClient* client) override;
    void provideInput(const blink::WebVector<float*>& audio_data,
        size_t number_of_frames) override;

    // RestartableAudioRendererSink implementation.
    void Start() override;
    void Stop() override;
    void Play() override;
    void Pause() override;
    bool SetVolume(double volume) override;
    OutputDevice* GetOutputDevice() override;
    void Initialize(const AudioParameters& params,
        RenderCallback* renderer) override;

protected:
    ~WebAudioSourceProviderImpl() override;

private:
    // Calls setFormat() on |client_| from the Blink renderer thread.
    void OnSetFormat();

    // Closure that posts a task to call OnSetFormat() on the renderer thread.
    base::Closure set_format_cb_;

    // Set to true when Initialize() is called.
    int channels_;
    int sample_rate_;
    double volume_;

    // Tracks the current playback state.
    enum PlaybackState { kStopped,
        kStarted,
        kPlaying };
    PlaybackState state_;

    // Where audio comes from.
    AudioRendererSink::RenderCallback* renderer_;

    // When set via setClient() it overrides |sink_| for consuming audio.
    blink::WebAudioSourceProviderClient* client_;

    // Where audio ends up unless overridden by |client_|.
    base::Lock sink_lock_;
    scoped_refptr<RestartableAudioRendererSink> sink_;
    scoped_ptr<AudioBus> bus_wrapper_;

    // NOTE: Weak pointers must be invalidated before all other member variables.
    base::WeakPtrFactory<WebAudioSourceProviderImpl> weak_factory_;

    DISALLOW_IMPLICIT_CONSTRUCTORS(WebAudioSourceProviderImpl);
};

} // namespace media

#endif // MEDIA_BLINK_WEBAUDIOSOURCEPROVIDER_IMPL_H_

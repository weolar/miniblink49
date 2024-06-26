// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_AUDIO_OUTPUT_RESAMPLER_H_
#define MEDIA_AUDIO_AUDIO_OUTPUT_RESAMPLER_H_

#include <map>

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "media/audio/audio_io.h"
#include "media/audio/audio_manager.h"
#include "media/audio/audio_output_dispatcher_impl.h"
#include "media/audio/audio_parameters.h"

namespace media {

class OnMoreDataConverter;

// AudioOutputResampler is a browser-side resampling and buffering solution
// which ensures audio data is always output at given parameters.  See the
// AudioConverter class for details on the conversion process.
//
// AOR works by intercepting the AudioSourceCallback provided to StartStream()
// and redirecting it through an AudioConverter instance.  |total_bytes_delay|
// is adjusted for buffer delay caused by the conversion process.
//
// AOR will automatically fall back from AUDIO_PCM_LOW_LATENCY to
// AUDIO_PCM_LINEAR if the output device fails to open at the requested output
// parameters. If opening still fails, it will fallback to AUDIO_FAKE.
class MEDIA_EXPORT AudioOutputResampler : public AudioOutputDispatcher {
public:
    AudioOutputResampler(AudioManager* audio_manager,
        const AudioParameters& input_params,
        const AudioParameters& output_params,
        const std::string& output_device_id,
        const base::TimeDelta& close_delay);

    // AudioOutputDispatcher interface.
    bool OpenStream() override;
    bool StartStream(AudioOutputStream::AudioSourceCallback* callback,
        AudioOutputProxy* stream_proxy) override;
    void StopStream(AudioOutputProxy* stream_proxy) override;
    void StreamVolumeSet(AudioOutputProxy* stream_proxy, double volume) override;
    void CloseStream(AudioOutputProxy* stream_proxy) override;
    void Shutdown() override;

private:
    friend class base::RefCountedThreadSafe<AudioOutputResampler>;
    ~AudioOutputResampler() override;

    // Converts low latency based output parameters into high latency
    // appropriate output parameters in error situations.
    void SetupFallbackParams();

    // Used to reinitialize |dispatcher_|.
    void Reinitialize();

    // Used to initialize |dispatcher_|.
    void Initialize();

    // Dispatcher to proxy all AudioOutputDispatcher calls too.
    scoped_refptr<AudioOutputDispatcherImpl> dispatcher_;

    // Map of outstanding OnMoreDataConverter objects.  A new object is created
    // on every StartStream() call and destroyed on CloseStream().
    typedef std::map<AudioOutputProxy*, OnMoreDataConverter*> CallbackMap;
    CallbackMap callbacks_;

    // Used by AudioOutputDispatcherImpl; kept so we can reinitialize on the fly.
    base::TimeDelta close_delay_;

    // AudioParameters used to setup the output stream; changed upon fallback.
    AudioParameters output_params_;

    // The original AudioParameters we were constructed with.
    const AudioParameters original_output_params_;

    // Whether any streams have been opened through |dispatcher_|, if so we can't
    // fallback on future OpenStream() failures.
    bool streams_opened_;

    // The reinitialization timer provides a way to recover from temporary failure
    // states by clearing the dispatcher if all proxies have been closed and none
    // have been created within |close_delay_|.  Without this, audio may be lost
    // to a fake stream indefinitely for transient errors.
    base::Timer reinitialize_timer_;

    DISALLOW_COPY_AND_ASSIGN(AudioOutputResampler);
};

} // namespace media

#endif // MEDIA_AUDIO_AUDIO_OUTPUT_RESAMPLER_H_

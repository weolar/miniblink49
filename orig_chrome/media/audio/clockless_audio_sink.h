// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_CLOCKLESS_AUDIO_SINK_H_
#define MEDIA_AUDIO_CLOCKLESS_AUDIO_SINK_H_

#include <string>

#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "media/base/audio_renderer_sink.h"

namespace base {
class SingleThreadTaskRunner;
}

namespace media {
class AudioBus;
class ClocklessAudioSinkThread;
class OutputDevice;

// Implementation of an AudioRendererSink that consumes the audio as fast as
// possible. This class does not support multiple Play()/Pause() events.
class MEDIA_EXPORT ClocklessAudioSink
    : NON_EXPORTED_BASE(public AudioRendererSink) {
public:
    ClocklessAudioSink();

    // AudioRendererSink implementation.
    void Initialize(const AudioParameters& params,
        RenderCallback* callback) override;
    void Start() override;
    void Stop() override;
    void Pause() override;
    void Play() override;
    bool SetVolume(double volume) override;
    OutputDevice* GetOutputDevice() override;

    // Returns the time taken to consume all the audio.
    base::TimeDelta render_time() { return playback_time_; }

    // Enables audio frame hashing.  Must be called prior to Initialize().
    void StartAudioHashForTesting();

    // Returns the hash of all audio frames seen since construction.
    std::string GetAudioHashForTesting();

protected:
    ~ClocklessAudioSink() override;

private:
    scoped_ptr<ClocklessAudioSinkThread> thread_;
    bool initialized_;
    bool playing_;
    bool hashing_;

    // Time taken in last set of Render() calls.
    base::TimeDelta playback_time_;

    DISALLOW_COPY_AND_ASSIGN(ClocklessAudioSink);
};

} // namespace media

#endif // MEDIA_AUDIO_CLOCKLESS_AUDIO_SINK_H_

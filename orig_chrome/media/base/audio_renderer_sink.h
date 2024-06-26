// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_AUDIO_RENDERER_SINK_H_
#define MEDIA_BASE_AUDIO_RENDERER_SINK_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "media/audio/audio_output_ipc.h"
#include "media/audio/audio_parameters.h"
#include "media/base/audio_bus.h"
#include "media/base/media_export.h"
#include "media/base/output_device.h"
//#include "url/gurl.h"

namespace base {
class SingleThreadTaskRunner;
}

namespace media {

// AudioRendererSink is an interface representing the end-point for
// rendered audio.  An implementation is expected to
// periodically call Render() on a callback object.

class AudioRendererSink
    : public base::RefCountedThreadSafe<media::AudioRendererSink> {
public:
    class RenderCallback {
    public:
        // Attempts to completely fill all channels of |dest|, returns actual
        // number of frames filled.
        virtual int Render(AudioBus* dest, int audio_delay_milliseconds) = 0;

        // Signals an error has occurred.
        virtual void OnRenderError() = 0;

    protected:
        virtual ~RenderCallback() { }
    };

    // Sets important information about the audio stream format.
    // It must be called before any of the other methods.
    virtual void Initialize(const AudioParameters& params,
        RenderCallback* callback)
        = 0;

    // Starts audio playback.
    virtual void Start() = 0;

    // Stops audio playback and performs cleanup. It must be called before
    // destruction.
    virtual void Stop() = 0;

    // Pauses playback.
    virtual void Pause() = 0;

    // Resumes playback after calling Pause().
    virtual void Play() = 0;

    // Sets the playback volume, with range [0.0, 1.0] inclusive.
    // Returns |true| on success.
    virtual bool SetVolume(double volume) = 0;

    // Returns a pointer to the internal output device.
    // This pointer is not to be owned by the caller and is valid only during
    // the lifetime of the AudioRendererSink.
    // It can be null, which means that access to the output device is not
    // supported.
    virtual OutputDevice* GetOutputDevice() = 0;

protected:
    friend class base::RefCountedThreadSafe<AudioRendererSink>;
    virtual ~AudioRendererSink() { }
};

// Same as AudioRendererSink except that Initialize() and Start() can be called
// again after Stop().
// TODO(sandersd): Fold back into AudioRendererSink once all subclasses support
// this.

class RestartableAudioRendererSink : public AudioRendererSink {
protected:
    ~RestartableAudioRendererSink() override { }
};

} // namespace media

#endif // MEDIA_BASE_AUDIO_RENDERER_SINK_H_

// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_AUDIO_RENDERER_MIXER_H_
#define MEDIA_BASE_AUDIO_RENDERER_MIXER_H_

#include <map>
#include <string>

#include "base/synchronization/lock.h"
#include "base/time/time.h"
#include "media/base/audio_converter.h"
#include "media/base/audio_renderer_sink.h"

namespace media {

// Mixes a set of AudioConverter::InputCallbacks into a single output stream
// which is funneled into a single shared AudioRendererSink; saving a bundle
// on renderer side resources.
class MEDIA_EXPORT AudioRendererMixer
    : NON_EXPORTED_BASE(public AudioRendererSink::RenderCallback) {
public:
    AudioRendererMixer(const AudioParameters& input_params,
        const AudioParameters& output_params,
        const scoped_refptr<AudioRendererSink>& sink);
    ~AudioRendererMixer() override;

    // Add or remove a mixer input from mixing; called by AudioRendererMixerInput.
    void AddMixerInput(AudioConverter::InputCallback* input);
    void RemoveMixerInput(AudioConverter::InputCallback* input);

    // Since errors may occur even when no inputs are playing, an error callback
    // must be registered separately from adding a mixer input.  The same callback
    // must be given to both the functions.
    void AddErrorCallback(const base::Closure& error_cb);
    void RemoveErrorCallback(const base::Closure& error_cb);

    void set_pause_delay_for_testing(base::TimeDelta delay)
    {
        pause_delay_ = delay;
    }

    // TODO(guidou): remove this method. The output device of a mixer should
    // never be switched, as it may result in a discrepancy between the output
    // parameters of the new device and the output parameters with which the
    // mixer was initialized. See crbug.com/506507
    OutputDevice* GetOutputDevice();

private:
    // AudioRendererSink::RenderCallback implementation.
    int Render(AudioBus* audio_bus, int audio_delay_milliseconds) override;
    void OnRenderError() override;

    // Output sink for this mixer.
    scoped_refptr<AudioRendererSink> audio_sink_;

    // ---------------[ All variables below protected by |lock_| ]---------------
    base::Lock lock_;

    // List of error callbacks used by this mixer.
    typedef std::list<base::Closure> ErrorCallbackList;
    ErrorCallbackList error_callbacks_;

    // Handles mixing and resampling between input and output parameters.
    AudioConverter audio_converter_;

    // Handles physical stream pause when no inputs are playing.  For latency
    // reasons we don't want to immediately pause the physical stream.
    base::TimeDelta pause_delay_;
    base::TimeTicks last_play_time_;
    bool playing_;

    DISALLOW_COPY_AND_ASSIGN(AudioRendererMixer);
};

} // namespace media

#endif // MEDIA_BASE_AUDIO_RENDERER_MIXER_H_

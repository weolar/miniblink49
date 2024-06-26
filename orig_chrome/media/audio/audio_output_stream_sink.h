// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_AUDIO_OUTPUT_STREAM_SINK_H_
#define MEDIA_AUDIO_AUDIO_OUTPUT_STREAM_SINK_H_

#include <string>

#include "base/compiler_specific.h"
#include "base/single_thread_task_runner.h"
#include "base/synchronization/lock.h"
#include "media/audio/audio_io.h"
#include "media/base/audio_renderer_sink.h"
#include "media/base/media_export.h"

namespace media {

// Wrapper which exposes the browser side audio interface (AudioOutputStream) as
// if it were a renderer side audio interface (AudioRendererSink). Note: This
// will not work for sandboxed renderers.
//
// TODO(dalecurtis): Delete this class once we have a proper mojo audio service;
// tracked by http://crbug.com/425368
class MEDIA_EXPORT AudioOutputStreamSink
    : NON_EXPORTED_BASE(public RestartableAudioRendererSink),
      public AudioOutputStream::AudioSourceCallback {
public:
    AudioOutputStreamSink();

    // RestartableAudioRendererSink implementation.
    void Initialize(const AudioParameters& params,
        RenderCallback* callback) override;
    void Start() override;
    void Stop() override;
    void Pause() override;
    void Play() override;
    bool SetVolume(double volume) override;
    OutputDevice* GetOutputDevice() override;

    // AudioSourceCallback implementation.
    int OnMoreData(AudioBus* dest, uint32 total_bytes_delay) override;
    void OnError(AudioOutputStream* stream) override;

private:
    ~AudioOutputStreamSink() override;
    void ClearCallback();

    // Helper methods for running AudioManager methods on the audio thread.
    void DoStart(const AudioParameters& params);
    void DoStop();
    void DoPause();
    void DoPlay();
    void DoSetVolume(double volume);

    bool initialized_;
    bool started_;

    // Parameters provided by Initialize().
    AudioParameters params_;
    RenderCallback* render_callback_;

    // State latched for the audio thread.
    // |active_render_callback_| allows Stop()/Pause() to synchronously prevent
    // callbacks. Access is synchronized by |callback_lock_|.
    // |active_params_| is set on the audio thread and therefore does not need
    // synchronization.
    AudioParameters active_params_;
    RenderCallback* active_render_callback_;

    // Lock to synchronize setting and clearing of |active_render_callback_|.
    base::Lock callback_lock_;

    // The task runner for the audio thread.
    const scoped_refptr<base::SingleThreadTaskRunner> audio_task_runner_;

    // The actual AudioOutputStream, must only be accessed on the audio thread.
    AudioOutputStream* stream_;

    DISALLOW_COPY_AND_ASSIGN(AudioOutputStreamSink);
};

} // namepace media

#endif // MEDIA_AUDIO_AUDIO_OUTPUT_STREAM_SINK_H_

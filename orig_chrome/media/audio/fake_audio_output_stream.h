// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_FAKE_AUDIO_OUTPUT_STREAM_H_
#define MEDIA_AUDIO_FAKE_AUDIO_OUTPUT_STREAM_H_

#include "base/memory/scoped_ptr.h"
#include "media/audio/audio_io.h"
#include "media/audio/audio_parameters.h"
#include "media/audio/fake_audio_worker.h"

namespace media {

class AudioManagerBase;

// A fake implementation of AudioOutputStream.  Used for testing and when a real
// audio output device is unavailable or refusing output (e.g. remote desktop).
// Callbacks are driven on the AudioManager's message loop.
class MEDIA_EXPORT FakeAudioOutputStream : public AudioOutputStream {
public:
    static AudioOutputStream* MakeFakeStream(AudioManagerBase* manager,
        const AudioParameters& params);

    // AudioOutputStream implementation.
    bool Open() override;
    void Start(AudioSourceCallback* callback) override;
    void Stop() override;
    void SetVolume(double volume) override;
    void GetVolume(double* volume) override;
    void Close() override;

private:
    FakeAudioOutputStream(AudioManagerBase* manager,
        const AudioParameters& params);
    ~FakeAudioOutputStream() override;

    // Task that periodically calls OnMoreData() to consume audio data.
    void CallOnMoreData();

    AudioManagerBase* audio_manager_;
    AudioSourceCallback* callback_;
    FakeAudioWorker fake_worker_;
    scoped_ptr<AudioBus> audio_bus_;

    DISALLOW_COPY_AND_ASSIGN(FakeAudioOutputStream);
};

} // namespace media

#endif // MEDIA_AUDIO_FAKE_AUDIO_OUTPUT_STREAM_H_

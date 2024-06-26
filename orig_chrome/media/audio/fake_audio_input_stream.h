// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// A fake implementation of AudioInputStream, useful for testing purpose.

#ifndef MEDIA_AUDIO_FAKE_AUDIO_INPUT_STREAM_H_
#define MEDIA_AUDIO_FAKE_AUDIO_INPUT_STREAM_H_

#include <vector>

#include "base/callback_forward.h"
#include "base/memory/scoped_ptr.h"
#include "media/audio/audio_io.h"
#include "media/audio/audio_parameters.h"
#include "media/audio/fake_audio_worker.h"

namespace media {

class AudioBus;
class AudioManagerBase;
class SimpleSource;

// This class acts as a fake audio input stream. The default is to generate a
// beeping sound unless --use-file-for-fake-audio-capture=<file> is specified,
// in which case the indicated .wav file will be read and played into the
// stream.
class MEDIA_EXPORT FakeAudioInputStream
    : public AudioInputStream {
public:
    static AudioInputStream* MakeFakeStream(
        AudioManagerBase* manager, const AudioParameters& params);

    bool Open() override;
    void Start(AudioInputCallback* callback) override;
    void Stop() override;
    void Close() override;
    double GetMaxVolume() override;
    void SetVolume(double volume) override;
    double GetVolume() override;
    bool IsMuted() override;
    bool SetAutomaticGainControl(bool enabled) override;
    bool GetAutomaticGainControl() override;

    // Generate one beep sound. This method is called by FakeVideoCaptureDevice to
    // test audio/video synchronization. This is a static method because
    // FakeVideoCaptureDevice is disconnected from an audio device. This means
    // only one instance of this class gets to respond, which is okay because we
    // assume there's only one stream for this testing purpose. Furthermore this
    // method will do nothing if --use-file-for-fake-audio-capture is specified
    // since the input stream will be playing from a file instead of beeping.
    // TODO(hclam): Make this non-static. To do this we'll need to fix
    // crbug.com/159053 such that video capture device is aware of audio
    // input stream.
    static void BeepOnce();

private:
    FakeAudioInputStream(AudioManagerBase* manager,
        const AudioParameters& params);
    ~FakeAudioInputStream() override;

    scoped_ptr<AudioOutputStream::AudioSourceCallback> ChooseSource();
    void ReadAudioFromSource();

    AudioManagerBase* audio_manager_;
    AudioInputCallback* callback_;
    FakeAudioWorker fake_audio_worker_;
    AudioParameters params_;

    scoped_ptr<AudioOutputStream::AudioSourceCallback> audio_source_;
    scoped_ptr<media::AudioBus> audio_bus_;

    DISALLOW_COPY_AND_ASSIGN(FakeAudioInputStream);
};

} // namespace media

#endif // MEDIA_AUDIO_FAKE_AUDIO_INPUT_STREAM_H_

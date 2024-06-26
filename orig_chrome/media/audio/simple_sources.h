// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_SIMPLE_SOURCES_H_
#define MEDIA_AUDIO_SIMPLE_SOURCES_H_

#include "base/files/file_path.h"
#include "base/synchronization/lock.h"
#include "media/audio/audio_io.h"
#include "media/base/audio_converter.h"
#include "media/base/seekable_buffer.h"

namespace media {

class WavAudioHandler;

// An audio source that produces a pure sinusoidal tone.
class MEDIA_EXPORT SineWaveAudioSource
    : public AudioOutputStream::AudioSourceCallback {
public:
    // |channels| is the number of audio channels, |freq| is the frequency in
    // hertz and it has to be less than half of the sampling frequency
    // |sample_freq| or else you will get aliasing.
    SineWaveAudioSource(int channels, double freq, double sample_freq);
    ~SineWaveAudioSource() override;

    // Return up to |cap| samples of data via OnMoreData().  Use Reset() to
    // allow more data to be served.
    void CapSamples(int cap);
    void Reset();

    // Implementation of AudioSourceCallback.
    int OnMoreData(AudioBus* audio_bus, uint32 total_bytes_delay) override;
    void OnError(AudioOutputStream* stream) override;

    // The number of OnMoreData() and OnError() calls respectively.
    int callbacks() { return callbacks_; }
    int errors() { return errors_; }

protected:
    int channels_;
    double f_;
    int time_state_;
    int cap_;
    int callbacks_;
    int errors_;
    base::Lock time_lock_;
};

class FileSource : public AudioOutputStream::AudioSourceCallback,
                   public AudioConverter::InputCallback {
public:
    FileSource(const AudioParameters& params,
        const base::FilePath& path_to_wav_file);
    ~FileSource() override;

    // Implementation of AudioSourceCallback.
    int OnMoreData(AudioBus* audio_bus, uint32 total_bytes_delay) override;
    void OnError(AudioOutputStream* stream) override;

private:
    AudioParameters params_;
    base::FilePath path_to_wav_file_;
    scoped_ptr<uint8[]> wav_file_data_;
    scoped_ptr<WavAudioHandler> wav_audio_handler_;
    scoped_ptr<AudioConverter> file_audio_converter_;
    int wav_file_read_pos_;
    bool load_failed_;

    // Provides audio data from wav_audio_handler_ into the file audio converter.
    double ProvideInput(AudioBus* audio_bus,
        base::TimeDelta buffer_delay) override;

    // Loads the wav file on the first OnMoreData invocation.
    void LoadWavFile(const base::FilePath& path_to_wav_file);
};

class BeepingSource : public AudioOutputStream::AudioSourceCallback {
public:
    BeepingSource(const AudioParameters& params);
    ~BeepingSource() override;

    // Implementation of AudioSourceCallback.
    int OnMoreData(AudioBus* audio_bus, uint32 total_bytes_delay) override;
    void OnError(AudioOutputStream* stream) override;

    static void BeepOnce();

private:
    int buffer_size_;
    scoped_ptr<uint8[]> buffer_;
    AudioParameters params_;
    base::TimeTicks last_callback_time_;
    base::TimeDelta interval_from_last_beep_;
    int beep_duration_in_buffers_;
    int beep_generated_in_buffers_;
    int beep_period_in_frames_;
};

} // namespace media

#endif // MEDIA_AUDIO_SIMPLE_SOURCES_H_

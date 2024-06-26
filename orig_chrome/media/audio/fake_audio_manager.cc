// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/fake_audio_manager.h"

namespace media {

namespace {

    const int kDefaultInputBufferSize = 1024;
    const int kDefaultSampleRate = 48000;

} // namespace

FakeAudioManager::FakeAudioManager(AudioLogFactory* audio_log_factory)
    : AudioManagerBase(audio_log_factory)
{
}

FakeAudioManager::~FakeAudioManager()
{
    Shutdown();
}

// Implementation of AudioManager.
bool FakeAudioManager::HasAudioOutputDevices() { return false; }

bool FakeAudioManager::HasAudioInputDevices() { return false; }

// Implementation of AudioManagerBase.
AudioOutputStream* FakeAudioManager::MakeLinearOutputStream(
    const AudioParameters& params)
{
    return FakeAudioOutputStream::MakeFakeStream(this, params);
}

AudioOutputStream* FakeAudioManager::MakeLowLatencyOutputStream(
    const AudioParameters& params,
    const std::string& device_id)
{
    return FakeAudioOutputStream::MakeFakeStream(this, params);
}

AudioInputStream* FakeAudioManager::MakeLinearInputStream(
    const AudioParameters& params,
    const std::string& device_id)
{
    return FakeAudioInputStream::MakeFakeStream(this, params);
}

AudioInputStream* FakeAudioManager::MakeLowLatencyInputStream(
    const AudioParameters& params,
    const std::string& device_id)
{
    return FakeAudioInputStream::MakeFakeStream(this, params);
}

AudioParameters FakeAudioManager::GetPreferredOutputStreamParameters(
    const std::string& output_device_id,
    const AudioParameters& input_params)
{
    static const int kDefaultOutputBufferSize = 2048;
    static const int kDefaultSampleRate = 48000;
    ChannelLayout channel_layout = CHANNEL_LAYOUT_STEREO;
    int sample_rate = kDefaultSampleRate;
    int buffer_size = kDefaultOutputBufferSize;
    int bits_per_sample = 16;
    if (input_params.IsValid()) {
        sample_rate = input_params.sample_rate();
        bits_per_sample = input_params.bits_per_sample();
        channel_layout = input_params.channel_layout();
        buffer_size = std::min(input_params.frames_per_buffer(), buffer_size);
    }

    return AudioParameters(AudioParameters::AUDIO_PCM_LOW_LATENCY, channel_layout,
        sample_rate, bits_per_sample, buffer_size);
}

AudioParameters FakeAudioManager::GetInputStreamParameters(
    const std::string& device_id)
{
    return AudioParameters(
        AudioParameters::AUDIO_PCM_LOW_LATENCY, CHANNEL_LAYOUT_STEREO,
        kDefaultSampleRate, 16, kDefaultInputBufferSize);
}

} // namespace media

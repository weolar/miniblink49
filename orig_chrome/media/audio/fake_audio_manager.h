// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_FAKE_AUDIO_MANAGER_H_
#define MEDIA_AUDIO_FAKE_AUDIO_MANAGER_H_

#include "base/compiler_specific.h"
#include "media/audio/audio_manager_base.h"
#include "media/audio/fake_audio_input_stream.h"
#include "media/audio/fake_audio_output_stream.h"
#include <string>

namespace media {

class MEDIA_EXPORT FakeAudioManager : public AudioManagerBase {
public:
    FakeAudioManager(AudioLogFactory* audio_log_factory);

    // Implementation of AudioManager.
    bool HasAudioOutputDevices() override;
    bool HasAudioInputDevices() override;

    // Implementation of AudioManagerBase.
    AudioOutputStream* MakeLinearOutputStream(
        const AudioParameters& params) override;
    AudioOutputStream* MakeLowLatencyOutputStream(
        const AudioParameters& params,
        const std::string& device_id) override;
    AudioInputStream* MakeLinearInputStream(
        const AudioParameters& params,
        const std::string& device_id) override;
    AudioInputStream* MakeLowLatencyInputStream(
        const AudioParameters& params,
        const std::string& device_id) override;
    AudioParameters GetInputStreamParameters(
        const std::string& device_id) override;

protected:
    ~FakeAudioManager() override;

    AudioParameters GetPreferredOutputStreamParameters(
        const std::string& output_device_id,
        const AudioParameters& input_params) override;

private:
    DISALLOW_COPY_AND_ASSIGN(FakeAudioManager);
};

} // namespace media

#endif // MEDIA_AUDIO_FAKE_AUDIO_MANAGER_H_

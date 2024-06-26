// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_SOUNDS_AUDIO_STREAM_HANDLER_H_
#define MEDIA_AUDIO_SOUNDS_AUDIO_STREAM_HANDLER_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/strings/string_piece.h"
#include "base/threading/non_thread_safe.h"
#include "media/audio/audio_io.h"
#include "media/audio/audio_parameters.h"
#include "media/audio/sounds/wav_audio_handler.h"
#include "media/base/media_export.h"

namespace media {

class AudioManager;

// This class sends a sound to the audio manager.
class MEDIA_EXPORT AudioStreamHandler : public base::NonThreadSafe {
public:
    class TestObserver {
    public:
        virtual ~TestObserver() { }

        // Following methods will be called only from the audio thread.

        // Called when AudioOutputStreamProxy::Start() was successfully called.
        virtual void OnPlay() = 0;

        // Called when AudioOutputStreamProxy::Stop() was successfully called.
        virtual void OnStop(size_t cursor) = 0;
    };

    // C-tor for AudioStreamHandler. |wav_data| should be a raw
    // uncompressed WAVE data which will be sent to the audio manager.
    explicit AudioStreamHandler(const base::StringPiece& wav_data);
    virtual ~AudioStreamHandler();

    // Returns true iff AudioStreamHandler is correctly initialized;
    bool IsInitialized() const;

    // Plays sound.  Volume level will be set according to current settings
    // and won't be changed during playback. Returns true iff new playback
    // was successfully started.
    //
    // NOTE: if current playback isn't at end of stream, playback request
    // is dropped, but true is returned.
    bool Play();

    // Stops current playback.
    void Stop();

    const WavAudioHandler& wav_audio_handler() const { return wav_audio_; }

private:
    friend class AudioStreamHandlerTest;
    friend class SoundsManagerTest;

    class AudioStreamContainer;

    static void SetObserverForTesting(TestObserver* observer);
    static void SetAudioSourceForTesting(
        AudioOutputStream::AudioSourceCallback* source);

    WavAudioHandler wav_audio_;
    scoped_ptr<AudioStreamContainer> stream_;

    bool initialized_;

    DISALLOW_COPY_AND_ASSIGN(AudioStreamHandler);
};

} // namespace media

#endif // MEDIA_AUDIO_SOUNDS_AUDIO_STREAM_HANDLER_H_

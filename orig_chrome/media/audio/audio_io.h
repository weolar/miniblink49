// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_AUDIO_IO_H_
#define MEDIA_AUDIO_AUDIO_IO_H_

#include "base/basictypes.h"
#include "media/base/audio_bus.h"

// Low-level audio output support. To make sound there are 3 objects involved:
// - AudioSource : produces audio samples on a pull model. Implements
//   the AudioSourceCallback interface.
// - AudioOutputStream : uses the AudioSource to render audio on a given
//   channel, format and sample frequency configuration. Data from the
//   AudioSource is delivered in a 'pull' model.
// - AudioManager : factory for the AudioOutputStream objects, manager
//   of the hardware resources and mixer control.
//
// The number and configuration of AudioOutputStream does not need to match the
// physically available hardware resources. For example you can have:
//
//  MonoPCMSource1 --> MonoPCMStream1 --> |       | --> audio left channel
//  StereoPCMSource -> StereoPCMStream -> | mixer |
//  MonoPCMSource2 --> MonoPCMStream2 --> |       | --> audio right channel
//
// This facility's objective is mix and render audio with low overhead using
// the OS basic audio support, abstracting as much as possible the
// idiosyncrasies of each platform. Non-goals:
// - Positional, 3d audio
// - Dependence on non-default libraries such as DirectX 9, 10, XAudio
// - Digital signal processing or effects
// - Extra features if a specific hardware is installed (EAX, X-fi)
//
// The primary client of this facility is audio coming from several tabs.
// Specifically for this case we avoid supporting complex formats such as MP3
// or WMA. Complex format decoding should be done by the renderers.

// Models an audio stream that gets rendered to the audio hardware output.
// Because we support more audio streams than physically available channels
// a given AudioOutputStream might or might not talk directly to hardware.
// An audio stream allocates several buffers for audio data and calls
// AudioSourceCallback::OnMoreData() periodically to fill these buffers,
// as the data is written to the audio device. Size of each packet is determined
// by |samples_per_packet| specified in AudioParameters  when the stream is
// created.

namespace media {

class MEDIA_EXPORT AudioOutputStream {
public:
    // Audio sources must implement AudioSourceCallback. This interface will be
    // called in a random thread which very likely is a high priority thread. Do
    // not rely on using this thread TLS or make calls that alter the thread
    // itself such as creating Windows or initializing COM.
    class MEDIA_EXPORT AudioSourceCallback {
    public:
        virtual ~AudioSourceCallback() { }

        // Provide more data by fully filling |dest|.  The source will return
        // the number of frames it filled.  |total_bytes_delay| contains current
        // number of bytes of delay buffered by the AudioOutputStream.
        virtual int OnMoreData(AudioBus* dest, uint32 total_bytes_delay) = 0;

        // There was an error while playing a buffer. Audio source cannot be
        // destroyed yet. No direct action needed by the AudioStream, but it is
        // a good place to stop accumulating sound data since is is likely that
        // playback will not continue.
        virtual void OnError(AudioOutputStream* stream) = 0;
    };

    virtual ~AudioOutputStream() { }

    // Open the stream. false is returned if the stream cannot be opened.  Open()
    // must always be followed by a call to Close() even if Open() fails.
    virtual bool Open() = 0;

    // Starts playing audio and generating AudioSourceCallback::OnMoreData().
    // Since implementor of AudioOutputStream may have internal buffers, right
    // after calling this method initial buffers are fetched.
    //
    // The output stream does not take ownership of this callback.
    virtual void Start(AudioSourceCallback* callback) = 0;

    // Stops playing audio. Effect might not be instantaneous as the hardware
    // might have locked audio data that is processing.
    virtual void Stop() = 0;

    // Sets the relative volume, with range [0.0, 1.0] inclusive.
    virtual void SetVolume(double volume) = 0;

    // Gets the relative volume, with range [0.0, 1.0] inclusive.
    virtual void GetVolume(double* volume) = 0;

    // Close the stream. This also generates AudioSourceCallback::OnClose().
    // After calling this method, the object should not be used anymore.
    virtual void Close() = 0;
};

// Models an audio sink receiving recorded audio from the audio driver.
class MEDIA_EXPORT AudioInputStream {
public:
    class MEDIA_EXPORT AudioInputCallback {
    public:
        // Called by the audio recorder when a full packet of audio data is
        // available. This is called from a special audio thread and the
        // implementation should return as soon as possible.
        // TODO(henrika): should be pure virtual when old OnData() is phased out.
        virtual void OnData(AudioInputStream* stream,
            const AudioBus* source,
            uint32 hardware_delay_bytes,
            double volume) {};

        // TODO(henrika): don't use; to be removed.
        virtual void OnData(AudioInputStream* stream,
            const uint8* src,
            uint32 size,
            uint32 hardware_delay_bytes,
            double volume) {};

        // There was an error while recording audio. The audio sink cannot be
        // destroyed yet. No direct action needed by the AudioInputStream, but it
        // is a good place to stop accumulating sound data since is is likely that
        // recording will not continue.
        virtual void OnError(AudioInputStream* stream) = 0;

    protected:
        virtual ~AudioInputCallback() { }
    };

    virtual ~AudioInputStream() { }

    // Open the stream and prepares it for recording. Call Start() to actually
    // begin recording.
    virtual bool Open() = 0;

    // Starts recording audio and generating AudioInputCallback::OnData().
    // The input stream does not take ownership of this callback.
    virtual void Start(AudioInputCallback* callback) = 0;

    // Stops recording audio. Effect might not be instantaneous as there could be
    // pending audio callbacks in the queue which will be issued first before
    // recording stops.
    virtual void Stop() = 0;

    // Close the stream. This also generates AudioInputCallback::OnClose(). This
    // should be the last call made on this object.
    virtual void Close() = 0;

    // Returns the maximum microphone analog volume or 0.0 if device does not
    // have volume control.
    virtual double GetMaxVolume() = 0;

    // Sets the microphone analog volume, with range [0, max_volume] inclusive.
    virtual void SetVolume(double volume) = 0;

    // Returns the microphone analog volume, with range [0, max_volume] inclusive.
    virtual double GetVolume() = 0;

    // Sets the Automatic Gain Control (AGC) state.
    virtual bool SetAutomaticGainControl(bool enabled) = 0;

    // Returns the Automatic Gain Control (AGC) state.
    virtual bool GetAutomaticGainControl() = 0;

    // Returns the current muting state for the microphone.
    virtual bool IsMuted() = 0;
};

} // namespace media

#endif // MEDIA_AUDIO_AUDIO_IO_H_

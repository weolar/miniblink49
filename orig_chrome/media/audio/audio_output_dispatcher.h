// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// AudioOutputDispatcher is a single-threaded base class that dispatches
// creation and deletion of audio output streams. AudioOutputProxy objects use
// this class to allocate and recycle actual audio output streams. When playback
// is started, the proxy calls StartStream() to get an output stream that it
// uses to play audio. When playback is stopped, the proxy returns the stream
// back to the dispatcher by calling StopStream().
//
// AudioManagerBase creates one specialization of AudioOutputDispatcher on the
// audio thread for each possible set of audio parameters. I.e streams with
// different parameters are managed independently.  The AudioOutputDispatcher
// instance is then deleted on the audio thread when the AudioManager shuts
// down.

#ifndef MEDIA_AUDIO_AUDIO_OUTPUT_DISPATCHER_H_
#define MEDIA_AUDIO_AUDIO_OUTPUT_DISPATCHER_H_

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "media/audio/audio_io.h"
#include "media/audio/audio_manager.h"
#include "media/audio/audio_parameters.h"

namespace base {
class SingleThreadTaskRunner;
}

namespace media {

class AudioOutputProxy;

class MEDIA_EXPORT AudioOutputDispatcher
    : public base::RefCountedThreadSafe<AudioOutputDispatcher> {
public:
    AudioOutputDispatcher(AudioManager* audio_manager,
        const AudioParameters& params,
        const std::string& device_id);

    // Called by AudioOutputProxy to open the stream.
    // Returns false, if it fails to open it.
    virtual bool OpenStream() = 0;

    // Called by AudioOutputProxy when the stream is started.
    // Uses |callback| to get source data and report errors, if any.
    // Does *not* take ownership of this callback.
    // Returns true if started successfully, false otherwise.
    virtual bool StartStream(AudioOutputStream::AudioSourceCallback* callback,
        AudioOutputProxy* stream_proxy)
        = 0;

    // Called by AudioOutputProxy when the stream is stopped.
    // Ownership of the |stream_proxy| is passed to the dispatcher.
    virtual void StopStream(AudioOutputProxy* stream_proxy) = 0;

    // Called by AudioOutputProxy when the volume is set.
    virtual void StreamVolumeSet(AudioOutputProxy* stream_proxy,
        double volume)
        = 0;

    // Called by AudioOutputProxy when the stream is closed.
    virtual void CloseStream(AudioOutputProxy* stream_proxy) = 0;

    // Called on the audio thread when the AudioManager is shutting down.
    virtual void Shutdown() = 0;

    const std::string& device_id() const { return device_id_; }

protected:
    friend class base::RefCountedThreadSafe<AudioOutputDispatcher>;
    virtual ~AudioOutputDispatcher();

    // A no-reference-held pointer (we don't want circular references) back to the
    // AudioManager that owns this object.
    AudioManager* audio_manager_;
    const scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
    const AudioParameters params_;
    std::string device_id_;

private:
    DISALLOW_COPY_AND_ASSIGN(AudioOutputDispatcher);
};

} // namespace media

#endif // MEDIA_AUDIO_AUDIO_OUTPUT_DISPATCHER_H_

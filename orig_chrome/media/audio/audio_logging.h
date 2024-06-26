// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_AUDIO_LOGGING_H_
#define MEDIA_AUDIO_AUDIO_LOGGING_H_

#include <string>

#include "base/memory/scoped_ptr.h"

namespace media {
class AudioParameters;

// AudioLog logs state information about an active audio component.  Each method
// takes a |component_id| along with method specific information.  Its methods
// are safe to call from any thread.
class AudioLog {
public:
    virtual ~AudioLog() { }

    // Called when an audio component is created.  |params| are the parameters of
    // the created stream.  |device_id| is the id of the audio device opened by
    // the created stream.
    virtual void OnCreated(int component_id,
        const media::AudioParameters& params,
        const std::string& device_id)
        = 0;

    // Called when an audio component is started, generally this is synonymous
    // with "playing."
    virtual void OnStarted(int component_id) = 0;

    // Called when an audio component is stopped, generally this is synonymous
    // with "paused."
    virtual void OnStopped(int component_id) = 0;

    // Called when an audio component is closed, generally this is synonymous
    // with "deleted."
    virtual void OnClosed(int component_id) = 0;

    // Called when an audio component encounters an error.
    virtual void OnError(int component_id) = 0;

    // Called when an audio component changes volume.  |volume| is the new volume.
    virtual void OnSetVolume(int component_id, double volume) = 0;

    // Called when an audio component switches output device. |device_id| is the
    // new audio output device.
    virtual void OnSwitchOutputDevice(int component_id,
        const std::string& device_id)
        = 0;
};

// AudioLogFactory dispenses AudioLog instances to owning classes for tracking
// AudioComponent behavior.  All AudioComponents have the concept of an owning
// class:
//
//    - AudioInputRendererHost for AudioInputController
//    - AudioRendererHost for AudioOutputController
//    - AudioOutputDispatcherImpl for AudioOutputStream
//
// Each of these owning classes may own multiple instances of each component, as
// such each AudioLog supports logging for multiple instances.
class AudioLogFactory {
public:
    enum AudioComponent {
        // Input controllers have a 1:1 mapping with streams, so there's no need to
        // track both controllers and streams.
        AUDIO_INPUT_CONTROLLER,
        // Output controllers may or may not be backed by an active stream, so we
        // need to track both controllers and streams.
        AUDIO_OUTPUT_CONTROLLER,
        AUDIO_OUTPUT_STREAM,
        AUDIO_COMPONENT_MAX
    };

    // Create a new AudioLog object for tracking the behavior for one or more
    // instances of the given component.  Each instance of an "owning" class must
    // create its own AudioLog.
    virtual scoped_ptr<AudioLog> CreateAudioLog(AudioComponent component) = 0;

protected:
    virtual ~AudioLogFactory() { }
};

} // namespace media

#endif // MEDIA_AUDIO_AUDIO_LOGGING_H_

// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_AUDIO_CAPTURER_SOURCE_H_
#define MEDIA_BASE_AUDIO_CAPTURER_SOURCE_H_

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "media/audio/audio_parameters.h"
#include "media/base/audio_bus.h"
#include "media/base/media_export.h"
#include <string>
#include <vector>

namespace media {

// AudioCapturerSource is an interface representing the source for
// captured audio.  An implementation will periodically call Capture() on a
// callback object.
class AudioCapturerSource
    : public base::RefCountedThreadSafe<media::AudioCapturerSource> {
public:
    class CaptureCallback {
    public:
        // Callback to deliver the captured data from the OS.
        virtual void Capture(const AudioBus* audio_source,
            int audio_delay_milliseconds,
            double volume,
            bool key_pressed)
            = 0;

        // Signals an error has occurred.
        virtual void OnCaptureError(const std::string& message) = 0;

    protected:
        virtual ~CaptureCallback() { }
    };

    // Sets information about the audio stream format and the device
    // to be used. It must be called before any of the other methods.
    // The |session_id| is used by the browser to identify which input device to
    // be used. For clients who do not care about device permission and device
    // selection, pass |session_id| using
    // AudioInputDeviceManager::kFakeOpenSessionId.
    virtual void Initialize(const AudioParameters& params,
        CaptureCallback* callback,
        int session_id)
        = 0;

    // Starts the audio recording.
    virtual void Start() = 0;

    // Stops the audio recording. This API is synchronous, and no more data
    // callback will be passed to the client after it is being called.
    virtual void Stop() = 0;

    // Sets the capture volume, with range [0.0, 1.0] inclusive.
    virtual void SetVolume(double volume) = 0;

    // Enables or disables the WebRtc AGC control.
    virtual void SetAutomaticGainControl(bool enable) = 0;

protected:
    friend class base::RefCountedThreadSafe<AudioCapturerSource>;
    virtual ~AudioCapturerSource() { }
};

} // namespace media

#endif // MEDIA_BASE_AUDIO_CAPTURER_SOURCE_H_

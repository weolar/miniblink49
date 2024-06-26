// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_MEDIA_AUDIO_DEVICE_FACTORY_H_
#define CONTENT_RENDERER_MEDIA_AUDIO_DEVICE_FACTORY_H_

#include <string>

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/memory/ref_counted.h"

namespace media {
class AudioInputDevice;
class AudioOutputDevice;
}

namespace url {
class Origin;
}

namespace content {

// A factory for creating AudioOutputDevices and AudioInputDevices.  There is a
// global factory function that can be installed for the purposes of testing to
// provide specialized implementations.
class AudioDeviceFactory {
public:
    // Creates an AudioOutputDevice.
    // |render_frame_id| refers to the RenderFrame containing the entity
    // producing the audio. If |session_id| is nonzero, it is used by the browser
    // to select the correct input device ID and its associated output device, if
    // it exists. If |session_id| is zero, |device_id| and |security_origin|
    // identify the output device to use.
    // If |session_id| is zero and |device_id| and |security_origin| are empty,
    // the default output device will be selected.
    static scoped_refptr<media::AudioOutputDevice> NewOutputDevice(
        int render_frame_id,
        int session_id,
        const std::string& device_id,
        const std::string& security_origin);

    // Creates an AudioInputDevice using the currently registered factory.
    // |render_frame_id| refers to the RenderFrame containing the entity
    // consuming the audio.
    static scoped_refptr<media::AudioInputDevice> NewInputDevice(int render_frame_id);

protected:
    AudioDeviceFactory();
    virtual ~AudioDeviceFactory();

    // You can derive from this class and specify an implementation for these
    // functions to provide alternate audio device implementations.
    // If the return value of either of these function is NULL, we fall back
    // on the default implementation.
    virtual media::AudioOutputDevice* CreateOutputDevice(
        int render_frame_id,
        int sesssion_id,
        const std::string& device_id,
        const std::string& security_origin) = 0;
    virtual media::AudioInputDevice* CreateInputDevice(int render_frame_id) = 0;

private:
    // The current globally registered factory. This is NULL when we should
    // create the default AudioRendererSinks.
    static AudioDeviceFactory* factory_;

    DISALLOW_COPY_AND_ASSIGN(AudioDeviceFactory);
};

} // namespace content

#endif // CONTENT_RENDERER_MEDIA_AUDIO_DEVICE_FACTORY_H_

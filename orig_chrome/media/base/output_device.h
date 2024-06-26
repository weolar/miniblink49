// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OUTPUT_DEVICE_H_
#define MEDIA_BASE_OUTPUT_DEVICE_H_

#include <string>

#include "base/callback.h"
#include "media/audio/audio_parameters.h"
#include "media/base/media_export.h"
//#include "url/origin.h"

namespace media {

// Result of an audio output device switch operation
enum OutputDeviceStatus {
    OUTPUT_DEVICE_STATUS_OK = 0,
    OUTPUT_DEVICE_STATUS_ERROR_NOT_FOUND,
    OUTPUT_DEVICE_STATUS_ERROR_NOT_AUTHORIZED,
    OUTPUT_DEVICE_STATUS_ERROR_INTERNAL,
    OUTPUT_DEVICE_STATUS_LAST = OUTPUT_DEVICE_STATUS_ERROR_INTERNAL,
};

typedef base::Callback<void(OutputDeviceStatus)> SwitchOutputDeviceCB;

// OutputDevice is an interface that allows performing operations related
// audio output devices.

class OutputDevice {
public:
    // Attempts to switch the audio output device.
    // Once the attempt is finished, |callback| is invoked with the
    // result of the operation passed as a parameter. The result is a value from
    // the  media::SwitchOutputDeviceResult enum.
    // There is no guarantee about the thread where |callback| will
    // be invoked, so users are advised to use media::BindToCurrentLoop() to
    // ensure that |callback| runs on the correct thread.
    // Note also that copy constructors and destructors for arguments bound to
    // |callback| may run on arbitrary threads as |callback| is moved across
    // threads. It is advisable to bind arguments such that they are released by
    // |callback| when it runs in order to avoid surprises.
    virtual void SwitchOutputDevice(const std::string& device_id,
        //const url::Origin& security_origin,
        const std::string& security_origin,
        const SwitchOutputDeviceCB& callback)
        = 0;

    // Returns the device's audio output parameters.
    // The return value is undefined if the device status (as returned by
    // GetDeviceStatus()) is different from OUTPUT_DEVICE_STATUS_OK.
    // If the parameters are not available, this method may block until they
    // become available.
    // This method must never be called on the IO thread.
    virtual AudioParameters GetOutputParameters() = 0;

    // Returns the status of output device.
    // If the status is not available, this method may block until it becomes
    // available. Must never be called on the IO thread.
    virtual OutputDeviceStatus GetDeviceStatus() = 0;

protected:
    virtual ~OutputDevice() { }
};

} // namespace media

#endif // MEDIA_BASE_OUTPUT_DEVICE_H_

// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_AUDIO_DEVICE_NAME_H_
#define MEDIA_AUDIO_AUDIO_DEVICE_NAME_H_

#include "media/base/media_export.h"
#include <list>
#include <string>

namespace media {

struct MEDIA_EXPORT AudioDeviceName {
    AudioDeviceName();
    AudioDeviceName(const std::string& device_name,
        const std::string& unique_id);

    std::string device_name; // Friendly name of the device.
    std::string unique_id; // Unique identifier for the device.
};

typedef std::list<AudioDeviceName> AudioDeviceNames;

} // namespace media

#endif // MEDIA_AUDIO_AUDIO_DEVICE_NAME_H_

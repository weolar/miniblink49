// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/audio_device_name.h"

namespace media {

AudioDeviceName::AudioDeviceName() { }

AudioDeviceName::AudioDeviceName(const std::string& device_name,
    const std::string& unique_id)
    : device_name(device_name)
    , unique_id(unique_id)
{
}

} // namespace media

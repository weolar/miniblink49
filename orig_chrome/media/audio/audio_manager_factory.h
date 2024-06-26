// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_AUDIO_MANAGER_FACTORY_H_
#define MEDIA_AUDIO_AUDIO_MANAGER_FACTORY_H_

#include "media/base/media_export.h"

namespace media {

class AudioManager;
class AudioLogFactory;

// Allows a platform-specific implementation of AudioManager to be provided in
// place of the default implementation at run-time.
class MEDIA_EXPORT AudioManagerFactory {
public:
    virtual ~AudioManagerFactory() { }

    // Creates an instance of AudioManager implementation. Caller owns the
    // returned instance. |audio_log_factory| must outlive the returned instance.
    virtual AudioManager* CreateInstance(AudioLogFactory* audio_log_factory) = 0;
};

} // namespace media

#endif // MEDIA_AUDIO_AUDIO_MANAGER_FACTORY_H_

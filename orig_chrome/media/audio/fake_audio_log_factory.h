// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_FAKE_AUDIO_LOG_FACTORY_H_
#define MEDIA_AUDIO_FAKE_AUDIO_LOG_FACTORY_H_

#include "base/compiler_specific.h"
#include "media/audio/audio_logging.h"
#include "media/base/media_export.h"

namespace media {

// Creates stub AudioLog instances, for testing, which do nothing.
class MEDIA_EXPORT FakeAudioLogFactory
    : NON_EXPORTED_BASE(public AudioLogFactory) {
public:
    FakeAudioLogFactory();
    ~FakeAudioLogFactory() override;
    scoped_ptr<AudioLog> CreateAudioLog(AudioComponent component) override;

private:
    DISALLOW_COPY_AND_ASSIGN(FakeAudioLogFactory);
};

} // namespace media

#endif // MEDIA_AUDIO_FAKE_AUDIO_LOG_FACTORY_H_

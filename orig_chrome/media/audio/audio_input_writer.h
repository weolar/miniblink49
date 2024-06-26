// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_AUDIO_INPUT_WRITER_H_
#define MEDIA_AUDIO_AUDIO_INPUT_WRITER_H_

namespace media {

class AudioBus;

// A writer interface used by AudioInputController for writing audio data to
// file for debugging purposes.
class AudioInputWriter {
public:
    virtual ~AudioInputWriter() { }

    // Write |data| to file.
    virtual void Write(scoped_ptr<AudioBus> data) = 0;
};

} // namspace media

#endif // MEDIA_AUDIO_AUDIO_INPUT_WRITER_H_

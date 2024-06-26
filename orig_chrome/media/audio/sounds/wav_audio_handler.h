// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_SOUNDS_WAV_AUDIO_HANDLER_H_
#define MEDIA_AUDIO_SOUNDS_WAV_AUDIO_HANDLER_H_

#include "base/strings/string_piece.h"
#include "base/time/time.h"
#include "media/base/media_export.h"

namespace media {

class AudioBus;

// This class provides the input from wav file format.  See
// https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
class MEDIA_EXPORT WavAudioHandler {
public:
    explicit WavAudioHandler(const base::StringPiece& wav_data);
    virtual ~WavAudioHandler();

    // Returns true when cursor points to the end of the track.
    bool AtEnd(size_t cursor) const;

    // Copies the audio data to |bus| starting from the |cursor| and in
    // the case of success stores the number of written bytes in
    // |bytes_written|. |bytes_written| should not be NULL.
    bool CopyTo(AudioBus* bus, size_t cursor, size_t* bytes_written) const;

    // Accessors.
    const base::StringPiece& data() const { return data_; }
    uint16_t num_channels() const { return num_channels_; }
    uint32_t sample_rate() const { return sample_rate_; }
    uint16_t bits_per_sample() const { return bits_per_sample_; }
    uint32_t total_frames() const { return total_frames_; }

    // Returns the duration of the entire audio chunk.
    base::TimeDelta GetDuration() const;

private:
    // Parses a chunk of wav format data. Returns the length of the chunk.
    int ParseSubChunk(const base::StringPiece& data);

    // Parses the 'fmt' section chunk and stores |params_|.
    bool ParseFmtChunk(const base::StringPiece& data);

    // Parses the 'data' section chunk and stores |data_|.
    bool ParseDataChunk(const base::StringPiece& data);

    // Data part of the |wav_data_|.
    base::StringPiece data_;

    uint16_t num_channels_;
    uint32_t sample_rate_;
    uint16_t bits_per_sample_;
    uint32_t total_frames_;
};

} // namespace media

#endif // MEDIA_AUDIO_SOUNDS_WAV_AUDIO_HANDLER_H_

// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FILTERS_AUDIO_FILE_READER_H_
#define MEDIA_FILTERS_AUDIO_FILE_READER_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "media/base/media_export.h"
#include "media/filters/ffmpeg_glue.h"

struct AVCodecContext;
struct AVPacket;
struct AVStream;

namespace base {
class TimeDelta;
}

namespace media {

class AudioBus;
class FFmpegURLProtocol;

class MEDIA_EXPORT AudioFileReader {
public:
    // Audio file data will be read using the given protocol.
    // The AudioFileReader does not take ownership of |protocol| and
    // simply maintains a weak reference to it.
    explicit AudioFileReader(FFmpegURLProtocol* protocol);
    virtual ~AudioFileReader();

    // Open() reads the audio data format so that the sample_rate(),
    // channels(), GetDuration(), and GetNumberOfFrames() methods can be called.
    // It returns |true| on success.
    bool Open();
    void Close();

    // After a call to Open(), attempts to fully fill |audio_bus| with decoded
    // audio data.  Any unfilled frames will be zeroed out.
    // |audio_data| must be of the same size as channels().
    // The audio data will be decoded as floating-point linear PCM with
    // a nominal range of -1.0 -> +1.0.
    // Returns the number of sample-frames actually read which will always be
    // <= audio_bus->frames()
    int Read(AudioBus* audio_bus);

    // These methods can be called once Open() has been called.
    int channels() const { return channels_; }
    int sample_rate() const { return sample_rate_; }

    // Please note that GetDuration() and GetNumberOfFrames() attempt to be
    // accurate, but are only estimates.  For some encoded formats, the actual
    // duration of the file can only be determined once all the file data has been
    // read. The Read() method returns the actual number of sample-frames it has
    // read.
    base::TimeDelta GetDuration() const;
    int GetNumberOfFrames() const;

    // The methods below are helper methods which allow AudioFileReader to double
    // as a test utility for demuxing audio files.
    // --------------------------------------------------------------------------

    // Similar to Open() but does not initialize the decoder.
    bool OpenDemuxerForTesting();

    // Returns true if a packet could be demuxed from the first audio stream in
    // the file, |output_packet| will contain the demuxed packet then.
    bool ReadPacketForTesting(AVPacket* output_packet);

    // Seeks to the given point and returns true if successful.  |seek_time| will
    // be converted to the stream's time base automatically.
    bool SeekForTesting(base::TimeDelta seek_time);

    const AVStream* GetAVStreamForTesting() const;
    const AVCodecContext* codec_context_for_testing() const
    {
        return codec_context_;
    }

private:
    bool OpenDemuxer();
    bool OpenDecoder();
    bool ReadPacket(AVPacket* output_packet);

    scoped_ptr<FFmpegGlue> glue_;
    AVCodecContext* codec_context_;
    int stream_index_;
    FFmpegURLProtocol* protocol_;
    int channels_;
    int sample_rate_;

    // AVSampleFormat initially requested; not Chrome's SampleFormat.
    int av_sample_format_;

    DISALLOW_COPY_AND_ASSIGN(AudioFileReader);
};

} // namespace media

#endif // MEDIA_FILTERS_AUDIO_FILE_READER_H_

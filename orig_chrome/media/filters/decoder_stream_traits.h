// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FILTERS_DECODER_STREAM_TRAITS_H_
#define MEDIA_FILTERS_DECODER_STREAM_TRAITS_H_

#include "media/base/demuxer_stream.h"
#include "media/base/pipeline_status.h"

namespace media {

class AudioBuffer;
class AudioDecoder;
class DecryptingAudioDecoder;
class DecryptingVideoDecoder;
class DemuxerStream;
class VideoDecoder;
class VideoFrame;

template <DemuxerStream::Type StreamType>
struct DecoderStreamTraits {
};

template <>
struct DecoderStreamTraits<DemuxerStream::AUDIO> {
    typedef AudioBuffer OutputType;
    typedef AudioDecoder DecoderType;
    typedef DecryptingAudioDecoder DecryptingDecoderType;
    typedef base::Callback<void(bool success)> InitCB;
    typedef base::Callback<void(const scoped_refptr<OutputType>&)> OutputCB;

    static std::string ToString();
    static void InitializeDecoder(DecoderType* decoder,
        DemuxerStream* stream,
        const InitCB& init_cb,
        const OutputCB& output_cb);
    static bool NeedsBitstreamConversion(DecoderType* decoder) { return false; }
    static void ReportStatistics(const StatisticsCB& statistics_cb,
        int bytes_decoded);
    static scoped_refptr<OutputType> CreateEOSOutput();
};

template <>
struct DecoderStreamTraits<DemuxerStream::VIDEO> {
    typedef VideoFrame OutputType;
    typedef VideoDecoder DecoderType;
    typedef DecryptingVideoDecoder DecryptingDecoderType;
    typedef base::Callback<void(bool success)> InitCB;
    typedef base::Callback<void(const scoped_refptr<OutputType>&)> OutputCB;

    static std::string ToString();
    static void InitializeDecoder(DecoderType* decoder,
        DemuxerStream* stream,
        const InitCB& init_cb,
        const OutputCB& output_cb);
    static bool NeedsBitstreamConversion(DecoderType* decoder);
    static void ReportStatistics(const StatisticsCB& statistics_cb,
        int bytes_decoded);
    static scoped_refptr<OutputType> CreateEOSOutput();
};

} // namespace media

#endif // MEDIA_FILTERS_DECODER_STREAM_TRAITS_H_

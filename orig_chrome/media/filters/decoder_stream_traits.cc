// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/decoder_stream_traits.h"

#include "base/logging.h"
#include "media/base/audio_buffer.h"
#include "media/base/audio_decoder.h"
#include "media/base/audio_decoder_config.h"
#include "media/base/video_decoder.h"
#include "media/base/video_decoder_config.h"
#include "media/base/video_frame.h"

namespace media {

std::string DecoderStreamTraits<DemuxerStream::AUDIO>::ToString()
{
    return "audio";
}

void DecoderStreamTraits<DemuxerStream::AUDIO>::InitializeDecoder(
    DecoderType* decoder,
    DemuxerStream* stream,
    const InitCB& init_cb,
    const OutputCB& output_cb)
{
    DCHECK(stream->audio_decoder_config().IsValidConfig());
    decoder->Initialize(stream->audio_decoder_config(), init_cb, output_cb);
}

void DecoderStreamTraits<DemuxerStream::AUDIO>::ReportStatistics(
    const StatisticsCB& statistics_cb,
    int bytes_decoded)
{
    PipelineStatistics statistics;
    statistics.audio_bytes_decoded = bytes_decoded;
    statistics_cb.Run(statistics);
}

scoped_refptr<DecoderStreamTraits<DemuxerStream::AUDIO>::OutputType>
DecoderStreamTraits<DemuxerStream::AUDIO>::CreateEOSOutput()
{
    return OutputType::CreateEOSBuffer();
}

std::string DecoderStreamTraits<DemuxerStream::VIDEO>::ToString()
{
    return "video";
}

void DecoderStreamTraits<DemuxerStream::VIDEO>::InitializeDecoder(
    DecoderType* decoder,
    DemuxerStream* stream,
    const InitCB& init_cb,
    const OutputCB& output_cb)
{
    DCHECK(stream->video_decoder_config().IsValidConfig());
    decoder->Initialize(stream->video_decoder_config(),
        stream->liveness() == DemuxerStream::LIVENESS_LIVE,
        init_cb, output_cb);
}

bool DecoderStreamTraits<DemuxerStream::VIDEO>::NeedsBitstreamConversion(
    DecoderType* decoder)
{
    return decoder->NeedsBitstreamConversion();
}

void DecoderStreamTraits<DemuxerStream::VIDEO>::ReportStatistics(
    const StatisticsCB& statistics_cb,
    int bytes_decoded)
{
    PipelineStatistics statistics;
    statistics.video_bytes_decoded = bytes_decoded;
    statistics_cb.Run(statistics);
}

scoped_refptr<DecoderStreamTraits<DemuxerStream::VIDEO>::OutputType>
DecoderStreamTraits<DemuxerStream::VIDEO>::CreateEOSOutput()
{
    return OutputType::CreateEOSFrame();
}

} // namespace media

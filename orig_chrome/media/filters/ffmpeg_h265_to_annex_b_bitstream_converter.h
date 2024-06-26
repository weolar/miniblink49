// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FILTERS_FFMPEG_H265_TO_ANNEX_B_BITSTREAM_CONVERTER_H_
#define MEDIA_FILTERS_FFMPEG_H265_TO_ANNEX_B_BITSTREAM_CONVERTER_H_

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "media/base/media_export.h"
#include "media/filters/ffmpeg_bitstream_converter.h"
#include "media/formats/mp4/hevc.h"

// Forward declarations for FFmpeg datatypes used.
struct AVCodecContext;
struct AVPacket;

namespace media {

// Bitstream converter that converts H.265 bitstream based FFmpeg packets into
// H.265 Annex B bytestream format.
class MEDIA_EXPORT FFmpegH265ToAnnexBBitstreamConverter
    : public FFmpegBitstreamConverter {
public:
    // The |stream_codec_context| will be used during conversion and should be the
    // AVCodecContext for the stream sourcing these packets. A reference to
    // |stream_codec_context| is retained, so it must outlive this class.
    explicit FFmpegH265ToAnnexBBitstreamConverter(AVCodecContext* stream_codec_context);

    ~FFmpegH265ToAnnexBBitstreamConverter() override;

    // FFmpegBitstreamConverter implementation.
    bool ConvertPacket(AVPacket* packet) override;

private:
    scoped_ptr<mp4::HEVCDecoderConfigurationRecord> hevc_config_;

    // Variable to hold a pointer to memory where we can access the global
    // data from the FFmpeg file format's global headers.
    AVCodecContext* stream_codec_context_;

    DISALLOW_COPY_AND_ASSIGN(FFmpegH265ToAnnexBBitstreamConverter);
};

} // namespace media

#endif // MEDIA_FILTERS_FFMPEG_H265_TO_ANNEX_B_BITSTREAM_CONVERTER_H_

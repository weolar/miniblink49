// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/media_file_checker.h"

#include <map>

#include "base/bind.h"
#include "base/time/time.h"
#include "media/ffmpeg/ffmpeg_common.h"
#include "media/filters/blocking_url_protocol.h"
#include "media/filters/ffmpeg_glue.h"
#include "media/filters/file_data_source.h"

namespace media {

static const int64 kMaxCheckTimeInSeconds = 5;

static void OnError(bool* called)
{
    *called = false;
}

MediaFileChecker::MediaFileChecker(base::File file)
    : file_(file.Pass())
{
}

MediaFileChecker::~MediaFileChecker()
{
}

bool MediaFileChecker::Start(base::TimeDelta check_time)
{
    media::FileDataSource source(file_.Pass());
    bool read_ok = true;
    media::BlockingUrlProtocol protocol(&source, base::Bind(&OnError, &read_ok));
    media::FFmpegGlue glue(&protocol);
    AVFormatContext* format_context = glue.format_context();

    if (!glue.OpenContext())
        return false;

    if (avformat_find_stream_info(format_context, NULL) < 0)
        return false;

    // Remember the codec context for any decodable audio or video streams.
    std::map<int, AVCodecContext*> stream_contexts;
    for (size_t i = 0; i < format_context->nb_streams; ++i) {
        AVCodecContext* c = format_context->streams[i]->codec;
        if (c->codec_type == AVMEDIA_TYPE_AUDIO || c->codec_type == AVMEDIA_TYPE_VIDEO) {
            AVCodec* codec = avcodec_find_decoder(c->codec_id);
            if (codec && avcodec_open2(c, codec, NULL) >= 0)
                stream_contexts[i] = c;
        }
    }

    if (stream_contexts.size() == 0)
        return false;

    AVPacket packet;
    scoped_ptr<AVFrame, media::ScopedPtrAVFreeFrame> frame(av_frame_alloc());
    int result = 0;

    const base::TimeTicks deadline = base::TimeTicks::Now() + std::min(check_time, base::TimeDelta::FromSeconds(kMaxCheckTimeInSeconds));
    do {
        result = av_read_frame(glue.format_context(), &packet);
        if (result < 0)
            break;

        std::map<int, AVCodecContext*>::const_iterator it = stream_contexts.find(packet.stream_index);
        if (it == stream_contexts.end()) {
            av_packet_unref(&packet);
            continue;
        }
        AVCodecContext* av_context = it->second;

        int frame_decoded = 0;
        if (av_context->codec_type == AVMEDIA_TYPE_AUDIO) {
            // A shallow copy of packet so we can slide packet.data as frames are
            // decoded; otherwise av_packet_unref() will corrupt memory.
            AVPacket temp_packet = packet;
            do {
                result = avcodec_decode_audio4(av_context, frame.get(), &frame_decoded,
                    &temp_packet);
                if (result < 0)
                    break;
                av_frame_unref(frame.get());
                temp_packet.size -= result;
                temp_packet.data += result;
                frame_decoded = 0;
            } while (temp_packet.size > 0);
        } else if (av_context->codec_type == AVMEDIA_TYPE_VIDEO) {
            result = avcodec_decode_video2(av_context, frame.get(), &frame_decoded,
                &packet);
            if (result >= 0 && frame_decoded)
                av_frame_unref(frame.get());
        }
        av_packet_unref(&packet);
    } while (base::TimeTicks::Now() < deadline && read_ok && result >= 0);

    return read_ok && (result == AVERROR_EOF || result >= 0);
}

} // namespace media

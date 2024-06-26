// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FORMATS_MP2T_ES_ADAPTER_VIDEO_H_
#define MEDIA_FORMATS_MP2T_ES_ADAPTER_VIDEO_H_

#include <deque>
#include <list>
#include <utility>

#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "base/time/time.h"
#include "media/base/media_export.h"
#include "media/base/stream_parser_buffer.h"

namespace media {

class VideoDecoderConfig;

namespace mp2t {

    // Some constraints of the MSE spec are not necessarily met by video streams
    // inside an Mpeg2 TS stream.
    // The goal of the ES adapter is to modify the incoming buffers to meet these
    // constraints, e.g.
    // - get the frame duration,
    // - replace the leading non-key frames by the first key frame to avoid
    //   creating a hole in the video timeline.
    class MEDIA_EXPORT EsAdapterVideo {
    public:
        typedef base::Callback<void(const VideoDecoderConfig&)> NewVideoConfigCB;
        typedef base::Callback<void(scoped_refptr<StreamParserBuffer>)> EmitBufferCB;

        EsAdapterVideo(
            const NewVideoConfigCB& new_video_config_cb,
            const EmitBufferCB& emit_buffer_cb);
        ~EsAdapterVideo();

        // Force the emission of the pending video buffers.
        void Flush();

        // Reset the ES adapter to its initial state.
        void Reset();

        // Provide the configuration that applies to the upcoming video buffers.
        void OnConfigChanged(const VideoDecoderConfig& video_decoder_config);

        // Provide a new video buffer.
        // Returns true when successful.
        bool OnNewBuffer(
            const scoped_refptr<StreamParserBuffer>& stream_parser_buffer);

    private:
        typedef std::deque<scoped_refptr<StreamParserBuffer>> BufferQueue;
        typedef std::pair<int64, VideoDecoderConfig> ConfigEntry;

        void ProcessPendingBuffers(bool flush);

        // Return the PTS of the frame that comes just after |current_pts| in
        // presentation order. Return kNoTimestamp() if not found.
        base::TimeDelta GetNextFramePts(base::TimeDelta current_pts);

        // Replace the leading non key frames by |stream_parser_buffer|
        // (this one must be a key frame).
        void ReplaceDiscardedFrames(
            const scoped_refptr<StreamParserBuffer>& stream_parser_buffer);

        NewVideoConfigCB new_video_config_cb_;
        EmitBufferCB emit_buffer_cb_;

        bool has_valid_config_;
        bool has_valid_frame_;

        // Duration of the last video frame.
        base::TimeDelta last_frame_duration_;

        // Association between a video config and a buffer index.
        std::list<ConfigEntry> config_list_;

        // Global index of the first buffer in |buffer_list_|.
        int64 buffer_index_;

        // List of buffer to be emitted and PTS of frames already emitted.
        BufferQueue buffer_list_;
        std::list<base::TimeDelta> emitted_pts_;

        // Minimum PTS/DTS since the last Reset.
        bool has_valid_initial_timestamp_;
        base::TimeDelta min_pts_;
        DecodeTimestamp min_dts_;

        // Number of frames to replace with the first valid key frame.
        int discarded_frame_count_;

        DISALLOW_COPY_AND_ASSIGN(EsAdapterVideo);
    };

} // namespace mp2t
} // namespace media

#endif // MEDIA_FORMATS_MP2T_ES_ADAPTER_VIDEO_H_

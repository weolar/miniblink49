// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FILTERS_LIBWEBM_MUXER_H_
#define MEDIA_FILTERS_LIBWEBM_MUXER_H_

#include "base/callback.h"
#include "base/numerics/safe_math.h"
#include "base/strings/string_piece.h"
#include "base/threading/thread_checker.h"
#include "base/time/time.h"
#include "media/base/media_export.h"
#include "media/base/video_codecs.h"
#include "third_party/libwebm/source/mkvmuxer.hpp"

namespace gfx {
class Size;
} // namespace gfx

namespace media {

class VideoFrame;
class AudioParameters;

// Adapter class to manage a WebM container [1], a simplified version of a
// Matroska container [2], composed of an EBML header, and a single Segment
// including at least a Track Section and a number of SimpleBlocks each
// containing a single encoded video or audio frame. WebM container has no
// Trailer.
// Clients will push encoded VPx video frames and Opus audio frames one by one
// via OnEncoded{Video|Audio}(). libwebm will eventually ping the WriteDataCB
// passed on contructor with the wrapped encoded data.
// WebmMuxer is designed for use on a single thread.
// [1] http://www.webmproject.org/docs/container/
// [2] http://www.matroska.org/technical/specs/index.html
class MEDIA_EXPORT WebmMuxer : public NON_EXPORTED_BASE(mkvmuxer::IMkvWriter) {
public:
    // Callback to be called when WebmMuxer is ready to write a chunk of data,
    // either any file header or a SingleBlock.
    using WriteDataCB = base::Callback<void(base::StringPiece)>;

    // |codec| can be VP8 or VP9 and should coincide with whatever is sent in
    // OnEncodedVideo().
    WebmMuxer(VideoCodec codec,
        bool has_video_,
        bool has_audio_,
        const WriteDataCB& write_data_callback);
    ~WebmMuxer() override;

    // Functions to add video and audio frames with |encoded_data.data()|
    // to WebM Segment.
    void OnEncodedVideo(const scoped_refptr<VideoFrame>& video_frame,
        scoped_ptr<std::string> encoded_data,
        base::TimeTicks timestamp,
        bool is_key_frame);
    void OnEncodedAudio(const media::AudioParameters& params,
        scoped_ptr<std::string> encoded_data,
        base::TimeTicks timestamp);

private:
    friend class WebmMuxerTest;

    // Methods for creating and adding video and audio tracks, called upon
    // receiving the first frame of a given Track.
    // AddVideoTrack adds |frame_size| and |frame_rate| to the Segment
    // info, although individual frames passed to OnEncodedVideo() can have any
    // frame size.
    void AddVideoTrack(const gfx::Size& frame_size, double frame_rate);
    void AddAudioTrack(const media::AudioParameters& params);

    // IMkvWriter interface.
    mkvmuxer::int32 Write(const void* buf, mkvmuxer::uint32 len) override;
    mkvmuxer::int64 Position() const override;
    mkvmuxer::int32 Position(mkvmuxer::int64 position) override;
    bool Seekable() const override;
    void ElementStartNotify(mkvmuxer::uint64 element_id,
        mkvmuxer::int64 position) override;

    // Used to DCHECK that we are called on the correct thread.
    base::ThreadChecker thread_checker_;

    // Video Codec configured: VP9 if true, otherwise VP8 is used by default.
    const bool use_vp9_;

    // Caller-side identifiers to interact with |segment_|, initialised upon
    // first frame arrival to Add{Video, Audio}Track().
    uint8_t video_track_index_;
    uint8_t audio_track_index_;

    // Origin of times for frame timestamps.
    base::TimeTicks first_frame_timestamp_;
    base::TimeDelta most_recent_timestamp_;

    // TODO(ajose): Change these when support is added for multiple tracks.
    // http://crbug.com/528523
    const bool has_video_;
    const bool has_audio_;

    // Callback to dump written data as being called by libwebm.
    const WriteDataCB write_data_callback_;

    // Rolling counter of the position in bytes of the written goo.
    base::CheckedNumeric<mkvmuxer::int64> position_;

    // The MkvMuxer active element.
    mkvmuxer::Segment segment_;

    DISALLOW_COPY_AND_ASSIGN(WebmMuxer);
};

} // namespace media

#endif // MEDIA_FILTERS_LIBWEBM_MUXER_H_

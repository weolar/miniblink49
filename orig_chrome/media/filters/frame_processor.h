// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FILTERS_FRAME_PROCESSOR_H_
#define MEDIA_FILTERS_FRAME_PROCESSOR_H_

#include <map>

#include "base/basictypes.h"
#include "base/callback_forward.h"
#include "base/time/time.h"
#include "media/base/media_export.h"
#include "media/base/media_log.h"
#include "media/base/stream_parser.h"
#include "media/filters/chunk_demuxer.h"

namespace media {

class MseTrackBuffer;

// Helper class that implements Media Source Extension's coded frame processing
// algorithm.
class MEDIA_EXPORT FrameProcessor {
public:
    typedef base::Callback<void(base::TimeDelta)> UpdateDurationCB;

    // TODO(wolenetz/acolwell): Ensure that all TrackIds are coherent and unique
    // for each track buffer. For now, special track identifiers are used for each
    // of audio and video here, and text TrackIds are assumed to be non-negative.
    // See http://crbug.com/341581.
    enum {
        kAudioTrackId = -2,
        kVideoTrackId = -3
    };

    FrameProcessor(const UpdateDurationCB& update_duration_cb,
        const scoped_refptr<MediaLog>& media_log);
    ~FrameProcessor();

    // Get/set the current append mode, which if true means "sequence" and if
    // false means "segments".
    // See http://www.w3.org/TR/media-source/#widl-SourceBuffer-mode.
    bool sequence_mode() { return sequence_mode_; }
    void SetSequenceMode(bool sequence_mode);

    // Processes buffers in |audio_buffers|, |video_buffers|, and |text_map|.
    // Returns true on success or false on failure which indicates decode error.
    // |append_window_start| and |append_window_end| correspond to the MSE spec's
    // similarly named source buffer attributes that are used in coded frame
    // processing.
    // |*new_media_segment| tracks whether the next buffers processed within the
    // append window represent the start of a new media segment. This method may
    // both use and update this flag.
    // Uses |*timestamp_offset| according to the coded frame processing algorithm,
    // including updating it as required in 'sequence' mode frame processing.
    bool ProcessFrames(const StreamParser::BufferQueue& audio_buffers,
        const StreamParser::BufferQueue& video_buffers,
        const StreamParser::TextBufferQueueMap& text_map,
        base::TimeDelta append_window_start,
        base::TimeDelta append_window_end,
        base::TimeDelta* timestamp_offset);

    // Signals the frame processor to update its group start timestamp to be
    // |timestamp_offset| if it is in sequence append mode.
    void SetGroupStartTimestampIfInSequenceMode(base::TimeDelta timestamp_offset);

    // Adds a new track with unique track ID |id|.
    // If |id| has previously been added, returns false to indicate error.
    // Otherwise, returns true, indicating future ProcessFrames() will emit
    // frames for the track |id| to |stream|.
    bool AddTrack(StreamParser::TrackId id, ChunkDemuxerStream* stream);

    // Updates the internal mapping of TrackId to track buffer for the track
    // buffer formerly associated with |old_id| to be associated with |new_id|.
    // Returns false to indicate failure due to either no existing track buffer
    // for |old_id| or collision with previous track buffer already mapped to
    // |new_id|. Otherwise returns true.
    bool UpdateTrack(StreamParser::TrackId old_id, StreamParser::TrackId new_id);

    // Sets the need random access point flag on all track buffers to true.
    void SetAllTrackBuffersNeedRandomAccessPoint();

    // Resets state for the coded frame processing algorithm as described in steps
    // 2-5 of the MSE Reset Parser State algorithm described at
    // http://www.w3.org/TR/media-source/#sourcebuffer-reset-parser-state
    void Reset();

    // Must be called when the audio config is updated.  Used to manage when
    // the preroll buffer is cleared and the allowed "fudge" factor between
    // preroll buffers.
    void OnPossibleAudioConfigUpdate(const AudioDecoderConfig& config);

private:
    friend class FrameProcessorTest;
    typedef std::map<StreamParser::TrackId, MseTrackBuffer*> TrackBufferMap;

    // If |track_buffers_| contains |id|, returns a pointer to the associated
    // MseTrackBuffer. Otherwise, returns NULL.
    MseTrackBuffer* FindTrack(StreamParser::TrackId id);

    // Signals all track buffers' streams that a coded frame group is starting
    // with decode timestamp |start_timestamp|.
    void NotifyStartOfCodedFrameGroup(DecodeTimestamp start_timestamp);

    // Helper that signals each track buffer to append any processed, but not yet
    // appended, frames to its stream. Returns true on success, or false if one or
    // more of the appends failed.
    bool FlushProcessedFrames();

    // Handles partial append window trimming of |buffer|.  Returns true if the
    // given |buffer| can be partially trimmed or have preroll added; otherwise,
    // returns false.
    //
    // If |buffer| overlaps |append_window_start|, the portion of |buffer| before
    // |append_window_start| will be marked for post-decode discard.  Further, if
    // |audio_preroll_buffer_| exists and abuts |buffer|, it will be set as
    // preroll on |buffer| and |audio_preroll_buffer_| will be cleared.  If the
    // preroll buffer does not abut |buffer|, it will be discarded unused.
    //
    // Likewise, if |buffer| overlaps |append_window_end|, the portion of |buffer|
    // after |append_window_end| will be marked for post-decode discard.
    //
    // If |buffer| lies entirely before |append_window_start|, and thus would
    // normally be discarded, |audio_preroll_buffer_| will be set to |buffer| and
    // the method will return false.
    bool HandlePartialAppendWindowTrimming(
        base::TimeDelta append_window_start,
        base::TimeDelta append_window_end,
        const scoped_refptr<StreamParserBuffer>& buffer);

    // Helper that processes one frame with the coded frame processing algorithm.
    // Returns false on error or true on success.
    bool ProcessFrame(const scoped_refptr<StreamParserBuffer>& frame,
        base::TimeDelta append_window_start,
        base::TimeDelta append_window_end,
        base::TimeDelta* timestamp_offset);

    // TrackId-indexed map of each track's stream.
    TrackBufferMap track_buffers_;

    // The last audio buffer seen by the frame processor that was removed because
    // it was entirely before the start of the append window.
    scoped_refptr<StreamParserBuffer> audio_preroll_buffer_;

    // The AudioDecoderConfig associated with buffers handed to ProcessFrames().
    AudioDecoderConfig current_audio_config_;
    base::TimeDelta sample_duration_;

    // The AppendMode of the associated SourceBuffer.
    // See SetSequenceMode() for interpretation of |sequence_mode_|.
    // Per http://www.w3.org/TR/media-source/#widl-SourceBuffer-mode:
    // Controls how a sequence of media segments are handled. This is initially
    // set to false ("segments").
    bool sequence_mode_ = false;

    // Flag to track whether or not the next processed frame is a continuation of
    // a coded frame group. This flag resets to false upon detection of
    // discontinuity, and becomes true once a processed coded frame for the
    // current coded frame group is sent to its track buffer.
    bool in_coded_frame_group_ = false;

    // Tracks the MSE coded frame processing variable of same name.
    // Initially kNoTimestamp(), meaning "unset".
    base::TimeDelta group_start_timestamp_;

    // Tracks the MSE coded frame processing variable of same name. It stores the
    // highest coded frame end timestamp across all coded frames in the current
    // coded frame group. It is set to 0 when the SourceBuffer object is created
    // and gets updated by ProcessFrames().
    base::TimeDelta group_end_timestamp_;

    UpdateDurationCB update_duration_cb_;

    // MediaLog for reporting messages and properties to debug content and engine.
    scoped_refptr<MediaLog> media_log_;

    // Counters that limit spam to |media_log_| for frame processor warnings.
    int num_dropped_preroll_warnings_ = 0;
    int num_dts_beyond_pts_warnings_ = 0;

    DISALLOW_COPY_AND_ASSIGN(FrameProcessor);
};

} // namespace media

#endif // MEDIA_FILTERS_FRAME_PROCESSOR_H_

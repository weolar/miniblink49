// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/frame_processor.h"

#include <cstdlib>

#include "base/stl_util.h"
#include "media/base/stream_parser_buffer.h"
#include "media/base/timestamp_constants.h"

namespace media {

const int kMaxDroppedPrerollWarnings = 10;
const int kMaxDtsBeyondPtsWarnings = 10;

// Helper class to capture per-track details needed by a frame processor. Some
// of this information may be duplicated in the short-term in the associated
// ChunkDemuxerStream and SourceBufferStream for a track.
// This parallels the MSE spec each of a SourceBuffer's Track Buffers at
// http://www.w3.org/TR/media-source/#track-buffers.
class MseTrackBuffer {
public:
    explicit MseTrackBuffer(ChunkDemuxerStream* stream);
    ~MseTrackBuffer();

    // Get/set |last_decode_timestamp_|.
    DecodeTimestamp last_decode_timestamp() const
    {
        return last_decode_timestamp_;
    }
    void set_last_decode_timestamp(DecodeTimestamp timestamp)
    {
        last_decode_timestamp_ = timestamp;
    }

    // Get/set |last_frame_duration_|.
    base::TimeDelta last_frame_duration() const
    {
        return last_frame_duration_;
    }
    void set_last_frame_duration(base::TimeDelta duration)
    {
        last_frame_duration_ = duration;
    }

    // Gets |highest_presentation_timestamp_|.
    base::TimeDelta highest_presentation_timestamp() const
    {
        return highest_presentation_timestamp_;
    }

    // Get/set |needs_random_access_point_|.
    bool needs_random_access_point() const
    {
        return needs_random_access_point_;
    }
    void set_needs_random_access_point(bool needs_random_access_point)
    {
        needs_random_access_point_ = needs_random_access_point;
    }

    // Gets a pointer to this track's ChunkDemuxerStream.
    ChunkDemuxerStream* stream() const { return stream_; }

    // Unsets |last_decode_timestamp_|, unsets |last_frame_duration_|,
    // unsets |highest_presentation_timestamp_|, and sets
    // |needs_random_access_point_| to true.
    void Reset();

    // If |highest_presentation_timestamp_| is unset or |timestamp| is greater
    // than |highest_presentation_timestamp_|, sets
    // |highest_presentation_timestamp_| to |timestamp|. Note that bidirectional
    // prediction between coded frames can cause |timestamp| to not be
    // monotonically increasing even though the decode timestamps are
    // monotonically increasing.
    void SetHighestPresentationTimestampIfIncreased(base::TimeDelta timestamp);

    // Adds |frame| to the end of |processed_frames_|.
    void EnqueueProcessedFrame(const scoped_refptr<StreamParserBuffer>& frame);

    // Appends |processed_frames_|, if not empty, to |stream_| and clears
    // |processed_frames_|. Returns false if append failed, true otherwise.
    // |processed_frames_| is cleared in both cases.
    bool FlushProcessedFrames();

private:
    // The decode timestamp of the last coded frame appended in the current coded
    // frame group. Initially kNoTimestamp(), meaning "unset".
    DecodeTimestamp last_decode_timestamp_;

    // The coded frame duration of the last coded frame appended in the current
    // coded frame group. Initially kNoTimestamp(), meaning "unset".
    base::TimeDelta last_frame_duration_;

    // The highest presentation timestamp encountered in a coded frame appended
    // in the current coded frame group. Initially kNoTimestamp(), meaning
    // "unset".
    base::TimeDelta highest_presentation_timestamp_;

    // Keeps track of whether the track buffer is waiting for a random access
    // point coded frame. Initially set to true to indicate that a random access
    // point coded frame is needed before anything can be added to the track
    // buffer.
    bool needs_random_access_point_;

    // Pointer to the stream associated with this track. The stream is not owned
    // by |this|.
    ChunkDemuxerStream* const stream_;

    // Queue of processed frames that have not yet been appended to |stream_|.
    // EnqueueProcessedFrame() adds to this queue, and FlushProcessedFrames()
    // clears it.
    StreamParser::BufferQueue processed_frames_;

    DISALLOW_COPY_AND_ASSIGN(MseTrackBuffer);
};

MseTrackBuffer::MseTrackBuffer(ChunkDemuxerStream* stream)
    : last_decode_timestamp_(kNoDecodeTimestamp())
    , last_frame_duration_(kNoTimestamp())
    , highest_presentation_timestamp_(kNoTimestamp())
    , needs_random_access_point_(true)
    , stream_(stream)
{
    DCHECK(stream_);
}

MseTrackBuffer::~MseTrackBuffer()
{
    DVLOG(2) << __FUNCTION__ << "()";
}

void MseTrackBuffer::Reset()
{
    DVLOG(2) << __FUNCTION__ << "()";

    last_decode_timestamp_ = kNoDecodeTimestamp();
    last_frame_duration_ = kNoTimestamp();
    highest_presentation_timestamp_ = kNoTimestamp();
    needs_random_access_point_ = true;
}

void MseTrackBuffer::SetHighestPresentationTimestampIfIncreased(
    base::TimeDelta timestamp)
{
    if (highest_presentation_timestamp_ == kNoTimestamp() || timestamp > highest_presentation_timestamp_) {
        highest_presentation_timestamp_ = timestamp;
    }
}

void MseTrackBuffer::EnqueueProcessedFrame(
    const scoped_refptr<StreamParserBuffer>& frame)
{
    processed_frames_.push_back(frame);
}

bool MseTrackBuffer::FlushProcessedFrames()
{
    if (processed_frames_.empty())
        return true;

    bool result = stream_->Append(processed_frames_);
    processed_frames_.clear();

    DVLOG_IF(3, !result) << __FUNCTION__
                         << "(): Failure appending processed frames to stream";

    return result;
}

FrameProcessor::FrameProcessor(const UpdateDurationCB& update_duration_cb,
    const scoped_refptr<MediaLog>& media_log)
    : group_start_timestamp_(kNoTimestamp())
    , update_duration_cb_(update_duration_cb)
    , media_log_(media_log)
{
    DVLOG(2) << __FUNCTION__ << "()";
    DCHECK(!update_duration_cb.is_null());
}

FrameProcessor::~FrameProcessor()
{
    DVLOG(2) << __FUNCTION__ << "()";
    STLDeleteValues(&track_buffers_);
}

void FrameProcessor::SetSequenceMode(bool sequence_mode)
{
    DVLOG(2) << __FUNCTION__ << "(" << sequence_mode << ")";

    // Per April 1, 2014 MSE spec editor's draft:
    // https://dvcs.w3.org/hg/html-media/raw-file/d471a4412040/media-source/media-source.html#widl-SourceBuffer-mode
    // Step 7: If the new mode equals "sequence", then set the group start
    // timestamp to the group end timestamp.
    if (sequence_mode) {
        DCHECK(kNoTimestamp() != group_end_timestamp_);
        group_start_timestamp_ = group_end_timestamp_;
    }

    // Step 8: Update the attribute to new mode.
    sequence_mode_ = sequence_mode;
}

bool FrameProcessor::ProcessFrames(
    const StreamParser::BufferQueue& audio_buffers,
    const StreamParser::BufferQueue& video_buffers,
    const StreamParser::TextBufferQueueMap& text_map,
    base::TimeDelta append_window_start,
    base::TimeDelta append_window_end,
    base::TimeDelta* timestamp_offset)
{
    StreamParser::BufferQueue frames;
    if (!MergeBufferQueues(audio_buffers, video_buffers, text_map, &frames)) {
        MEDIA_LOG(ERROR, media_log_) << "Parsed buffers not in DTS sequence";
        return false;
    }

    DCHECK(!frames.empty());

    // Implements the coded frame processing algorithm's outer loop for step 1.
    // Note that ProcessFrame() implements an inner loop for a single frame that
    // handles "jump to the Loop Top step to restart processing of the current
    // coded frame" per April 1, 2014 MSE spec editor's draft:
    // https://dvcs.w3.org/hg/html-media/raw-file/d471a4412040/media-source/
    //     media-source.html#sourcebuffer-coded-frame-processing
    // 1. For each coded frame in the media segment run the following steps:
    for (StreamParser::BufferQueue::const_iterator frames_itr = frames.begin(); frames_itr != frames.end(); ++frames_itr) {
        if (!ProcessFrame(*frames_itr, append_window_start, append_window_end, timestamp_offset)) {
            FlushProcessedFrames();
            return false;
        }
    }

    if (!FlushProcessedFrames())
        return false;

    // 2. - 4. Are handled by the WebMediaPlayer / Pipeline / Media Element.

    // Step 5:
    update_duration_cb_.Run(group_end_timestamp_);

    return true;
}

void FrameProcessor::SetGroupStartTimestampIfInSequenceMode(
    base::TimeDelta timestamp_offset)
{
    DVLOG(2) << __FUNCTION__ << "(" << timestamp_offset.InSecondsF() << ")";
    DCHECK(kNoTimestamp() != timestamp_offset);
    if (sequence_mode_)
        group_start_timestamp_ = timestamp_offset;

    // Changes to timestampOffset should invalidate the preroll buffer.
    audio_preroll_buffer_ = NULL;
}

bool FrameProcessor::AddTrack(StreamParser::TrackId id,
    ChunkDemuxerStream* stream)
{
    DVLOG(2) << __FUNCTION__ << "(): id=" << id;

    MseTrackBuffer* existing_track = FindTrack(id);
    DCHECK(!existing_track);
    if (existing_track) {
        MEDIA_LOG(ERROR, media_log_) << "Failure adding track with duplicate ID "
                                     << id;
        return false;
    }

    track_buffers_[id] = new MseTrackBuffer(stream);
    return true;
}

bool FrameProcessor::UpdateTrack(StreamParser::TrackId old_id,
    StreamParser::TrackId new_id)
{
    DVLOG(2) << __FUNCTION__ << "() : old_id=" << old_id << ", new_id=" << new_id;

    if (old_id == new_id || !FindTrack(old_id) || FindTrack(new_id)) {
        MEDIA_LOG(ERROR, media_log_) << "Failure updating track id from " << old_id
                                     << " to " << new_id;
        return false;
    }

    track_buffers_[new_id] = track_buffers_[old_id];
    CHECK_EQ(1u, track_buffers_.erase(old_id));
    return true;
}

void FrameProcessor::SetAllTrackBuffersNeedRandomAccessPoint()
{
    for (TrackBufferMap::iterator itr = track_buffers_.begin();
         itr != track_buffers_.end();
         ++itr) {
        itr->second->set_needs_random_access_point(true);
    }
}

void FrameProcessor::Reset()
{
    DVLOG(2) << __FUNCTION__ << "()";
    for (TrackBufferMap::iterator itr = track_buffers_.begin();
         itr != track_buffers_.end(); ++itr) {
        itr->second->Reset();
    }

    // Maintain current |in_coded_frame_group_| state for Reset() during
    // sequence mode. Reset it here only if in segments mode.
    if (!sequence_mode_) {
        in_coded_frame_group_ = false;
        return;
    }

    // Sequence mode
    DCHECK(kNoTimestamp() != group_end_timestamp_);
    group_start_timestamp_ = group_end_timestamp_;
}

void FrameProcessor::OnPossibleAudioConfigUpdate(
    const AudioDecoderConfig& config)
{
    DCHECK(config.IsValidConfig());

    // Always clear the preroll buffer when a config update is received.
    audio_preroll_buffer_ = NULL;

    if (config.Matches(current_audio_config_))
        return;

    current_audio_config_ = config;
    sample_duration_ = base::TimeDelta::FromSecondsD(
        1.0 / current_audio_config_.samples_per_second());
}

MseTrackBuffer* FrameProcessor::FindTrack(StreamParser::TrackId id)
{
    TrackBufferMap::iterator itr = track_buffers_.find(id);
    if (itr == track_buffers_.end())
        return NULL;

    return itr->second;
}

void FrameProcessor::NotifyStartOfCodedFrameGroup(DecodeTimestamp start_timestamp)
{
    DVLOG(2) << __FUNCTION__ << "(" << start_timestamp.InSecondsF() << ")";

    for (TrackBufferMap::iterator itr = track_buffers_.begin();
         itr != track_buffers_.end();
         ++itr) {
        itr->second->stream()->OnStartOfCodedFrameGroup(start_timestamp);
    }
}

bool FrameProcessor::FlushProcessedFrames()
{
    DVLOG(2) << __FUNCTION__ << "()";

    bool result = true;
    for (TrackBufferMap::iterator itr = track_buffers_.begin();
         itr != track_buffers_.end();
         ++itr) {
        if (!itr->second->FlushProcessedFrames())
            result = false;
    }

    return result;
}

bool FrameProcessor::HandlePartialAppendWindowTrimming(
    base::TimeDelta append_window_start,
    base::TimeDelta append_window_end,
    const scoped_refptr<StreamParserBuffer>& buffer)
{
    DCHECK(buffer->duration() > base::TimeDelta());
    DCHECK_EQ(DemuxerStream::AUDIO, buffer->type());
    DCHECK(buffer->is_key_frame());

    const base::TimeDelta frame_end_timestamp = buffer->timestamp() + buffer->duration();

    // If the buffer is entirely before |append_window_start|, save it as preroll
    // for the first buffer which overlaps |append_window_start|.
    if (buffer->timestamp() < append_window_start && frame_end_timestamp <= append_window_start) {
        audio_preroll_buffer_ = buffer;
        return false;
    }

    // If the buffer is entirely after |append_window_end| there's nothing to do.
    if (buffer->timestamp() >= append_window_end)
        return false;

    DCHECK(buffer->timestamp() >= append_window_start || frame_end_timestamp > append_window_start);

    bool processed_buffer = false;

    // If we have a preroll buffer see if we can attach it to the first buffer
    // overlapping or after |append_window_start|.
    if (audio_preroll_buffer_.get()) {
        // We only want to use the preroll buffer if it directly precedes (less
        // than one sample apart) the current buffer.
        const int64 delta = (audio_preroll_buffer_->timestamp() + audio_preroll_buffer_->duration() - buffer->timestamp())
                                .InMicroseconds();
        if (std::abs(delta) < sample_duration_.InMicroseconds()) {
            DVLOG(1) << "Attaching audio preroll buffer ["
                     << audio_preroll_buffer_->timestamp().InSecondsF() << ", "
                     << (audio_preroll_buffer_->timestamp() + audio_preroll_buffer_->duration()).InSecondsF() << ") to "
                     << buffer->timestamp().InSecondsF();
            buffer->SetPrerollBuffer(audio_preroll_buffer_);
            processed_buffer = true;
        } else {
            LIMITED_MEDIA_LOG(DEBUG, media_log_, num_dropped_preroll_warnings_,
                kMaxDroppedPrerollWarnings)
                << "Partial append window trimming dropping unused audio preroll "
                   "buffer with PTS "
                << audio_preroll_buffer_->timestamp().InMicroseconds()
                << "us that ends too far (" << delta
                << "us) from next buffer with PTS "
                << buffer->timestamp().InMicroseconds() << "us";
        }
        audio_preroll_buffer_ = NULL;
    }

    // See if a partial discard can be done around |append_window_start|.
    if (buffer->timestamp() < append_window_start) {
        DVLOG(1) << "Truncating buffer which overlaps append window start."
                 << " presentation_timestamp " << buffer->timestamp().InSecondsF()
                 << " frame_end_timestamp " << frame_end_timestamp.InSecondsF()
                 << " append_window_start " << append_window_start.InSecondsF();

        // Mark the overlapping portion of the buffer for discard.
        buffer->set_discard_padding(std::make_pair(
            append_window_start - buffer->timestamp(), base::TimeDelta()));

        // Adjust the timestamp of this buffer forward to |append_window_start| and
        // decrease the duration to compensate. Adjust DTS by the same delta as PTS
        // to help prevent spurious discontinuities when DTS > PTS.
        base::TimeDelta pts_delta = append_window_start - buffer->timestamp();
        buffer->set_timestamp(append_window_start);
        buffer->SetDecodeTimestamp(buffer->GetDecodeTimestamp() + pts_delta);
        buffer->set_duration(frame_end_timestamp - append_window_start);
        processed_buffer = true;
    }

    // See if a partial discard can be done around |append_window_end|.
    if (frame_end_timestamp > append_window_end) {
        DVLOG(1) << "Truncating buffer which overlaps append window end."
                 << " presentation_timestamp " << buffer->timestamp().InSecondsF()
                 << " frame_end_timestamp " << frame_end_timestamp.InSecondsF()
                 << " append_window_end " << append_window_end.InSecondsF();

        // Mark the overlapping portion of the buffer for discard.
        buffer->set_discard_padding(
            std::make_pair(buffer->discard_padding().first,
                frame_end_timestamp - append_window_end));

        // Decrease the duration of the buffer to remove the discarded portion.
        buffer->set_duration(append_window_end - buffer->timestamp());
        processed_buffer = true;
    }

    return processed_buffer;
}

bool FrameProcessor::ProcessFrame(
    const scoped_refptr<StreamParserBuffer>& frame,
    base::TimeDelta append_window_start,
    base::TimeDelta append_window_end,
    base::TimeDelta* timestamp_offset)
{
    // Implements the loop within step 1 of the coded frame processing algorithm
    // for a single input frame per April 1, 2014 MSE spec editor's draft:
    // https://dvcs.w3.org/hg/html-media/raw-file/d471a4412040/media-source/
    //     media-source.html#sourcebuffer-coded-frame-processing

    while (true) {
        // 1. Loop Top: Let presentation timestamp be a double precision floating
        //    point representation of the coded frame's presentation timestamp in
        //    seconds.
        // 2. Let decode timestamp be a double precision floating point
        //    representation of the coded frame's decode timestamp in seconds.
        // 3. Let frame duration be a double precision floating point representation
        //    of the coded frame's duration in seconds.
        // We use base::TimeDelta and DecodeTimestamp instead of double.
        base::TimeDelta presentation_timestamp = frame->timestamp();
        DecodeTimestamp decode_timestamp = frame->GetDecodeTimestamp();
        base::TimeDelta frame_duration = frame->duration();

        DVLOG(3) << __FUNCTION__ << ": Processing frame "
                 << "Type=" << frame->type()
                 << ", TrackID=" << frame->track_id()
                 << ", PTS=" << presentation_timestamp.InSecondsF()
                 << ", DTS=" << decode_timestamp.InSecondsF()
                 << ", DUR=" << frame_duration.InSecondsF()
                 << ", RAP=" << frame->is_key_frame();

        // Sanity check the timestamps.
        if (presentation_timestamp == kNoTimestamp()) {
            MEDIA_LOG(ERROR, media_log_) << "Unknown PTS for " << frame->GetTypeName()
                                         << " frame";
            return false;
        }
        if (decode_timestamp == kNoDecodeTimestamp()) {
            MEDIA_LOG(ERROR, media_log_) << "Unknown DTS for " << frame->GetTypeName()
                                         << " frame";
            return false;
        }
        if (decode_timestamp.ToPresentationTime() > presentation_timestamp) {
            // TODO(wolenetz): Determine whether DTS>PTS should really be allowed. See
            // http://crbug.com/354518.
            LIMITED_MEDIA_LOG(DEBUG, media_log_, num_dts_beyond_pts_warnings_,
                kMaxDtsBeyondPtsWarnings)
                << "Parsed " << frame->GetTypeName() << " frame has DTS "
                << decode_timestamp.InMicroseconds()
                << "us, which is after the frame's PTS "
                << presentation_timestamp.InMicroseconds() << "us";
            DVLOG(2) << __FUNCTION__ << ": WARNING: Frame DTS("
                     << decode_timestamp.InSecondsF() << ") > PTS("
                     << presentation_timestamp.InSecondsF()
                     << "), frame type=" << frame->GetTypeName();
        }

        // TODO(acolwell/wolenetz): All stream parsers must emit valid (positive)
        // frame durations. For now, we allow non-negative frame duration.
        // See http://crbug.com/351166.
        if (frame_duration == kNoTimestamp()) {
            MEDIA_LOG(ERROR, media_log_)
                << "Unknown duration for " << frame->GetTypeName() << " frame at PTS "
                << presentation_timestamp.InMicroseconds() << "us";
            return false;
        }
        if (frame_duration < base::TimeDelta()) {
            MEDIA_LOG(ERROR, media_log_)
                << "Negative duration " << frame_duration.InMicroseconds()
                << "us for " << frame->GetTypeName() << " frame at PTS "
                << presentation_timestamp.InMicroseconds() << "us";
            return false;
        }

        // 4. If mode equals "sequence" and group start timestamp is set, then run
        //    the following steps:
        if (sequence_mode_ && group_start_timestamp_ != kNoTimestamp()) {
            // 4.1. Set timestampOffset equal to group start timestamp -
            //      presentation timestamp.
            *timestamp_offset = group_start_timestamp_ - presentation_timestamp;

            DVLOG(3) << __FUNCTION__ << ": updated timestampOffset is now "
                     << timestamp_offset->InSecondsF();

            // 4.2. Set group end timestamp equal to group start timestamp.
            group_end_timestamp_ = group_start_timestamp_;

            // 4.3. Set the need random access point flag on all track buffers to
            //      true.
            SetAllTrackBuffersNeedRandomAccessPoint();

            // 4.4. Unset group start timestamp.
            group_start_timestamp_ = kNoTimestamp();
        }

        // 5. If timestampOffset is not 0, then run the following steps:
        if (*timestamp_offset != base::TimeDelta()) {
            // 5.1. Add timestampOffset to the presentation timestamp.
            // Note: |frame| PTS is only updated if it survives discontinuity
            // processing.
            presentation_timestamp += *timestamp_offset;

            // 5.2. Add timestampOffset to the decode timestamp.
            // Frame DTS is only updated if it survives discontinuity processing.
            decode_timestamp += *timestamp_offset;
        }

        // 6. Let track buffer equal the track buffer that the coded frame will be
        //    added to.

        // Remap audio and video track types to their special singleton identifiers.
        StreamParser::TrackId track_id = kAudioTrackId;
        switch (frame->type()) {
        case DemuxerStream::AUDIO:
            break;
        case DemuxerStream::VIDEO:
            track_id = kVideoTrackId;
            break;
        case DemuxerStream::TEXT:
            track_id = frame->track_id();
            break;
        case DemuxerStream::UNKNOWN:
        case DemuxerStream::NUM_TYPES:
            DCHECK(false) << ": Invalid frame type " << frame->type();
            return false;
        }

        MseTrackBuffer* track_buffer = FindTrack(track_id);
        if (!track_buffer) {
            MEDIA_LOG(ERROR, media_log_)
                << "Unknown track with type " << frame->GetTypeName()
                << ", frame processor track id " << track_id
                << ", and parser track id " << frame->track_id();
            return false;
        }

        // 7. If last decode timestamp for track buffer is set and decode timestamp
        //    is less than last decode timestamp
        //    OR
        //    If last decode timestamp for track buffer is set and the difference
        //    between decode timestamp and last decode timestamp is greater than 2
        //    times last frame duration:
        DecodeTimestamp track_last_decode_timestamp  = track_buffer->last_decode_timestamp();
        if (track_last_decode_timestamp != kNoDecodeTimestamp()) {
            base::TimeDelta track_dts_delta = decode_timestamp - track_last_decode_timestamp;
            if (track_dts_delta < base::TimeDelta() || track_dts_delta > 2 * track_buffer->last_frame_duration()) {
                DCHECK(in_coded_frame_group_);
                // 7.1. If mode equals "segments": Set group end timestamp to
                //      presentation timestamp.
                //      If mode equals "sequence": Set group start timestamp equal to
                //      the group end timestamp.
                if (!sequence_mode_) {
                    group_end_timestamp_ = presentation_timestamp;
                    // This triggers a discontinuity so we need to treat the next frames
                    // appended within the append window as if they were the beginning of
                    // a new segment.
                    in_coded_frame_group_ = true;
                } else {
                    DVLOG(3) << __FUNCTION__ << " : Sequence mode discontinuity, GETS: "
                             << group_end_timestamp_.InSecondsF();
                    DCHECK(kNoTimestamp() != group_end_timestamp_);
                    group_start_timestamp_ = group_end_timestamp_;
                }

                // 7.2. - 7.5.:
                Reset();

                // 7.6. Jump to the Loop Top step above to restart processing of the
                //      current coded frame.
                DVLOG(3) << __FUNCTION__ << ": Discontinuity: reprocessing frame";
                continue;
            }
        }

        // 9. Let frame end timestamp equal the sum of presentation timestamp and
        //    frame duration.
        base::TimeDelta frame_end_timestamp = presentation_timestamp + frame_duration;

        // 10.  If presentation timestamp is less than appendWindowStart, then set
        //      the need random access point flag to true, drop the coded frame, and
        //      jump to the top of the loop to start processing the next coded
        //      frame.
        // Note: We keep the result of partial discard of a buffer that overlaps
        //      |append_window_start| and does not end after |append_window_end|.
        // 11. If frame end timestamp is greater than appendWindowEnd, then set the
        //     need random access point flag to true, drop the coded frame, and jump
        //     to the top of the loop to start processing the next coded frame.
        frame->set_timestamp(presentation_timestamp);
        frame->SetDecodeTimestamp(decode_timestamp);
        if (track_buffer->stream()->supports_partial_append_window_trimming() && HandlePartialAppendWindowTrimming(append_window_start, append_window_end, frame)) {
            // |frame| has been partially trimmed or had preroll added.  Though
            // |frame|'s duration may have changed, do not update |frame_duration|
            // here, so |track_buffer|'s last frame duration update uses original
            // frame duration and reduces spurious discontinuity detection.
            decode_timestamp = frame->GetDecodeTimestamp();
            presentation_timestamp = frame->timestamp();
            frame_end_timestamp = frame->timestamp() + frame->duration();
        }

        if (presentation_timestamp < append_window_start || frame_end_timestamp > append_window_end) {
            track_buffer->set_needs_random_access_point(true);
            DVLOG(3) << "Dropping frame that is outside append window.";
            return true;
        }

        // Note: This step is relocated, versus April 1 spec, to allow append window
        // processing to first filter coded frames shifted by |timestamp_offset_| in
        // such a way that their PTS is negative.
        // 8. If the presentation timestamp or decode timestamp is less than the
        // presentation start time, then run the end of stream algorithm with the
        // error parameter set to "decode", and abort these steps.
        DCHECK(presentation_timestamp >= base::TimeDelta());
        if (decode_timestamp < DecodeTimestamp()) {
            // B-frames may still result in negative DTS here after being shifted by
            // |timestamp_offset_|.
            MEDIA_LOG(ERROR, media_log_)
                << frame->GetTypeName() << " frame with PTS "
                << presentation_timestamp.InMicroseconds() << "us has negative DTS "
                << decode_timestamp.InMicroseconds()
                << "us after applying timestampOffset, handling any discontinuity, "
                   "and filtering against append window";
            return false;
        }

        // 12. If the need random access point flag on track buffer equals true,
        //     then run the following steps:
        if (track_buffer->needs_random_access_point()) {
            // 12.1. If the coded frame is not a random access point, then drop the
            //       coded frame and jump to the top of the loop to start processing
            //       the next coded frame.
            if (!frame->is_key_frame()) {
                DVLOG(3) << __FUNCTION__
                         << ": Dropping frame that is not a random access point";
                return true;
            }

            // 12.2. Set the need random access point flag on track buffer to false.
            track_buffer->set_needs_random_access_point(false);
        }

        // If it is the first in a new coded frame group (such as following a
        // discontinuity), notify all the track buffers' streams that a coded frame
        // group is starting.
        if (!in_coded_frame_group_) {
            // First, complete the append to track buffer streams of the previous
            // coded frame group's frames, if any.
            if (!FlushProcessedFrames())
                return false;

            // TODO(acolwell/wolenetz): This should be changed to a presentation
            // timestamp. See http://crbug.com/402502
            NotifyStartOfCodedFrameGroup(decode_timestamp);
            in_coded_frame_group_ = true;
        }

        DVLOG(3) << __FUNCTION__ << ": Sending processed frame to stream, "
                 << "PTS=" << presentation_timestamp.InSecondsF()
                 << ", DTS=" << decode_timestamp.InSecondsF();

        // Steps 13-18: Note, we optimize by appending groups of contiguous
        // processed frames for each track buffer at end of ProcessFrames() or prior
        // to NotifyNewMediaSegmentStarting().
        // TODO(wolenetz): Refactor SourceBufferStream to conform to spec GC timing.
        // See http://crbug.com/371197.
        track_buffer->EnqueueProcessedFrame(frame);

        // 19. Set last decode timestamp for track buffer to decode timestamp.
        track_buffer->set_last_decode_timestamp(decode_timestamp);

        // 20. Set last frame duration for track buffer to frame duration.
        track_buffer->set_last_frame_duration(frame_duration);

        // 21. If highest presentation timestamp for track buffer is unset or frame
        //     end timestamp is greater than highest presentation timestamp, then
        //     set highest presentation timestamp for track buffer to frame end
        //     timestamp.
        track_buffer->SetHighestPresentationTimestampIfIncreased(
            frame_end_timestamp);

        // 22. If frame end timestamp is greater than group end timestamp, then set
        //     group end timestamp equal to frame end timestamp.
        if (frame_end_timestamp > group_end_timestamp_)
            group_end_timestamp_ = frame_end_timestamp;
        DCHECK(group_end_timestamp_ >= base::TimeDelta());

        return true;
    }

    NOTREACHED();
    return false;
}

} // namespace media

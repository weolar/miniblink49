// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/source_buffer_stream.h"

#include <algorithm>
#include <map>
#include <sstream>

#include "base/bind.h"
#include "base/logging.h"
#include "base/trace_event/trace_event.h"
#include "media/base/audio_splicer.h"
#include "media/base/timestamp_constants.h"
#include "media/filters/source_buffer_platform.h"
#include "media/filters/source_buffer_range.h"

namespace media {

namespace {

    // An arbitrarily-chosen number to estimate the duration of a buffer if none is
    // set and there's not enough information to get a better estimate.
    const int kDefaultBufferDurationInMs = 125;

    // Limit the number of MEDIA_LOG() logs for splice buffer generation warnings
    // and successes. Though these values are high enough to possibly exhaust the
    // media internals event cache (along with other events), these logs are
    // important for debugging splice generation.
    const int kMaxSpliceGenerationWarningLogs = 50;
    const int kMaxSpliceGenerationSuccessLogs = 20;

    // Limit the number of MEDIA_LOG() logs for track buffer time gaps.
    const int kMaxTrackBufferGapWarningLogs = 20;

    // Limit the number of MEDIA_LOG() logs for MSE GC algorithm warnings.
    const int kMaxGarbageCollectAlgorithmWarningLogs = 20;

    // Helper method that returns true if |ranges| is sorted in increasing order,
    // false otherwise.
    bool IsRangeListSorted(const std::list<media::SourceBufferRange*>& ranges)
    {
        DecodeTimestamp prev = kNoDecodeTimestamp();
        for (std::list<SourceBufferRange*>::const_iterator itr = ranges.begin(); itr != ranges.end(); ++itr) {
            if (prev != kNoDecodeTimestamp() && prev >= (*itr)->GetStartTimestamp())
                return false;
            prev = (*itr)->GetEndTimestamp();
        }
        return true;
    }

    // Returns an estimate of how far from the beginning or end of a range a buffer
    // can be to still be considered in the range, given the |approximate_duration|
    // of a buffer in the stream.
    // TODO(wolenetz): Once all stream parsers emit accurate frame durations, use
    // logic like FrameProcessor (2*last_frame_duration + last_decode_timestamp)
    // instead of an overall maximum interbuffer delta for range discontinuity
    // detection, and adjust similarly for splice frame discontinuity detection.
    // See http://crbug.com/351489 and http://crbug.com/351166.
    base::TimeDelta ComputeFudgeRoom(base::TimeDelta approximate_duration)
    {
        // Because we do not know exactly when is the next timestamp, any buffer
        // that starts within 2x the approximate duration of a buffer is considered
        // within this range.
        return 2 * approximate_duration;
    }

    // The amount of time the beginning of the buffered data can differ from the
    // start time in order to still be considered the start of stream.
    base::TimeDelta kSeekToStartFudgeRoom()
    {
        return base::TimeDelta::FromMilliseconds(1000);
    }

    // Helper method for logging, converts a range into a readable string.
    std::string RangeToString(const SourceBufferRange& range)
    {
        if (range.size_in_bytes() == 0) {
            return "[]";
        }
        std::stringstream ss;
        ss << "[" << range.GetStartTimestamp().InSecondsF()
           << ";" << range.GetEndTimestamp().InSecondsF()
           << "(" << range.GetBufferedEndTimestamp().InSecondsF() << ")]";
        return ss.str();
    }

    // Helper method for logging, converts a set of ranges into a readable string.
    std::string RangesToString(const SourceBufferStream::RangeList& ranges)
    {
        if (ranges.empty())
            return "<EMPTY>";

        std::stringstream ss;
        for (const auto* range_ptr : ranges) {
            if (range_ptr != ranges.front())
                ss << " ";
            ss << RangeToString(*range_ptr);
        }
        return ss.str();
    }

    std::string BufferQueueToLogString(
        const SourceBufferStream::BufferQueue& buffers)
    {
        std::stringstream result;
        if (buffers.front()->GetDecodeTimestamp().InMicroseconds() == buffers.front()->timestamp().InMicroseconds() && buffers.back()->GetDecodeTimestamp().InMicroseconds() == buffers.back()->timestamp().InMicroseconds()) {
            result << "dts/pts=[" << buffers.front()->timestamp().InSecondsF() << ";"
                   << buffers.back()->timestamp().InSecondsF() << "(last frame dur="
                   << buffers.back()->duration().InSecondsF() << ")]";
        } else {
            result << "dts=[" << buffers.front()->GetDecodeTimestamp().InSecondsF()
                   << ";" << buffers.back()->GetDecodeTimestamp().InSecondsF()
                   << "] pts=[" << buffers.front()->timestamp().InSecondsF() << ";"
                   << buffers.back()->timestamp().InSecondsF() << "(last frame dur="
                   << buffers.back()->duration().InSecondsF() << ")]";
        }
        return result.str();
    }

    SourceBufferRange::GapPolicy TypeToGapPolicy(SourceBufferStream::Type type)
    {
        switch (type) {
        case SourceBufferStream::kAudio:
        case SourceBufferStream::kVideo:
            return SourceBufferRange::NO_GAPS_ALLOWED;
        case SourceBufferStream::kText:
            return SourceBufferRange::ALLOW_GAPS;
        }

        NOTREACHED();
        return SourceBufferRange::NO_GAPS_ALLOWED;
    }

} // namespace

SourceBufferStream::SourceBufferStream(const AudioDecoderConfig& audio_config,
    const scoped_refptr<MediaLog>& media_log,
    bool splice_frames_enabled)
    : media_log_(media_log)
    , seek_buffer_timestamp_(kNoTimestamp())
    , coded_frame_group_start_time_(kNoDecodeTimestamp())
    , range_for_next_append_(ranges_.end())
    , last_output_buffer_timestamp_(kNoDecodeTimestamp())
    , max_interbuffer_distance_(kNoTimestamp())
    , memory_limit_(kSourceBufferAudioMemoryLimit)
    , splice_frames_enabled_(splice_frames_enabled)
{
    DCHECK(audio_config.IsValidConfig());
    audio_configs_.push_back(audio_config);
}

SourceBufferStream::SourceBufferStream(const VideoDecoderConfig& video_config,
    const scoped_refptr<MediaLog>& media_log,
    bool splice_frames_enabled)
    : media_log_(media_log)
    , seek_buffer_timestamp_(kNoTimestamp())
    , coded_frame_group_start_time_(kNoDecodeTimestamp())
    , range_for_next_append_(ranges_.end())
    , last_output_buffer_timestamp_(kNoDecodeTimestamp())
    , max_interbuffer_distance_(kNoTimestamp())
    , memory_limit_(kSourceBufferVideoMemoryLimit)
    , splice_frames_enabled_(splice_frames_enabled)
{
    DCHECK(video_config.IsValidConfig());
    video_configs_.push_back(video_config);
}

SourceBufferStream::SourceBufferStream(const TextTrackConfig& text_config,
    const scoped_refptr<MediaLog>& media_log,
    bool splice_frames_enabled)
    : media_log_(media_log)
    , text_track_config_(text_config)
    , seek_buffer_timestamp_(kNoTimestamp())
    , coded_frame_group_start_time_(kNoDecodeTimestamp())
    , range_for_next_append_(ranges_.end())
    , last_output_buffer_timestamp_(kNoDecodeTimestamp())
    , max_interbuffer_distance_(kNoTimestamp())
    , memory_limit_(kSourceBufferAudioMemoryLimit)
    , splice_frames_enabled_(splice_frames_enabled)
{
}

SourceBufferStream::~SourceBufferStream()
{
    while (!ranges_.empty()) {
        delete ranges_.front();
        ranges_.pop_front();
    }
}

void SourceBufferStream::OnStartOfCodedFrameGroup(DecodeTimestamp coded_frame_group_start_time)
{
    DVLOG(1) << __FUNCTION__ << " " << GetStreamTypeName() << " (" << coded_frame_group_start_time.InSecondsF() << ")";
    DCHECK(!end_of_stream_);
    coded_frame_group_start_time_ = coded_frame_group_start_time;
    new_coded_frame_group_ = true;

    RangeList::iterator last_range = range_for_next_append_;
    range_for_next_append_ = FindExistingRangeFor(coded_frame_group_start_time);

    // Only reset |last_appended_buffer_timestamp_| if this new media segment is
    // not adjacent to the previous media segment appended to the stream.
    if (range_for_next_append_ == ranges_.end() || !AreAdjacentInSequence(last_appended_buffer_timestamp_, coded_frame_group_start_time)) {
        last_appended_buffer_timestamp_ = kNoDecodeTimestamp();
        last_appended_buffer_duration_ = kNoTimestamp();
        last_appended_buffer_is_keyframe_ = false;
        DVLOG(3) << __FUNCTION__ << " next appended buffers will be in a new range";
    } else if (last_range != ranges_.end()) {
        DCHECK(last_range == range_for_next_append_);
        DVLOG(3) << __FUNCTION__ << " next appended buffers will continue range "
                 << "unless intervening remove makes discontinuity";
    }
}

bool SourceBufferStream::Append(const BufferQueue& buffers)
{
    TRACE_EVENT2("media", "SourceBufferStream::Append",
        "stream type", GetStreamTypeName(),
        "buffers to append", buffers.size());

    DCHECK(!buffers.empty());
    DCHECK(coded_frame_group_start_time_ != kNoDecodeTimestamp());
    DCHECK(coded_frame_group_start_time_ <= buffers.front()->GetDecodeTimestamp());
    DCHECK(!end_of_stream_);

    DVLOG(1) << __FUNCTION__ << " " << GetStreamTypeName()
             << ": buffers " << BufferQueueToLogString(buffers);

//     // New media segments must begin with a keyframe.
//     // TODO(wolenetz): Relax this requirement. See http://crbug.com/229412.
//     if (new_media_segment_ && !buffers.front()->is_key_frame()) {
//         MEDIA_LOG(ERROR, media_log_) << "Media segment did not begin with key "
//                                         "frame. Support for such segments will be "
//                                         "available in a future version. Please see "
//                                         "https://crbug.com/229412.";
//         return false;
//     }

    // New coded frame groups emitted by the coded frame processor must begin with
    // a keyframe. TODO(wolenetz): Change this to [DCHECK + MEDIA_LOG(ERROR...) +
    // return false] once the CHECK has baked in a stable release. See
    // https://crbug.com/580621.
    CHECK(!new_coded_frame_group_ || buffers.front()->is_key_frame());
    // Buffers within a coded frame group should be monotonically increasing.
    if (!IsMonotonicallyIncreasing(buffers))
        return false;

    if (coded_frame_group_start_time_  < DecodeTimestamp() || buffers.front()->GetDecodeTimestamp() < DecodeTimestamp()) {
        MEDIA_LOG(ERROR, media_log_) << "Cannot append a coded frame group with negative timestamps.";
        return false;
    }

    if (!IsNextTimestampValid(buffers.front()->GetDecodeTimestamp(),
            buffers.front()->is_key_frame())) {
        const DecodeTimestamp& dts = buffers.front()->GetDecodeTimestamp();
        MEDIA_LOG(ERROR, media_log_)
            << "Invalid same timestamp construct detected at"
            << " time " << dts.InSecondsF();

        return false;
    }

    UpdateMaxInterbufferDistance(buffers);
    SetConfigIds(buffers);

    // Save a snapshot of stream state before range modifications are made.
    DecodeTimestamp next_buffer_timestamp = GetNextBufferTimestamp();
    BufferQueue deleted_buffers;

    PrepareRangesForNextAppend(buffers, &deleted_buffers);

    // If there's a range for |buffers|, insert |buffers| accordingly. Otherwise,
    // create a new range with |buffers|.
    if (range_for_next_append_ != ranges_.end()) {
        (*range_for_next_append_)->AppendBuffersToEnd(buffers);
        last_appended_buffer_timestamp_ = buffers.back()->GetDecodeTimestamp();
        last_appended_buffer_duration_ = buffers.back()->duration();
        last_appended_buffer_is_keyframe_ = buffers.back()->is_key_frame();
    } else {
        DecodeTimestamp new_range_start_time = std::min(coded_frame_group_start_time_, buffers.front()->GetDecodeTimestamp());
        const BufferQueue* buffers_for_new_range = &buffers;
        BufferQueue trimmed_buffers;

        // If the new range is not being created because of a new coded frame group,
        // then we must make sure that we start with a key frame.  This can happen
        // if the GOP in the previous append gets destroyed by a Remove() call.
        if (!new_coded_frame_group_) {
            BufferQueue::const_iterator itr = buffers.begin();

            // Scan past all the non-key-frames.
            while (itr != buffers.end() && !(*itr)->is_key_frame()) {
                ++itr;
            }

            // If we didn't find a key frame, then update the last appended
            // buffer state and return.
            if (itr == buffers.end()) {
                last_appended_buffer_timestamp_ = buffers.back()->GetDecodeTimestamp();
                last_appended_buffer_duration_ = buffers.back()->duration();
                last_appended_buffer_is_keyframe_ = buffers.back()->is_key_frame();
                DVLOG(1) << __FUNCTION__ << " " << GetStreamTypeName()
                         << ": new buffers in the middle of media segment depend on"
                            "keyframe that has been removed, and contain no keyframes."
                            "Skipping further processing.";
                DVLOG(1) << __FUNCTION__ << " " << GetStreamTypeName()
                         << ": done. ranges_=" << RangesToString(ranges_);
                return true;
            } else if (itr != buffers.begin()) {
                // Copy the first key frame and everything after it into
                // |trimmed_buffers|.
                trimmed_buffers.assign(itr, buffers.end());
                buffers_for_new_range = &trimmed_buffers;
            }

            new_range_start_time = buffers_for_new_range->front()->GetDecodeTimestamp();
        }

        range_for_next_append_ = AddToRanges(new SourceBufferRange(
            TypeToGapPolicy(GetType()),
            *buffers_for_new_range, new_range_start_time,
            base::Bind(&SourceBufferStream::GetMaxInterbufferDistance,
                base::Unretained(this))));
        last_appended_buffer_timestamp_ = buffers_for_new_range->back()->GetDecodeTimestamp();
        last_appended_buffer_duration_ = buffers_for_new_range->back()->duration();
        last_appended_buffer_is_keyframe_ = buffers_for_new_range->back()->is_key_frame();
    }

    new_coded_frame_group_ = false;

    MergeWithAdjacentRangeIfNecessary(range_for_next_append_);

    // Seek to try to fulfill a previous call to Seek().
    if (seek_pending_) {
        DCHECK(!selected_range_);
        DCHECK(deleted_buffers.empty());
        Seek(seek_buffer_timestamp_);
    }

    if (!deleted_buffers.empty()) {
        DecodeTimestamp start_of_deleted = deleted_buffers.front()->GetDecodeTimestamp();

        DCHECK(track_buffer_.empty() || track_buffer_.back()->GetDecodeTimestamp() < start_of_deleted)
            << "decode timestamp "
            << track_buffer_.back()->GetDecodeTimestamp().InSecondsF() << " sec"
            << ", start_of_deleted " << start_of_deleted.InSecondsF() << " sec";

        track_buffer_.insert(track_buffer_.end(), deleted_buffers.begin(),
            deleted_buffers.end());
        DVLOG(3) << __FUNCTION__ << " " << GetStreamTypeName() << " Added "
                 << deleted_buffers.size()
                 << " buffers to track buffer. TB size is now "
                 << track_buffer_.size();
    } else {
        DVLOG(3) << __FUNCTION__ << " " << GetStreamTypeName()
                 << " No deleted buffers for track buffer";
    }

    // Prune any extra buffers in |track_buffer_| if new keyframes
    // are appended to the range covered by |track_buffer_|.
    if (!track_buffer_.empty()) {
        DecodeTimestamp keyframe_timestamp = FindKeyframeAfterTimestamp(track_buffer_.front()->GetDecodeTimestamp());
        if (keyframe_timestamp != kNoDecodeTimestamp())
            PruneTrackBuffer(keyframe_timestamp);
    }

    SetSelectedRangeIfNeeded(next_buffer_timestamp);

    DVLOG(1) << __FUNCTION__ << " " << GetStreamTypeName()
             << ": done. ranges_=" << RangesToString(ranges_);
    DCHECK(IsRangeListSorted(ranges_));
    DCHECK(OnlySelectedRangeIsSeeked());
    return true;
}

void SourceBufferStream::Remove(base::TimeDelta start, base::TimeDelta end,
    base::TimeDelta duration)
{
    DVLOG(1) << __FUNCTION__ << " " << GetStreamTypeName()
             << " (" << start.InSecondsF() << ", " << end.InSecondsF()
             << ", " << duration.InSecondsF() << ")";
    DCHECK(start >= base::TimeDelta()) << start.InSecondsF();
    DCHECK(start < end) << "start " << start.InSecondsF()
                        << " end " << end.InSecondsF();
    DCHECK(duration != kNoTimestamp());

    DecodeTimestamp start_dts = DecodeTimestamp::FromPresentationTime(start);
    DecodeTimestamp end_dts = DecodeTimestamp::FromPresentationTime(end);
    DecodeTimestamp remove_end_timestamp = DecodeTimestamp::FromPresentationTime(duration);
    DecodeTimestamp keyframe_timestamp = FindKeyframeAfterTimestamp(end_dts);
    if (keyframe_timestamp != kNoDecodeTimestamp()) {
        remove_end_timestamp = keyframe_timestamp;
    } else if (end_dts < remove_end_timestamp) {
        remove_end_timestamp = end_dts;
    }

    BufferQueue deleted_buffers;
    RemoveInternal(start_dts, remove_end_timestamp, false, &deleted_buffers);

    if (!deleted_buffers.empty()) {
        // Buffers for the current position have been removed.
        SetSelectedRangeIfNeeded(deleted_buffers.front()->GetDecodeTimestamp());
        if (last_output_buffer_timestamp_ == kNoDecodeTimestamp()) {
            // We just removed buffers for the current playback position for this
            // stream, yet we also had output no buffer since the last Seek.
            // Re-seek to prevent stall.
            DVLOG(1) << __FUNCTION__ << " " << GetStreamTypeName()
                     << ": re-seeking to " << seek_buffer_timestamp_
                     << " to prevent stall if this time becomes buffered again";
            Seek(seek_buffer_timestamp_);
        }
    }
}

void SourceBufferStream::RemoveInternal(DecodeTimestamp start,
    DecodeTimestamp end,
    bool exclude_start,
    BufferQueue* deleted_buffers)
{
    DVLOG(2) << __FUNCTION__ << " " << GetStreamTypeName() << " ("
             << start.InSecondsF() << ", " << end.InSecondsF() << ", "
             << exclude_start << ")";
    DVLOG(3) << __FUNCTION__ << " " << GetStreamTypeName()
             << ": before remove ranges_=" << RangesToString(ranges_);

    DCHECK(start >= DecodeTimestamp());
    DCHECK(start < end) << "start " << start.InSecondsF()
                        << " end " << end.InSecondsF();
    DCHECK(deleted_buffers);

    RangeList::iterator itr = ranges_.begin();

    while (itr != ranges_.end()) {
        SourceBufferRange* range = *itr;
        if (range->GetStartTimestamp() >= end)
            break;

        // Split off any remaining GOPs starting at or after |end| and add it to
        // |ranges_|.
        SourceBufferRange* new_range = range->SplitRange(end);
        if (new_range) {
            itr = ranges_.insert(++itr, new_range);
            --itr;

            // Update the selected range if the next buffer position was transferred
            // to |new_range|.
            if (new_range->HasNextBufferPosition())
                SetSelectedRange(new_range);
        }

        // Truncate the current range so that it only contains data before
        // the removal range.
        BufferQueue saved_buffers;
        bool delete_range = range->TruncateAt(start, &saved_buffers, exclude_start);

        // Check to see if the current playback position was removed and
        // update the selected range appropriately.
        if (!saved_buffers.empty()) {
            DCHECK(!range->HasNextBufferPosition());
            DCHECK(deleted_buffers->empty());

            *deleted_buffers = saved_buffers;
        }

        if (range == selected_range_ && !range->HasNextBufferPosition())
            SetSelectedRange(NULL);

        // If the current range now is completely covered by the removal
        // range then delete it and move on.
        if (delete_range) {
            DeleteAndRemoveRange(&itr);
            continue;
        }

        // Clear |range_for_next_append_| if we determine that the removal
        // operation makes it impossible for the next append to be added
        // to the current range.
        if (range_for_next_append_ != ranges_.end() && *range_for_next_append_ == range && last_appended_buffer_timestamp_ != kNoDecodeTimestamp()) {
            DecodeTimestamp potential_next_append_timestamp = last_appended_buffer_timestamp_ + base::TimeDelta::FromInternalValue(1);

            if (!range->BelongsToRange(potential_next_append_timestamp)) {
                DVLOG(1) << "Resetting range_for_next_append_ since the next append"
                         << " can't add to the current range.";
                range_for_next_append_ = FindExistingRangeFor(potential_next_append_timestamp);
            }
        }

        // Move on to the next range.
        ++itr;
    }

    DVLOG(3) << __FUNCTION__ << " " << GetStreamTypeName()
             << ": after remove ranges_=" << RangesToString(ranges_);

    DCHECK(IsRangeListSorted(ranges_));
    DCHECK(OnlySelectedRangeIsSeeked());
}

void SourceBufferStream::ResetSeekState()
{
    SetSelectedRange(NULL);
    track_buffer_.clear();
    config_change_pending_ = false;
    last_output_buffer_timestamp_ = kNoDecodeTimestamp();
    just_exhausted_track_buffer_ = false;
    splice_buffers_index_ = 0;
    pending_buffer_ = NULL;
    pending_buffers_complete_ = false;
}

bool SourceBufferStream::ShouldSeekToStartOfBuffered(
    base::TimeDelta seek_timestamp) const
{
    if (ranges_.empty())
        return false;
    base::TimeDelta beginning_of_buffered = ranges_.front()->GetStartTimestamp().ToPresentationTime();
    return (seek_timestamp <= beginning_of_buffered && beginning_of_buffered < kSeekToStartFudgeRoom());
}

bool SourceBufferStream::IsMonotonicallyIncreasing(
    const BufferQueue& buffers) const
{
    DCHECK(!buffers.empty());
    DecodeTimestamp prev_timestamp = last_appended_buffer_timestamp_;
    bool prev_is_keyframe = last_appended_buffer_is_keyframe_;
    for (BufferQueue::const_iterator itr = buffers.begin();
         itr != buffers.end(); ++itr) {
        DecodeTimestamp current_timestamp = (*itr)->GetDecodeTimestamp();
        bool current_is_keyframe = (*itr)->is_key_frame();
        DCHECK(current_timestamp != kNoDecodeTimestamp());
        DCHECK((*itr)->duration() >= base::TimeDelta())
            << "Packet with invalid duration."
            << " pts " << (*itr)->timestamp().InSecondsF()
            << " dts " << (*itr)->GetDecodeTimestamp().InSecondsF()
            << " dur " << (*itr)->duration().InSecondsF();

        if (prev_timestamp != kNoDecodeTimestamp()) {
            if (current_timestamp < prev_timestamp) {
                MEDIA_LOG(ERROR, media_log_)
                    << "Buffers did not monotonically increase.";
                return false;
            }

            if (current_timestamp == prev_timestamp && !SourceBufferRange::AllowSameTimestamp(prev_is_keyframe, current_is_keyframe)) {
                MEDIA_LOG(ERROR, media_log_) << "Unexpected combination of buffers with"
                                             << " the same timestamp detected at "
                                             << current_timestamp.InSecondsF();
                return false;
            }
        }

        prev_timestamp = current_timestamp;
        prev_is_keyframe = current_is_keyframe;
    }
    return true;
}

bool SourceBufferStream::IsNextTimestampValid(
    DecodeTimestamp next_timestamp, bool next_is_keyframe) const
{
    return (last_appended_buffer_timestamp_ != next_timestamp) || new_coded_frame_group_ || SourceBufferRange::AllowSameTimestamp(last_appended_buffer_is_keyframe_, next_is_keyframe);
}

bool SourceBufferStream::OnlySelectedRangeIsSeeked() const
{
    for (RangeList::const_iterator itr = ranges_.begin();
         itr != ranges_.end(); ++itr) {
        if ((*itr)->HasNextBufferPosition() && (*itr) != selected_range_)
            return false;
    }
    return !selected_range_ || selected_range_->HasNextBufferPosition();
}

void SourceBufferStream::UpdateMaxInterbufferDistance(
    const BufferQueue& buffers)
{
    DCHECK(!buffers.empty());
    DecodeTimestamp prev_timestamp = last_appended_buffer_timestamp_;
    for (BufferQueue::const_iterator itr = buffers.begin();
         itr != buffers.end(); ++itr) {
        DecodeTimestamp current_timestamp = (*itr)->GetDecodeTimestamp();
        DCHECK(current_timestamp != kNoDecodeTimestamp());

        base::TimeDelta interbuffer_distance = (*itr)->duration();
        DCHECK(interbuffer_distance >= base::TimeDelta());

        if (prev_timestamp != kNoDecodeTimestamp()) {
            interbuffer_distance = std::max(current_timestamp - prev_timestamp, interbuffer_distance);
        }

        if (interbuffer_distance > base::TimeDelta()) {
            if (max_interbuffer_distance_ == kNoTimestamp()) {
                max_interbuffer_distance_ = interbuffer_distance;
            } else {
                max_interbuffer_distance_ = std::max(max_interbuffer_distance_, interbuffer_distance);
            }
        }
        prev_timestamp = current_timestamp;
    }
}

void SourceBufferStream::SetConfigIds(const BufferQueue& buffers)
{
    for (BufferQueue::const_iterator itr = buffers.begin();
         itr != buffers.end(); ++itr) {
        (*itr)->SetConfigId(append_config_index_);
    }
}

bool SourceBufferStream::GarbageCollectIfNeeded(DecodeTimestamp media_time,
    size_t newDataSize)
{
    DCHECK(media_time != kNoDecodeTimestamp());
    // Garbage collection should only happen before/during appending new data,
    // which should not happen in end-of-stream state.
    DCHECK(!end_of_stream_);
    // Compute size of |ranges_|.
    size_t ranges_size = GetBufferedSize();

    // Sanity and overflow checks
    if ((newDataSize > memory_limit_) || (ranges_size + newDataSize < ranges_size)) {
        LIMITED_MEDIA_LOG(DEBUG, media_log_, num_garbage_collect_algorithm_logs_,
            kMaxGarbageCollectAlgorithmWarningLogs)
            << GetStreamTypeName() << " stream: "
            << "new append of newDataSize=" << newDataSize
            << " bytes exceeds memory_limit_=" << memory_limit_
            << ", currently buffered ranges_size=" << ranges_size;
        return false;
    }

    // Return if we're under or at the memory limit.
    if (ranges_size + newDataSize <= memory_limit_)
        return true;

    size_t bytes_to_free = ranges_size + newDataSize - memory_limit_;

    DVLOG(2) << __FUNCTION__ << " " << GetStreamTypeName() << ": Before GC"
             << " media_time=" << media_time.InSecondsF()
             << " ranges_=" << RangesToString(ranges_)
             << " seek_pending_=" << seek_pending_
             << " ranges_size=" << ranges_size
             << " newDataSize=" << newDataSize
             << " memory_limit_=" << memory_limit_
             << " last_appended_buffer_timestamp_="
             << last_appended_buffer_timestamp_.InSecondsF();

    if (selected_range_ && !seek_pending_ && media_time > selected_range_->GetBufferedEndTimestamp()) {
        // Strictly speaking |media_time| (taken from HTMLMediaElement::currentTime)
        // should always be in the buffered ranges, but media::Pipeline uses audio
        // stream as the main time source, when audio is present.
        // In cases when audio and video streams have different buffered ranges, the
        // |media_time| value might be slightly outside of the video stream buffered
        // range. In those cases we need to clamp |media_time| value to the current
        // stream buffered ranges, to ensure the MSE garbage collection algorithm
        // works correctly (see crbug.com/563292 for details).
        DecodeTimestamp selected_buffered_end = selected_range_->GetBufferedEndTimestamp();

        DVLOG(2) << __FUNCTION__ << " media_time " << media_time.InSecondsF()
                 << " is outside of selected_range_=["
                 << selected_range_->GetStartTimestamp().InSecondsF() << ";"
                 << selected_buffered_end.InSecondsF()
                 << "] clamping media_time to be "
                 << selected_buffered_end.InSecondsF();
        media_time = selected_buffered_end;
    }

    size_t bytes_freed = 0;

    // If last appended buffer position was earlier than the current playback time
    // then try deleting data between last append and current media_time.
    if (last_appended_buffer_timestamp_ != kNoDecodeTimestamp() && last_appended_buffer_duration_ != kNoTimestamp() && media_time > last_appended_buffer_timestamp_ + last_appended_buffer_duration_) {
        size_t between = FreeBuffersAfterLastAppended(bytes_to_free, media_time);
        DVLOG(3) << __FUNCTION__ << " FreeBuffersAfterLastAppended "
                 << " released " << between << " bytes"
                 << " ranges_=" << RangesToString(ranges_);
        bytes_freed += between;

        // Some players start appending data at the new seek target position before
        // actually initiating the seek operation (i.e. they try to improve seek
        // performance by prebuffering some data at the seek target position and
        // initiating seek once enough data is pre-buffered. In those cases we'll
        // see that data is being appended at some new position, but there is no
        // pending seek reported yet. In this situation we need to try preserving
        // the most recently appended data, i.e. data belonging to the same buffered
        // range as the most recent append.
        if (range_for_next_append_ != ranges_.end()) {
            DCHECK((*range_for_next_append_)->GetStartTimestamp() <= media_time);
            media_time = (*range_for_next_append_)->GetStartTimestamp();
            DVLOG(3) << __FUNCTION__ << " media_time adjusted to "
                     << media_time.InSecondsF();
        }
    }

    // If there is an unsatisfied pending seek, we can safely remove all data that
    // is earlier than seek target, then remove from the back until we reach the
    // most recently appended GOP and then remove from the front if we still don't
    // have enough space for the upcoming append.
    if (bytes_freed < bytes_to_free && seek_pending_) {
        DCHECK(!ranges_.empty());
        // All data earlier than the seek target |media_time| can be removed safely
        size_t front = FreeBuffers(bytes_to_free - bytes_freed, media_time, false);
        DVLOG(3) << __FUNCTION__ << " Removed " << front << " bytes from the"
                 << " front. ranges_=" << RangesToString(ranges_);
        bytes_freed += front;

        // If removing data earlier than |media_time| didn't free up enough space,
        // then try deleting from the back until we reach most recently appended GOP
        if (bytes_freed < bytes_to_free) {
            size_t back = FreeBuffers(bytes_to_free - bytes_freed, media_time, true);
            DVLOG(3) << __FUNCTION__ << " Removed " << back << " bytes from the back"
                     << " ranges_=" << RangesToString(ranges_);
            bytes_freed += back;
        }

        // If even that wasn't enough, then try greedily deleting from the front,
        // that should allow us to remove as much data as necessary to succeed.
        if (bytes_freed < bytes_to_free) {
            size_t front2 = FreeBuffers(bytes_to_free - bytes_freed,
                ranges_.back()->GetEndTimestamp(), false);
            DVLOG(3) << __FUNCTION__ << " Removed " << front << " bytes from the"
                     << " front. ranges_=" << RangesToString(ranges_);
            bytes_freed += front2;
        }
        DCHECK(bytes_freed >= bytes_to_free);
    }

    // Try removing data from the front of the SourceBuffer up to |media_time|
    // position.
    if (bytes_freed < bytes_to_free) {
        size_t front = FreeBuffers(bytes_to_free - bytes_freed, media_time, false);
        DVLOG(3) << __FUNCTION__ << " Removed " << front << " bytes from the"
                 << " front. ranges_=" << RangesToString(ranges_);
        bytes_freed += front;
    }

    // Try removing data from the back of the SourceBuffer, until we reach the
    // most recent append position.
    if (bytes_freed < bytes_to_free) {
        size_t back = FreeBuffers(bytes_to_free - bytes_freed, media_time, true);
        DVLOG(3) << __FUNCTION__ << " Removed " << back << " bytes from the back."
                 << " ranges_=" << RangesToString(ranges_);
        bytes_freed += back;
    }

    DVLOG(2) << __FUNCTION__ << " " << GetStreamTypeName() << ": After GC"
             << " bytes_to_free=" << bytes_to_free
             << " bytes_freed=" << bytes_freed
             << " ranges_=" << RangesToString(ranges_);

    return bytes_freed >= bytes_to_free;
}

size_t SourceBufferStream::FreeBuffersAfterLastAppended(
    size_t total_bytes_to_free, DecodeTimestamp media_time)
{
    DVLOG(4) << __FUNCTION__ << " last_appended_buffer_timestamp_="
             << last_appended_buffer_timestamp_.InSecondsF()
             << " media_time=" << media_time.InSecondsF();

    DecodeTimestamp remove_range_start = last_appended_buffer_timestamp_;
    if (last_appended_buffer_is_keyframe_)
        remove_range_start += GetMaxInterbufferDistance();

    DecodeTimestamp remove_range_start_keyframe = FindKeyframeAfterTimestamp(
        remove_range_start);
    if (remove_range_start_keyframe != kNoDecodeTimestamp())
        remove_range_start = remove_range_start_keyframe;
    if (remove_range_start >= media_time)
        return 0;

    DecodeTimestamp remove_range_end;
    size_t bytes_freed = GetRemovalRange(remove_range_start,
        media_time,
        total_bytes_to_free,
        &remove_range_end);
    if (bytes_freed > 0) {
        DVLOG(4) << __FUNCTION__ << " removing ["
                 << remove_range_start.ToPresentationTime().InSecondsF() << ";"
                 << remove_range_end.ToPresentationTime().InSecondsF() << "]";
        Remove(remove_range_start.ToPresentationTime(),
            remove_range_end.ToPresentationTime(),
            media_time.ToPresentationTime());
    }

    return bytes_freed;
}

size_t SourceBufferStream::GetRemovalRange(
    DecodeTimestamp start_timestamp, DecodeTimestamp end_timestamp,
    size_t total_bytes_to_free, DecodeTimestamp* removal_end_timestamp)
{
    DCHECK(start_timestamp >= DecodeTimestamp()) << start_timestamp.InSecondsF();
    DCHECK(start_timestamp < end_timestamp)
        << "start " << start_timestamp.InSecondsF()
        << ", end " << end_timestamp.InSecondsF();

    size_t bytes_freed = 0;

    for (RangeList::iterator itr = ranges_.begin();
         itr != ranges_.end() && bytes_freed < total_bytes_to_free; ++itr) {
        SourceBufferRange* range = *itr;
        if (range->GetStartTimestamp() >= end_timestamp)
            break;
        if (range->GetEndTimestamp() < start_timestamp)
            continue;

        size_t bytes_to_free = total_bytes_to_free - bytes_freed;
        size_t bytes_removed = range->GetRemovalGOP(
            start_timestamp, end_timestamp, bytes_to_free, removal_end_timestamp);
        bytes_freed += bytes_removed;
    }
    return bytes_freed;
}

size_t SourceBufferStream::FreeBuffers(size_t total_bytes_to_free,
    DecodeTimestamp media_time,
    bool reverse_direction)
{
    TRACE_EVENT2("media", "SourceBufferStream::FreeBuffers",
        "total bytes to free", total_bytes_to_free,
        "reverse direction", reverse_direction);

    DCHECK_GT(total_bytes_to_free, 0u);
    size_t bytes_freed = 0;

    // This range will save the last GOP appended to |range_for_next_append_|
    // if the buffers surrounding it get deleted during garbage collection.
    SourceBufferRange* new_range_for_append = NULL;

    while (!ranges_.empty() && bytes_freed < total_bytes_to_free) {
        SourceBufferRange* current_range = NULL;
        BufferQueue buffers;
        size_t bytes_deleted = 0;

        if (reverse_direction) {
            current_range = ranges_.back();
            DVLOG(5) << "current_range=" << RangeToString(*current_range);
            if (current_range->LastGOPContainsNextBufferPosition()) {
                DCHECK_EQ(current_range, selected_range_);
                DVLOG(5) << "current_range contains next read position, stopping GC";
                break;
            }
            DVLOG(5) << "Deleting GOP from back: " << RangeToString(*current_range);
            bytes_deleted = current_range->DeleteGOPFromBack(&buffers);
        } else {
            current_range = ranges_.front();
            DVLOG(5) << "current_range=" << RangeToString(*current_range);
            if (!current_range->FirstGOPEarlierThanMediaTime(media_time)) {
                // We have removed all data up to the GOP that contains current playback
                // position, we can't delete any further.
                DVLOG(5) << "current_range contains playback position, stopping GC";
                break;
            }
            DVLOG(4) << "Deleting GOP from front: " << RangeToString(*current_range);
            bytes_deleted = current_range->DeleteGOPFromFront(&buffers);
        }

        // Check to see if we've just deleted the GOP that was last appended.
        DecodeTimestamp end_timestamp = buffers.back()->GetDecodeTimestamp();
        if (end_timestamp == last_appended_buffer_timestamp_) {
            DCHECK(last_appended_buffer_timestamp_ != kNoDecodeTimestamp());
            DCHECK(!new_range_for_append);

            // Create a new range containing these buffers.
            new_range_for_append = new SourceBufferRange(
                TypeToGapPolicy(GetType()),
                buffers, kNoDecodeTimestamp(),
                base::Bind(&SourceBufferStream::GetMaxInterbufferDistance,
                    base::Unretained(this)));
            range_for_next_append_ = ranges_.end();
        } else {
            bytes_freed += bytes_deleted;
        }

        if (current_range->size_in_bytes() == 0) {
            DCHECK_NE(current_range, selected_range_);
            DCHECK(range_for_next_append_ == ranges_.end() || *range_for_next_append_ != current_range);
            delete current_range;
            reverse_direction ? ranges_.pop_back() : ranges_.pop_front();
        }

        if (reverse_direction && new_range_for_append) {
            // We don't want to delete any further, or we'll be creating gaps
            break;
        }
    }

    // Insert |new_range_for_append| into |ranges_|, if applicable.
    if (new_range_for_append) {
        range_for_next_append_ = AddToRanges(new_range_for_append);
        DCHECK(range_for_next_append_ != ranges_.end());

        // Check to see if we need to merge |new_range_for_append| with the range
        // before or after it. |new_range_for_append| is created whenever the last
        // GOP appended is encountered, regardless of whether any buffers after it
        // are ultimately deleted. Merging is necessary if there were no buffers
        // (or very few buffers) deleted after creating |new_range_for_append|.
        if (range_for_next_append_ != ranges_.begin()) {
            RangeList::iterator range_before_next = range_for_next_append_;
            --range_before_next;
            MergeWithAdjacentRangeIfNecessary(range_before_next);
        }
        MergeWithAdjacentRangeIfNecessary(range_for_next_append_);
    }
    return bytes_freed;
}

void SourceBufferStream::PrepareRangesForNextAppend(
    const BufferQueue& new_buffers, BufferQueue* deleted_buffers)
{
    DCHECK(deleted_buffers);

    // Handle splices between the existing buffers and the new buffers.  If a
    // splice is generated the timestamp and duration of the first buffer in
    // |new_buffers| will be modified.
    if (splice_frames_enabled_)
        GenerateSpliceFrame(new_buffers);

    DecodeTimestamp prev_timestamp = last_appended_buffer_timestamp_;
    bool prev_is_keyframe = last_appended_buffer_is_keyframe_;
    DecodeTimestamp next_timestamp = new_buffers.front()->GetDecodeTimestamp();
    bool next_is_keyframe = new_buffers.front()->is_key_frame();

    if (prev_timestamp != kNoDecodeTimestamp() && prev_timestamp != next_timestamp) {
        // Clean up the old buffers between the last appended buffer and the
        // beginning of |new_buffers|.
        RemoveInternal(prev_timestamp, next_timestamp, true, deleted_buffers);
    }

    // Make the delete range exclusive if we are dealing with an allowed same
    // timestamp situation. This prevents the first buffer in the current append
    // from deleting the last buffer in the previous append if both buffers
    // have the same timestamp.
    //
    // The delete range should never be exclusive if a splice frame was generated
    // because we don't generate splice frames for same timestamp situations.
    DCHECK(new_buffers.front()->splice_timestamp() != new_buffers.front()->timestamp());
    const bool exclude_start = new_buffers.front()->splice_buffers().empty() && prev_timestamp == next_timestamp && SourceBufferRange::AllowSameTimestamp(prev_is_keyframe, next_is_keyframe);

    // Delete the buffers that |new_buffers| overlaps.
    DecodeTimestamp start = new_buffers.front()->GetDecodeTimestamp();
    DecodeTimestamp end = new_buffers.back()->GetDecodeTimestamp();
    base::TimeDelta duration = new_buffers.back()->duration();

    // Set end time for remove to include the duration of last buffer. If the
    // duration is estimated, use 1 microsecond instead to ensure frames are not
    // accidentally removed due to over-estimation.
    if (duration != kNoTimestamp() && duration > base::TimeDelta() && !new_buffers.back()->is_duration_estimated()) {
        end += duration;
    } else {
        // TODO(chcunningham): Emit warning when 0ms durations are not expected.
        // http://crbug.com/312836
        end += base::TimeDelta::FromInternalValue(1);
    }

    RemoveInternal(start, end, exclude_start, deleted_buffers);
}

bool SourceBufferStream::AreAdjacentInSequence(
    DecodeTimestamp first_timestamp, DecodeTimestamp second_timestamp) const
{
    return first_timestamp < second_timestamp && second_timestamp <= first_timestamp + ComputeFudgeRoom(GetMaxInterbufferDistance());
}

void SourceBufferStream::PruneTrackBuffer(const DecodeTimestamp timestamp)
{
    // If we don't have the next timestamp, we don't have anything to delete.
    if (timestamp == kNoDecodeTimestamp())
        return;

    while (!track_buffer_.empty() && track_buffer_.back()->GetDecodeTimestamp() >= timestamp) {
        track_buffer_.pop_back();
    }
    DVLOG(3) << __FUNCTION__ << " " << GetStreamTypeName()
             << " Removed all buffers with DTS >= " << timestamp.InSecondsF()
             << ". New track buffer size:" << track_buffer_.size();
}

void SourceBufferStream::MergeWithAdjacentRangeIfNecessary(
    const RangeList::iterator& range_with_new_buffers_itr)
{
    DCHECK(range_with_new_buffers_itr != ranges_.end());

    SourceBufferRange* range_with_new_buffers = *range_with_new_buffers_itr;
    RangeList::iterator next_range_itr = range_with_new_buffers_itr;
    ++next_range_itr;

    if (next_range_itr == ranges_.end() || !range_with_new_buffers->CanAppendRangeToEnd(**next_range_itr)) {
        return;
    }

    bool transfer_current_position = selected_range_ == *next_range_itr;
    DVLOG(3) << __FUNCTION__ << " " << GetStreamTypeName()
             << " merging " << RangeToString(*range_with_new_buffers)
             << " into " << RangeToString(**next_range_itr);
    range_with_new_buffers->AppendRangeToEnd(**next_range_itr,
        transfer_current_position);
    // Update |selected_range_| pointer if |range| has become selected after
    // merges.
    if (transfer_current_position)
        SetSelectedRange(range_with_new_buffers);

    if (next_range_itr == range_for_next_append_)
        range_for_next_append_ = range_with_new_buffers_itr;

    DeleteAndRemoveRange(&next_range_itr);
}

void SourceBufferStream::Seek(base::TimeDelta timestamp)
{
    DCHECK(timestamp >= base::TimeDelta());
    DVLOG(1) << __FUNCTION__ << " " << GetStreamTypeName()
             << " (" << timestamp.InSecondsF() << ")";
    ResetSeekState();

    seek_buffer_timestamp_ = timestamp;
    seek_pending_ = true;

    if (ShouldSeekToStartOfBuffered(timestamp)) {
        ranges_.front()->SeekToStart();
        SetSelectedRange(ranges_.front());
        seek_pending_ = false;
        return;
    }

    DecodeTimestamp seek_dts = DecodeTimestamp::FromPresentationTime(timestamp);

    RangeList::iterator itr = ranges_.end();
    for (itr = ranges_.begin(); itr != ranges_.end(); ++itr) {
        if ((*itr)->CanSeekTo(seek_dts))
            break;
    }

    if (itr == ranges_.end())
        return;

    SeekAndSetSelectedRange(*itr, seek_dts);
    seek_pending_ = false;
}

bool SourceBufferStream::IsSeekPending() const
{
    return seek_pending_ && !IsEndOfStreamReached();
}

void SourceBufferStream::OnSetDuration(base::TimeDelta duration)
{
    DecodeTimestamp duration_dts = DecodeTimestamp::FromPresentationTime(duration);
    DVLOG(1) << __FUNCTION__ << " " << GetStreamTypeName()
             << " (" << duration.InSecondsF() << ")";

    RangeList::iterator itr = ranges_.end();
    for (itr = ranges_.begin(); itr != ranges_.end(); ++itr) {
        if ((*itr)->GetEndTimestamp() > duration_dts)
            break;
    }
    if (itr == ranges_.end())
        return;

    // Need to partially truncate this range.
    if ((*itr)->GetStartTimestamp() < duration_dts) {
        bool delete_range = (*itr)->TruncateAt(duration_dts, NULL, false);
        if ((*itr == selected_range_) && !selected_range_->HasNextBufferPosition())
            SetSelectedRange(NULL);

        if (delete_range) {
            DeleteAndRemoveRange(&itr);
        } else {
            ++itr;
        }
    }

    // Delete all ranges that begin after |duration_dts|.
    while (itr != ranges_.end()) {
        // If we're about to delete the selected range, also reset the seek state.
        DCHECK((*itr)->GetStartTimestamp() >= duration_dts);
        if (*itr == selected_range_)
            ResetSeekState();
        DeleteAndRemoveRange(&itr);
    }
}

SourceBufferStream::Status SourceBufferStream::GetNextBuffer(
    scoped_refptr<StreamParserBuffer>* out_buffer)
{
    DVLOG(2) << __FUNCTION__ << " " << GetStreamTypeName();
    if (!pending_buffer_.get()) {
        const SourceBufferStream::Status status = GetNextBufferInternal(out_buffer);
        if (status != SourceBufferStream::kSuccess || !SetPendingBuffer(out_buffer)) {
            DVLOG(2) << __FUNCTION__ << " " << GetStreamTypeName()
                     << ": no pending buffer, returning status " << status;
            return status;
        }
    }

    if (!pending_buffer_->splice_buffers().empty()) {
        const SourceBufferStream::Status status = HandleNextBufferWithSplice(out_buffer);
        DVLOG(2) << __FUNCTION__ << " " << GetStreamTypeName()
                 << ": handled next buffer with splice, returning status "
                 << status;
        return status;
    }

    DCHECK(pending_buffer_->preroll_buffer().get());

    const SourceBufferStream::Status status = HandleNextBufferWithPreroll(out_buffer);
    DVLOG(2) << __FUNCTION__ << " " << GetStreamTypeName()
             << ": handled next buffer with preroll, returning status "
             << status;
    return status;
}

SourceBufferStream::Status SourceBufferStream::HandleNextBufferWithSplice(
    scoped_refptr<StreamParserBuffer>* out_buffer)
{
    const BufferQueue& splice_buffers = pending_buffer_->splice_buffers();
    const size_t last_splice_buffer_index = splice_buffers.size() - 1;

    // Are there any splice buffers left to hand out?  The last buffer should be
    // handed out separately since it represents the first post-splice buffer.
    if (splice_buffers_index_ < last_splice_buffer_index) {
        // Account for config changes which occur between fade out buffers.
        if (current_config_index_ != splice_buffers[splice_buffers_index_]->GetConfigId()) {
            config_change_pending_ = true;
            DVLOG(1) << "Config change (splice buffer config ID does not match).";
            return SourceBufferStream::kConfigChange;
        }

        // Every pre splice buffer must have the same splice_timestamp().
        DCHECK(pending_buffer_->splice_timestamp() == splice_buffers[splice_buffers_index_]->splice_timestamp());

        // No pre splice buffers should have preroll.
        DCHECK(!splice_buffers[splice_buffers_index_]->preroll_buffer().get());

        *out_buffer = splice_buffers[splice_buffers_index_++];
        return SourceBufferStream::kSuccess;
    }

    // Did we hand out the last pre-splice buffer on the previous call?
    if (!pending_buffers_complete_) {
        DCHECK_EQ(splice_buffers_index_, last_splice_buffer_index);
        pending_buffers_complete_ = true;
        config_change_pending_ = true;
        DVLOG(1) << "Config change (forced for fade in of splice frame).";
        return SourceBufferStream::kConfigChange;
    }

    // All pre-splice buffers have been handed out and a config change completed,
    // so hand out the final buffer for fade in.  Because a config change is
    // always issued prior to handing out this buffer, any changes in config id
    // have been inherently handled.
    DCHECK(pending_buffers_complete_);
    DCHECK_EQ(splice_buffers_index_, splice_buffers.size() - 1);
    DCHECK(splice_buffers.back()->splice_timestamp() == kNoTimestamp());
    *out_buffer = splice_buffers.back();
    pending_buffer_ = NULL;

    // If the last splice buffer has preroll, hand off to the preroll handler.
    return SetPendingBuffer(out_buffer) ? HandleNextBufferWithPreroll(out_buffer)
                                        : SourceBufferStream::kSuccess;
}

SourceBufferStream::Status SourceBufferStream::HandleNextBufferWithPreroll(
    scoped_refptr<StreamParserBuffer>* out_buffer)
{
    // Any config change should have already been handled.
    DCHECK_EQ(current_config_index_, pending_buffer_->GetConfigId());

    // Check if the preroll buffer has already been handed out.
    if (!pending_buffers_complete_) {
        pending_buffers_complete_ = true;
        *out_buffer = pending_buffer_->preroll_buffer();
        return SourceBufferStream::kSuccess;
    }

    // Preroll complete, hand out the final buffer.
    *out_buffer = pending_buffer_;
    pending_buffer_ = NULL;
    return SourceBufferStream::kSuccess;
}

SourceBufferStream::Status SourceBufferStream::GetNextBufferInternal(
    scoped_refptr<StreamParserBuffer>* out_buffer)
{
    CHECK(!config_change_pending_);

    if (!track_buffer_.empty()) {
        DCHECK(!selected_range_);
        scoped_refptr<StreamParserBuffer>& next_buffer = track_buffer_.front();

        // If the next buffer is an audio splice frame, the next effective config id
        // comes from the first splice buffer.
        if (next_buffer->GetSpliceBufferConfigId(0) != current_config_index_) {
            config_change_pending_ = true;
            DVLOG(1) << "Config change (track buffer config ID does not match).";
            return kConfigChange;
        }

        DVLOG(3) << __FUNCTION__ << " Next buffer coming from track_buffer_";
        *out_buffer = next_buffer;
        track_buffer_.pop_front();
        WarnIfTrackBufferExhaustionSkipsForward(*out_buffer);
        last_output_buffer_timestamp_ = (*out_buffer)->GetDecodeTimestamp();

        // If the track buffer becomes empty, then try to set the selected range
        // based on the timestamp of this buffer being returned.
        if (track_buffer_.empty()) {
            just_exhausted_track_buffer_ = true;
            SetSelectedRangeIfNeeded(last_output_buffer_timestamp_);
        }

        return kSuccess;
    }

    DCHECK(track_buffer_.empty());
    if (!selected_range_ || !selected_range_->HasNextBuffer()) {
        if (IsEndOfStreamReached()) {
            return kEndOfStream;
        }
        DVLOG(3) << __FUNCTION__ << " " << GetStreamTypeName()
                 << ": returning kNeedBuffer "
                 << (selected_range_ ? "(selected range has no next buffer)"
                                     : "(no selected range)");
        return kNeedBuffer;
    }

    if (selected_range_->GetNextConfigId() != current_config_index_) {
        config_change_pending_ = true;
        DVLOG(1) << "Config change (selected range config ID does not match).";
        return kConfigChange;
    }

    CHECK(selected_range_->GetNextBuffer(out_buffer));
    WarnIfTrackBufferExhaustionSkipsForward(*out_buffer);
    last_output_buffer_timestamp_ = (*out_buffer)->GetDecodeTimestamp();
    return kSuccess;
}

void SourceBufferStream::WarnIfTrackBufferExhaustionSkipsForward(
    const scoped_refptr<StreamParserBuffer>& next_buffer)
{
    if (!just_exhausted_track_buffer_)
        return;

    just_exhausted_track_buffer_ = false;
    DCHECK(next_buffer->is_key_frame());
    DecodeTimestamp next_output_buffer_timestamp = next_buffer->GetDecodeTimestamp();
    base::TimeDelta delta = next_output_buffer_timestamp - last_output_buffer_timestamp_;
    DCHECK_GE(delta, base::TimeDelta());
    if (delta > GetMaxInterbufferDistance()) {
        LIMITED_MEDIA_LOG(DEBUG, media_log_, num_track_buffer_gap_warning_logs_,
            kMaxTrackBufferGapWarningLogs)
            << "Media append that overlapped current playback position caused time "
               "gap in playing "
            << GetStreamTypeName() << " stream because the next keyframe is "
            << delta.InMilliseconds() << "ms beyond last overlapped frame. Media "
                                         "may appear temporarily frozen.";
    }
}

DecodeTimestamp SourceBufferStream::GetNextBufferTimestamp()
{
    if (!track_buffer_.empty())
        return track_buffer_.front()->GetDecodeTimestamp();

    if (!selected_range_)
        return kNoDecodeTimestamp();

    DCHECK(selected_range_->HasNextBufferPosition());
    return selected_range_->GetNextTimestamp();
}

SourceBufferStream::RangeList::iterator
SourceBufferStream::FindExistingRangeFor(DecodeTimestamp start_timestamp)
{
    for (RangeList::iterator itr = ranges_.begin(); itr != ranges_.end(); ++itr) {
        if ((*itr)->BelongsToRange(start_timestamp))
            return itr;
    }
    return ranges_.end();
}

SourceBufferStream::RangeList::iterator
SourceBufferStream::AddToRanges(SourceBufferRange* new_range)
{
    DecodeTimestamp start_timestamp = new_range->GetStartTimestamp();
    RangeList::iterator itr = ranges_.end();
    for (itr = ranges_.begin(); itr != ranges_.end(); ++itr) {
        if ((*itr)->GetStartTimestamp() > start_timestamp)
            break;
    }
    return ranges_.insert(itr, new_range);
}

SourceBufferStream::RangeList::iterator
SourceBufferStream::GetSelectedRangeItr()
{
    DCHECK(selected_range_);
    RangeList::iterator itr = ranges_.end();
    for (itr = ranges_.begin(); itr != ranges_.end(); ++itr) {
        if (*itr == selected_range_)
            break;
    }
    DCHECK(itr != ranges_.end());
    return itr;
}

void SourceBufferStream::SeekAndSetSelectedRange(
    SourceBufferRange* range, DecodeTimestamp seek_timestamp)
{
    if (range)
        range->Seek(seek_timestamp);
    SetSelectedRange(range);
}

void SourceBufferStream::SetSelectedRange(SourceBufferRange* range)
{
    DVLOG(1) << __FUNCTION__ << " " << GetStreamTypeName() << ": "
             << selected_range_ << " "
             << (selected_range_ ? RangeToString(*selected_range_) : "")
             << " -> " << range << " " << (range ? RangeToString(*range) : "");
    if (selected_range_)
        selected_range_->ResetNextBufferPosition();
    DCHECK(!range || range->HasNextBufferPosition());
    selected_range_ = range;
}

Ranges<base::TimeDelta> SourceBufferStream::GetBufferedTime() const
{
    Ranges<base::TimeDelta> ranges;
    for (RangeList::const_iterator itr = ranges_.begin();
         itr != ranges_.end(); ++itr) {
        ranges.Add((*itr)->GetStartTimestamp().ToPresentationTime(),
            (*itr)->GetBufferedEndTimestamp().ToPresentationTime());
    }
    return ranges;
}

base::TimeDelta SourceBufferStream::GetBufferedDuration() const
{
    if (ranges_.empty())
        return base::TimeDelta();

    return ranges_.back()->GetBufferedEndTimestamp().ToPresentationTime();
}

size_t SourceBufferStream::GetBufferedSize() const
{
    size_t ranges_size = 0;
    for (const auto& range : ranges_)
        ranges_size += range->size_in_bytes();
    return ranges_size;
}

void SourceBufferStream::MarkEndOfStream()
{
    DCHECK(!end_of_stream_);
    end_of_stream_ = true;
}

void SourceBufferStream::UnmarkEndOfStream()
{
    DCHECK(end_of_stream_);
    end_of_stream_ = false;
}

bool SourceBufferStream::IsEndOfStreamReached() const
{
    if (!end_of_stream_ || !track_buffer_.empty())
        return false;

    if (ranges_.empty())
        return true;

    if (seek_pending_) {
        base::TimeDelta last_range_end_time = ranges_.back()->GetBufferedEndTimestamp().ToPresentationTime();
        return seek_buffer_timestamp_ >= last_range_end_time;
    }

    if (!selected_range_)
        return true;

    return selected_range_ == ranges_.back();
}

const AudioDecoderConfig& SourceBufferStream::GetCurrentAudioDecoderConfig()
{
    if (config_change_pending_)
        CompleteConfigChange();
    return audio_configs_[current_config_index_];
}

const VideoDecoderConfig& SourceBufferStream::GetCurrentVideoDecoderConfig()
{
    if (config_change_pending_)
        CompleteConfigChange();
    return video_configs_[current_config_index_];
}

const TextTrackConfig& SourceBufferStream::GetCurrentTextTrackConfig()
{
    return text_track_config_;
}

base::TimeDelta SourceBufferStream::GetMaxInterbufferDistance() const
{
    if (max_interbuffer_distance_ == kNoTimestamp())
        return base::TimeDelta::FromMilliseconds(kDefaultBufferDurationInMs);
    return max_interbuffer_distance_;
}

bool SourceBufferStream::UpdateAudioConfig(const AudioDecoderConfig& config)
{
    DCHECK(!audio_configs_.empty());
    DCHECK(video_configs_.empty());
    DVLOG(3) << "UpdateAudioConfig.";

    if (audio_configs_[0].codec() != config.codec()) {
        MEDIA_LOG(ERROR, media_log_) << "Audio codec changes not allowed.";
        return false;
    }

    if (audio_configs_[0].is_encrypted() != config.is_encrypted()) {
        MEDIA_LOG(ERROR, media_log_) << "Audio encryption changes not allowed.";
        return false;
    }

    // Check to see if the new config matches an existing one.
    for (size_t i = 0; i < audio_configs_.size(); ++i) {
        if (config.Matches(audio_configs_[i])) {
            append_config_index_ = i;
            return true;
        }
    }

    // No matches found so let's add this one to the list.
    append_config_index_ = audio_configs_.size();
    DVLOG(2) << "New audio config - index: " << append_config_index_;
    audio_configs_.resize(audio_configs_.size() + 1);
    audio_configs_[append_config_index_] = config;
    return true;
}

bool SourceBufferStream::UpdateVideoConfig(const VideoDecoderConfig& config)
{
    DCHECK(!video_configs_.empty());
    DCHECK(audio_configs_.empty());
    DVLOG(3) << "UpdateVideoConfig.";

    if (video_configs_[0].codec() != config.codec()) {
        MEDIA_LOG(ERROR, media_log_) << "Video codec changes not allowed.";
        return false;
    }

    if (video_configs_[0].is_encrypted() != config.is_encrypted()) {
        MEDIA_LOG(ERROR, media_log_) << "Video encryption changes not allowed.";
        return false;
    }

    // Check to see if the new config matches an existing one.
    for (size_t i = 0; i < video_configs_.size(); ++i) {
        if (config.Matches(video_configs_[i])) {
            append_config_index_ = i;
            return true;
        }
    }

    // No matches found so let's add this one to the list.
    append_config_index_ = video_configs_.size();
    DVLOG(2) << "New video config - index: " << append_config_index_;
    video_configs_.resize(video_configs_.size() + 1);
    video_configs_[append_config_index_] = config;
    return true;
}

void SourceBufferStream::CompleteConfigChange()
{
    config_change_pending_ = false;

    if (pending_buffer_.get()) {
        current_config_index_ = pending_buffer_->GetSpliceBufferConfigId(splice_buffers_index_);
        return;
    }

    if (!track_buffer_.empty()) {
        current_config_index_ = track_buffer_.front()->GetSpliceBufferConfigId(0);
        return;
    }

    if (selected_range_ && selected_range_->HasNextBuffer())
        current_config_index_ = selected_range_->GetNextConfigId();
}

void SourceBufferStream::SetSelectedRangeIfNeeded(
    const DecodeTimestamp timestamp)
{
    DVLOG(2) << __FUNCTION__ << " " << GetStreamTypeName()
             << "(" << timestamp.InSecondsF() << ")";

    if (selected_range_) {
        DCHECK(track_buffer_.empty());
        return;
    }

    if (!track_buffer_.empty()) {
        DCHECK(!selected_range_);
        return;
    }

    DecodeTimestamp start_timestamp = timestamp;

    // If the next buffer timestamp is not known then use a timestamp just after
    // the timestamp on the last buffer returned by GetNextBuffer().
    if (start_timestamp == kNoDecodeTimestamp()) {
        if (last_output_buffer_timestamp_ == kNoDecodeTimestamp()) {
            DVLOG(2) << __FUNCTION__ << " " << GetStreamTypeName()
                     << " no previous output timestamp";
            return;
        }

        start_timestamp = last_output_buffer_timestamp_ + base::TimeDelta::FromInternalValue(1);
    }

    DecodeTimestamp seek_timestamp = FindNewSelectedRangeSeekTimestamp(start_timestamp);

    // If we don't have buffered data to seek to, then return.
    if (seek_timestamp == kNoDecodeTimestamp()) {
        DVLOG(2) << __FUNCTION__ << " " << GetStreamTypeName()
                 << " couldn't find new selected range seek timestamp";
        return;
    }

    DCHECK(track_buffer_.empty());
    SeekAndSetSelectedRange(*FindExistingRangeFor(seek_timestamp),
        seek_timestamp);
}

DecodeTimestamp SourceBufferStream::FindNewSelectedRangeSeekTimestamp(
    const DecodeTimestamp start_timestamp)
{
    DCHECK(start_timestamp != kNoDecodeTimestamp());
    DCHECK(start_timestamp >= DecodeTimestamp());

    RangeList::iterator itr = ranges_.begin();

    for (; itr != ranges_.end(); ++itr) {
        if ((*itr)->GetEndTimestamp() >= start_timestamp) {
            break;
        }
    }

    if (itr == ranges_.end()) {
        DVLOG(2) << __FUNCTION__ << " " << GetStreamTypeName()
                 << " no buffered data for dts=" << start_timestamp.InSecondsF();
        return kNoDecodeTimestamp();
    }

    // First check for a keyframe timestamp >= |start_timestamp|
    // in the current range.
    DecodeTimestamp keyframe_timestamp = (*itr)->NextKeyframeTimestamp(start_timestamp);

    if (keyframe_timestamp != kNoDecodeTimestamp())
        return keyframe_timestamp;

    // If a keyframe was not found then look for a keyframe that is
    // "close enough" in the current or next range.
    DecodeTimestamp end_timestamp = start_timestamp + ComputeFudgeRoom(GetMaxInterbufferDistance());
    DCHECK(start_timestamp < end_timestamp);

    // Make sure the current range doesn't start beyond |end_timestamp|.
    if ((*itr)->GetStartTimestamp() >= end_timestamp)
        return kNoDecodeTimestamp();

    keyframe_timestamp = (*itr)->KeyframeBeforeTimestamp(end_timestamp);

    // Check to see if the keyframe is within the acceptable range
    // (|start_timestamp|, |end_timestamp|].
    if (keyframe_timestamp != kNoDecodeTimestamp() && start_timestamp < keyframe_timestamp && keyframe_timestamp <= end_timestamp) {
        return keyframe_timestamp;
    }

    // If |end_timestamp| is within this range, then no other checks are
    // necessary.
    if (end_timestamp <= (*itr)->GetEndTimestamp())
        return kNoDecodeTimestamp();

    // Move on to the next range.
    ++itr;

    // Return early if the next range does not contain |end_timestamp|.
    if (itr == ranges_.end() || (*itr)->GetStartTimestamp() >= end_timestamp)
        return kNoDecodeTimestamp();

    keyframe_timestamp = (*itr)->KeyframeBeforeTimestamp(end_timestamp);

    // Check to see if the keyframe is within the acceptable range
    // (|start_timestamp|, |end_timestamp|].
    if (keyframe_timestamp != kNoDecodeTimestamp() && start_timestamp < keyframe_timestamp && keyframe_timestamp <= end_timestamp) {
        return keyframe_timestamp;
    }

    return kNoDecodeTimestamp();
}

DecodeTimestamp SourceBufferStream::FindKeyframeAfterTimestamp(
    const DecodeTimestamp timestamp)
{
    DCHECK(timestamp != kNoDecodeTimestamp());

    RangeList::iterator itr = FindExistingRangeFor(timestamp);

    if (itr == ranges_.end())
        return kNoDecodeTimestamp();

    // First check for a keyframe timestamp >= |timestamp|
    // in the current range.
    return (*itr)->NextKeyframeTimestamp(timestamp);
}

std::string SourceBufferStream::GetStreamTypeName() const
{
    switch (GetType()) {
    case kAudio:
        return "AUDIO";
    case kVideo:
        return "VIDEO";
    case kText:
        return "TEXT";
    }
    NOTREACHED();
    return "";
}

SourceBufferStream::Type SourceBufferStream::GetType() const
{
    if (!audio_configs_.empty())
        return kAudio;
    if (!video_configs_.empty())
        return kVideo;
    DCHECK_NE(text_track_config_.kind(), kTextNone);
    return kText;
}

void SourceBufferStream::DeleteAndRemoveRange(RangeList::iterator* itr)
{
    DVLOG(1) << __FUNCTION__;

    DCHECK(*itr != ranges_.end());
    if (**itr == selected_range_) {
        DVLOG(1) << __FUNCTION__ << " deleting selected range.";
        SetSelectedRange(NULL);
    }

    if (*itr == range_for_next_append_) {
        DVLOG(1) << __FUNCTION__ << " deleting range_for_next_append_.";
        range_for_next_append_ = ranges_.end();
        last_appended_buffer_timestamp_ = kNoDecodeTimestamp();
        last_appended_buffer_is_keyframe_ = false;
    }

    delete **itr;
    *itr = ranges_.erase(*itr);
}

void SourceBufferStream::GenerateSpliceFrame(const BufferQueue& new_buffers)
{
    DCHECK(!new_buffers.empty());

    // Splice frames are only supported for audio.
    if (GetType() != kAudio)
        return;

    // Find the overlapped range (if any).
    const base::TimeDelta splice_timestamp = new_buffers.front()->timestamp();
    const DecodeTimestamp splice_dts = DecodeTimestamp::FromPresentationTime(splice_timestamp);
    RangeList::iterator range_itr = FindExistingRangeFor(splice_dts);
    if (range_itr == ranges_.end())
        return;

    const DecodeTimestamp max_splice_end_dts = splice_dts + base::TimeDelta::FromMilliseconds(AudioSplicer::kCrossfadeDurationInMilliseconds);

    // Find all buffers involved before the splice point.
    BufferQueue pre_splice_buffers;
    if (!(*range_itr)->GetBuffersInRange(splice_dts, max_splice_end_dts, &pre_splice_buffers)) {
        return;
    }

    // If there are gaps in the timeline, it's possible that we only find buffers
    // after the splice point but within the splice range.  For simplicity, we do
    // not generate splice frames in this case.
    //
    // We also do not want to generate splices if the first new buffer replaces an
    // existing buffer exactly.
    if (pre_splice_buffers.front()->timestamp() >= splice_timestamp) {
        LIMITED_MEDIA_LOG(DEBUG, media_log_, num_splice_generation_warning_logs_,
            kMaxSpliceGenerationWarningLogs)
            << "Skipping splice frame generation: first new buffer at "
            << splice_timestamp.InMicroseconds()
            << "us begins at or before existing buffer at "
            << pre_splice_buffers.front()->timestamp().InMicroseconds() << "us.";
        DVLOG(1) << "Skipping splice: overlapped buffers begin at or after the "
                    "first new buffer.";
        return;
    }

    // If any |pre_splice_buffers| are already splices or preroll, do not generate
    // a splice.
    for (size_t i = 0; i < pre_splice_buffers.size(); ++i) {
        const BufferQueue& original_splice_buffers = pre_splice_buffers[i]->splice_buffers();
        if (!original_splice_buffers.empty()) {
            LIMITED_MEDIA_LOG(DEBUG, media_log_, num_splice_generation_warning_logs_,
                kMaxSpliceGenerationWarningLogs)
                << "Skipping splice frame generation: overlapped buffers at "
                << pre_splice_buffers[i]->timestamp().InMicroseconds()
                << "us are in a previously buffered splice.";
            DVLOG(1) << "Can't generate splice: overlapped buffers contain a "
                        "pre-existing splice.";
            return;
        }

        if (pre_splice_buffers[i]->preroll_buffer().get()) {
            LIMITED_MEDIA_LOG(DEBUG, media_log_, num_splice_generation_warning_logs_,
                kMaxSpliceGenerationWarningLogs)
                << "Skipping splice frame generation: overlapped buffers at "
                << pre_splice_buffers[i]->timestamp().InMicroseconds()
                << "us contain preroll.";
            DVLOG(1) << "Can't generate splice: overlapped buffers contain preroll.";
            return;
        }
    }

    // Don't generate splice frames which represent less than a millisecond (which
    // is frequently the extent of timestamp resolution for poorly encoded media)
    // or less than two samples (need at least two to crossfade).
    const base::TimeDelta splice_duration = pre_splice_buffers.back()->timestamp() + pre_splice_buffers.back()->duration() - splice_timestamp;
    const base::TimeDelta minimum_splice_duration = std::max(
        base::TimeDelta::FromMilliseconds(1),
        base::TimeDelta::FromSecondsD(
            2.0 / audio_configs_[append_config_index_].samples_per_second()));
    if (splice_duration < minimum_splice_duration) {
        LIMITED_MEDIA_LOG(DEBUG, media_log_, num_splice_generation_warning_logs_,
            kMaxSpliceGenerationWarningLogs)
            << "Skipping splice frame generation: not enough samples for splicing "
               "new buffer at "
            << splice_timestamp.InMicroseconds() << "us. Have "
            << splice_duration.InMicroseconds() << "us, but need "
            << minimum_splice_duration.InMicroseconds() << "us.";
        DVLOG(1) << "Can't generate splice: not enough samples for crossfade; have "
                 << splice_duration.InMicroseconds() << "us, but need "
                 << minimum_splice_duration.InMicroseconds() << "us.";
        return;
    }

    DVLOG(1) << "Generating splice frame @ " << new_buffers.front()->timestamp()
             << ", splice duration: " << splice_duration.InMicroseconds()
             << " us";
    LIMITED_MEDIA_LOG(DEBUG, media_log_, num_splice_generation_success_logs_,
        kMaxSpliceGenerationSuccessLogs)
        << "Generated splice of overlap duration "
        << splice_duration.InMicroseconds() << "us into new buffer at "
        << splice_timestamp.InMicroseconds() << "us.";
    new_buffers.front()->ConvertToSpliceBuffer(pre_splice_buffers);
}

bool SourceBufferStream::SetPendingBuffer(
    scoped_refptr<StreamParserBuffer>* out_buffer)
{
    DCHECK(out_buffer->get());
    DCHECK(!pending_buffer_.get());

    const bool have_splice_buffers = !(*out_buffer)->splice_buffers().empty();
    const bool have_preroll_buffer = !!(*out_buffer)->preroll_buffer().get();

    if (!have_splice_buffers && !have_preroll_buffer)
        return false;

    DCHECK_NE(have_splice_buffers, have_preroll_buffer);
    splice_buffers_index_ = 0;
    pending_buffer_.swap(*out_buffer);
    pending_buffers_complete_ = false;
    return true;
}

} // namespace media

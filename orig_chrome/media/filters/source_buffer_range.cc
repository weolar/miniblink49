// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/source_buffer_range.h"

#include <algorithm>

#include "media/base/timestamp_constants.h"

namespace media {

// Comparison operators for std::upper_bound() and std::lower_bound().
static bool CompareTimeDeltaToStreamParserBuffer(
    const DecodeTimestamp& decode_timestamp,
    const scoped_refptr<StreamParserBuffer>& buffer)
{
    return decode_timestamp < buffer->GetDecodeTimestamp();
}
static bool CompareStreamParserBufferToTimeDelta(
    const scoped_refptr<StreamParserBuffer>& buffer,
    const DecodeTimestamp& decode_timestamp)
{
    return buffer->GetDecodeTimestamp() < decode_timestamp;
}

bool SourceBufferRange::AllowSameTimestamp(
    bool prev_is_keyframe, bool current_is_keyframe)
{
    return prev_is_keyframe || !current_is_keyframe;
}

SourceBufferRange::SourceBufferRange(
    GapPolicy gap_policy, const BufferQueue& new_buffers,
    DecodeTimestamp media_segment_start_time,
    const InterbufferDistanceCB& interbuffer_distance_cb)
    : gap_policy_(gap_policy)
    , keyframe_map_index_base_(0)
    , next_buffer_index_(-1)
    , media_segment_start_time_(media_segment_start_time)
    , interbuffer_distance_cb_(interbuffer_distance_cb)
    , size_in_bytes_(0)
{
    CHECK(!new_buffers.empty());
    DCHECK(new_buffers.front()->is_key_frame());
    DCHECK(!interbuffer_distance_cb.is_null());
    AppendBuffersToEnd(new_buffers);
}

SourceBufferRange::~SourceBufferRange() { }

void SourceBufferRange::AppendBuffersToEnd(const BufferQueue& new_buffers)
{
    DCHECK(buffers_.empty() || CanAppendBuffersToEnd(new_buffers));
    DCHECK(media_segment_start_time_ == kNoDecodeTimestamp() || media_segment_start_time_ <= new_buffers.front()->GetDecodeTimestamp());

    AdjustEstimatedDurationForNewAppend(new_buffers);

    for (BufferQueue::const_iterator itr = new_buffers.begin();
         itr != new_buffers.end();
         ++itr) {
        DCHECK((*itr)->GetDecodeTimestamp() != kNoDecodeTimestamp());
        buffers_.push_back(*itr);
        DCHECK_GE((*itr)->data_size(), 0);
        size_in_bytes_ += (*itr)->data_size();

        if ((*itr)->is_key_frame()) {
            keyframe_map_.insert(
                std::make_pair((*itr)->GetDecodeTimestamp(),
                    buffers_.size() - 1 + keyframe_map_index_base_));
        }
    }
}

void SourceBufferRange::AdjustEstimatedDurationForNewAppend(
    const BufferQueue& new_buffers)
{
    if (buffers_.empty() || new_buffers.empty()) {
        return;
    }

    // If the last of the previously appended buffers contains estimated duration,
    // we now refine that estimate by taking the PTS delta from the first new
    // buffer being appended.
    const auto& last_appended_buffer = buffers_.back();
    if (last_appended_buffer->is_duration_estimated()) {
        base::TimeDelta timestamp_delta = new_buffers.front()->timestamp() - last_appended_buffer->timestamp();
        DCHECK(timestamp_delta > base::TimeDelta());
        if (last_appended_buffer->duration() != timestamp_delta) {
            DVLOG(1) << "Replacing estimated duration ("
                     << last_appended_buffer->duration()
                     << ") from previous range-end with derived duration ("
                     << timestamp_delta << ").";
            last_appended_buffer->set_duration(timestamp_delta);
        }
    }
}

void SourceBufferRange::Seek(DecodeTimestamp timestamp)
{
    DCHECK(CanSeekTo(timestamp));
    DCHECK(!keyframe_map_.empty());

    KeyframeMap::iterator result = GetFirstKeyframeAtOrBefore(timestamp);
    next_buffer_index_ = result->second - keyframe_map_index_base_;
    DCHECK_LT(next_buffer_index_, static_cast<int>(buffers_.size()));
}

void SourceBufferRange::SeekAheadTo(DecodeTimestamp timestamp)
{
    SeekAhead(timestamp, false);
}

void SourceBufferRange::SeekAheadPast(DecodeTimestamp timestamp)
{
    SeekAhead(timestamp, true);
}

void SourceBufferRange::SeekAhead(DecodeTimestamp timestamp,
    bool skip_given_timestamp)
{
    DCHECK(!keyframe_map_.empty());

    KeyframeMap::iterator result = GetFirstKeyframeAt(timestamp, skip_given_timestamp);

    // If there isn't a keyframe after |timestamp|, then seek to end and return
    // kNoTimestamp to signal such.
    if (result == keyframe_map_.end()) {
        next_buffer_index_ = -1;
        return;
    }
    next_buffer_index_ = result->second - keyframe_map_index_base_;
    DCHECK_LT(next_buffer_index_, static_cast<int>(buffers_.size()));
}

void SourceBufferRange::SeekToStart()
{
    DCHECK(!buffers_.empty());
    next_buffer_index_ = 0;
}

SourceBufferRange* SourceBufferRange::SplitRange(DecodeTimestamp timestamp)
{
    CHECK(!buffers_.empty());

    // Find the first keyframe at or after |timestamp|.
    KeyframeMap::iterator new_beginning_keyframe = GetFirstKeyframeAt(timestamp, false);

    // If there is no keyframe after |timestamp|, we can't split the range.
    if (new_beginning_keyframe == keyframe_map_.end())
        return NULL;

    // Remove the data beginning at |keyframe_index| from |buffers_| and save it
    // into |removed_buffers|.
    int keyframe_index = new_beginning_keyframe->second - keyframe_map_index_base_;
    DCHECK_LT(keyframe_index, static_cast<int>(buffers_.size()));
    BufferQueue::iterator starting_point = buffers_.begin() + keyframe_index;
    BufferQueue removed_buffers(starting_point, buffers_.end());

    DecodeTimestamp new_range_start_timestamp = kNoDecodeTimestamp();
    if (GetStartTimestamp() < buffers_.front()->GetDecodeTimestamp() && timestamp < removed_buffers.front()->GetDecodeTimestamp()) {
        // The split is in the gap between |media_segment_start_time_| and
        // the first buffer of the new range so we should set the start
        // time of the new range to |timestamp| so we preserve part of the
        // gap in the new range.
        new_range_start_timestamp = timestamp;
    }

    keyframe_map_.erase(new_beginning_keyframe, keyframe_map_.end());
    FreeBufferRange(starting_point, buffers_.end());

    // Create a new range with |removed_buffers|.
    SourceBufferRange* split_range = new SourceBufferRange(
        gap_policy_, removed_buffers, new_range_start_timestamp,
        interbuffer_distance_cb_);

    // If the next buffer position is now in |split_range|, update the state of
    // this range and |split_range| accordingly.
    if (next_buffer_index_ >= static_cast<int>(buffers_.size())) {
        split_range->next_buffer_index_ = next_buffer_index_ - keyframe_index;
        ResetNextBufferPosition();
    }

    return split_range;
}

SourceBufferRange::BufferQueue::iterator SourceBufferRange::GetBufferItrAt(
    DecodeTimestamp timestamp,
    bool skip_given_timestamp)
{
    return skip_given_timestamp
        ? std::upper_bound(buffers_.begin(),
            buffers_.end(),
            timestamp,
            CompareTimeDeltaToStreamParserBuffer)
        : std::lower_bound(buffers_.begin(),
            buffers_.end(),
            timestamp,
            CompareStreamParserBufferToTimeDelta);
}

SourceBufferRange::KeyframeMap::iterator
SourceBufferRange::GetFirstKeyframeAt(DecodeTimestamp timestamp,
    bool skip_given_timestamp)
{
    return skip_given_timestamp ? keyframe_map_.upper_bound(timestamp) : keyframe_map_.lower_bound(timestamp);
}

SourceBufferRange::KeyframeMap::iterator
SourceBufferRange::GetFirstKeyframeAtOrBefore(DecodeTimestamp timestamp)
{
    KeyframeMap::iterator result = keyframe_map_.lower_bound(timestamp);
    // lower_bound() returns the first element >= |timestamp|, so we want the
    // previous element if it did not return the element exactly equal to
    // |timestamp|.
    if (result != keyframe_map_.begin() && (result == keyframe_map_.end() || result->first != timestamp)) {
        --result;
    }
    return result;
}

void SourceBufferRange::DeleteAll(BufferQueue* removed_buffers)
{
    TruncateAt(buffers_.begin(), removed_buffers);
}

bool SourceBufferRange::TruncateAt(
    DecodeTimestamp timestamp, BufferQueue* removed_buffers,
    bool is_exclusive)
{
    // Find the place in |buffers_| where we will begin deleting data.
    BufferQueue::iterator starting_point = GetBufferItrAt(timestamp, is_exclusive);
    return TruncateAt(starting_point, removed_buffers);
}

size_t SourceBufferRange::DeleteGOPFromFront(BufferQueue* deleted_buffers)
{
    DCHECK(!FirstGOPContainsNextBufferPosition());
    DCHECK(deleted_buffers);

    int buffers_deleted = 0;
    size_t total_bytes_deleted = 0;

    KeyframeMap::iterator front = keyframe_map_.begin();
    DCHECK(front != keyframe_map_.end());

    // Delete the keyframe at the start of |keyframe_map_|.
    keyframe_map_.erase(front);

    // Now we need to delete all the buffers that depend on the keyframe we've
    // just deleted.
    int end_index = keyframe_map_.size() > 0 ? keyframe_map_.begin()->second - keyframe_map_index_base_ : buffers_.size();

    // Delete buffers from the beginning of the buffered range up until (but not
    // including) the next keyframe.
    for (int i = 0; i < end_index; i++) {
        DCHECK_GE(buffers_.front()->data_size(), 0);
        size_t bytes_deleted = buffers_.front()->data_size();
        DCHECK_GE(size_in_bytes_, bytes_deleted);
        size_in_bytes_ -= bytes_deleted;
        total_bytes_deleted += bytes_deleted;
        deleted_buffers->push_back(buffers_.front());
        buffers_.pop_front();
        ++buffers_deleted;
    }

    // Update |keyframe_map_index_base_| to account for the deleted buffers.
    keyframe_map_index_base_ += buffers_deleted;

    if (next_buffer_index_ > -1) {
        next_buffer_index_ -= buffers_deleted;
        DCHECK_GE(next_buffer_index_, 0);
    }

    // Invalidate media segment start time if we've deleted the first buffer of
    // the range.
    if (buffers_deleted > 0)
        media_segment_start_time_ = kNoDecodeTimestamp();

    return total_bytes_deleted;
}

size_t SourceBufferRange::DeleteGOPFromBack(BufferQueue* deleted_buffers)
{
    DCHECK(!LastGOPContainsNextBufferPosition());
    DCHECK(deleted_buffers);

    // Remove the last GOP's keyframe from the |keyframe_map_|.
    KeyframeMap::iterator back = keyframe_map_.end();
    DCHECK_GT(keyframe_map_.size(), 0u);
    --back;

    // The index of the first buffer in the last GOP is equal to the new size of
    // |buffers_| after that GOP is deleted.
    size_t goal_size = back->second - keyframe_map_index_base_;
    keyframe_map_.erase(back);

    size_t total_bytes_deleted = 0;
    while (buffers_.size() != goal_size) {
        DCHECK_GE(buffers_.back()->data_size(), 0);
        size_t bytes_deleted = buffers_.back()->data_size();
        DCHECK_GE(size_in_bytes_, bytes_deleted);
        size_in_bytes_ -= bytes_deleted;
        total_bytes_deleted += bytes_deleted;
        // We're removing buffers from the back, so push each removed buffer to the
        // front of |deleted_buffers| so that |deleted_buffers| are in nondecreasing
        // order.
        deleted_buffers->push_front(buffers_.back());
        buffers_.pop_back();
    }

    return total_bytes_deleted;
}

size_t SourceBufferRange::GetRemovalGOP(
    DecodeTimestamp start_timestamp, DecodeTimestamp end_timestamp,
    size_t total_bytes_to_free, DecodeTimestamp* removal_end_timestamp)
{
    size_t bytes_removed = 0;

    KeyframeMap::iterator gop_itr = GetFirstKeyframeAt(start_timestamp, false);
    if (gop_itr == keyframe_map_.end())
        return 0;
    int keyframe_index = gop_itr->second - keyframe_map_index_base_;
    BufferQueue::iterator buffer_itr = buffers_.begin() + keyframe_index;
    KeyframeMap::iterator gop_end = keyframe_map_.end();
    if (end_timestamp < GetBufferedEndTimestamp())
        gop_end = GetFirstKeyframeAtOrBefore(end_timestamp);

    // Check if the removal range is within a GOP and skip the loop if so.
    // [keyframe]...[start_timestamp]...[end_timestamp]...[keyframe]
    KeyframeMap::iterator gop_itr_prev = gop_itr;
    if (gop_itr_prev != keyframe_map_.begin() && --gop_itr_prev == gop_end)
        gop_end = gop_itr;

    while (gop_itr != gop_end && bytes_removed < total_bytes_to_free) {
        ++gop_itr;

        size_t gop_size = 0;
        int next_gop_index = gop_itr == keyframe_map_.end() ? buffers_.size() : gop_itr->second - keyframe_map_index_base_;
        BufferQueue::iterator next_gop_start = buffers_.begin() + next_gop_index;
        for (; buffer_itr != next_gop_start; ++buffer_itr) {
            DCHECK_GE((*buffer_itr)->data_size(), 0);
            gop_size += (*buffer_itr)->data_size();
        }

        bytes_removed += gop_size;
    }
    if (bytes_removed > 0) {
        *removal_end_timestamp = gop_itr == keyframe_map_.end() ? GetBufferedEndTimestamp() : gop_itr->first;
    }
    return bytes_removed;
}

bool SourceBufferRange::FirstGOPEarlierThanMediaTime(
    DecodeTimestamp media_time) const
{
    if (keyframe_map_.size() == 1u)
        return (GetEndTimestamp() < media_time);

    KeyframeMap::const_iterator second_gop = keyframe_map_.begin();
    ++second_gop;
    return second_gop->first <= media_time;
}

bool SourceBufferRange::FirstGOPContainsNextBufferPosition() const
{
    if (!HasNextBufferPosition())
        return false;

    // If there is only one GOP, it must contain the next buffer position.
    if (keyframe_map_.size() == 1u)
        return true;

    KeyframeMap::const_iterator second_gop = keyframe_map_.begin();
    ++second_gop;
    return next_buffer_index_ < second_gop->second - keyframe_map_index_base_;
}

bool SourceBufferRange::LastGOPContainsNextBufferPosition() const
{
    if (!HasNextBufferPosition())
        return false;

    // If there is only one GOP, it must contain the next buffer position.
    if (keyframe_map_.size() == 1u)
        return true;

    KeyframeMap::const_iterator last_gop = keyframe_map_.end();
    --last_gop;
    return last_gop->second - keyframe_map_index_base_ <= next_buffer_index_;
}

void SourceBufferRange::FreeBufferRange(
    const BufferQueue::iterator& starting_point,
    const BufferQueue::iterator& ending_point)
{
    for (BufferQueue::iterator itr = starting_point;
         itr != ending_point; ++itr) {
        DCHECK_GE((*itr)->data_size(), 0);
        size_t itr_data_size = static_cast<size_t>((*itr)->data_size());
        DCHECK_GE(size_in_bytes_, itr_data_size);
        size_in_bytes_ -= itr_data_size;
    }
    buffers_.erase(starting_point, ending_point);
}

bool SourceBufferRange::TruncateAt(
    const BufferQueue::iterator& starting_point, BufferQueue* removed_buffers)
{
    DCHECK(!removed_buffers || removed_buffers->empty());

    // Return if we're not deleting anything.
    if (starting_point == buffers_.end())
        return buffers_.empty();

    // Reset the next buffer index if we will be deleting the buffer that's next
    // in sequence.
    if (HasNextBufferPosition()) {
        DecodeTimestamp next_buffer_timestamp = GetNextTimestamp();
        if (next_buffer_timestamp == kNoDecodeTimestamp() || next_buffer_timestamp >= (*starting_point)->GetDecodeTimestamp()) {
            if (HasNextBuffer() && removed_buffers) {
                int starting_offset = starting_point - buffers_.begin();
                int next_buffer_offset = next_buffer_index_ - starting_offset;
                DCHECK_GE(next_buffer_offset, 0);
                BufferQueue saved(starting_point + next_buffer_offset, buffers_.end());
                removed_buffers->swap(saved);
            }
            ResetNextBufferPosition();
        }
    }

    // Remove keyframes from |starting_point| onward.
    KeyframeMap::iterator starting_point_keyframe = keyframe_map_.lower_bound((*starting_point)->GetDecodeTimestamp());
    keyframe_map_.erase(starting_point_keyframe, keyframe_map_.end());

    // Remove everything from |starting_point| onward.
    FreeBufferRange(starting_point, buffers_.end());
    return buffers_.empty();
}

bool SourceBufferRange::GetNextBuffer(
    scoped_refptr<StreamParserBuffer>* out_buffer)
{
    if (!HasNextBuffer())
        return false;

    *out_buffer = buffers_[next_buffer_index_];
    next_buffer_index_++;
    return true;
}

bool SourceBufferRange::HasNextBuffer() const
{
    return next_buffer_index_ >= 0 && next_buffer_index_ < static_cast<int>(buffers_.size());
}

int SourceBufferRange::GetNextConfigId() const
{
    DCHECK(HasNextBuffer());
    // If the next buffer is an audio splice frame, the next effective config id
    // comes from the first fade out preroll buffer.
    return buffers_[next_buffer_index_]->GetSpliceBufferConfigId(0);
}

DecodeTimestamp SourceBufferRange::GetNextTimestamp() const
{
    DCHECK(!buffers_.empty());
    DCHECK(HasNextBufferPosition());

    if (next_buffer_index_ >= static_cast<int>(buffers_.size())) {
        return kNoDecodeTimestamp();
    }

    return buffers_[next_buffer_index_]->GetDecodeTimestamp();
}

bool SourceBufferRange::HasNextBufferPosition() const
{
    return next_buffer_index_ >= 0;
}

void SourceBufferRange::ResetNextBufferPosition()
{
    next_buffer_index_ = -1;
}

void SourceBufferRange::AppendRangeToEnd(const SourceBufferRange& range,
    bool transfer_current_position)
{
    DCHECK(CanAppendRangeToEnd(range));
    DCHECK(!buffers_.empty());

    if (transfer_current_position && range.next_buffer_index_ >= 0)
        next_buffer_index_ = range.next_buffer_index_ + buffers_.size();

    AppendBuffersToEnd(range.buffers_);
}

bool SourceBufferRange::CanAppendRangeToEnd(
    const SourceBufferRange& range) const
{
    return CanAppendBuffersToEnd(range.buffers_);
}

bool SourceBufferRange::CanAppendBuffersToEnd(
    const BufferQueue& buffers) const
{
    DCHECK(!buffers_.empty());
    return IsNextInSequence(buffers.front()->GetDecodeTimestamp(),
        buffers.front()->is_key_frame());
}

bool SourceBufferRange::BelongsToRange(DecodeTimestamp timestamp) const
{
    DCHECK(!buffers_.empty());

    return (IsNextInSequence(timestamp, false) || (GetStartTimestamp() <= timestamp && timestamp <= GetEndTimestamp()));
}

bool SourceBufferRange::CanSeekTo(DecodeTimestamp timestamp) const
{
    DecodeTimestamp start_timestamp = std::max(DecodeTimestamp(), GetStartTimestamp() - GetFudgeRoom());
    return !keyframe_map_.empty() && start_timestamp <= timestamp && timestamp < GetBufferedEndTimestamp();
}

bool SourceBufferRange::CompletelyOverlaps(
    const SourceBufferRange& range) const
{
    return GetStartTimestamp() <= range.GetStartTimestamp() && GetEndTimestamp() >= range.GetEndTimestamp();
}

bool SourceBufferRange::EndOverlaps(const SourceBufferRange& range) const
{
    return range.GetStartTimestamp() <= GetEndTimestamp() && GetEndTimestamp() < range.GetEndTimestamp();
}

DecodeTimestamp SourceBufferRange::GetStartTimestamp() const
{
    DCHECK(!buffers_.empty());
    DecodeTimestamp start_timestamp = media_segment_start_time_;
    if (start_timestamp == kNoDecodeTimestamp())
        start_timestamp = buffers_.front()->GetDecodeTimestamp();
    return start_timestamp;
}

DecodeTimestamp SourceBufferRange::GetEndTimestamp() const
{
    DCHECK(!buffers_.empty());
    return buffers_.back()->GetDecodeTimestamp();
}

DecodeTimestamp SourceBufferRange::GetBufferedEndTimestamp() const
{
    DCHECK(!buffers_.empty());
    base::TimeDelta duration = buffers_.back()->duration();
    if (duration == kNoTimestamp() || duration == base::TimeDelta())
        duration = GetApproximateDuration();
    return GetEndTimestamp() + duration;
}

DecodeTimestamp SourceBufferRange::NextKeyframeTimestamp(
    DecodeTimestamp timestamp)
{
    DCHECK(!keyframe_map_.empty());

    if (timestamp < GetStartTimestamp() || timestamp >= GetBufferedEndTimestamp())
        return kNoDecodeTimestamp();

    KeyframeMap::iterator itr = GetFirstKeyframeAt(timestamp, false);
    if (itr == keyframe_map_.end())
        return kNoDecodeTimestamp();

    // If the timestamp is inside the gap between the start of the media
    // segment and the first buffer, then just pretend there is a
    // keyframe at the specified timestamp.
    if (itr == keyframe_map_.begin() && timestamp > media_segment_start_time_ && timestamp < itr->first) {
        return timestamp;
    }

    return itr->first;
}

DecodeTimestamp SourceBufferRange::KeyframeBeforeTimestamp(
    DecodeTimestamp timestamp)
{
    DCHECK(!keyframe_map_.empty());

    if (timestamp < GetStartTimestamp() || timestamp >= GetBufferedEndTimestamp())
        return kNoDecodeTimestamp();

    return GetFirstKeyframeAtOrBefore(timestamp)->first;
}

bool SourceBufferRange::IsNextInSequence(
    DecodeTimestamp timestamp, bool is_key_frame) const
{
    DecodeTimestamp end = buffers_.back()->GetDecodeTimestamp();
    if (end < timestamp && (gap_policy_ == ALLOW_GAPS || timestamp <= end + GetFudgeRoom())) {
        return true;
    }

    return timestamp == end && AllowSameTimestamp(buffers_.back()->is_key_frame(), is_key_frame);
}

base::TimeDelta SourceBufferRange::GetFudgeRoom() const
{
    // Because we do not know exactly when is the next timestamp, any buffer
    // that starts within 2x the approximate duration of a buffer is considered
    // within this range.
    return 2 * GetApproximateDuration();
}

base::TimeDelta SourceBufferRange::GetApproximateDuration() const
{
    base::TimeDelta max_interbuffer_distance = interbuffer_distance_cb_.Run();
    DCHECK(max_interbuffer_distance != kNoTimestamp());
    return max_interbuffer_distance;
}

bool SourceBufferRange::GetBuffersInRange(DecodeTimestamp start,
    DecodeTimestamp end,
    BufferQueue* buffers)
{
    // Find the nearest buffer with a decode timestamp <= start.
    const DecodeTimestamp first_timestamp = KeyframeBeforeTimestamp(start);
    if (first_timestamp == kNoDecodeTimestamp())
        return false;

    // Find all buffers involved in the range.
    const size_t previous_size = buffers->size();
    for (BufferQueue::iterator it = GetBufferItrAt(first_timestamp, false);
         it != buffers_.end();
         ++it) {
        const scoped_refptr<StreamParserBuffer>& buffer = *it;
        // Buffers without duration are not supported, so bail if we encounter any.
        if (buffer->duration() == kNoTimestamp() || buffer->duration() <= base::TimeDelta()) {
            return false;
        }
        if (buffer->end_of_stream() || buffer->timestamp() >= end.ToPresentationTime()) {
            break;
        }

        if (buffer->timestamp() + buffer->duration() <= start.ToPresentationTime())
            continue;
        buffers->push_back(buffer);
    }
    return previous_size < buffers->size();
}

} // namespace media

// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FILTERS_SOURCE_BUFFER_RANGE_H_
#define MEDIA_FILTERS_SOURCE_BUFFER_RANGE_H_

#include <map>

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "media/base/stream_parser_buffer.h"

namespace media {

// Helper class representing a range of buffered data. All buffers in a
// SourceBufferRange are ordered sequentially in decode timestamp order with no
// gaps.
class SourceBufferRange {
public:
    // Returns the maximum distance in time between any buffer seen in this
    // stream. Used to estimate the duration of a buffer if its duration is not
    // known.
    typedef base::Callback<base::TimeDelta()> InterbufferDistanceCB;

    typedef StreamParser::BufferQueue BufferQueue;

    // Policy for handling large gaps between buffers. Continuous media like
    // audio & video should use NO_GAPS_ALLOWED. Discontinuous media like
    // timed text should use ALLOW_GAPS because large differences in timestamps
    // are common and acceptable.
    enum GapPolicy {
        NO_GAPS_ALLOWED,
        ALLOW_GAPS
    };

    // Buffers with the same timestamp are only allowed under certain conditions.
    // More precisely, it is allowed in all situations except when the previous
    // frame is not a key frame and the current is a key frame.
    // Examples of situations where DTS of two consecutive frames can be equal:
    // - Video: VP8 Alt-Ref frames.
    // - Video: IPBPBP...: DTS for I frame and for P frame can be equal.
    // - Text track cues that start at same time.
    // Returns true if |prev_is_keyframe| and |current_is_keyframe| indicate a
    // same timestamp situation that is allowed. False is returned otherwise.
    static bool AllowSameTimestamp(bool prev_is_keyframe,
        bool current_is_keyframe);

    // Creates a source buffer range with |new_buffers|. |new_buffers| cannot be
    // empty and the front of |new_buffers| must be a keyframe.
    // |media_segment_start_time| refers to the starting timestamp for the media
    // segment to which these buffers belong.
    SourceBufferRange(GapPolicy gap_policy,
        const BufferQueue& new_buffers,
        DecodeTimestamp media_segment_start_time,
        const InterbufferDistanceCB& interbuffer_distance_cb);

    ~SourceBufferRange();

    // Appends |buffers| to the end of the range and updates |keyframe_map_| as
    // it encounters new keyframes. Assumes |buffers| belongs at the end of the
    // range.
    void AppendBuffersToEnd(const BufferQueue& buffers);
    bool CanAppendBuffersToEnd(const BufferQueue& buffers) const;

    // Appends the buffers from |range| into this range.
    // The first buffer in |range| must come directly after the last buffer
    // in this range.
    // If |transfer_current_position| is true, |range|'s |next_buffer_index_|
    // is transfered to this SourceBufferRange.
    void AppendRangeToEnd(const SourceBufferRange& range,
        bool transfer_current_position);
    bool CanAppendRangeToEnd(const SourceBufferRange& range) const;

    // Updates |next_buffer_index_| to point to the Buffer containing |timestamp|.
    // Assumes |timestamp| is valid and in this range.
    void Seek(DecodeTimestamp timestamp);

    // Updates |next_buffer_index_| to point to next keyframe after or equal to
    // |timestamp|.
    void SeekAheadTo(DecodeTimestamp timestamp);

    // Updates |next_buffer_index_| to point to next keyframe strictly after
    // |timestamp|.
    void SeekAheadPast(DecodeTimestamp timestamp);

    // Seeks to the beginning of the range.
    void SeekToStart();

    // Finds the next keyframe from |buffers_| starting at or after |timestamp|
    // and creates and returns a new SourceBufferRange with the buffers from that
    // keyframe onward. The buffers in the new SourceBufferRange are moved out of
    // this range. If there is no keyframe at or after |timestamp|, SplitRange()
    // returns null and this range is unmodified.
    SourceBufferRange* SplitRange(DecodeTimestamp timestamp);

    // Deletes the buffers from this range starting at |timestamp|, exclusive if
    // |is_exclusive| is true, inclusive otherwise.
    // Resets |next_buffer_index_| if the buffer at |next_buffer_index_| was
    // deleted, and deletes the |keyframe_map_| entries for the buffers that
    // were removed.
    // |deleted_buffers| contains the buffers that were deleted from this range,
    // starting at the buffer that had been at |next_buffer_index_|.
    // Returns true if everything in the range was deleted. Otherwise
    // returns false.
    bool TruncateAt(DecodeTimestamp timestamp,
        BufferQueue* deleted_buffers, bool is_exclusive);
    // Deletes all buffers in range.
    void DeleteAll(BufferQueue* deleted_buffers);

    // Deletes a GOP from the front or back of the range and moves these
    // buffers into |deleted_buffers|. Returns the number of bytes deleted from
    // the range (i.e. the size in bytes of |deleted_buffers|).
    size_t DeleteGOPFromFront(BufferQueue* deleted_buffers);
    size_t DeleteGOPFromBack(BufferQueue* deleted_buffers);

    // Gets the range of GOP to secure at least |bytes_to_free| from
    // [|start_timestamp|, |end_timestamp|).
    // Returns the size of the buffers to secure if the buffers of
    // [|start_timestamp|, |end_removal_timestamp|) is removed.
    // Will not update |end_removal_timestamp| if the returned size is 0.
    size_t GetRemovalGOP(
        DecodeTimestamp start_timestamp, DecodeTimestamp end_timestamp,
        size_t bytes_to_free, DecodeTimestamp* end_removal_timestamp);

    bool FirstGOPEarlierThanMediaTime(DecodeTimestamp media_time) const;

    // Indicates whether the GOP at the beginning or end of the range contains the
    // next buffer position.
    bool FirstGOPContainsNextBufferPosition() const;
    bool LastGOPContainsNextBufferPosition() const;

    // Updates |out_buffer| with the next buffer in presentation order. Seek()
    // must be called before calls to GetNextBuffer(), and buffers are returned
    // in order from the last call to Seek(). Returns true if |out_buffer| is
    // filled with a valid buffer, false if there is not enough data to fulfill
    // the request.
    bool GetNextBuffer(scoped_refptr<StreamParserBuffer>* out_buffer);
    bool HasNextBuffer() const;

    // Returns the config ID for the buffer that will be returned by
    // GetNextBuffer().
    int GetNextConfigId() const;

    // Returns true if the range knows the position of the next buffer it should
    // return, i.e. it has been Seek()ed. This does not necessarily mean that it
    // has the next buffer yet.
    bool HasNextBufferPosition() const;

    // Resets this range to an "unseeked" state.
    void ResetNextBufferPosition();

    // Returns the timestamp of the next buffer that will be returned from
    // GetNextBuffer(), or kNoTimestamp() if the timestamp is unknown.
    DecodeTimestamp GetNextTimestamp() const;

    // Returns the start timestamp of the range.
    DecodeTimestamp GetStartTimestamp() const;

    // Returns the timestamp of the last buffer in the range.
    DecodeTimestamp GetEndTimestamp() const;

    // Returns the timestamp for the end of the buffered region in this range.
    // This is an approximation if the duration for the last buffer in the range
    // is unset.
    DecodeTimestamp GetBufferedEndTimestamp() const;

    // Gets the timestamp for the keyframe that is after |timestamp|. If
    // there isn't a keyframe in the range after |timestamp| then kNoTimestamp()
    // is returned. If |timestamp| is in the "gap" between the value  returned by
    // GetStartTimestamp() and the timestamp on the first buffer in |buffers_|,
    // then |timestamp| is returned.
    DecodeTimestamp NextKeyframeTimestamp(DecodeTimestamp timestamp);

    // Gets the timestamp for the closest keyframe that is <= |timestamp|. If
    // there isn't a keyframe before |timestamp| or |timestamp| is outside
    // this range, then kNoTimestamp() is returned.
    DecodeTimestamp KeyframeBeforeTimestamp(DecodeTimestamp timestamp);

    // Returns whether a buffer with a starting timestamp of |timestamp| would
    // belong in this range. This includes a buffer that would be appended to
    // the end of the range.
    bool BelongsToRange(DecodeTimestamp timestamp) const;

    // Returns true if the range has enough data to seek to the specified
    // |timestamp|, false otherwise.
    bool CanSeekTo(DecodeTimestamp timestamp) const;

    // Returns true if this range's buffered timespan completely overlaps the
    // buffered timespan of |range|.
    bool CompletelyOverlaps(const SourceBufferRange& range) const;

    // Returns true if the end of this range contains buffers that overlaps with
    // the beginning of |range|.
    bool EndOverlaps(const SourceBufferRange& range) const;

    // Returns true if |timestamp| is the timestamp of the next buffer in
    // sequence after |buffers_.back()|, false otherwise.
    bool IsNextInSequence(DecodeTimestamp timestamp, bool is_key_frame) const;

    // Adds all buffers which overlap [start, end) to the end of |buffers|.  If
    // no buffers exist in the range returns false, true otherwise.
    bool GetBuffersInRange(DecodeTimestamp start, DecodeTimestamp end,
        BufferQueue* buffers);

    size_t size_in_bytes() const { return size_in_bytes_; }

private:
    typedef std::map<DecodeTimestamp, int> KeyframeMap;

    // Called during AppendBuffersToEnd to adjust estimated duration at the
    // end of the last append to match the delta in timestamps between
    // the last append and the upcoming append. This is a workaround for
    // WebM media where a duration is not always specified.
    void AdjustEstimatedDurationForNewAppend(const BufferQueue& new_buffers);

    // Seeks the range to the next keyframe after |timestamp|. If
    // |skip_given_timestamp| is true, the seek will go to a keyframe with a
    // timestamp strictly greater than |timestamp|.
    void SeekAhead(DecodeTimestamp timestamp, bool skip_given_timestamp);

    // Returns an iterator in |buffers_| pointing to the buffer at |timestamp|.
    // If |skip_given_timestamp| is true, this returns the first buffer with
    // timestamp greater than |timestamp|.
    BufferQueue::iterator GetBufferItrAt(
        DecodeTimestamp timestamp, bool skip_given_timestamp);

    // Returns an iterator in |keyframe_map_| pointing to the next keyframe after
    // |timestamp|. If |skip_given_timestamp| is true, this returns the first
    // keyframe with a timestamp strictly greater than |timestamp|.
    KeyframeMap::iterator GetFirstKeyframeAt(
        DecodeTimestamp timestamp, bool skip_given_timestamp);

    // Returns an iterator in |keyframe_map_| pointing to the first keyframe
    // before or at |timestamp|.
    KeyframeMap::iterator GetFirstKeyframeAtOrBefore(DecodeTimestamp timestamp);

    // Helper method to delete buffers in |buffers_| starting at
    // |starting_point|, an iterator in |buffers_|.
    // Returns true if everything in the range was removed. Returns
    // false if the range still contains buffers.
    bool TruncateAt(const BufferQueue::iterator& starting_point,
        BufferQueue* deleted_buffers);

    // Frees the buffers in |buffers_| from [|start_point|,|ending_point|) and
    // updates the |size_in_bytes_| accordingly. Does not update |keyframe_map_|.
    void FreeBufferRange(const BufferQueue::iterator& starting_point,
        const BufferQueue::iterator& ending_point);

    // Returns the distance in time estimating how far from the beginning or end
    // of this range a buffer can be to considered in the range.
    base::TimeDelta GetFudgeRoom() const;

    // Returns the approximate duration of a buffer in this range.
    base::TimeDelta GetApproximateDuration() const;

    // Keeps track of whether gaps are allowed.
    const GapPolicy gap_policy_;

    // An ordered list of buffers in this range.
    BufferQueue buffers_;

    // Maps keyframe timestamps to its index position in |buffers_|.
    KeyframeMap keyframe_map_;

    // Index base of all positions in |keyframe_map_|. In other words, the
    // real position of entry |k| of |keyframe_map_| in the range is:
    //   keyframe_map_[k] - keyframe_map_index_base_
    int keyframe_map_index_base_;

    // Index into |buffers_| for the next buffer to be returned by
    // GetNextBuffer(), set to -1 before Seek().
    int next_buffer_index_;

    // If the first buffer in this range is the beginning of a media segment,
    // |media_segment_start_time_| is the time when the media segment begins.
    // |media_segment_start_time_| may be <= the timestamp of the first buffer in
    // |buffers_|. |media_segment_start_time_| is kNoTimestamp() if this range
    // does not start at the beginning of a media segment, which can only happen
    // garbage collection or after an end overlap that results in a split range
    // (we don't have a way of knowing the media segment timestamp for the new
    // range).
    DecodeTimestamp media_segment_start_time_;

    // Called to get the largest interbuffer distance seen so far in the stream.
    InterbufferDistanceCB interbuffer_distance_cb_;

    // Stores the amount of memory taken up by the data in |buffers_|.
    size_t size_in_bytes_;

    DISALLOW_COPY_AND_ASSIGN(SourceBufferRange);
};

} // namespace media

#endif // MEDIA_FILTERS_SOURCE_BUFFER_RANGE_H_

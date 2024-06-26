// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_STREAM_PARSER_H_
#define MEDIA_BASE_STREAM_PARSER_H_

#include <deque>
#include <map>
#include <string>
#include <vector>

#include "base/callback_forward.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "media/base/demuxer_stream.h"
#include "media/base/eme_constants.h"
#include "media/base/media_export.h"
#include "media/base/media_log.h"

namespace media {

class AudioDecoderConfig;
class StreamParserBuffer;
class TextTrackConfig;
class VideoDecoderConfig;

// Abstract interface for parsing media byte streams.
class MEDIA_EXPORT StreamParser {
public:
    typedef std::deque<scoped_refptr<StreamParserBuffer>> BufferQueue;

    // Range of |TrackId| is dependent upon stream parsers. It is currently
    // the key for the buffer's text track config in the applicable
    // TextTrackConfigMap (which is passed in StreamParser::NewConfigCB), or
    // 0 for other media types that currently allow at most one track.
    // WebMTracksParser uses -1 as an invalid text track number.
    // TODO(wolenetz/acolwell): Change to size_type while fixing stream parsers to
    // emit validated track configuration and buffer vectors rather than max 1
    // audio, max 1 video, and N text tracks in a map keyed by
    // bytestream-specific-ranged track numbers. See http://crbug.com/341581.
    typedef int TrackId;

    // Map of text track ID to the track configuration.
    typedef std::map<TrackId, TextTrackConfig> TextTrackConfigMap;

    // Map of text track ID to decode-timestamp-ordered buffers for the track.
    typedef std::map<TrackId, const BufferQueue> TextBufferQueueMap;

    // Stream parameters passed in InitCB.
    struct InitParameters {
        InitParameters(base::TimeDelta duration);

        // Stream duration.
        base::TimeDelta duration;

        // Indicates the source time associated with presentation timestamp 0. A
        // null Time is returned if no mapping to Time exists.
        base::Time timeline_offset;

        // Indicates that timestampOffset should be updated based on the earliest
        // end timestamp (audio or video) provided during each NewBuffersCB.
        bool auto_update_timestamp_offset;

        // Indicates live stream.
        DemuxerStream::Liveness liveness;
    };

    // Indicates completion of parser initialization.
    //   params - Stream parameters.
    typedef base::Callback<void(const InitParameters& params)> InitCB;

    // Indicates when new stream configurations have been parsed.
    // First parameter - The new audio configuration. If the config is not valid
    //                   then it means that there isn't an audio stream.
    // Second parameter - The new video configuration. If the config is not valid
    //                    then it means that there isn't an audio stream.
    // Third parameter - The new text tracks configuration.  If the map is empty,
    //                   then no text tracks were parsed from the stream.
    // Return value - True if the new configurations are accepted.
    //                False if the new configurations are not supported
    //                and indicates that a parsing error should be signalled.
    typedef base::Callback<bool(const AudioDecoderConfig&,
        const VideoDecoderConfig&,
        const TextTrackConfigMap&)>
        NewConfigCB;

    // New stream buffers have been parsed.
    // First parameter - A queue of newly parsed audio buffers.
    // Second parameter - A queue of newly parsed video buffers.
    // Third parameter - A map of text track ids to queues of newly parsed inband
    //                   text buffers. If the map is not empty, it must contain
    //                   at least one track with a non-empty queue of text
    //                   buffers.
    // Return value - True indicates that the buffers are accepted.
    //                False if something was wrong with the buffers and a parsing
    //                error should be signalled.
    typedef base::Callback<bool(const BufferQueue&,
        const BufferQueue&,
        const TextBufferQueueMap&)>
        NewBuffersCB;

    // Signals the beginning of a new media segment.
    typedef base::Callback<void()> NewMediaSegmentCB;

    // A new potentially encrypted stream has been parsed.
    // First parameter - The type of the initialization data associated with the
    //                   stream.
    // Second parameter - The initialization data associated with the stream.
    typedef base::Callback<void(EmeInitDataType, const std::vector<uint8>&)>
        EncryptedMediaInitDataCB;

    StreamParser();
    virtual ~StreamParser();

    // Initializes the parser with necessary callbacks. Must be called before any
    // data is passed to Parse(). |init_cb| will be called once enough data has
    // been parsed to determine the initial stream configurations, presentation
    // start time, and duration. If |ignore_text_track| is true, then no text
    // buffers should be passed later by the parser to |new_buffers_cb|.
    virtual void Init(
        const InitCB& init_cb,
        const NewConfigCB& config_cb,
        const NewBuffersCB& new_buffers_cb,
        bool ignore_text_track,
        const EncryptedMediaInitDataCB& encrypted_media_init_data_cb,
        const NewMediaSegmentCB& new_segment_cb,
        const base::Closure& end_of_segment_cb,
        const scoped_refptr<MediaLog>& media_log)
        = 0;

    // Called when a seek occurs. This flushes the current parser state
    // and puts the parser in a state where it can receive data for the new seek
    // point.
    virtual void Flush() = 0;

    // Called when there is new data to parse.
    //
    // Returns true if the parse succeeds.
    virtual bool Parse(const uint8* buf, int size) = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(StreamParser);
};

// Appends to |merged_buffers| the provided buffers in decode-timestamp order.
// Any previous contents of |merged_buffers| is assumed to have lower
// decode timestamps versus the provided buffers. All provided buffer queues
// are assumed to already be in decode-timestamp order.
// Returns false if any of the provided audio/video/text buffers are found
// to not be in decode timestamp order, or have a decode timestamp less than
// the last buffer, if any, in |merged_buffers|. Partial results may exist
// in |merged_buffers| in this case. Returns true on success.
// No validation of media type within the various buffer queues is done here.
// TODO(wolenetz/acolwell): Merge incrementally in parsers to eliminate
// subtle issues with tie-breaking. See http://crbug.com/338484.
MEDIA_EXPORT bool MergeBufferQueues(
    const StreamParser::BufferQueue& audio_buffers,
    const StreamParser::BufferQueue& video_buffers,
    const StreamParser::TextBufferQueueMap& text_buffers,
    StreamParser::BufferQueue* merged_buffers);

} // namespace media

#endif // MEDIA_BASE_STREAM_PARSER_H_

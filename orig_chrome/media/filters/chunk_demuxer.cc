// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/chunk_demuxer.h"

#include <algorithm>
#include <limits>
#include <list>

#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/location.h"
#include "base/stl_util.h"
#include "media/base/audio_decoder_config.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/stream_parser_buffer.h"
#include "media/base/timestamp_constants.h"
#include "media/base/video_decoder_config.h"
#include "media/filters/frame_processor.h"
#include "media/filters/stream_parser_factory.h"

using base::TimeDelta;

namespace media {

static TimeDelta EndTimestamp(const StreamParser::BufferQueue& queue)
{
    return queue.back()->timestamp() + queue.back()->duration();
}

// List of time ranges for each SourceBuffer.
typedef std::list<Ranges<TimeDelta>> RangesList;
static Ranges<TimeDelta> ComputeIntersection(const RangesList& activeRanges,
    bool ended)
{
    // Implementation of HTMLMediaElement.buffered algorithm in MSE spec.
    // https://dvcs.w3.org/hg/html-media/raw-file/default/media-source/media-source.html#dom-htmlmediaelement.buffered

    // Step 1: If activeSourceBuffers.length equals 0 then return an empty
    //  TimeRanges object and abort these steps.
    if (activeRanges.empty())
        return Ranges<TimeDelta>();

    // Step 2: Let active ranges be the ranges returned by buffered for each
    //  SourceBuffer object in activeSourceBuffers.
    // Step 3: Let highest end time be the largest range end time in the active
    //  ranges.
    TimeDelta highest_end_time;
    for (RangesList::const_iterator itr = activeRanges.begin();
         itr != activeRanges.end(); ++itr) {
        if (!itr->size())
            continue;

        highest_end_time = std::max(highest_end_time, itr->end(itr->size() - 1));
    }

    // Step 4: Let intersection ranges equal a TimeRange object containing a
    //  single range from 0 to highest end time.
    Ranges<TimeDelta> intersection_ranges;
    intersection_ranges.Add(TimeDelta(), highest_end_time);

    // Step 5: For each SourceBuffer object in activeSourceBuffers run the
    //  following steps:
    for (RangesList::const_iterator itr = activeRanges.begin();
         itr != activeRanges.end(); ++itr) {
        // Step 5.1: Let source ranges equal the ranges returned by the buffered
        //  attribute on the current SourceBuffer.
        Ranges<TimeDelta> source_ranges = *itr;

        // Step 5.2: If readyState is "ended", then set the end time on the last
        //  range in source ranges to highest end time.
        if (ended && source_ranges.size() > 0u) {
            source_ranges.Add(source_ranges.start(source_ranges.size() - 1),
                highest_end_time);
        }

        // Step 5.3: Let new intersection ranges equal the intersection between
        // the intersection ranges and the source ranges.
        // Step 5.4: Replace the ranges in intersection ranges with the new
        // intersection ranges.
        intersection_ranges = intersection_ranges.IntersectionWith(source_ranges);
    }

    return intersection_ranges;
}

// Contains state belonging to a source id.
// TODO: SourceState needs to be moved to a separate file and covered with unit
// tests (see crbug.com/525836)
class SourceState {
public:
    // Callback signature used to create ChunkDemuxerStreams.
    typedef base::Callback<ChunkDemuxerStream*(
        DemuxerStream::Type)>
        CreateDemuxerStreamCB;

    typedef ChunkDemuxer::InitSegmentReceivedCB InitSegmentReceivedCB;

    typedef base::Callback<void(
        ChunkDemuxerStream*, const TextTrackConfig&)>
        NewTextTrackCB;

    SourceState(scoped_ptr<StreamParser> stream_parser,
        scoped_ptr<FrameProcessor> frame_processor,
        const CreateDemuxerStreamCB& create_demuxer_stream_cb,
        const scoped_refptr<MediaLog>& media_log);

    ~SourceState();

    void Init(const StreamParser::InitCB& init_cb,
        bool allow_audio,
        bool allow_video,
        const StreamParser::EncryptedMediaInitDataCB&
            encrypted_media_init_data_cb,
        const NewTextTrackCB& new_text_track_cb);

    // Appends new data to the StreamParser.
    // Returns true if the data was successfully appended. Returns false if an
    // error occurred. |*timestamp_offset| is used and possibly updated by the
    // append. |append_window_start| and |append_window_end| correspond to the MSE
    // spec's similarly named source buffer attributes that are used in coded
    // frame processing. |init_segment_received_cb| is run for each new fully
    // parsed initialization segment.
    bool Append(const uint8* data,
        size_t length,
        TimeDelta append_window_start,
        TimeDelta append_window_end,
        TimeDelta* timestamp_offset,
        const InitSegmentReceivedCB& init_segment_received_cb);

    // Aborts the current append sequence and resets the parser.
    void ResetParserState(TimeDelta append_window_start,
        TimeDelta append_window_end,
        TimeDelta* timestamp_offset);

    // Calls Remove(|start|, |end|, |duration|) on all
    // ChunkDemuxerStreams managed by this object.
    void Remove(TimeDelta start, TimeDelta end, TimeDelta duration);

    // If the buffer is full, attempts to try to free up space, as specified in
    // the "Coded Frame Eviction Algorithm" in the Media Source Extensions Spec.
    // Returns false iff buffer is still full after running eviction.
    // https://w3c.github.io/media-source/#sourcebuffer-coded-frame-eviction
    bool EvictCodedFrames(DecodeTimestamp media_time, size_t newDataSize);

    // Returns true if currently parsing a media segment, or false otherwise.
    bool parsing_media_segment() const { return parsing_media_segment_; }

    // Sets |frame_processor_|'s sequence mode to |sequence_mode|.
    void SetSequenceMode(bool sequence_mode);

    // Signals the coded frame processor to update its group start timestamp to be
    // |timestamp_offset| if it is in sequence append mode.
    void SetGroupStartTimestampIfInSequenceMode(base::TimeDelta timestamp_offset);

    // Returns the range of buffered data in this source, capped at |duration|.
    // |ended| - Set to true if end of stream has been signaled and the special
    // end of stream range logic needs to be executed.
    Ranges<TimeDelta> GetBufferedRanges(TimeDelta duration, bool ended) const;

    // Returns the highest buffered duration across all streams managed
    // by this object.
    // Returns TimeDelta() if none of the streams contain buffered data.
    TimeDelta GetMaxBufferedDuration() const;

    // Helper methods that call methods with similar names on all the
    // ChunkDemuxerStreams managed by this object.
    void StartReturningData();
    void AbortReads();
    void Seek(TimeDelta seek_time);
    void CompletePendingReadIfPossible();
    void OnSetDuration(TimeDelta duration);
    void MarkEndOfStream();
    void UnmarkEndOfStream();
    void Shutdown();
    // Sets the memory limit on each stream of a specific type.
    // |memory_limit| is the maximum number of bytes each stream of type |type|
    // is allowed to hold in its buffer.
    void SetMemoryLimits(DemuxerStream::Type type, size_t memory_limit);
    bool IsSeekWaitingForData() const;

private:
    // Called by the |stream_parser_| when a new initialization segment is
    // encountered.
    // Returns true on a successful call. Returns false if an error occurred while
    // processing decoder configurations.
    bool OnNewConfigs(bool allow_audio, bool allow_video,
        const AudioDecoderConfig& audio_config,
        const VideoDecoderConfig& video_config,
        const StreamParser::TextTrackConfigMap& text_configs);

    // Called by the |stream_parser_| at the beginning of a new media segment.
    void OnNewMediaSegment();

    // Called by the |stream_parser_| at the end of a media segment.
    void OnEndOfMediaSegment();

    // Called by the |stream_parser_| when new buffers have been parsed.
    // It processes the new buffers using |frame_processor_|, which includes
    // appending the processed frames to associated demuxer streams for each
    // frame's track.
    // Returns true on a successful call. Returns false if an error occurred while
    // processing the buffers.
    bool OnNewBuffers(const StreamParser::BufferQueue& audio_buffers,
        const StreamParser::BufferQueue& video_buffers,
        const StreamParser::TextBufferQueueMap& text_map);

    void OnSourceInitDone(const StreamParser::InitParameters& params);

    // EstimateVideoDataSize uses some heuristics to estimate the size of the
    // video size in the chunk of muxed audio/video data without parsing it.
    // This is used by EvictCodedFrames algorithm, which happens before Append
    // (and therefore before parsing is performed) to prepare space for new data.
    size_t EstimateVideoDataSize(size_t muxed_data_chunk_size) const;

    CreateDemuxerStreamCB create_demuxer_stream_cb_;
    NewTextTrackCB new_text_track_cb_;

    // During Append(), if OnNewBuffers() coded frame processing updates the
    // timestamp offset then |*timestamp_offset_during_append_| is also updated
    // so Append()'s caller can know the new offset. This pointer is only non-NULL
    // during the lifetime of an Append() call.
    TimeDelta* timestamp_offset_during_append_;

    // During Append(), coded frame processing triggered by OnNewBuffers()
    // requires these two attributes. These are only valid during the lifetime of
    // an Append() call.
    TimeDelta append_window_start_during_append_;
    TimeDelta append_window_end_during_append_;

    // Set to true if the next buffers appended within the append window
    // represent the start of a new media segment. This flag being set
    // triggers a call to |new_segment_cb_| when the new buffers are
    // appended. The flag is set on actual media segment boundaries and
    // when the "append window" filtering causes discontinuities in the
    // appended data.
    // TODO(wolenetz/acolwell): Investigate if we need this, or if coded frame
    // processing's discontinuity logic is enough. See http://crbug.com/351489.
    //bool new_media_segment_;

    // Keeps track of whether a media segment is being parsed.
    bool parsing_media_segment_;

    // The object used to parse appended data.
    scoped_ptr<StreamParser> stream_parser_;

    ChunkDemuxerStream* audio_; // Not owned by |this|.
    ChunkDemuxerStream* video_; // Not owned by |this|.

    typedef std::map<StreamParser::TrackId, ChunkDemuxerStream*> TextStreamMap;
    TextStreamMap text_stream_map_; // |this| owns the map's stream pointers.

    scoped_ptr<FrameProcessor> frame_processor_;
    scoped_refptr<MediaLog> media_log_;
    StreamParser::InitCB init_cb_;

    // During Append(), OnNewConfigs() will trigger the initialization segment
    // received algorithm. This callback is only non-NULL during the lifetime of
    // an Append() call. Note, the MSE spec explicitly disallows this algorithm
    // during an Abort(), since Abort() is allowed only to emit coded frames, and
    // only if the parser is PARSING_MEDIA_SEGMENT (not an INIT segment).
    InitSegmentReceivedCB init_segment_received_cb_;

    // Indicates that timestampOffset should be updated automatically during
    // OnNewBuffers() based on the earliest end timestamp of the buffers provided.
    // TODO(wolenetz): Refactor this function while integrating April 29, 2014
    // changes to MSE spec. See http://crbug.com/371499.
    bool auto_update_timestamp_offset_;

    DISALLOW_COPY_AND_ASSIGN(SourceState);
};

SourceState::SourceState(scoped_ptr<StreamParser> stream_parser,
    scoped_ptr<FrameProcessor> frame_processor,
    const CreateDemuxerStreamCB& create_demuxer_stream_cb,
    const scoped_refptr<MediaLog>& media_log)
    : create_demuxer_stream_cb_(create_demuxer_stream_cb)
    , timestamp_offset_during_append_(NULL)
    //, new_media_segment_(false)
    , parsing_media_segment_(false)
    , stream_parser_(stream_parser.release())
    , audio_(NULL)
    , video_(NULL)
    , frame_processor_(frame_processor.release())
    , media_log_(media_log)
    , auto_update_timestamp_offset_(false)
{
    DCHECK(!create_demuxer_stream_cb_.is_null());
    DCHECK(frame_processor_);
}

SourceState::~SourceState()
{
    Shutdown();

    STLDeleteValues(&text_stream_map_);
}

void SourceState::Init(
    const StreamParser::InitCB& init_cb,
    bool allow_audio,
    bool allow_video,
    const StreamParser::EncryptedMediaInitDataCB& encrypted_media_init_data_cb,
    const NewTextTrackCB& new_text_track_cb)
{
    new_text_track_cb_ = new_text_track_cb;
    init_cb_ = init_cb;

    stream_parser_->Init(
        base::Bind(&SourceState::OnSourceInitDone, base::Unretained(this)),
        base::Bind(&SourceState::OnNewConfigs, base::Unretained(this), allow_audio, allow_video),
        base::Bind(&SourceState::OnNewBuffers, base::Unretained(this)),
        new_text_track_cb_.is_null(),
        encrypted_media_init_data_cb,
        base::Bind(&SourceState::OnNewMediaSegment, base::Unretained(this)),
        base::Bind(&SourceState::OnEndOfMediaSegment, base::Unretained(this)),
        media_log_);
}

void SourceState::SetSequenceMode(bool sequence_mode)
{
    DCHECK(!parsing_media_segment_);

    frame_processor_->SetSequenceMode(sequence_mode);
}

void SourceState::SetGroupStartTimestampIfInSequenceMode(
    base::TimeDelta timestamp_offset)
{
    DCHECK(!parsing_media_segment_);

    frame_processor_->SetGroupStartTimestampIfInSequenceMode(timestamp_offset);
}

bool SourceState::Append(
    const uint8* data,
    size_t length,
    TimeDelta append_window_start,
    TimeDelta append_window_end,
    TimeDelta* timestamp_offset,
    const InitSegmentReceivedCB& init_segment_received_cb)
{
    DCHECK(timestamp_offset);
    DCHECK(!timestamp_offset_during_append_);
    DCHECK(!init_segment_received_cb.is_null());
    DCHECK(init_segment_received_cb_.is_null());
    append_window_start_during_append_ = append_window_start;
    append_window_end_during_append_ = append_window_end;
    timestamp_offset_during_append_ = timestamp_offset;
    init_segment_received_cb_ = init_segment_received_cb;

    // TODO(wolenetz/acolwell): Curry and pass a NewBuffersCB here bound with
    // append window and timestamp offset pointer. See http://crbug.com/351454.
    bool result = stream_parser_->Parse(data, length);
    if (!result) {
        MEDIA_LOG(ERROR, media_log_)
            << __FUNCTION__ << ": stream parsing failed."
            << " Data size=" << length
            << " append_window_start=" << append_window_start.InSecondsF()
            << " append_window_end=" << append_window_end.InSecondsF();
    }
    timestamp_offset_during_append_ = NULL;
    init_segment_received_cb_.Reset();
    return result;
}

void SourceState::ResetParserState(TimeDelta append_window_start,
    TimeDelta append_window_end,
    base::TimeDelta* timestamp_offset)
{
    DCHECK(timestamp_offset);
    DCHECK(!timestamp_offset_during_append_);
    timestamp_offset_during_append_ = timestamp_offset;
    append_window_start_during_append_ = append_window_start;
    append_window_end_during_append_ = append_window_end;

    stream_parser_->Flush();
    timestamp_offset_during_append_ = NULL;

    frame_processor_->Reset();
    parsing_media_segment_ = false;
}

void SourceState::Remove(TimeDelta start, TimeDelta end, TimeDelta duration)
{
    if (audio_)
        audio_->Remove(start, end, duration);

    if (video_)
        video_->Remove(start, end, duration);

    for (TextStreamMap::iterator itr = text_stream_map_.begin();
         itr != text_stream_map_.end(); ++itr) {
        itr->second->Remove(start, end, duration);
    }
}

size_t SourceState::EstimateVideoDataSize(size_t muxed_data_chunk_size) const
{
    DCHECK(audio_);
    DCHECK(video_);

    size_t videoBufferedSize = video_->GetBufferedSize();
    size_t audioBufferedSize = audio_->GetBufferedSize();
    if (videoBufferedSize == 0 || audioBufferedSize == 0) {
        // At this point either audio or video buffer is empty, which means buffer
        // levels are probably low anyway and we should have enough space in the
        // buffers for appending new data, so just take a very rough guess.
        return muxed_data_chunk_size * 7 / 8;
    }

    // We need to estimate how much audio and video data is going to be in the
    // newly appended data chunk to make space for the new data. And we need to do
    // that without parsing the data (which will happen later, in the Append
    // phase). So for now we can only rely on some heuristic here. Let's assume
    // that the proportion of the audio/video in the new data chunk is the same as
    // the current ratio of buffered audio/video.
    // Longer term this should go away once we further change the MSE GC algorithm
    // to work across all streams of a SourceBuffer (see crbug.com/520704).
    double videoBufferedSizeF = static_cast<double>(videoBufferedSize);
    double audioBufferedSizeF = static_cast<double>(audioBufferedSize);

    double totalBufferedSizeF = videoBufferedSizeF + audioBufferedSizeF;
    CHECK_GT(totalBufferedSizeF, 0.0);

    double videoRatio = videoBufferedSizeF / totalBufferedSizeF;
    CHECK_GE(videoRatio, 0.0);
    CHECK_LE(videoRatio, 1.0);
    double estimatedVideoSize = muxed_data_chunk_size * videoRatio;
    return static_cast<size_t>(estimatedVideoSize);
}

bool SourceState::EvictCodedFrames(DecodeTimestamp media_time,
    size_t newDataSize)
{
    bool success = true;

    DVLOG(3) << __FUNCTION__ << " media_time=" << media_time.InSecondsF()
             << " newDataSize=" << newDataSize
             << " videoBufferedSize=" << (video_ ? video_->GetBufferedSize() : 0)
             << " audioBufferedSize=" << (audio_ ? audio_->GetBufferedSize() : 0);

    size_t newAudioSize = 0;
    size_t newVideoSize = 0;
    if (audio_ && video_) {
        newVideoSize = EstimateVideoDataSize(newDataSize);
        newAudioSize = newDataSize - newVideoSize;
    } else if (video_) {
        newVideoSize = newDataSize;
    } else if (audio_) {
        newAudioSize = newDataSize;
    }

    DVLOG(3) << __FUNCTION__ << " estimated audio/video sizes: "
             << " newVideoSize=" << newVideoSize
             << " newAudioSize=" << newAudioSize;

    if (audio_)
        success = audio_->EvictCodedFrames(media_time, newAudioSize) && success;

    if (video_)
        success = video_->EvictCodedFrames(media_time, newVideoSize) && success;

    for (TextStreamMap::iterator itr = text_stream_map_.begin();
         itr != text_stream_map_.end(); ++itr) {
        success = itr->second->EvictCodedFrames(media_time, 0) && success;
    }

    DVLOG(3) << __FUNCTION__ << " result=" << success
             << " videoBufferedSize=" << (video_ ? video_->GetBufferedSize() : 0)
             << " audioBufferedSize=" << (audio_ ? audio_->GetBufferedSize() : 0);

    return success;
}

Ranges<TimeDelta> SourceState::GetBufferedRanges(TimeDelta duration,
    bool ended) const
{
    // TODO(acolwell): When we start allowing disabled tracks we'll need to update
    // this code to only add ranges from active tracks.
    RangesList ranges_list;
    if (audio_)
        ranges_list.push_back(audio_->GetBufferedRanges(duration));

    if (video_)
        ranges_list.push_back(video_->GetBufferedRanges(duration));

    for (TextStreamMap::const_iterator itr = text_stream_map_.begin();
         itr != text_stream_map_.end(); ++itr) {
        ranges_list.push_back(itr->second->GetBufferedRanges(duration));
    }

    return ComputeIntersection(ranges_list, ended);
}

TimeDelta SourceState::GetMaxBufferedDuration() const
{
    TimeDelta max_duration;

    if (audio_)
        max_duration = std::max(max_duration, audio_->GetBufferedDuration());

    if (video_)
        max_duration = std::max(max_duration, video_->GetBufferedDuration());

    for (TextStreamMap::const_iterator itr = text_stream_map_.begin();
         itr != text_stream_map_.end(); ++itr) {
        max_duration = std::max(max_duration, itr->second->GetBufferedDuration());
    }

    return max_duration;
}

void SourceState::StartReturningData()
{
    if (audio_)
        audio_->StartReturningData();

    if (video_)
        video_->StartReturningData();

    for (TextStreamMap::iterator itr = text_stream_map_.begin();
         itr != text_stream_map_.end(); ++itr) {
        itr->second->StartReturningData();
    }
}

void SourceState::AbortReads()
{
    if (audio_)
        audio_->AbortReads();

    if (video_)
        video_->AbortReads();

    for (TextStreamMap::iterator itr = text_stream_map_.begin();
         itr != text_stream_map_.end(); ++itr) {
        itr->second->AbortReads();
    }
}

void SourceState::Seek(TimeDelta seek_time)
{
    if (audio_)
        audio_->Seek(seek_time);

    if (video_)
        video_->Seek(seek_time);

    for (TextStreamMap::iterator itr = text_stream_map_.begin();
         itr != text_stream_map_.end(); ++itr) {
        itr->second->Seek(seek_time);
    }
}

void SourceState::CompletePendingReadIfPossible()
{
    if (audio_)
        audio_->CompletePendingReadIfPossible();

    if (video_)
        video_->CompletePendingReadIfPossible();

    for (TextStreamMap::iterator itr = text_stream_map_.begin();
         itr != text_stream_map_.end(); ++itr) {
        itr->second->CompletePendingReadIfPossible();
    }
}

void SourceState::OnSetDuration(TimeDelta duration)
{
    if (audio_)
        audio_->OnSetDuration(duration);

    if (video_)
        video_->OnSetDuration(duration);

    for (TextStreamMap::iterator itr = text_stream_map_.begin();
         itr != text_stream_map_.end(); ++itr) {
        itr->second->OnSetDuration(duration);
    }
}

void SourceState::MarkEndOfStream()
{
    if (audio_)
        audio_->MarkEndOfStream();

    if (video_)
        video_->MarkEndOfStream();

    for (TextStreamMap::iterator itr = text_stream_map_.begin();
         itr != text_stream_map_.end(); ++itr) {
        itr->second->MarkEndOfStream();
    }
}

void SourceState::UnmarkEndOfStream()
{
    if (audio_)
        audio_->UnmarkEndOfStream();

    if (video_)
        video_->UnmarkEndOfStream();

    for (TextStreamMap::iterator itr = text_stream_map_.begin();
         itr != text_stream_map_.end(); ++itr) {
        itr->second->UnmarkEndOfStream();
    }
}

void SourceState::Shutdown()
{
    if (audio_)
        audio_->Shutdown();

    if (video_)
        video_->Shutdown();

    for (TextStreamMap::iterator itr = text_stream_map_.begin();
         itr != text_stream_map_.end(); ++itr) {
        itr->second->Shutdown();
    }
}

void SourceState::SetMemoryLimits(DemuxerStream::Type type,
    size_t memory_limit)
{
    switch (type) {
    case DemuxerStream::AUDIO:
        if (audio_)
            audio_->SetStreamMemoryLimit(memory_limit);
        break;
    case DemuxerStream::VIDEO:
        if (video_)
            video_->SetStreamMemoryLimit(memory_limit);
        break;
    case DemuxerStream::TEXT:
        for (TextStreamMap::iterator itr = text_stream_map_.begin();
             itr != text_stream_map_.end(); ++itr) {
            itr->second->SetStreamMemoryLimit(memory_limit);
        }
        break;
    case DemuxerStream::UNKNOWN:
    case DemuxerStream::NUM_TYPES:
        NOTREACHED();
        break;
    }
}

bool SourceState::IsSeekWaitingForData() const
{
    if (audio_ && audio_->IsSeekWaitingForData())
        return true;

    if (video_ && video_->IsSeekWaitingForData())
        return true;

    // NOTE: We are intentionally not checking the text tracks
    // because text tracks are discontinuous and may not have data
    // for the seek position. This is ok and playback should not be
    // stalled because we don't have cues. If cues, with timestamps after
    // the seek time, eventually arrive they will be delivered properly
    // in response to ChunkDemuxerStream::Read() calls.

    return false;
}

bool SourceState::OnNewConfigs(
    bool allow_audio, bool allow_video,
    const AudioDecoderConfig& audio_config,
    const VideoDecoderConfig& video_config,
    const StreamParser::TextTrackConfigMap& text_configs)
{
    DVLOG(1) << "OnNewConfigs(" << allow_audio << ", " << allow_video
             << ", " << audio_config.IsValidConfig()
             << ", " << video_config.IsValidConfig() << ")";
    DCHECK(!init_segment_received_cb_.is_null());

    if (!audio_config.IsValidConfig() && !video_config.IsValidConfig()) {
        DVLOG(1) << "OnNewConfigs() : Audio & video config are not valid!";
        return false;
    }

    // Signal an error if we get configuration info for stream types that weren't
    // specified in AddId() or more configs after a stream is initialized.
    if (allow_audio != audio_config.IsValidConfig()) {
        MEDIA_LOG(ERROR, media_log_)
            << "Initialization segment"
            << (audio_config.IsValidConfig() ? " has" : " does not have")
            << " an audio track, but the mimetype"
            << (allow_audio ? " specifies" : " does not specify")
            << " an audio codec.";
        return false;
    }

    if (allow_video != video_config.IsValidConfig()) {
        MEDIA_LOG(ERROR, media_log_)
            << "Initialization segment"
            << (video_config.IsValidConfig() ? " has" : " does not have")
            << " a video track, but the mimetype"
            << (allow_video ? " specifies" : " does not specify")
            << " a video codec.";
        return false;
    }

    bool success = true;
    if (audio_config.IsValidConfig()) {
        if (!audio_) {
            media_log_->SetBooleanProperty("found_audio_stream", true);
        }
        if (!audio_ || audio_->audio_decoder_config().codec() != audio_config.codec()) {
            media_log_->SetStringProperty("audio_codec_name",
                audio_config.GetHumanReadableCodecName());
        }

        if (!audio_) {
            audio_ = create_demuxer_stream_cb_.Run(DemuxerStream::AUDIO);

            if (!audio_) {
                DVLOG(1) << "Failed to create an audio stream.";
                return false;
            }

            if (!frame_processor_->AddTrack(FrameProcessor::kAudioTrackId, audio_)) {
                DVLOG(1) << "Failed to add audio track to frame processor.";
                return false;
            }
        }

        frame_processor_->OnPossibleAudioConfigUpdate(audio_config);
        success &= audio_->UpdateAudioConfig(audio_config, media_log_);
    }

    if (video_config.IsValidConfig()) {
        if (!video_) {
            media_log_->SetBooleanProperty("found_video_stream", true);
        }
        if (!video_ || video_->video_decoder_config().codec() != video_config.codec()) {
            media_log_->SetStringProperty("video_codec_name",
                video_config.GetHumanReadableCodecName());
        }

        if (!video_) {
            video_ = create_demuxer_stream_cb_.Run(DemuxerStream::VIDEO);

            if (!video_) {
                DVLOG(1) << "Failed to create a video stream.";
                return false;
            }

            if (!frame_processor_->AddTrack(FrameProcessor::kVideoTrackId, video_)) {
                DVLOG(1) << "Failed to add video track to frame processor.";
                return false;
            }
        }

        success &= video_->UpdateVideoConfig(video_config, media_log_);
    }

    typedef StreamParser::TextTrackConfigMap::const_iterator TextConfigItr;
    if (text_stream_map_.empty()) {
        for (TextConfigItr itr = text_configs.begin();
             itr != text_configs.end(); ++itr) {
            ChunkDemuxerStream* const text_stream = create_demuxer_stream_cb_.Run(DemuxerStream::TEXT);
            if (!frame_processor_->AddTrack(itr->first, text_stream)) {
                success &= false;
                MEDIA_LOG(ERROR, media_log_) << "Failed to add text track ID "
                                             << itr->first << " to frame processor.";
                break;
            }
            text_stream->UpdateTextConfig(itr->second, media_log_);
            text_stream_map_[itr->first] = text_stream;
            new_text_track_cb_.Run(text_stream, itr->second);
        }
    } else {
        const size_t text_count = text_stream_map_.size();
        if (text_configs.size() != text_count) {
            success &= false;
            MEDIA_LOG(ERROR, media_log_)
                << "The number of text track configs changed.";
        } else if (text_count == 1) {
            TextConfigItr config_itr = text_configs.begin();
            TextStreamMap::iterator stream_itr = text_stream_map_.begin();
            ChunkDemuxerStream* text_stream = stream_itr->second;
            TextTrackConfig old_config = text_stream->text_track_config();
            TextTrackConfig new_config(config_itr->second.kind(),
                config_itr->second.label(),
                config_itr->second.language(),
                old_config.id());
            if (!new_config.Matches(old_config)) {
                success &= false;
                MEDIA_LOG(ERROR, media_log_)
                    << "New text track config does not match old one.";
            } else {
                StreamParser::TrackId old_id = stream_itr->first;
                StreamParser::TrackId new_id = config_itr->first;
                if (new_id != old_id) {
                    if (frame_processor_->UpdateTrack(old_id, new_id)) {
                        text_stream_map_.clear();
                        text_stream_map_[config_itr->first] = text_stream;
                    } else {
                        success &= false;
                        MEDIA_LOG(ERROR, media_log_)
                            << "Error remapping single text track number";
                    }
                }
            }
        } else {
            for (TextConfigItr config_itr = text_configs.begin();
                 config_itr != text_configs.end(); ++config_itr) {
                TextStreamMap::iterator stream_itr = text_stream_map_.find(config_itr->first);
                if (stream_itr == text_stream_map_.end()) {
                    success &= false;
                    MEDIA_LOG(ERROR, media_log_)
                        << "Unexpected text track configuration for track ID "
                        << config_itr->first;
                    break;
                }

                const TextTrackConfig& new_config = config_itr->second;
                ChunkDemuxerStream* stream = stream_itr->second;
                TextTrackConfig old_config = stream->text_track_config();
                if (!new_config.Matches(old_config)) {
                    success &= false;
                    MEDIA_LOG(ERROR, media_log_) << "New text track config for track ID "
                                                 << config_itr->first
                                                 << " does not match old one.";
                    break;
                }
            }
        }
    }

    frame_processor_->SetAllTrackBuffersNeedRandomAccessPoint();

    DVLOG(1) << "OnNewConfigs() : " << (success ? "success" : "failed");
    if (success)
        init_segment_received_cb_.Run();

    return success;
}

void SourceState::OnNewMediaSegment()
{
    DVLOG(2) << "OnNewMediaSegment()";
    parsing_media_segment_ = true;
}

void SourceState::OnEndOfMediaSegment()
{
    DVLOG(2) << "OnEndOfMediaSegment()";
    parsing_media_segment_ = false;
}

bool SourceState::OnNewBuffers(
    const StreamParser::BufferQueue& audio_buffers,
    const StreamParser::BufferQueue& video_buffers,
    const StreamParser::TextBufferQueueMap& text_map)
{
    DVLOG(2) << "OnNewBuffers()";
    DCHECK(timestamp_offset_during_append_);
    DCHECK(parsing_media_segment_);

    const TimeDelta timestamp_offset_before_processing = *timestamp_offset_during_append_;

    // Calculate the new timestamp offset for audio/video tracks if the stream
    // parser has requested automatic updates.
    TimeDelta new_timestamp_offset = timestamp_offset_before_processing;
    if (auto_update_timestamp_offset_) {
        const bool have_audio_buffers = !audio_buffers.empty();
        const bool have_video_buffers = !video_buffers.empty();
        if (have_audio_buffers && have_video_buffers) {
            new_timestamp_offset += std::min(EndTimestamp(audio_buffers), EndTimestamp(video_buffers));
        } else if (have_audio_buffers) {
            new_timestamp_offset += EndTimestamp(audio_buffers);
        } else if (have_video_buffers) {
            new_timestamp_offset += EndTimestamp(video_buffers);
        }
    }

    if (!frame_processor_->ProcessFrames(audio_buffers,
            video_buffers,
            text_map,
            append_window_start_during_append_,
            append_window_end_during_append_,
            //&new_media_segment_,
            timestamp_offset_during_append_)) {
        return false;
    }

    // Only update the timestamp offset if the frame processor hasn't already.
    if (auto_update_timestamp_offset_ && timestamp_offset_before_processing == *timestamp_offset_during_append_) {
        *timestamp_offset_during_append_ = new_timestamp_offset;
    }

    return true;
}

void SourceState::OnSourceInitDone(const StreamParser::InitParameters& params)
{
    auto_update_timestamp_offset_ = params.auto_update_timestamp_offset;
    base::ResetAndReturn(&init_cb_).Run(params);
}

ChunkDemuxerStream::ChunkDemuxerStream(Type type,
    bool splice_frames_enabled)
    : type_(type)
    , liveness_(DemuxerStream::LIVENESS_UNKNOWN)
    , state_(UNINITIALIZED)
    , splice_frames_enabled_(splice_frames_enabled)
    , partial_append_window_trimming_enabled_(false)
{
}

void ChunkDemuxerStream::StartReturningData()
{
    DVLOG(1) << "ChunkDemuxerStream::StartReturningData()";
    base::AutoLock auto_lock(lock_);
    DCHECK(read_cb_.is_null());
    ChangeState_Locked(RETURNING_DATA_FOR_READS);
}

void ChunkDemuxerStream::AbortReads()
{
    DVLOG(1) << "ChunkDemuxerStream::AbortReads()";
    base::AutoLock auto_lock(lock_);
    ChangeState_Locked(RETURNING_ABORT_FOR_READS);
    if (!read_cb_.is_null())
        base::ResetAndReturn(&read_cb_).Run(kAborted, NULL);
}

void ChunkDemuxerStream::CompletePendingReadIfPossible()
{
    base::AutoLock auto_lock(lock_);
    if (read_cb_.is_null())
        return;

    CompletePendingReadIfPossible_Locked();
}

void ChunkDemuxerStream::Shutdown()
{
    DVLOG(1) << "ChunkDemuxerStream::Shutdown()";
    base::AutoLock auto_lock(lock_);
    ChangeState_Locked(SHUTDOWN);

    // Pass an end of stream buffer to the pending callback to signal that no more
    // data will be sent.
    if (!read_cb_.is_null()) {
        base::ResetAndReturn(&read_cb_).Run(DemuxerStream::kOk,
            StreamParserBuffer::CreateEOSBuffer());
    }
}

bool ChunkDemuxerStream::IsSeekWaitingForData() const
{
    base::AutoLock auto_lock(lock_);

    // This method should not be called for text tracks. See the note in
    // SourceState::IsSeekWaitingForData().
    DCHECK_NE(type_, DemuxerStream::TEXT);

    return stream_->IsSeekPending();
}

void ChunkDemuxerStream::Seek(TimeDelta time)
{
    DVLOG(1) << "ChunkDemuxerStream::Seek(" << time.InSecondsF() << ")";
    base::AutoLock auto_lock(lock_);
    DCHECK(read_cb_.is_null());
    DCHECK(state_ == UNINITIALIZED || state_ == RETURNING_ABORT_FOR_READS)
        << state_;

    stream_->Seek(time);
}

bool ChunkDemuxerStream::Append(const StreamParser::BufferQueue& buffers)
{
    if (buffers.empty())
        return false;

    base::AutoLock auto_lock(lock_);
    DCHECK_NE(state_, SHUTDOWN);
    if (!stream_->Append(buffers)) {
        DVLOG(1) << "ChunkDemuxerStream::Append() : stream append failed";
        return false;
    }

    if (!read_cb_.is_null())
        CompletePendingReadIfPossible_Locked();

    return true;
}

void ChunkDemuxerStream::Remove(TimeDelta start, TimeDelta end,
    TimeDelta duration)
{
    base::AutoLock auto_lock(lock_);
    stream_->Remove(start, end, duration);
}

bool ChunkDemuxerStream::EvictCodedFrames(DecodeTimestamp media_time,
    size_t newDataSize)
{
    base::AutoLock auto_lock(lock_);
    return stream_->GarbageCollectIfNeeded(media_time, newDataSize);
}

void ChunkDemuxerStream::OnSetDuration(TimeDelta duration)
{
    base::AutoLock auto_lock(lock_);
    stream_->OnSetDuration(duration);
}

Ranges<TimeDelta> ChunkDemuxerStream::GetBufferedRanges(
    TimeDelta duration) const
{
    base::AutoLock auto_lock(lock_);

    if (type_ == TEXT) {
        // Since text tracks are discontinuous and the lack of cues should not block
        // playback, report the buffered range for text tracks as [0, |duration|) so
        // that intesections with audio & video tracks are computed correctly when
        // no cues are present.
        Ranges<TimeDelta> text_range;
        text_range.Add(TimeDelta(), duration);
        return text_range;
    }

    Ranges<TimeDelta> range = stream_->GetBufferedTime();

    if (range.size() == 0u)
        return range;

    // Clamp the end of the stream's buffered ranges to fit within the duration.
    // This can be done by intersecting the stream's range with the valid time
    // range.
    Ranges<TimeDelta> valid_time_range;
    valid_time_range.Add(range.start(0), duration);
    return range.IntersectionWith(valid_time_range);
}

TimeDelta ChunkDemuxerStream::GetBufferedDuration() const
{
    return stream_->GetBufferedDuration();
}

size_t ChunkDemuxerStream::GetBufferedSize() const
{
    return stream_->GetBufferedSize();
}

void ChunkDemuxerStream::OnStartOfCodedFrameGroup(DecodeTimestamp start_timestamp)
{
    DVLOG(2) << "ChunkDemuxerStream::OnStartOfCodedFrameGroup("
             << start_timestamp.InSecondsF() << ")";
    base::AutoLock auto_lock(lock_);
    stream_->OnStartOfCodedFrameGroup(start_timestamp);
}

bool ChunkDemuxerStream::UpdateAudioConfig(
    const AudioDecoderConfig& config,
    const scoped_refptr<MediaLog>& media_log)
{
    DCHECK(config.IsValidConfig());
    DCHECK_EQ(type_, AUDIO);
    base::AutoLock auto_lock(lock_);
    if (!stream_) {
        DCHECK_EQ(state_, UNINITIALIZED);

        // On platforms which support splice frames, enable splice frames and
        // partial append window support for most codecs (notably: not opus).
        const bool codec_supported = config.codec() == kCodecMP3 || config.codec() == kCodecAAC || config.codec() == kCodecVorbis;
        splice_frames_enabled_ = splice_frames_enabled_ && codec_supported;
        partial_append_window_trimming_enabled_ = splice_frames_enabled_ && codec_supported;

        stream_.reset(
            new SourceBufferStream(config, media_log, splice_frames_enabled_));
        return true;
    }

    return stream_->UpdateAudioConfig(config);
}

bool ChunkDemuxerStream::UpdateVideoConfig(
    const VideoDecoderConfig& config,
    const scoped_refptr<MediaLog>& media_log)
{
    DCHECK(config.IsValidConfig());
    DCHECK_EQ(type_, VIDEO);
    base::AutoLock auto_lock(lock_);

    if (!stream_) {
        DCHECK_EQ(state_, UNINITIALIZED);
        stream_.reset(
            new SourceBufferStream(config, media_log, splice_frames_enabled_));
        return true;
    }

    return stream_->UpdateVideoConfig(config);
}

void ChunkDemuxerStream::UpdateTextConfig(
    const TextTrackConfig& config,
    const scoped_refptr<MediaLog>& media_log)
{
    DCHECK_EQ(type_, TEXT);
    base::AutoLock auto_lock(lock_);
    DCHECK(!stream_);
    DCHECK_EQ(state_, UNINITIALIZED);
    stream_.reset(
        new SourceBufferStream(config, media_log, splice_frames_enabled_));
}

void ChunkDemuxerStream::MarkEndOfStream()
{
    base::AutoLock auto_lock(lock_);
    stream_->MarkEndOfStream();
}

void ChunkDemuxerStream::UnmarkEndOfStream()
{
    base::AutoLock auto_lock(lock_);
    stream_->UnmarkEndOfStream();
}

// DemuxerStream methods.
void ChunkDemuxerStream::Read(const ReadCB& read_cb)
{
    base::AutoLock auto_lock(lock_);
    DCHECK_NE(state_, UNINITIALIZED);
    DCHECK(read_cb_.is_null());

    read_cb_ = BindToCurrentLoop(read_cb);
    CompletePendingReadIfPossible_Locked();
}

DemuxerStream::Type ChunkDemuxerStream::type() const { return type_; }

DemuxerStream::Liveness ChunkDemuxerStream::liveness() const
{
    base::AutoLock auto_lock(lock_);
    return liveness_;
}

AudioDecoderConfig ChunkDemuxerStream::audio_decoder_config()
{
    CHECK_EQ(type_, AUDIO);
    base::AutoLock auto_lock(lock_);
    return stream_->GetCurrentAudioDecoderConfig();
}

VideoDecoderConfig ChunkDemuxerStream::video_decoder_config()
{
    CHECK_EQ(type_, VIDEO);
    base::AutoLock auto_lock(lock_);
    return stream_->GetCurrentVideoDecoderConfig();
}

bool ChunkDemuxerStream::SupportsConfigChanges() { return true; }

VideoRotation ChunkDemuxerStream::video_rotation()
{
    return VIDEO_ROTATION_0;
}

TextTrackConfig ChunkDemuxerStream::text_track_config()
{
    CHECK_EQ(type_, TEXT);
    base::AutoLock auto_lock(lock_);
    return stream_->GetCurrentTextTrackConfig();
}

void ChunkDemuxerStream::SetStreamMemoryLimit(size_t memory_limit)
{
    stream_->set_memory_limit(memory_limit);
}

void ChunkDemuxerStream::SetLiveness(Liveness liveness)
{
    base::AutoLock auto_lock(lock_);
    liveness_ = liveness;
}

void ChunkDemuxerStream::ChangeState_Locked(State state)
{
    lock_.AssertAcquired();
    DVLOG(1) << "ChunkDemuxerStream::ChangeState_Locked() : "
             << "type " << type_
             << " - " << state_ << " -> " << state;
    state_ = state;
}

ChunkDemuxerStream::~ChunkDemuxerStream() { }

void ChunkDemuxerStream::CompletePendingReadIfPossible_Locked()
{
    lock_.AssertAcquired();
    DCHECK(!read_cb_.is_null());

    DemuxerStream::Status status;
    scoped_refptr<StreamParserBuffer> buffer;

    switch (state_) {
    case UNINITIALIZED:
        NOTREACHED();
        return;
    case RETURNING_DATA_FOR_READS:
        switch (stream_->GetNextBuffer(&buffer)) {
        case SourceBufferStream::kSuccess:
            status = DemuxerStream::kOk;
            DVLOG(2) << __FUNCTION__ << ": returning kOk, type " << type_
                     << ", dts " << buffer->GetDecodeTimestamp().InSecondsF()
                     << ", pts " << buffer->timestamp().InSecondsF()
                     << ", dur " << buffer->duration().InSecondsF()
                     << ", key " << buffer->is_key_frame();
            break;
        case SourceBufferStream::kNeedBuffer:
            // Return early without calling |read_cb_| since we don't have
            // any data to return yet.
            DVLOG(2) << __FUNCTION__ << ": returning kNeedBuffer, type "
                     << type_;
            return;
        case SourceBufferStream::kEndOfStream:
            status = DemuxerStream::kOk;
            buffer = StreamParserBuffer::CreateEOSBuffer();
            DVLOG(2) << __FUNCTION__ << ": returning kOk with EOS buffer, type "
                     << type_;
            break;
        case SourceBufferStream::kConfigChange:
            status = kConfigChanged;
            buffer = NULL;
            DVLOG(2) << __FUNCTION__ << ": returning kConfigChange, type "
                     << type_;
            break;
        }
        break;
    case RETURNING_ABORT_FOR_READS:
        // Null buffers should be returned in this state since we are waiting
        // for a seek. Any buffers in the SourceBuffer should NOT be returned
        // because they are associated with the seek.
        status = DemuxerStream::kAborted;
        buffer = NULL;
        DVLOG(2) << __FUNCTION__ << ": returning kAborted, type " << type_;
        break;
    case SHUTDOWN:
        status = DemuxerStream::kOk;
        buffer = StreamParserBuffer::CreateEOSBuffer();
        DVLOG(2) << __FUNCTION__ << ": returning kOk with EOS buffer, type "
                 << type_;
        break;
    }

    base::ResetAndReturn(&read_cb_).Run(status, buffer);
}

ChunkDemuxer::ChunkDemuxer(
    const base::Closure& open_cb,
    const EncryptedMediaInitDataCB& encrypted_media_init_data_cb,
    const scoped_refptr<MediaLog>& media_log,
    bool splice_frames_enabled)
    : state_(WAITING_FOR_INIT)
    , cancel_next_seek_(false)
    , host_(NULL)
    , open_cb_(open_cb)
    , encrypted_media_init_data_cb_(encrypted_media_init_data_cb)
    , enable_text_(false)
    , media_log_(media_log)
    , duration_(kNoTimestamp())
    , user_specified_duration_(-1)
    , liveness_(DemuxerStream::LIVENESS_UNKNOWN)
    , splice_frames_enabled_(splice_frames_enabled)
{
    DCHECK(!open_cb_.is_null());
    DCHECK(!encrypted_media_init_data_cb_.is_null());
}

std::string ChunkDemuxer::GetDisplayName() const
{
    return "ChunkDemuxer";
}

void ChunkDemuxer::Initialize(
    DemuxerHost* host,
    const PipelineStatusCB& cb,
    bool enable_text_tracks)
{
    DVLOG(1) << "Init()";

    base::AutoLock auto_lock(lock_);

    // The |init_cb_| must only be run after this method returns, so always post.
    init_cb_ = BindToCurrentLoop(cb);
    if (state_ == SHUTDOWN) {
        base::ResetAndReturn(&init_cb_).Run(DEMUXER_ERROR_COULD_NOT_OPEN);
        return;
    }
    DCHECK_EQ(state_, WAITING_FOR_INIT);
    host_ = host;
    enable_text_ = enable_text_tracks;

    ChangeState_Locked(INITIALIZING);

    base::ResetAndReturn(&open_cb_).Run();
}

void ChunkDemuxer::Stop()
{
    DVLOG(1) << "Stop()";
    Shutdown();
}

void ChunkDemuxer::Seek(TimeDelta time, const PipelineStatusCB& cb)
{
    DVLOG(1) << "Seek(" << time.InSecondsF() << ")";
    DCHECK(time >= TimeDelta());

    base::AutoLock auto_lock(lock_);
    DCHECK(seek_cb_.is_null());

    seek_cb_ = BindToCurrentLoop(cb);
    if (state_ != INITIALIZED && state_ != ENDED) {
        base::ResetAndReturn(&seek_cb_).Run(PIPELINE_ERROR_INVALID_STATE);
        return;
    }

    if (cancel_next_seek_) {
        cancel_next_seek_ = false;
        base::ResetAndReturn(&seek_cb_).Run(PIPELINE_OK);
        return;
    }

    SeekAllSources(time);
    StartReturningData();

    if (IsSeekWaitingForData_Locked()) {
        DVLOG(1) << "Seek() : waiting for more data to arrive.";
        return;
    }

    base::ResetAndReturn(&seek_cb_).Run(PIPELINE_OK);
}

// Demuxer implementation.
base::Time ChunkDemuxer::GetTimelineOffset() const
{
    return timeline_offset_;
}

DemuxerStream* ChunkDemuxer::GetStream(DemuxerStream::Type type)
{
    DCHECK_NE(type, DemuxerStream::TEXT);
    base::AutoLock auto_lock(lock_);
    if (type == DemuxerStream::VIDEO)
        return video_.get();

    if (type == DemuxerStream::AUDIO)
        return audio_.get();

    return NULL;
}

TimeDelta ChunkDemuxer::GetStartTime() const
{
    return TimeDelta();
}

int64_t ChunkDemuxer::GetMemoryUsage() const
{
    base::AutoLock auto_lock(lock_);
    return (audio_ ? audio_->GetBufferedSize() : 0) + (video_ ? video_->GetBufferedSize() : 0);
}

void ChunkDemuxer::StartWaitingForSeek(TimeDelta seek_time)
{
    DVLOG(1) << "StartWaitingForSeek()";
    base::AutoLock auto_lock(lock_);
    DCHECK(state_ == INITIALIZED || state_ == ENDED || state_ == SHUTDOWN || state_ == PARSE_ERROR) << state_;
    DCHECK(seek_cb_.is_null());

    if (state_ == SHUTDOWN || state_ == PARSE_ERROR)
        return;

    AbortPendingReads();
    SeekAllSources(seek_time);

    // Cancel state set in CancelPendingSeek() since we want to
    // accept the next Seek().
    cancel_next_seek_ = false;
}

void ChunkDemuxer::CancelPendingSeek(TimeDelta seek_time)
{
    base::AutoLock auto_lock(lock_);
    DCHECK_NE(state_, INITIALIZING);
    DCHECK(seek_cb_.is_null() || IsSeekWaitingForData_Locked());

    if (cancel_next_seek_)
        return;

    AbortPendingReads();
    SeekAllSources(seek_time);

    if (seek_cb_.is_null()) {
        cancel_next_seek_ = true;
        return;
    }

    base::ResetAndReturn(&seek_cb_).Run(PIPELINE_OK);
}

ChunkDemuxer::Status ChunkDemuxer::AddId(const std::string& id,
    const std::string& type,
    std::vector<std::string>& codecs)
{
    base::AutoLock auto_lock(lock_);

    if ((state_ != WAITING_FOR_INIT && state_ != INITIALIZING) || IsValidId(id))
        return kReachedIdLimit;

    bool has_audio = false;
    bool has_video = false;
    scoped_ptr<media::StreamParser> stream_parser(StreamParserFactory::Create(
        type, codecs, media_log_, &has_audio, &has_video));

    if (!stream_parser)
        return ChunkDemuxer::kNotSupported;

    if ((has_audio && !source_id_audio_.empty()) || (has_video && !source_id_video_.empty()))
        return kReachedIdLimit;

    if (has_audio)
        source_id_audio_ = id;

    if (has_video)
        source_id_video_ = id;

    scoped_ptr<FrameProcessor> frame_processor(
        new FrameProcessor(base::Bind(&ChunkDemuxer::IncreaseDurationIfNecessary,
                               base::Unretained(this)),
            media_log_));

    scoped_ptr<SourceState> source_state(new SourceState(
        stream_parser.Pass(), frame_processor.Pass(),
        base::Bind(&ChunkDemuxer::CreateDemuxerStream, base::Unretained(this)),
        media_log_));

    SourceState::NewTextTrackCB new_text_track_cb;

    if (enable_text_) {
        new_text_track_cb = base::Bind(&ChunkDemuxer::OnNewTextTrack,
            base::Unretained(this));
    }

    source_state->Init(
        base::Bind(&ChunkDemuxer::OnSourceInitDone, base::Unretained(this)),
        has_audio, has_video, encrypted_media_init_data_cb_, new_text_track_cb);

    source_state_map_[id] = source_state.release();
    return kOk;
}

void ChunkDemuxer::RemoveId(const std::string& id)
{
    base::AutoLock auto_lock(lock_);
    CHECK(IsValidId(id));

    delete source_state_map_[id];
    source_state_map_.erase(id);

    if (source_id_audio_ == id)
        source_id_audio_ = "";

    if (source_id_video_ == id)
        source_id_video_ = "";
}

Ranges<TimeDelta> ChunkDemuxer::GetBufferedRanges(const std::string& id) const
{
    base::AutoLock auto_lock(lock_);
    DCHECK(!id.empty());

    SourceStateMap::const_iterator itr = source_state_map_.find(id);

    DCHECK(itr != source_state_map_.end());
    return itr->second->GetBufferedRanges(duration_, state_ == ENDED);
}

bool ChunkDemuxer::EvictCodedFrames(const std::string& id,
    base::TimeDelta currentMediaTime,
    size_t newDataSize)
{
    DVLOG(1) << __FUNCTION__ << "(" << id << ")"
             << " media_time=" << currentMediaTime.InSecondsF()
             << " newDataSize=" << newDataSize;
    base::AutoLock auto_lock(lock_);

    // Note: The direct conversion from PTS to DTS is safe here, since we don't
    // need to know currentTime precisely for GC. GC only needs to know which GOP
    // currentTime points to.
    DecodeTimestamp media_time_dts = DecodeTimestamp::FromPresentationTime(currentMediaTime);

    DCHECK(!id.empty());
    SourceStateMap::const_iterator itr = source_state_map_.find(id);
    if (itr == source_state_map_.end()) {
        LOG(WARNING) << __FUNCTION__ << " stream " << id << " not found";
        return false;
    }
    return itr->second->EvictCodedFrames(media_time_dts, newDataSize);
}

void ChunkDemuxer::AppendData(
    const std::string& id,
    const uint8* data,
    size_t length,
    TimeDelta append_window_start,
    TimeDelta append_window_end,
    TimeDelta* timestamp_offset,
    const InitSegmentReceivedCB& init_segment_received_cb)
{
    DVLOG(1) << "AppendData(" << id << ", " << length << ")";

    DCHECK(!id.empty());
    DCHECK(timestamp_offset);
    DCHECK(!init_segment_received_cb.is_null());

    Ranges<TimeDelta> ranges;

    {
        base::AutoLock auto_lock(lock_);
        DCHECK_NE(state_, ENDED);

        // Capture if any of the SourceBuffers are waiting for data before we start
        // parsing.
        bool old_waiting_for_data = IsSeekWaitingForData_Locked();

        if (length == 0u)
            return;

        DCHECK(data);

        switch (state_) {
        case INITIALIZING:
        case INITIALIZED:
            DCHECK(IsValidId(id));
            if (!source_state_map_[id]->Append(data, length,
                    append_window_start,
                    append_window_end,
                    timestamp_offset,
                    init_segment_received_cb)) {
                ReportError_Locked(PIPELINE_ERROR_DECODE);
                return;
            }
            break;

        case PARSE_ERROR:
            DVLOG(1) << "AppendData(): Ignoring data after a parse error.";
            return;

        case WAITING_FOR_INIT:
        case ENDED:
        case SHUTDOWN:
            DVLOG(1) << "AppendData(): called in unexpected state " << state_;
            return;
        }

        // Check to see if data was appended at the pending seek point. This
        // indicates we have parsed enough data to complete the seek.
        if (old_waiting_for_data && !IsSeekWaitingForData_Locked() && !seek_cb_.is_null()) {
            base::ResetAndReturn(&seek_cb_).Run(PIPELINE_OK);
        }

        ranges = GetBufferedRanges_Locked();
    }

    for (size_t i = 0; i < ranges.size(); ++i)
        host_->AddBufferedTimeRange(ranges.start(i), ranges.end(i));
}

void ChunkDemuxer::ResetParserState(const std::string& id,
    TimeDelta append_window_start,
    TimeDelta append_window_end,
    TimeDelta* timestamp_offset)
{
    DVLOG(1) << "ResetParserState(" << id << ")";
    base::AutoLock auto_lock(lock_);
    DCHECK(!id.empty());
    CHECK(IsValidId(id));
    bool old_waiting_for_data = IsSeekWaitingForData_Locked();
    source_state_map_[id]->ResetParserState(append_window_start,
        append_window_end,
        timestamp_offset);
    // ResetParserState can possibly emit some buffers.
    // Need to check whether seeking can be completed.
    if (old_waiting_for_data && !IsSeekWaitingForData_Locked() && !seek_cb_.is_null()) {
        base::ResetAndReturn(&seek_cb_).Run(PIPELINE_OK);
    }
}

void ChunkDemuxer::Remove(const std::string& id, TimeDelta start,
    TimeDelta end)
{
    DVLOG(1) << "Remove(" << id << ", " << start.InSecondsF()
             << ", " << end.InSecondsF() << ")";
    base::AutoLock auto_lock(lock_);

    DCHECK(!id.empty());
    CHECK(IsValidId(id));
    DCHECK(start >= base::TimeDelta()) << start.InSecondsF();
    DCHECK(start < end) << "start " << start.InSecondsF()
                        << " end " << end.InSecondsF();
    DCHECK(duration_ != kNoTimestamp());
    DCHECK(start <= duration_) << "start " << start.InSecondsF()
                               << " duration " << duration_.InSecondsF();

    if (start == duration_)
        return;

    source_state_map_[id]->Remove(start, end, duration_);
}

double ChunkDemuxer::GetDuration()
{
    base::AutoLock auto_lock(lock_);
    return GetDuration_Locked();
}

double ChunkDemuxer::GetDuration_Locked()
{
    lock_.AssertAcquired();
    if (duration_ == kNoTimestamp())
        return std::numeric_limits<double>::quiet_NaN();

    // Return positive infinity if the resource is unbounded.
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/video.html#dom-media-duration
    if (duration_ == kInfiniteDuration())
        return std::numeric_limits<double>::infinity();

    if (user_specified_duration_ >= 0)
        return user_specified_duration_;

    return duration_.InSecondsF();
}

void ChunkDemuxer::SetDuration(double duration)
{
    base::AutoLock auto_lock(lock_);
    DVLOG(1) << "SetDuration(" << duration << ")";
    DCHECK_GE(duration, 0);

    if (duration == GetDuration_Locked())
        return;

    // Compute & bounds check the TimeDelta representation of duration.
    // This can be different if the value of |duration| doesn't fit the range or
    // precision of TimeDelta.
    TimeDelta min_duration = TimeDelta::FromInternalValue(1);
    // Don't use TimeDelta::Max() here, as we want the largest finite time delta.
    TimeDelta max_duration = TimeDelta::FromInternalValue(kint64max - 1);
    double min_duration_in_seconds = min_duration.InSecondsF();
    double max_duration_in_seconds = max_duration.InSecondsF();

    TimeDelta duration_td;
    if (duration == std::numeric_limits<double>::infinity()) {
        duration_td = media::kInfiniteDuration();
    } else if (duration < min_duration_in_seconds) {
        duration_td = min_duration;
    } else if (duration > max_duration_in_seconds) {
        duration_td = max_duration;
    } else {
        duration_td = TimeDelta::FromMicroseconds(
            duration * base::Time::kMicrosecondsPerSecond);
    }

    DCHECK(duration_td > TimeDelta());

    user_specified_duration_ = duration;
    duration_ = duration_td;
    host_->SetDuration(duration_);

    for (SourceStateMap::iterator itr = source_state_map_.begin();
         itr != source_state_map_.end(); ++itr) {
        itr->second->OnSetDuration(duration_);
    }
}

bool ChunkDemuxer::IsParsingMediaSegment(const std::string& id)
{
    base::AutoLock auto_lock(lock_);
    DVLOG(1) << "IsParsingMediaSegment(" << id << ")";
    CHECK(IsValidId(id));

    return source_state_map_[id]->parsing_media_segment();
}

void ChunkDemuxer::SetSequenceMode(const std::string& id,
    bool sequence_mode)
{
    base::AutoLock auto_lock(lock_);
    DVLOG(1) << "SetSequenceMode(" << id << ", " << sequence_mode << ")";
    CHECK(IsValidId(id));
    DCHECK_NE(state_, ENDED);

    source_state_map_[id]->SetSequenceMode(sequence_mode);
}

void ChunkDemuxer::SetGroupStartTimestampIfInSequenceMode(
    const std::string& id,
    base::TimeDelta timestamp_offset)
{
    base::AutoLock auto_lock(lock_);
    DVLOG(1) << "SetGroupStartTimestampIfInSequenceMode(" << id << ", "
             << timestamp_offset.InSecondsF() << ")";
    CHECK(IsValidId(id));
    DCHECK_NE(state_, ENDED);

    source_state_map_[id]->SetGroupStartTimestampIfInSequenceMode(
        timestamp_offset);
}

void ChunkDemuxer::MarkEndOfStream(PipelineStatus status)
{
    DVLOG(1) << "MarkEndOfStream(" << status << ")";
    base::AutoLock auto_lock(lock_);
    DCHECK_NE(state_, WAITING_FOR_INIT);
    DCHECK_NE(state_, ENDED);

    if (state_ == SHUTDOWN || state_ == PARSE_ERROR)
        return;

    if (state_ == INITIALIZING) {
        ReportError_Locked(DEMUXER_ERROR_COULD_NOT_OPEN);
        return;
    }

    bool old_waiting_for_data = IsSeekWaitingForData_Locked();
    for (SourceStateMap::iterator itr = source_state_map_.begin();
         itr != source_state_map_.end(); ++itr) {
        itr->second->MarkEndOfStream();
    }

    CompletePendingReadsIfPossible();

    // Give a chance to resume the pending seek process.
    if (status != PIPELINE_OK) {
        ReportError_Locked(status);
        return;
    }

    ChangeState_Locked(ENDED);
    DecreaseDurationIfNecessary();

    if (old_waiting_for_data && !IsSeekWaitingForData_Locked() && !seek_cb_.is_null()) {
        base::ResetAndReturn(&seek_cb_).Run(PIPELINE_OK);
    }
}

void ChunkDemuxer::UnmarkEndOfStream()
{
    DVLOG(1) << "UnmarkEndOfStream()";
    base::AutoLock auto_lock(lock_);
    DCHECK_EQ(state_, ENDED);

    ChangeState_Locked(INITIALIZED);

    for (SourceStateMap::iterator itr = source_state_map_.begin();
         itr != source_state_map_.end(); ++itr) {
        itr->second->UnmarkEndOfStream();
    }
}

void ChunkDemuxer::Shutdown()
{
    DVLOG(1) << "Shutdown()";
    base::AutoLock auto_lock(lock_);

    if (state_ == SHUTDOWN)
        return;

    ShutdownAllStreams();

    ChangeState_Locked(SHUTDOWN);

    if (!seek_cb_.is_null())
        base::ResetAndReturn(&seek_cb_).Run(PIPELINE_ERROR_ABORT);
}

void ChunkDemuxer::SetMemoryLimits(DemuxerStream::Type type,
    size_t memory_limit)
{
    for (SourceStateMap::iterator itr = source_state_map_.begin();
         itr != source_state_map_.end(); ++itr) {
        itr->second->SetMemoryLimits(type, memory_limit);
    }
}

void ChunkDemuxer::ChangeState_Locked(State new_state)
{
    lock_.AssertAcquired();
    DVLOG(1) << "ChunkDemuxer::ChangeState_Locked() : "
             << state_ << " -> " << new_state;
    state_ = new_state;
}

ChunkDemuxer::~ChunkDemuxer()
{
    DCHECK_NE(state_, INITIALIZED);

    STLDeleteValues(&source_state_map_);
}

void ChunkDemuxer::ReportError_Locked(PipelineStatus error)
{
    DVLOG(1) << "ReportError_Locked(" << error << ")";
    lock_.AssertAcquired();
    DCHECK_NE(error, PIPELINE_OK);

    ChangeState_Locked(PARSE_ERROR);

    PipelineStatusCB cb;

    if (!init_cb_.is_null()) {
        std::swap(cb, init_cb_);
    } else {
        if (!seek_cb_.is_null())
            std::swap(cb, seek_cb_);

        ShutdownAllStreams();
    }

    if (!cb.is_null()) {
        cb.Run(error);
        return;
    }

    base::AutoUnlock auto_unlock(lock_);
    host_->OnDemuxerError(error);
}

bool ChunkDemuxer::IsSeekWaitingForData_Locked() const
{
    lock_.AssertAcquired();
    for (SourceStateMap::const_iterator itr = source_state_map_.begin();
         itr != source_state_map_.end(); ++itr) {
        if (itr->second->IsSeekWaitingForData())
            return true;
    }

    return false;
}

void ChunkDemuxer::OnSourceInitDone(
    const StreamParser::InitParameters& params)
{
    DVLOG(1) << "OnSourceInitDone(" << params.duration.InSecondsF() << ")";
    lock_.AssertAcquired();
    DCHECK_EQ(state_, INITIALIZING);
    if (!audio_ && !video_) {
        ReportError_Locked(DEMUXER_ERROR_COULD_NOT_OPEN);
        return;
    }

    if (params.duration != TimeDelta() && duration_ == kNoTimestamp())
        UpdateDuration(params.duration);

    if (!params.timeline_offset.is_null()) {
        if (!timeline_offset_.is_null() && params.timeline_offset != timeline_offset_) {
            MEDIA_LOG(ERROR, media_log_)
                << "Timeline offset is not the same across all SourceBuffers.";
            ReportError_Locked(DEMUXER_ERROR_COULD_NOT_OPEN);
            return;
        }

        timeline_offset_ = params.timeline_offset;
    }

    if (params.liveness != DemuxerStream::LIVENESS_UNKNOWN) {
        if (audio_)
            audio_->SetLiveness(params.liveness);
        if (video_)
            video_->SetLiveness(params.liveness);
    }

    // Wait until all streams have initialized.
    if ((!source_id_audio_.empty() && !audio_) || (!source_id_video_.empty() && !video_)) {
        return;
    }

    SeekAllSources(GetStartTime());
    StartReturningData();

    if (duration_ == kNoTimestamp())
        duration_ = kInfiniteDuration();

    // The demuxer is now initialized after the |start_timestamp_| was set.
    ChangeState_Locked(INITIALIZED);
    base::ResetAndReturn(&init_cb_).Run(PIPELINE_OK);
}

ChunkDemuxerStream*
ChunkDemuxer::CreateDemuxerStream(DemuxerStream::Type type)
{
    switch (type) {
    case DemuxerStream::AUDIO:
        if (audio_)
            return NULL;
        audio_.reset(
            new ChunkDemuxerStream(DemuxerStream::AUDIO, splice_frames_enabled_));
        return audio_.get();
        break;
    case DemuxerStream::VIDEO:
        if (video_)
            return NULL;
        video_.reset(
            new ChunkDemuxerStream(DemuxerStream::VIDEO, splice_frames_enabled_));
        return video_.get();
        break;
    case DemuxerStream::TEXT: {
        return new ChunkDemuxerStream(DemuxerStream::TEXT,
            splice_frames_enabled_);
        break;
    }
    case DemuxerStream::UNKNOWN:
    case DemuxerStream::NUM_TYPES:
        NOTREACHED();
        return NULL;
    }
    NOTREACHED();
    return NULL;
}

void ChunkDemuxer::OnNewTextTrack(ChunkDemuxerStream* text_stream,
    const TextTrackConfig& config)
{
    lock_.AssertAcquired();
    DCHECK_NE(state_, SHUTDOWN);
    host_->AddTextStream(text_stream, config);
}

bool ChunkDemuxer::IsValidId(const std::string& source_id) const
{
    lock_.AssertAcquired();
    return source_state_map_.count(source_id) > 0u;
}

void ChunkDemuxer::UpdateDuration(TimeDelta new_duration)
{
    DCHECK(duration_ != new_duration);
    user_specified_duration_ = -1;
    duration_ = new_duration;
    host_->SetDuration(new_duration);
}

void ChunkDemuxer::IncreaseDurationIfNecessary(TimeDelta new_duration)
{
    DCHECK(new_duration != kNoTimestamp());
    DCHECK(new_duration != kInfiniteDuration());

    // Per April 1, 2014 MSE spec editor's draft:
    // https://dvcs.w3.org/hg/html-media/raw-file/d471a4412040/media-source/
    //     media-source.html#sourcebuffer-coded-frame-processing
    // 5. If the media segment contains data beyond the current duration, then run
    //    the duration change algorithm with new duration set to the maximum of
    //    the current duration and the group end timestamp.

    if (new_duration <= duration_)
        return;

    DVLOG(2) << __FUNCTION__ << ": Increasing duration: "
             << duration_.InSecondsF() << " -> " << new_duration.InSecondsF();

    UpdateDuration(new_duration);
}

void ChunkDemuxer::DecreaseDurationIfNecessary()
{
    lock_.AssertAcquired();

    TimeDelta max_duration;

    for (SourceStateMap::const_iterator itr = source_state_map_.begin();
         itr != source_state_map_.end(); ++itr) {
        max_duration = std::max(max_duration,
            itr->second->GetMaxBufferedDuration());
    }

    if (max_duration == TimeDelta())
        return;

    if (max_duration < duration_)
        UpdateDuration(max_duration);
}

Ranges<TimeDelta> ChunkDemuxer::GetBufferedRanges() const
{
    base::AutoLock auto_lock(lock_);
    return GetBufferedRanges_Locked();
}

Ranges<TimeDelta> ChunkDemuxer::GetBufferedRanges_Locked() const
{
    lock_.AssertAcquired();

    bool ended = state_ == ENDED;
    // TODO(acolwell): When we start allowing SourceBuffers that are not active,
    // we'll need to update this loop to only add ranges from active sources.
    RangesList ranges_list;
    for (SourceStateMap::const_iterator itr = source_state_map_.begin();
         itr != source_state_map_.end(); ++itr) {
        ranges_list.push_back(itr->second->GetBufferedRanges(duration_, ended));
    }

    return ComputeIntersection(ranges_list, ended);
}

void ChunkDemuxer::StartReturningData()
{
    for (SourceStateMap::iterator itr = source_state_map_.begin();
         itr != source_state_map_.end(); ++itr) {
        itr->second->StartReturningData();
    }
}

void ChunkDemuxer::AbortPendingReads()
{
    for (SourceStateMap::iterator itr = source_state_map_.begin();
         itr != source_state_map_.end(); ++itr) {
        itr->second->AbortReads();
    }
}

void ChunkDemuxer::SeekAllSources(TimeDelta seek_time)
{
    for (SourceStateMap::iterator itr = source_state_map_.begin();
         itr != source_state_map_.end(); ++itr) {
        itr->second->Seek(seek_time);
    }
}

void ChunkDemuxer::CompletePendingReadsIfPossible()
{
    for (SourceStateMap::iterator itr = source_state_map_.begin();
         itr != source_state_map_.end(); ++itr) {
        itr->second->CompletePendingReadIfPossible();
    }
}

void ChunkDemuxer::ShutdownAllStreams()
{
    for (SourceStateMap::iterator itr = source_state_map_.begin();
         itr != source_state_map_.end(); ++itr) {
        itr->second->Shutdown();
    }
}

} // namespace media

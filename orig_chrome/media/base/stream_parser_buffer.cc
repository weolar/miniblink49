// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/stream_parser_buffer.h"

#include "base/logging.h"
#include "media/base/timestamp_constants.h"

namespace media {

static scoped_refptr<StreamParserBuffer> CopyBuffer(
    const StreamParserBuffer& buffer)
{
    if (buffer.end_of_stream())
        return StreamParserBuffer::CreateEOSBuffer();

    scoped_refptr<StreamParserBuffer> copied_buffer = StreamParserBuffer::CopyFrom(buffer.data(),
        buffer.data_size(),
        buffer.side_data(),
        buffer.side_data_size(),
        buffer.is_key_frame(),
        buffer.type(),
        buffer.track_id());
    copied_buffer->SetDecodeTimestamp(buffer.GetDecodeTimestamp());
    copied_buffer->SetConfigId(buffer.GetConfigId());
    copied_buffer->set_timestamp(buffer.timestamp());
    copied_buffer->set_duration(buffer.duration());
    copied_buffer->set_is_duration_estimated(buffer.is_duration_estimated());
    copied_buffer->set_discard_padding(buffer.discard_padding());
    copied_buffer->set_splice_timestamp(buffer.splice_timestamp());
    const DecryptConfig* decrypt_config = buffer.decrypt_config();
    if (decrypt_config) {
        copied_buffer->set_decrypt_config(
            make_scoped_ptr(new DecryptConfig(decrypt_config->key_id(),
                decrypt_config->iv(),
                decrypt_config->subsamples())));
    }

    return copied_buffer;
}

scoped_refptr<StreamParserBuffer> StreamParserBuffer::CreateEOSBuffer()
{
    return make_scoped_refptr(new StreamParserBuffer(NULL, 0, NULL, 0, false,
        DemuxerStream::UNKNOWN, 0));
}

scoped_refptr<StreamParserBuffer> StreamParserBuffer::CopyFrom(
    const uint8* data, int data_size, bool is_key_frame, Type type,
    TrackId track_id)
{
    return make_scoped_refptr(
        new StreamParserBuffer(data, data_size, NULL, 0, is_key_frame, type,
            track_id));
}

scoped_refptr<StreamParserBuffer> StreamParserBuffer::CopyFrom(
    const uint8* data, int data_size,
    const uint8* side_data, int side_data_size,
    bool is_key_frame, Type type, TrackId track_id)
{
    return make_scoped_refptr(
        new StreamParserBuffer(data, data_size, side_data, side_data_size,
            is_key_frame, type, track_id));
}

DecodeTimestamp StreamParserBuffer::GetDecodeTimestamp() const
{
    if (decode_timestamp_ == kNoDecodeTimestamp())
        return DecodeTimestamp::FromPresentationTime(timestamp());
    return decode_timestamp_;
}

void StreamParserBuffer::SetDecodeTimestamp(DecodeTimestamp timestamp)
{
    decode_timestamp_ = timestamp;
    if (preroll_buffer_.get())
        preroll_buffer_->SetDecodeTimestamp(timestamp);
}

StreamParserBuffer::StreamParserBuffer(const uint8* data,
    int data_size,
    const uint8* side_data,
    int side_data_size,
    bool is_key_frame,
    Type type,
    TrackId track_id)
    : DecoderBuffer(data, data_size, side_data, side_data_size)
    , decode_timestamp_(kNoDecodeTimestamp())
    , config_id_(kInvalidConfigId)
    , type_(type)
    , track_id_(track_id)
    , is_duration_estimated_(false)
{
    // TODO(scherkus): Should DataBuffer constructor accept a timestamp and
    // duration to force clients to set them? Today they end up being zero which
    // is both a common and valid value and could lead to bugs.
    if (data) {
        set_duration(kNoTimestamp());
    }

    if (is_key_frame)
        set_is_key_frame(true);
}

StreamParserBuffer::~StreamParserBuffer() { }

int StreamParserBuffer::GetConfigId() const
{
    return config_id_;
}

void StreamParserBuffer::SetConfigId(int config_id)
{
    config_id_ = config_id;
    if (preroll_buffer_.get())
        preroll_buffer_->SetConfigId(config_id);
}

int StreamParserBuffer::GetSpliceBufferConfigId(size_t index) const
{
    return index < splice_buffers().size()
        ? splice_buffers_[index]->GetConfigId()
        : GetConfigId();
}

const char* StreamParserBuffer::GetTypeName() const
{
    switch (type()) {
    case DemuxerStream::AUDIO:
        return "audio";
    case DemuxerStream::VIDEO:
        return "video";
    case DemuxerStream::TEXT:
        return "text";
    case DemuxerStream::UNKNOWN:
        return "unknown";
    case DemuxerStream::NUM_TYPES:
        // Fall-through to NOTREACHED().
        break;
    }
    NOTREACHED();
    return "";
}

void StreamParserBuffer::ConvertToSpliceBuffer(
    const BufferQueue& pre_splice_buffers)
{
    DCHECK(splice_buffers_.empty());
    DCHECK(duration() > base::TimeDelta())
        << "Only buffers with a valid duration can convert to a splice buffer."
        << " pts " << timestamp().InSecondsF()
        << " dts " << GetDecodeTimestamp().InSecondsF()
        << " dur " << duration().InSecondsF();
    DCHECK(!end_of_stream());

    // Splicing requires non-estimated sample accurate durations to be confident
    // things will sound smooth. Also, we cannot be certain whether estimated
    // overlap is really a splice scenario, or just over estimation.
    DCHECK(!is_duration_estimated_);

    // Make a copy of this first, before making any changes.
    scoped_refptr<StreamParserBuffer> overlapping_buffer = CopyBuffer(*this);
    overlapping_buffer->set_splice_timestamp(kNoTimestamp());

    const scoped_refptr<StreamParserBuffer>& first_splice_buffer = pre_splice_buffers.front();

    // Ensure the given buffers are actually before the splice point.
    DCHECK(first_splice_buffer->timestamp() <= overlapping_buffer->timestamp());

    // TODO(dalecurtis): We should also clear |data| and |side_data|, but since
    // that implies EOS care must be taken to ensure there are no clients relying
    // on that behavior.

    // Move over any preroll from this buffer.
    if (preroll_buffer_.get()) {
        DCHECK(!overlapping_buffer->preroll_buffer_.get());
        overlapping_buffer->preroll_buffer_.swap(preroll_buffer_);
    }

    // Rewrite |this| buffer as a splice buffer.
    SetDecodeTimestamp(first_splice_buffer->GetDecodeTimestamp());
    SetConfigId(first_splice_buffer->GetConfigId());
    set_timestamp(first_splice_buffer->timestamp());
    set_is_key_frame(first_splice_buffer->is_key_frame());
    type_ = first_splice_buffer->type();
    track_id_ = first_splice_buffer->track_id();
    set_splice_timestamp(overlapping_buffer->timestamp());

    // The splice duration is the duration of all buffers before the splice plus
    // the highest ending timestamp after the splice point.
    DCHECK(overlapping_buffer->duration() > base::TimeDelta());
    DCHECK(pre_splice_buffers.back()->duration() > base::TimeDelta());
    set_duration(
        std::max(overlapping_buffer->timestamp() + overlapping_buffer->duration(),
            pre_splice_buffers.back()->timestamp() + pre_splice_buffers.back()->duration())
        - first_splice_buffer->timestamp());

    // Copy all pre splice buffers into our wrapper buffer.
    for (BufferQueue::const_iterator it = pre_splice_buffers.begin();
         it != pre_splice_buffers.end();
         ++it) {
        const scoped_refptr<StreamParserBuffer>& buffer = *it;
        DCHECK(!buffer->end_of_stream());
        DCHECK(!buffer->preroll_buffer().get());
        DCHECK(buffer->splice_buffers().empty());
        DCHECK(!buffer->is_duration_estimated());
        splice_buffers_.push_back(CopyBuffer(*buffer.get()));
        splice_buffers_.back()->set_splice_timestamp(splice_timestamp());
    }

    splice_buffers_.push_back(overlapping_buffer);
}

void StreamParserBuffer::SetPrerollBuffer(
    const scoped_refptr<StreamParserBuffer>& preroll_buffer)
{
    DCHECK(!preroll_buffer_.get());
    DCHECK(!end_of_stream());
    DCHECK(!preroll_buffer->end_of_stream());
    DCHECK(!preroll_buffer->preroll_buffer_.get());
    DCHECK(preroll_buffer->splice_timestamp() == kNoTimestamp());
    DCHECK(preroll_buffer->splice_buffers().empty());
    DCHECK(preroll_buffer->timestamp() <= timestamp());
    DCHECK(preroll_buffer->discard_padding() == DecoderBuffer::DiscardPadding());
    DCHECK_EQ(preroll_buffer->type(), type());
    DCHECK_EQ(preroll_buffer->track_id(), track_id());

    preroll_buffer_ = preroll_buffer;
    preroll_buffer_->set_timestamp(timestamp());
    preroll_buffer_->SetDecodeTimestamp(GetDecodeTimestamp());

    // Mark the entire buffer for discard.
    preroll_buffer_->set_discard_padding(
        std::make_pair(kInfiniteDuration(), base::TimeDelta()));
}

void StreamParserBuffer::set_timestamp(base::TimeDelta timestamp)
{
    DecoderBuffer::set_timestamp(timestamp);
    if (preroll_buffer_.get())
        preroll_buffer_->set_timestamp(timestamp);
}

} // namespace media

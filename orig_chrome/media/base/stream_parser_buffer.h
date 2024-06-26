// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_STREAM_PARSER_BUFFER_H_
#define MEDIA_BASE_STREAM_PARSER_BUFFER_H_

#include <deque>

#include "media/base/decoder_buffer.h"
#include "media/base/demuxer_stream.h"
#include "media/base/media_export.h"
#include "media/base/stream_parser.h"
#include "media/base/timestamp_constants.h"

namespace media {

// Simple wrapper around base::TimeDelta that represents a decode timestamp.
// Making DecodeTimestamp a different type makes it easier to determine whether
// code is operating on presentation or decode timestamps and makes conversions
// between the two types explicit and easy to spot.
class DecodeTimestamp {
public:
    DecodeTimestamp() { }
    DecodeTimestamp(const DecodeTimestamp& rhs)
        : ts_(rhs.ts_)
    {
    }
    DecodeTimestamp& operator=(const DecodeTimestamp& rhs)
    {
        if (&rhs != this)
            ts_ = rhs.ts_;
        return *this;
    }

    // Only operators that are actually used by the code have been defined.
    // Reviewers should pay close attention to the addition of new operators.
    bool operator<(const DecodeTimestamp& rhs) const { return ts_ < rhs.ts_; }
    bool operator>(const DecodeTimestamp& rhs) const { return ts_ > rhs.ts_; }
    bool operator==(const DecodeTimestamp& rhs) const { return ts_ == rhs.ts_; }
    bool operator!=(const DecodeTimestamp& rhs) const { return ts_ != rhs.ts_; }
    bool operator>=(const DecodeTimestamp& rhs) const { return ts_ >= rhs.ts_; }
    bool operator<=(const DecodeTimestamp& rhs) const { return ts_ <= rhs.ts_; }

    base::TimeDelta operator-(const DecodeTimestamp& rhs) const
    {
        return ts_ - rhs.ts_;
    }

    DecodeTimestamp& operator+=(const base::TimeDelta& rhs)
    {
        ts_ += rhs;
        return *this;
    }

    DecodeTimestamp& operator-=(const base::TimeDelta& rhs)
    {
        ts_ -= rhs;
        return *this;
    }

    DecodeTimestamp operator+(const base::TimeDelta& rhs) const
    {
        return DecodeTimestamp(ts_ + rhs);
    }

    DecodeTimestamp operator-(const base::TimeDelta& rhs) const
    {
        return DecodeTimestamp(ts_ - rhs);
    }

    int64 operator/(const base::TimeDelta& rhs) const
    {
        return ts_ / rhs;
    }

    static DecodeTimestamp FromSecondsD(double seconds)
    {
        return DecodeTimestamp(base::TimeDelta::FromSecondsD(seconds));
    }

    static DecodeTimestamp FromMilliseconds(int64 milliseconds)
    {
        return DecodeTimestamp(base::TimeDelta::FromMilliseconds(milliseconds));
    }

    static DecodeTimestamp FromMicroseconds(int64 microseconds)
    {
        return DecodeTimestamp(base::TimeDelta::FromMicroseconds(microseconds));
    }

    // This method is used to explicitly call out when presentation timestamps
    // are being converted to a decode timestamp.
    static DecodeTimestamp FromPresentationTime(base::TimeDelta timestamp)
    {
        return DecodeTimestamp(timestamp);
    }

    double InSecondsF() const { return ts_.InSecondsF(); }
    int64 InMilliseconds() const { return ts_.InMilliseconds(); }
    int64 InMicroseconds() const { return ts_.InMicroseconds(); }

    // TODO(acolwell): Remove once all the hacks are gone. This method is called
    // by hacks where a decode time is being used as a presentation time.
    base::TimeDelta ToPresentationTime() const { return ts_; }

private:
    explicit DecodeTimestamp(base::TimeDelta timestamp)
        : ts_(timestamp)
    {
    }

    base::TimeDelta ts_;
};

MEDIA_EXPORT extern inline DecodeTimestamp kNoDecodeTimestamp()
{
    return DecodeTimestamp::FromPresentationTime(kNoTimestamp());
}

class MEDIA_EXPORT StreamParserBuffer : public DecoderBuffer {
public:
    // Value used to signal an invalid decoder config ID.
    enum { kInvalidConfigId = -1 };

    typedef DemuxerStream::Type Type;
    typedef StreamParser::TrackId TrackId;

    static scoped_refptr<StreamParserBuffer> CreateEOSBuffer();

    static scoped_refptr<StreamParserBuffer> CopyFrom(
        const uint8* data, int data_size, bool is_key_frame, Type type,
        TrackId track_id);
    static scoped_refptr<StreamParserBuffer> CopyFrom(
        const uint8* data, int data_size,
        const uint8* side_data, int side_data_size, bool is_key_frame, Type type,
        TrackId track_id);

    // Decode timestamp. If not explicitly set, or set to kNoTimestamp(), the
    // value will be taken from the normal timestamp.
    DecodeTimestamp GetDecodeTimestamp() const;
    void SetDecodeTimestamp(DecodeTimestamp timestamp);

    // Gets/sets the ID of the decoder config associated with this buffer.
    int GetConfigId() const;
    void SetConfigId(int config_id);

    // Returns the config ID of this buffer if it has no splice buffers or
    // |index| is out of range.  Otherwise returns the config ID for the
    // buffer in |splice_buffers_| at position |index|.
    int GetSpliceBufferConfigId(size_t index) const;

    // Gets the parser's media type associated with this buffer. Value is
    // meaningless for EOS buffers.
    Type type() const { return type_; }
    const char* GetTypeName() const;

    // Gets the parser's track ID associated with this buffer. Value is
    // meaningless for EOS buffers.
    TrackId track_id() const { return track_id_; }

    // Converts this buffer to a splice buffer.  |pre_splice_buffers| must not
    // have any EOS buffers, must not have any splice buffers, nor must have any
    // buffer with preroll.
    //
    // |pre_splice_buffers| will be deep copied and each copy's splice_timestamp()
    // will be set to this buffer's splice_timestamp().  A copy of |this|, with a
    // splice_timestamp() of kNoTimestamp(), will be added to the end of
    // |splice_buffers_|.
    //
    // See the Audio Splice Frame Algorithm in the MSE specification for details.
    typedef StreamParser::BufferQueue BufferQueue;
    void ConvertToSpliceBuffer(const BufferQueue& pre_splice_buffers);
    const BufferQueue& splice_buffers() const { return splice_buffers_; }

    // Specifies a buffer which must be decoded prior to this one to ensure this
    // buffer can be accurately decoded.  The given buffer must be of the same
    // type, must not be a splice buffer, must not have any discard padding, and
    // must not be an end of stream buffer.  |preroll| is not copied.
    //
    // It's expected that this preroll buffer will be discarded entirely post
    // decoding.  As such it's discard_padding() will be set to kInfiniteDuration.
    //
    // All future timestamp, decode timestamp, config id, or track id changes to
    // this buffer will be applied to the preroll buffer as well.
    void SetPrerollBuffer(const scoped_refptr<StreamParserBuffer>& preroll);
    const scoped_refptr<StreamParserBuffer>& preroll_buffer()
    {
        return preroll_buffer_;
    }

    void set_timestamp(base::TimeDelta timestamp) override;

    bool is_duration_estimated() const { return is_duration_estimated_; }

    void set_is_duration_estimated(bool is_estimated)
    {
        is_duration_estimated_ = is_estimated;
    }

private:
    StreamParserBuffer(const uint8* data, int data_size,
        const uint8* side_data, int side_data_size,
        bool is_key_frame, Type type,
        TrackId track_id);
    ~StreamParserBuffer() override;

    DecodeTimestamp decode_timestamp_;
    int config_id_;
    Type type_;
    TrackId track_id_;
    BufferQueue splice_buffers_;
    scoped_refptr<StreamParserBuffer> preroll_buffer_;
    bool is_duration_estimated_;

    DISALLOW_COPY_AND_ASSIGN(StreamParserBuffer);
};

} // namespace media

#endif // MEDIA_BASE_STREAM_PARSER_BUFFER_H_

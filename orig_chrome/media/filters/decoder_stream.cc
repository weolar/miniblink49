// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/decoder_stream.h"

#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/decoder_buffer.h"
#include "media/base/limits.h"
#include "media/base/media_log.h"
#include "media/base/timestamp_constants.h"
#include "media/base/video_decoder.h"
#include "media/base/video_frame.h"
#include "media/filters/decrypting_demuxer_stream.h"

namespace media {

// TODO(rileya): Devise a better way of specifying trace/UMA/etc strings for
// templated classes such as this.
template <DemuxerStream::Type StreamType>
static const char* GetTraceString();

#define FUNCTION_DVLOG(level) \
    DVLOG(level) << __FUNCTION__ << "<" << GetStreamTypeString() << ">"

template <>
const char* GetTraceString<DemuxerStream::VIDEO>()
{
    return "DecoderStream<VIDEO>::Decode";
}

template <>
const char* GetTraceString<DemuxerStream::AUDIO>()
{
    return "DecoderStream<AUDIO>::Decode";
}

template <DemuxerStream::Type StreamType>
DecoderStream<StreamType>::DecoderStream(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    ScopedVector<Decoder> decoders,
    const scoped_refptr<MediaLog>& media_log)
    : task_runner_(task_runner)
    , media_log_(media_log)
    , state_(STATE_UNINITIALIZED)
    , stream_(NULL)
    , decoder_selector_(new DecoderSelector<StreamType>(task_runner,
          decoders.Pass(),
          media_log))
    , decoded_frames_since_fallback_(0)
    , active_splice_(false)
    , decoding_eos_(false)
    , pending_decode_requests_(0)
    , weak_factory_(this)
{
}

template <DemuxerStream::Type StreamType>
DecoderStream<StreamType>::~DecoderStream()
{
    FUNCTION_DVLOG(2);
    DCHECK(task_runner_->BelongsToCurrentThread());

    decoder_selector_.reset();

    if (!init_cb_.is_null()) {
        task_runner_->PostTask(FROM_HERE,
            base::Bind(base::ResetAndReturn(&init_cb_), false));
    }
    if (!read_cb_.is_null()) {
        task_runner_->PostTask(FROM_HERE, base::Bind(base::ResetAndReturn(&read_cb_), ABORTED, scoped_refptr<Output>()));
    }
    if (!reset_cb_.is_null())
        task_runner_->PostTask(FROM_HERE, base::ResetAndReturn(&reset_cb_));

    stream_ = NULL;
    decoder_.reset();
    decrypting_demuxer_stream_.reset();
}

template <DemuxerStream::Type StreamType>
std::string DecoderStream<StreamType>::GetStreamTypeString()
{
    return DecoderStreamTraits<StreamType>::ToString();
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::Initialize(
    DemuxerStream* stream,
    const InitCB& init_cb,
    const SetCdmReadyCB& set_cdm_ready_cb,
    const StatisticsCB& statistics_cb,
    const base::Closure& waiting_for_decryption_key_cb)
{
    FUNCTION_DVLOG(2);
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, STATE_UNINITIALIZED);
    DCHECK(init_cb_.is_null());
    DCHECK(!init_cb.is_null());

    statistics_cb_ = statistics_cb;
    init_cb_ = init_cb;
    waiting_for_decryption_key_cb_ = waiting_for_decryption_key_cb;
    stream_ = stream;

    state_ = STATE_INITIALIZING;
    SelectDecoder(set_cdm_ready_cb);
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::Read(const ReadCB& read_cb)
{
    FUNCTION_DVLOG(2);
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(state_ != STATE_UNINITIALIZED && state_ != STATE_INITIALIZING)
        << state_;
    // No two reads in the flight at any time.
    DCHECK(read_cb_.is_null());
    // No read during resetting or stopping process.
    DCHECK(reset_cb_.is_null());

    if (state_ == STATE_ERROR) {
        task_runner_->PostTask(
            FROM_HERE, base::Bind(read_cb, DECODE_ERROR, scoped_refptr<Output>()));
        return;
    }

    if (state_ == STATE_END_OF_STREAM && ready_outputs_.empty()) {
        task_runner_->PostTask(
            FROM_HERE, base::Bind(read_cb, OK, StreamTraits::CreateEOSOutput()));
        return;
    }

    if (!ready_outputs_.empty()) {
        task_runner_->PostTask(FROM_HERE,
            base::Bind(read_cb, OK, ready_outputs_.front()));
        ready_outputs_.pop_front();
    } else {
        read_cb_ = read_cb;
    }

    if (state_ == STATE_NORMAL && CanDecodeMore())
        ReadFromDemuxerStream();
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::Reset(const base::Closure& closure)
{
    FUNCTION_DVLOG(2);
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_NE(state_, STATE_UNINITIALIZED);
    DCHECK(reset_cb_.is_null());

    reset_cb_ = closure;

    if (!read_cb_.is_null()) {
        task_runner_->PostTask(FROM_HERE, base::Bind(base::ResetAndReturn(&read_cb_), ABORTED, scoped_refptr<Output>()));
    }

    ready_outputs_.clear();

    // During decoder reinitialization, the Decoder does not need to be and
    // cannot be Reset(). |decrypting_demuxer_stream_| was reset before decoder
    // reinitialization.
    if (state_ == STATE_REINITIALIZING_DECODER)
        return;

    // During pending demuxer read and when not using DecryptingDemuxerStream,
    // the Decoder will be reset after demuxer read is returned
    // (in OnBufferReady()).
    if (state_ == STATE_PENDING_DEMUXER_READ && !decrypting_demuxer_stream_)
        return;

    if (decrypting_demuxer_stream_) {
        decrypting_demuxer_stream_->Reset(base::Bind(
            &DecoderStream<StreamType>::ResetDecoder, weak_factory_.GetWeakPtr()));
        return;
    }

    ResetDecoder();
}

template <DemuxerStream::Type StreamType>
bool DecoderStream<StreamType>::CanReadWithoutStalling() const
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    return !ready_outputs_.empty() || decoder_->CanReadWithoutStalling();
}

template <>
bool DecoderStream<DemuxerStream::AUDIO>::CanReadWithoutStalling() const
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    return true;
}

template <DemuxerStream::Type StreamType>
int DecoderStream<StreamType>::GetMaxDecodeRequests() const
{
    return decoder_->GetMaxDecodeRequests();
}

template <>
int DecoderStream<DemuxerStream::AUDIO>::GetMaxDecodeRequests() const
{
    return 1;
}

template <DemuxerStream::Type StreamType>
bool DecoderStream<StreamType>::CanDecodeMore() const
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // Limit total number of outputs stored in |ready_outputs_| and being decoded.
    // It only makes sense to saturate decoder completely when output queue is
    // empty.
    int num_decodes = static_cast<int>(ready_outputs_.size()) + pending_decode_requests_;
    return !decoding_eos_ && num_decodes < GetMaxDecodeRequests();
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::SelectDecoder(
    const SetCdmReadyCB& set_cdm_ready_cb)
{
    decoder_selector_->SelectDecoder(
        stream_, set_cdm_ready_cb,
        base::Bind(&DecoderStream<StreamType>::OnDecoderSelected,
            weak_factory_.GetWeakPtr()),
        base::Bind(&DecoderStream<StreamType>::OnDecodeOutputReady,
            weak_factory_.GetWeakPtr()),
        waiting_for_decryption_key_cb_);
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::OnDecoderSelected(
    scoped_ptr<Decoder> selected_decoder,
    scoped_ptr<DecryptingDemuxerStream> decrypting_demuxer_stream)
{
    FUNCTION_DVLOG(2) << ": "
                      << (selected_decoder ? selected_decoder->GetDisplayName()
                                           : "No decoder selected.");
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(state_ == STATE_INITIALIZING || state_ == STATE_REINITIALIZING_DECODER)
        << state_;
    if (state_ == STATE_INITIALIZING) {
        DCHECK(!init_cb_.is_null());
        DCHECK(read_cb_.is_null());
        DCHECK(reset_cb_.is_null());
    } else {
        DCHECK(decoder_);
    }

    previous_decoder_ = decoder_.Pass();
    decoded_frames_since_fallback_ = 0;
    decoder_ = selected_decoder.Pass();
    if (decrypting_demuxer_stream) {
        decrypting_demuxer_stream_ = decrypting_demuxer_stream.Pass();
        stream_ = decrypting_demuxer_stream_.get();
    }

    if (!decoder_) {
        if (state_ == STATE_INITIALIZING) {
            state_ = STATE_UNINITIALIZED;
            MEDIA_LOG(ERROR, media_log_) << GetStreamTypeString()
                                         << " decoder initialization failed";
            base::ResetAndReturn(&init_cb_).Run(false);
        } else {
            CompleteDecoderReinitialization(false);
        }
        return;
    }

    media_log_->SetBooleanProperty(GetStreamTypeString() + "_dds",
        decrypting_demuxer_stream_);
    media_log_->SetStringProperty(GetStreamTypeString() + "_decoder",
        decoder_->GetDisplayName());

    if (state_ == STATE_REINITIALIZING_DECODER) {
        CompleteDecoderReinitialization(true);
        return;
    }

    // Initialization succeeded.
    state_ = STATE_NORMAL;
    if (StreamTraits::NeedsBitstreamConversion(decoder_.get()))
        stream_->EnableBitstreamConverter();
    base::ResetAndReturn(&init_cb_).Run(true);
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::SatisfyRead(
    Status status,
    const scoped_refptr<Output>& output)
{
    DCHECK(!read_cb_.is_null());
    base::ResetAndReturn(&read_cb_).Run(status, output);
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::Decode(
    const scoped_refptr<DecoderBuffer>& buffer)
{
    FUNCTION_DVLOG(2);
    DCHECK(state_ == STATE_NORMAL || state_ == STATE_FLUSHING_DECODER) << state_;
    DCHECK_LT(pending_decode_requests_, GetMaxDecodeRequests());
    DCHECK(reset_cb_.is_null());
    DCHECK(buffer.get());

    int buffer_size = buffer->end_of_stream() ? 0 : buffer->data_size();

    TRACE_EVENT_ASYNC_BEGIN2(
        "media", GetTraceString<StreamType>(), this, "key frame",
        !buffer->end_of_stream() && buffer->is_key_frame(), "timestamp (ms)",
        !buffer->end_of_stream() ? buffer->timestamp().InMilliseconds() : 0);

    if (buffer->end_of_stream())
        decoding_eos_ = true;

    ++pending_decode_requests_;
    decoder_->Decode(buffer,
        base::Bind(&DecoderStream<StreamType>::OnDecodeDone,
            weak_factory_.GetWeakPtr(),
            buffer_size,
            buffer->end_of_stream()));
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::FlushDecoder()
{
    Decode(DecoderBuffer::CreateEOSBuffer());
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::OnDecodeDone(int buffer_size,
    bool end_of_stream,
    typename Decoder::Status status)
{
    FUNCTION_DVLOG(2) << ": " << status;
    DCHECK(state_ == STATE_NORMAL || state_ == STATE_FLUSHING_DECODER || state_ == STATE_PENDING_DEMUXER_READ || state_ == STATE_ERROR)
        << state_;
    DCHECK_GT(pending_decode_requests_, 0);

    --pending_decode_requests_;

    TRACE_EVENT_ASYNC_END0("media", GetTraceString<StreamType>(), this);

    if (end_of_stream) {
        DCHECK(!pending_decode_requests_);
        decoding_eos_ = false;
    }

    if (state_ == STATE_ERROR) {
        DCHECK(read_cb_.is_null());
        return;
    }

    // Drop decoding result if Reset() was called during decoding.
    // The resetting process will be handled when the decoder is reset.
    if (!reset_cb_.is_null())
        return;

    switch (status) {
    case Decoder::kDecodeError:
        state_ = STATE_ERROR;
        MEDIA_LOG(ERROR, media_log_) << GetStreamTypeString() << " decode error";
        ready_outputs_.clear();
        if (!read_cb_.is_null())
            SatisfyRead(DECODE_ERROR, NULL);
        return;

    case Decoder::kAborted:
        // Decoder can return kAborted during Reset() or during destruction.
        return;

    case Decoder::kOk:
        // Any successful decode counts!
        if (buffer_size > 0)
            StreamTraits::ReportStatistics(statistics_cb_, buffer_size);

        if (state_ == STATE_NORMAL) {
            if (end_of_stream) {
                state_ = STATE_END_OF_STREAM;
                if (ready_outputs_.empty() && !read_cb_.is_null())
                    SatisfyRead(OK, StreamTraits::CreateEOSOutput());
                return;
            }

            if (CanDecodeMore())
                ReadFromDemuxerStream();
            return;
        }

        if (state_ == STATE_FLUSHING_DECODER && !pending_decode_requests_)
            ReinitializeDecoder();
        return;
    }
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::OnDecodeOutputReady(
    const scoped_refptr<Output>& output)
{
    FUNCTION_DVLOG(2) << ": " << output->timestamp().InMilliseconds() << " ms";
    DCHECK(output.get());
    DCHECK(state_ == STATE_NORMAL || state_ == STATE_FLUSHING_DECODER || state_ == STATE_PENDING_DEMUXER_READ || state_ == STATE_ERROR)
        << state_;

    if (state_ == STATE_ERROR) {
        DCHECK(read_cb_.is_null());
        return;
    }

    // Drop decoding result if Reset() was called during decoding.
    // The resetting process will be handled when the decoder is reset.
    if (!reset_cb_.is_null())
        return;

    if (!read_cb_.is_null()) {
        // If |ready_outputs_| was non-empty, the read would have already been
        // satisifed by Read().
        DCHECK(ready_outputs_.empty());
        SatisfyRead(OK, output);
        return;
    }

    // Store decoded output.
    ready_outputs_.push_back(output);

    // Destruct any previous decoder once we've decoded enough frames to ensure
    // that it's no longer in use.
    if (previous_decoder_ && ++decoded_frames_since_fallback_ > limits::kMaxVideoFrames) {
        previous_decoder_.reset();
    }
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::ReadFromDemuxerStream()
{
    FUNCTION_DVLOG(2);
    DCHECK_EQ(state_, STATE_NORMAL);
    DCHECK(CanDecodeMore());
    DCHECK(reset_cb_.is_null());

    state_ = STATE_PENDING_DEMUXER_READ;
    stream_->Read(base::Bind(&DecoderStream<StreamType>::OnBufferReady,
        weak_factory_.GetWeakPtr()));
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::OnBufferReady(
    DemuxerStream::Status status,
    const scoped_refptr<DecoderBuffer>& buffer)
{
    FUNCTION_DVLOG(2) << ": " << status << ", "
                      << (buffer.get() ? buffer->AsHumanReadableString()
                                       : "NULL");

    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(state_ == STATE_PENDING_DEMUXER_READ || state_ == STATE_ERROR)
        << state_;
    DCHECK_EQ(buffer.get() != NULL, status == DemuxerStream::kOk) << status;

    // Decoding has been stopped (e.g due to an error).
    if (state_ != STATE_PENDING_DEMUXER_READ) {
        DCHECK(state_ == STATE_ERROR);
        DCHECK(read_cb_.is_null());
        return;
    }

    state_ = STATE_NORMAL;

    if (status == DemuxerStream::kConfigChanged) {
        FUNCTION_DVLOG(2) << ": "
                          << "ConfigChanged";
        DCHECK(stream_->SupportsConfigChanges());

        if (!config_change_observer_cb_.is_null())
            config_change_observer_cb_.Run();

        state_ = STATE_FLUSHING_DECODER;
        if (!reset_cb_.is_null()) {
            // If we are using DecryptingDemuxerStream, we already called DDS::Reset()
            // which will continue the resetting process in it's callback.
            if (!decrypting_demuxer_stream_)
                Reset(base::ResetAndReturn(&reset_cb_));
            // Reinitialization will continue after Reset() is done.
        } else {
            FlushDecoder();
        }
        return;
    }

    if (!reset_cb_.is_null()) {
        // If we are using DecryptingDemuxerStream, we already called DDS::Reset()
        // which will continue the resetting process in it's callback.
        if (!decrypting_demuxer_stream_)
            Reset(base::ResetAndReturn(&reset_cb_));
        return;
    }

    if (status == DemuxerStream::kAborted) {
        if (!read_cb_.is_null())
            SatisfyRead(DEMUXER_READ_ABORTED, NULL);
        return;
    }

    if (!splice_observer_cb_.is_null() && !buffer->end_of_stream()) {
        const bool has_splice_ts = buffer->splice_timestamp() != kNoTimestamp();
        if (active_splice_ || has_splice_ts) {
            splice_observer_cb_.Run(buffer->splice_timestamp());
            active_splice_ = has_splice_ts;
        }
    }

    DCHECK(status == DemuxerStream::kOk) << status;
    Decode(buffer);

    // Read more data if the decoder supports multiple parallel decoding requests.
    if (CanDecodeMore())
        ReadFromDemuxerStream();
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::ReinitializeDecoder()
{
    FUNCTION_DVLOG(2);
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, STATE_FLUSHING_DECODER);
    DCHECK_EQ(pending_decode_requests_, 0);

    state_ = STATE_REINITIALIZING_DECODER;
    DecoderStreamTraits<StreamType>::InitializeDecoder(
        decoder_.get(), stream_,
        base::Bind(&DecoderStream<StreamType>::OnDecoderReinitialized,
            weak_factory_.GetWeakPtr()),
        base::Bind(&DecoderStream<StreamType>::OnDecodeOutputReady,
            weak_factory_.GetWeakPtr()));
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::OnDecoderReinitialized(bool success)
{
    FUNCTION_DVLOG(2);
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, STATE_REINITIALIZING_DECODER);

    // ReinitializeDecoder() can be called in two cases:
    // 1, Flushing decoder finished (see OnDecodeOutputReady()).
    // 2, Reset() was called during flushing decoder (see OnDecoderReset()).
    // Also, Reset() can be called during pending ReinitializeDecoder().
    // This function needs to handle them all!

    if (!success) {
        // Reinitialization failed. Try to fall back to one of the remaining
        // decoders. This will consume at least one decoder so doing it more than
        // once is safe.
        // For simplicity, don't attempt to fall back to a decryptor. Calling this
        // with a null callback ensures that one won't be selected.
        SelectDecoder(SetCdmReadyCB());
    } else {
        CompleteDecoderReinitialization(true);
    }
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::CompleteDecoderReinitialization(bool success)
{
    FUNCTION_DVLOG(2);
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, STATE_REINITIALIZING_DECODER);

    state_ = success ? STATE_NORMAL : STATE_ERROR;

    if (!reset_cb_.is_null()) {
        base::ResetAndReturn(&reset_cb_).Run();
        return;
    }

    if (read_cb_.is_null())
        return;

    if (state_ == STATE_ERROR) {
        MEDIA_LOG(ERROR, media_log_) << GetStreamTypeString()
                                     << " decoder reinitialization failed";
        SatisfyRead(DECODE_ERROR, NULL);
        return;
    }

    ReadFromDemuxerStream();
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::ResetDecoder()
{
    FUNCTION_DVLOG(2);
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(state_ == STATE_NORMAL || state_ == STATE_FLUSHING_DECODER || state_ == STATE_ERROR || state_ == STATE_END_OF_STREAM) << state_;
    DCHECK(!reset_cb_.is_null());

    decoder_->Reset(base::Bind(&DecoderStream<StreamType>::OnDecoderReset,
        weak_factory_.GetWeakPtr()));
}

template <DemuxerStream::Type StreamType>
void DecoderStream<StreamType>::OnDecoderReset()
{
    FUNCTION_DVLOG(2);
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(state_ == STATE_NORMAL || state_ == STATE_FLUSHING_DECODER || state_ == STATE_ERROR || state_ == STATE_END_OF_STREAM) << state_;
    // If Reset() was called during pending read, read callback should be fired
    // before the reset callback is fired.
    DCHECK(read_cb_.is_null());
    DCHECK(!reset_cb_.is_null());

    if (state_ != STATE_FLUSHING_DECODER) {
        state_ = STATE_NORMAL;
        active_splice_ = false;
        base::ResetAndReturn(&reset_cb_).Run();
        return;
    }

    // The resetting process will be continued in OnDecoderReinitialized().
    ReinitializeDecoder();
}

template class DecoderStream<DemuxerStream::VIDEO>;
template class DecoderStream<DemuxerStream::AUDIO>;

} // namespace media

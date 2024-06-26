// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/decrypting_video_decoder.h"

#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/single_thread_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/decoder_buffer.h"
#include "media/base/media_log.h"
#include "media/base/pipeline.h"
#include "media/base/video_frame.h"

namespace media {

const char DecryptingVideoDecoder::kDecoderName[] = "DecryptingVideoDecoder";

DecryptingVideoDecoder::DecryptingVideoDecoder(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    const scoped_refptr<MediaLog>& media_log,
    const SetCdmReadyCB& set_cdm_ready_cb,
    const base::Closure& waiting_for_decryption_key_cb)
    : task_runner_(task_runner)
    , media_log_(media_log)
    , state_(kUninitialized)
    , waiting_for_decryption_key_cb_(waiting_for_decryption_key_cb)
    , set_cdm_ready_cb_(set_cdm_ready_cb)
    , decryptor_(NULL)
    , key_added_while_decode_pending_(false)
    , trace_id_(0)
    , weak_factory_(this)
{
}

std::string DecryptingVideoDecoder::GetDisplayName() const
{
    return kDecoderName;
}

void DecryptingVideoDecoder::Initialize(const VideoDecoderConfig& config,
    bool /* low_delay */,
    const InitCB& init_cb,
    const OutputCB& output_cb)
{
    DVLOG(2) << "Initialize()";
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(state_ == kUninitialized || state_ == kIdle || state_ == kDecodeFinished) << state_;
    DCHECK(decode_cb_.is_null());
    DCHECK(reset_cb_.is_null());
    DCHECK(config.IsValidConfig());
    DCHECK(config.is_encrypted());

    init_cb_ = BindToCurrentLoop(init_cb);
    output_cb_ = BindToCurrentLoop(output_cb);
    weak_this_ = weak_factory_.GetWeakPtr();
    config_ = config;

    if (state_ == kUninitialized) {
        state_ = kDecryptorRequested;
        set_cdm_ready_cb_.Run(BindToCurrentLoop(
            base::Bind(&DecryptingVideoDecoder::SetCdm, weak_this_)));
        return;
    }

    // Reinitialization.
    decryptor_->DeinitializeDecoder(Decryptor::kVideo);
    state_ = kPendingDecoderInit;
    decryptor_->InitializeVideoDecoder(config, BindToCurrentLoop(base::Bind(&DecryptingVideoDecoder::FinishInitialization, weak_this_)));
}

void DecryptingVideoDecoder::Decode(const scoped_refptr<DecoderBuffer>& buffer,
    const DecodeCB& decode_cb)
{
    DVLOG(3) << "Decode()";
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(state_ == kIdle || state_ == kDecodeFinished || state_ == kError) << state_;
    DCHECK(!decode_cb.is_null());
    CHECK(decode_cb_.is_null()) << "Overlapping decodes are not supported.";

    decode_cb_ = BindToCurrentLoop(decode_cb);

    if (state_ == kError) {
        base::ResetAndReturn(&decode_cb_).Run(kDecodeError);
        return;
    }

    // Return empty frames if decoding has finished.
    if (state_ == kDecodeFinished) {
        base::ResetAndReturn(&decode_cb_).Run(kOk);
        return;
    }

    pending_buffer_to_decode_ = buffer;
    state_ = kPendingDecode;
    DecodePendingBuffer();
}

void DecryptingVideoDecoder::Reset(const base::Closure& closure)
{
    DVLOG(2) << "Reset() - state: " << state_;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(state_ == kIdle || state_ == kPendingDecode || state_ == kWaitingForKey || state_ == kDecodeFinished || state_ == kError) << state_;
    DCHECK(init_cb_.is_null()); // No Reset() during pending initialization.
    DCHECK(reset_cb_.is_null());

    reset_cb_ = BindToCurrentLoop(closure);

    decryptor_->ResetDecoder(Decryptor::kVideo);

    // Reset() cannot complete if the decode callback is still pending.
    // Defer the resetting process in this case. The |reset_cb_| will be fired
    // after the decode callback is fired - see DecryptAndDecodeBuffer() and
    // DeliverFrame().
    if (state_ == kPendingDecode) {
        DCHECK(!decode_cb_.is_null());
        return;
    }

    if (state_ == kWaitingForKey) {
        DCHECK(!decode_cb_.is_null());
        pending_buffer_to_decode_ = NULL;
        base::ResetAndReturn(&decode_cb_).Run(kAborted);
    }

    DCHECK(decode_cb_.is_null());
    DoReset();
}

DecryptingVideoDecoder::~DecryptingVideoDecoder()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (state_ == kUninitialized)
        return;

    if (decryptor_) {
        decryptor_->DeinitializeDecoder(Decryptor::kVideo);
        decryptor_ = NULL;
    }
    if (!set_cdm_ready_cb_.is_null())
        base::ResetAndReturn(&set_cdm_ready_cb_).Run(CdmReadyCB());
    pending_buffer_to_decode_ = NULL;
    if (!init_cb_.is_null())
        base::ResetAndReturn(&init_cb_).Run(false);
    if (!decode_cb_.is_null())
        base::ResetAndReturn(&decode_cb_).Run(kAborted);
    if (!reset_cb_.is_null())
        base::ResetAndReturn(&reset_cb_).Run();
}

void DecryptingVideoDecoder::SetCdm(CdmContext* cdm_context,
    const CdmAttachedCB& cdm_attached_cb)
{
    DVLOG(2) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, kDecryptorRequested) << state_;
    DCHECK(!init_cb_.is_null());
    DCHECK(!set_cdm_ready_cb_.is_null());
    set_cdm_ready_cb_.Reset();

    if (!cdm_context || !cdm_context->GetDecryptor()) {
        MEDIA_LOG(DEBUG, media_log_) << GetDisplayName() << ": no decryptor set";
        base::ResetAndReturn(&init_cb_).Run(false);
        state_ = kError;
        cdm_attached_cb.Run(false);
        return;
    }

    decryptor_ = cdm_context->GetDecryptor();

    state_ = kPendingDecoderInit;
    decryptor_->InitializeVideoDecoder(
        config_,
        BindToCurrentLoop(base::Bind(
            &DecryptingVideoDecoder::FinishInitialization, weak_this_)));
    cdm_attached_cb.Run(true);
}

void DecryptingVideoDecoder::FinishInitialization(bool success)
{
    DVLOG(2) << "FinishInitialization()";
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, kPendingDecoderInit) << state_;
    DCHECK(!init_cb_.is_null());
    DCHECK(reset_cb_.is_null()); // No Reset() before initialization finished.
    DCHECK(decode_cb_.is_null()); // No Decode() before initialization finished.

    if (!success) {
        MEDIA_LOG(DEBUG, media_log_) << GetDisplayName()
                                     << ": failed to init decoder on decryptor";
        base::ResetAndReturn(&init_cb_).Run(false);
        decryptor_ = NULL;
        state_ = kError;
        return;
    }

    decryptor_->RegisterNewKeyCB(
        Decryptor::kVideo,
        BindToCurrentLoop(
            base::Bind(&DecryptingVideoDecoder::OnKeyAdded, weak_this_)));

    // Success!
    state_ = kIdle;
    base::ResetAndReturn(&init_cb_).Run(true);
}

void DecryptingVideoDecoder::DecodePendingBuffer()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, kPendingDecode) << state_;
    TRACE_EVENT_ASYNC_BEGIN0(
        "media", "DecryptingVideoDecoder::DecodePendingBuffer", ++trace_id_);

    int buffer_size = 0;
    if (!pending_buffer_to_decode_->end_of_stream()) {
        buffer_size = pending_buffer_to_decode_->data_size();
    }

    decryptor_->DecryptAndDecodeVideo(
        pending_buffer_to_decode_, BindToCurrentLoop(base::Bind(&DecryptingVideoDecoder::DeliverFrame, weak_this_, buffer_size)));
}

void DecryptingVideoDecoder::DeliverFrame(
    int buffer_size,
    Decryptor::Status status,
    const scoped_refptr<VideoFrame>& frame)
{
    DVLOG(3) << "DeliverFrame() - status: " << status;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, kPendingDecode) << state_;
    DCHECK(!decode_cb_.is_null());
    DCHECK(pending_buffer_to_decode_.get());

    TRACE_EVENT_ASYNC_END2(
        "media", "DecryptingVideoDecoder::DecodePendingBuffer", trace_id_,
        "buffer_size", buffer_size, "status", status);

    bool need_to_try_again_if_nokey_is_returned = key_added_while_decode_pending_;
    key_added_while_decode_pending_ = false;

    scoped_refptr<DecoderBuffer> scoped_pending_buffer_to_decode = pending_buffer_to_decode_;
    pending_buffer_to_decode_ = NULL;

    if (!reset_cb_.is_null()) {
        base::ResetAndReturn(&decode_cb_).Run(kAborted);
        DoReset();
        return;
    }

    DCHECK_EQ(status == Decryptor::kSuccess, frame.get() != NULL);

    if (status == Decryptor::kError) {
        DVLOG(2) << "DeliverFrame() - kError";
        MEDIA_LOG(ERROR, media_log_) << GetDisplayName() << ": decode error";
        state_ = kError;
        base::ResetAndReturn(&decode_cb_).Run(kDecodeError);
        return;
    }

    if (status == Decryptor::kNoKey) {
        DVLOG(2) << "DeliverFrame() - kNoKey";
        MEDIA_LOG(DEBUG, media_log_) << GetDisplayName() << ": no key";

        // Set |pending_buffer_to_decode_| back as we need to try decoding the
        // pending buffer again when new key is added to the decryptor.
        pending_buffer_to_decode_ = scoped_pending_buffer_to_decode;

        if (need_to_try_again_if_nokey_is_returned) {
            // The |state_| is still kPendingDecode.
            DecodePendingBuffer();
            return;
        }

        state_ = kWaitingForKey;
        waiting_for_decryption_key_cb_.Run();
        return;
    }

    if (status == Decryptor::kNeedMoreData) {
        DVLOG(2) << "DeliverFrame() - kNeedMoreData";
        state_ = scoped_pending_buffer_to_decode->end_of_stream() ? kDecodeFinished
                                                                  : kIdle;
        base::ResetAndReturn(&decode_cb_).Run(kOk);
        return;
    }

    DCHECK_EQ(status, Decryptor::kSuccess);
    // No frame returned with kSuccess should be end-of-stream frame.
    DCHECK(!frame->metadata()->IsTrue(VideoFrameMetadata::END_OF_STREAM));
    output_cb_.Run(frame);

    if (scoped_pending_buffer_to_decode->end_of_stream()) {
        // Set |pending_buffer_to_decode_| back as we need to keep flushing the
        // decryptor.
        pending_buffer_to_decode_ = scoped_pending_buffer_to_decode;
        DecodePendingBuffer();
        return;
    }

    state_ = kIdle;
    base::ResetAndReturn(&decode_cb_).Run(kOk);
}

void DecryptingVideoDecoder::OnKeyAdded()
{
    DVLOG(2) << "OnKeyAdded()";
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (state_ == kPendingDecode) {
        key_added_while_decode_pending_ = true;
        return;
    }

    if (state_ == kWaitingForKey) {
        state_ = kPendingDecode;
        DecodePendingBuffer();
    }
}

void DecryptingVideoDecoder::DoReset()
{
    DCHECK(init_cb_.is_null());
    DCHECK(decode_cb_.is_null());
    state_ = kIdle;
    base::ResetAndReturn(&reset_cb_).Run();
}

} // namespace media

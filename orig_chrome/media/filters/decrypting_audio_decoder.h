// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FILTERS_DECRYPTING_AUDIO_DECODER_H_
#define MEDIA_FILTERS_DECRYPTING_AUDIO_DECODER_H_

#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "media/base/audio_decoder.h"
#include "media/base/cdm_context.h"
#include "media/base/decryptor.h"
#include "media/base/demuxer_stream.h"

namespace base {
class SingleThreadTaskRunner;
}

namespace media {

class AudioTimestampHelper;
class DecoderBuffer;
class Decryptor;
class MediaLog;

// Decryptor-based AudioDecoder implementation that can decrypt and decode
// encrypted audio buffers and return decrypted and decompressed audio frames.
// All public APIs and callbacks are trampolined to the |task_runner_| so
// that no locks are required for thread safety.
class MEDIA_EXPORT DecryptingAudioDecoder : public AudioDecoder {
public:
    DecryptingAudioDecoder(
        const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
        const scoped_refptr<MediaLog>& media_log,
        const SetCdmReadyCB& set_cdm_ready_cb,
        const base::Closure& waiting_for_decryption_key_cb);
    ~DecryptingAudioDecoder() override;

    // AudioDecoder implementation.
    std::string GetDisplayName() const override;
    void Initialize(const AudioDecoderConfig& config,
        const InitCB& init_cb,
        const OutputCB& output_cb) override;
    void Decode(const scoped_refptr<DecoderBuffer>& buffer,
        const DecodeCB& decode_cb) override;
    void Reset(const base::Closure& closure) override;

private:
    // For a detailed state diagram please see this link: http://goo.gl/8jAok
    // TODO(xhwang): Add a ASCII state diagram in this file after this class
    // stabilizes.
    // TODO(xhwang): Update this diagram for DecryptingAudioDecoder.
    enum State {
        kUninitialized = 0,
        kDecryptorRequested,
        kPendingDecoderInit,
        kIdle,
        kPendingDecode,
        kWaitingForKey,
        kDecodeFinished,
        kError
    };

    // Callback to set CDM. |cdm_attached_cb| is called when the decryptor in the
    // CDM has been completely attached to the pipeline.
    void SetCdm(CdmContext* cdm_context, const CdmAttachedCB& cdm_attached_cb);

    // Initializes the audio decoder on the |decryptor_| with |config_|.
    void InitializeDecoder();

    // Callback for Decryptor::InitializeAudioDecoder() during initialization.
    void FinishInitialization(bool success);

    void DecodePendingBuffer();

    // Callback for Decryptor::DecryptAndDecodeAudio().
    void DeliverFrame(int buffer_size,
        Decryptor::Status status,
        const Decryptor::AudioFrames& frames);

    // Callback for the |decryptor_| to notify this object that a new key has been
    // added.
    void OnKeyAdded();

    // Resets decoder and calls |reset_cb_|.
    void DoReset();

    // Sets timestamps for |frames| and then passes them to |output_cb_|.
    void ProcessDecodedFrames(const Decryptor::AudioFrames& frames);

    scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

    scoped_refptr<MediaLog> media_log_;

    State state_;

    InitCB init_cb_;
    OutputCB output_cb_;
    DecodeCB decode_cb_;
    base::Closure reset_cb_;
    base::Closure waiting_for_decryption_key_cb_;

    // The current decoder configuration.
    AudioDecoderConfig config_;

    // Callback to request/cancel CDM ready notification.
    SetCdmReadyCB set_cdm_ready_cb_;

    Decryptor* decryptor_;

    // The buffer that needs decrypting/decoding.
    scoped_refptr<media::DecoderBuffer> pending_buffer_to_decode_;

    // Indicates the situation where new key is added during pending decode
    // (in other words, this variable can only be set in state kPendingDecode).
    // If this variable is true and kNoKey is returned then we need to try
    // decrypting/decoding again in case the newly added key is the correct
    // decryption key.
    bool key_added_while_decode_pending_;

    scoped_ptr<AudioTimestampHelper> timestamp_helper_;

    base::WeakPtr<DecryptingAudioDecoder> weak_this_;
    base::WeakPtrFactory<DecryptingAudioDecoder> weak_factory_;

    DISALLOW_COPY_AND_ASSIGN(DecryptingAudioDecoder);
};

} // namespace media

#endif // MEDIA_FILTERS_DECRYPTING_AUDIO_DECODER_H_

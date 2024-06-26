// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_AUDIO_DECODER_H_
#define MEDIA_BASE_AUDIO_DECODER_H_

#include <string>

#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "media/base/audio_decoder_config.h"
#include "media/base/channel_layout.h"
#include "media/base/decoder_buffer.h"
#include "media/base/media_export.h"
#include "media/base/pipeline_status.h"

namespace media {

class AudioBuffer;
class DemuxerStream;

class MEDIA_EXPORT AudioDecoder {
public:
    // Status codes for decode operations.
    // TODO(rileya): Now that both AudioDecoder and VideoDecoder Status enums
    // match, break them into a decoder_status.h.
    enum Status {
        kOk, // We're all good.
        kAborted, // We aborted as a result of Reset() or destruction.
        kDecodeError // A decoding error occurred.
    };

    // Callback for VideoDecoder initialization.
    typedef base::Callback<void(bool success)> InitCB;

    // Callback for AudioDecoder to return a decoded frame whenever it becomes
    // available. Only non-EOS frames should be returned via this callback.
    typedef base::Callback<void(const scoped_refptr<AudioBuffer>&)> OutputCB;

    // Callback for Decode(). Called after the decoder has completed decoding
    // corresponding DecoderBuffer, indicating that it's ready to accept another
    // buffer to decode.
    typedef base::Callback<void(Status)> DecodeCB;

    AudioDecoder();

    // Fires any pending callbacks, stops and destroys the decoder.
    // Note: Since this is a destructor, |this| will be destroyed after this call.
    // Make sure the callbacks fired from this call doesn't post any task that
    // depends on |this|.
    virtual ~AudioDecoder();

    // Returns the name of the decoder for logging purpose.
    virtual std::string GetDisplayName() const = 0;

    // Initializes an AudioDecoder with the given DemuxerStream, executing the
    // callback upon completion.
    //  |init_cb| is used to return initialization status.
    //  |output_cb| is called for decoded audio buffers (see Decode()).
    virtual void Initialize(const AudioDecoderConfig& config,
        const InitCB& init_cb,
        const OutputCB& output_cb)
        = 0;

    // Requests samples to be decoded. Only one decode may be in flight at any
    // given time. Once the buffer is decoded the decoder calls |decode_cb|.
    // |output_cb| specified in Initialize() is called for each decoded buffer,
    // before or after |decode_cb|.
    //
    // Implementations guarantee that the callbacks will not be called from within
    // this method.
    //
    // If |buffer| is an EOS buffer then the decoder must be flushed, i.e.
    // |output_cb| must be called for each frame pending in the queue and
    // |decode_cb| must be called after that.
    virtual void Decode(const scoped_refptr<DecoderBuffer>& buffer,
        const DecodeCB& decode_cb)
        = 0;

    // Resets decoder state. All pending Decode() requests will be finished or
    // aborted before |closure| is called.
    virtual void Reset(const base::Closure& closure) = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(AudioDecoder);
};

} // namespace media

#endif // MEDIA_BASE_AUDIO_DECODER_H_

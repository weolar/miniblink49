// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_RENDERER_H_
#define MEDIA_BASE_RENDERER_H_

#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "base/time/time.h"
#include "media/base/buffering_state.h"
#include "media/base/cdm_context.h"
#include "media/base/media_export.h"
#include "media/base/pipeline_status.h"

namespace media {

class DemuxerStreamProvider;
class VideoFrame;

class MEDIA_EXPORT Renderer {
public:
    Renderer();

    // Stops rendering and fires any pending callbacks.
    virtual ~Renderer();

    // Initializes the Renderer with |demuxer_stream_provider|, executing
    // |init_cb| upon completion.  If initialization fails, only |init_cb| (not
    // |error_cb|) should be called.  |demuxer_stream_provider| must be valid for
    // the lifetime of the Renderer object.  |init_cb| must only be run after this
    // method has returned.  Firing |init_cb| may result in the immediate
    // destruction of the caller, so it must be run only prior to returning.
    //
    // Permanent callbacks:
    // - |statistics_cb|: Executed periodically with rendering statistics.
    // - |buffering_state_cb|: Executed when buffering state is changed.
    // - |ended_cb|: Executed when rendering has reached the end of stream.
    // - |error_cb|: Executed if any error was encountered after initialization.
    // - |waiting_for_decryption_key_cb|: Executed whenever the key needed to
    //                                    decrypt the stream is not available.
    virtual void Initialize(
        DemuxerStreamProvider* demuxer_stream_provider,
        const PipelineStatusCB& init_cb,
        const StatisticsCB& statistics_cb,
        const BufferingStateCB& buffering_state_cb,
        const base::Closure& ended_cb,
        const PipelineStatusCB& error_cb,
        const base::Closure& waiting_for_decryption_key_cb)
        = 0;

    // Associates the |cdm_context| with this Renderer for decryption (and
    // decoding) of media data, then fires |cdm_attached_cb| with the result.
    virtual void SetCdm(CdmContext* cdm_context,
        const CdmAttachedCB& cdm_attached_cb)
        = 0;

    // The following functions must be called after Initialize().

    // Discards any buffered data, executing |flush_cb| when completed.
    virtual void Flush(const base::Closure& flush_cb) = 0;

    // Starts rendering from |time|.
    virtual void StartPlayingFrom(base::TimeDelta time) = 0;

    // Updates the current playback rate. The default playback rate should be 1.
    virtual void SetPlaybackRate(double playback_rate) = 0;

    // Sets the output volume. The default volume should be 1.
    virtual void SetVolume(float volume) = 0;

    // Returns the current media time.
    virtual base::TimeDelta GetMediaTime() = 0;

    // Returns whether |this| renders audio.
    virtual bool HasAudio() = 0;

    // Returns whether |this| renders video.
    virtual bool HasVideo() = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(Renderer);
};

} // namespace media

#endif // MEDIA_BASE_RENDERER_H_

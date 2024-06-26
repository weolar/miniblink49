// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_AUDIO_RENDERER_H_
#define MEDIA_BASE_AUDIO_RENDERER_H_

#include "base/callback.h"
#include "base/time/time.h"
#include "media/base/buffering_state.h"
#include "media/base/cdm_context.h"
#include "media/base/media_export.h"
#include "media/base/pipeline_status.h"

namespace media {

class DemuxerStream;
class TimeSource;

class MEDIA_EXPORT AudioRenderer {
public:
    AudioRenderer();

    // Stop all operations and fire all pending callbacks.
    virtual ~AudioRenderer();

    // Initialize an AudioRenderer with |stream|, executing |init_cb| upon
    // completion. If initialization fails, only |init_cb| (not |error_cb|) will
    // be called.
    //
    // |set_cdm_ready_cb| is fired when a CDM is needed, i.e. when the |stream| is
    // encrypted.
    //
    // |statistics_cb| is executed periodically with audio rendering stats.
    //
    // |buffering_state_cb| is executed when audio rendering has either run out of
    // data or has enough data to continue playback.
    //
    // |ended_cb| is executed when audio rendering has reached the end of stream.
    //
    // |error_cb| is executed if an error was encountered after initialization.
    //
    // |waiting_for_decryption_key_cb| is called whenever the key needed to
    // decrypt the stream is not available.
    virtual void Initialize(
        DemuxerStream* stream,
        const PipelineStatusCB& init_cb,
        const SetCdmReadyCB& set_cdm_ready_cb,
        const StatisticsCB& statistics_cb,
        const BufferingStateCB& buffering_state_cb,
        const base::Closure& ended_cb,
        const PipelineStatusCB& error_cb,
        const base::Closure& waiting_for_decryption_key_cb)
        = 0;

    // Returns the TimeSource associated with audio rendering.
    virtual TimeSource* GetTimeSource() = 0;

    // Discard any audio data, executing |callback| when completed.
    //
    // Clients should expect |buffering_state_cb| to be called with
    // BUFFERING_HAVE_NOTHING while flushing is in progress.
    virtual void Flush(const base::Closure& callback) = 0;

    // Starts playback by reading from |stream| and decoding and rendering audio.
    //
    // Only valid to call after a successful Initialize() or Flush().
    virtual void StartPlaying() = 0;

    // Sets the output volume.
    virtual void SetVolume(float volume) = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(AudioRenderer);
};

} // namespace media

#endif // MEDIA_BASE_AUDIO_RENDERER_H_

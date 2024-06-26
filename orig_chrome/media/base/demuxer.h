// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_DEMUXER_H_
#define MEDIA_BASE_DEMUXER_H_

#include <vector>

#include "base/time/time.h"
#include "media/base/data_source.h"
#include "media/base/demuxer_stream.h"
#include "media/base/demuxer_stream_provider.h"
#include "media/base/eme_constants.h"
#include "media/base/media_export.h"
#include "media/base/pipeline_status.h"

namespace media {

class TextTrackConfig;

class MEDIA_EXPORT DemuxerHost {
public:
    // Notify the host that time range [start,end] has been buffered.
    virtual void AddBufferedTimeRange(base::TimeDelta start,
        base::TimeDelta end)
        = 0;

    // Sets the duration of the media in microseconds.
    // Duration may be kInfiniteDuration() if the duration is not known.
    virtual void SetDuration(base::TimeDelta duration) = 0;

    // Stops execution of the pipeline due to a fatal error.  Do not call this
    // method with PIPELINE_OK.
    virtual void OnDemuxerError(PipelineStatus error) = 0;

    // Add |text_stream| to the collection managed by the text renderer.
    virtual void AddTextStream(DemuxerStream* text_stream,
        const TextTrackConfig& config)
        = 0;

    // Remove |text_stream| from the presentation.
    virtual void RemoveTextStream(DemuxerStream* text_stream) = 0;

protected:
    virtual ~DemuxerHost();
};

class MEDIA_EXPORT Demuxer : public DemuxerStreamProvider {
public:
    // A new potentially encrypted stream has been parsed.
    // First parameter - The type of initialization data.
    // Second parameter - The initialization data associated with the stream.
    typedef base::Callback<void(EmeInitDataType type,
        const std::vector<uint8>& init_data)>
        EncryptedMediaInitDataCB;

    Demuxer();
    ~Demuxer() override;

    // Returns the name of the demuxer for logging purpose.
    virtual std::string GetDisplayName() const = 0;

    // Completes initialization of the demuxer.
    //
    // The demuxer does not own |host| as it is guaranteed to outlive the
    // lifetime of the demuxer. Don't delete it!  |status_cb| must only be run
    // after this method has returned.
    virtual void Initialize(DemuxerHost* host,
        const PipelineStatusCB& status_cb,
        bool enable_text_tracks)
        = 0;

    // Carry out any actions required to seek to the given time, executing the
    // callback upon completion.
    virtual void Seek(base::TimeDelta time,
        const PipelineStatusCB& status_cb)
        = 0;

    // Stops this demuxer.
    //
    // After this call the demuxer may be destroyed. It is illegal to call any
    // method (including Stop()) after a demuxer has stopped.
    virtual void Stop() = 0;

    // Returns the starting time for the media file; it's always positive.
    virtual base::TimeDelta GetStartTime() const = 0;

    // Returns Time represented by presentation timestamp 0.
    // If the timstamps are not associated with a Time, then
    // a null Time is returned.
    virtual base::Time GetTimelineOffset() const = 0;

    // Returns the memory usage in bytes for the demuxer. May be called from any
    // thread.
    virtual int64_t GetMemoryUsage() const = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(Demuxer);
};

} // namespace media

#endif // MEDIA_BASE_DEMUXER_H_

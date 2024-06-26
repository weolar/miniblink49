// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_PIPELINE_H_
#define MEDIA_BASE_PIPELINE_H_

#include "base/gtest_prod_util.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/synchronization/lock.h"
#include "base/threading/thread_checker.h"
#include "base/time/default_tick_clock.h"
#include "media/base/buffering_state.h"
#include "media/base/cdm_context.h"
#include "media/base/demuxer.h"
#include "media/base/media_export.h"
#include "media/base/pipeline_status.h"
#include "media/base/ranges.h"
#include "media/base/serial_runner.h"
#include "media/base/text_track.h"
#include "media/base/video_rotation.h"
#include "ui/gfx/geometry/size.h"

namespace base {
class SingleThreadTaskRunner;
class TimeDelta;
}

namespace media {

class MediaLog;
class Renderer;
class TextRenderer;
class TextTrackConfig;
class TimeDeltaInterpolator;
class VideoFrame;

// Metadata describing a pipeline once it has been initialized.
struct PipelineMetadata {
    PipelineMetadata()
        : has_audio(false)
        , has_video(false)
        , video_rotation(VIDEO_ROTATION_0)
    {
    }

    bool has_audio;
    bool has_video;
    gfx::Size natural_size;
    VideoRotation video_rotation;
    base::Time timeline_offset;
};

typedef base::Callback<void(PipelineMetadata)> PipelineMetadataCB;

// Pipeline runs the media pipeline.  Filters are created and called on the
// task runner injected into this object. Pipeline works like a state
// machine to perform asynchronous initialization, pausing, seeking and playing.
//
// Here's a state diagram that describes the lifetime of this object.
//
//   [ *Created ]                       [ Any State ]
//         | Start()                         | Stop() / SetError()
//         V                                 V
//   [ InitXXX (for each filter) ]      [ Stopping ]
//         |                                 |
//         V                                 V
//   [ Playing ] <-- [ Seeking ]        [ Stopped ]
//         |               ^
//         `---------------'
//              Seek()
//
// Initialization is a series of state transitions from "Created" through each
// filter initialization state.  When all filter initialization states have
// completed, we are implicitly in a "Paused" state.  At that point we simulate
// a Seek() to the beginning of the media to give filters a chance to preroll.
// From then on the normal Seek() transitions are carried out and we start
// playing the media.
//
// If any error ever happens, this object will transition to the "Error" state
// from any state. If Stop() is ever called, this object will transition to
// "Stopped" state.
class MEDIA_EXPORT Pipeline : public DemuxerHost {
public:
    // Used to paint VideoFrame.
    typedef base::Callback<void(const scoped_refptr<VideoFrame>&)> PaintCB;

    // Constructs a media pipeline that will execute on |task_runner|.
    Pipeline(const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
        MediaLog* media_log);
    ~Pipeline() override;

    // Build a pipeline to using the given |demuxer| and |renderer| to construct
    // a filter chain, executing |seek_cb| when the initial seek has completed.
    //
    // The following permanent callbacks will be executed as follows up until
    // Stop() has completed:
    //   |ended_cb| will be executed whenever the media reaches the end.
    //   |error_cb| will be executed whenever an error occurs but hasn't been
    //              reported already through another callback.
    //   |metadata_cb| will be executed when the content duration, container video
    //                 size, start time, and whether the content has audio and/or
    //                 video in supported formats are known.
    //   |buffering_state_cb| will be executed whenever there are changes in the
    //                        overall buffering state of the pipeline.
    //   |duration_change_cb| optional callback that will be executed whenever the
    //                        presentation duration changes.
    //   |add_text_track_cb| will be executed whenever a text track is added.
    //   |waiting_for_decryption_key_cb| will be executed whenever the key needed
    //                                   to decrypt the stream is not available.
    // It is an error to call this method after the pipeline has already started.
    void Start(Demuxer* demuxer,
        scoped_ptr<Renderer> renderer,
        const base::Closure& ended_cb,
        const PipelineStatusCB& error_cb,
        const PipelineStatusCB& seek_cb,
        const PipelineMetadataCB& metadata_cb,
        const BufferingStateCB& buffering_state_cb,
        const base::Closure& duration_change_cb,
        const AddTextTrackCB& add_text_track_cb,
        const base::Closure& waiting_for_decryption_key_cb);

    // Asynchronously stops the pipeline, executing |stop_cb| when the pipeline
    // teardown has completed.
    //
    // Stop() must complete before destroying the pipeline. It it permissible to
    // call Stop() at any point during the lifetime of the pipeline.
    //
    // It is safe to delete the pipeline during the execution of |stop_cb|.
    void Stop(const base::Closure& stop_cb);

    // Attempt to seek to the position specified by time.  |seek_cb| will be
    // executed when the all filters in the pipeline have processed the seek.
    //
    // Clients are expected to call GetMediaTime() to check whether the seek
    // succeeded.
    //
    // It is an error to call this method if the pipeline has not started.
    void Seek(base::TimeDelta time, const PipelineStatusCB& seek_cb);

    // Returns true if the pipeline has been started via Start().  If IsRunning()
    // returns true, it is expected that Stop() will be called before destroying
    // the pipeline.
    bool IsRunning() const;

    // Gets the current playback rate of the pipeline.  When the pipeline is
    // started, the playback rate will be 0.0.  A rate of 1.0 indicates
    // that the pipeline is rendering the media at the standard rate.  Valid
    // values for playback rate are >= 0.0.
    double GetPlaybackRate() const;

    // Attempt to adjust the playback rate. Setting a playback rate of 0.0 pauses
    // all rendering of the media.  A rate of 1.0 indicates a normal playback
    // rate.  Values for the playback rate must be greater than or equal to 0.0.
    //
    // TODO(scherkus): What about maximum rate?  Does HTML5 specify a max?
    void SetPlaybackRate(double playback_rate);

    // Gets the current volume setting being used by the audio renderer.  When
    // the pipeline is started, this value will be 1.0f.  Valid values range
    // from 0.0f to 1.0f.
    float GetVolume() const;

    // Attempt to set the volume of the audio renderer.  Valid values for volume
    // range from 0.0f (muted) to 1.0f (full volume).  This value affects all
    // channels proportionately for multi-channel audio streams.
    void SetVolume(float volume);

    // Returns the current media playback time, which progresses from 0 until
    // GetMediaDuration().
    base::TimeDelta GetMediaTime() const;

    // Get approximate time ranges of buffered media.
    Ranges<base::TimeDelta> GetBufferedTimeRanges() const;

    // Get the duration of the media in microseconds.  If the duration has not
    // been determined yet, then returns 0.
    base::TimeDelta GetMediaDuration() const;

    // Return true if loading progress has been made since the last time this
    // method was called.
    bool DidLoadingProgress();

    // Gets the current pipeline statistics.
    PipelineStatistics GetStatistics() const;

    void SetCdm(CdmContext* cdm_context, const CdmAttachedCB& cdm_attached_cb);

    void SetErrorForTesting(PipelineStatus status);
    bool HasWeakPtrsForTesting() const;

private:
    FRIEND_TEST_ALL_PREFIXES(PipelineTest, GetBufferedTimeRanges);
    FRIEND_TEST_ALL_PREFIXES(PipelineTest, EndedCallback);
    FRIEND_TEST_ALL_PREFIXES(PipelineTest, AudioStreamShorterThanVideo);
    friend class MediaLog;

    // Pipeline states, as described above.
    enum State {
        kCreated,
        kInitDemuxer,
        kInitRenderer,
        kSeeking,
        kPlaying,
        kStopping,
        kStopped,
    };

    // Updates |state_|. All state transitions should use this call.
    void SetState(State next_state);

    static const char* GetStateString(State state);
    State GetNextState() const;

    // Helper method that runs & resets |seek_cb_| and resets |seek_timestamp_|
    // and |seek_pending_|.
    void FinishSeek();

    // DemuxerHost implementaion.
    void AddBufferedTimeRange(base::TimeDelta start,
        base::TimeDelta end) override;
    void SetDuration(base::TimeDelta duration) override;
    void OnDemuxerError(PipelineStatus error) override;
    void AddTextStream(DemuxerStream* text_stream,
        const TextTrackConfig& config) override;
    void RemoveTextStream(DemuxerStream* text_stream) override;

    // Callback executed when a rendering error happened, initiating the teardown
    // sequence.
    void OnError(PipelineStatus error);

    // Callback executed by filters to update statistics.
    void OnUpdateStatistics(const PipelineStatistics& stats_delta);

    // The following "task" methods correspond to the public methods, but these
    // methods are run as the result of posting a task to the Pipeline's
    // task runner.
    void StartTask();

    // Stops and destroys all filters, placing the pipeline in the kStopped state.
    void StopTask(const base::Closure& stop_cb);

    // Carries out stopping and destroying all filters, placing the pipeline in
    // the kStopped state.
    void ErrorChangedTask(PipelineStatus error);

    // Carries out notifying filters that the playback rate has changed.
    void PlaybackRateChangedTask(double playback_rate);

    // Carries out notifying filters that the volume has changed.
    void VolumeChangedTask(float volume);

    // Carries out notifying filters that we are seeking to a new timestamp.
    void SeekTask(base::TimeDelta time, const PipelineStatusCB& seek_cb);

    // Carries out setting the |cdm_context| in |renderer_|, and then fires
    // |cdm_attached_cb| with the result. If |renderer_| is null,
    // |cdm_attached_cb| will be fired immediately with true, and |cdm_context|
    // will be set in |renderer_| later when |renderer_| is created.
    void SetCdmTask(CdmContext* cdm_context,
        const CdmAttachedCB& cdm_attached_cb);

    // Callbacks executed when a renderer has ended.
    void OnRendererEnded();
    void OnTextRendererEnded();
    void RunEndedCallbackIfNeeded();

    scoped_ptr<TextRenderer> CreateTextRenderer();

    // Carries out adding a new text stream to the text renderer.
    void AddTextStreamTask(DemuxerStream* text_stream,
        const TextTrackConfig& config);

    // Carries out removing a text stream from the text renderer.
    void RemoveTextStreamTask(DemuxerStream* text_stream);

    // Callbacks executed when a text track is added.
    void OnAddTextTrack(const TextTrackConfig& config,
        const AddTextTrackDoneCB& done_cb);

    // Kicks off initialization for each media object, executing |done_cb| with
    // the result when completed.
    void InitializeDemuxer(const PipelineStatusCB& done_cb);
    void InitializeRenderer(const PipelineStatusCB& done_cb);

    void StateTransitionTask(PipelineStatus status);

    // Initiates an asynchronous pause-flush-seek-preroll call sequence
    // executing |done_cb| with the final status when completed.
    void DoSeek(base::TimeDelta seek_timestamp, const PipelineStatusCB& done_cb);

    // Initiates an asynchronous pause-flush-stop call sequence executing
    // |done_cb| when completed.
    void DoStop(const PipelineStatusCB& done_cb);
    void OnStopCompleted(PipelineStatus status);

    void ReportMetadata();

    void BufferingStateChanged(BufferingState new_buffering_state);

    // Task runner used to execute pipeline tasks.
    scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

    // MediaLog to which to log events.
    scoped_refptr<MediaLog> media_log_;

    // Lock used to serialize access for the following data members.
    mutable base::Lock lock_;

    // Whether or not the pipeline is running.
    bool running_;

    // Amount of available buffered data as reported by |demuxer_|.
    Ranges<base::TimeDelta> buffered_time_ranges_;

    // True when AddBufferedTimeRange() has been called more recently than
    // DidLoadingProgress().
    bool did_loading_progress_;

    // Current volume level (from 0.0f to 1.0f).  This value is set immediately
    // via SetVolume() and a task is dispatched on the task runner to notify the
    // filters.
    float volume_;

    // Current playback rate (>= 0.0).  This value is set immediately via
    // SetPlaybackRate() and a task is dispatched on the task runner to notify
    // the filters.
    double playback_rate_;

    // Current duration as reported by |demuxer_|.
    base::TimeDelta duration_;

    // Status of the pipeline.  Initialized to PIPELINE_OK which indicates that
    // the pipeline is operating correctly. Any other value indicates that the
    // pipeline is stopped or is stopping.  Clients can call the Stop() method to
    // reset the pipeline state, and restore this to PIPELINE_OK.
    PipelineStatus status_;

    // The following data members are only accessed by tasks posted to
    // |task_runner_|.

    // Member that tracks the current state.
    State state_;

    // The timestamp to start playback from after starting/seeking has completed.
    base::TimeDelta start_timestamp_;

    // Whether we've received the audio/video/text ended events.
    bool renderer_ended_;
    bool text_renderer_ended_;

    // Temporary callback used for Start() and Seek().
    PipelineStatusCB seek_cb_;

    // Temporary callback used for Stop().
    base::Closure stop_cb_;

    // Permanent callbacks passed in via Start().
    base::Closure ended_cb_;
    PipelineStatusCB error_cb_;
    PipelineMetadataCB metadata_cb_;
    BufferingStateCB buffering_state_cb_;
    base::Closure duration_change_cb_;
    AddTextTrackCB add_text_track_cb_;
    base::Closure waiting_for_decryption_key_cb_;

    // Holds the initialized demuxer. Used for seeking. Owned by client.
    Demuxer* demuxer_;

    // Holds the initialized renderers. Used for setting the volume,
    // playback rate, and determining when playback has finished.
    scoped_ptr<Renderer> renderer_;
    scoped_ptr<TextRenderer> text_renderer_;

    PipelineStatistics statistics_;

    scoped_ptr<SerialRunner> pending_callbacks_;

    // CdmContext to be used to decrypt (and decode) encrypted stream in this
    // pipeline. Non-null only when SetCdm() is called and the pipeline has not
    // been started. Then during Start(), this value will be set on |renderer_|.
    CdmContext* pending_cdm_context_;

    base::ThreadChecker thread_checker_;

    // NOTE: Weak pointers must be invalidated before all other member variables.
    base::WeakPtrFactory<Pipeline> weak_factory_;

    DISALLOW_COPY_AND_ASSIGN(Pipeline);
};

} // namespace media

#endif // MEDIA_BASE_PIPELINE_H_

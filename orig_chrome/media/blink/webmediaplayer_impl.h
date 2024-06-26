// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_WEBMEDIAPLAYER_IMPL_H_
#define MEDIA_BLINK_WEBMEDIAPLAYER_IMPL_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread.h"
#include "media/base/cdm_factory.h"
#include "media/base/pipeline.h"
#include "media/base/renderer_factory.h"
#include "media/base/text_track.h"
#include "media/blink/buffered_data_source.h"
#include "media/blink/buffered_data_source_host_impl.h"
#include "media/blink/encrypted_media_player_support.h"
#include "media/blink/media_blink_export.h"
#include "media/blink/video_frame_compositor.h"
#include "media/blink/webmediaplayer_params.h"
#include "media/blink/webmediaplayer_util.h"
#include "media/renderers/skcanvas_video_renderer.h"
#include "third_party/WebKit/public/platform/WebAudioSourceProvider.h"
#include "third_party/WebKit/public/platform/WebContentDecryptionModuleResult.h"
#include "third_party/WebKit/public/platform/WebMediaPlayer.h"
//#include "url/gurl.h"

namespace blink {
class WebGraphicsContext3D;
class WebLocalFrame;
class WebMediaPlayerClient;
class WebMediaPlayerEncryptedMediaClient;
}

namespace base {
class SingleThreadTaskRunner;
class TaskRunner;
}

namespace cc_blink {
class WebLayerImpl;
}

namespace media {

class AudioHardwareConfig;
class ChunkDemuxer;
class GpuVideoAcceleratorFactories;
class MediaLog;
class VideoFrameCompositor;
class WebAudioSourceProviderImpl;
class WebMediaPlayerDelegate;
class WebTextTrackImpl;

// The canonical implementation of blink::WebMediaPlayer that's backed by
// Pipeline. Handles normal resource loading, Media Source, and
// Encrypted Media.
class MEDIA_BLINK_EXPORT WebMediaPlayerImpl
    : public NON_EXPORTED_BASE(blink::WebMediaPlayer),
      public base::SupportsWeakPtr<WebMediaPlayerImpl> {
public:
    // Constructs a WebMediaPlayer implementation using Chromium's media stack.
    // |delegate| may be null. |renderer| may also be null, in which case an
    // internal renderer will be created.
    // TODO(xhwang): Drop the internal renderer path and always pass in a renderer
    // here.
    WebMediaPlayerImpl(
        blink::WebLocalFrame* frame,
        blink::WebMediaPlayerClient* client,
        blink::WebMediaPlayerEncryptedMediaClient* encrypted_client,
        base::WeakPtr<WebMediaPlayerDelegate> delegate,
        scoped_ptr<RendererFactory> renderer_factory,
        CdmFactory* cdm_factory,
        const WebMediaPlayerParams& params);
    ~WebMediaPlayerImpl() override;

    void load(LoadType load_type, const blink::WebURL& url, CORSMode cors_mode, bool) override;

    // Playback controls.
    void play() override;
    void pause() override;
    bool supportsSave() const override;
    void seek(double seconds) override;
    void setRate(double rate) override;
    void setVolume(double volume) override;
    void setSinkId(const blink::WebString& sink_id,
        //const blink::WebSecurityOrigin& security_origin,
        blink::WebSetSinkIdCallbacks* web_callback) override;
    void setPreload(blink::WebMediaPlayer::Preload preload) override;
    blink::WebTimeRanges buffered() const override;
    blink::WebTimeRanges seekable() const override;

#ifndef MINIBLINK_NO_CHANGE
    void setContentsToNativeWindowOffset(const blink::WebPoint& p) override
    {
    }
    bool handleMouseEvent(const blink::WebMouseEvent& evt) override
    {
        return true;
    }
    bool handleKeyboardEvent(const blink::WebKeyboardEvent& evt) override
    {
        return true;
    }
    void showMediaControls() override { }
    void hideMediaControls() override { }
#endif

    // Methods for painting.
    void paint(blink::WebCanvas* canvas,
        const blink::WebRect& rect,
        unsigned char alpha,
        SkXfermode::Mode mode) override;

    // True if the loaded media has a playable video/audio track.
    bool hasVideo() const override;
    bool hasAudio() const override;

    // Dimensions of the video.
    blink::WebSize naturalSize() const override;

    // Getters of playback state.
    bool paused() const override;
    bool seeking() const override;
    double duration() const override;
    virtual double timelineOffset() const;
    double currentTime() const override;

    // Internal states of loading and network.
    // TODO(hclam): Ask the pipeline about the state rather than having reading
    // them from members which would cause race conditions.
    blink::WebMediaPlayer::NetworkState networkState() const override;
    blink::WebMediaPlayer::ReadyState readyState() const override;

    bool didLoadingProgress() override;

    bool hasSingleSecurityOrigin() const override;
    bool didPassCORSAccessCheck() const override;

    double mediaTimeForTimeValue(double timeValue) const override;

    unsigned decodedFrameCount() const override;
    unsigned droppedFrameCount() const override;
    unsigned audioDecodedByteCount() const override;
    unsigned videoDecodedByteCount() const override;

    bool copyVideoTextureToPlatformTexture(
        blink::WebGraphicsContext3D* web_graphics_context,
        unsigned int texture,
        unsigned int internal_format,
        unsigned int type,
        bool premultiply_alpha,
        bool flip_y) override;

    blink::WebAudioSourceProvider* audioSourceProvider() override;

    MediaKeyException generateKeyRequest(
        const blink::WebString& key_system,
        const unsigned char* init_data,
        unsigned init_data_length) override;

    MediaKeyException addKey(const blink::WebString& key_system,
        const unsigned char* key,
        unsigned key_length,
        const unsigned char* init_data,
        unsigned init_data_length,
        const blink::WebString& session_id) override;

    MediaKeyException cancelKeyRequest(
        const blink::WebString& key_system,
        const blink::WebString& session_id) override;

    void setContentDecryptionModule(
        blink::WebContentDecryptionModule* cdm,
        blink::WebContentDecryptionModuleResult result) override;

    void OnPipelineSeeked(bool time_changed, PipelineStatus status);
    void OnPipelineEnded();
    void OnPipelineError(PipelineStatus error);
    void OnPipelineMetadata(PipelineMetadata metadata);
    void OnPipelineBufferingStateChanged(BufferingState buffering_state);
    void OnDemuxerOpened();
    void OnAddTextTrack(const TextTrackConfig& config,
        const AddTextTrackDoneCB& done_cb);

private:
    // Called after |defer_load_cb_| has decided to allow the load. If
    // |defer_load_cb_| is null this is called immediately.
    void DoLoad(LoadType load_type,
        const blink::WebURL& url,
        CORSMode cors_mode);

    // Called after asynchronous initialization of a data source completed.
    void DataSourceInitialized(bool success);

    // Called when the data source is downloading or paused.
    void NotifyDownloading(bool is_downloading);

    // Creates a Renderer that will be used by the |pipeline_|.
    scoped_ptr<Renderer> CreateRenderer();

    // Finishes starting the pipeline due to a call to load().
    void StartPipeline();

    // Helpers that set the network/ready state and notifies the client if
    // they've changed.
    void SetNetworkState(blink::WebMediaPlayer::NetworkState state);
    void SetReadyState(blink::WebMediaPlayer::ReadyState state);

    // Gets the duration value reported by the pipeline.
    double GetPipelineDuration() const;

    // Callbacks from |pipeline_| that are forwarded to |client_|.
    void OnDurationChanged();
    void OnNaturalSizeChanged(gfx::Size size);
    void OnOpacityChanged(bool opaque);

    // Called by VideoRendererImpl on its internal thread with the new frame to be
    // painted.
    void FrameReady(const scoped_refptr<VideoFrame>& frame);

    // Returns the current video frame from |compositor_|. Blocks until the
    // compositor can return the frame.
    scoped_refptr<VideoFrame> GetCurrentFrameFromCompositor();

    // Called when the demuxer encounters encrypted streams.
    void OnEncryptedMediaInitData(EmeInitDataType init_data_type,
        const std::vector<uint8>& init_data);

    // Called when a decoder detects that the key needed to decrypt the stream
    // is not available.
    void OnWaitingForDecryptionKey();

    // Sets |cdm_context| on the pipeline and fires |cdm_attached_cb| when done.
    // Parameter order is reversed for easy binding.
    void SetCdm(const CdmAttachedCB& cdm_attached_cb, CdmContext* cdm_context);

    // Called when a CDM has been attached to the |pipeline_|.
    void OnCdmAttached(bool success);

    // Updates |paused_time_| to the current media time with consideration for the
    // |ended_| state by clamping current time to duration upon |ended_|.
    void UpdatePausedTime();

    // Notifies |delegate_| that playback has started or was paused; also starts
    // or stops the memory usage reporting timer respectively.
    void NotifyPlaybackStarted();
    void NotifyPlaybackPaused();

    // Called at low frequency to tell external observers how much memory we're
    // using for video playback.  Called by |memory_usage_reporting_timer_|.
    void ReportMemoryUsage();

    blink::WebLocalFrame* frame_;

    // TODO(hclam): get rid of these members and read from the pipeline directly.
    blink::WebMediaPlayer::NetworkState network_state_;
    blink::WebMediaPlayer::ReadyState ready_state_;

    // Preload state for when |data_source_| is created after setPreload().
    BufferedDataSource::Preload preload_;

    // Task runner for posting tasks on Chrome's main thread. Also used
    // for DCHECKs so methods calls won't execute in the wrong thread.
    const scoped_refptr<base::SingleThreadTaskRunner> main_task_runner_;

    scoped_refptr<base::SingleThreadTaskRunner> media_task_runner_;
    scoped_refptr<base::TaskRunner> worker_task_runner_;
    scoped_refptr<MediaLog> media_log_;
    Pipeline pipeline_;

    // The LoadType passed in the |load_type| parameter of the load() call.
    LoadType load_type_;

    // Cache of metadata for answering hasAudio(), hasVideo(), and naturalSize().
    PipelineMetadata pipeline_metadata_;

    // Whether the video is known to be opaque or not.
    bool opaque_;

    // Playback state.
    //
    // TODO(scherkus): we have these because Pipeline favours the simplicity of a
    // single "playback rate" over worrying about paused/stopped etc...  It forces
    // all clients to manage the pause+playback rate externally, but is that
    // really a bad thing?
    //
    // TODO(scherkus): since SetPlaybackRate(0) is asynchronous and we don't want
    // to hang the render thread during pause(), we record the time at the same
    // time we pause and then return that value in currentTime().  Otherwise our
    // clock can creep forward a little bit while the asynchronous
    // SetPlaybackRate(0) is being executed.
    double playback_rate_;
    bool paused_;
    base::TimeDelta paused_time_;
    bool seeking_;
    base::TimeDelta seek_time_; // Meaningless when |seeking_| is false.

    // TODO(scherkus): Replace with an explicit ended signal to HTMLMediaElement,
    // see http://crbug.com/409280
    bool ended_;

    // Seek gets pending if another seek is in progress. Only last pending seek
    // will have effect.
    bool pending_seek_;
    // |pending_seek_time_| is meaningless when |pending_seek_| is false.
    base::TimeDelta pending_seek_time_;

    // Tracks whether to issue time changed notifications during buffering state
    // changes.
    bool should_notify_time_changed_;

    blink::WebMediaPlayerClient* client_;
    blink::WebMediaPlayerEncryptedMediaClient* encrypted_client_;

    base::WeakPtr<WebMediaPlayerDelegate> delegate_;

    WebMediaPlayerParams::DeferLoadCB defer_load_cb_;
    WebMediaPlayerParams::Context3DCB context_3d_cb_;

    // Members for notifying upstream clients about internal memory usage.  The
    // |adjust_allocated_memory_cb_| must only be called on |main_task_runner_|.
    base::RepeatingTimer memory_usage_reporting_timer_;
    WebMediaPlayerParams::AdjustAllocatedMemoryCB adjust_allocated_memory_cb_;
    int64_t last_reported_memory_usage_;

    // Routes audio playback to either AudioRendererSink or WebAudio.
    scoped_refptr<WebAudioSourceProviderImpl> audio_source_provider_;

    bool supports_save_;

    // These two are mutually exclusive:
    //   |data_source_| is used for regular resource loads.
    //   |chunk_demuxer_| is used for Media Source resource loads.
    //
    // |demuxer_| will contain the appropriate demuxer based on which resource
    // load strategy we're using.
    scoped_ptr<BufferedDataSource> data_source_;
    scoped_ptr<Demuxer> demuxer_;
    ChunkDemuxer* chunk_demuxer_;

    BufferedDataSourceHostImpl buffered_data_source_host_;

    // Video rendering members.
    scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner_;
    VideoFrameCompositor* compositor_; // Deleted on |compositor_task_runner_|.
    SkCanvasVideoRenderer skcanvas_video_renderer_;

    // The compositor layer for displaying the video content when using composited
    // playback.
    scoped_ptr<cc_blink::WebLayerImpl> video_weblayer_;

    EncryptedMediaPlayerSupport encrypted_media_support_;

    scoped_ptr<blink::WebContentDecryptionModuleResult> set_cdm_result_;

    scoped_ptr<RendererFactory> renderer_factory_;

    DISALLOW_COPY_AND_ASSIGN(WebMediaPlayerImpl);
};

} // namespace media

#endif // MEDIA_BLINK_WEBMEDIAPLAYER_IMPL_H_

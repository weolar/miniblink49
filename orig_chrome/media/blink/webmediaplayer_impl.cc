// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/blink/webmediaplayer_impl.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>

#include "base/bind.h"
#include "base/callback.h"
#include "base/callback_helpers.h"
#include "base/debug/alias.h"
#include "base/debug/crash_logging.h"
#include "base/metrics/histogram.h"
#include "base/single_thread_task_runner.h"
#include "base/synchronization/waitable_event.h"
#include "base/thread_task_runner_handle.h"
#include "base/trace_event/trace_event.h"
#include "cc/blink/web_layer_impl.h"
#include "cc/layers/video_layer.h"
#include "gpu/blink/webgraphicscontext3d_impl.h"
#include "media/audio/null_audio_sink.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/cdm_context.h"
#include "media/base/limits.h"
#include "media/base/media_log.h"
#include "media/base/text_renderer.h"
#include "media/base/timestamp_constants.h"
#include "media/base/video_frame.h"
#include "media/blink/texttrack_impl.h"
#include "media/blink/webaudiosourceprovider_impl.h"
#include "media/blink/webcontentdecryptionmodule_impl.h"
#include "media/blink/webinbandtexttrack_impl.h"
#include "media/blink/webmediaplayer_delegate.h"
#include "media/blink/webmediaplayer_util.h"
#include "media/blink/webmediasource_impl.h"
#include "media/filters/chunk_demuxer.h"
#include "media/filters/ffmpeg_demuxer.h"
#include "third_party/WebKit/public/platform/WebEncryptedMediaTypes.h"
#include "third_party/WebKit/public/platform/WebMediaPlayerClient.h"
#include "third_party/WebKit/public/platform/WebMediaPlayerEncryptedMediaClient.h"
#include "third_party/WebKit/public/platform/WebMediaSource.h"
#include "third_party/WebKit/public/platform/WebRect.h"
#include "third_party/WebKit/public/platform/WebSize.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/platform/WebURL.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebRuntimeFeatures.h"
#include "third_party/WebKit/public/web/WebSecurityOrigin.h"
#include "third_party/WebKit/public/web/WebView.h"

using blink::WebCanvas;
using blink::WebMediaPlayer;
using blink::WebRect;
using blink::WebSize;
using blink::WebString;

namespace {

// Limits the range of playback rate.
//
// TODO(kylep): Revisit these.
//
// Vista has substantially lower performance than XP or Windows7.  If you speed
// up a video too much, it can't keep up, and rendering stops updating except on
// the time bar. For really high speeds, audio becomes a bottleneck and we just
// use up the data we have, which may not achieve the speed requested, but will
// not crash the tab.
//
// A very slow speed, ie 0.00000001x, causes the machine to lock up. (It seems
// like a busy loop). It gets unresponsive, although its not completely dead.
//
// Also our timers are not very accurate (especially for ogg), which becomes
// evident at low speeds and on Vista. Since other speeds are risky and outside
// the norms, we think 1/16x to 16x is a safe and useful range for now.
const double kMinRate = 0.0625;
const double kMaxRate = 16.0;

void SetSinkIdOnMediaThread(
    scoped_refptr<media::WebAudioSourceProviderImpl> sink,
    const std::string& device_id,
    const /*url::Origin*/ std::string& security_origin,
    const media::SwitchOutputDeviceCB& callback)
{
    if (sink->GetOutputDevice()) {
        sink->GetOutputDevice()->SwitchOutputDevice(device_id, security_origin,
            callback);
    } else {
        callback.Run(media::OUTPUT_DEVICE_STATUS_ERROR_INTERNAL);
    }
}

} // namespace

namespace media {

class BufferedDataSourceHostImpl;

#define STATIC_ASSERT_MATCHING_ENUM(name)                                                                                \
    static_assert(static_cast<int>(WebMediaPlayer::CORSMode##name) == static_cast<int>(BufferedResourceLoader::k##name), \
        "mismatching enum values: " #name)
STATIC_ASSERT_MATCHING_ENUM(Unspecified);
STATIC_ASSERT_MATCHING_ENUM(Anonymous);
STATIC_ASSERT_MATCHING_ENUM(UseCredentials);
#undef STATIC_ASSERT_MATCHING_ENUM

#define BIND_TO_RENDER_LOOP(function)                     \
    (DCHECK(main_task_runner_->BelongsToCurrentThread()), \
        BindToCurrentLoop(base::Bind(function, AsWeakPtr())))

#define BIND_TO_RENDER_LOOP1(function, arg1)              \
    (DCHECK(main_task_runner_->BelongsToCurrentThread()), \
        BindToCurrentLoop(base::Bind(function, AsWeakPtr(), arg1)))

WebMediaPlayerImpl::WebMediaPlayerImpl(
    blink::WebLocalFrame* frame,
    blink::WebMediaPlayerClient* client,
    blink::WebMediaPlayerEncryptedMediaClient* encrypted_client,
    base::WeakPtr<WebMediaPlayerDelegate> delegate,
    scoped_ptr<RendererFactory> renderer_factory,
    CdmFactory* cdm_factory,
    const WebMediaPlayerParams& params)
    : frame_(frame)
    , network_state_(WebMediaPlayer::NetworkStateEmpty)
    , ready_state_(WebMediaPlayer::ReadyStateHaveNothing)
    , preload_(BufferedDataSource::AUTO)
    , main_task_runner_(base::ThreadTaskRunnerHandle::Get())
    , media_task_runner_(params.media_task_runner())
    , worker_task_runner_(params.worker_task_runner())
    , media_log_(params.media_log())
    , pipeline_(media_task_runner_, media_log_.get())
    , load_type_(LoadTypeURL)
    , opaque_(false)
    , playback_rate_(0.0)
    , paused_(true)
    , seeking_(false)
    , ended_(false)
    , pending_seek_(false)
    , should_notify_time_changed_(false)
    , client_(client)
    , encrypted_client_(encrypted_client)
    , delegate_(delegate)
    , defer_load_cb_(params.defer_load_cb())
    , context_3d_cb_(params.context_3d_cb())
    , adjust_allocated_memory_cb_(params.adjust_allocated_memory_cb())
    , last_reported_memory_usage_(0)
    , supports_save_(true)
    , chunk_demuxer_(NULL)
    ,
    // Threaded compositing isn't enabled universally yet.
    compositor_task_runner_(
        params.compositor_task_runner()
            ? params.compositor_task_runner()
            : base::MessageLoop::current()->task_runner())
    , compositor_(new VideoFrameCompositor(
          compositor_task_runner_,
          BIND_TO_RENDER_LOOP(&WebMediaPlayerImpl::OnNaturalSizeChanged),
          BIND_TO_RENDER_LOOP(&WebMediaPlayerImpl::OnOpacityChanged)))
    , encrypted_media_support_(cdm_factory,
          encrypted_client,
          params.media_permission(),
          base::Bind(&WebMediaPlayerImpl::SetCdm,
              AsWeakPtr(),
              base::Bind(&IgnoreCdmAttached)))
    , renderer_factory_(renderer_factory.Pass())
{
    DCHECK(!adjust_allocated_memory_cb_.is_null());

    media_log_->AddEvent(
        media_log_->CreateEvent(MediaLogEvent::WEBMEDIAPLAYER_CREATED));

    if (params.initial_cdm()) {
        //         SetCdm(base::Bind(&IgnoreCdmAttached),
        //             ToWebContentDecryptionModuleImpl(params.initial_cdm())
        //                 ->GetCdmContext());
        DebugBreak();
    }

    // TODO(xhwang): When we use an external Renderer, many methods won't work,
    // e.g. GetCurrentFrameFromCompositor(). See http://crbug.com/434861

    // Use the null sink if no sink was provided.
    audio_source_provider_ = new WebAudioSourceProviderImpl(
        params.audio_renderer_sink().get()
            ? params.audio_renderer_sink()
            : new NullAudioSink(media_task_runner_));
}

WebMediaPlayerImpl::~WebMediaPlayerImpl()
{
    client_->setWebLayer(NULL);

    DCHECK(main_task_runner_->BelongsToCurrentThread());

    if (delegate_)
        delegate_->PlayerGone(this);

    // Abort any pending IO so stopping the pipeline doesn't get blocked.
    if (data_source_)
        data_source_->Abort();
    if (chunk_demuxer_) {
        chunk_demuxer_->Shutdown();
        chunk_demuxer_ = NULL;
    }

    renderer_factory_.reset();

    // Make sure to kill the pipeline so there's no more media threads running.
    // Note: stopping the pipeline might block for a long time.
    base::WaitableEvent waiter(false, false);
    pipeline_.Stop(
        base::Bind(&base::WaitableEvent::Signal, base::Unretained(&waiter)));
    waiter.Wait();

    if (last_reported_memory_usage_)
        adjust_allocated_memory_cb_.Run(-last_reported_memory_usage_);

    compositor_task_runner_->DeleteSoon(FROM_HERE, compositor_);

    media_log_->AddEvent(
        media_log_->CreateEvent(MediaLogEvent::WEBMEDIAPLAYER_DESTROYED));
}

void WebMediaPlayerImpl::load(LoadType load_type, const blink::WebURL& url, CORSMode cors_mode, bool)
{
    //DVLOG(1) << __FUNCTION__ << "(" << load_type << ", " << url << ", " << cors_mode << ")";
    if (!defer_load_cb_.is_null()) {
        defer_load_cb_.Run(base::Bind(
            &WebMediaPlayerImpl::DoLoad, AsWeakPtr(), load_type, url, cors_mode));
        return;
    }
    DoLoad(load_type, url, cors_mode);
}

void WebMediaPlayerImpl::DoLoad(LoadType load_type,
    const blink::WebURL& url,
    CORSMode cors_mode)
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    //     GURL gurl(url);
    //     ReportMetrics(load_type, gurl, GURL(frame_->document().securityOrigin().toString()));
    //
    //     // Set subresource URL for crash reporting.
    //     base::debug::SetCrashKeyValue("subresource_url", gurl.spec());

    load_type_ = load_type;

    SetNetworkState(WebMediaPlayer::NetworkStateLoading);
    SetReadyState(WebMediaPlayer::ReadyStateHaveNothing);
    media_log_->AddEvent(media_log_->CreateLoadEvent(url.spec()));

    // Media source pipelines can start immediately.
    if (load_type == LoadTypeMediaSource) {
        supports_save_ = false;
        StartPipeline();
        return;
    }

    // Otherwise it's a regular request which requires resolving the URL first.
    data_source_.reset(new BufferedDataSource(
        url,
        static_cast<BufferedResourceLoader::CORSMode>(cors_mode),
        main_task_runner_,
        frame_,
        media_log_.get(),
        &buffered_data_source_host_,
        base::Bind(&WebMediaPlayerImpl::NotifyDownloading, AsWeakPtr())));
    data_source_->SetPreload(preload_);
    data_source_->Initialize(
        base::Bind(&WebMediaPlayerImpl::DataSourceInitialized, AsWeakPtr()));
}

void WebMediaPlayerImpl::play()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    paused_ = false;
    pipeline_.SetPlaybackRate(playback_rate_);
    if (data_source_)
        data_source_->MediaIsPlaying();

    media_log_->AddEvent(media_log_->CreateEvent(MediaLogEvent::PLAY));

    if (delegate_ && playback_rate_ > 0)
        NotifyPlaybackStarted();
}

void WebMediaPlayerImpl::pause()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    const bool was_already_paused = paused_ || playback_rate_ == 0;
    paused_ = true;
    pipeline_.SetPlaybackRate(0.0);
    if (data_source_)
        data_source_->MediaIsPaused();
    UpdatePausedTime();

    media_log_->AddEvent(media_log_->CreateEvent(MediaLogEvent::PAUSE));

    if (!was_already_paused && delegate_)
        NotifyPlaybackPaused();
}

bool WebMediaPlayerImpl::supportsSave() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());
    return supports_save_;
}

void WebMediaPlayerImpl::seek(double seconds)
{
    DVLOG(1) << __FUNCTION__ << "(" << seconds << "s)";
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    ended_ = false;

    ReadyState old_state = ready_state_;
    if (ready_state_ > WebMediaPlayer::ReadyStateHaveMetadata)
        SetReadyState(WebMediaPlayer::ReadyStateHaveMetadata);

    base::TimeDelta new_seek_time = base::TimeDelta::FromSecondsD(seconds);

    if (seeking_) {
        if (new_seek_time == seek_time_) {
            if (chunk_demuxer_) {
                // Don't suppress any redundant in-progress MSE seek. There could have
                // been changes to the underlying buffers after seeking the demuxer and
                // before receiving OnPipelineSeeked() for the currently in-progress
                // seek.
                //                 MEDIA_LOG(DEBUG, media_log_)
                //                     << "Detected MediaSource seek to same time as in-progress seek to "
                //                     << seek_time_ << ".";
            } else {
                // Suppress all redundant seeks if unrestricted by media source demuxer
                // API.
                pending_seek_ = false;
                pending_seek_time_ = base::TimeDelta();
                return;
            }
        }

        pending_seek_ = true;
        pending_seek_time_ = new_seek_time;
        if (chunk_demuxer_)
            chunk_demuxer_->CancelPendingSeek(pending_seek_time_);
        return;
    }

    media_log_->AddEvent(media_log_->CreateSeekEvent(seconds));

    // Update our paused time.
    // For non-MSE playbacks, in paused state ignore the seek operations to
    // current time if the loading is completed and generate
    // OnPipelineBufferingStateChanged event to eventually fire seeking and seeked
    // events. We don't short-circuit MSE seeks in this logic because the
    // underlying buffers around the seek time might have changed (or even been
    // removed) since previous seek/preroll/pause action, and the pipeline might
    // need to flush so the new buffers are decoded and rendered instead of the
    // old ones.
    if (paused_) {
        if (paused_time_ != new_seek_time || chunk_demuxer_) {
            paused_time_ = new_seek_time;
        } else if (old_state == ReadyStateHaveEnoughData) {
            main_task_runner_->PostTask(
                FROM_HERE,
                base::Bind(&WebMediaPlayerImpl::OnPipelineBufferingStateChanged,
                    AsWeakPtr(), BUFFERING_HAVE_ENOUGH));
            return;
        }
    }

    seeking_ = true;
    seek_time_ = new_seek_time;

    if (chunk_demuxer_)
        chunk_demuxer_->StartWaitingForSeek(seek_time_);

    // Kick off the asynchronous seek!
    pipeline_.Seek(seek_time_, BIND_TO_RENDER_LOOP1(&WebMediaPlayerImpl::OnPipelineSeeked, true));
}

void WebMediaPlayerImpl::setRate(double rate)
{
    DVLOG(1) << __FUNCTION__ << "(" << rate << ")";
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    // TODO(kylep): Remove when support for negatives is added. Also, modify the
    // following checks so rewind uses reasonable values also.
    if (rate < 0.0)
        return;

    // Limit rates to reasonable values by clamping.
    if (rate != 0.0) {
        if (rate < kMinRate)
            rate = kMinRate;
        else if (rate > kMaxRate)
            rate = kMaxRate;
        if (playback_rate_ == 0 && !paused_ && delegate_)
            NotifyPlaybackStarted();
    } else if (playback_rate_ != 0 && !paused_ && delegate_) {
        NotifyPlaybackPaused();
    }

    playback_rate_ = rate;
    if (!paused_) {
        pipeline_.SetPlaybackRate(rate);
        if (data_source_)
            data_source_->MediaPlaybackRateChanged(rate);
    }
}

void WebMediaPlayerImpl::setVolume(double volume)
{
    DVLOG(1) << __FUNCTION__ << "(" << volume << ")";
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    pipeline_.SetVolume(volume);
}

void WebMediaPlayerImpl::setSinkId(
    const blink::WebString& sink_id,
    //     const blink::WebSecurityOrigin& security_origin,
    blink::WebSetSinkIdCallbacks* web_callback)
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());
    DVLOG(1) << __FUNCTION__;

    std::string security_origin;
    media::SwitchOutputDeviceCB callback = media::ConvertToSwitchOutputDeviceCB(web_callback);
    media_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&SetSinkIdOnMediaThread, audio_source_provider_,
            sink_id.utf8(), /*static_cast<url::Origin>*/ (security_origin),
            callback));
}

#define STATIC_ASSERT_MATCHING_ENUM(webkit_name, chromium_name)                                                         \
    static_assert(static_cast<int>(WebMediaPlayer::webkit_name) == static_cast<int>(BufferedDataSource::chromium_name), \
        "mismatching enum values: " #webkit_name)
STATIC_ASSERT_MATCHING_ENUM(PreloadNone, NONE);
STATIC_ASSERT_MATCHING_ENUM(PreloadMetaData, METADATA);
STATIC_ASSERT_MATCHING_ENUM(PreloadAuto, AUTO);
#undef STATIC_ASSERT_MATCHING_ENUM

void WebMediaPlayerImpl::setPreload(WebMediaPlayer::Preload preload)
{
    DVLOG(1) << __FUNCTION__ << "(" << preload << ")";
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    preload_ = static_cast<BufferedDataSource::Preload>(preload);
    if (data_source_)
        data_source_->SetPreload(preload_);
}

bool WebMediaPlayerImpl::hasVideo() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    return pipeline_metadata_.has_video;
}

bool WebMediaPlayerImpl::hasAudio() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    return pipeline_metadata_.has_audio;
}

blink::WebSize WebMediaPlayerImpl::naturalSize() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    return blink::WebSize(pipeline_metadata_.natural_size);
}

bool WebMediaPlayerImpl::paused() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    return pipeline_.GetPlaybackRate() == 0.0f;
}

bool WebMediaPlayerImpl::seeking() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    if (ready_state_ == WebMediaPlayer::ReadyStateHaveNothing)
        return false;

    return seeking_;
}

double WebMediaPlayerImpl::duration() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    if (ready_state_ == WebMediaPlayer::ReadyStateHaveNothing)
        return std::numeric_limits<double>::quiet_NaN();

    return GetPipelineDuration();
}

double WebMediaPlayerImpl::timelineOffset() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    if (pipeline_metadata_.timeline_offset.is_null())
        return std::numeric_limits<double>::quiet_NaN();

    return pipeline_metadata_.timeline_offset.ToJsTime();
}

double WebMediaPlayerImpl::currentTime() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());
    DCHECK_NE(ready_state_, WebMediaPlayer::ReadyStateHaveNothing);

    // TODO(scherkus): Replace with an explicit ended signal to HTMLMediaElement,
    // see http://crbug.com/409280
    if (ended_)
        return duration();

    // We know the current seek time better than pipeline: pipeline may processing
    // an earlier seek before a pending seek has been started, or it might not yet
    // have the current seek time returnable via GetMediaTime().
    if (seeking()) {
        return pending_seek_ ? pending_seek_time_.InSecondsF()
                             : seek_time_.InSecondsF();
    }

    return (paused_ ? paused_time_ : pipeline_.GetMediaTime()).InSecondsF();
}

WebMediaPlayer::NetworkState WebMediaPlayerImpl::networkState() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());
    return network_state_;
}

WebMediaPlayer::ReadyState WebMediaPlayerImpl::readyState() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());
    return ready_state_;
}

blink::WebTimeRanges WebMediaPlayerImpl::buffered() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    Ranges<base::TimeDelta> buffered_time_ranges = pipeline_.GetBufferedTimeRanges();

    const base::TimeDelta duration = pipeline_.GetMediaDuration();
    if (duration != kInfiniteDuration()) {
        buffered_data_source_host_.AddBufferedTimeRanges(
            &buffered_time_ranges, duration);
    }
    return ConvertToWebTimeRanges(buffered_time_ranges);
}

blink::WebTimeRanges WebMediaPlayerImpl::seekable() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    if (ready_state_ < WebMediaPlayer::ReadyStateHaveMetadata)
        return blink::WebTimeRanges();

    const double seekable_end = duration();

    // Allow a special exception for seeks to zero for streaming sources with a
    // finite duration; this allows looping to work.
    const bool allow_seek_to_zero = data_source_ && data_source_->IsStreaming() && std::isfinite(seekable_end);

    // TODO(dalecurtis): Technically this allows seeking on media which return an
    // infinite duration so long as DataSource::IsStreaming() is false.  While not
    // expected, disabling this breaks semi-live players, http://crbug.com/427412.
    const blink::WebTimeRange seekable_range(
        0.0, allow_seek_to_zero ? 0.0 : seekable_end);
    return blink::WebTimeRanges(&seekable_range, 1);
}

bool WebMediaPlayerImpl::didLoadingProgress()
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());
    bool pipeline_progress = pipeline_.DidLoadingProgress();
    bool data_progress = buffered_data_source_host_.DidLoadingProgress();
    return pipeline_progress || data_progress;
}

void WebMediaPlayerImpl::paint(blink::WebCanvas* canvas,
    const blink::WebRect& rect,
    unsigned char alpha,
    SkXfermode::Mode mode)
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());
    TRACE_EVENT0("media", "WebMediaPlayerImpl:paint");

    // TODO(scherkus): Clarify paint() API contract to better understand when and
    // why it's being called. For example, today paint() is called when:
    //   - We haven't reached HAVE_CURRENT_DATA and need to paint black
    //   - We're painting to a canvas
    // See http://crbug.com/341225 http://crbug.com/342621 for details.
    scoped_refptr<VideoFrame> video_frame = GetCurrentFrameFromCompositor();

    gfx::Rect gfx_rect(rect);
    Context3D context_3d;
    if (video_frame.get() && video_frame->HasTextures()) {
        if (!context_3d_cb_.is_null())
            context_3d = context_3d_cb_.Run();
        // GPU Process crashed.
        if (!context_3d.gl)
            return;
    }
    skcanvas_video_renderer_.Paint(video_frame, canvas, gfx::RectF(gfx_rect),
        alpha, mode, pipeline_metadata_.video_rotation,
        context_3d);
}

bool WebMediaPlayerImpl::hasSingleSecurityOrigin() const
{
    if (data_source_)
        return data_source_->HasSingleOrigin();
    return true;
}

bool WebMediaPlayerImpl::didPassCORSAccessCheck() const
{
    if (data_source_)
        return data_source_->DidPassCORSAccessCheck();
    return false;
}

double WebMediaPlayerImpl::mediaTimeForTimeValue(double timeValue) const
{
    return base::TimeDelta::FromSecondsD(timeValue).InSecondsF();
}

unsigned WebMediaPlayerImpl::decodedFrameCount() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    PipelineStatistics stats = pipeline_.GetStatistics();
    return stats.video_frames_decoded;
}

unsigned WebMediaPlayerImpl::droppedFrameCount() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    PipelineStatistics stats = pipeline_.GetStatistics();
    return stats.video_frames_dropped;
}

unsigned WebMediaPlayerImpl::audioDecodedByteCount() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    PipelineStatistics stats = pipeline_.GetStatistics();
    return stats.audio_bytes_decoded;
}

unsigned WebMediaPlayerImpl::videoDecodedByteCount() const
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    PipelineStatistics stats = pipeline_.GetStatistics();
    return stats.video_bytes_decoded;
}

bool WebMediaPlayerImpl::copyVideoTextureToPlatformTexture(
    blink::WebGraphicsContext3D* web_graphics_context,
    unsigned int texture,
    unsigned int internal_format,
    unsigned int type,
    bool premultiply_alpha,
    bool flip_y)
{
    TRACE_EVENT0("media", "WebMediaPlayerImpl:copyVideoTextureToPlatformTexture");

    scoped_refptr<VideoFrame> video_frame = GetCurrentFrameFromCompositor();

    if (!video_frame.get() || !video_frame->HasTextures() || media::VideoFrame::NumPlanes(video_frame->format()) != 1) {
        return false;
    }

    // TODO(dshwang): need more elegant way to convert WebGraphicsContext3D to
    // GLES2Interface.
    gpu::gles2::GLES2Interface* gl = static_cast<gpu_blink::WebGraphicsContext3DImpl*>(web_graphics_context)
                                         ->GetGLInterface();
    SkCanvasVideoRenderer::CopyVideoFrameSingleTextureToGLTexture(
        gl, video_frame.get(), texture, internal_format, type, premultiply_alpha,
        flip_y);
    return true;
}

WebMediaPlayer::MediaKeyException
WebMediaPlayerImpl::generateKeyRequest(const WebString& key_system,
    const unsigned char* init_data,
    unsigned init_data_length)
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    return encrypted_media_support_.GenerateKeyRequest(
        frame_, key_system, init_data, init_data_length);
}

WebMediaPlayer::MediaKeyException WebMediaPlayerImpl::addKey(
    const WebString& key_system,
    const unsigned char* key,
    unsigned key_length,
    const unsigned char* init_data,
    unsigned init_data_length,
    const WebString& session_id)
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    return encrypted_media_support_.AddKey(
        key_system, key, key_length, init_data, init_data_length, session_id);
}

WebMediaPlayer::MediaKeyException WebMediaPlayerImpl::cancelKeyRequest(
    const WebString& key_system,
    const WebString& session_id)
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    return encrypted_media_support_.CancelKeyRequest(key_system, session_id);
}

void WebMediaPlayerImpl::setContentDecryptionModule(
    blink::WebContentDecryptionModule* cdm,
    blink::WebContentDecryptionModuleResult result)
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    // Once the CDM is set it can't be cleared as there may be frames being
    // decrypted on other threads. So fail this request.
    // http://crbug.com/462365#c7.
    if (!cdm) {
        result.completeWithError(
            blink::WebContentDecryptionModuleExceptionInvalidStateError, 0,
            "The existing MediaKeys object cannot be removed at this time.");
        return;
    }

    // Although unlikely, it is possible that multiple calls happen
    // simultaneously, so fail this call if there is already one pending.
    if (set_cdm_result_) {
        result.completeWithError(
            blink::WebContentDecryptionModuleExceptionInvalidStateError, 0,
            "Unable to set MediaKeys object at this time.");
        return;
    }

    // Create a local copy of |result| to avoid problems with the callback
    // getting passed to the media thread and causing |result| to be destructed
    // on the wrong thread in some failure conditions.
    set_cdm_result_.reset(new blink::WebContentDecryptionModuleResult(result));

    //     SetCdm(BIND_TO_RENDER_LOOP(&WebMediaPlayerImpl::OnCdmAttached),
    //         ToWebContentDecryptionModuleImpl(cdm)->GetCdmContext());
    DebugBreak();
}

void WebMediaPlayerImpl::OnEncryptedMediaInitData(
    EmeInitDataType init_data_type,
    const std::vector<uint8>& init_data)
{
    DCHECK(init_data_type != EmeInitDataType::UNKNOWN);

    // Do not fire "encrypted" event if encrypted media is not enabled.
    // TODO(xhwang): Handle this in |client_|.
    if (!blink::WebRuntimeFeatures::isPrefixedEncryptedMediaEnabled() && !blink::WebRuntimeFeatures::isEncryptedMediaEnabled()) {
        return;
    }

    // TODO(xhwang): Update this UMA name.
    UMA_HISTOGRAM_COUNTS("Media.EME.NeedKey", 1);

    encrypted_media_support_.SetInitDataType(init_data_type);

    encrypted_client_->encrypted(
        ConvertToWebInitDataType(init_data_type), vector_as_array(&init_data),
        base::saturated_cast<unsigned int>(init_data.size()));
}

void WebMediaPlayerImpl::OnWaitingForDecryptionKey()
{
    encrypted_client_->didBlockPlaybackWaitingForKey();

    // TODO(jrummell): didResumePlaybackBlockedForKey() should only be called
    // when a key has been successfully added (e.g. OnSessionKeysChange() with
    // |has_additional_usable_key| = true). http://crbug.com/461903
    encrypted_client_->didResumePlaybackBlockedForKey();
}

void WebMediaPlayerImpl::SetCdm(const CdmAttachedCB& cdm_attached_cb,
    CdmContext* cdm_context)
{
    // If CDM initialization succeeded, tell the pipeline about it.
    if (cdm_context)
        pipeline_.SetCdm(cdm_context, cdm_attached_cb);
}

void WebMediaPlayerImpl::OnCdmAttached(bool success)
{
    if (success) {
        set_cdm_result_->complete();
        set_cdm_result_.reset();
        return;
    }

    set_cdm_result_->completeWithError(
        blink::WebContentDecryptionModuleExceptionNotSupportedError, 0,
        "Unable to set MediaKeys object");
    set_cdm_result_.reset();
}

void WebMediaPlayerImpl::OnPipelineSeeked(bool time_changed,
    PipelineStatus status)
{
    DVLOG(1) << __FUNCTION__ << "(" << time_changed << ", " << status << ")";
    DCHECK(main_task_runner_->BelongsToCurrentThread());
    seeking_ = false;
    seek_time_ = base::TimeDelta();
    if (pending_seek_) {
        double pending_seek_seconds = pending_seek_time_.InSecondsF();
        pending_seek_ = false;
        pending_seek_time_ = base::TimeDelta();
        seek(pending_seek_seconds);
        return;
    }

    if (status != PIPELINE_OK) {
        OnPipelineError(status);
        return;
    }

    // Update our paused time.
    if (paused_)
        UpdatePausedTime();

    should_notify_time_changed_ = time_changed;
}

void WebMediaPlayerImpl::OnPipelineEnded()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    // Ignore state changes until we've completed all outstanding seeks.
    if (seeking_ || pending_seek_)
        return;

    ended_ = true;
    client_->timeChanged();
}

void WebMediaPlayerImpl::OnPipelineError(PipelineStatus error)
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());
    DCHECK_NE(error, PIPELINE_OK);

    if (ready_state_ == WebMediaPlayer::ReadyStateHaveNothing) {
        // Any error that occurs before reaching ReadyStateHaveMetadata should
        // be considered a format error.
        SetNetworkState(WebMediaPlayer::NetworkStateFormatError);
        return;
    }

    SetNetworkState(PipelineErrorToNetworkState(error));
}

void WebMediaPlayerImpl::OnPipelineMetadata(
    PipelineMetadata metadata)
{
    DVLOG(1) << __FUNCTION__;

    pipeline_metadata_ = metadata;

    UMA_HISTOGRAM_ENUMERATION("Media.VideoRotation", metadata.video_rotation,
        VIDEO_ROTATION_MAX + 1);
    SetReadyState(WebMediaPlayer::ReadyStateHaveMetadata);

    if (hasVideo()) {
        DCHECK(!video_weblayer_);
        scoped_refptr<cc::VideoLayer> layer = cc::VideoLayer::Create(cc_blink::WebLayerImpl::LayerSettings(),
            compositor_, pipeline_metadata_.video_rotation);

        if (pipeline_metadata_.video_rotation == VIDEO_ROTATION_90 || pipeline_metadata_.video_rotation == VIDEO_ROTATION_270) {
            gfx::Size size = pipeline_metadata_.natural_size;
            pipeline_metadata_.natural_size = gfx::Size(size.height(), size.width());
        }

        video_weblayer_.reset(new cc_blink::WebLayerImpl(layer));
        video_weblayer_->layer()->SetContentsOpaque(opaque_);
        video_weblayer_->SetContentsOpaqueIsFixed(true);
        client_->setWebLayer(video_weblayer_.get());
    }
}

void WebMediaPlayerImpl::OnPipelineBufferingStateChanged(
    BufferingState buffering_state)
{
    DVLOG(1) << __FUNCTION__ << "(" << buffering_state << ")";

    // Ignore buffering state changes until we've completed all outstanding seeks.
    if (seeking_ || pending_seek_)
        return;

    // TODO(scherkus): Handle other buffering states when Pipeline starts using
    // them and translate them ready state changes http://crbug.com/144683
    DCHECK_EQ(buffering_state, BUFFERING_HAVE_ENOUGH);
    SetReadyState(WebMediaPlayer::ReadyStateHaveEnoughData);

    // Let the DataSource know we have enough data. It may use this information to
    // release unused network connections.
    if (data_source_)
        data_source_->OnBufferingHaveEnough();

    // Blink expects a timeChanged() in response to a seek().
    if (should_notify_time_changed_)
        client_->timeChanged();

    // Once we have enough, start reporting the total memory usage. We'll also
    // report once playback starts.
    ReportMemoryUsage();
}

void WebMediaPlayerImpl::OnDemuxerOpened()
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());
    client_->mediaSourceOpened(
        new WebMediaSourceImpl(chunk_demuxer_, media_log_));
}

void WebMediaPlayerImpl::OnAddTextTrack(
    const TextTrackConfig& config,
    const AddTextTrackDoneCB& done_cb)
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    const WebInbandTextTrackImpl::Kind web_kind = static_cast<WebInbandTextTrackImpl::Kind>(config.kind());
    const blink::WebString web_label = blink::WebString::fromUTF8(config.label());
    const blink::WebString web_language = blink::WebString::fromUTF8(config.language());
    const blink::WebString web_id = blink::WebString::fromUTF8(config.id());

    scoped_ptr<WebInbandTextTrackImpl> web_inband_text_track(
        new WebInbandTextTrackImpl(web_kind, web_label, web_language, web_id));

    scoped_ptr<TextTrack> text_track(new TextTrackImpl(
        main_task_runner_, client_, web_inband_text_track.Pass()));

    done_cb.Run(text_track.Pass());
}

void WebMediaPlayerImpl::DataSourceInitialized(bool success)
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    if (!success) {
        SetNetworkState(WebMediaPlayer::NetworkStateFormatError);
        return;
    }

    StartPipeline();
}

void WebMediaPlayerImpl::NotifyDownloading(bool is_downloading)
{
    if (!is_downloading && network_state_ == WebMediaPlayer::NetworkStateLoading)
        SetNetworkState(WebMediaPlayer::NetworkStateIdle);
    else if (is_downloading && network_state_ == WebMediaPlayer::NetworkStateIdle)
        SetNetworkState(WebMediaPlayer::NetworkStateLoading);
    media_log_->AddEvent(
        media_log_->CreateBooleanEvent(
            MediaLogEvent::NETWORK_ACTIVITY_SET,
            "is_downloading_data", is_downloading));
}

void WebMediaPlayerImpl::StartPipeline()
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    Demuxer::EncryptedMediaInitDataCB encrypted_media_init_data_cb = BIND_TO_RENDER_LOOP(&WebMediaPlayerImpl::OnEncryptedMediaInitData);

    // Figure out which demuxer to use.
    if (load_type_ != LoadTypeMediaSource) {
        DCHECK(!chunk_demuxer_);
        DCHECK(data_source_);

        demuxer_.reset(new FFmpegDemuxer(media_task_runner_, data_source_.get(),
            encrypted_media_init_data_cb, media_log_));
    } else {
        DCHECK(!chunk_demuxer_);
        DCHECK(!data_source_);

        chunk_demuxer_ = new ChunkDemuxer(
            BIND_TO_RENDER_LOOP(&WebMediaPlayerImpl::OnDemuxerOpened),
            encrypted_media_init_data_cb, media_log_, true);
        demuxer_.reset(chunk_demuxer_);
    }

    // ... and we're ready to go!
    seeking_ = true;

    pipeline_.Start(
        demuxer_.get(), renderer_factory_->CreateRenderer(media_task_runner_, worker_task_runner_, audio_source_provider_.get(), compositor_),
        BIND_TO_RENDER_LOOP(&WebMediaPlayerImpl::OnPipelineEnded),
        BIND_TO_RENDER_LOOP(&WebMediaPlayerImpl::OnPipelineError),
        BIND_TO_RENDER_LOOP1(&WebMediaPlayerImpl::OnPipelineSeeked, false),
        BIND_TO_RENDER_LOOP(&WebMediaPlayerImpl::OnPipelineMetadata),
        BIND_TO_RENDER_LOOP(&WebMediaPlayerImpl::OnPipelineBufferingStateChanged),
        BIND_TO_RENDER_LOOP(&WebMediaPlayerImpl::OnDurationChanged),
        BIND_TO_RENDER_LOOP(&WebMediaPlayerImpl::OnAddTextTrack),
        BIND_TO_RENDER_LOOP(&WebMediaPlayerImpl::OnWaitingForDecryptionKey));
}

void WebMediaPlayerImpl::SetNetworkState(WebMediaPlayer::NetworkState state)
{
    DVLOG(1) << __FUNCTION__ << "(" << state << ")";
    DCHECK(main_task_runner_->BelongsToCurrentThread());
    network_state_ = state;
    // Always notify to ensure client has the latest value.
    client_->networkStateChanged();
}

void WebMediaPlayerImpl::SetReadyState(WebMediaPlayer::ReadyState state)
{
    DVLOG(1) << __FUNCTION__ << "(" << state << ")";
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    if (state == WebMediaPlayer::ReadyStateHaveEnoughData && data_source_ && data_source_->assume_fully_buffered() && network_state_ == WebMediaPlayer::NetworkStateLoading)
        SetNetworkState(WebMediaPlayer::NetworkStateLoaded);

    ready_state_ = state;
    // Always notify to ensure client has the latest value.
    client_->readyStateChanged();
}

blink::WebAudioSourceProvider* WebMediaPlayerImpl::audioSourceProvider()
{
    return audio_source_provider_.get();
}

double WebMediaPlayerImpl::GetPipelineDuration() const
{
    base::TimeDelta duration = pipeline_.GetMediaDuration();

    // Return positive infinity if the resource is unbounded.
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/video.html#dom-media-duration
    if (duration == kInfiniteDuration())
        return std::numeric_limits<double>::infinity();

    return duration.InSecondsF();
}

void WebMediaPlayerImpl::OnDurationChanged()
{
    if (ready_state_ == WebMediaPlayer::ReadyStateHaveNothing)
        return;

    client_->durationChanged();
}

void WebMediaPlayerImpl::OnNaturalSizeChanged(gfx::Size size)
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());
    DCHECK_NE(ready_state_, WebMediaPlayer::ReadyStateHaveNothing);
    TRACE_EVENT0("media", "WebMediaPlayerImpl::OnNaturalSizeChanged");

    media_log_->AddEvent(
        media_log_->CreateVideoSizeSetEvent(size.width(), size.height()));
    pipeline_metadata_.natural_size = size;

    client_->sizeChanged();
}

void WebMediaPlayerImpl::OnOpacityChanged(bool opaque)
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());
    DCHECK_NE(ready_state_, WebMediaPlayer::ReadyStateHaveNothing);

    opaque_ = opaque;
    // Modify content opaqueness of cc::Layer directly so that
    // SetContentsOpaqueIsFixed is ignored.
    if (video_weblayer_)
        video_weblayer_->layer()->SetContentsOpaque(opaque_);
}

static void GetCurrentFrameAndSignal(
    VideoFrameCompositor* compositor,
    scoped_refptr<VideoFrame>* video_frame_out,
    base::WaitableEvent* event)
{
    TRACE_EVENT0("media", "GetCurrentFrameAndSignal");
    *video_frame_out = compositor->GetCurrentFrameAndUpdateIfStale();
    event->Signal();
}

scoped_refptr<VideoFrame>
WebMediaPlayerImpl::GetCurrentFrameFromCompositor()
{
    TRACE_EVENT0("media", "WebMediaPlayerImpl::GetCurrentFrameFromCompositor");
    if (compositor_task_runner_->BelongsToCurrentThread())
        return compositor_->GetCurrentFrameAndUpdateIfStale();

    // Use a posted task and waitable event instead of a lock otherwise
    // WebGL/Canvas can see different content than what the compositor is seeing.
    scoped_refptr<VideoFrame> video_frame;
    base::WaitableEvent event(false, false);
    compositor_task_runner_->PostTask(FROM_HERE,
        base::Bind(&GetCurrentFrameAndSignal,
            base::Unretained(compositor_),
            &video_frame,
            &event));
    event.Wait();
    return video_frame;
}

void WebMediaPlayerImpl::UpdatePausedTime()
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    // pause() may be called after playback has ended and the HTMLMediaElement
    // requires that currentTime() == duration() after ending.  We want to ensure
    // |paused_time_| matches currentTime() in this case or a future seek() may
    // incorrectly discard what it thinks is a seek to the existing time.
    paused_time_ = ended_ ? pipeline_.GetMediaDuration() : pipeline_.GetMediaTime();
}

void WebMediaPlayerImpl::NotifyPlaybackStarted()
{
    if (delegate_)
        delegate_->DidPlay(this);
    if (!memory_usage_reporting_timer_.IsRunning()) {
        memory_usage_reporting_timer_.Start(FROM_HERE,
            base::TimeDelta::FromSeconds(2), this,
            &WebMediaPlayerImpl::ReportMemoryUsage);
    }
}

void WebMediaPlayerImpl::NotifyPlaybackPaused()
{
    if (delegate_)
        delegate_->DidPause(this);
    memory_usage_reporting_timer_.Stop();
    ReportMemoryUsage();
}

void WebMediaPlayerImpl::ReportMemoryUsage()
{
    DCHECK(main_task_runner_->BelongsToCurrentThread());

    const PipelineStatistics stats = pipeline_.GetStatistics();
    const int64_t current_memory_usage = stats.audio_memory_usage + stats.video_memory_usage + (data_source_ ? data_source_->GetMemoryUsage() : 0) + (demuxer_ ? demuxer_->GetMemoryUsage() : 0);

    DVLOG(2) << "Memory Usage -- Audio: " << stats.audio_memory_usage
             << ", Video: " << stats.video_memory_usage << ", DataSource: "
             << (data_source_ ? data_source_->GetMemoryUsage() : 0)
             << ", Demuxer: " << (demuxer_ ? demuxer_->GetMemoryUsage() : 0);

    const int64_t delta = current_memory_usage - last_reported_memory_usage_;
    last_reported_memory_usage_ = current_memory_usage;
    adjust_allocated_memory_cb_.Run(delta);
}

} // namespace media

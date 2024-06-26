// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/renderers/default_renderer_factory.h"

#include "base/bind.h"
#include "base/single_thread_task_runner.h"
#include "build/build_config.h"
#include "media/base/media_log.h"
#include "media/filters/gpu_video_decoder.h"
#include "media/renderers/audio_renderer_impl.h"
#include "media/renderers/gpu_video_accelerator_factories.h"
#include "media/renderers/renderer_impl.h"
#include "media/renderers/video_renderer_impl.h"

#if !defined(MEDIA_DISABLE_FFMPEG)
#include "media/filters/ffmpeg_audio_decoder.h"
#if !defined(DISABLE_FFMPEG_VIDEO_DECODERS)
#include "media/filters/ffmpeg_video_decoder.h"
#endif
#endif

#if !defined(OS_ANDROID) || defined(ENABLE_MEDIA_PIPELINE_ON_ANDROID)
#include "media/filters/opus_audio_decoder.h"
#endif

#if !defined(MEDIA_DISABLE_LIBVPX)
#include "media/filters/vpx_video_decoder.h"
#endif

namespace media {

DefaultRendererFactory::DefaultRendererFactory(
    const scoped_refptr<MediaLog>& media_log,
    GpuVideoAcceleratorFactories* gpu_factories,
    const AudioHardwareConfig& audio_hardware_config)
    : media_log_(media_log)
    , gpu_factories_(gpu_factories)
    , audio_hardware_config_(audio_hardware_config)
{
}

DefaultRendererFactory::~DefaultRendererFactory()
{
}

scoped_ptr<Renderer> DefaultRendererFactory::CreateRenderer(
    const scoped_refptr<base::SingleThreadTaskRunner>& media_task_runner,
    const scoped_refptr<base::TaskRunner>& worker_task_runner,
    AudioRendererSink* audio_renderer_sink,
    VideoRendererSink* video_renderer_sink)
{
    DCHECK(audio_renderer_sink);

    // Create our audio decoders and renderer.
    ScopedVector<AudioDecoder> audio_decoders;

#if !defined(MEDIA_DISABLE_FFMPEG)
    audio_decoders.push_back(new FFmpegAudioDecoder(media_task_runner, media_log_));
#endif

#if !defined(OS_ANDROID) || defined(ENABLE_MEDIA_PIPELINE_ON_ANDROID)
    //audio_decoders.push_back(new OpusAudioDecoder(media_task_runner)); // weolar
#endif

    scoped_ptr<AudioRenderer> audio_renderer(new AudioRendererImpl(
        media_task_runner, audio_renderer_sink, audio_decoders.Pass(),
        audio_hardware_config_, media_log_));

    // Create our video decoders and renderer.
    ScopedVector<VideoDecoder> video_decoders;

    // |gpu_factories_| requires that its entry points be called on its
    // |GetTaskRunner()|.  Since |pipeline_| will own decoders created from the
    // factories, require that their message loops are identical.
    DCHECK(!gpu_factories_ || (gpu_factories_->GetTaskRunner() == media_task_runner.get()));

    if (gpu_factories_)
        video_decoders.push_back(new GpuVideoDecoder(gpu_factories_));

#if !defined(MEDIA_DISABLE_LIBVPX)
        //video_decoders.push_back(new VpxVideoDecoder(media_task_runner)); // weolar
#endif

#if !defined(MEDIA_DISABLE_FFMPEG) && !defined(DISABLE_FFMPEG_VIDEO_DECODERS)
    video_decoders.push_back(new FFmpegVideoDecoder(media_task_runner));
#endif

    scoped_ptr<VideoRenderer> video_renderer(new VideoRendererImpl(
        media_task_runner, worker_task_runner, video_renderer_sink,
        video_decoders.Pass(), true, gpu_factories_, media_log_));

    // Create renderer.
    return scoped_ptr<Renderer>(new RendererImpl(
        media_task_runner, audio_renderer.Pass(), video_renderer.Pass()));
}

} // namespace media

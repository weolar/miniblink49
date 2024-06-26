// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_RENDERERS_DEFAULT_RENDERER_FACTORY_H_
#define MEDIA_RENDERERS_DEFAULT_RENDERER_FACTORY_H_

#include "base/callback.h"
#include "media/base/media_export.h"
#include "media/base/renderer_factory.h"

namespace media {

class AudioHardwareConfig;
class AudioRendererSink;
class GpuVideoAcceleratorFactories;
class MediaLog;
class VideoRendererSink;

// The default factory class for creating RendererImpl.
class MEDIA_EXPORT DefaultRendererFactory : public RendererFactory {
public:
    DefaultRendererFactory(const scoped_refptr<MediaLog>& media_log,
        GpuVideoAcceleratorFactories* gpu_factories,
        const AudioHardwareConfig& audio_hardware_config);
    ~DefaultRendererFactory() final;

    scoped_ptr<Renderer> CreateRenderer(
        const scoped_refptr<base::SingleThreadTaskRunner>& media_task_runner,
        const scoped_refptr<base::TaskRunner>& worker_task_runner,
        AudioRendererSink* audio_renderer_sink,
        VideoRendererSink* video_renderer_sink) final;

private:
    scoped_refptr<MediaLog> media_log_;

    // Factories for supporting video accelerators. May be null.
    GpuVideoAcceleratorFactories* gpu_factories_;

    const AudioHardwareConfig& audio_hardware_config_;

    DISALLOW_COPY_AND_ASSIGN(DefaultRendererFactory);
};

} // namespace media

#endif // MEDIA_RENDERERS_DEFAULT_RENDERER_FACTORY_H_

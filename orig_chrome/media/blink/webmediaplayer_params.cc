// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/blink/webmediaplayer_params.h"

#include "base/single_thread_task_runner.h"
#include "base/task_runner.h"
#include "media/base/audio_renderer_sink.h"
#include "media/base/media_log.h"

namespace media {

WebMediaPlayerParams::WebMediaPlayerParams(
    const DeferLoadCB& defer_load_cb,
    const scoped_refptr<RestartableAudioRendererSink>& audio_renderer_sink,
    const scoped_refptr<MediaLog>& media_log,
    const scoped_refptr<base::SingleThreadTaskRunner>& media_task_runner,
    const scoped_refptr<base::TaskRunner>& worker_task_runner,
    const scoped_refptr<base::SingleThreadTaskRunner>& compositor_task_runner,
    const Context3DCB& context_3d_cb,
    const AdjustAllocatedMemoryCB& adjust_allocated_memory_cb,
    MediaPermission* media_permission,
    blink::WebContentDecryptionModule* initial_cdm)
    : defer_load_cb_(defer_load_cb)
    , audio_renderer_sink_(audio_renderer_sink)
    , media_log_(media_log)
    , media_task_runner_(media_task_runner)
    , worker_task_runner_(worker_task_runner)
    , compositor_task_runner_(compositor_task_runner)
    , context_3d_cb_(context_3d_cb)
    , adjust_allocated_memory_cb_(adjust_allocated_memory_cb)
    , media_permission_(media_permission)
    , initial_cdm_(initial_cdm)
{
}

WebMediaPlayerParams::~WebMediaPlayerParams() { }

} // namespace media

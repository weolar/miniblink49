// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_WEBMEDIAPLAYER_PARAMS_H_
#define MEDIA_BLINK_WEBMEDIAPLAYER_PARAMS_H_

#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "media/blink/media_blink_export.h"
#include "media/filters/context_3d.h"

namespace base {
class SingleThreadTaskRunner;
class TaskRunner;
}

namespace blink {
class WebContentDecryptionModule;
class WebMediaPlayerClient;
}

namespace media {

class RestartableAudioRendererSink;
class MediaLog;
class MediaPermission;

// Holds parameters for constructing WebMediaPlayerImpl without having
// to plumb arguments through various abstraction layers.
class MEDIA_BLINK_EXPORT WebMediaPlayerParams {
public:
    typedef base::Callback<void(const base::Closure&)> DeferLoadCB;
    typedef base::Callback<Context3D()> Context3DCB;

    // Callback to tell V8 about the amount of memory used by the WebMediaPlayer
    // instance.  The input parameter is the delta in bytes since the last call to
    // AdjustAllocatedMemoryCB and the return value is the total number of bytes
    // used by objects external to V8.  Note: this value includes things that are
    // not the WebMediaPlayer!
    typedef base::Callback<int64_t(int64_t)> AdjustAllocatedMemoryCB;

    // |defer_load_cb|, |audio_renderer_sink|, |compositor_task_runner|, and
    // |context_3d_cb| may be null.
    WebMediaPlayerParams(
        const DeferLoadCB& defer_load_cb,
        const scoped_refptr<RestartableAudioRendererSink>& audio_renderer_sink,
        const scoped_refptr<MediaLog>& media_log,
        const scoped_refptr<base::SingleThreadTaskRunner>& media_task_runner,
        const scoped_refptr<base::TaskRunner>& worker_task_runner,
        const scoped_refptr<base::SingleThreadTaskRunner>& compositor_task_runner,
        const Context3DCB& context_3d,
        const AdjustAllocatedMemoryCB& adjust_allocated_memory_cb,
        MediaPermission* media_permission,
        blink::WebContentDecryptionModule* initial_cdm);

    ~WebMediaPlayerParams();

    DeferLoadCB defer_load_cb() const { return defer_load_cb_; }

    const scoped_refptr<RestartableAudioRendererSink>& audio_renderer_sink()
        const
    {
        return audio_renderer_sink_;
    }

    const scoped_refptr<MediaLog>& media_log() const
    {
        return media_log_;
    }

    const scoped_refptr<base::SingleThreadTaskRunner>& media_task_runner() const
    {
        return media_task_runner_;
    }

    const scoped_refptr<base::TaskRunner> worker_task_runner() const
    {
        return worker_task_runner_;
    }

    const scoped_refptr<base::SingleThreadTaskRunner>& compositor_task_runner()
        const
    {
        return compositor_task_runner_;
    }

    Context3DCB context_3d_cb() const { return context_3d_cb_; }

    MediaPermission* media_permission() const { return media_permission_; }

    blink::WebContentDecryptionModule* initial_cdm() const
    {
        return initial_cdm_;
    }

    AdjustAllocatedMemoryCB adjust_allocated_memory_cb() const
    {
        return adjust_allocated_memory_cb_;
    }

private:
    DeferLoadCB defer_load_cb_;
    scoped_refptr<RestartableAudioRendererSink> audio_renderer_sink_;
    scoped_refptr<MediaLog> media_log_;
    scoped_refptr<base::SingleThreadTaskRunner> media_task_runner_;
    scoped_refptr<base::TaskRunner> worker_task_runner_;
    scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner_;
    Context3DCB context_3d_cb_;
    AdjustAllocatedMemoryCB adjust_allocated_memory_cb_;

    // TODO(xhwang): Remove after prefixed EME API support is removed.
    MediaPermission* media_permission_;
    blink::WebContentDecryptionModule* initial_cdm_;

    DISALLOW_IMPLICIT_CONSTRUCTORS(WebMediaPlayerParams);
};

} // namespace media

#endif // MEDIA_BLINK_WEBMEDIAPLAYER_PARAMS_H_

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/media.h"

#include "base/files/file_path.h"
#include "base/lazy_instance.h"
#include "base/path_service.h"
#include "base/synchronization/lock.h"
#include "base/trace_event/trace_event.h"
#include "build/build_config.h"
#include "media/base/yuv_convert.h"

#if !defined(MEDIA_DISABLE_FFMPEG)
#include "media/ffmpeg/ffmpeg_common.h"
#endif

namespace media {

// Media must only be initialized once, so use a LazyInstance to ensure this.
class MediaInitializer {
private:
    friend struct base::DefaultLazyInstanceTraits<MediaInitializer>;

    MediaInitializer()
    {
        TRACE_EVENT_WARMUP_CATEGORY("audio");
        TRACE_EVENT_WARMUP_CATEGORY("media");

        // Perform initialization of libraries which require runtime CPU detection.
        InitializeCPUSpecificYUVConversions();

#if !defined(MEDIA_DISABLE_FFMPEG)
        // Initialize CPU flags outside of the sandbox as this may query /proc for
        // details on the current CPU for NEON, VFP, etc optimizations.
        av_get_cpu_flags();

        // Disable logging as it interferes with layout tests.
        av_log_set_level(AV_LOG_QUIET);

#if defined(ALLOCATOR_SHIM)
        // Remove allocation limit from ffmpeg, so calls go down to shim layer.
        av_max_alloc(0);
#endif // defined(ALLOCATOR_SHIM)

#endif // !defined(MEDIA_DISABLE_FFMPEG)
    }

    ~MediaInitializer()
    {
        NOTREACHED() << "MediaInitializer should be leaky!";
    }

    DISALLOW_COPY_AND_ASSIGN(MediaInitializer);
};

static base::LazyInstance<MediaInitializer>::Leaky g_media_library = LAZY_INSTANCE_INITIALIZER;

void InitializeMediaLibrary()
{
    g_media_library.Get();
}

} // namespace media

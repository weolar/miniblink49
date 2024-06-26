// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/renderer_settings.h"

#include <limits>
#include <windows.h>

#include "base/logging.h"
#include "base/time/time.h"

namespace cc {

RendererSettings::RendererSettings()
    : allow_antialiasing(true)
    , force_antialiasing(false)
    , force_blending_with_shaders(false)
    , partial_swap_enabled(false)
    , finish_rendering_on_resize(false)
    , should_clear_root_render_pass(true)
    , disable_display_vsync(false)
    , delay_releasing_overlay_resources(false)
    , refresh_rate(60.0)
    , highp_threshold_min(0)
    , use_rgba_4444_textures(false)
    , texture_id_allocation_chunk_size(64)
{
    // gl_wrapper.dll!gfx::WinVSyncProvider::GetVSyncParameters(const base::Callback<void __cdecl(base::TimeTicks, base::TimeDelta)> &callback)
    // content.dll!content::PassThroughImageTransportSurface::SendVSyncUpdateIfAvailable()
    // content.dll!content::PassThroughImageTransportSurface::SwapBuffers()
    // gpu.dll!gpu::gles2::GLES2DecoderImpl::DoSwapBuffers()
    static bool s_is_init = false;
    
    if (!s_is_init) {
        // When DWM compositing is active all displays are normalized to the
        // refresh rate of the primary display, and won't composite any faster.
        // If DWM compositing is disabled, though, we can use the refresh rates
        // reported by each display, which will help systems that have mis-matched
        // displays that run at different frequencies.
        HMONITOR monitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX monitor_info;
        monitor_info.cbSize = sizeof(MONITORINFOEX);
        BOOL result = GetMonitorInfo(monitor, &monitor_info);
        if (result) {
            DEVMODE display_info;
            display_info.dmSize = sizeof(DEVMODE);
            display_info.dmDriverExtra = 0;
            result = EnumDisplaySettings(monitor_info.szDevice, ENUM_CURRENT_SETTINGS, &display_info);
            if (result && display_info.dmDisplayFrequency > 1) {
                //base::TimeDelta interval;
                //interval = base::TimeDelta::FromMicroseconds((1.0 / static_cast<double>(display_info.dmDisplayFrequency)) * base::Time::kMicrosecondsPerSecond);
                refresh_rate = display_info.dmDisplayFrequency;
            }
        }
        s_is_init = true;
    }
}

RendererSettings::~RendererSettings()
{
}

} // namespace cc

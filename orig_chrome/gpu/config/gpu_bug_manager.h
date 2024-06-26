// Copyright (c) 2015 The Tencent Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// 所有自定义的针对特殊GPU的处理逻辑统一放在这个文件里面

#ifndef GPU_CONFIG_GPU_BUG_MANAGER_H_
#define GPU_CONFIG_GPU_BUG_MANAGER_H_

#include "base/basictypes.h"
#include "base/memory/singleton.h"
#include "gpu/gpu_export.h"

namespace gpu {

class GPU_EXPORT GpuBugManager {
public:
    static GpuBugManager* GetInstance();
    bool disable_egl_khr_client_wait_sync() { return disable_egl_khr_client_wait_sync_; }
    bool disable_egl_image_texture_access() { return disable_egl_image_texture_access_; }
    bool use_teximage2d_free_memory() { return use_teximage2d_free_memory_; }
    bool force_gl_finish_after_bind_frame_buffer() { return force_gl_finish_after_bind_frame_buffer_; }

private:
    friend struct DefaultSingletonTraits<GpuBugManager>;

    GpuBugManager();
    void Initialize();

    bool disable_egl_khr_client_wait_sync_;
    bool disable_egl_image_texture_access_;
    bool use_teximage2d_free_memory_;
    bool force_gl_finish_after_bind_frame_buffer_;

    DISALLOW_COPY_AND_ASSIGN(GpuBugManager);
};
} // namespace gpu

#endif

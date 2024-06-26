// Copyright (c) 2015 The Tencent Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/config/gpu_bug_manager.h"
#include "base/android/sys_utils.h"
#include "content/browser/gpu/gpu_data_manager_impl.h"
#include "gpu/config/gpu_info.h"
namespace gpu {

// static
GpuBugManager* GpuBugManager::GetInstance()
{
    return Singleton<GpuBugManager>::get();
}

GpuBugManager::GpuBugManager()
    : disable_egl_khr_client_wait_sync_(false)
    , disable_egl_image_texture_access_(false)
    , use_teximage2d_free_memory_(false)
    , force_gl_finish_after_bind_frame_buffer_(false)
{
    Initialize();
}

void GpuBugManager::Initialize()
{
    std::string gl_vendor = content::GpuDataManagerImpl::GetInstance()->GetGPUInfo().gl_vendor;
    std::string gl_renderer = content::GpuDataManagerImpl::GetInstance()->GetGPUInfo().gl_renderer;
    std::string gl_version = content::GpuDataManagerImpl::GetInstance()->GetGPUInfo().gl_version;

    // TODO(Perryuwang): ARM Mali的GPU在os4.2以下版本中存在bug，调用eglClientWaitSyncKHR卡死.
    //   所以，针对ARM Mali的GPU做特殊处理，不去调用eglClientWaitSyncKHR.
    if (!base::android::SysUtils::RunningOnJellyBeanMR1OrHigher() && std::string::npos != gl_vendor.find("ARM")) {
        disable_egl_khr_client_wait_sync_ = true;
    }

    // 为了规避vivo X3,U9200上玩H5游戏修仙三国时，花屏卡死的问题。 BUG：50389770,50643236
    // johnstonli Mali-T624和Mali-T628 KHRImage指令存在缺陷，当前参考主线QB使用ShareGroup和Sync指令实现纹理拷贝 fixbug 50840423
    // Vivante GC2000的GPU调用eglCreateImageKHR创建EGLImage失败，导致canvas显示黑屏，走ShareGroup方式。BUG: 50921921
    if ((!base::android::SysUtils::RunningOnLollipopOrHigher() && std::string::npos != gl_renderer.find("PowerVR SGX 544MP")) || (!base::android::SysUtils::RunningOnJellyBeanOrHigher() && (std::string::npos != gl_renderer.find("PowerVR SGX 540"))) || (std::string::npos != gl_renderer.find("Mali-T624")) || (std::string::npos != gl_renderer.find("Mali-T628")) || (std::string::npos != gl_renderer.find("Vivante GC2000"))) {
        disable_egl_image_texture_access_ = true;
    }

    // 华为U9200上调用glDeleteTextures释放纹理时，GPU并没有真正将纹理内存释放，存在内存泄漏。BUG:50657936
    // 为了规避这个问题，在glDeleteTextures前调用glTexImage2D重新指定1X1的大小，避免大的纹理内存未释放。
    if (!base::android::SysUtils::RunningOnJellyBeanOrHigher() && (std::string::npos != gl_renderer.find("PowerVR SGX 540"))) {
        use_teximage2d_free_memory_ = true;
    }

    // 小米4上运行有bindFrameBuffer的webGL用例时，偶尔会有闪屏。
    // 在bindFrameBuffer后强制调用glFinish规避此bug。Bug:50708292
    if (!base::android::SysUtils::RunningOnLollipopOrHigher() && (std::string::npos != gl_renderer.find("Adreno (TM) 330")) && (std::string::npos != gl_version.find("OpenGL ES 3.0"))) {
        force_gl_finish_after_bind_frame_buffer_ = true;
    }

    /*
 LOG(INFO) << "GpuBugManager::Initialize. gl_vendor:" << gl_vendor
           << ", gl_renderer:" << gl_renderer << ", gl_version:" << gl_version
           << ", disable_egl_khr_client_wait_sync_:" << disable_egl_khr_client_wait_sync_
           << ", disable_egl_image_texture_access_:" << disable_egl_image_texture_access_
           << ", use_teximage2d_free_memory_:" << use_teximage2d_free_memory_
           << ", force_gl_finish_after_bind_frame_buffer_:" << force_gl_finish_after_bind_frame_buffer_;
  */
}

} // namespace gpu

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/async_pixel_transfer_manager.h"

#include "base/android/sys_utils.h"
#include "base/debug/trace_event.h"
#include "gpu/command_buffer/service/async_pixel_transfer_manager_egl.h"
#include "gpu/command_buffer/service/async_pixel_transfer_manager_idle.h"
#include "gpu/command_buffer/service/async_pixel_transfer_manager_stub.h"
#include "gpu/command_buffer/service/async_pixel_transfer_manager_sync.h"
#ifdef TENCENT_CHANGES
#include "gpu/command_buffer/service/async_pixel_transfer_manager_share_group.h"
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#endif
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_implementation.h"

namespace gpu {
namespace {

enum GpuType {
  GPU_BROADCOM,
  GPU_IMAGINATION,
  GPU_NVIDIA_ES31,
  GPU_ADRENO_420,
#ifdef TENCENT_CHANGES
  GPU_Hisilicon,
  GPU_Vivante_OS_4_4,
#endif
  GPU_OTHER,
};

std::string MakeString(const char* s) {
  return std::string(s ? s : "");
}

GpuType GetGpuType() {
  const std::string vendor = MakeString(
      reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
  const std::string renderer = MakeString(
      reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
  const std::string version = MakeString(
      reinterpret_cast<const char*>(glGetString(GL_VERSION)));
#ifdef TENCENT_CHANGES
  const std::string extensions = MakeString(
      reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));
#endif

  if (vendor.find("Broadcom") != std::string::npos)
    return GPU_BROADCOM;

  if (vendor.find("Imagination") != std::string::npos)
    return GPU_IMAGINATION;

  if (vendor.find("NVIDIA") != std::string::npos &&
      version.find("OpenGL ES 3.1") != std::string::npos) {
    return GPU_NVIDIA_ES31;
  }

  if (vendor.find("Qualcomm") != std::string::npos &&
      renderer.find("Adreno (TM) 420") != std::string::npos) {
    return GPU_ADRENO_420;
  }

#ifdef TENCENT_CHANGES
  if (vendor.find("Hisilicon") != std::string::npos)
    return GPU_Hisilicon;

  //因为没有手机，暂时用chromium中判断Vivante GPU的代码。
  if ((extensions.find("GL_VIV_shader_binary") != std::string::npos) &&
		  base::android::SysUtils::RunningOnKitKatOrHigher())
    return GPU_Vivante_OS_4_4;
#endif

  return GPU_OTHER;
}

bool AllowTransferThreadForGpu() {
  GpuType gpu = GetGpuType();
#ifdef TENCENT_CHANGES
  return gpu != GPU_BROADCOM && gpu != GPU_IMAGINATION &&
         gpu != GPU_NVIDIA_ES31 && gpu != GPU_ADRENO_420 &&
         gpu != GPU_Vivante_OS_4_4;
#else
  return gpu != GPU_BROADCOM && gpu != GPU_IMAGINATION &&
         gpu != GPU_NVIDIA_ES31 && gpu != GPU_ADRENO_420;
#endif
}

}

// We only used threaded uploads when we can:
// - Create EGLImages out of OpenGL textures (EGL_KHR_gl_texture_2D_image)
// - Bind EGLImages to OpenGL textures (GL_OES_EGL_image)
// - Use fences (to test for upload completion).
// - The heap size is large enough.
// TODO(kaanb|epenner): Remove the IsImagination() check pending the
// resolution of crbug.com/249147
// TODO(kaanb|epenner): Remove the IsLowEndDevice() check pending the
// resolution of crbug.com/271929
AsyncPixelTransferManager* AsyncPixelTransferManager::Create(
    gfx::GLContext* context) {
  DCHECK(context->IsCurrent(NULL));
#ifdef TENCENT_CHANGES
  const CommandLine* cmd = CommandLine::ForCurrentProcess();
#endif
  switch (gfx::GetGLImplementation()) {
    case gfx::kGLImplementationEGLGLES2:
      DCHECK(context);
      if (!base::android::SysUtils::IsLowEndDevice() &&
          context->HasExtension("EGL_KHR_fence_sync") &&
#ifdef TENCENT_CHANGES
          (context->HasExtension("EGL_KHR_image") || GetGpuType() == GPU_Hisilicon) &&
#else
          context->HasExtension("EGL_KHR_image") &&
#endif
          context->HasExtension("EGL_KHR_image_base") &&
          context->HasExtension("EGL_KHR_gl_texture_2D_image") &&
          context->HasExtension("GL_OES_EGL_image") &&
#ifdef TENCENT_CHANGES
          AllowTransferThreadForGpu() &&
          // 后台强制走EglImage的方式上传纹理
          (!cmd->HasSwitch(switches::kForceUploadTextureMode) ||
           (cmd->GetSwitchValueASCII(switches::kForceUploadTextureMode) ==
                   switches::kUploadTextureModeEglImage))) {
#else
          AllowTransferThreadForGpu()) {
#endif
        TRACE_EVENT0("gpu", "AsyncPixelTransferManager_CreateWithThread");
        return new AsyncPixelTransferManagerEGL;
      }
#ifdef TENCENT_CHANGES
      //Android5.0以上的系统，上传纹理已经不在UI线程了，而变到RT线程，所以不需要使用下面的方式进行异步上传纹理。
      if (!base::android::SysUtils::RunningOnLollipopOrHigher() &&
          // 后台强制走ShareGroup的方式上传纹理
          (!cmd->HasSwitch(switches::kForceUploadTextureMode) || 
            (cmd->GetSwitchValueASCII(switches::kForceUploadTextureMode) ==
              switches::kUploadTextureModeShareGroup))) {
        AsyncPixelTransferManager* transferManager =
    		    static_cast<AsyncPixelTransferManager*>(AsyncPixelTransferManagerShareGroup::Create(context));
        if (transferManager)
    	    return transferManager;
      }
#endif
      return new AsyncPixelTransferManagerIdle;
    case gfx::kGLImplementationOSMesaGL: {
      TRACE_EVENT0("gpu", "AsyncPixelTransferManager_CreateIdle");
      return new AsyncPixelTransferManagerIdle;
    }
    case gfx::kGLImplementationMockGL:
      return new AsyncPixelTransferManagerStub;
    default:
      NOTREACHED();
      return NULL;
  }
}

}  // namespace gpu

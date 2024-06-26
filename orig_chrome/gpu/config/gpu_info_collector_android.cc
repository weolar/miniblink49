// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/config/gpu_info_collector.h"

#include "base/android/build_info.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/native_library.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_piece.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "ui/gl/egl_util.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_surface.h"

namespace {

std::pair<std::string, size_t> GetVersionFromString(
    const std::string& version_string,
    size_t begin = 0)
{
    begin = version_string.find_first_of("0123456789", begin);
    if (begin == std::string::npos)
        return std::make_pair("", std::string::npos);

    size_t end = version_string.find_first_not_of("01234567890.", begin);
    std::string sub_string;
    if (end != std::string::npos)
        sub_string = version_string.substr(begin, end - begin);
    else
        sub_string = version_string.substr(begin);
    std::vector<std::string> pieces = base::SplitString(
        sub_string, ".", base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
    if (pieces.size() >= 2)
        return std::make_pair(pieces[0] + "." + pieces[1], end);
    else
        return std::make_pair("", end);
}

std::string GetDriverVersionFromString(const std::string& version_string)
{
    // We expect that android GL_VERSION strings will be of a form
    // similar to: "OpenGL ES 2.0 V@6.0 AU@ (CL@2946718)" where the
    // first match to [0-9][0-9.]* is the OpenGL ES version number, and
    // the second match to [0-9][0-9.]* is the driver version (in this
    // case, 6.0).
    // It is currently assumed that the driver version has at least one
    // period in it, and only the first two components are significant.
    size_t begin = GetVersionFromString(version_string).second;
    if (begin == std::string::npos)
        return "0";

    std::pair<std::string, size_t> driver_version = GetVersionFromString(version_string, begin);
    if (driver_version.first == "")
        return "0";

    return driver_version.first;
}

gpu::CollectInfoResult CollectDriverInfo(gpu::GPUInfo* gpu_info)
{
    // Go through the process of loading GL libs and initializing an EGL
    // context so that we can get GL vendor/version/renderer strings.
    base::NativeLibrary gles_library, egl_library;
    base::NativeLibraryLoadError error;
    gles_library = base::LoadNativeLibrary(base::FilePath("libGLESv2.so"), &error);
    if (!gles_library)
        LOG(FATAL) << "Failed to load libGLESv2.so";

    egl_library = base::LoadNativeLibrary(base::FilePath("libEGL.so"), &error);
    if (!egl_library)
        LOG(FATAL) << "Failed to load libEGL.so";

    typedef void* (*eglGetProcAddressProc)(const char* name);

    auto eglGetProcAddressFn = reinterpret_cast<eglGetProcAddressProc>(
        base::GetFunctionPointerFromNativeLibrary(egl_library,
            "eglGetProcAddress"));
    if (!eglGetProcAddressFn)
        LOG(FATAL) << "eglGetProcAddress not found.";

    auto get_func = [eglGetProcAddressFn, gles_library, egl_library](
                        const char* name) {
        void* proc;
        proc = base::GetFunctionPointerFromNativeLibrary(egl_library, name);
        if (proc)
            return proc;
        proc = base::GetFunctionPointerFromNativeLibrary(gles_library, name);
        if (proc)
            return proc;
        proc = eglGetProcAddressFn(name);
        if (proc)
            return proc;
        LOG(FATAL) << "Failed to look up " << name;
        return (void*)nullptr;
    };

#define LOOKUP_FUNC(x) auto x##Fn = reinterpret_cast<gfx::x##Proc>(get_func(#x))

    LOOKUP_FUNC(eglGetError);
    LOOKUP_FUNC(eglQueryString);
    LOOKUP_FUNC(eglGetCurrentContext);
    LOOKUP_FUNC(eglGetCurrentDisplay);
    LOOKUP_FUNC(eglGetCurrentSurface);
    LOOKUP_FUNC(eglGetDisplay);
    LOOKUP_FUNC(eglInitialize);
    LOOKUP_FUNC(eglChooseConfig);
    LOOKUP_FUNC(eglCreateContext);
    LOOKUP_FUNC(eglCreatePbufferSurface);
    LOOKUP_FUNC(eglMakeCurrent);
    LOOKUP_FUNC(eglDestroySurface);
    LOOKUP_FUNC(eglDestroyContext);

    LOOKUP_FUNC(glGetString);
    LOOKUP_FUNC(glGetIntegerv);

#undef LOOKUP_FUNC

    EGLDisplay curr_display = eglGetCurrentDisplayFn();
    EGLContext curr_context = eglGetCurrentContextFn();
    EGLSurface curr_draw_surface = eglGetCurrentSurfaceFn(EGL_DRAW);
    EGLSurface curr_read_surface = eglGetCurrentSurfaceFn(EGL_READ);

    EGLDisplay temp_display = EGL_NO_DISPLAY;
    EGLContext temp_context = EGL_NO_CONTEXT;
    EGLSurface temp_surface = EGL_NO_SURFACE;

    const EGLint kConfigAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_NONE
    };
    const EGLint kContextAttribs[] = {
        EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_EXT,
        EGL_LOSE_CONTEXT_ON_RESET_EXT,
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    const EGLint kSurfaceAttribs[] = {
        EGL_WIDTH, 1,
        EGL_HEIGHT, 1,
        EGL_NONE
    };

    EGLint major, minor;

    EGLConfig config;
    EGLint num_configs;

    auto errorstr = [eglGetErrorFn]() {
        uint32_t err = eglGetErrorFn();
        return base::StringPrintf("%s (%x)", ui::GetEGLErrorString(err), err);
    };

    temp_display = eglGetDisplayFn(EGL_DEFAULT_DISPLAY);

    if (temp_display == EGL_NO_DISPLAY) {
        LOG(FATAL) << "failed to get display. " << errorstr();
    }

    eglInitializeFn(temp_display, &major, &minor);

    bool egl_create_context_robustness_supported = strstr(reinterpret_cast<const char*>(
                                                              eglQueryStringFn(temp_display, EGL_EXTENSIONS)),
                                                       "EGL_EXT_create_context_robustness")
        != NULL;

    if (!eglChooseConfigFn(temp_display, kConfigAttribs, &config, 1,
            &num_configs)) {
        LOG(FATAL) << "failed to choose an egl config. " << errorstr();
    }

    temp_context = eglCreateContextFn(
        temp_display, config, EGL_NO_CONTEXT,
        kContextAttribs + (egl_create_context_robustness_supported ? 0 : 2));
    if (temp_context == EGL_NO_CONTEXT) {
        LOG(FATAL)
            << "failed to create a temporary context for fetching driver strings. "
            << errorstr();
    }

    temp_surface = eglCreatePbufferSurfaceFn(temp_display, config, kSurfaceAttribs);

    if (temp_surface == EGL_NO_SURFACE) {
        eglDestroyContextFn(temp_display, temp_context);
        LOG(FATAL)
            << "failed to create a pbuffer surface for fetching driver strings. "
            << errorstr();
    }

    eglMakeCurrentFn(temp_display, temp_surface, temp_surface, temp_context);

    gpu_info->gl_vendor = reinterpret_cast<const char*>(glGetStringFn(GL_VENDOR));
    gpu_info->gl_version = reinterpret_cast<const char*>(glGetStringFn(GL_VERSION));
    gpu_info->gl_renderer = reinterpret_cast<const char*>(glGetStringFn(GL_RENDERER));
    gpu_info->gl_extensions = reinterpret_cast<const char*>(glGetStringFn(GL_EXTENSIONS));

    GLint max_samples = 0;
    glGetIntegervFn(GL_MAX_SAMPLES, &max_samples);
    gpu_info->max_msaa_samples = base::IntToString(max_samples);

    bool supports_robustness = gpu_info->gl_extensions.find("GL_EXT_robustness") != std::string::npos || gpu_info->gl_extensions.find("GL_KHR_robustness") != std::string::npos || gpu_info->gl_extensions.find("GL_ARB_robustness") != std::string::npos;

    if (supports_robustness) {
        glGetIntegervFn(
            GL_RESET_NOTIFICATION_STRATEGY_ARB,
            reinterpret_cast<GLint*>(&gpu_info->gl_reset_notification_strategy));
    }

    std::string glsl_version_string = reinterpret_cast<const char*>(glGetStringFn(GL_SHADING_LANGUAGE_VERSION));

    std::string glsl_version = GetVersionFromString(glsl_version_string).first;
    gpu_info->pixel_shader_version = glsl_version;
    gpu_info->vertex_shader_version = glsl_version;

    if (curr_display != EGL_NO_DISPLAY && curr_context != EGL_NO_CONTEXT) {
        eglMakeCurrentFn(curr_display, curr_draw_surface, curr_read_surface,
            curr_context);
    } else {
        eglMakeCurrentFn(temp_display, EGL_NO_SURFACE, EGL_NO_SURFACE,
            EGL_NO_CONTEXT);
    }

    eglDestroySurfaceFn(temp_display, temp_surface);
    eglDestroyContextFn(temp_display, temp_context);

    return gpu::kCollectInfoSuccess;
}

}

namespace gpu {

CollectInfoResult CollectContextGraphicsInfo(GPUInfo* gpu_info)
{
    /// TODO(tobiasjs) Check if CollectGraphicsInfo in gpu_main.cc
    /// really only needs basic graphics info on all platforms, and if
    /// so switch it to using that and make this the NOP that it really
    /// should be, to avoid potential double collection of info.
    return CollectBasicGraphicsInfo(gpu_info);
}

CollectInfoResult CollectGpuID(uint32* vendor_id, uint32* device_id)
{
    DCHECK(vendor_id && device_id);
    *vendor_id = 0;
    *device_id = 0;
    return kCollectInfoNonFatalFailure;
}

CollectInfoResult CollectBasicGraphicsInfo(GPUInfo* gpu_info)
{
    gpu_info->can_lose_context = false;

    gpu_info->machine_model_name = base::android::BuildInfo::GetInstance()->model();

    // Create a short-lived context on the UI thread to collect the GL strings.
    // Make sure we restore the existing context if there is one.
    CollectInfoResult result = CollectDriverInfo(gpu_info);
    if (result == kCollectInfoSuccess)
        result = CollectDriverInfoGL(gpu_info);
    gpu_info->basic_info_state = result;
    gpu_info->context_info_state = result;
    return result;
}

CollectInfoResult CollectDriverInfoGL(GPUInfo* gpu_info)
{
    gpu_info->driver_version = GetDriverVersionFromString(
        gpu_info->gl_version);
    gpu_info->gpu.vendor_string = gpu_info->gl_vendor;
    gpu_info->gpu.device_string = gpu_info->gl_renderer;
    return kCollectInfoSuccess;
}

void MergeGPUInfo(GPUInfo* basic_gpu_info,
    const GPUInfo& context_gpu_info)
{
    MergeGPUInfoGL(basic_gpu_info, context_gpu_info);
}

} // namespace gpu

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/config/gpu_info_collector.h"

#include "base/command_line.h"
#include "base/debug/trace_event.h"
#include "base/logging.h"
#include "base/strings/string_split.h"
#include "ui/gl/gl_surface_egl.h"
#include "ui/gl/gl_switches.h"

namespace gpu {

CollectInfoResult CollectContextGraphicsInfo(GPUInfo* gpu_info) {
  DCHECK(gpu_info);
  TRACE_EVENT0("gpu", "gpu_info_collector::CollectGraphicsInfo");
  CollectInfoResult result = CollectGraphicsInfoGL(gpu_info);
  if (CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kGpuNoContextLost)) {
    gpu_info->can_lose_context = false;
  } else {
#if defined(OS_CHROMEOS)
    gpu_info->can_lose_context = false;
#else
    gpu_info->can_lose_context =
        !gfx::GLSurfaceEGL::IsCreateContextRobustnessSupported();
#endif
  }

  gpu_info->finalized = true;
  return result;
}

GpuIDResult CollectGpuID(uint32* vendor_id, uint32* device_id) {
  DCHECK(vendor_id && device_id);
  *vendor_id = 0;
  *device_id = 0;
  return kGpuIDNotSupported;
}

CollectInfoResult CollectBasicGraphicsInfo(GPUInfo* gpu_info) {
  gpu_info->can_lose_context = false;
  return kCollectInfoSuccess;
}

CollectInfoResult CollectDriverInfoGL(GPUInfo* gpu_info) {
  DCHECK(gpu_info);
  // Extract driver vendor, version from a string like:
  // "OpenGL ES 3.0 V@6.0 AU@ (CL@2946718)"
  size_t begin = gpu_info->gl_version.find_first_of("0123456789");
  if (begin == std::string::npos)
    return kCollectInfoNonFatalFailure;

  std::string sub_string = gpu_info->gl_version.substr(begin);
  std::vector<std::string> pieces;
  base::SplitStringAlongWhitespace(sub_string, &pieces);
  if (pieces.size() < 3)
    return kCollectInfoNonFatalFailure;

  std::string driver_version = pieces[2];
  size_t pos = driver_version.find_first_not_of("0123456789.");
  if (pos == 0)
    return kCollectInfoNonFatalFailure;

  if (pos != std::string::npos)
    driver_version = driver_version.substr(0, pos);

  gpu_info->driver_vendor = pieces[1];
  gpu_info->driver_version = driver_version;
  return kCollectInfoSuccess;
}

void MergeGPUInfo(GPUInfo* basic_gpu_info,
                  const GPUInfo& context_gpu_info) {
  MergeGPUInfoGL(basic_gpu_info, context_gpu_info);
}

}  // namespace gpu

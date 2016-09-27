// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/gpu_switches.h"
#include "base/basictypes.h"
#ifdef TENCENT_CHANGES
#include "gpu/command_buffer/service/mailbox_synchronizer.h"
#endif

namespace switches {

// Always return success when compiling a shader. Linking will still fail.
const char kCompileShaderAlwaysSucceeds[]   = "compile-shader-always-succeeds";

// Disable the GL error log limit.
const char kDisableGLErrorLimit[]           = "disable-gl-error-limit";

// Disable the GLSL translator.
const char kDisableGLSLTranslator[]         = "disable-glsl-translator";

// Disable workarounds for various GPU driver bugs.
const char kDisableGpuDriverBugWorkarounds[] =
    "disable-gpu-driver-bug-workarounds";

// Turn off user-defined name hashing in shaders.
const char kDisableShaderNameHashing[]      = "disable-shader-name-hashing";

// Turn on Logging GPU commands.
const char kEnableGPUCommandLogging[]       = "enable-gpu-command-logging";

// Turn on Calling GL Error after every command.
const char kEnableGPUDebugging[]            = "enable-gpu-debugging";

// Enable GPU service logging. Note: This is the same switch as the one in
// gl_switches.cc. It's defined here again to avoid dependencies between
// dlls.
const char kEnableGPUServiceLoggingGPU[]    = "enable-gpu-service-logging";

// Turn off gpu program caching
const char kDisableGpuProgramCache[]        = "disable-gpu-program-cache";

// Enforce GL minimums.
const char kEnforceGLMinimums[]             = "enforce-gl-minimums";

// Sets the total amount of memory that may be allocated for GPU resources
const char kForceGpuMemAvailableMb[]        = "force-gpu-mem-available-mb";

// Pass a set of GpuDriverBugWorkaroundType ids, seperated by ','.
const char kGpuDriverBugWorkarounds[] = "gpu-driver-bug-workarounds";

// Sets the maximum size of the in-memory gpu program cache, in kb
const char kGpuProgramCacheSizeKb[]         = "gpu-program-cache-size-kb";

// Disables the GPU shader on disk cache.
const char kDisableGpuShaderDiskCache[]     = "disable-gpu-shader-disk-cache";

// Allows async texture uploads (off main thread) via GL context sharing.
const char kEnableShareGroupAsyncTextureUpload[] =
    "enable-share-group-async-texture-upload";

const char* kGpuSwitches[] = {
  kCompileShaderAlwaysSucceeds,
  kDisableGLErrorLimit,
  kDisableGLSLTranslator,
  kDisableGpuDriverBugWorkarounds,
  kDisableShaderNameHashing,
  kEnableGPUCommandLogging,
  kEnableGPUDebugging,
  kEnableGPUServiceLoggingGPU,
  kDisableGpuProgramCache,
  kEnforceGLMinimums,
  kForceGpuMemAvailableMb,
  kGpuDriverBugWorkarounds,
  kGpuProgramCacheSizeKb,
  kDisableGpuShaderDiskCache,
  kEnableShareGroupAsyncTextureUpload,
};

const int kNumGpuSwitches = arraysize(kGpuSwitches);

}  // namespace switches

#ifdef TENCENT_CHANGES
bool UseShareGroupInMailBoxSynchronizer() {
  if (gpu::gles2::MailboxSynchronizer::GetInstance())
	return gpu::gles2::MailboxSynchronizer::GetInstance()->IsUseShareGroup();
  return false;
}
#endif

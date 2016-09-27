// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_CONFIG_GPU_PERFORMANCE_STATS_H_
#define GPU_CONFIG_GPU_PERFORMANCE_STATS_H_

#include "gpu/gpu_export.h"

namespace gpu {

struct GPU_EXPORT GpuPerformanceStats {
  GpuPerformanceStats() : graphics(0.f), gaming(0.f), overall(0.f) {}

  float graphics;
  float gaming;
  float overall;
};

}  // namespace gpu

#endif  // GPU_CONFIG_GPU_PERFORMANCE_STATS_H_


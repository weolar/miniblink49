// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RASTER_SCOPED_GPU_RASTER_H_
#define CC_RASTER_SCOPED_GPU_RASTER_H_

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/output/context_provider.h"

namespace cc {

// The following class is needed to modify GL resources using GPU
// raster. The user must ensure that they only use GPU raster on
// GL resources while an instance of this class is alive.
class CC_EXPORT ScopedGpuRaster {
public:
    explicit ScopedGpuRaster(ContextProvider* context_provider);
    ~ScopedGpuRaster();

private:
    void BeginGpuRaster();
    void EndGpuRaster();

    ContextProvider* context_provider_;

    DISALLOW_COPY_AND_ASSIGN(ScopedGpuRaster);
};

} // namespace cc

#endif // CC_RASTER_SCOPED_GPU_RASTER_H_

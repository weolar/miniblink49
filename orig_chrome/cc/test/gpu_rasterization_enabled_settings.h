// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_GPU_RASTERIZATION_ENABLED_SETTINGS_H_
#define CC_TEST_GPU_RASTERIZATION_ENABLED_SETTINGS_H_

#include "cc/trees/layer_tree_settings.h"

namespace cc {

class GpuRasterizationEnabledSettings : public LayerTreeSettings {
public:
    GpuRasterizationEnabledSettings()
    {
        gpu_rasterization_enabled = true;
    }
};

} // namespace cc

#endif // CC_TEST_GPU_RASTERIZATION_ENABLED_SETTINGS_H_

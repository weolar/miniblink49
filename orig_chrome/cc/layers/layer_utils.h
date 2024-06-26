// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_LAYER_UTILS_H_
#define CC_LAYERS_LAYER_UTILS_H_

#include "cc/base/cc_export.h"

namespace gfx {
class BoxF;
} // namespace gfx

namespace cc {
class LayerImpl;

class CC_EXPORT LayerUtils {
public:
    // Computes a box in screen space that should entirely contain the layer's
    // bounds through the entirety of the layer's current animation. Returns
    // true and sets |out| to the inflation if there are animations that can
    // inflate bounds in the path to the root layer and that it was able to
    // inflate correctly. Returns false otherwise.
    static bool GetAnimationBounds(const LayerImpl& layer, gfx::BoxF* out);
};

} // namespace cc

#endif // CC_LAYERS_LAYER_UTILS_H_

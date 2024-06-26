// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_SOLID_COLOR_CONTENT_LAYER_CLIENT_H_
#define CC_TEST_SOLID_COLOR_CONTENT_LAYER_CLIENT_H_

#include "base/compiler_specific.h"
#include "cc/layers/content_layer_client.h"
#include "third_party/skia/include/core/SkColor.h"

namespace cc {

class SolidColorContentLayerClient : public ContentLayerClient {
public:
    explicit SolidColorContentLayerClient(SkColor color)
        : color_(color)
    {
    }

    // ContentLayerClient implementation.
    void PaintContents(SkCanvas* canvas,
        const gfx::Rect& rect,
        PaintingControlSetting painting_control) override;
    scoped_refptr<DisplayItemList> PaintContentsToDisplayList(
        const gfx::Rect& clip,
        PaintingControlSetting painting_control) override;
    bool FillsBoundsCompletely() const override;
    size_t GetApproximateUnsharedMemoryUsage() const override;

private:
    SkColor color_;
};

} // namespace cc

#endif // CC_TEST_SOLID_COLOR_CONTENT_LAYER_CLIENT_H_

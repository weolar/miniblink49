// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_CLIP_DISPLAY_ITEM_H_
#define CC_PLAYBACK_CLIP_DISPLAY_ITEM_H_

#include <vector>

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/playback/display_item.h"
#include "third_party/skia/include/core/SkRRect.h"
#include "ui/gfx/geometry/rect.h"

class SkCanvas;

namespace cc {

class CC_EXPORT ClipDisplayItem : public DisplayItem {
public:
    ClipDisplayItem();
    ~ClipDisplayItem() override;

    void SetNew(gfx::Rect clip_rect,
        const std::vector<SkRRect>& rounded_clip_rects);

    void Raster(SkCanvas* canvas,
        const gfx::Rect& canvas_target_playback_rect,
        SkPicture::AbortCallback* callback) const override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    void ProcessForBounds(
        DisplayItemListBoundsCalculator* calculator) const override;

private:
    gfx::Rect clip_rect_;
    std::vector<SkRRect> rounded_clip_rects_;
};

class CC_EXPORT EndClipDisplayItem : public DisplayItem {
public:
    EndClipDisplayItem();
    ~EndClipDisplayItem() override;

    void Raster(SkCanvas* canvas,
        const gfx::Rect& canvas_target_playback_rect,
        SkPicture::AbortCallback* callback) const override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    void ProcessForBounds(
        DisplayItemListBoundsCalculator* calculator) const override;
};

} // namespace cc

#endif // CC_PLAYBACK_CLIP_DISPLAY_ITEM_H_

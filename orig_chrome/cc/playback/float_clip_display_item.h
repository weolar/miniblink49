// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_FLOAT_CLIP_DISPLAY_ITEM_H_
#define CC_PLAYBACK_FLOAT_CLIP_DISPLAY_ITEM_H_

#include <vector>

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/playback/display_item.h"
#include "ui/gfx/geometry/rect_f.h"

class SkCanvas;

namespace cc {

class CC_EXPORT FloatClipDisplayItem : public DisplayItem {
public:
    FloatClipDisplayItem();
    ~FloatClipDisplayItem() override;

    void SetNew(const gfx::RectF& clip_rect);

    void Raster(SkCanvas* canvas,
        const gfx::Rect& canvas_target_playback_rect,
        SkPicture::AbortCallback* callback) const override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    void ProcessForBounds(
        DisplayItemListBoundsCalculator* calculator) const override;

private:
    gfx::RectF clip_rect_;
};

class CC_EXPORT EndFloatClipDisplayItem : public DisplayItem {
public:
    EndFloatClipDisplayItem();
    ~EndFloatClipDisplayItem() override;

    static scoped_ptr<EndFloatClipDisplayItem> Create()
    {
        return make_scoped_ptr(new EndFloatClipDisplayItem());
    }

    void Raster(SkCanvas* canvas,
        const gfx::Rect& canvas_target_playback_rect,
        SkPicture::AbortCallback* callback) const override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    void ProcessForBounds(
        DisplayItemListBoundsCalculator* calculator) const override;
};

} // namespace cc

#endif // CC_PLAYBACK_FLOAT_CLIP_DISPLAY_ITEM_H_

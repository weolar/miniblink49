// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_TRANSFORM_DISPLAY_ITEM_H_
#define CC_PLAYBACK_TRANSFORM_DISPLAY_ITEM_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/playback/display_item.h"
#include "ui/gfx/transform.h"

class SkCanvas;

namespace cc {

class CC_EXPORT TransformDisplayItem : public DisplayItem {
public:
    TransformDisplayItem();
    ~TransformDisplayItem() override;

    void SetNew(const gfx::Transform& transform);

    void Raster(SkCanvas* canvas,
        const gfx::Rect& canvas_target_playback_rect,
        SkPicture::AbortCallback* callback) const override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    void ProcessForBounds(
        DisplayItemListBoundsCalculator* calculator) const override;

private:
    gfx::Transform transform_;
};

class CC_EXPORT EndTransformDisplayItem : public DisplayItem {
public:
    EndTransformDisplayItem();
    ~EndTransformDisplayItem() override;

    static scoped_ptr<EndTransformDisplayItem> Create()
    {
        return make_scoped_ptr(new EndTransformDisplayItem());
    }

    void Raster(SkCanvas* canvas,
        const gfx::Rect& canvas_target_playback_rect,
        SkPicture::AbortCallback* callback) const override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    void ProcessForBounds(
        DisplayItemListBoundsCalculator* calculator) const override;
};

} // namespace cc

#endif // CC_PLAYBACK_TRANSFORM_DISPLAY_ITEM_H_

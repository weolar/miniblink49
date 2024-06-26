// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_FILTER_DISPLAY_ITEM_H_
#define CC_PLAYBACK_FILTER_DISPLAY_ITEM_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/output/filter_operations.h"
#include "cc/playback/display_item.h"
#include "ui/gfx/geometry/rect_f.h"

class SkCanvas;

namespace cc {

class CC_EXPORT FilterDisplayItem : public DisplayItem {
public:
    FilterDisplayItem();
    ~FilterDisplayItem() override;

    void SetNew(const FilterOperations& filters, const gfx::RectF& bounds);

    void Raster(SkCanvas* canvas,
        const gfx::Rect& canvas_target_playback_rect,
        SkPicture::AbortCallback* callback) const override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    void ProcessForBounds(
        DisplayItemListBoundsCalculator* calculator) const override;

private:
    FilterOperations filters_;
    gfx::RectF bounds_;
};

class CC_EXPORT EndFilterDisplayItem : public DisplayItem {
public:
    EndFilterDisplayItem();
    ~EndFilterDisplayItem() override;

    static scoped_ptr<EndFilterDisplayItem> Create()
    {
        return make_scoped_ptr(new EndFilterDisplayItem());
    }

    void Raster(SkCanvas* canvas,
        const gfx::Rect& canvas_target_playback_rect,
        SkPicture::AbortCallback* callback) const override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    void ProcessForBounds(
        DisplayItemListBoundsCalculator* calculator) const override;
};

} // namespace cc

#endif // CC_PLAYBACK_FILTER_DISPLAY_ITEM_H_

// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_COMPOSITING_DISPLAY_ITEM_H_
#define CC_PLAYBACK_COMPOSITING_DISPLAY_ITEM_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/playback/display_item.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkColorFilter.h"
#include "third_party/skia/include/core/SkRect.h"
#include "third_party/skia/include/core/SkXfermode.h"
#include "ui/gfx/geometry/rect_f.h"

class SkCanvas;

namespace cc {

class CC_EXPORT CompositingDisplayItem : public DisplayItem {
public:
    CompositingDisplayItem();
    ~CompositingDisplayItem() override;

    void SetNew(uint8_t alpha,
        SkXfermode::Mode xfermode,
        SkRect* bounds,
        skia::RefPtr<SkColorFilter> color_filter);

    void Raster(SkCanvas* canvas,
        const gfx::Rect& canvas_target_playback_rect,
        SkPicture::AbortCallback* callback) const override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    void ProcessForBounds(
        DisplayItemListBoundsCalculator* calculator) const override;

private:
    uint8_t alpha_;
    SkXfermode::Mode xfermode_;
    bool has_bounds_;
    SkRect bounds_;
    skia::RefPtr<SkColorFilter> color_filter_;
};

class CC_EXPORT EndCompositingDisplayItem : public DisplayItem {
public:
    EndCompositingDisplayItem();
    ~EndCompositingDisplayItem() override;

    static scoped_ptr<EndCompositingDisplayItem> Create()
    {
        return make_scoped_ptr(new EndCompositingDisplayItem());
    }

    void Raster(SkCanvas* canvas,
        const gfx::Rect& canvas_target_playback_rect,
        SkPicture::AbortCallback* callback) const override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    void ProcessForBounds(
        DisplayItemListBoundsCalculator* calculator) const override;
};

} // namespace cc

#endif // CC_PLAYBACK_COMPOSITING_DISPLAY_ITEM_H_

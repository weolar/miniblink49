// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_CLIP_PATH_DISPLAY_ITEM_H_
#define CC_PLAYBACK_CLIP_PATH_DISPLAY_ITEM_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/playback/display_item.h"
#include "third_party/skia/include/core/SkPath.h"
#include "third_party/skia/include/core/SkRegion.h"

class SkCanvas;

namespace cc {

class CC_EXPORT ClipPathDisplayItem : public DisplayItem {
public:
    ClipPathDisplayItem();
    ~ClipPathDisplayItem() override;

    void SetNew(const SkPath& path, SkRegion::Op clip_op, bool antialias);

    void Raster(SkCanvas* canvas,
        const gfx::Rect& canvas_target_playback_rect,
        SkPicture::AbortCallback* callback) const override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    void ProcessForBounds(
        DisplayItemListBoundsCalculator* calculator) const override;

private:
    SkPath clip_path_;
    SkRegion::Op clip_op_;
    bool antialias_;
};

class CC_EXPORT EndClipPathDisplayItem : public DisplayItem {
public:
    EndClipPathDisplayItem();
    ~EndClipPathDisplayItem() override;

    static scoped_ptr<EndClipPathDisplayItem> Create()
    {
        return make_scoped_ptr(new EndClipPathDisplayItem());
    }

    void Raster(SkCanvas* canvas,
        const gfx::Rect& canvas_target_playback_rect,
        SkPicture::AbortCallback* callback) const override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    void ProcessForBounds(
        DisplayItemListBoundsCalculator* calculator) const override;
};

} // namespace cc

#endif // CC_PLAYBACK_CLIP_PATH_DISPLAY_ITEM_H_

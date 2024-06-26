// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_DRAWING_DISPLAY_ITEM_H_
#define CC_PLAYBACK_DRAWING_DISPLAY_ITEM_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/playback/display_item.h"
#include "skia/ext/refptr.h"
#include "ui/gfx/geometry/point_f.h"

class SkCanvas;
class SkPicture;

namespace cc {

class CC_EXPORT DrawingDisplayItem : public DisplayItem {
public:
    DrawingDisplayItem();
    ~DrawingDisplayItem() override;

    void SetNew(skia::RefPtr<SkPicture> picture);

    void Raster(SkCanvas* canvas,
        const gfx::Rect& canvas_playback_rect,
        SkPicture::AbortCallback* callback) const override;
    void AsValueInto(base::trace_event::TracedValue* array) const override;
    void ProcessForBounds(
        DisplayItemListBoundsCalculator* calculator) const override;

    void CloneTo(DrawingDisplayItem* item) const;

private:
    skia::RefPtr<SkPicture> picture_;
};

} // namespace cc

#endif // CC_PLAYBACK_DRAWING_DISPLAY_ITEM_H_

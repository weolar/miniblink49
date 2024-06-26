// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_CONTENT_LAYER_CLIENT_H_
#define CC_TEST_FAKE_CONTENT_LAYER_CLIENT_H_

#include <utility>
#include <vector>

#include "base/compiler_specific.h"
#include "cc/layers/content_layer_client.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/transform.h"

class SkImage;

namespace cc {

class FakeContentLayerClient : public ContentLayerClient {
public:
    struct ImageData {
        ImageData(const SkImage* image,
            const gfx::Point& point,
            const SkPaint& paint);
        ImageData(const SkImage* image,
            const gfx::Transform& transform,
            const SkPaint& paint);
        ~ImageData();
        skia::RefPtr<const SkImage> image;
        gfx::Point point;
        gfx::Transform transform;
        SkPaint paint;
    };

    FakeContentLayerClient();
    ~FakeContentLayerClient() override;

    void PaintContents(SkCanvas* canvas,
        const gfx::Rect& rect,
        PaintingControlSetting painting_control) override;
    scoped_refptr<DisplayItemList> PaintContentsToDisplayList(
        const gfx::Rect& clip,
        PaintingControlSetting painting_control) override;
    bool FillsBoundsCompletely() const override;
    size_t GetApproximateUnsharedMemoryUsage() const override;

    void set_fill_with_nonsolid_color(bool nonsolid)
    {
        fill_with_nonsolid_color_ = nonsolid;
    }

    void add_draw_rect(const gfx::Rect& rect, const SkPaint& paint)
    {
        draw_rects_.push_back(std::make_pair(gfx::RectF(rect), paint));
    }

    void add_draw_rectf(const gfx::RectF& rect, const SkPaint& paint)
    {
        draw_rects_.push_back(std::make_pair(rect, paint));
    }

    void add_draw_image(const SkImage* image,
        const gfx::Point& point,
        const SkPaint& paint)
    {
        ImageData data(image, point, paint);
        draw_images_.push_back(data);
    }

    void add_draw_image_with_transform(const SkImage* image,
        const gfx::Transform& transform,
        const SkPaint& paint)
    {
        ImageData data(image, transform, paint);
        draw_images_.push_back(data);
    }

    SkCanvas* last_canvas() const { return last_canvas_; }

    PaintingControlSetting last_painting_control() const
    {
        return last_painting_control_;
    }

    void set_reported_memory_usage(size_t reported_memory_usage)
    {
        reported_memory_usage_ = reported_memory_usage;
    }

private:
    typedef std::vector<std::pair<gfx::RectF, SkPaint>> RectPaintVector;
    typedef std::vector<ImageData> ImageVector;

    bool fill_with_nonsolid_color_;
    RectPaintVector draw_rects_;
    ImageVector draw_images_;
    SkCanvas* last_canvas_;
    PaintingControlSetting last_painting_control_;
    size_t reported_memory_usage_;
};

} // namespace cc

#endif // CC_TEST_FAKE_CONTENT_LAYER_CLIENT_H_

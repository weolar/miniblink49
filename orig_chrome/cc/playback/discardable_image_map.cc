// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/discardable_image_map.h"

#include <algorithm>
#include <limits>

#include "cc/base/math_util.h"
#include "cc/playback/display_item_list.h"
#include "third_party/skia/include/utils/SkNWayCanvas.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/skia_util.h"

namespace cc {

namespace {

    SkRect MapRect(const SkMatrix& matrix, const SkRect& src)
    {
        SkRect dst;
        matrix.mapRect(&dst, src);
        return dst;
    }

    // We're using an NWay canvas with no added canvases, so in effect
    // non-overridden functions are no-ops.
    class DiscardableImagesMetadataCanvas : public SkNWayCanvas {
    public:
        DiscardableImagesMetadataCanvas(int width,
            int height,
            std::vector<PositionImage>* image_set)
            : SkNWayCanvas(width, height)
            , image_set_(image_set)
            , canvas_bounds_(SkRect::MakeIWH(width, height))
        {
        }

    protected:
        // we need to "undo" the behavior of SkNWayCanvas, which will try to forward
        // it.
        void onDrawPicture(const SkPicture* picture,
            const SkMatrix* matrix,
            const SkPaint* paint) override
        {
            SkCanvas::onDrawPicture(picture, matrix, paint);
        }

        void onDrawImage(const SkImage* image,
            SkScalar x,
            SkScalar y,
            const SkPaint* paint) override
        {
            const SkMatrix& ctm = this->getTotalMatrix();
            AddImage(image, MapRect(ctm, SkRect::MakeXYWH(x, y, image->width(), image->height())),
                ctm, paint);
        }

        void onDrawImageRect(const SkImage* image,
            const SkRect* src,
            const SkRect& dst,
            const SkPaint* paint
            // , SrcRectConstraint
            ) override
        {
            const SkMatrix& ctm = this->getTotalMatrix();
            SkRect src_storage;
            if (!src) {
                src_storage = SkRect::MakeIWH(image->width(), image->height());
                src = &src_storage;
            }
            SkMatrix matrix;
            matrix.setRectToRect(*src, dst, SkMatrix::kFill_ScaleToFit);
            matrix.postConcat(ctm);
            AddImage(image, MapRect(ctm, dst), matrix, paint);
        }

        void onDrawImageNine(const SkImage* image,
            const SkIRect& center,
            const SkRect& dst,
            const SkPaint* paint) override
        {
            AddImage(image, dst, this->getTotalMatrix(), paint);
        }

    private:
        void AddImage(const SkImage* image,
            const SkRect& rect,
            const SkMatrix& matrix,
            const SkPaint* paint)
        {
            DebugBreak();
            if (rect.intersects(canvas_bounds_) /*&& image->isLazyGenerated()*/) {
                SkFilterQuality filter_quality = kNone_SkFilterQuality;
                if (paint) {
                    filter_quality = paint->getFilterQuality();
                }
                image_set_->push_back(PositionImage(image, rect, matrix, filter_quality));
            }
        }

        std::vector<PositionImage>* image_set_;
        const SkRect canvas_bounds_;
    };

} // namespace

DiscardableImageMap::DiscardableImageMap() { }

DiscardableImageMap::~DiscardableImageMap() { }

scoped_ptr<SkCanvas> DiscardableImageMap::BeginGeneratingMetadata(
    const gfx::Size& bounds)
{
    DCHECK(all_images_.empty());
    return scoped_ptr<SkCanvas>(new DiscardableImagesMetadataCanvas(
        bounds.width(), bounds.height(), &all_images_));
}

void DiscardableImageMap::EndGeneratingMetadata()
{
    images_rtree_.Build(all_images_, [](const PositionImage& image) {
        return gfx::SkRectToRectF(image.image_rect);
    });
}

void DiscardableImageMap::GetDiscardableImagesInRect(
    const gfx::Rect& rect,
    std::vector<PositionImage>* images)
{
    std::vector<size_t> indices;
    images_rtree_.Search(gfx::RectF(rect), &indices);
    for (size_t index : indices)
        images->push_back(all_images_[index]);
}

DiscardableImageMap::ScopedMetadataGenerator::ScopedMetadataGenerator(
    DiscardableImageMap* image_map,
    const gfx::Size& bounds)
    : image_map_(image_map)
    , metadata_canvas_(image_map->BeginGeneratingMetadata(bounds))
{
}

DiscardableImageMap::ScopedMetadataGenerator::~ScopedMetadataGenerator()
{
    image_map_->EndGeneratingMetadata();
}

} // namespace cc

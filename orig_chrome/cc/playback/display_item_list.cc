// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/display_item_list.h"

#include <string>

#include "base/numerics/safe_conversions.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/base/math_util.h"
#include "cc/debug/picture_debug_util.h"
#include "cc/debug/traced_display_item_list.h"
#include "cc/debug/traced_value.h"
#include "cc/playback/display_item_list_settings.h"
#include "cc/playback/largest_display_item.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "third_party/skia/include/utils/SkPictureUtils.h"
#include "ui/gfx/skia_util.h"

namespace cc {

namespace {

    // We don't perform per-layer solid color analysis when there are too many skia
    // operations.
    const int kOpCountThatIsOkToAnalyze = 10;

    bool DisplayItemsTracingEnabled()
    {
        bool tracing_enabled = false;
        TRACE_EVENT_CATEGORY_GROUP_ENABLED(
            TRACE_DISABLED_BY_DEFAULT("cc.debug.display_items"), &tracing_enabled);
        return tracing_enabled;
    }

    const int kDefaultNumDisplayItemsToReserve = 100;

} // namespace

scoped_refptr<DisplayItemList> DisplayItemList::Create(
    const gfx::Rect& layer_rect,
    const DisplayItemListSettings& settings)
{
    return make_scoped_refptr(new DisplayItemList(
        layer_rect, settings,
        !settings.use_cached_picture || DisplayItemsTracingEnabled()));
}

DisplayItemList::DisplayItemList(gfx::Rect layer_rect,
    const DisplayItemListSettings& settings,
    bool retain_individual_display_items)
    : items_(LargestDisplayItemSize(), kDefaultNumDisplayItemsToReserve)
    , use_cached_picture_(settings.use_cached_picture)
    , retain_individual_display_items_(retain_individual_display_items)
    , layer_rect_(layer_rect)
    , is_suitable_for_gpu_rasterization_(true)
    , approximate_op_count_(0)
    , picture_memory_usage_(0)
    , external_memory_usage_(0)
{
#if DCHECK_IS_ON()
    needs_process_ = false;
#endif
    if (use_cached_picture_) {
        SkRTreeFactory factory;
        recorder_.reset(new SkPictureRecorder());
        canvas_ = skia::SharePtr(recorder_->beginRecording(
            layer_rect_.width(), layer_rect_.height(), &factory));
        canvas_->translate(-layer_rect_.x(), -layer_rect_.y());
        canvas_->clipRect(gfx::RectToSkRect(layer_rect_));
    }
}

DisplayItemList::~DisplayItemList()
{
}

void DisplayItemList::Raster(SkCanvas* canvas,
    SkPicture::AbortCallback* callback,
    const gfx::Rect& canvas_target_playback_rect,
    float contents_scale) const
{
    DCHECK(ProcessAppendedItemsCalled());
    if (!use_cached_picture_) {
        canvas->save();
        canvas->scale(contents_scale, contents_scale);
        for (auto* item : items_)
            item->Raster(canvas, canvas_target_playback_rect, callback);
        canvas->restore();
    } else {
        DCHECK(picture_);

        canvas->save();
        canvas->scale(contents_scale, contents_scale);
        canvas->translate(layer_rect_.x(), layer_rect_.y());
        if (callback) {
            // If we have a callback, we need to call |draw()|, |drawPicture()|
            // doesn't take a callback.  This is used by |AnalysisCanvas| to early
            // out.
            picture_->playback(canvas, callback);
        } else {
            // Prefer to call |drawPicture()| on the canvas since it could place the
            // entire picture on the canvas instead of parsing the skia operations.
            canvas->drawPicture(picture_.get());
        }
        canvas->restore();
    }
}

void DisplayItemList::ProcessAppendedItemsOnTheFly()
{
    if (retain_individual_display_items_)
        return;
    if (items_.size() >= kDefaultNumDisplayItemsToReserve) {
        ProcessAppendedItems();
        // This function exists to keep the |items_| from growing indefinitely if
        // we're not going to store them anyway. So the items better be deleted
        // after |items_| grows too large and we process it.
        DCHECK(items_.empty());
    }
}

void DisplayItemList::ProcessAppendedItems()
{
#if DCHECK_IS_ON()
    needs_process_ = false;
#endif
    for (const DisplayItem* item : items_) {
        if (use_cached_picture_) {
            // When using a cached picture we will calculate gpu suitability on the
            // entire cached picture instead of the items. This is more permissive
            // since none of the items might individually trigger a veto even though
            // they collectively have enough "bad" operations that a corresponding
            // Picture would get vetoed. See crbug.com/513016.
            DCHECK(canvas_);
            approximate_op_count_ += item->approximate_op_count();
            item->Raster(canvas_.get(), gfx::Rect(), nullptr);
        } else {
            is_suitable_for_gpu_rasterization_ &= item->is_suitable_for_gpu_rasterization();
            approximate_op_count_ += item->approximate_op_count();
        }

        if (retain_individual_display_items_) {
            // Warning: this double-counts SkPicture data if use_cached_picture_ is
            // also true.
            external_memory_usage_ += item->external_memory_usage();
        }
    }

    if (!retain_individual_display_items_)
        items_.clear();
}

void DisplayItemList::RasterIntoCanvas(const DisplayItem& item)
{
    DCHECK(canvas_);
    DCHECK(!retain_individual_display_items_);
    approximate_op_count_ += item.approximate_op_count();

    item.Raster(canvas_.get(), gfx::Rect(), nullptr);
}

bool DisplayItemList::RetainsIndividualDisplayItems() const
{
    return retain_individual_display_items_;
}

void DisplayItemList::RemoveLast()
{
    // We cannot remove the last item if it has been squashed into a picture.
    // The last item should not have been handled by ProcessAppendedItems, so we
    // don't need to remove it from approximate_op_count_, etc.
    DCHECK(retain_individual_display_items_);
    DCHECK(!use_cached_picture_);
    items_.RemoveLast();
}

void DisplayItemList::Finalize()
{
    ProcessAppendedItems();

    if (use_cached_picture_) {
        // Convert to an SkPicture for faster rasterization.
        DCHECK(use_cached_picture_);
        DCHECK(!picture_);
        picture_ = skia::AdoptRef(recorder_->endRecordingAsPicture());
        DCHECK(picture_);
        picture_memory_usage_ = SkPictureUtils::ApproximateBytesUsed(picture_.get());
        recorder_.reset();
        canvas_.clear();
        is_suitable_for_gpu_rasterization_ = picture_->suitableForGpuRasterization(nullptr);
    }
}

bool DisplayItemList::IsSuitableForGpuRasterization() const
{
    DCHECK(ProcessAppendedItemsCalled());
    return is_suitable_for_gpu_rasterization_;
}

int DisplayItemList::ApproximateOpCount() const
{
    DCHECK(ProcessAppendedItemsCalled());
    return approximate_op_count_;
}

size_t DisplayItemList::ApproximateMemoryUsage() const
{
    DCHECK(ProcessAppendedItemsCalled());
    // We double-count in this case. Produce zero to avoid being misleading.
    if (use_cached_picture_ && retain_individual_display_items_)
        return 0;

    DCHECK_IMPLIES(use_cached_picture_, picture_);

    size_t memory_usage = sizeof(*this);

    // Memory outside this class due to |items_|.
    memory_usage += items_.GetCapacityInBytes() + external_memory_usage_;

    // Memory outside this class due to |picture|.
    memory_usage += picture_memory_usage_;

    // TODO(jbroman): Does anything else owned by this class substantially
    // contribute to memory usage?

    return memory_usage;
}

bool DisplayItemList::ShouldBeAnalyzedForSolidColor() const
{
    return ApproximateOpCount() <= kOpCountThatIsOkToAnalyze;
}

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
DisplayItemList::AsValue(bool include_items) const
{
    DCHECK(ProcessAppendedItemsCalled());
    scoped_refptr<base::trace_event::TracedValue> state = new base::trace_event::TracedValue();

    //   state->BeginDictionary("params");
    //   if (include_items) {
    //     state->BeginArray("items");
    //     for (const DisplayItem* item : items_) {
    //       item->AsValueInto(state.get());
    //     }
    //     state->EndArray();  // "items".
    //   }
    //   state->SetValue("layer_rect", MathUtil::AsValue(layer_rect_));
    //   state->EndDictionary();  // "params".
    //
    //   if (!layer_rect_.IsEmpty()) {
    //     SkPictureRecorder recorder;
    //     SkCanvas* canvas =
    //         recorder.beginRecording(layer_rect_.width(), layer_rect_.height());
    //     canvas->translate(-layer_rect_.x(), -layer_rect_.y());
    //     canvas->clipRect(gfx::RectToSkRect(layer_rect_));
    //     Raster(canvas, NULL, gfx::Rect(), 1.f);
    //     skia::RefPtr<SkPicture> picture =
    //         skia::AdoptRef(recorder.endRecordingAsPicture());
    //
    //     std::string b64_picture;
    //     PictureDebugUtil::SerializeAsBase64(picture.get(), &b64_picture);
    //     state->SetString("skp64", b64_picture);
    //   }
    DebugBreak();

    return state;
}

void DisplayItemList::EmitTraceSnapshot() const
{
    DCHECK(ProcessAppendedItemsCalled());
    TRACE_EVENT_OBJECT_SNAPSHOT_WITH_ID(
        TRACE_DISABLED_BY_DEFAULT("cc.debug.display_items") "," TRACE_DISABLED_BY_DEFAULT("cc.debug.picture") "," TRACE_DISABLED_BY_DEFAULT("devtools.timeline.picture"),
        "cc::DisplayItemList", this,
        TracedDisplayItemList::AsTraceableDisplayItemList(this,
            DisplayItemsTracingEnabled()));
}

void DisplayItemList::GenerateDiscardableImagesMetadata()
{
    DCHECK(ProcessAppendedItemsCalled());
    // This should be only called once, and only after CreateAndCacheSkPicture.
    DCHECK(picture_);
    DCHECK(image_map_.empty());
    if (!picture_->willPlayBackBitmaps())
        return;

    // The cached picture is translated by -layer_rect_.origin during record,
    // so we need to offset that back in order to get right positioning for
    // images.
    DiscardableImageMap::ScopedMetadataGenerator generator(
        &image_map_, gfx::Size(layer_rect_.right(), layer_rect_.bottom()));
    generator.canvas()->translate(layer_rect_.x(), layer_rect_.y());
    generator.canvas()->drawPicture(picture_.get());
}

void DisplayItemList::GetDiscardableImagesInRect(
    const gfx::Rect& rect,
    std::vector<PositionImage>* images)
{
    image_map_.GetDiscardableImagesInRect(rect, images);
}

} // namespace cc

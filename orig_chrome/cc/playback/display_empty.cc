
#include "third_party/skia/include/core/SkPictureRecorder.h"

#include "cc/playback/clip_display_item.h"
#include "cc/playback/clip_path_display_item.h"
#include "cc/playback/compositing_display_item.h"
#include "cc/playback/display_item_list.h"
#include "cc/playback/display_item_list_settings.h"
#include "cc/playback/drawing_display_item.h"
#include "cc/playback/filter_display_item.h"
#include "cc/playback/float_clip_display_item.h"
#include "cc/playback/transform_display_item.h"

namespace cc {

DisplayItemListSettings::DisplayItemListSettings()
    : use_cached_picture(false)
{
    DebugBreak();
}

DisplayItemListSettings::DisplayItemListSettings(const proto::DisplayItemListSettings& proto)
    : use_cached_picture(false)
{
    DebugBreak();
}

DisplayItemListSettings::~DisplayItemListSettings()
{
}

void DisplayItemListSettings::ToProtobuf(
    proto::DisplayItemListSettings* proto) const
{
    DebugBreak();
}

//////////////////////////////////////////////////////////////////////////
DisplayItemList::DisplayItemList(gfx::Rect layer_rect,
    const DisplayItemListSettings& settings,
    bool retain_individual_display_items)
    : settings_(settings)
    , retain_individual_display_items_(retain_individual_display_items)
    , layer_rect_(layer_rect)
    , is_suitable_for_gpu_rasterization_(true)
    , approximate_op_count_(0)
    , picture_memory_usage_(0)
    , external_memory_usage_(0)
{
    DebugBreak();
}

DisplayItemList::~DisplayItemList()
{
}

scoped_refptr<DisplayItemList> DisplayItemList::Create(
    const gfx::Rect& layer_rect,
    const DisplayItemListSettings& settings)
{
    DebugBreak();
    return nullptr;
}

void DisplayItemList::Finalize()
{
    DebugBreak();
}

void DisplayItemList::RasterIntoCanvas(const DisplayItem& item)
{
    DebugBreak();
}

bool DisplayItemList::RetainsIndividualDisplayItems() const
{
    return retain_individual_display_items_;
}

void DisplayItemList::ProcessAppendedItemsOnTheFly()
{
    DebugBreak();
}

//////////////////////////////////////////////////////////////////////////

ClipDisplayItem::ClipDisplayItem()
{
    DebugBreak();
}

ClipDisplayItem::~ClipDisplayItem()
{
}

void ClipDisplayItem::SetNew(gfx::Rect clip_rect,
    const std::vector<SkRRect>& rounded_clip_rects)
{
    DebugBreak();
}

//////////////////////////////////////////////////////////////////////////

EndClipDisplayItem::EndClipDisplayItem()
{
    DebugBreak();
}

EndClipDisplayItem::~EndClipDisplayItem()
{
}

//////////////////////////////////////////////////////////////////////////

ClipPathDisplayItem::ClipPathDisplayItem()
{
    DebugBreak();
}

ClipPathDisplayItem::~ClipPathDisplayItem()
{
}

void ClipPathDisplayItem::SetNew(const SkPath& path, SkRegion::Op clip_op, bool antialias)
{
    DebugBreak();
}

EndClipPathDisplayItem::EndClipPathDisplayItem()
{
    DebugBreak();
}

EndClipPathDisplayItem::~EndClipPathDisplayItem()
{
}

//////////////////////////////////////////////////////////////////////////

CompositingDisplayItem::CompositingDisplayItem()
{
    DebugBreak();
}

CompositingDisplayItem::~CompositingDisplayItem()
{
}

void CompositingDisplayItem::SetNew(uint8_t alpha,
    SkXfermode::Mode xfermode,
    SkRect* bounds,
    skia::RefPtr<SkColorFilter> color_filter)
{
    DebugBreak();
}

//////////////////////////////////////////////////////////////////////////

EndCompositingDisplayItem::EndCompositingDisplayItem()
{
    DebugBreak();
}

EndCompositingDisplayItem::~EndCompositingDisplayItem()
{
}

//////////////////////////////////////////////////////////////////////////

DrawingDisplayItem::DrawingDisplayItem()
{
    DebugBreak();
}

DrawingDisplayItem::~DrawingDisplayItem()
{
}

void DrawingDisplayItem::SetNew(skia::RefPtr<SkPicture> picture)
{
    DebugBreak();
}

//////////////////////////////////////////////////////////////////////////

FilterDisplayItem::FilterDisplayItem()
{
    DebugBreak();
}

FilterDisplayItem::~FilterDisplayItem()
{
}

void FilterDisplayItem::SetNew(const FilterOperations& filters, const gfx::RectF& bounds)
{
    DebugBreak();
}

//////////////////////////////////////////////////////////////////////////
// FilterDisplayItem::FilterDisplayItem() {
//   DebugBreak();
// }
//
// FilterDisplayItem::~FilterDisplayItem() {
// }
//
// void FilterDisplayItem::SetNew(const FilterOperations& filters,
//   const gfx::RectF& bounds) {
//   DebugBreak();
// }

////

FloatClipDisplayItem::FloatClipDisplayItem()
{
    DebugBreak();
}

FloatClipDisplayItem::~FloatClipDisplayItem()
{
}

void FloatClipDisplayItem::SetNew(const gfx::RectF& clip_rect)
{
    DebugBreak();
}

//////////////////////////////////////////////////////////////////////////
EndFloatClipDisplayItem::EndFloatClipDisplayItem()
{
    DebugBreak();
}

EndFloatClipDisplayItem::~EndFloatClipDisplayItem()
{
}

//////////////////////////////////////////////////////////////////////////

TransformDisplayItem::TransformDisplayItem()
    : transform_(gfx::Transform::kSkipInitialization)
{
    DebugBreak();
}

TransformDisplayItem::~TransformDisplayItem()
{
}

void TransformDisplayItem::SetNew(const gfx::Transform& transform)
{
    DebugBreak();
}

//////////////////////////////////////////////////////////////////////////

EndFilterDisplayItem::EndFilterDisplayItem()
{
    DebugBreak();
}

EndFilterDisplayItem::~EndFilterDisplayItem()
{
}

//////////////////////////////////////////////////////////////////////////

EndTransformDisplayItem::EndTransformDisplayItem()
{
    DebugBreak();
}

EndTransformDisplayItem::~EndTransformDisplayItem()
{
}

void EndTransformDisplayItem::ToProtobuf(proto::DisplayItem* proto) const
{
    DebugBreak();
}

void EndTransformDisplayItem::FromProtobuf(const proto::DisplayItem& proto)
{
    DebugBreak();
}

void EndTransformDisplayItem::Raster(
    SkCanvas* canvas,
    const gfx::Rect& canvas_target_playback_rect,
    SkPicture::AbortCallback* callback) const
{
    DebugBreak();
}

void EndTransformDisplayItem::AsValueInto(
    base::trace_event::TracedValue* array) const
{
    DebugBreak();
}

} // namespace cc
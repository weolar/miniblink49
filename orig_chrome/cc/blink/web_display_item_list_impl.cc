// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_display_item_list_impl.h"

#include <vector>

#include "cc/blink/web_filter_operations_impl.h"
#include "cc/playback/clip_display_item.h"
#include "cc/playback/clip_path_display_item.h"
#include "cc/playback/compositing_display_item.h"
#include "cc/playback/display_item_list_settings.h"
#include "cc/playback/drawing_display_item.h"
#include "cc/playback/filter_display_item.h"
#include "cc/playback/float_clip_display_item.h"
#include "cc/playback/transform_display_item.h"
#include "skia/ext/refptr.h"
#include "third_party/WebKit/public/platform/WebFloatRect.h"
#include "third_party/WebKit/public/platform/WebRect.h"
#include "third_party/skia/include/core/SkColorFilter.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/utils/SkMatrix44.h"
#include "ui/gfx/geometry/safe_integer_conversions.h"
#include "ui/gfx/transform.h"

namespace cc_blink {

namespace {

    scoped_refptr<cc::DisplayItemList> CreateUncachedDisplayItemListForBlink()
    {
        cc::DisplayItemListSettings settings;
        settings.use_cached_picture = false;
        gfx::Rect layer_rect;
        return cc::DisplayItemList::Create(layer_rect, settings);
    }

} // namespace

WebDisplayItemListImpl::WebDisplayItemListImpl()
    : display_item_list_(CreateUncachedDisplayItemListForBlink())
{
}

WebDisplayItemListImpl::WebDisplayItemListImpl(
    cc::DisplayItemList* display_list)
    : display_item_list_(display_list)
{
}

void WebDisplayItemListImpl::appendDrawingItem(const SkPicture* picture)
{
    if (display_item_list_->RetainsIndividualDisplayItems()) {
        auto* item = display_item_list_->CreateAndAppendItem<cc::DrawingDisplayItem>();
        item->SetNew(skia::SharePtr(const_cast<SkPicture*>(picture)));
    } else {
        cc::DrawingDisplayItem item;
        item.SetNew(skia::SharePtr(const_cast<SkPicture*>(picture)));
        display_item_list_->RasterIntoCanvas(item);
    }
}

void WebDisplayItemListImpl::appendClipItem(
    const blink::WebRect& clip_rect,
    const blink::WebVector<SkRRect>& rounded_clip_rects)
{
    std::vector<SkRRect> rounded_rects;
    for (size_t i = 0; i < rounded_clip_rects.size(); ++i) {
        rounded_rects.push_back(rounded_clip_rects[i]);
    }
    if (display_item_list_->RetainsIndividualDisplayItems()) {
        auto* item = display_item_list_->CreateAndAppendItem<cc::ClipDisplayItem>();
        item->SetNew(clip_rect, rounded_rects);
    } else {
        cc::ClipDisplayItem item;
        item.SetNew(clip_rect, rounded_rects);
        display_item_list_->RasterIntoCanvas(item);
    }
}

void WebDisplayItemListImpl::appendEndClipItem()
{
    if (display_item_list_->RetainsIndividualDisplayItems()) {
        display_item_list_->CreateAndAppendItem<cc::EndClipDisplayItem>();
    } else {
        display_item_list_->RasterIntoCanvas(cc::EndClipDisplayItem());
    }
}

void WebDisplayItemListImpl::appendClipPathItem(const SkPath& clip_path,
    SkRegion::Op clip_op,
    bool antialias)
{
    if (display_item_list_->RetainsIndividualDisplayItems()) {
        auto* item = display_item_list_->CreateAndAppendItem<cc::ClipPathDisplayItem>();
        item->SetNew(clip_path, clip_op, antialias);
    } else {
        cc::ClipPathDisplayItem item;
        item.SetNew(clip_path, clip_op, antialias);
        display_item_list_->RasterIntoCanvas(item);
    }
}

void WebDisplayItemListImpl::appendEndClipPathItem()
{
    if (display_item_list_->RetainsIndividualDisplayItems()) {
        display_item_list_->CreateAndAppendItem<cc::EndClipPathDisplayItem>();
    } else {
        display_item_list_->RasterIntoCanvas(cc::EndClipPathDisplayItem());
    }
}

void WebDisplayItemListImpl::appendFloatClipItem(
    const blink::WebFloatRect& clip_rect)
{
    if (display_item_list_->RetainsIndividualDisplayItems()) {
        auto* item = display_item_list_->CreateAndAppendItem<cc::FloatClipDisplayItem>();
        item->SetNew(clip_rect);
    } else {
        cc::FloatClipDisplayItem item;
        item.SetNew(clip_rect);
        display_item_list_->RasterIntoCanvas(item);
    }
}

void WebDisplayItemListImpl::appendEndFloatClipItem()
{
    if (display_item_list_->RetainsIndividualDisplayItems()) {
        display_item_list_->CreateAndAppendItem<cc::EndFloatClipDisplayItem>();
    } else {
        display_item_list_->RasterIntoCanvas(cc::EndFloatClipDisplayItem());
    }
}

void WebDisplayItemListImpl::appendTransformItem(const SkMatrix44& matrix)
{
    gfx::Transform transform;
    transform.matrix() = matrix;

    if (display_item_list_->RetainsIndividualDisplayItems()) {
        auto* item = display_item_list_->CreateAndAppendItem<cc::TransformDisplayItem>();
        item->SetNew(transform);
    } else {
        cc::TransformDisplayItem item;
        item.SetNew(transform);
        display_item_list_->RasterIntoCanvas(item);
    }
}

void WebDisplayItemListImpl::appendEndTransformItem()
{
    if (display_item_list_->RetainsIndividualDisplayItems()) {
        display_item_list_->CreateAndAppendItem<cc::EndTransformDisplayItem>();
    } else {
        display_item_list_->RasterIntoCanvas(cc::EndTransformDisplayItem());
    }
}

void WebDisplayItemListImpl::appendCompositingItem(
    float opacity,
    SkXfermode::Mode xfermode,
    SkRect* bounds,
    SkColorFilter* color_filter)
{
    DCHECK_GE(opacity, 0.f);
    DCHECK_LE(opacity, 1.f);
    // TODO(ajuma): This should really be rounding instead of flooring the alpha
    // value, but that breaks slimming paint reftests.

    if (display_item_list_->RetainsIndividualDisplayItems()) {
        auto* item = display_item_list_->CreateAndAppendItem<cc::CompositingDisplayItem>();
        item->SetNew(static_cast<uint8_t>(gfx::ToFlooredInt(255 * opacity)),
            xfermode, bounds, skia::SharePtr(color_filter));
    } else {
        cc::CompositingDisplayItem item;
        item.SetNew(static_cast<uint8_t>(gfx::ToFlooredInt(255 * opacity)),
            xfermode, bounds, skia::SharePtr(color_filter));
        display_item_list_->RasterIntoCanvas(item);
    }
}

void WebDisplayItemListImpl::appendEndCompositingItem()
{
    if (display_item_list_->RetainsIndividualDisplayItems()) {
        display_item_list_->CreateAndAppendItem<cc::EndCompositingDisplayItem>();
    } else {
        display_item_list_->RasterIntoCanvas(cc::EndCompositingDisplayItem());
    }
}

void WebDisplayItemListImpl::appendFilterItem(
    const blink::WebFilterOperations& filters,
    const blink::WebFloatRect& bounds)
{
    const WebFilterOperationsImpl& filters_impl = static_cast<const WebFilterOperationsImpl&>(filters);

    if (display_item_list_->RetainsIndividualDisplayItems()) {
        auto* item = display_item_list_->CreateAndAppendItem<cc::FilterDisplayItem>();
        item->SetNew(filters_impl.AsFilterOperations(), bounds);
    } else {
        cc::FilterDisplayItem item;
        item.SetNew(filters_impl.AsFilterOperations(), bounds);
        display_item_list_->RasterIntoCanvas(item);
    }
}

void WebDisplayItemListImpl::appendEndFilterItem()
{
    if (display_item_list_->RetainsIndividualDisplayItems()) {
        display_item_list_->CreateAndAppendItem<cc::EndFilterDisplayItem>();
    } else {
        display_item_list_->RasterIntoCanvas(cc::EndFilterDisplayItem());
    }
}

void WebDisplayItemListImpl::appendScrollItem(
    const blink::WebSize& scrollOffset,
    ScrollContainerId)
{
    SkMatrix44 matrix;
    matrix.setTranslate(-scrollOffset.width, -scrollOffset.height, 0);
    appendTransformItem(matrix);
}

void WebDisplayItemListImpl::appendEndScrollItem()
{
    appendEndTransformItem();
}

WebDisplayItemListImpl::~WebDisplayItemListImpl()
{
}

} // namespace cc_blink

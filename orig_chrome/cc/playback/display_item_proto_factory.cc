// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/display_item_proto_factory.h"

#include "cc/playback/clip_display_item.h"
#include "cc/playback/clip_path_display_item.h"
#include "cc/playback/compositing_display_item.h"
#include "cc/playback/drawing_display_item.h"
#include "cc/playback/filter_display_item.h"
#include "cc/playback/float_clip_display_item.h"
#include "cc/playback/transform_display_item.h"
#include "cc/proto/display_item.pb.h"

namespace cc {

// static
DisplayItem* DisplayItemProtoFactory::AllocateAndConstruct(
    scoped_refptr<DisplayItemList> list,
    const proto::DisplayItem& proto)
{
    switch (proto.type()) {
    case proto::DisplayItem::Type_Clip:
        return list->CreateAndAppendItem<ClipDisplayItem>();
    case proto::DisplayItem::Type_EndClip:
        return list->CreateAndAppendItem<EndClipDisplayItem>();
    case proto::DisplayItem::Type_ClipPath:
        return list->CreateAndAppendItem<ClipPathDisplayItem>();
    case proto::DisplayItem::Type_EndClipPath:
        return list->CreateAndAppendItem<EndClipPathDisplayItem>();
    case proto::DisplayItem::Type_Compositing:
        return list->CreateAndAppendItem<CompositingDisplayItem>();
    case proto::DisplayItem::Type_EndCompositing:
        return list->CreateAndAppendItem<EndCompositingDisplayItem>();
    case proto::DisplayItem::Type_Drawing:
        return list->CreateAndAppendItem<DrawingDisplayItem>();
    case proto::DisplayItem::Type_Filter:
        return list->CreateAndAppendItem<FilterDisplayItem>();
    case proto::DisplayItem::Type_EndFilter:
        return list->CreateAndAppendItem<EndFilterDisplayItem>();
    case proto::DisplayItem::Type_FloatClip:
        return list->CreateAndAppendItem<FloatClipDisplayItem>();
    case proto::DisplayItem::Type_EndFloatClip:
        return list->CreateAndAppendItem<EndFloatClipDisplayItem>();
    case proto::DisplayItem::Type_Transform:
        return list->CreateAndAppendItem<TransformDisplayItem>();
    case proto::DisplayItem::Type_EndTransform:
        return list->CreateAndAppendItem<EndTransformDisplayItem>();
    }

    NOTREACHED();
    return nullptr;
}

} // namespace cc

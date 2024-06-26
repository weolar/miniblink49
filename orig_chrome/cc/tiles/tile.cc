// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/tiles/tile.h"

#include <algorithm>

#include "base/numerics/safe_conversions.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/base/math_util.h"
#include "cc/debug/traced_value.h"
#include "cc/tiles/tile_manager.h"

namespace cc {

Tile::Tile(TileManager* tile_manager,
    const CreateInfo& info,
    int layer_id,
    int source_frame_number,
    int flags)
    : tile_manager_(tile_manager)
    , content_rect_(info.content_rect)
    , enclosing_layer_rect_(info.enclosing_layer_rect)
    , contents_scale_(info.contents_scale)
    , layer_id_(layer_id)
    , source_frame_number_(source_frame_number)
    , flags_(flags)
    , tiling_i_index_(info.tiling_i_index)
    , tiling_j_index_(info.tiling_j_index)
    , required_for_activation_(false)
    , required_for_draw_(false)
    , id_(tile_manager->GetUniqueTileId())
    , invalidated_id_(0)
    , scheduled_priority_(0)
{
}

Tile::~Tile()
{
    TRACE_EVENT_OBJECT_DELETED_WITH_ID(
        TRACE_DISABLED_BY_DEFAULT("cc.debug"),
        "cc::Tile", this);
}

void Tile::AsValueInto(base::trace_event::TracedValue* value) const
{
    TracedValue::MakeDictIntoImplicitSnapshotWithCategory(
        TRACE_DISABLED_BY_DEFAULT("cc.debug"), value, "cc::Tile", this);
    value->SetDouble("contents_scale", contents_scale_);

    MathUtil::AddToTracedValue("content_rect", content_rect_, value);

    value->SetInteger("layer_id", layer_id_);

    value->BeginDictionary("draw_info");
    draw_info_.AsValueInto(value);
    value->EndDictionary();

    value->SetBoolean("has_resource", draw_info().has_resource());
    value->SetBoolean("is_using_gpu_memory",
        draw_info().has_resource() || HasRasterTask());
    value->SetInteger("scheduled_priority", scheduled_priority_);
    value->SetBoolean("use_picture_analysis", use_picture_analysis());
    value->SetInteger("gpu_memory_usage",
        base::saturated_cast<int>(GPUMemoryUsageInBytes()));
}

size_t Tile::GPUMemoryUsageInBytes() const
{
    if (draw_info_.resource_) {
        // We can use UncheckedSizeInBytes, since the tile size is determined by the
        // compositor.
        return ResourceUtil::UncheckedSizeInBytes<size_t>(
            draw_info_.resource_->size(), draw_info_.resource_->format());
    }
    return 0;
}

void Tile::Deleter::operator()(Tile* tile) const
{
    tile->tile_manager_->Release(tile);
}

} // namespace cc

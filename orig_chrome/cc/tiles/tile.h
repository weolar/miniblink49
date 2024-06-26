// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TILES_TILE_H_
#define CC_TILES_TILE_H_

#include "base/memory/ref_counted.h"
#include "cc/tiles/tile_draw_info.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

class PrioritizedTile;
class TileManager;
struct TilePriority;

class CC_EXPORT Tile {
public:
    class CC_EXPORT Deleter {
    public:
        void operator()(Tile* tile) const;
    };

    class CC_EXPORT CreateInfo {
    public:
        int tiling_i_index;
        int tiling_j_index;
        gfx::Rect enclosing_layer_rect;
        gfx::Rect content_rect;
        float contents_scale;

        CreateInfo(int tiling_i_index,
            int tiling_j_index,
            const gfx::Rect& enclosing_layer_rect,
            const gfx::Rect& content_rect,
            float contents_scale)
            : tiling_i_index(tiling_i_index)
            , tiling_j_index(tiling_j_index)
            , enclosing_layer_rect(enclosing_layer_rect)
            , content_rect(content_rect)
            , contents_scale(contents_scale)
        {
        }
    };

    enum TileRasterFlags { USE_PICTURE_ANALYSIS = 1 << 0,
        IS_OPAQUE = 1 << 1 };

    typedef uint64 Id;

    Id id() const
    {
        return id_;
    }

    // TODO(vmpstr): Move this to the iterators.
    bool required_for_activation() const { return required_for_activation_; }
    void set_required_for_activation(bool is_required)
    {
        required_for_activation_ = is_required;
    }
    bool required_for_draw() const { return required_for_draw_; }
    void set_required_for_draw(bool is_required)
    {
        required_for_draw_ = is_required;
    }

    bool use_picture_analysis() const
    {
        return !!(flags_ & USE_PICTURE_ANALYSIS);
    }

    bool is_opaque() const { return !!(flags_ & IS_OPAQUE); }

    void AsValueInto(base::trace_event::TracedValue* value) const;

    const TileDrawInfo& draw_info() const { return draw_info_; }
    TileDrawInfo& draw_info() { return draw_info_; }

    float contents_scale() const { return contents_scale_; }
    const gfx::Rect& content_rect() const { return content_rect_; }
    const gfx::Rect& enclosing_layer_rect() const
    {
        return enclosing_layer_rect_;
    }

    int layer_id() const { return layer_id_; }

    int source_frame_number() const { return source_frame_number_; }

    size_t GPUMemoryUsageInBytes() const;

    const gfx::Size& desired_texture_size() const { return content_rect_.size(); }

    int tiling_i_index() const { return tiling_i_index_; }
    int tiling_j_index() const { return tiling_j_index_; }

    void SetInvalidated(const gfx::Rect& invalid_content_rect,
        Id previous_tile_id)
    {
        invalidated_content_rect_ = invalid_content_rect;
        invalidated_id_ = previous_tile_id;
    }

    Id invalidated_id() const { return invalidated_id_; }
    const gfx::Rect& invalidated_content_rect() const
    {
        return invalidated_content_rect_;
    }

private:
    friend class TileManager;
    friend class FakeTileManager;
    friend class FakePictureLayerImpl;

    // Methods called by by tile manager.
    Tile(TileManager* tile_manager,
        const CreateInfo& info,
        int layer_id,
        int source_frame_number,
        int flags);
    ~Tile();

    bool HasRasterTask() const { return !!raster_task_.get(); }

    TileManager* const tile_manager_;
    const gfx::Rect content_rect_;
    const gfx::Rect enclosing_layer_rect_;
    const float contents_scale_;

    TileDrawInfo draw_info_;

    const int layer_id_;
    const int source_frame_number_;
    const int flags_;
    const int tiling_i_index_;
    const int tiling_j_index_;
    bool required_for_activation_ : 1;
    bool required_for_draw_ : 1;

    Id id_;

    // The rect bounding the changes in this Tile vs the previous tile it
    // replaced.
    gfx::Rect invalidated_content_rect_;
    // The |id_| of the Tile that was invalidated and replaced by this tile.
    Id invalidated_id_;

    unsigned scheduled_priority_;

    scoped_refptr<RasterTask> raster_task_;

    DISALLOW_COPY_AND_ASSIGN(Tile);
};

using ScopedTilePtr = scoped_ptr<Tile, Tile::Deleter>;

} // namespace cc

#endif // CC_TILES_TILE_H_

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TILES_TILE_DRAW_INFO_H_
#define CC_TILES_TILE_DRAW_INFO_H_

#include "base/memory/scoped_ptr.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/raster/tile_task_runner.h"
#include "cc/resources/platform_color.h"
#include "cc/resources/resource_provider.h"
#include "cc/resources/scoped_resource.h"

namespace cc {

// This class holds all the state relevant to drawing a tile.
class CC_EXPORT TileDrawInfo {
public:
    enum Mode { RESOURCE_MODE,
        SOLID_COLOR_MODE,
        OOM_MODE };

    TileDrawInfo();
    ~TileDrawInfo();

    Mode mode() const { return mode_; }

    bool IsReadyToDraw() const
    {
        switch (mode_) {
        case RESOURCE_MODE:
            return !!resource_;
        case SOLID_COLOR_MODE:
        case OOM_MODE:
            return true;
        }
        NOTREACHED();
        return false;
    }
    bool NeedsRaster() const
    {
        switch (mode_) {
        case RESOURCE_MODE:
            return !resource_;
        case SOLID_COLOR_MODE:
            return false;
        case OOM_MODE:
            return true;
        }
        NOTREACHED();
        return false;
    }

    ResourceId resource_id() const
    {
        DCHECK(mode_ == RESOURCE_MODE);
        DCHECK(resource_);
        return resource_->id();
    }

    gfx::Size resource_size() const
    {
        DCHECK(mode_ == RESOURCE_MODE);
        DCHECK(resource_);
        return resource_->size();
    }

    SkColor solid_color() const
    {
        DCHECK(mode_ == SOLID_COLOR_MODE);
        return solid_color_;
    }

    bool contents_swizzled() const { return contents_swizzled_; }

    bool requires_resource() const
    {
        return mode_ == RESOURCE_MODE || mode_ == OOM_MODE;
    }

    inline bool has_resource() const { return !!resource_; }

    void SetSolidColorForTesting(SkColor color) { set_solid_color(color); }

    void AsValueInto(base::trace_event::TracedValue* state) const;

    void set_was_ever_ready_to_draw() { was_ever_ready_to_draw_ = true; }
    void set_was_ever_used_to_draw() { was_ever_used_to_draw_ = true; }

private:
    friend class Tile;
    friend class TileManager;

    void set_use_resource() { mode_ = RESOURCE_MODE; }

    void set_solid_color(const SkColor& color)
    {
        mode_ = SOLID_COLOR_MODE;
        solid_color_ = color;
    }

    void set_oom() { mode_ = OOM_MODE; }

    Mode mode_;
    SkColor solid_color_;
    Resource* resource_;
    bool contents_swizzled_;

    // Used for gathering UMA stats.
    bool was_ever_ready_to_draw_;
    bool was_ever_used_to_draw_;
};

} // namespace cc

#endif // CC_TILES_TILE_DRAW_INFO_H_

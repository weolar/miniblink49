// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_DAMAGE_TRACKER_H_
#define CC_TREES_DAMAGE_TRACKER_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/layers/layer_lists.h"
#include "ui/gfx/geometry/rect.h"
#include <vector>

class SkImageFilter;

namespace gfx {
class Rect;
}

namespace cc {

class FilterOperations;
class LayerImpl;
class RenderSurfaceImpl;

// Computes the region where pixels have actually changed on a
// RenderSurfaceImpl. This region is used to scissor what is actually drawn to
// the screen to save GPU computation and bandwidth.
class CC_EXPORT DamageTracker {
public:
    static scoped_ptr<DamageTracker> Create();
    ~DamageTracker();

    void DidDrawDamagedArea() { current_damage_rect_ = gfx::Rect(); }
    void AddDamageNextUpdate(const gfx::Rect& dmg)
    {
        current_damage_rect_.Union(dmg);
    }
    void UpdateDamageTrackingState(
        const LayerImplList& layer_list,
        int target_surface_layer_id,
        bool target_surface_property_changed_only_from_descendant,
        const gfx::Rect& target_surface_content_rect,
        LayerImpl* target_surface_mask_layer,
        const FilterOperations& filters);

    gfx::Rect current_damage_rect() { return current_damage_rect_; }

private:
    DamageTracker();

    gfx::Rect TrackDamageFromActiveLayers(const LayerImplList& layer_list,
        int target_surface_layer_id);
    gfx::Rect TrackDamageFromSurfaceMask(LayerImpl* target_surface_mask_layer);
    gfx::Rect TrackDamageFromLeftoverRects();

    void PrepareRectHistoryForUpdate();

    // These helper functions are used only in TrackDamageFromActiveLayers().
    void ExtendDamageForLayer(LayerImpl* layer, gfx::Rect* target_damage_rect);
    void ExtendDamageForRenderSurface(LayerImpl* layer,
        gfx::Rect* target_damage_rect);

    struct RectMapData {
        RectMapData()
            : layer_id_(0)
            , mailboxId_(0)
        {
        }
        explicit RectMapData(int layer_id)
            : layer_id_(layer_id)
            , mailboxId_(0)
        {
        }
        void Update(const gfx::Rect& rect, unsigned int mailboxId)
        {
            mailboxId_ = mailboxId;
            rect_ = rect;
        }

        bool operator<(const RectMapData& other) const
        {
            return layer_id_ < other.layer_id_;
        }

        int layer_id_;
        unsigned int mailboxId_;
        gfx::Rect rect_;
    };
    typedef std::vector<RectMapData> SortedRectMap;

    RectMapData& RectDataForLayer(int layer_id, bool* layer_is_new);

    SortedRectMap rect_history_;

    unsigned int mailboxId_;
    gfx::Rect current_damage_rect_;

    DISALLOW_COPY_AND_ASSIGN(DamageTracker);
};

} // namespace cc

#endif // CC_TREES_DAMAGE_TRACKER_H_

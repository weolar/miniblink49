// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_PIXEL_REF_MAP_H_
#define CC_PLAYBACK_PIXEL_REF_MAP_H_

#include <utility>
#include <vector>

#include "base/containers/hash_tables.h"
#include "base/lazy_instance.h"
#include "base/memory/ref_counted.h"
#include "cc/base/cc_export.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

class SkPixelRef;

namespace cc {

class Picture;
class DisplayItemList;

typedef std::pair<int, int> PixelRefMapKey;
typedef std::vector<SkPixelRef*> PixelRefs;
typedef base::hash_map<PixelRefMapKey, PixelRefs> PixelRefHashmap;

// This class is used and owned by cc Picture class. It is used to gather pixel
// refs which would happen after record. It takes in |cell_size| to decide how
// big each grid cell should be.
class CC_EXPORT PixelRefMap {
public:
    explicit PixelRefMap(const gfx::Size& cell_size);
    ~PixelRefMap();
    void GatherPixelRefsFromPicture(SkPicture* picture);

    bool empty() const { return data_hash_map_.empty(); }

    // This iterator imprecisely returns the set of pixel refs that are needed to
    // raster this layer rect from this picture.  Internally, pixel refs are
    // clumped into tile grid buckets, so there may be false positives.
    class CC_EXPORT Iterator {
    public:
        // Default iterator constructor that is used as place holder for invalid
        // Iterator.
        Iterator();
        Iterator(const gfx::Rect& layer_rect, const Picture* picture);
        Iterator(const gfx::Rect& layer_rect, const DisplayItemList* picture);
        ~Iterator();

        SkPixelRef* operator->() const
        {
            DCHECK_LT(current_index_, current_pixel_refs_->size());
            return (*current_pixel_refs_)[current_index_];
        }

        SkPixelRef* operator*() const
        {
            DCHECK_LT(current_index_, current_pixel_refs_->size());
            return (*current_pixel_refs_)[current_index_];
        }

        Iterator& operator++();
        operator bool() const
        {
            return current_index_ < current_pixel_refs_->size();
        }

    private:
        void PointToFirstPixelRef(const gfx::Rect& query_rect);

        static base::LazyInstance<PixelRefs> empty_pixel_refs_;
        const PixelRefMap* target_pixel_ref_map_;
        const PixelRefs* current_pixel_refs_;
        unsigned current_index_;

        gfx::Rect map_layer_rect_;

        gfx::Point min_point_;
        gfx::Point max_point_;
        int current_x_;
        int current_y_;
    };

private:
    gfx::Point min_pixel_cell_;
    gfx::Point max_pixel_cell_;
    gfx::Size cell_size_;

    PixelRefHashmap data_hash_map_;
};

} // namespace cc

#endif // CC_PLAYBACK_PIXEL_REF_MAP_H_

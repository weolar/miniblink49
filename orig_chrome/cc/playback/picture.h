// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_PICTURE_H_
#define CC_PLAYBACK_PICTURE_H_

#include "base/basictypes.h"
#include "base/containers/hash_tables.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/trace_event/trace_event.h"
#include "cc/base/cc_export.h"
#include "cc/base/region.h"
#include "cc/playback/pixel_ref_map.h"
#include "cc/playback/recording_source.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "ui/gfx/geometry/rect.h"

class SkPixelRef;

namespace base {
class Value;
}

namespace skia {
class AnalysisCanvas;
}

namespace cc {

class ContentLayerClient;

class CC_EXPORT Picture
    : public base::RefCountedThreadSafe<Picture> {
public:
    static scoped_refptr<Picture> Create(
        const gfx::Rect& layer_rect,
        ContentLayerClient* client,
        const gfx::Size& tile_grid_size,
        bool gather_pixels_refs,
        RecordingSource::RecordingMode recording_mode);
    static scoped_refptr<Picture> CreateFromValue(const base::Value* value);
    static scoped_refptr<Picture> CreateFromSkpValue(const base::Value* value);

    gfx::Rect LayerRect() const { return layer_rect_; }

    // Has Record() been called yet?
    bool HasRecording() const { return picture_.get() != NULL; }

    bool IsSuitableForGpuRasterization(const char** reason) const;
    int ApproximateOpCount() const;
    size_t ApproximateMemoryUsage() const;

    bool HasText() const;

    // Apply this scale and raster the negated region into the canvas.
    // |negated_content_region| specifies the region to be clipped out of the
    // raster operation, i.e., the parts of the canvas which will not get drawn
    // to.
    int Raster(SkCanvas* canvas,
        SkPicture::AbortCallback* callback,
        const Region& negated_content_region,
        float contents_scale) const;

    // Draw the picture directly into the given canvas, without applying any
    // clip/scale/layer transformations.
    void Replay(SkCanvas* canvas, SkPicture::AbortCallback* callback = NULL);

    scoped_ptr<base::Value> AsValue() const;

    void EmitTraceSnapshot() const;
    void EmitTraceSnapshotAlias(Picture* original) const;

    bool WillPlayBackBitmaps() const { return picture_->willPlayBackBitmaps(); }

    PixelRefMap::Iterator GetPixelRefMapIterator(
        const gfx::Rect& layer_rect) const;

private:
    Picture(const gfx::Rect& layer_rect, const gfx::Size& tile_grid_size);
    // This constructor assumes SkPicture is already ref'd and transfers
    // ownership to this picture.
    Picture(const skia::RefPtr<SkPicture>&,
        const gfx::Rect& layer_rect,
        const PixelRefMap& pixel_refs);
    // This constructor will call AdoptRef on the SkPicture.
    Picture(SkPicture*, const gfx::Rect& layer_rect);
    ~Picture();

    // Record a paint operation. To be able to safely use this SkPicture for
    // playback on a different thread this can only be called once.
    void Record(ContentLayerClient* client,
        RecordingSource::RecordingMode recording_mode);

    // Gather pixel refs from recording.
    void GatherPixelRefs();

    gfx::Rect layer_rect_;
    skia::RefPtr<SkPicture> picture_;

    PixelRefMap pixel_refs_;

    scoped_refptr<base::trace_event::ConvertableToTraceFormat>
    AsTraceableRasterData(float scale) const;
    scoped_refptr<base::trace_event::ConvertableToTraceFormat>
    AsTraceableRecordData() const;

    friend class base::RefCountedThreadSafe<Picture>;
    friend class PixelRefMap::Iterator;
    DISALLOW_COPY_AND_ASSIGN(Picture);
};

} // namespace cc

#endif // CC_PLAYBACK_PICTURE_H_

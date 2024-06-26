// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_FILTER_OPERATION_H_
#define CC_OUTPUT_FILTER_OPERATION_H_

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkImageFilter.h"
#include "third_party/skia/include/core/SkRegion.h"
#include "third_party/skia/include/core/SkScalar.h"
#include "ui/gfx/geometry/point.h"

namespace base {
namespace trace_event {
    class TracedValue;
}
class Value;
}

namespace cc {

class CC_EXPORT FilterOperation {
public:
    enum FilterType {
        GRAYSCALE,
        SEPIA,
        SATURATE,
        HUE_ROTATE,
        INVERT,
        BRIGHTNESS,
        CONTRAST,
        OPACITY,
        BLUR,
        DROP_SHADOW,
        COLOR_MATRIX,
        ZOOM,
        REFERENCE,
        SATURATING_BRIGHTNESS, // Not used in CSS/SVG.
        ALPHA_THRESHOLD, // Not used in CSS/SVG.
        FILTER_TYPE_LAST = ALPHA_THRESHOLD
    };

    FilterOperation(const FilterOperation& other);

    ~FilterOperation();

    FilterType type() const { return type_; }

    float amount() const
    {
        DCHECK_NE(type_, COLOR_MATRIX);
        DCHECK_NE(type_, REFERENCE);
        return amount_;
    }

    float outer_threshold() const
    {
        DCHECK_EQ(type_, ALPHA_THRESHOLD);
        return outer_threshold_;
    }

    gfx::Point drop_shadow_offset() const
    {
        DCHECK_EQ(type_, DROP_SHADOW);
        return drop_shadow_offset_;
    }

    SkColor drop_shadow_color() const
    {
        DCHECK_EQ(type_, DROP_SHADOW);
        return drop_shadow_color_;
    }

    skia::RefPtr<SkImageFilter> image_filter() const
    {
        DCHECK_EQ(type_, REFERENCE);
        return image_filter_;
    }

    const SkScalar* matrix() const
    {
        DCHECK_EQ(type_, COLOR_MATRIX);
        return matrix_;
    }

    int zoom_inset() const
    {
        DCHECK_EQ(type_, ZOOM);
        return zoom_inset_;
    }

    const SkRegion& region() const
    {
        DCHECK_EQ(type_, ALPHA_THRESHOLD);
        return region_;
    }

    static FilterOperation CreateGrayscaleFilter(float amount)
    {
        return FilterOperation(GRAYSCALE, amount);
    }

    static FilterOperation CreateSepiaFilter(float amount)
    {
        return FilterOperation(SEPIA, amount);
    }

    static FilterOperation CreateSaturateFilter(float amount)
    {
        return FilterOperation(SATURATE, amount);
    }

    static FilterOperation CreateHueRotateFilter(float amount)
    {
        return FilterOperation(HUE_ROTATE, amount);
    }

    static FilterOperation CreateInvertFilter(float amount)
    {
        return FilterOperation(INVERT, amount);
    }

    static FilterOperation CreateBrightnessFilter(float amount)
    {
        return FilterOperation(BRIGHTNESS, amount);
    }

    static FilterOperation CreateContrastFilter(float amount)
    {
        return FilterOperation(CONTRAST, amount);
    }

    static FilterOperation CreateOpacityFilter(float amount)
    {
        return FilterOperation(OPACITY, amount);
    }

    static FilterOperation CreateBlurFilter(float amount)
    {
        return FilterOperation(BLUR, amount);
    }

    static FilterOperation CreateDropShadowFilter(const gfx::Point& offset,
        float std_deviation,
        SkColor color)
    {
        return FilterOperation(DROP_SHADOW, offset, std_deviation, color);
    }

    static FilterOperation CreateColorMatrixFilter(SkScalar matrix[20])
    {
        return FilterOperation(COLOR_MATRIX, matrix);
    }

    static FilterOperation CreateZoomFilter(float amount, int inset)
    {
        return FilterOperation(ZOOM, amount, inset);
    }

    static FilterOperation CreateReferenceFilter(
        const skia::RefPtr<SkImageFilter>& image_filter)
    {
        return FilterOperation(REFERENCE, image_filter);
    }

    static FilterOperation CreateSaturatingBrightnessFilter(float amount)
    {
        return FilterOperation(SATURATING_BRIGHTNESS, amount);
    }

    static FilterOperation CreateAlphaThresholdFilter(const SkRegion& region,
        float inner_threshold,
        float outer_threshold)
    {
        return FilterOperation(ALPHA_THRESHOLD, region,
            inner_threshold, outer_threshold);
    }

    bool operator==(const FilterOperation& other) const;

    bool operator!=(const FilterOperation& other) const
    {
        return !(*this == other);
    }

    // Methods for restoring a FilterOperation.
    static FilterOperation CreateEmptyFilter()
    {
        return FilterOperation(GRAYSCALE, 0.f);
    }

    void set_type(FilterType type) { type_ = type; }

    void set_amount(float amount)
    {
        DCHECK_NE(type_, COLOR_MATRIX);
        DCHECK_NE(type_, REFERENCE);
        amount_ = amount;
    }

    void set_outer_threshold(float outer_threshold)
    {
        DCHECK_EQ(type_, ALPHA_THRESHOLD);
        outer_threshold_ = outer_threshold;
    }

    void set_drop_shadow_offset(const gfx::Point& offset)
    {
        DCHECK_EQ(type_, DROP_SHADOW);
        drop_shadow_offset_ = offset;
    }

    void set_drop_shadow_color(SkColor color)
    {
        DCHECK_EQ(type_, DROP_SHADOW);
        drop_shadow_color_ = color;
    }

    void set_image_filter(const skia::RefPtr<SkImageFilter>& image_filter)
    {
        DCHECK_EQ(type_, REFERENCE);
        image_filter_ = image_filter;
    }

    void set_matrix(const SkScalar matrix[20])
    {
        DCHECK_EQ(type_, COLOR_MATRIX);
        for (unsigned i = 0; i < 20; ++i)
            matrix_[i] = matrix[i];
    }

    void set_zoom_inset(int inset)
    {
        DCHECK_EQ(type_, ZOOM);
        zoom_inset_ = inset;
    }

    void set_region(const SkRegion& region)
    {
        DCHECK_EQ(type_, ALPHA_THRESHOLD);
        region_ = region;
    }

    // Given two filters of the same type, returns a filter operation created by
    // linearly interpolating a |progress| fraction from |from| to |to|. If either
    // |from| or |to| (but not both) is null, it is treated as a no-op filter of
    // the same type as the other given filter. If both |from| and |to| are null,
    // or if |from| and |to| are non-null but of different types, returns a
    // no-op filter.
    static FilterOperation Blend(const FilterOperation* from,
        const FilterOperation* to,
        double progress);

    void AsValueInto(base::trace_event::TracedValue* value) const;

private:
    FilterOperation(FilterType type, float amount);

    FilterOperation(FilterType type,
        const gfx::Point& offset,
        float stdDeviation,
        SkColor color);

    FilterOperation(FilterType, SkScalar matrix[20]);

    FilterOperation(FilterType type, float amount, int inset);

    FilterOperation(FilterType type,
        const skia::RefPtr<SkImageFilter>& image_filter);

    FilterOperation(FilterType type,
        const SkRegion& region,
        float inner_threshold,
        float outer_threshold);

    FilterType type_;
    float amount_;
    float outer_threshold_;
    gfx::Point drop_shadow_offset_;
    SkColor drop_shadow_color_;
    skia::RefPtr<SkImageFilter> image_filter_;
    SkScalar matrix_[20];
    int zoom_inset_;
    SkRegion region_;
};

} // namespace cc

#endif // CC_OUTPUT_FILTER_OPERATION_H_

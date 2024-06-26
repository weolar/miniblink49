// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>

#include "base/trace_event/trace_event_argument.h"
#include "base/values.h"
#include "cc/base/math_util.h"
#include "cc/output/filter_operation.h"
#include "ui/gfx/animation/tween.h"

namespace cc {

bool FilterOperation::operator==(const FilterOperation& other) const
{
    if (type_ != other.type_)
        return false;
    if (type_ == COLOR_MATRIX)
        return !memcmp(matrix_, other.matrix_, sizeof(matrix_));
    if (type_ == DROP_SHADOW) {
        return amount_ == other.amount_ && drop_shadow_offset_ == other.drop_shadow_offset_ && drop_shadow_color_ == other.drop_shadow_color_;
    }
    if (type_ == REFERENCE)
        return image_filter_.get() == other.image_filter_.get();
    if (type_ == ALPHA_THRESHOLD) {
        return region_ == other.region_ && amount_ == other.amount_ && outer_threshold_ == other.outer_threshold_;
    }
    return amount_ == other.amount_;
}

FilterOperation::FilterOperation(FilterType type, float amount)
    : type_(type)
    , amount_(amount)
    , outer_threshold_(0)
    , drop_shadow_offset_(0, 0)
    , drop_shadow_color_(0)
    , zoom_inset_(0)
{
    DCHECK_NE(type_, DROP_SHADOW);
    DCHECK_NE(type_, COLOR_MATRIX);
    DCHECK_NE(type_, REFERENCE);
    memset(matrix_, 0, sizeof(matrix_));
}

FilterOperation::FilterOperation(FilterType type,
    const gfx::Point& offset,
    float stdDeviation,
    SkColor color)
    : type_(type)
    , amount_(stdDeviation)
    , outer_threshold_(0)
    , drop_shadow_offset_(offset)
    , drop_shadow_color_(color)
    , zoom_inset_(0)
{
    DCHECK_EQ(type_, DROP_SHADOW);
    memset(matrix_, 0, sizeof(matrix_));
}

FilterOperation::FilterOperation(FilterType type, SkScalar matrix[20])
    : type_(type)
    , amount_(0)
    , outer_threshold_(0)
    , drop_shadow_offset_(0, 0)
    , drop_shadow_color_(0)
    , zoom_inset_(0)
{
    DCHECK_EQ(type_, COLOR_MATRIX);
    memcpy(matrix_, matrix, sizeof(matrix_));
}

FilterOperation::FilterOperation(FilterType type, float amount, int inset)
    : type_(type)
    , amount_(amount)
    , outer_threshold_(0)
    , drop_shadow_offset_(0, 0)
    , drop_shadow_color_(0)
    , zoom_inset_(inset)
{
    DCHECK_EQ(type_, ZOOM);
    memset(matrix_, 0, sizeof(matrix_));
}

FilterOperation::FilterOperation(
    FilterType type,
    const skia::RefPtr<SkImageFilter>& image_filter)
    : type_(type)
    , amount_(0)
    , outer_threshold_(0)
    , drop_shadow_offset_(0, 0)
    , drop_shadow_color_(0)
    , image_filter_(image_filter)
    , zoom_inset_(0)
{
    DCHECK_EQ(type_, REFERENCE);
    memset(matrix_, 0, sizeof(matrix_));
}

FilterOperation::FilterOperation(FilterType type,
    const SkRegion& region,
    float inner_threshold,
    float outer_threshold)
    : type_(type)
    , amount_(inner_threshold)
    , outer_threshold_(outer_threshold)
    , drop_shadow_offset_(0, 0)
    , drop_shadow_color_(0)
    , zoom_inset_(0)
    , region_(region)
{
    DCHECK_EQ(type_, ALPHA_THRESHOLD);
    memset(matrix_, 0, sizeof(matrix_));
}

FilterOperation::FilterOperation(const FilterOperation& other)
    : type_(other.type_)
    , amount_(other.amount_)
    , outer_threshold_(other.outer_threshold_)
    , drop_shadow_offset_(other.drop_shadow_offset_)
    , drop_shadow_color_(other.drop_shadow_color_)
    , image_filter_(other.image_filter_)
    , zoom_inset_(other.zoom_inset_)
    , region_(other.region_)
{
    memcpy(matrix_, other.matrix_, sizeof(matrix_));
}

FilterOperation::~FilterOperation()
{
}

static FilterOperation CreateNoOpFilter(FilterOperation::FilterType type)
{
    switch (type) {
    case FilterOperation::GRAYSCALE:
        return FilterOperation::CreateGrayscaleFilter(0.f);
    case FilterOperation::SEPIA:
        return FilterOperation::CreateSepiaFilter(0.f);
    case FilterOperation::SATURATE:
        return FilterOperation::CreateSaturateFilter(1.f);
    case FilterOperation::HUE_ROTATE:
        return FilterOperation::CreateHueRotateFilter(0.f);
    case FilterOperation::INVERT:
        return FilterOperation::CreateInvertFilter(0.f);
    case FilterOperation::BRIGHTNESS:
        return FilterOperation::CreateBrightnessFilter(1.f);
    case FilterOperation::CONTRAST:
        return FilterOperation::CreateContrastFilter(1.f);
    case FilterOperation::OPACITY:
        return FilterOperation::CreateOpacityFilter(1.f);
    case FilterOperation::BLUR:
        return FilterOperation::CreateBlurFilter(0.f);
    case FilterOperation::DROP_SHADOW:
        return FilterOperation::CreateDropShadowFilter(
            gfx::Point(0, 0), 0.f, SK_ColorTRANSPARENT);
    case FilterOperation::COLOR_MATRIX: {
        SkScalar matrix[20];
        memset(matrix, 0, 20 * sizeof(SkScalar));
        matrix[0] = matrix[6] = matrix[12] = matrix[18] = 1.f;
        return FilterOperation::CreateColorMatrixFilter(matrix);
    }
    case FilterOperation::ZOOM:
        return FilterOperation::CreateZoomFilter(1.f, 0);
    case FilterOperation::SATURATING_BRIGHTNESS:
        return FilterOperation::CreateSaturatingBrightnessFilter(0.f);
    case FilterOperation::REFERENCE:
        return FilterOperation::CreateReferenceFilter(
            skia::RefPtr<SkImageFilter>());
    case FilterOperation::ALPHA_THRESHOLD:
        return FilterOperation::CreateAlphaThresholdFilter(SkRegion(), 1.f, 0.f);
    }
    NOTREACHED();
    return FilterOperation::CreateEmptyFilter();
}

static float ClampAmountForFilterType(float amount,
    FilterOperation::FilterType type)
{
    switch (type) {
    case FilterOperation::GRAYSCALE:
    case FilterOperation::SEPIA:
    case FilterOperation::INVERT:
    case FilterOperation::OPACITY:
    case FilterOperation::ALPHA_THRESHOLD:
        return MathUtil::ClampToRange(amount, 0.f, 1.f);
    case FilterOperation::SATURATE:
    case FilterOperation::BRIGHTNESS:
    case FilterOperation::CONTRAST:
    case FilterOperation::BLUR:
    case FilterOperation::DROP_SHADOW:
        return std::max(amount, 0.f);
    case FilterOperation::ZOOM:
        return std::max(amount, 1.f);
    case FilterOperation::HUE_ROTATE:
    case FilterOperation::SATURATING_BRIGHTNESS:
        return amount;
    case FilterOperation::COLOR_MATRIX:
    case FilterOperation::REFERENCE:
        NOTREACHED();
        return amount;
    }
    NOTREACHED();
    return amount;
}

// static
FilterOperation FilterOperation::Blend(const FilterOperation* from,
    const FilterOperation* to,
    double progress)
{
    FilterOperation blended_filter = FilterOperation::CreateEmptyFilter();

    if (!from && !to)
        return blended_filter;

    const FilterOperation& from_op = from ? *from : CreateNoOpFilter(to->type());
    const FilterOperation& to_op = to ? *to : CreateNoOpFilter(from->type());

    if (from_op.type() != to_op.type())
        return blended_filter;

    DCHECK(to_op.type() != FilterOperation::COLOR_MATRIX);
    blended_filter.set_type(to_op.type());

    if (to_op.type() == FilterOperation::REFERENCE) {
        if (progress > 0.5)
            blended_filter.set_image_filter(to_op.image_filter());
        else
            blended_filter.set_image_filter(from_op.image_filter());
        return blended_filter;
    }

    blended_filter.set_amount(ClampAmountForFilterType(
        gfx::Tween::FloatValueBetween(progress, from_op.amount(), to_op.amount()),
        to_op.type()));

    if (to_op.type() == FilterOperation::DROP_SHADOW) {
        gfx::Point blended_offset(
            gfx::Tween::LinearIntValueBetween(progress,
                from_op.drop_shadow_offset().x(),
                to_op.drop_shadow_offset().x()),
            gfx::Tween::LinearIntValueBetween(progress,
                from_op.drop_shadow_offset().y(),
                to_op.drop_shadow_offset().y()));
        blended_filter.set_drop_shadow_offset(blended_offset);
        blended_filter.set_drop_shadow_color(gfx::Tween::ColorValueBetween(
            progress, from_op.drop_shadow_color(), to_op.drop_shadow_color()));
    } else if (to_op.type() == FilterOperation::ZOOM) {
        blended_filter.set_zoom_inset(
            std::max(gfx::Tween::LinearIntValueBetween(
                         from_op.zoom_inset(), to_op.zoom_inset(), progress),
                0));
    } else if (to_op.type() == FilterOperation::ALPHA_THRESHOLD) {
        blended_filter.set_outer_threshold(ClampAmountForFilterType(
            gfx::Tween::FloatValueBetween(progress,
                from_op.outer_threshold(),
                to_op.outer_threshold()),
            to_op.type()));
        blended_filter.set_region(to_op.region());
    }

    return blended_filter;
}

void FilterOperation::AsValueInto(base::trace_event::TracedValue* value) const
{
    value->SetInteger("type", type_);
    switch (type_) {
    case FilterOperation::GRAYSCALE:
    case FilterOperation::SEPIA:
    case FilterOperation::SATURATE:
    case FilterOperation::HUE_ROTATE:
    case FilterOperation::INVERT:
    case FilterOperation::BRIGHTNESS:
    case FilterOperation::CONTRAST:
    case FilterOperation::OPACITY:
    case FilterOperation::BLUR:
    case FilterOperation::SATURATING_BRIGHTNESS:
        value->SetDouble("amount", amount_);
        break;
    case FilterOperation::DROP_SHADOW:
        value->SetDouble("std_deviation", amount_);
        //MathUtil::AddToTracedValue("offset", drop_shadow_offset_, value);
        DebugBreak();
        value->SetInteger("color", drop_shadow_color_);
        break;
    case FilterOperation::COLOR_MATRIX: {
        value->BeginArray("matrix");
        for (size_t i = 0; i < arraysize(matrix_); ++i)
            value->AppendDouble(matrix_[i]);
        value->EndArray();
        break;
    }
    case FilterOperation::ZOOM:
        value->SetDouble("amount", amount_);
        value->SetDouble("inset", zoom_inset_);
        break;
    case FilterOperation::REFERENCE: {
        int count_inputs = 0;
        bool can_filter_image_gpu = false;
        if (image_filter_) {
            count_inputs = image_filter_->countInputs();
            can_filter_image_gpu = image_filter_->canFilterImageGPU();
        }
        value->SetBoolean("is_null", !image_filter_);
        value->SetInteger("count_inputs", count_inputs);
        value->SetBoolean("can_filter_image_gpu", can_filter_image_gpu);
        break;
    }
    case FilterOperation::ALPHA_THRESHOLD: {
        value->SetDouble("inner_threshold", amount_);
        value->SetDouble("outer_threshold", outer_threshold_);
        scoped_ptr<base::ListValue> region_value(new base::ListValue());
        value->BeginArray("region");
        for (SkRegion::Iterator it(region_); !it.done(); it.next()) {
            value->AppendInteger(it.rect().x());
            value->AppendInteger(it.rect().y());
            value->AppendInteger(it.rect().width());
            value->AppendInteger(it.rect().height());
        }
        value->EndArray();
    } break;
    }
}

} // namespace cc

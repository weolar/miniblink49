// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/overlay_candidate.h"

#include "base/logging.h"
#include "cc/base/math_util.h"
#include "cc/quads/io_surface_draw_quad.h"
#include "cc/quads/solid_color_draw_quad.h"
#include "cc/quads/stream_video_draw_quad.h"
#include "cc/quads/texture_draw_quad.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/vector3d_f.h"
#include <algorithm>
#include <limits>

namespace cc {

namespace {
    // Tolerance for considering axis vector elements to be zero.
    const SkMScalar kEpsilon = std::numeric_limits<float>::epsilon();

    enum Axis { NONE,
        AXIS_POS_X,
        AXIS_NEG_X,
        AXIS_POS_Y,
        AXIS_NEG_Y };

    Axis VectorToAxis(const gfx::Vector3dF& vec)
    {
        if (std::abs(vec.z()) > kEpsilon)
            return NONE;
        const bool x_zero = (std::abs(vec.x()) <= kEpsilon);
        const bool y_zero = (std::abs(vec.y()) <= kEpsilon);
        if (x_zero && !y_zero)
            return (vec.y() > 0) ? AXIS_POS_Y : AXIS_NEG_Y;
        else if (y_zero && !x_zero)
            return (vec.x() > 0) ? AXIS_POS_X : AXIS_NEG_X;
        else
            return NONE;
    }

    gfx::OverlayTransform GetOverlayTransform(const gfx::Transform& quad_transform,
        bool y_flipped)
    {
        if (!quad_transform.Preserves2dAxisAlignment()) {
            return gfx::OVERLAY_TRANSFORM_INVALID;
        }

        gfx::Vector3dF x_axis = MathUtil::GetXAxis(quad_transform);
        gfx::Vector3dF y_axis = MathUtil::GetYAxis(quad_transform);
        if (y_flipped) {
            y_axis.Scale(-1);
        }

        Axis x_to = VectorToAxis(x_axis);
        Axis y_to = VectorToAxis(y_axis);

        if (x_to == AXIS_POS_X && y_to == AXIS_POS_Y)
            return gfx::OVERLAY_TRANSFORM_NONE;
        else if (x_to == AXIS_NEG_X && y_to == AXIS_POS_Y)
            return gfx::OVERLAY_TRANSFORM_FLIP_HORIZONTAL;
        else if (x_to == AXIS_POS_X && y_to == AXIS_NEG_Y)
            return gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL;
        else if (x_to == AXIS_NEG_Y && y_to == AXIS_POS_X)
            return gfx::OVERLAY_TRANSFORM_ROTATE_270;
        else if (x_to == AXIS_NEG_X && y_to == AXIS_NEG_Y)
            return gfx::OVERLAY_TRANSFORM_ROTATE_180;
        else if (x_to == AXIS_POS_Y && y_to == AXIS_NEG_X)
            return gfx::OVERLAY_TRANSFORM_ROTATE_90;
        else
            return gfx::OVERLAY_TRANSFORM_INVALID;
    }

    // Apply transform |delta| to |in| and return the resulting transform,
    // or OVERLAY_TRANSFORM_INVALID.
    gfx::OverlayTransform ComposeTransforms(gfx::OverlayTransform delta,
        gfx::OverlayTransform in)
    {
        // There are 8 different possible transforms. We can characterize these
        // by looking at where the origin moves and the direction the horizontal goes.
        // (TL=top-left, BR=bottom-right, H=horizontal, V=vertical).
        // NONE: TL, H
        // FLIP_VERTICAL: BL, H
        // FLIP_HORIZONTAL: TR, H
        // ROTATE_90: TR, V
        // ROTATE_180: BR, H
        // ROTATE_270: BL, V
        // Missing transforms: TL, V & BR, V
        // Basic combinations:
        // Flip X & Y -> Rotate 180 (TL,H -> TR,H -> BR,H or TL,H -> BL,H -> BR,H)
        // Flip X or Y + Rotate 180 -> other flip (eg, TL,H -> TR,H -> BL,H)
        // Rotate + Rotate simply adds values.
        // Rotate 90/270 + flip is invalid because we can only have verticals with
        // the origin in TR or BL.
        if (delta == gfx::OVERLAY_TRANSFORM_NONE)
            return in;
        switch (in) {
        case gfx::OVERLAY_TRANSFORM_NONE:
            return delta;
        case gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL:
            switch (delta) {
            case gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL:
                return gfx::OVERLAY_TRANSFORM_NONE;
            case gfx::OVERLAY_TRANSFORM_FLIP_HORIZONTAL:
                return gfx::OVERLAY_TRANSFORM_ROTATE_180;
            case gfx::OVERLAY_TRANSFORM_ROTATE_180:
                return gfx::OVERLAY_TRANSFORM_FLIP_HORIZONTAL;
            default:
                return gfx::OVERLAY_TRANSFORM_INVALID;
            }
            break;
        case gfx::OVERLAY_TRANSFORM_FLIP_HORIZONTAL:
            switch (delta) {
            case gfx::OVERLAY_TRANSFORM_FLIP_HORIZONTAL:
                return gfx::OVERLAY_TRANSFORM_NONE;
            case gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL:
                return gfx::OVERLAY_TRANSFORM_ROTATE_180;
            case gfx::OVERLAY_TRANSFORM_ROTATE_90:
            case gfx::OVERLAY_TRANSFORM_ROTATE_180:
                return gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL;
            case gfx::OVERLAY_TRANSFORM_ROTATE_270:
            default:
                return gfx::OVERLAY_TRANSFORM_INVALID;
            }
            break;
        case gfx::OVERLAY_TRANSFORM_ROTATE_90:
            switch (delta) {
            case gfx::OVERLAY_TRANSFORM_ROTATE_90:
                return gfx::OVERLAY_TRANSFORM_ROTATE_180;
            case gfx::OVERLAY_TRANSFORM_ROTATE_180:
                return gfx::OVERLAY_TRANSFORM_ROTATE_270;
            case gfx::OVERLAY_TRANSFORM_ROTATE_270:
                return gfx::OVERLAY_TRANSFORM_NONE;
            default:
                return gfx::OVERLAY_TRANSFORM_INVALID;
            }
            break;
        case gfx::OVERLAY_TRANSFORM_ROTATE_180:
            switch (delta) {
            case gfx::OVERLAY_TRANSFORM_FLIP_HORIZONTAL:
                return gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL;
            case gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL:
                return gfx::OVERLAY_TRANSFORM_FLIP_HORIZONTAL;
            case gfx::OVERLAY_TRANSFORM_ROTATE_90:
                return gfx::OVERLAY_TRANSFORM_ROTATE_270;
            case gfx::OVERLAY_TRANSFORM_ROTATE_180:
                return gfx::OVERLAY_TRANSFORM_NONE;
            case gfx::OVERLAY_TRANSFORM_ROTATE_270:
                return gfx::OVERLAY_TRANSFORM_ROTATE_90;
            default:
                return gfx::OVERLAY_TRANSFORM_INVALID;
            }
            break;
        case gfx::OVERLAY_TRANSFORM_ROTATE_270:
            switch (delta) {
            case gfx::OVERLAY_TRANSFORM_ROTATE_90:
                return gfx::OVERLAY_TRANSFORM_NONE;
            case gfx::OVERLAY_TRANSFORM_ROTATE_180:
                return gfx::OVERLAY_TRANSFORM_ROTATE_90;
            case gfx::OVERLAY_TRANSFORM_ROTATE_270:
                return gfx::OVERLAY_TRANSFORM_ROTATE_180;
            default:
                return gfx::OVERLAY_TRANSFORM_INVALID;
            }
            break;
        default:
            return gfx::OVERLAY_TRANSFORM_INVALID;
        }
    }

} // namespace

OverlayCandidate::OverlayCandidate()
    : transform(gfx::OVERLAY_TRANSFORM_NONE)
    , format(RGBA_8888)
    , uv_rect(0.f, 0.f, 1.f, 1.f)
    , is_clipped(false)
    , use_output_surface_for_resource(false)
    , resource_id(0)
    , plane_z_order(0)
    , overlay_handled(false)
{
}

OverlayCandidate::~OverlayCandidate() { }

// static
bool OverlayCandidate::FromDrawQuad(const DrawQuad* quad,
    OverlayCandidate* candidate)
{
    if (quad->needs_blending || quad->shared_quad_state->opacity != 1.f || quad->shared_quad_state->blend_mode != SkXfermode::kSrcOver_Mode)
        return false;

    auto& transform = quad->shared_quad_state->quad_to_target_transform;
    candidate->display_rect = gfx::RectF(quad->rect);
    transform.TransformRect(&candidate->display_rect);
    candidate->quad_rect_in_target_space = MathUtil::MapEnclosingClippedRect(transform, quad->rect);

    candidate->format = RGBA_8888;
    candidate->clip_rect = quad->shared_quad_state->clip_rect;
    candidate->is_clipped = quad->shared_quad_state->is_clipped;

    switch (quad->material) {
    case DrawQuad::TEXTURE_CONTENT:
        return FromTextureQuad(TextureDrawQuad::MaterialCast(quad), candidate);
    case DrawQuad::STREAM_VIDEO_CONTENT:
        return FromStreamVideoQuad(StreamVideoDrawQuad::MaterialCast(quad),
            candidate);
    case DrawQuad::IO_SURFACE_CONTENT:
        return FromIOSurfaceQuad(IOSurfaceDrawQuad::MaterialCast(quad),
            candidate);
    default:
        break;
    }

    return false;
}

// static
bool OverlayCandidate::FromTextureQuad(const TextureDrawQuad* quad,
    OverlayCandidate* candidate)
{
    if (!quad->allow_overlay())
        return false;
    gfx::OverlayTransform overlay_transform = GetOverlayTransform(
        quad->shared_quad_state->quad_to_target_transform, quad->y_flipped);
    if (quad->background_color != SK_ColorTRANSPARENT || quad->premultiplied_alpha || overlay_transform == gfx::OVERLAY_TRANSFORM_INVALID)
        return false;
    candidate->resource_id = quad->resource_id();
    candidate->resource_size_in_pixels = quad->resource_size_in_pixels();
    candidate->transform = overlay_transform;
    candidate->uv_rect = BoundingRect(quad->uv_top_left, quad->uv_bottom_right);
    return true;
}

// static
bool OverlayCandidate::FromStreamVideoQuad(const StreamVideoDrawQuad* quad,
    OverlayCandidate* candidate)
{
    if (!quad->allow_overlay())
        return false;
    gfx::OverlayTransform overlay_transform = GetOverlayTransform(
        quad->shared_quad_state->quad_to_target_transform, false);
    if (overlay_transform == gfx::OVERLAY_TRANSFORM_INVALID)
        return false;
    if (!quad->matrix.IsScaleOrTranslation()) {
        // We cannot handle anything other than scaling & translation for texture
        // coordinates yet.
        return false;
    }
    candidate->resource_id = quad->resource_id();
    candidate->resource_size_in_pixels = quad->resource_size_in_pixels();
    candidate->transform = overlay_transform;

    gfx::Point3F uv0 = gfx::Point3F(0, 0, 0);
    gfx::Point3F uv1 = gfx::Point3F(1, 1, 0);
    quad->matrix.TransformPoint(&uv0);
    quad->matrix.TransformPoint(&uv1);
    gfx::Vector3dF delta = uv1 - uv0;
    if (delta.x() < 0) {
        candidate->transform = ComposeTransforms(
            gfx::OVERLAY_TRANSFORM_FLIP_HORIZONTAL, candidate->transform);
        float x0 = uv0.x();
        uv0.set_x(uv1.x());
        uv1.set_x(x0);
        delta.set_x(-delta.x());
    }

    if (delta.y() < 0) {
        // In this situation, uv0y < uv1y. Since we overlay inverted, a request
        // to invert the source texture means we can just output the texture
        // normally and it will be correct.
        candidate->uv_rect = gfx::RectF(uv0.x(), uv1.y(), delta.x(), -delta.y());
    } else {
        candidate->transform = ComposeTransforms(
            gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL, candidate->transform);
        candidate->uv_rect = gfx::RectF(uv0.x(), uv0.y(), delta.x(), delta.y());
    }
    return true;
}

// static
bool OverlayCandidate::FromIOSurfaceQuad(const IOSurfaceDrawQuad* quad,
    OverlayCandidate* candidate)
{
    if (!quad->allow_overlay)
        return false;
    gfx::OverlayTransform overlay_transform = GetOverlayTransform(
        quad->shared_quad_state->quad_to_target_transform, false);
    if (overlay_transform != gfx::OVERLAY_TRANSFORM_NONE)
        return false;
    candidate->resource_id = quad->io_surface_resource_id();
    candidate->resource_size_in_pixels = quad->io_surface_size;
    candidate->transform = overlay_transform;
    candidate->uv_rect = gfx::RectF(1.f, 1.f);
    return true;
}

} // namespace cc

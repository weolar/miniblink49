// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Needed on Windows to get |M_PI| from <cmath>
#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include <algorithm>
#include <cmath>
#include <limits>

#include "mc/animation/TransformOperation.h"
#include "mc/animation/TransformOperations.h"
#include "mc/base/FloatBox.h"
#include "mc/base/TransformUtil.h"
// #include "ui/gfx/geometry/box_f.h"
// #include "ui/gfx/geometry/vector3d_f.h"
// #include "ui/gfx/transform_util.h"

namespace {
const SkMScalar kAngleEpsilon = 1e-4f;
}

namespace mc {

bool TransformOperation::isIdentity() const 
{
    return matrix.isIdentity();
}

inline bool ApproximatelyZero(SkMScalar x, SkMScalar tolerance) 
{
    return std::abs(x) <= tolerance;
}

inline bool ApproximatelyOne(SkMScalar x, SkMScalar tolerance) 
{
    return std::abs(x - SkDoubleToMScalar(1.0)) <= tolerance;
}

static bool isOperationIdentity(const TransformOperation* operation)
{
    return !operation || operation->isIdentity();
}

static bool shareSameAxis(const TransformOperation* from,
    const TransformOperation* to,
    SkMScalar* axis_x,
    SkMScalar* axis_y,
    SkMScalar* axis_z,
    SkMScalar* angle_from) 
{
    if (isOperationIdentity(from) && isOperationIdentity(to))
        return false;

    if (isOperationIdentity(from) && !isOperationIdentity(to)) {
        *axis_x = to->rotate.axis.x;
        *axis_y = to->rotate.axis.y;
        *axis_z = to->rotate.axis.z;
        *angle_from = 0;
        return true;
    }

    if (!isOperationIdentity(from) && isOperationIdentity(to)) {
        *axis_x = from->rotate.axis.x;
        *axis_y = from->rotate.axis.y;
        *axis_z = from->rotate.axis.z;
        *angle_from = from->rotate.angle;
        return true;
    }

    SkMScalar length_2 = from->rotate.axis.x * from->rotate.axis.x +
        from->rotate.axis.y * from->rotate.axis.y +
        from->rotate.axis.z * from->rotate.axis.z;
    SkMScalar other_length_2 = to->rotate.axis.x * to->rotate.axis.x +
        to->rotate.axis.y * to->rotate.axis.y +
        to->rotate.axis.z * to->rotate.axis.z;

    if (length_2 <= kAngleEpsilon || other_length_2 <= kAngleEpsilon)
        return false;

    SkMScalar dot = to->rotate.axis.x * from->rotate.axis.x +
        to->rotate.axis.y * from->rotate.axis.y +
        to->rotate.axis.z * from->rotate.axis.z;
    SkMScalar error =
        std::abs(SK_MScalar1 - (dot * dot) / (length_2 * other_length_2));
    bool result = error < kAngleEpsilon;
    if (result) {
        *axis_x = to->rotate.axis.x;
        *axis_y = to->rotate.axis.y;
        *axis_z = to->rotate.axis.z;
        // If the axes are pointing in opposite directions, we need to reverse
        // the angle.
        *angle_from = dot > 0 ? from->rotate.angle : -from->rotate.angle;
    }
    return result;
}

static SkMScalar blendSkMScalars(SkMScalar from, SkMScalar to, SkMScalar progress)
{
    return from * (1 - progress) + to * progress;
}

bool TransformOperation::blendTransformOperations(const TransformOperation* from, const TransformOperation* to, SkMScalar progress, SkMatrix44* result)
{
    if (isOperationIdentity(from) && isOperationIdentity(to))
        return true;

    TransformOperation::Type interpolation_type = TransformOperation::TRANSFORM_OPERATION_IDENTITY;
    if (isOperationIdentity(to))
        interpolation_type = from->type;
    else
        interpolation_type = to->type;

    switch (interpolation_type) {
    case TransformOperation::TRANSFORM_OPERATION_TRANSLATE:
    {
        SkMScalar from_x = isOperationIdentity(from) ? 0 : from->translate.x;
        SkMScalar from_y = isOperationIdentity(from) ? 0 : from->translate.y;
        SkMScalar from_z = isOperationIdentity(from) ? 0 : from->translate.z;
        SkMScalar to_x = isOperationIdentity(to) ? 0 : to->translate.x;
        SkMScalar to_y = isOperationIdentity(to) ? 0 : to->translate.y;
        SkMScalar to_z = isOperationIdentity(to) ? 0 : to->translate.z;
        result->preTranslate(
            blendSkMScalars(from_x, to_x, progress),
            blendSkMScalars(from_y, to_y, progress),
            blendSkMScalars(from_z, to_z, progress));
        break;
    }
    case TransformOperation::TRANSFORM_OPERATION_ROTATE:
    {
        SkMScalar axis_x = 0;
        SkMScalar axis_y = 0;
        SkMScalar axis_z = 1;
        SkMScalar from_angle = 0;
        SkMScalar to_angle = isOperationIdentity(to) ? 0 : to->rotate.angle;
        if (shareSameAxis(from, to, &axis_x, &axis_y, &axis_z, &from_angle)) {
            rotateAbout(result, blink::FloatPoint3D(axis_x, axis_y, axis_z), blendSkMScalars(from_angle, to_angle, progress));
        } else {
            SkMatrix44 to_matrix(SkMatrix44::kIdentity_Constructor);
            if (!isOperationIdentity(to))
                to_matrix = to->matrix;
            SkMatrix44 from_matrix(SkMatrix44::kIdentity_Constructor);
            if (!isOperationIdentity(from))
                from_matrix = from->matrix;
            *result = to_matrix;
            if (!transformBlend(from_matrix, progress, result))
                return false;
        }
        break;
    }
    case TransformOperation::TRANSFORM_OPERATION_SCALE:
    {
        SkMScalar from_x = isOperationIdentity(from) ? 1 : from->scale.x;
        SkMScalar from_y = isOperationIdentity(from) ? 1 : from->scale.y;
        SkMScalar from_z = isOperationIdentity(from) ? 1 : from->scale.z;
        SkMScalar to_x = isOperationIdentity(to) ? 1 : to->scale.x;
        SkMScalar to_y = isOperationIdentity(to) ? 1 : to->scale.y;
        SkMScalar to_z = isOperationIdentity(to) ? 1 : to->scale.z;
        result->preScale(blendSkMScalars(from_x, to_x, progress),
            blendSkMScalars(from_y, to_y, progress),
            blendSkMScalars(from_z, to_z, progress));
        break;
    }
    case TransformOperation::TRANSFORM_OPERATION_SKEW:
    {
        SkMScalar from_x = isOperationIdentity(from) ? 0 : from->skew.x;
        SkMScalar from_y = isOperationIdentity(from) ? 0 : from->skew.y;
        SkMScalar to_x = isOperationIdentity(to) ? 0 : to->skew.x;
        SkMScalar to_y = isOperationIdentity(to) ? 0 : to->skew.y;
        skewOp(result, blendSkMScalars(from_x, to_x, progress), blendSkMScalars(from_y, to_y, progress));
        break;
    }
    case TransformOperation::TRANSFORM_OPERATION_PERSPECTIVE:
    {
        SkMScalar from_perspective_depth =
            isOperationIdentity(from) ? std::numeric_limits<SkMScalar>::max()
            : from->perspective_depth;
        SkMScalar to_perspective_depth =
            isOperationIdentity(to) ? std::numeric_limits<SkMScalar>::max()
            : to->perspective_depth;
        if (from_perspective_depth == 0.f || to_perspective_depth == 0.f)
            return false;

        SkMScalar blended_perspective_depth = blendSkMScalars(
            1.f / from_perspective_depth, 1.f / to_perspective_depth, progress);

        if (blended_perspective_depth == 0.f)
            return false;

        applyPerspectiveDepth(result, 1.f / blended_perspective_depth);
        break;
    }
    case TransformOperation::TRANSFORM_OPERATION_MATRIX:
    {
        SkMatrix44 to_matrix(SkMatrix44::kIdentity_Constructor);
        if (!isOperationIdentity(to))
            to_matrix = to->matrix;
        SkMatrix44 from_matrix(SkMatrix44::kIdentity_Constructor);
        if (!isOperationIdentity(from))
            from_matrix = from->matrix;
        *result = to_matrix;
        if (!transformBlend(from_matrix, progress, result))
            return false;
        break;
    }
    case TransformOperation::TRANSFORM_OPERATION_IDENTITY:
        // Do nothing.
        break;
    }

    return true;
}

// If p = (px, py) is a point in the plane being rotated about (0, 0, nz), this
// function computes the angles we would have to rotate from p to get to
// (length(p), 0), (-length(p), 0), (0, length(p)), (0, -length(p)). If nz is
// negative, these angles will need to be reversed.
static void findCandidatesInPlane(
    float px,
    float py,
    float nz,
    double* candidates,
    int* num_candidates
    ) 
{
    double phi = atan2(px, py);
    *num_candidates = 4;
    candidates[0] = phi;
    for (int i = 1; i < *num_candidates; ++i)
        candidates[i] = candidates[i - 1] + M_PI_2;
    if (nz < 0.f) {
        for (int i = 0; i < *num_candidates; ++i)
            candidates[i] *= -1.f;
    }
}

static float radiansToDegrees(float radians) 
{
    return (180.f * radians) / M_PI;
}

static float degreesToRadians(float degrees) 
{
    return (M_PI * degrees) / 180.f;
}

double lengthSquared(const blink::FloatPoint3D& point)
{
    return static_cast<double>(point.x()) * point.x() + 
        static_cast<double>(point.y()) * point.y() +
        static_cast<double>(point.z()) * point.z();
}

float getLength(const blink::FloatPoint3D& point)
{
    return static_cast<float>(std::sqrt(lengthSquared(point)));
}

static void boundingBoxForArc(const blink::FloatPoint3D& point,
    const TransformOperation* from,
    const TransformOperation* to,
    SkMScalar min_progress,
    SkMScalar max_progress,
    FloatBox* box)
{
    const TransformOperation* exemplar = from ? from : to;
    blink::FloatPoint3D axis(exemplar->rotate.axis.x, exemplar->rotate.axis.y, exemplar->rotate.axis.z);

    const bool x_is_zero = axis.x() == 0.f;
    const bool y_is_zero = axis.y() == 0.f;
    const bool z_is_zero = axis.z() == 0.f;

    // We will have at most 6 angles to test (excluding from->angle and
    // to->angle).
    static const int kMaxNumCandidates = 6;
    double candidates[kMaxNumCandidates];
    int num_candidates = kMaxNumCandidates;

    if (x_is_zero && y_is_zero && z_is_zero)
        return;

    SkMScalar from_angle = from ? from->rotate.angle : 0.f;
    SkMScalar to_angle = to ? to->rotate.angle : 0.f;

    // If the axes of rotation are pointing in opposite directions, we need to
    // flip one of the angles. Note, if both |from| and |to| exist, then axis will
    // correspond to |from|.
    if (from && to) {
        blink::FloatPoint3D other_axis(to->rotate.axis.x, to->rotate.axis.y, to->rotate.axis.z);
        if (dotProduct(axis, other_axis) < 0.f)
            to_angle *= -1.f;
    }

    float min_degrees = SkMScalarToFloat(blendSkMScalars(from_angle, to_angle, min_progress));
    float max_degrees = SkMScalarToFloat(blendSkMScalars(from_angle, to_angle, max_progress));
    if (max_degrees < min_degrees)
        std::swap(min_degrees, max_degrees);

    SkMatrix44 from_transform(SkMatrix44::kIdentity_Constructor);
    rotateAbout(&from_transform, axis, min_degrees);
    SkMatrix44 to_transform(SkMatrix44::kIdentity_Constructor);
    rotateAbout(&to_transform, axis, max_degrees);

    *box = FloatBox();

    blink::FloatPoint3D point_rotated_from = point;
    transformPointInternal(from_transform, &point_rotated_from);
    blink::FloatPoint3D point_rotated_to = point;
    transformPointInternal(to_transform, &point_rotated_to);

    box->set_origin(point_rotated_from);
    box->ExpandTo(point_rotated_to);

    if (x_is_zero && y_is_zero) {
        findCandidatesInPlane(point.x(), point.y(), axis.z(), candidates, &num_candidates);
    } else if (x_is_zero && z_is_zero) {
        findCandidatesInPlane(point.z(), point.x(), axis.y(), candidates, &num_candidates);
    } else if (y_is_zero && z_is_zero) {
        findCandidatesInPlane(point.y(), point.z(), axis.x(), candidates, &num_candidates);
    } else {
        blink::FloatPoint3D normal = axis;
        float scaleVal = 1.f / getLength(normal);
        normal.scale(scaleVal, scaleVal, scaleVal);

        // First, find center of rotation.
        blink::FloatPoint3D origin;
        blink::FloatPoint3D to_point = point - origin;
        scaleVal = dotProduct(to_point, normal);
        blink::FloatPoint3D center = origin + scaleVector3d(normal, scaleVal, scaleVal, scaleVal);

        // Now we need to find two vectors in the plane of rotation. One pointing
        // towards point and another, perpendicular vector in the plane.
        blink::FloatPoint3D v1 = point - center;
        float v1_length = getLength(v1);
        if (v1_length == 0.f)
            return;

        scaleVal = 1.f / v1_length;
        v1.scale(scaleVal, scaleVal, scaleVal);
        blink::FloatPoint3D v2 = crossProduct(normal, v1);
        // v1 is the basis vector in the direction of the point.
        // i.e. with a rotation of 0, v1 is our +x vector.
        // v2 is a perpenticular basis vector of our plane (+y).

        // Take the parametric equation of a circle.
        // x = r*cos(t); y = r*sin(t);
        // We can treat that as a circle on the plane v1xv2.
        // From that we get the parametric equations for a circle on the
        // plane in 3d space of:
        // x(t) = r*cos(t)*v1.x + r*sin(t)*v2.x + cx
        // y(t) = r*cos(t)*v1.y + r*sin(t)*v2.y + cy
        // z(t) = r*cos(t)*v1.z + r*sin(t)*v2.z + cz
        // Taking the derivative of (x, y, z) and solving for 0 gives us our
        // maximum/minimum x, y, z values.
        // x'(t) = r*cos(t)*v2.x - r*sin(t)*v1.x = 0
        // tan(t) = v2.x/v1.x
        // t = atan2(v2.x, v1.x) + n*M_PI;
        candidates[0] = atan2(v2.x(), v1.x());
        candidates[1] = candidates[0] + M_PI;
        candidates[2] = atan2(v2.y(), v1.y());
        candidates[3] = candidates[2] + M_PI;
        candidates[4] = atan2(v2.z(), v1.z());
        candidates[5] = candidates[4] + M_PI;
    }

    double min_radians = degreesToRadians(min_degrees);
    double max_radians = degreesToRadians(max_degrees);

    for (int i = 0; i < num_candidates; ++i) {
        double radians = candidates[i];
        while (radians < min_radians)
            radians += 2.0 * M_PI;
        while (radians > max_radians)
            radians -= 2.0 * M_PI;
        if (radians < min_radians)
            continue;

        SkMatrix44 rotation;
        rotateAbout(&rotation, axis, radiansToDegrees(radians));
        blink::FloatPoint3D rotated = point;
        transformPointInternal(rotation, &rotated);

        box->ExpandTo(rotated);
    }
}

bool TransformOperation::blendedBoundsForBox(
    const FloatBox& box,
    const TransformOperation* from,
    const TransformOperation* to,
    SkMScalar min_progress,
    SkMScalar max_progress,
    FloatBox* bounds
    ) 
{
    bool is_identity_from = isOperationIdentity(from);
    bool is_identity_to = isOperationIdentity(to);
    if (is_identity_from && is_identity_to) {
        *bounds = box;
        return true;
    }

    TransformOperation::Type interpolation_type =
        TransformOperation::TRANSFORM_OPERATION_IDENTITY;
    if (is_identity_to)
        interpolation_type = from->type;
    else
        interpolation_type = to->type;

    switch (interpolation_type) {
    case TransformOperation::TRANSFORM_OPERATION_IDENTITY:
        *bounds = box;
        return true;
    case TransformOperation::TRANSFORM_OPERATION_TRANSLATE:
    case TransformOperation::TRANSFORM_OPERATION_SKEW:
    case TransformOperation::TRANSFORM_OPERATION_PERSPECTIVE:
    case TransformOperation::TRANSFORM_OPERATION_SCALE:
    {
        SkMatrix44 from_transform(SkMatrix44::kIdentity_Constructor);
        SkMatrix44 to_transform(SkMatrix44::kIdentity_Constructor);
        if (!blendTransformOperations(from, to, min_progress, &from_transform) ||
            !blendTransformOperations(from, to, max_progress, &to_transform))
            return false;

        *bounds = box;
        transformBox(from_transform, bounds);

        FloatBox to_box = box;
        transformBox(to_transform, &to_box);
        bounds->ExpandTo(to_box);

        return true;
    }
    case TransformOperation::TRANSFORM_OPERATION_ROTATE:
    {
        SkMScalar axis_x = 0;
        SkMScalar axis_y = 0;
        SkMScalar axis_z = 1;
        SkMScalar from_angle = 0;
        if (!shareSameAxis(from, to, &axis_x, &axis_y, &axis_z, &from_angle))
            return false;

        bool first_point = true;
        for (int i = 0; i < 8; ++i) {
            blink::FloatPoint3D corner = box.origin();
            corner += blink::FloatPoint3D(i & 1 ? box.width() : 0.f,
                i & 2 ? box.height() : 0.f,
                i & 4 ? box.depth() : 0.f);
            FloatBox box_for_arc;
            boundingBoxForArc(corner, from, to, min_progress, max_progress, &box_for_arc);
            if (first_point)
                *bounds = box_for_arc;
            else
                bounds->Union(box_for_arc);
            first_point = false;
        }
        return true;
    }
    case TransformOperation::TRANSFORM_OPERATION_MATRIX:
        return false;
    }
    DebugBreak();
    return false;
}

}  // namespace cc

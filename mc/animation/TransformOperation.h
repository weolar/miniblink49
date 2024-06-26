// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_TransformOperation_h
#define mc_animation_TransformOperation_h

#include "mc/base/FloatBox.h"
#include "third_party/skia/include/utils/SkMatrix44.h"

namespace gfx {
class BoxF;
class Transform;
}

namespace mc {

struct TransformOperation {
    enum Type {
        TRANSFORM_OPERATION_TRANSLATE,
        TRANSFORM_OPERATION_ROTATE,
        TRANSFORM_OPERATION_SCALE,
        TRANSFORM_OPERATION_SKEW,
        TRANSFORM_OPERATION_PERSPECTIVE,
        TRANSFORM_OPERATION_MATRIX,
        TRANSFORM_OPERATION_IDENTITY
    };

    TransformOperation() : type(TRANSFORM_OPERATION_IDENTITY) {}

    Type type;
    SkMatrix44 matrix;

    union {
        SkMScalar perspective_depth;

        struct {
            SkMScalar x, y;
        } skew;

        struct {
            SkMScalar x, y, z;
        } scale;

        struct {
            SkMScalar x, y, z;
        } translate;

        struct {
            struct {
                SkMScalar x, y, z;
            } axis;

            SkMScalar angle;
        } rotate;
    };

    bool isIdentity() const;

    static bool blendTransformOperations(
        const TransformOperation* from,
        const TransformOperation* to,
        SkMScalar progress,
        SkMatrix44* result);

    static bool blendedBoundsForBox(
        const FloatBox& box,
        const TransformOperation* from,
        const TransformOperation* to,
        SkMScalar min_progress,
        SkMScalar max_progress,
        FloatBox* bounds);
};

}  // namespace mc

#endif  // CC_ANIMATION_TRANSFORM_OPERATION_H_

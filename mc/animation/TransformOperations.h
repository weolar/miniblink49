// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_TransformOperations_h
#define mc_animation_TransformOperations_h

#include <vector>

#include "mc/animation/TransformOperation.h"
#include "third_party/skia/include/utils/SkMatrix44.h"
#include "third_party/WebKit/Source/platform/geometry/FloatPoint3D.h"
#include "mc/base/TransformUtil.h"

namespace mc {

// Transform operations are a decomposed transformation matrix. It can be
// applied to obtain a gfx::Transform at any time, and can be blended
// intelligently with other transform operations, so long as they represent the
// same decomposition. For example, if we have a transform that is made up of
// a rotation followed by skew, it can be blended intelligently with another
// transform made up of a rotation followed by a skew. Blending is possible if
// we have two dissimilar sets of transform operations, but the effect may not
// be what was intended. For more information, see the comments for the blend
// function below.
class TransformOperations {
public:
    TransformOperations();
    TransformOperations(const TransformOperations& other);
    ~TransformOperations();

    // Returns a transformation matrix representing these transform operations.
    SkMatrix44 apply() const;

    // Given another set of transform operations and a progress in the range
    // [0, 1], returns a transformation matrix representing the intermediate
    // value. If this->MatchesTypes(from), then each of the operations are
    // blended separately and then combined. Otherwise, the two sets of
    // transforms are baked to matrices (using apply), and the matrices are
    // then decomposed and interpolated. For more information, see
    // http://www.w3.org/TR/2011/WD-css3-2d-transforms-20111215/#matrix-decomposition.
    SkMatrix44 blend(const TransformOperations& from, SkMScalar progress) const;

    // Sets |bounds| be the bounding box for the region within which |box| will
    // exist when it is transformed by the result of calling Blend on |from| and
    // with progress in the range [min_progress, max_progress]. If this region
    // cannot be computed, returns false.
    bool blendedBoundsForBox(const FloatBox& box,
        const TransformOperations& from,
        SkMScalar min_progress,
        SkMScalar max_progress,
        FloatBox* bounds) const;

    // Returns true if these operations affect scale.
    bool affectsScale() const;

    // Returns true if these operations are only translations.
    bool isTranslation() const;

    // Returns false if the operations affect 2d axis alignment.
    bool preservesAxisAlignment() const;

    // Returns true if this operation and its descendants have the same types
    // as other and its descendants.
    bool matchesTypes(const TransformOperations& other) const;

    // Returns true if these operations can be blended. It will only return
    // false if we must resort to matrix interpolation, and matrix interpolation
    // fails (this can happen if either matrix cannot be decomposed).
    bool canBlendWith(const TransformOperations& other) const;

    // If these operations have no more than one scale operation, and if the only
    // other operations are translations, sets |scale| to the scale component
    // of these operations. Otherwise, returns false.
    bool scaleComponent(blink::FloatPoint3D* scale) const;

    void appendTranslate(SkMScalar x, SkMScalar y, SkMScalar z);
    void appendRotate(SkMScalar x, SkMScalar y, SkMScalar z, SkMScalar degrees);
    void appendScale(SkMScalar x, SkMScalar y, SkMScalar z);
    void appendSkew(SkMScalar x, SkMScalar y);
    void appendPerspective(SkMScalar depth);
    void appendMatrix(const SkMatrix44& matrix);
    void appendIdentity();
    bool isIdentity() const;

private:
    bool blendInternal(const TransformOperations& from, SkMScalar progress, SkMatrix44* result) const;

    std::vector<TransformOperation> m_operations;

    bool computeDecomposedTransform() const;

    // For efficiency, we cache the decomposed transform.
    mutable WTF::OwnPtr<DecomposedTransform> m_decomposedTransform;
    mutable bool m_decomposedTransformDirty;
};

}  // namespace mc

#endif  // mc_animation_TransformOperations_h
